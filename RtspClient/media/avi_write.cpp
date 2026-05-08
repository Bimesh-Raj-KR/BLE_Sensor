/***************************************************************************************
 *
 *  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 *
 *  By downloading, copying, installing or using the software you agree to this license.
 *  If you do not agree to this license, do not download, install, 
 *  copy or use the software.
 *
 *  Copyright (C) 2014-2025, Happytimesoft Corporation, all rights reserved.
 *
 *  Redistribution and use in binary forms, with or without modification, are permitted.
 *
 *  Unless required by applicable law or agreed to in writing, software distributed 
 *  under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 *  CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
 *  language governing permissions and limitations under the License.
 *
****************************************************************************************/

#include "sys_inc.h"
#include "avi.h"
#include "avi_write.h"
#include "h264.h"
#include "h265.h"
#include "mjpeg.h"
#include "media_util.h"
#include "media_format.h"
#include "h264_util.h"
#include "h265_util.h"
#include <math.h>


int avi_write_uint16_(AVICTX * p_ctx, uint16 w)
{
    return fwrite(&w, 2, 1, p_ctx->fp);
}

int avi_write_uint32_(AVICTX * p_ctx, uint32 dw)
{
    return fwrite(&dw, 4, 1, p_ctx->fp);
}

int avi_write_fourcc_(AVICTX * p_ctx, const char fcc[4])
{
    return fwrite(fcc, 4, 1, p_ctx->fp);
}

int avi_write_buffer_(AVICTX * p_ctx, char * p_data, int len)
{
    return fwrite(p_data, len, 1, p_ctx->fp);
}

#define avi_write_uint16(p_ctx, w) do{ if(avi_write_uint16_(p_ctx, w) != 1) goto w_err; }while(0)
#define avi_write_uint32(p_ctx, w) do{ if(avi_write_uint32_(p_ctx, w) != 1) goto w_err; }while(0)
#define avi_write_fourcc(p_ctx, fcc) do{ if(avi_write_fourcc_(p_ctx, fcc) != 1) goto w_err; }while(0)
#define avi_write_buffer(p_ctx, p_data, len) do{ if(avi_write_buffer_(p_ctx, p_data, len) != 1) goto w_err; }while(0)

void avi_free_idx(AVICTX * p_ctx)
{
    if (p_ctx->idx)
    {
        free(p_ctx->idx);
        p_ctx->idx = NULL;
    }
    
    if (p_ctx->idx_fp)
    {
        fclose(p_ctx->idx_fp);
        p_ctx->idx_fp = NULL;
    }

    p_ctx->i_idx = 0;
    p_ctx->i_idx_max = 0;
}

int avi_write_idx(AVICTX * p_ctx)
{
    avi_write_fourcc(p_ctx, "idx1");
    avi_write_uint32(p_ctx,  p_ctx->i_idx * 16);

    if (p_ctx->ctxf_idx_m == 1)
    {
        if (p_ctx->i_idx > 0)
        {
            if (fwrite(p_ctx->idx, p_ctx->i_idx * 16, 1, p_ctx->fp) != 1)
            {
                return -1;
            }
        }
    }
    else if (p_ctx->idx_fp)
    {
        // Write index data in a fixed buffer to an index file
        if (p_ctx->idx_fix_off > 0)
        {
            if (fwrite(p_ctx->idx_fix, p_ctx->idx_fix_off * 4, 1, p_ctx->idx_fp) != 1)
            {
                goto w_err;
            }

            p_ctx->idx_fix_off = 0;
        }

        // Write index temporary file data to the avi file
        
        fseek(p_ctx->idx_fp, 0, SEEK_END);
        
        int idx_len = ftell(p_ctx->idx_fp);
        
        if (idx_len != (p_ctx->i_idx * 16))
        {
            log_print(HT_LOG_ERR, "%s, idx real len[%d] != idx file len[%d],idx_fix_off[%d]!!!\r\n", 
                __FUNCTION__, p_ctx->i_idx * 16, idx_len, p_ctx->idx_fix_off * 4);
            return -1;
        }

        fseek(p_ctx->fp, 0, SEEK_END);
        fseek(p_ctx->idx_fp, 0, SEEK_SET);
        
        int rlen;
        
        do
        {
            rlen = fread(p_ctx->idx_fix, 1, sizeof(p_ctx->idx_fix), p_ctx->idx_fp);
            if (rlen <= 0)
            {
                break;
            }
            
            if (fwrite(p_ctx->idx_fix, rlen, 1, p_ctx->fp) != 1)
            {
                log_print(HT_LOG_ERR, "%s, write idx into avi file failed!!!\r\n", __FUNCTION__);
                return -1;
            }
        } while(rlen > 0);
    }

    return 0;

w_err:

    return -1;
}

void avi_set_dw(void * p, uint32 dw)
{
    uint8 * ptr = (uint8 *)p;

    ptr[0] = (dw      ) & 0xff;
    ptr[1] = (dw >> 8 ) & 0xff;
    ptr[2] = (dw >> 16) & 0xff;
    ptr[3] = (dw >> 24) & 0xff;
}

int avi_end(AVICTX * p_ctx)
{
    if (p_ctx->fp == NULL)
    {
        return -1;
    }
    
    p_ctx->i_movi_end = ftell(p_ctx->fp);

    if (avi_write_idx(p_ctx) < 0)
    {
        goto end_err;
    }
    
    p_ctx->i_riff = ftell(p_ctx->fp);

    if (avi_write_header(p_ctx) < 0)
    {
        goto end_err;
    }
    
    // AVI file has been completed, delete the temporary index file
    if (p_ctx->idx_fp)
    {
        fclose(p_ctx->idx_fp);
        p_ctx->idx_fp = NULL;
        
#if __WINDOWS_OS__
        char filename[256];
        snprintf(filename, sizeof(filename)-1, "%s.idx", p_ctx->filename);
        remove(filename);
#else

        char cmds[256];
        snprintf(cmds, sizeof(cmds)-1, "rm -f %s.idx", p_ctx->filename);
        system(cmds);
#endif        
    }

end_err:

    if (p_ctx->fp)
    {
        fclose(p_ctx->fp);
        p_ctx->fp = NULL;
    }

    avi_free_idx(p_ctx);

    return 0;
}

/**************************************************************************/
AVICTX * avi_write_open(const char * filename)
{
    AVICTX * p_ctx = (AVICTX *)malloc(sizeof(AVICTX));
    if (p_ctx == NULL)
    {
        log_print(HT_LOG_ERR, "%s, malloc fail!!!\r\n", __FUNCTION__);
        return NULL;
    }

    memset(p_ctx, 0, sizeof(AVICTX));

    p_ctx->ctxf_write = 1;

    p_ctx->fp = fopen(filename, "wb+");
    if (p_ctx->fp == NULL)
    {
        free(p_ctx);
        log_print(HT_LOG_ERR, "%s, fopen [%s] failed!!!\r\n", __FUNCTION__, filename);
        return NULL;
    }

    strncpy(p_ctx->filename, filename, sizeof(p_ctx->filename));

    char idx_path[256];
    snprintf(idx_path, sizeof(idx_path), "%s.idx", filename);
    
    p_ctx->idx_fp = fopen(idx_path, "wb+");
    if (p_ctx->idx_fp == NULL)
    {
        fclose(p_ctx->fp);
        free(p_ctx);
        log_print(HT_LOG_ERR, "%s, fopen [%s] failed!!!\r\n", __FUNCTION__, idx_path);
        return NULL;
    }

    p_ctx->mutex = sys_os_create_mutex();

    return p_ctx;
}

int avi_write_video_frame(AVICTX * p_ctx, void * p_data, uint32 len, uint32 ts, int b_key)
{
    int ret = -1;
    BOOL decodable = TRUE;

    if (NULL == p_ctx || NULL == p_ctx->fp)
    {
        return -1;
    }

    int i_pos = ftell(p_ctx->fp);

    avi_write_fourcc(p_ctx, "00dc");
    avi_write_uint32(p_ctx, len);

    if (fwrite(p_data, len, 1, p_ctx->fp) != 1)
    {
        goto w_err;
    }

    if (len & 0x01)    /* pad */
    {
        fputc(0, p_ctx->fp);
    }
    
    if (p_ctx->ctxf_idx_m == 1)
    {
        if (p_ctx->i_idx_max <= p_ctx->i_idx)
        {
            p_ctx->i_idx_max += 1000;
            p_ctx->idx = (int *)realloc(p_ctx->idx, p_ctx->i_idx_max * 16);
            if (p_ctx->idx == NULL)
            {
                log_print(HT_LOG_ERR, "%s, realloc ret null!!!\r\n", __FUNCTION__);
            }
        }

        if (p_ctx->idx)
        {
            memcpy(&p_ctx->idx[4*p_ctx->i_idx+0], "00dc", 4);
            avi_set_dw(&p_ctx->idx[4*p_ctx->i_idx+1], b_key ? AVIIF_KEYFRAME : 0);
            avi_set_dw(&p_ctx->idx[4*p_ctx->i_idx+2], i_pos);
            avi_set_dw(&p_ctx->idx[4*p_ctx->i_idx+3], len);
        
            p_ctx->i_idx++;
        }
    }
    else if (p_ctx->idx_fp)
    {
        memcpy(&p_ctx->idx_fix[p_ctx->idx_fix_off + 0], "00dc", 4);
        avi_set_dw(&p_ctx->idx_fix[p_ctx->idx_fix_off + 1], b_key ? AVIIF_KEYFRAME : 0);
        avi_set_dw(&p_ctx->idx_fix[p_ctx->idx_fix_off + 2], i_pos);
        avi_set_dw(&p_ctx->idx_fix[p_ctx->idx_fix_off + 3], len);

        p_ctx->idx_fix_off += 4;
        
        if (p_ctx->idx_fix_off == (sizeof(p_ctx->idx_fix) / sizeof(int)))
        {
            if (fwrite(p_ctx->idx_fix, sizeof(p_ctx->idx_fix), 1, p_ctx->idx_fp) != 1)
            {
                goto w_err;
            }

            p_ctx->idx_fix_off = 0;
        }

        p_ctx->i_idx++;
    }

    if (memcmp(p_ctx->v_fcc, "H264", 4) == 0 || memcmp(p_ctx->v_fcc, "H265", 4) == 0)
    {
        int codec = VIDEO_CODEC_H264;
            
        if (memcmp(p_ctx->v_fcc, "H264", 4) == 0)
        {
            codec = VIDEO_CODEC_H264;
        }
        else if (memcmp(p_ctx->v_fcc, "H265", 4) == 0)
        {
            codec = VIDEO_CODEC_H265;
        }

        decodable = avc_is_decodable_frame((uint8*)p_data, len, codec);
    }

    if (decodable)
    {
        p_ctx->i_frame_video++;
    }

    if (p_ctx->v_s_time == 0)
    {
        p_ctx->v_s_time = sys_os_get_ms();
        p_ctx->v_e_time = p_ctx->v_s_time;
    }
    else
    {
        p_ctx->v_e_time = sys_os_get_ms();
    }

    p_ctx->prev_ts = ts;
    
    ret = ftell(p_ctx->fp);

w_err:

    if (ret < 0)
    {
        log_print(HT_LOG_ERR, "%s, ret[%d] err[%d] [%s]!!!\r\n", __FUNCTION__, ret, errno, strerror(errno));

        if (p_ctx->fp)
        {
            fclose(p_ctx->fp);
            p_ctx->fp = NULL;
        }
        
        if (p_ctx->idx_fp)
        {
            fclose(p_ctx->idx_fp);
            p_ctx->idx_fp = NULL;
        }
    }
    
    return ret;
}

int avi_write_h264_nalu(AVICTX * p_ctx, uint8 * sps, int sps_len, uint8 * pps, int pps_len)
{
    int flag = 0;

    if (sps_len > 0 && sps)
    {
        if (avi_write_video_frame(p_ctx, sps, sps_len, 0, 0) > 0)
        {
            flag = 1;
        }
    }

    if (pps_len > 0 && pps)
    {
        if (avi_write_video_frame(p_ctx, pps, pps_len, 0, 0) > 0)
        {
            flag = 1;
        }
    }

    p_ctx->ctxf_nalu = flag;

    return flag;
}

int avi_write_h265_nalu(AVICTX * p_ctx, uint8 * vps, int vps_len, uint8 * sps, int sps_len, uint8 * pps, int pps_len)
{
    int flag = 0;

    if (vps_len > 0 && vps)
    {
        if (avi_write_video_frame(p_ctx, vps, vps_len, 0, 0) > 0)
        {
            flag = 1;
        }
    }

    if (sps_len > 0 && sps)
    {
        if (avi_write_video_frame(p_ctx, sps, sps_len, 0, 0) > 0)
        {
            flag = 1;
        }
    }

    if (pps_len > 0 && pps)
    {
        if (avi_write_video_frame(p_ctx, pps, pps_len, 0, 0) > 0)
        {
            flag = 1;
        }
    }

    p_ctx->ctxf_nalu = flag;
    
    return flag;
}

int avi_write_h264(AVICTX * p_ctx, void * p_data, uint32 len, uint32 ts, int b_key)
{
    if (p_ctx->ctxf_nalu)
    {
        if (!p_ctx->ctxf_iframe)
        {
            if (b_key)
            {
                if (avi_write_video_frame(p_ctx, p_data, len, ts, b_key) < 0)
                {
                    log_print(HT_LOG_ERR, "%s, avi_write_video_frame failed\r\n", __FUNCTION__);
                    return -1;
                }
                else
                {
                    p_ctx->ctxf_iframe = 1;
                }
            }
        }
        else
        {
            if (avi_write_video_frame(p_ctx, p_data, len, ts, b_key) < 0)
            {
                log_print(HT_LOG_ERR, "%s, avi_write_video_frame failed\r\n", __FUNCTION__);
                return -1;
            }
        }
    }

    return 0;
}

int avi_write_h265(AVICTX * p_ctx, void * p_data, uint32 len, uint32 ts, int b_key)
{
    if (p_ctx->ctxf_nalu)
    {
        if (!p_ctx->ctxf_iframe)
        {
            if (b_key)
            {
                if (avi_write_video_frame(p_ctx, p_data, len, ts, b_key) < 0)
                {
                    log_print(HT_LOG_ERR, "%s, avi_write_video_frame failed\r\n", __FUNCTION__);
                    return -1;
                }
                else
                {
                    p_ctx->ctxf_iframe = 1;
                }
            }
        }
        else
        {
            if (avi_write_video_frame(p_ctx, p_data, len, ts, b_key) < 0)
            {
                log_print(HT_LOG_ERR, "%s, avi_write_video_frame failed\r\n", __FUNCTION__);
                return -1;
            }
        }
    }

    return 0;
}

int avi_write_video(AVICTX * p_ctx, uint8 * p_data, uint32 len, uint32 ts, int b_key)
{
    int ret = 0;
    
    sys_os_mutex_enter(p_ctx->mutex);

    if (p_ctx->v_fps == 0)
    {
        avi_calc_fps(p_ctx);
    }

    if (memcmp(p_ctx->v_fcc, "H264", 4) == 0)
    {
        ret = avi_write_h264(p_ctx, p_data, len, ts, b_key);
    }
    else if (memcmp(p_ctx->v_fcc, "H265", 4) == 0)
    {
        ret = avi_write_h265(p_ctx, p_data, len, ts, b_key);
    }
    else
    {
        ret = avi_write_video_frame(p_ctx, p_data, len, ts, b_key);
    }

    sys_os_mutex_leave(p_ctx->mutex);
    
    return ret;
}

int avi_write_nalu(AVICTX * p_ctx, uint8 * vps, int vps_len, uint8 * sps, int sps_len, uint8 * pps, int pps_len)
{
    int ret = 0;

    if (p_ctx->ctxf_nalu)
    {
        return 0;
    }

    sys_os_mutex_enter(p_ctx->mutex);
    
    if (memcmp(p_ctx->v_fcc, "H264", 4) == 0)
    {
        ret = avi_write_h264_nalu(p_ctx, sps, sps_len, pps, pps_len);
    }
    else if (memcmp(p_ctx->v_fcc, "H265", 4) == 0)
    {
        ret = avi_write_h265_nalu(p_ctx, vps, vps_len, sps, sps_len, pps, pps_len);
    }

    sys_os_mutex_leave(p_ctx->mutex);
    
    return ret;
}

int avi_write_audio(AVICTX * p_ctx, uint8 * p_data, uint32 len, uint32 ts)
{
    int ret = -1;

    if (NULL == p_ctx)
    {
        return -1;
    }

    if (p_ctx->ctxf_video)
    {
        if (memcmp(p_ctx->v_fcc, "H264", 4) == 0 || 
            memcmp(p_ctx->v_fcc, "H265", 4) == 0)
        {
            if (!p_ctx->ctxf_iframe)
            {
                return 0;
            }
        }
    }

    if (p_ctx->a_fmt == AUDIO_FORMAT_AAC) // AAC
    {
        if (p_data[0] == 0xFF && (p_data[1] & 0xF0) == 0xF0)
        {
            // skip ADTS header
            p_data += 7;
            len -= 7;
        }
    }
    
    sys_os_mutex_enter(p_ctx->mutex);
    
    if (NULL == p_ctx->fp)
    {
        sys_os_mutex_leave(p_ctx->mutex);
        return -1;
    }

    int i_pos = ftell(p_ctx->fp);

    /* chunk header */
    avi_write_fourcc(p_ctx, "01wb");
    avi_write_uint32(p_ctx, len);

    if (fwrite(p_data, len, 1, p_ctx->fp) != 1)
    {
        goto w_err;
    }
    
    if (len & 0x01)    /* pad */
    {
        fputc(0, p_ctx->fp);
    }
    
    if (p_ctx->ctxf_idx_m == 1)
    {
        if (p_ctx->i_idx_max <= p_ctx->i_idx)
        {
            p_ctx->i_idx_max += 1000;
            p_ctx->idx = (int *)realloc(p_ctx->idx, p_ctx->i_idx_max * 16);
            if (p_ctx->idx == NULL)
            {
                log_print(HT_LOG_ERR, "%s, realloc ret null!!!\r\n", __FUNCTION__);
            }
        }

        if (p_ctx->idx)
        {
            memcpy(&p_ctx->idx[4*p_ctx->i_idx+0], "01wb", 4);
            avi_set_dw(&p_ctx->idx[4*p_ctx->i_idx+1], AVIIF_KEYFRAME);
            avi_set_dw(&p_ctx->idx[4*p_ctx->i_idx+2], i_pos);
            avi_set_dw(&p_ctx->idx[4*p_ctx->i_idx+3], len);
        
            p_ctx->i_idx++;
        }
    }
    else if (p_ctx->idx_fp)
    {
        memcpy(&p_ctx->idx_fix[p_ctx->idx_fix_off + 0], "01wb", 4);
        avi_set_dw(&p_ctx->idx_fix[p_ctx->idx_fix_off + 1], AVIIF_KEYFRAME);
        avi_set_dw(&p_ctx->idx_fix[p_ctx->idx_fix_off + 2], i_pos);
        avi_set_dw(&p_ctx->idx_fix[p_ctx->idx_fix_off + 3], len);

        p_ctx->idx_fix_off += 4;
        
        if (p_ctx->idx_fix_off == (sizeof(p_ctx->idx_fix) / sizeof(int)))
        {
            if (fwrite(p_ctx->idx_fix, sizeof(p_ctx->idx_fix), 1, p_ctx->idx_fp) != 1)
            {
                goto w_err;
            }
            
            p_ctx->idx_fix_off = 0;
        }

        p_ctx->i_idx++;
    }

    p_ctx->i_frame_audio++;
    p_ctx->audio_total_bytes += len;

    if (p_ctx->a_s_time == 0)
    {
        p_ctx->a_s_time = sys_os_get_ms();
        p_ctx->a_e_time = p_ctx->a_s_time;
    }
    else
    {
        p_ctx->a_e_time = sys_os_get_ms();
    }
    
    ret = ftell(p_ctx->fp);

w_err:

    if (ret < 0)
    {
        log_print(HT_LOG_ERR, "%s, ret[%d]!!!\r\n", __FUNCTION__, ret);

        if (p_ctx->fp)
        {
            fclose(p_ctx->fp);
            p_ctx->fp = NULL;
        }
        
        if (p_ctx->idx_fp)
        {
            fclose(p_ctx->idx_fp);
            p_ctx->idx_fp = NULL;
        }
    }

    sys_os_mutex_leave(p_ctx->mutex);
    
    return ret;
}

void avi_write_close(AVICTX * p_ctx)
{
    if (NULL == p_ctx)
    {
        return;
    }
    
    sys_os_mutex_enter(p_ctx->mutex);

    avi_end(p_ctx);
    avi_free_idx(p_ctx);

    if (p_ctx->v_extra)
    {
        free(p_ctx->v_extra);
    }

    if (p_ctx->a_extra)
    {
        free(p_ctx->a_extra);
    }

    sys_os_mutex_leave(p_ctx->mutex);
    
    sys_os_destroy_mutex(p_ctx->mutex);

    free(p_ctx);
}

void avi_set_video_info(AVICTX * p_ctx, int fps, int width, int height, const char fcc[4])
{
    memcpy(p_ctx->v_fcc, fcc, 4); // "H264","H265","JPEG","MP4V"
    p_ctx->v_fps = fps;
    p_ctx->v_width  = width;
    p_ctx->v_height = height;

    p_ctx->ctxf_video = 1;
    p_ctx->ctxf_calcfps = fps > 0 ? 0 : 1;
}

void avi_set_video_extra_info(AVICTX * p_ctx, uint8 * extra, int extra_len)
{
    if (NULL != extra && extra_len > 0)
    {
        if (p_ctx->v_extra)
        {
            free(p_ctx->v_extra);
        }

        p_ctx->v_extra_len = 0;
        
        p_ctx->v_extra = (uint8*) malloc(extra_len);
        if (p_ctx->v_extra)
        {
            memcpy(p_ctx->v_extra, extra, extra_len);
            p_ctx->v_extra_len = extra_len;
        }
    }
}

void avi_set_audio_info(AVICTX * p_ctx, int chns, int rate, uint16 fmt)
{
    p_ctx->a_chns = chns;
    p_ctx->a_rate = rate;
    p_ctx->a_fmt = fmt;
    
    p_ctx->ctxf_audio = 1;
}

void avi_set_audio_extra_info(AVICTX * p_ctx, uint8 * extra, int extra_len)
{
    if (NULL != extra && extra_len > 0)
    {
        if (p_ctx->a_extra)
        {
            free(p_ctx->a_extra);
        }

        p_ctx->a_extra_len = 0;
        
        p_ctx->a_extra = (uint8*) malloc(extra_len);
        if (p_ctx->a_extra)
        {
            memcpy(p_ctx->a_extra, extra, extra_len);
            p_ctx->a_extra_len = extra_len;
        }
    }
}

void avi_build_video_hdr(AVICTX * p_ctx)
{
    if (p_ctx->ctxf_video == 0)
    {
        return;
    }

    avi_calc_fps(p_ctx);

    memcpy(&p_ctx->str_v.fccHandler, p_ctx->v_fcc, 4);
    
    p_ctx->str_v.fccType            = mmioFOURCC('v','i','d','s');    
    p_ctx->str_v.dwFlags            = 0;    // Contains AVITF_ flags 
    p_ctx->str_v.wPriority          = 0;
    p_ctx->str_v.wLanguage          = 0;
    p_ctx->str_v.dwInitialFrames    = 0;
    p_ctx->str_v.dwScale            = 1;    
    p_ctx->str_v.dwRate             = (p_ctx->v_fps == 0) ? 25 : p_ctx->v_fps;  // dwRate / dwScale == samplessecond 
    p_ctx->str_v.dwStart            = 0;
    p_ctx->str_v.dwLength           = p_ctx->i_frame_video; // In units above..
    p_ctx->str_v.dwSuggestedBufferSize = 1024*1024;
    p_ctx->str_v.dwQuality          = -1;
    p_ctx->str_v.dwSampleSize       = 0;
    p_ctx->str_v.rcFrame.left       = 0;
    p_ctx->str_v.rcFrame.top        = 0;
    p_ctx->str_v.rcFrame.right      = p_ctx->v_width;
    p_ctx->str_v.rcFrame.bottom     = p_ctx->v_height;

    memcpy(&p_ctx->bmp.biCompression, p_ctx->v_fcc, 4);
    
    p_ctx->bmp.biSize           = sizeof(BMPHDR);
    p_ctx->bmp.biWidth          = p_ctx->v_width;
    p_ctx->bmp.biHeight         = p_ctx->v_height;
    p_ctx->bmp.biPlanes         = 1;
    p_ctx->bmp.biBitCount       = 24;    
    p_ctx->bmp.biSizeImage      = p_ctx->v_width * p_ctx->v_height * 3;
    p_ctx->bmp.biXPelsPerMeter  = 0;
    p_ctx->bmp.biYPelsPerMeter  = 0;
    p_ctx->bmp.biClrUsed        = 0;
    p_ctx->bmp.biClrImportant   = 0;
}

void avi_build_audio_hdr(AVICTX * p_ctx)
{
    if (p_ctx->ctxf_audio == 0)
    {
        return;
    }

    int bps, blkalign, bytespersec;
    uint32 scale = 1;
    uint32 length;

    if (p_ctx->a_fmt == AUDIO_FORMAT_ALAW)      // G711A
    {
        bps = 8;
        blkalign = p_ctx->a_chns;
        bytespersec = p_ctx->a_rate * p_ctx->a_chns;
        length = p_ctx->audio_total_bytes / blkalign;
    }
    else if (p_ctx->a_fmt == AUDIO_FORMAT_MULAW) // G711U
    {
        bps = 8;
        blkalign = p_ctx->a_chns;
        bytespersec = p_ctx->a_rate * p_ctx->a_chns;
        length = p_ctx->audio_total_bytes / blkalign;
    }
    else if (p_ctx->a_fmt == AUDIO_FORMAT_AAC) // AAC
    {
        bps = 16;
        blkalign = 768 * p_ctx->a_chns; /* maximum bytes per frame */
        bytespersec = p_ctx->a_rate * p_ctx->a_chns / 8;
        scale = 1024;
        length = p_ctx->i_frame_audio;
    }
    else if (p_ctx->a_fmt == AUDIO_FORMAT_G726) // G726
    {
        bps = 8;
        blkalign = p_ctx->a_chns;
        bytespersec = p_ctx->a_rate * p_ctx->a_chns;
        length = p_ctx->audio_total_bytes / blkalign;
    }
    else if (p_ctx->a_fmt == AUDIO_FORMAT_G722) // G722
    {
        bps = 4;
        blkalign = p_ctx->a_chns;
        bytespersec = p_ctx->a_rate * p_ctx->a_chns;
        length = p_ctx->audio_total_bytes / blkalign;
    }
    else
    {
        bps = 8;
        blkalign = p_ctx->a_chns;
        bytespersec = p_ctx->a_rate * p_ctx->a_chns;
        length = p_ctx->audio_total_bytes / blkalign;
    }

    if (p_ctx->audio_total_bytes > 0 && p_ctx->a_e_time > p_ctx->a_s_time + 5000)
    {
        bytespersec = p_ctx->audio_total_bytes / ((p_ctx->a_e_time - p_ctx->a_s_time) / 1000);
    }
    
    p_ctx->str_a.fccType            = mmioFOURCC('a','u','d','s');
    p_ctx->str_a.fccHandler         = 1;
    p_ctx->str_a.dwFlags            = 0;    // Contains AVITF_ flags 
    p_ctx->str_a.wPriority          = 0;
    p_ctx->str_a.wLanguage          = 0;
    p_ctx->str_a.dwInitialFrames    = 0;
    p_ctx->str_a.dwScale            = scale;
    p_ctx->str_a.dwRate             = p_ctx->a_rate;
    p_ctx->str_a.dwStart            = 0;
    p_ctx->str_a.dwLength           = length;
    p_ctx->str_a.dwSuggestedBufferSize = 12*1024;
    p_ctx->str_a.dwQuality          = -1;
    p_ctx->str_a.dwSampleSize       = blkalign;
    p_ctx->str_a.rcFrame.left       = 0;
    p_ctx->str_a.rcFrame.top        = 0;
    p_ctx->str_a.rcFrame.right      = 0;
    p_ctx->str_a.rcFrame.bottom     = 0;
    
    p_ctx->wave.wFormatTag          = p_ctx->a_fmt;
    p_ctx->wave.nChannels           = p_ctx->a_chns;    
    p_ctx->wave.nSamplesPerSec      = p_ctx->a_rate;
    p_ctx->wave.nAvgBytesPerSec     = bytespersec;
    p_ctx->wave.nBlockAlign         = blkalign;
    p_ctx->wave.wBitsPerSample      = bps;
    p_ctx->wave.cbSize              = p_ctx->a_extra_len;
}

int avi_write_header(AVICTX * p_ctx)
{
    if (p_ctx->fp == NULL)
    {
        return -1;
    }

    log_print(HT_LOG_DBG, "%s, enter...\r\n", __FUNCTION__);
    
    avi_build_video_hdr(p_ctx);
    avi_build_audio_hdr(p_ctx);

    fseek(p_ctx->fp, 0, SEEK_SET);

    int tlen;
    int avih_len = sizeof(AVIMHDR) + 8;
    int strl_v_len = sizeof(AVISHDR) + 8 + sizeof(BMPHDR) + 8;
    int strl_a_len = sizeof(AVISHDR) + 8 + sizeof(WAVEFMT) + 8;
    int s_v_ll = strl_v_len + 12;
    int s_a_ll = strl_a_len + 12;
    int a_extra_len = 0;
    int a_pad_len = 0;
    int v_extra_len = 0;
    int v_pad_len = 0;

    if (p_ctx->a_extra && p_ctx->a_extra_len)
    {
        a_extra_len = p_ctx->a_extra_len;

        if (a_extra_len & 0x01)
        {
            a_pad_len = 1;
        }
    }

    if (p_ctx->v_extra && p_ctx->v_extra_len)
    {
        v_extra_len = p_ctx->v_extra_len;

        if (v_extra_len & 0x01)
        {
            v_pad_len = 1;
        }
    }

    avi_write_fourcc(p_ctx, "RIFF");
    avi_write_uint32(p_ctx, p_ctx->i_riff > 0 ? p_ctx->i_riff - 8 : 0xFFFFFFFF); // Total file length - ('RIFF') - 4 
    avi_write_fourcc(p_ctx, "AVI ");

    avi_write_fourcc(p_ctx, "LIST");

    tlen = 4 + avih_len;

    if (p_ctx->ctxf_video == 1)
    {
        tlen += s_v_ll + v_extra_len + v_pad_len;
    }
    
    if (p_ctx->ctxf_audio == 1)
    {
        tlen += s_a_ll + a_extra_len + a_pad_len;
    }

    avi_write_uint32(p_ctx,  tlen);
    
    avi_write_fourcc(p_ctx, "hdrl");

    avi_write_fourcc(p_ctx, "avih");
    avi_write_uint32(p_ctx, sizeof(AVIMHDR));

    if (p_ctx->v_fps == 0)
    {
        p_ctx->avi_hdr.dwMicroSecPerFrame   = 1000000 / 25;         // Video frame interval (in microseconds)
    }    
    else
    {
        p_ctx->avi_hdr.dwMicroSecPerFrame   = 1000000 / p_ctx->v_fps;   // Video frame interval (in microseconds)
    }
    
    p_ctx->avi_hdr.dwMaxBytesPerSec     = 0xffffffff;               // The maximum data rate of this AVI file
    p_ctx->avi_hdr.dwPaddingGranularity = 0;                        // Granularity of data padding
    p_ctx->avi_hdr.dwFlags              = AVIF_HASINDEX|AVIF_ISINTERLEAVED|AVIF_TRUSTCKTYPE;
    p_ctx->avi_hdr.dwTotalFrames        = p_ctx->i_frame_video;     // Total number of frames
    p_ctx->avi_hdr.dwInitialFrames      = 0;                        // Specify the initial number of frames for the interactive format

    if (p_ctx->ctxf_audio == 1)
    {
        p_ctx->avi_hdr.dwStreams        = 2;                        // The number of streams included in this file
    }    
    else
    {
        p_ctx->avi_hdr.dwStreams        = 1;                        // The number of streams included in this file
    }
    
    p_ctx->avi_hdr.dwSuggestedBufferSize= 1024*1024;                // It is recommended to read the cache size of this file (should be able to accommodate the largest block)
    p_ctx->avi_hdr.dwWidth              = p_ctx->v_width;           // The width of the video in pixels
    p_ctx->avi_hdr.dwHeight             = p_ctx->v_height;          // The height of the video in pixels

    avi_write_buffer(p_ctx, (char*)&p_ctx->avi_hdr, sizeof(AVIMHDR));

    if (p_ctx->ctxf_video == 1)
    {
        avi_write_fourcc(p_ctx, "LIST");
        avi_write_uint32(p_ctx,  4 + strl_v_len + v_extra_len + v_pad_len);
        avi_write_fourcc(p_ctx, "strl");                            // How many streams are there in the file, and how many 'strl' sublists there are

        avi_write_fourcc(p_ctx, "strh");
        avi_write_uint32(p_ctx,  sizeof(AVISHDR));
        avi_write_buffer(p_ctx, (char*)&p_ctx->str_v, sizeof(AVISHDR));

        avi_write_fourcc(p_ctx, "strf");
        avi_write_uint32(p_ctx,  sizeof(BMPHDR) + v_extra_len);
        avi_write_buffer(p_ctx, (char*)&p_ctx->bmp, sizeof(BMPHDR));

        // Write video extra information
        
        if (p_ctx->v_extra && p_ctx->v_extra_len)
        {
            avi_write_buffer(p_ctx, (char*)p_ctx->v_extra, p_ctx->v_extra_len);

            if (v_pad_len)    /* pad */
            {
                fputc(0, p_ctx->fp);
            }
        }
    }

    if (p_ctx->ctxf_audio == 1)
    {
        avi_write_fourcc(p_ctx, "LIST");
        avi_write_uint32(p_ctx,  4 + strl_a_len + a_extra_len + a_pad_len);
        avi_write_fourcc(p_ctx, "strl");

        avi_write_fourcc(p_ctx, "strh");
        avi_write_uint32(p_ctx,  sizeof(AVISHDR));
        avi_write_buffer(p_ctx, (char*)&p_ctx->str_a, sizeof(AVISHDR));

        avi_write_fourcc(p_ctx, "strf");
        avi_write_uint32(p_ctx,  sizeof(WAVEFMT) + a_extra_len);
        avi_write_buffer(p_ctx, (char*)&p_ctx->wave, sizeof(WAVEFMT));

        // Write audio extra information
        
        if (p_ctx->a_extra && p_ctx->a_extra_len)
        {
            avi_write_buffer(p_ctx, (char*)p_ctx->a_extra, p_ctx->a_extra_len);

            if (a_pad_len)    /* pad */
            {
                fputc(0, p_ctx->fp);
            }
        }
    }

    avi_write_fourcc(p_ctx, "LIST");
    avi_write_uint32(p_ctx,  p_ctx->i_movi_end > 0 ? (p_ctx->i_movi_end - p_ctx->i_movi + 4) : 0xFFFFFFFF);
    avi_write_fourcc(p_ctx, "movi");

    p_ctx->i_movi = ftell(p_ctx->fp);
    
    if (p_ctx->i_movi < 0)
    {
        goto w_err;
    }
    
    return 0;

w_err:

    if (p_ctx->fp)
    {
        fclose(p_ctx->fp);
        p_ctx->fp = NULL;
    }

    return -1;
}

int avi_update_header(AVICTX * p_ctx)
{
    log_print(HT_LOG_DBG, "%s, enter...\r\n", __FUNCTION__);

    if (NULL == p_ctx)
    {
        return -1;
    }
    
    sys_os_mutex_enter(p_ctx->mutex);

    if (NULL == p_ctx->fp)
    {
        sys_os_mutex_leave(p_ctx->mutex);
        return -1;
    }
    
    if (avi_write_header(p_ctx) == 0)
    {
        if (p_ctx->fp)
        {
            fseek(p_ctx->fp, 0, SEEK_END);

            sys_os_mutex_leave(p_ctx->mutex);
            return 0;
        }
    }

    sys_os_mutex_leave(p_ctx->mutex);
    return -1;
}

int avi_calc_fps(AVICTX * p_ctx)
{
    if (p_ctx->ctxf_calcfps && p_ctx->i_frame_video >= 100)
    {
        float fps = (float) (p_ctx->i_frame_video * 1000.0) / (p_ctx->v_e_time - p_ctx->v_s_time);
        p_ctx->v_fps = (uint32)(fps);

        log_print(HT_LOG_DBG, "%s, stime=%u, etime=%u, frames=%d, fps=%d\r\n", 
            __FUNCTION__, p_ctx->v_s_time, p_ctx->v_e_time, p_ctx->i_frame_video, p_ctx->v_fps);
    }

    return 0;
}

uint64 avi_get_file_length(AVICTX * p_ctx)
{
    if (p_ctx && p_ctx->fp)
    {
        return ftell(p_ctx->fp);
    }

    return 0;
}

uint32 avi_get_media_time(AVICTX * p_ctx)
{
    if (NULL == p_ctx)
    {
        return 0;
    }

    uint32 vtime = 0, atime = 0;
    
    if (p_ctx->ctxf_video)
    {
        vtime = p_ctx->v_e_time - p_ctx->v_s_time;
    }

    if (p_ctx->ctxf_audio)
    {
        atime = p_ctx->a_e_time - p_ctx->a_s_time;
    }

    return vtime > atime ? vtime : atime;
}



