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
#include "video_player.h"
#include "utils.h"
#include "media_util.h"
#include "media_parse.h"
#include "media_codec.h"
#include "h264.h"
#include "h265.h"

extern "C" 
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/intreadwrite.h>
#include <libavutil/avstring.h>
#include <libavutil/base64.h>
#include <libavutil/imgutils.h>
}

#if __WINDOWS_OS__
#include "video_render_d3d.h"
#include "video_render_gdi.h"
#include "audio_play_win.h"
#elif defined(IOS)
#include "video_render_sdl.h"
#include "audio_play_mac.h"
#include <Foundation/Foundation.h>
#elif __LINUX_OS__
#include "video_render_sdl.h"
#include "audio_play_qt.h"
#endif

void VideoDecoderCallback(AVFrame * frame, void * userdata)
{
    CVideoPlayer * pPlayer = (CVideoPlayer *) userdata;

    pPlayer->onVideoFrame(frame);
}

void AudioDecoderCallback(AVFrame * frame, void * userdata)
{
    CVideoPlayer * pPlayer = (CVideoPlayer *) userdata;

    pPlayer->onAudioFrame(frame);
}

void * AudioPlayThread(void * argv)
{
    CVideoPlayer * pPlayer = (CVideoPlayer *) argv;

    pPlayer->audioPlayThread();
    
    return NULL;
}

void * VideoPlayThread(void * argv)
{
    CVideoPlayer * pPlayer = (CVideoPlayer *) argv;

    pPlayer->videoPlayThread();
    
    return NULL;
}

CVideoPlayer::CVideoPlayer(QObject * parent)
: QObject(parent)
, m_bVideoInited(FALSE)
, m_bAudioInited(FALSE)
, m_pVideoDecoder(NULL)
, m_pAudioDecoder(NULL)
, m_pVideoRender(NULL)
, m_pAudioPlay(NULL)
, m_bPlaying(FALSE)
, m_bPaused(FALSE)
, m_nVideoWnd(0)
, m_bSizeChanged(FALSE)
, m_nRenderMode(RENDER_MODE_KEEP)
, m_nHWDecoding(HW_DECODING_AUTO)
, m_nDstVideoFmt(AV_PIX_FMT_YUV420P)
, m_bUpdown(FALSE)
, m_bSnapshot(FALSE)
, m_nSnapVideoFmt(VIDEO_FMT_BGR24)
, m_nVideoCodec(VIDEO_CODEC_NONE)
, m_nAudioCodec(AUDIO_CODEC_NONE)
, m_nSampleRate(0)
, m_nChannel(0)
, m_nBitPerSample(0)
, m_pSnapFrame(NULL)
, m_pRenderFrame(NULL)
, m_bRecording(FALSE)
, m_bNalFlag(FALSE)
, m_pAviCtx(NULL)
, m_pAudioListMutex(NULL)
, m_audioPlayFlag(FALSE)
, m_audioPlayThread(0)
, m_pVideoListMutex(NULL)
, m_videoPlayFlag(FALSE)
, m_videoPlayThread(0)
, m_nLastAudioPts(AV_NOPTS_VALUE)
, m_lastAudioTS(0)
{
    m_pRecordMutex = sys_os_create_mutex();

    memset(&m_h26XParamSets, 0, sizeof(H26XParamSets));
    memset(&m_audioClock, 0, sizeof(HTCLOCK));
    memset(&m_videoClock, 0, sizeof(HTCLOCK));
}

CVideoPlayer::~CVideoPlayer()
{
    close();
}

BOOL CVideoPlayer::open(QString fileName, WId hWnd)
{
    m_sFileName = fileName;
    m_nVideoWnd = hWnd;

    return TRUE;
}

void CVideoPlayer::close()
{
    closeVideo();
    closeAudio();

    if (m_pSnapFrame)
    {
        av_frame_free(&m_pSnapFrame);
    }
    
    if (m_pRenderFrame)
    {
        av_frame_free(&m_pRenderFrame);
    }

    stopRecord();

    sys_os_destroy_mutex(m_pRecordMutex);
    m_pRecordMutex = NULL;
}

void CVideoPlayer::setVolume(int volume)
{
    if (m_pAudioPlay)
    {
        m_pAudioPlay->setVolume(volume);
    }
}

void CVideoPlayer::snapshot(int videofmt)
{
    m_bSnapshot = TRUE;
    m_nSnapVideoFmt = videofmt;
}

BOOL CVideoPlayer::record(QString baseName)
{
    if (m_bRecording)
    {
        return TRUE;
    }

    QString path = getRecordPath();
    QString file = path + "/" + getTempFile(baseName, ".avi");

    m_sBaseName = baseName;
    
    m_pAviCtx = avi_write_open(file.toLocal8Bit().toStdString().c_str());
    if (NULL == m_pAviCtx)
    {
        log_print(HT_LOG_ERR, "%s, avi_write_open failed. %s\r\n", 
            __FUNCTION__, file.toLocal8Bit().toStdString().c_str());
        return FALSE;
    }
    
    if (!onRecord())
    {
        avi_write_close(m_pAviCtx);
        m_pAviCtx = NULL;
        
        return FALSE;
    }

    m_bRecording = TRUE;
    
    return m_bRecording;
}

void CVideoPlayer::stopRecord()
{
    sys_os_mutex_enter(m_pRecordMutex);
    
    m_bRecording = FALSE;
    m_bNalFlag = FALSE;

    memset(&m_h26XParamSets, 0, sizeof(H26XParamSets));

    if (m_pAviCtx)
    {
        avi_write_close(m_pAviCtx);
        m_pAviCtx = NULL;
    }

    sys_os_mutex_leave(m_pRecordMutex);
}

void CVideoPlayer::recordVideo(uint8 * data, int len, uint32 ts, uint16 seq)
{
    int codec = VIDEO_CODEC_NONE;

    if (!memcmp(m_pAviCtx->v_fcc, "H264", 4))
    {
        codec = VIDEO_CODEC_H264;
    }
    else if (!memcmp(m_pAviCtx->v_fcc, "H265", 4))
    {
        codec = VIDEO_CODEC_H265;
    }

    if ((VIDEO_CODEC_H264 == codec || VIDEO_CODEC_H265 == codec) && !m_bNalFlag)
    {
        if (avc_get_h26x_paramsets(data, len, codec, &m_h26XParamSets))
        {
            avi_write_nalu(m_pAviCtx, 
                m_h26XParamSets.vps, m_h26XParamSets.vps_size, 
                m_h26XParamSets.sps, m_h26XParamSets.sps_size,
                m_h26XParamSets.pps, m_h26XParamSets.pps_size);
            m_bNalFlag = 1;
        }
    }
    
    recordVideoEx(data, len, ts, seq);

    if (recordSwitchCheck())
    {
        recordFileSwitch();
    }
}

void CVideoPlayer::recordVideoEx(uint8 * data, int len, uint32 ts, uint16 seq)
{
    AVICTX * p_avictx = m_pAviCtx;

    if (p_avictx->v_width == 0 || p_avictx->v_height == 0)
    {
        int codec = VIDEO_CODEC_NONE;

        if (memcmp(p_avictx->v_fcc, "H264", 4) == 0)
        {
            codec = VIDEO_CODEC_H264;
        }
        else if (memcmp(p_avictx->v_fcc, "H265", 4) == 0)
        {
            codec = VIDEO_CODEC_H265;
        }
        else if (memcmp(p_avictx->v_fcc, "JPEG", 4) == 0)
        {
            codec = VIDEO_CODEC_JPEG;
        }
        else if (memcmp(p_avictx->v_fcc, "MP4V", 4) == 0)
        {
            codec = VIDEO_CODEC_MP4;
        }
        
        avc_parse_video_size(codec, data, len, &p_avictx->v_width, &p_avictx->v_height);
        
        if (p_avictx->v_width && p_avictx->v_height)
        {
            avi_update_header(p_avictx);
        }
    }

    int key = 0;

    if (memcmp(p_avictx->v_fcc, "H264", 4) == 0)
    {
        uint8 nalu_t = (data[4] & 0x1F);
        key = (nalu_t == 5 || nalu_t == 7 || nalu_t == 8);
    }
    else if (memcmp(p_avictx->v_fcc, "H265", 4) == 0)
    {
        uint8 nalu_t = (data[4] >> 1) & 0x3F;
        key = ((nalu_t >= 16 && nalu_t <= 21) || nalu_t == 32 || nalu_t == 33 || nalu_t == 34);
    } 
    else if (memcmp(p_avictx->v_fcc, "MP4V", 4) == 0)
    {
        key = 1;
    }
    else if (memcmp(p_avictx->v_fcc, "JPEG", 4) == 0)
    {
        key = 1;
    }
    
    avi_write_video(p_avictx, data, len, ts, key);
}

void CVideoPlayer::recordAudio(uint8 * data, int len, uint32 ts, uint16 seq)
{
    AVICTX * p_avictx = m_pAviCtx;
    
    avi_write_audio(p_avictx, data, len, ts);

    if (recordSwitchCheck())
    {
        recordFileSwitch();
    }
}

BOOL CVideoPlayer::recordSwitchCheck()
{
    uint64 tlen = avi_get_file_length(m_pAviCtx);
    uint32 mtime = avi_get_media_time(m_pAviCtx);

    uint32 recordSize = getRecordSize();
    if (recordSize == 0)
    {
        recordSize = 1048576; // max 1G file size
    }
    
    // Switch according to the recording size
    if (tlen > recordSize * 1024)
    {
        return TRUE;
    }

    uint32 recordTime = getRecordTime();
    
    // Switch according to the recording duration
    if (recordTime > 0 && mtime > recordTime * 1000)
    {
        return TRUE;
    }
 
    return FALSE;
}

void CVideoPlayer::recordFileSwitch()
{
    AVICTX * p_ctx;
    AVICTX * p_oldctx = m_pAviCtx;

    QString path = getRecordPath();
    QString file = path + "/" + getTempFile(m_sBaseName, ".avi");
    
    p_ctx = avi_write_open(file.toLocal8Bit().toStdString().c_str());
    if (NULL == p_ctx)
    {
        return;
    }
 
    p_ctx->ctxf_video = p_oldctx->ctxf_video;
    p_ctx->ctxf_audio = p_oldctx->ctxf_audio;
 
    if (p_ctx->ctxf_video)
    {
        avi_calc_fps(p_oldctx);
        avi_set_video_info(p_ctx, p_oldctx->v_fps, p_oldctx->v_width, p_oldctx->v_height, p_oldctx->v_fcc);
        avi_set_video_extra_info(p_ctx, p_oldctx->v_extra, p_oldctx->v_extra_len);
    }
 
    if (p_ctx->ctxf_audio)
    {
        avi_set_audio_info(p_ctx, p_oldctx->a_chns, p_oldctx->a_rate, p_oldctx->a_fmt);
        avi_set_audio_extra_info(p_ctx, p_oldctx->a_extra, p_oldctx->a_extra_len);
    }
    
    avi_write_close(p_oldctx);
 
    avi_update_header(p_ctx);

    m_pAviCtx = p_ctx;
    
    if (m_h26XParamSets.vps_size > 0 || 
        m_h26XParamSets.sps_size > 0 || 
        m_h26XParamSets.pps_size > 0)
    {
        avi_write_nalu(m_pAviCtx, 
            m_h26XParamSets.vps, m_h26XParamSets.vps_size, 
            m_h26XParamSets.sps, m_h26XParamSets.sps_size,
            m_h26XParamSets.pps, m_h26XParamSets.pps_size);
    }
}

BOOL CVideoPlayer::openVideo(enum AVCodecID codec, uint8 * extradata, int extradata_size)
{
    if (m_bVideoInited)
    {
        return TRUE;
    }
    
    m_pVideoDecoder = new CVideoDecoder();
    if (m_pVideoDecoder)
    {
        m_bVideoInited = m_pVideoDecoder->init(codec, extradata, extradata_size, m_nHWDecoding);
    }

    if (m_bVideoInited)
    {
        m_pVideoDecoder->setCallback(VideoDecoderCallback, this);

        m_pVideoListMutex = sys_os_create_mutex();

        m_videoPlayFlag = TRUE;
        m_videoPlayThread = sys_os_create_thread((void *)VideoPlayThread, this);
    }

    m_nVideoCodec = to_video_codec(codec);
    
    return m_bVideoInited;
}

BOOL CVideoPlayer::openVideo(int codec, uint8 * extradata, int extradata_size)
{
    return openVideo(to_video_avcodecid(codec), extradata, extradata_size);
}

void CVideoPlayer::closeVideo()
{
    m_videoPlayFlag = FALSE;

    // Wait for the video playback thread to exit
    sys_os_wait_thread(&m_videoPlayThread);
    
    if (m_pVideoDecoder)
    {
        delete m_pVideoDecoder;
        m_pVideoDecoder = NULL;
    }

    if (m_pVideoRender)
    {
        delete m_pVideoRender;
        m_pVideoRender = NULL;
    }

    // Released video frame
    while (m_videoFrameList.size() > 0)
    {
        AVFrame * pFrame = m_videoFrameList.front();

        m_videoFrameList.pop_front();

        av_frame_free(&pFrame);
    }

    if (m_pVideoListMutex)
    {
        sys_os_destroy_mutex(m_pVideoListMutex);
        m_pVideoListMutex = NULL;
    }
    
    m_bVideoInited = FALSE;
}

BOOL CVideoPlayer::openAudio(enum AVCodecID codec, int samplerate, int channels, int bitpersample)
{
    if (m_bAudioInited)
    {
        return TRUE;
    }
    
    m_pAudioDecoder = new CAudioDecoder();
    if (m_pAudioDecoder)
    {    
        m_bAudioInited = m_pAudioDecoder->init(codec, samplerate, channels, bitpersample);
    }

    if (m_bAudioInited)
    {
        m_pAudioDecoder->setCallback(AudioDecoderCallback, this);

#if __WINDOWS_OS__
        m_pAudioPlay = new CWAudioPlay();
#elif defined(IOS)
        m_pAudioPlay = new CMAudioPlay();
#elif __LINUX_OS__
        m_pAudioPlay = new CQAudioPlay();
#endif
        if (m_pAudioPlay)
        {
            m_pAudioPlay->startPlay(samplerate, channels);
        }

        m_pAudioListMutex = sys_os_create_mutex();

        m_audioPlayFlag = TRUE;
        m_audioPlayThread = sys_os_create_thread((void *)AudioPlayThread, this);
    }

    m_nAudioCodec = to_audio_codec(codec);
    m_nSampleRate = samplerate;
    m_nChannel = channels;
    m_nBitPerSample = bitpersample;

    return m_bAudioInited;
}

BOOL CVideoPlayer::openAudio(int codec, int samplerate, int channels, int bitpersample)
{
    return openAudio(to_audio_avcodecid(codec), samplerate, channels, bitpersample);
}

void CVideoPlayer::closeAudio()
{
    m_audioPlayFlag = FALSE;

    // Wait for the audio playback thread to exit
    sys_os_wait_thread(&m_audioPlayThread);
    
    if (m_pAudioDecoder)
    {
        delete m_pAudioDecoder;
        m_pAudioDecoder = NULL;
    }

    if (m_pAudioPlay)
    {
        delete m_pAudioPlay;
        m_pAudioPlay = NULL;
    }

    // Released audio frame
    while (m_audioFrameList.size() > 0)
    {
        AVFrame * pFrame = m_audioFrameList.front();

        m_audioFrameList.pop_front();

        av_frame_free(&pFrame);
    }

    if (m_pAudioListMutex)
    {
        sys_os_destroy_mutex(m_pAudioListMutex);
        m_pAudioListMutex = NULL;
    }

    m_bAudioInited = FALSE;
}

void CVideoPlayer::setWindowSize(QSize size)
{
    m_bSizeChanged = TRUE;
    m_size = size;
}

int CVideoPlayer::getVideoWidth()
{
    if (m_pVideoDecoder)
    {
        return m_pVideoDecoder->getWidth();
    }

    return 0;
}

int CVideoPlayer::getVideoHeight()
{
    if (m_pVideoDecoder)
    {
        return m_pVideoDecoder->getHeight();
    }

    return 0;
}

double CVideoPlayer::getFrameRate()
{
    if (m_pVideoDecoder)
    {
        return m_pVideoDecoder->getFrameRate();
    }

    return 0;
}

void CVideoPlayer::playVideo(uint8 * data, int len, uint32 ts, uint16 seq)
{
    sys_os_mutex_enter(m_pRecordMutex);
    if (m_bRecording)
    {
        recordVideo(data, len, ts, seq);
    }
    sys_os_mutex_leave(m_pRecordMutex);

    updateClock(&m_videoClock, ts, getVideoClock());
    
    if (m_bVideoInited)
    {
        m_pVideoDecoder->decode(data, len, m_videoClock.SyncTime.tv_sec * 1000000 + m_videoClock.SyncTime.tv_usec);
    }
}

void CVideoPlayer::playAudio(uint8 * data, int len, uint32 ts, uint16 seq)
{
    sys_os_mutex_enter(m_pRecordMutex);
    if (m_bRecording)
    {
        recordAudio(data, len, ts, seq);
    }
    sys_os_mutex_leave(m_pRecordMutex);

    updateClock(&m_audioClock, ts, getAudioClock());
    
    if (m_bAudioInited)
    {
        m_pAudioDecoder->decode(data, len, m_audioClock.SyncTime.tv_sec * 1000000 + m_audioClock.SyncTime.tv_usec);
    }
}

void CVideoPlayer::updateClock(HTCLOCK * clock, uint32 ts, int frequency)
{
    if (ts == 0)
    {
        return;
    }
    
    if (clock->SyncTime.tv_sec == 0 && clock->SyncTime.tv_usec == 0)
    {
        clock->SyncTimestamp = ts;
        gettimeofday(&clock->SyncTime, NULL);
    }

    int timestampDiff = ts - clock->SyncTimestamp;

    // Divide this by the timestamp frequency to get real time:
    double timeDiff = timestampDiff / (double)frequency;

    uint32 const million = 1000000;
    uint32 seconds, uSeconds;
    
    if (timeDiff >= 0.0) 
    {
        seconds = clock->SyncTime.tv_sec + (uint32)(timeDiff);
        uSeconds = clock->SyncTime.tv_usec + (uint32)((timeDiff - (uint32)timeDiff)*million);
        if (uSeconds >= million)
        {
            uSeconds -= million;
            ++seconds;
        }
    } 
    else 
    {
        timeDiff = -timeDiff;
        seconds = clock->SyncTime.tv_sec - (uint32)(timeDiff);
        uSeconds = clock->SyncTime.tv_usec - (uint32)((timeDiff - (uint32)timeDiff)*million);
        if ((int)uSeconds < 0) 
        {
            uSeconds += million;
            --seconds;
        }
    }

    // Save these as the new synchronization timestamp & time:
    clock->SyncTimestamp = ts;
    clock->SyncTime.tv_sec = seconds;
    clock->SyncTime.tv_usec = uSeconds;
}

BOOL CVideoPlayer::initFrame(AVFrame *& frame, int width, int height, AVPixelFormat pixfmt)
{
    if (width == 0 || height == 0 || pixfmt == AV_PIX_FMT_NONE)
    {
        return FALSE;
    }
    
    if (NULL == frame || frame->width != width || frame->height != height || frame->format != pixfmt)
    {
        if (frame)
        {
            av_frame_free(&frame);
        }
        
        frame = av_frame_alloc();        
        if (NULL == frame)
        {
            return FALSE;
        }

        frame->format = pixfmt;
        frame->width  = width; 
        frame->height = height;

        if (0 != av_frame_get_buffer(frame, 0))
        {
            av_frame_free(&frame);
            return FALSE;
        }

        av_frame_make_writable(frame);
    }

    return TRUE;
}

BOOL CVideoPlayer::initVideoRender(int width, int height)
{
    BOOL init = FALSE;

#if __WINDOWS_OS__
    m_pVideoRender = new CD3DVideoRender();
#elif __LINUX_OS__
    m_pVideoRender = new CSDLVideoRender();
#endif
    if (m_pVideoRender)
    {
        init = m_pVideoRender->init(m_nVideoWnd, width, height, VIDEO_FMT_YUV420P);
#ifdef IOS
        if (init)
        {
            m_pVideoRender->setWindowTitle(m_sFileName.toStdString().c_str());
        }
#endif
    }

#if __WINDOWS_OS__
    if (!init)
    {
        if (m_pVideoRender)
        {
            delete m_pVideoRender;
        }

        m_pVideoRender = new CGDIVideoRender();
        if (m_pVideoRender)
        {
            init = m_pVideoRender->init(m_nVideoWnd, width, height, VIDEO_FMT_BGR24);
            if (init)
            {
                m_bUpdown = TRUE;
                m_nDstVideoFmt = to_avpixelformat(VIDEO_FMT_BGR24);
                log_print(HT_LOG_INFO, "%s, use GDI to render video\r\n", __FUNCTION__);
            }
            else
            {
                delete m_pVideoRender;
                m_pVideoRender = NULL;
            }
        }
    }
    else
    {
        log_print(HT_LOG_INFO, "%s, use D3D to render video\r\n", __FUNCTION__);
    }
#endif

    return init;
}

BOOL CVideoPlayer::doSnapshot(AVFrame * frame)
{
    if (!initFrame(m_pSnapFrame, frame->width, frame->height, to_avpixelformat(m_nSnapVideoFmt)))
    {
        return FALSE;
    }

    if (NULL == convertFrame(frame, m_pSnapFrame, FALSE))
    {
        return FALSE;
    }
    
    emit snapshoted(m_pSnapFrame);

    return TRUE;
}

AVFrame * CVideoPlayer::convertFrame(AVFrame * srcframe, AVFrame * dstframe, BOOL updown)
{
    if (NULL == srcframe || NULL == dstframe)
    {
        return NULL;
    }
    
    SwsContext * swsctx = sws_getContext(srcframe->width, srcframe->height, (enum AVPixelFormat)srcframe->format,
        srcframe->width, srcframe->height, (enum AVPixelFormat)dstframe->format, SWS_BICUBIC, NULL, NULL, NULL);
    if (swsctx)
    {
        if (updown)
        {
            srcframe->data[0] += srcframe->linesize[0] * (srcframe->height - 1);
            srcframe->linesize[0] *= -1;
            srcframe->data[1] += srcframe->linesize[1] * (srcframe->height / 2 - 1);
            srcframe->linesize[1] *= -1;
            srcframe->data[2] += srcframe->linesize[2] * (srcframe->height / 2 - 1);
            srcframe->linesize[2] *= -1;
        }
                
        int ret = sws_scale(swsctx, srcframe->data, srcframe->linesize, 0, srcframe->height, dstframe->data, dstframe->linesize);
        if (ret > 0)
        {
            dstframe->pts = srcframe->pts;
            dstframe->pkt_dts = srcframe->pkt_dts;
            
            sws_freeContext(swsctx);
            return dstframe;
        }
        else
        {
            log_print(HT_LOG_ERR, "%s, sws_scale failed\r\n", __FUNCTION__);
            
            sws_freeContext(swsctx);
            return NULL;
        }
    }

    return NULL;
}

void CVideoPlayer::onVideoFrame(AVFrame * frame)
{
    // Perform snapshot request
    if (m_bSnapshot)
    {
        if (doSnapshot(frame))
        {
            m_bSnapshot = FALSE;
        }
    }

    AVFrame * dst = NULL;
    
    if (m_nDstVideoFmt != frame->format)
    {
        if (initFrame(m_pRenderFrame, frame->width, frame->height, m_nDstVideoFmt))
        {
            if (NULL != convertFrame(frame, m_pRenderFrame, m_bUpdown))
            {
                dst = av_frame_clone(m_pRenderFrame);
            }
        }
    }
    else 
    {
        dst = av_frame_clone(frame);
    }

    if (dst)
    {
        sys_os_mutex_enter(m_pVideoListMutex);

        if (m_videoFrameList.size() >= 10)
        {
            AVFrame * frame = m_videoFrameList.front();
            if (frame)
            {
                av_frame_free(&frame);
            }
            
            m_videoFrameList.pop_front();
        }
        
        m_videoFrameList.push_back(dst);
        
        sys_os_mutex_leave(m_pVideoListMutex);
    }
}

void CVideoPlayer::onAudioFrame(AVFrame * frame)
{
    AVFrame * dst = av_frame_clone(frame);
    if (dst)
    {
        while (m_audioPlayFlag && m_audioFrameList.size() >= 10)
        {
            usleep(10*1000);
        }
        
        sys_os_mutex_enter(m_pAudioListMutex);
        m_audioFrameList.push_back(dst);
        sys_os_mutex_leave(m_pAudioListMutex);
    }
}

/* Audio play thread */
void CVideoPlayer::audioPlayThread()
{
    while (m_audioPlayFlag)
    {
        AVFrame * pFrame = NULL;

        sys_os_mutex_enter(m_pAudioListMutex);
        if (m_audioFrameList.size() > 0)
        {
            pFrame = m_audioFrameList.front();

            m_audioFrameList.pop_front();
        }
        sys_os_mutex_leave(m_pAudioListMutex);

        if (pFrame)
        {
            // Save audio PTS and play time stamp
            m_lastAudioTS = sys_os_get_ms();
            m_nLastAudioPts = pFrame->pts;
            
            if (m_pAudioPlay)
            {
                // Will wait for the playback to complete before returning
                m_pAudioPlay->playAudio(pFrame->data[0], pFrame->nb_samples * pFrame->channels * 2);
            }
            else
            {
                usleep(1000000 * pFrame->nb_samples / m_nSampleRate);
            }
            
            av_frame_free(&pFrame);
        }
        else
        {
            // Fill the silence data
            
            uint8 buff[1024] = {0};
            m_pAudioPlay->playAudio(buff, 1024);
        }
    }
}

/* Video rendering thread */
void CVideoPlayer::videoPlayThread()
{
    int64 cur_delay = 0;
    int64 pre_delay = 0;
    uint32 cur_time = 0;
    uint32 pre_time = 0;
    AVFrame * pLastFrame = NULL;
    
    while (m_videoPlayFlag)
    {
        int size, fast = 0, slow = 0;
        AVFrame * pFrame = NULL;

        sys_os_mutex_enter(m_pVideoListMutex);
        size = m_videoFrameList.size();
        if (size > 0)
        {
            pFrame = m_videoFrameList.front();

            m_videoFrameList.pop_front();
        }
        else
        {
            pFrame = pLastFrame;
        }
        sys_os_mutex_leave(m_pVideoListMutex);

        if (pFrame)
        {
            // The Mac platform needs to initialize the SDL window on the UI main thread
            if (NULL == m_pVideoRender)
            {
                dispatch_sync(dispatch_get_main_queue(), ^{
                    initVideoRender(pFrame->width, pFrame->height);
                });
            }

            if (pFrame->format != m_nDstVideoFmt)
            {
                av_frame_free(&pFrame);
                continue;
            }

            // If there is audio, synchronize with audio
            if (m_nLastAudioPts != (uint64) AV_NOPTS_VALUE)
            {
                int diff = sys_os_get_ms() - m_lastAudioTS;
                uint64 vpts = pFrame->pts;
                uint64 epts = m_nLastAudioPts + diff * 1000;

                int ptsdiff = vpts - epts;

                // Video ahead of audio 200ms
                // If the video is 3 minutes ahead of the audio, the RTP timestamp may be incorrect, so skip it
                if (ptsdiff >= 200 * 1000 && ptsdiff <= 180 * 1000 * 1000)  
                {
                    fast = 1;
                }
                // Video is behind audio 200ms
                // If the video is 3 minutes behind the audio, the RTP timestamp may be inaccurate, so skip it
                else if (ptsdiff <= -200 * 1000 && ptsdiff >= -180 * 1000 *1000) 
                {
                    slow = 1;
                }
            }

            if (m_pVideoRender)
            {
                if (m_bSizeChanged)
                {
                    dispatch_sync(dispatch_get_main_queue(), ^{
                        m_pVideoRender->setWindowSize(m_size);
                        m_bSizeChanged = FALSE;
                    });
                }

                m_pVideoRender->render(pFrame, m_nRenderMode);
            }

            // Keep the last frame
            if (size > 1)
            {
                av_frame_free(&pFrame);
                av_frame_free(&pLastFrame);
                pLastFrame = NULL;
            }
            else if (pLastFrame != pFrame)
            {
                av_frame_free(&pLastFrame);

                pLastFrame = pFrame;
            }

            if (fast)
            {
                cur_delay = 1000/10.0 * 1000;   // The rendering fps is 10
            }
            else if (slow)
            {
                // don't sleep
                cur_delay = 0;
            }
            else
            {
                float fps = 30.0;
                
                if (m_pVideoDecoder)
                {
                    fps = m_pVideoDecoder->getFrameRate();
                    if (fps == 0)
                    {
                        fps = 30.0;
                    }
                }
                
                cur_delay = 1000/fps * 1000;   // The default rendering fps 
            }

            cur_time = sys_os_get_ms();

            if (pre_time > 0)
            {
                cur_delay += pre_delay - (cur_time - pre_time) * 1000;
                if (cur_delay < 1000)
                {
                    cur_delay = 0;
                }
            }

            pre_time = cur_time;
            pre_delay = cur_delay;

            if (cur_delay > 0)
            {
                usleep(cur_delay);
            }
        }
        else
        {
            usleep(1000);
        }
    }

    if (pLastFrame)
    {
        av_frame_free(&pLastFrame);
    }
}



