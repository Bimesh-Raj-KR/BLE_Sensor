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

#include "rtsp_player.h"
#include "utils.h"
#include "rtp.h"

/***************************************************************************************/

int rtsp_notify_cb(int evt, void * puser)
{
    CRtspPlayer * pPlayer = (CRtspPlayer *) puser;
    
    pPlayer->onNotify(evt);
    return 0;
}

int rtsp_audio_cb(uint8 * pdata, int len, uint32 ts, uint16 seq, void *puser)
{
    CRtspPlayer * pPlayer = (CRtspPlayer *) puser;

    pPlayer->onAudio(pdata, len, ts, seq);
    return 0;
}

int rtsp_video_cb(uint8 * pdata, int len, uint32 ts, uint16 seq, void *puser)
{
    CRtspPlayer * pPlayer = (CRtspPlayer *) puser;

    pPlayer->onVideo(pdata, len, ts, seq);
    return 0;
}

int rtsp_rtcp_cb(uint8 * pdata, int len, int type, void *puser)
{
    CRtspPlayer * pPlayer = (CRtspPlayer *) puser;

    pPlayer->onRtcp(pdata, len, type);
    return 0;
}

int rtsp_metadata_cb(uint8 * pdata, int len, uint32 ts, uint16 seq, void *puser)
{
    CRtspPlayer * pPlayer = (CRtspPlayer *) puser;
    
    return 0;
}


/***************************************************************************************/

CRtspPlayer::CRtspPlayer(QObject * parent)
: CVideoPlayer(parent)
, m_port(554)
{
    memset(m_ip, 0, sizeof(m_ip));
}

CRtspPlayer::~CRtspPlayer()
{
    close();
}

BOOL CRtspPlayer::open(QString fileName, WId hWnd)
{
    close();

    CVideoPlayer::open(fileName, hWnd);

    int port;
    char proto[32], username[100], password[100], host[100], path[200];

    url_split(fileName.toStdString().c_str(), proto, sizeof(proto), username, sizeof(username), 
        password, sizeof(password), host, sizeof(host), &port, path, sizeof(path));

    if (strcasecmp(proto, "rtsp") == 0)
    {
        port = (port == -1) ? 554 : port;
    }
#ifdef OVER_HTTP    
    else if (strcasecmp(proto, "http") == 0)
    {
        port = (port == -1) ? 80 : port;
    }
    else if (strcasecmp(proto, "https") == 0)
    {
        port = (port == -1) ? 443 : port;
    }
#endif
#ifdef OVER_WEBSOCKET    
    else if (strcasecmp(proto, "ws") == 0)
    {
        port = (port == -1) ? 80 : port;
    }
    else if (strcasecmp(proto, "wss") == 0)
    {
        port = (port == -1) ? 443 : port;
    }
#endif
#ifdef RTSPS
    else if (strcasecmp(proto, "rtsps") == 0)
    {
        port = (port == -1) ? 322 : port;
    }
#endif
    else
    {
        return FALSE;
    }

    if (host[0] == '\0')
    {
        return FALSE;
    }

    if (username[0] != '\0')
    {
        m_acct = username;
    }

    if (password[0] != '\0')
    {
        m_pass = password;
    }

    strncpy(m_ip, host, sizeof(m_ip) - 1);

    m_port = port;
    
    m_rtsp.set_notify_cb(rtsp_notify_cb, this);
    m_rtsp.set_audio_cb(rtsp_audio_cb);
    m_rtsp.set_video_cb(rtsp_video_cb);
    m_rtsp.set_rtcp_cb(rtsp_rtcp_cb);
#ifdef METADATA
    m_rtsp.set_metadata_cb(rtsp_metadata_cb);
#endif

    return TRUE;
}

void CRtspPlayer::close()
{
    // stop connection
    m_rtsp.rtsp_stop();
    m_rtsp.rtsp_close();

    m_bPlaying = FALSE;
    m_bPaused = FALSE;
    m_port = 554;

    memset(m_ip, 0, sizeof(m_ip));

    CVideoPlayer::close();
}

BOOL CRtspPlayer::play()
{
    if (m_rtsp.rtsp_start(m_sFileName.toStdString().c_str(), m_acct.toStdString().c_str(), m_pass.toStdString().c_str()))
    {
        m_bPlaying = TRUE;
    }

    return m_bPlaying;
}

void CRtspPlayer::stop()
{
    if (m_bPlaying || m_bPaused)
    {
        m_rtsp.rtsp_stop();
    }

    m_bPlaying = FALSE;
    m_bPaused = FALSE;
}

BOOL CRtspPlayer::pause()
{
    if (m_bPlaying)
    {
        if (m_rtsp.rtsp_pause())
        {
            m_bPaused = TRUE;
            m_bPlaying = FALSE;
        }
    }
    else if (m_bPaused)
    {
        if (m_rtsp.rtsp_play(0))
        {
            m_bPaused = FALSE;
            m_bPlaying = TRUE;
        }
    }

    return m_bPaused;
}

BOOL CRtspPlayer::seek(int pos)
{
    if (pos < 0 || pos > 100)
    {
        return FALSE;
    }

    int64 duration = getDuration();
    if (duration <= 0)
    {
        return FALSE;
    }

    int ntp_pos = duration / 100 * pos;
    
    m_rtsp.rtsp_play(ntp_pos);

    return TRUE;
}

int64 CRtspPlayer::getElapse()
{
    uint32 cur_ts;
    uint32 init_ts;
    int frequency;

    RCUA * p_rua = m_rtsp.get_rua();
    if (NULL == p_rua)
    {
        return 0;
    }
    
    if (m_videoClock.SyncTime.tv_sec > m_audioClock.SyncTime.tv_sec)
    {
        cur_ts = m_videoClock.SyncTimestamp;
        init_ts = p_rua->video_init_ts;
        frequency = getVideoClock();
    }
    else if (m_videoClock.SyncTime.tv_sec == m_audioClock.SyncTime.tv_sec && 
        m_videoClock.SyncTime.tv_usec > m_audioClock.SyncTime.tv_usec)
    {
        cur_ts = m_videoClock.SyncTimestamp;
        init_ts = p_rua->video_init_ts;
        frequency = getVideoClock();
    }
    else 
    {
        cur_ts = m_audioClock.SyncTimestamp;
        init_ts = p_rua->audio_init_ts;
        frequency = getAudioClock();
    }
    
    if (init_ts == (uint32)-1)
    {
        return 0;
    }

    int64 elapse;
    int timestampDiff = cur_ts - init_ts;

    // Divide this by the timestamp frequency to get real time:
    double timeDiff = timestampDiff / (double)frequency;

    if (timeDiff >= 0.0) 
    {
        elapse = p_rua->media_start + timeDiff * 1000;
    } 
    else 
    {
        timeDiff = -timeDiff;
        elapse = p_rua->media_start + timeDiff * 1000;
    }
    
    return elapse;
}

int64 CRtspPlayer::getDuration()
{
    RCUA * p_rua = m_rtsp.get_rua();
    if (p_rua)
    {
        return p_rua->play_end - p_rua->play_start;
    }

    return 0;
}

int CRtspPlayer::getVideoClock()
{
    return 90000;
}

int CRtspPlayer::getAudioClock()
{
    return m_nSampleRate;
}

int CRtspPlayer::getVideoCodec()
{
    return m_rtsp.video_codec();
}

int CRtspPlayer::getAudioCodec()
{
    return m_rtsp.audio_codec();
}

void CRtspPlayer::setRtpMulticast(BOOL flag)
{
    m_rtsp.set_rtp_multicast(flag);
}

void CRtspPlayer::setRtpOverUdp(BOOL flag)
{
    m_rtsp.set_rtp_over_udp(flag);
}

void CRtspPlayer::setMetadataChannel(BOOL flag)
{
    m_rtsp.set_channel(AV_METADATA_CH, flag);
}

#ifdef BACKCHANNEL

int CRtspPlayer::getBCFlag()
{
    return m_rtsp.get_bc_flag();
}

void CRtspPlayer::setBCFlag(int flag)
{
    m_rtsp.set_bc_flag(flag);
}

int CRtspPlayer::getBCDataFlag()
{
    return m_rtsp.get_bc_data_flag();
}

void CRtspPlayer::setBCDataFlag(int flag)
{
    if (m_rtsp.get_bc_data_flag())
    {
        m_rtsp.set_bc_data_flag(0);
    }
    else
    {
        m_rtsp.set_bc_data_flag(1);
    }
}

void CRtspPlayer::setAudioDevice(int index)
{
    m_rtsp.set_audio_device(index);
}

#endif // end of BACKCHANNEL

#ifdef REPLAY

int CRtspPlayer::getReplayFlag()
{
    return m_rtsp.get_replay_flag();
}

void CRtspPlayer::setReplayFlag(int flag)
{
    m_rtsp.set_replay_flag(flag);
}

void CRtspPlayer::setScale(double scale)
{
    m_rtsp.set_scale(scale);
}

void CRtspPlayer::setRateControlFlag(int flag)
{
    m_rtsp.set_rate_control_flag(flag);
}

void CRtspPlayer::setImmediateFlag(int flag)
{
    m_rtsp.set_immediate_flag(flag);
}

void CRtspPlayer::setFramesFlag(int flag, int interval)
{
    m_rtsp.set_frames_flag(flag, interval);
}

void CRtspPlayer::setReplayRange(time_t start, time_t end)
{
    m_rtsp.set_replay_range(start, end);
}

#endif // end of REPLAY

#ifdef OVER_HTTP

void CRtspPlayer::setRtspOverHttp(int flag, int port)
{
    m_rtsp.set_rtsp_over_http(flag, port);
}

#endif // OVER_HTTP

#ifdef OVER_WEBSOCKET

void CRtspPlayer::setRtspOverWs(int flag, int port)
{
    m_rtsp.set_rtsp_over_ws(flag, port);
}

#endif // OVER_WEBSOCKET

void CRtspPlayer::onNotify(int evt)
{
    if (evt == RTSP_EVE_CONNSUCC)
    {
        int videoCodec = m_rtsp.video_codec();
        int audioCodec = m_rtsp.audio_codec();

        if (VIDEO_CODEC_NONE != videoCodec)
        {
            openVideo(videoCodec);
        }

        if (AUDIO_CODEC_NONE != audioCodec)
        {
            openAudio(audioCodec, m_rtsp.get_audio_samplerate(), m_rtsp.get_audio_channels(), m_rtsp.get_audio_bitpersample());
        }
    }

    emit notify(evt);
}

void CRtspPlayer::onAudio(uint8 * pdata, int len, uint32 ts, uint16 seq)
{
    playAudio(pdata, len, ts, seq);
}

void CRtspPlayer::onVideo(uint8 * pdata, int len, uint32 ts, uint16 seq)
{
    playVideo(pdata, len, ts, seq);
}

void CRtspPlayer::onRtcp(uint8 * pdata, int len, int type)
{
    uint8 * p_rtp = pdata;
    uint32  rtp_len = len;

    // Check for the 20-byte RTCP header:
    if (rtp_len < 20)
    {
        return;
    }

    uint8 pt = p_rtp[1];
    uint32 rtpHdr = ntohl(*(uint32*)p_rtp); p_rtp += 4; rtp_len -= 4;
    uint32 ssrc = ntohl(*(uint32*)(p_rtp)); p_rtp += 4; rtp_len -= 4;
    uint32 ntpMSW = ntohl(*(uint32*)(p_rtp)); p_rtp += 4; rtp_len -= 4;
    uint32 ntpLSW = ntohl(*(uint32*)(p_rtp)); p_rtp += 4; rtp_len -= 4;
    uint32 rtpTimestamp = ntohl(*(uint32*)(p_rtp));

    if (pt != RTCP_SR)
    {
        return;
    }

    // Check the RTP version number (it should be 2)
    if ((rtpHdr & 0xC0000000) != 0x80000000)
    {
        return;
    }

    if (AV_TYPE_AUDIO == type)
    {
        m_audioClock.SyncTimestamp = rtpTimestamp;
        m_audioClock.SyncTime.tv_sec = ntpMSW - 0x83AA7E80; // 1/1/1900 -> 1/1/1970
        double microseconds = (ntpLSW * 15625.0) / 0x04000000; // 10^6/2^32
        m_audioClock.SyncTime.tv_usec = (unsigned)(microseconds+0.5);
    }
    else if (AV_TYPE_VIDEO == type)
    {
        m_videoClock.SyncTimestamp = rtpTimestamp;
        m_videoClock.SyncTime.tv_sec = ntpMSW - 0x83AA7E80; // 1/1/1900 -> 1/1/1970
        double microseconds = (ntpLSW * 15625.0) / 0x04000000; // 10^6/2^32
        m_videoClock.SyncTime.tv_usec = (unsigned)(microseconds+0.5);
    }
}

BOOL CRtspPlayer::onRecord()
{
    CRtspClient * p_rtsp = &m_rtsp;
    AVICTX * p_avictx = m_pAviCtx;
    
    int vcodec = p_rtsp->video_codec();    
    int v_extra_len = 0;
    uint8 v_extra[1024];
    
    if (VIDEO_CODEC_H264 == vcodec)
    {
        avi_set_video_info(p_avictx, 0, 0, 0, "H264");

        uint8 sps[512], pps[512];
        int sps_len = 0, pps_len = 0;
        
        if (p_rtsp->get_h264_params(sps, &sps_len, pps, &pps_len))
        {
            memcpy(v_extra, sps, sps_len);
            memcpy(v_extra + sps_len, pps, pps_len);

            v_extra_len = sps_len + pps_len;
        }

        if (v_extra_len > 0)
        {
            avi_set_video_extra_info(p_avictx, v_extra, v_extra_len);
        }
    }
    else if (VIDEO_CODEC_H265 == vcodec)
    {
        avi_set_video_info(p_avictx, 0, 0, 0, "H265");

        uint8 vps[512], sps[512], pps[512];
        int vps_len = 0, sps_len = 0, pps_len = 0;
            
        if (p_rtsp->get_h265_params(sps, &sps_len, pps, &pps_len, vps, &vps_len))
        {
            memcpy(v_extra, vps, vps_len);
            memcpy(v_extra + vps_len, sps, sps_len);
            memcpy(v_extra + vps_len + sps_len, pps, pps_len);

            v_extra_len = vps_len + sps_len + pps_len;
        }

        if (v_extra_len > 0)
        {
            avi_set_video_extra_info(p_avictx, v_extra, v_extra_len);
        }
    }
    else if (VIDEO_CODEC_JPEG == vcodec)
    {
        avi_set_video_info(p_avictx, 0, 0, 0, "JPEG");
    }
    else if (VIDEO_CODEC_MP4 == vcodec)
    {
        avi_set_video_info(p_avictx, 0, 0, 0, "MP4V");
    }

    int acodec = p_rtsp->audio_codec();
    int sr = p_rtsp->get_audio_samplerate();
    int ch = p_rtsp->get_audio_channels();
        
    if (AUDIO_CODEC_G711A == acodec)
    {
        avi_set_audio_info(p_avictx, ch, sr, AUDIO_FORMAT_ALAW);
    }
    else if (AUDIO_CODEC_G711U == acodec)
    {
        avi_set_audio_info(p_avictx, ch, sr, AUDIO_FORMAT_MULAW);
    }
    else if (AUDIO_CODEC_G726 == acodec)
    {
        avi_set_audio_info(p_avictx, ch, sr, AUDIO_FORMAT_G726);
    }
    else if (AUDIO_CODEC_G722 == acodec)
    {
        avi_set_audio_info(p_avictx, ch, sr, AUDIO_FORMAT_G722);
    }
    else if (AUDIO_CODEC_AAC == acodec)
    {
        avi_set_audio_info(p_avictx, ch, sr, AUDIO_FORMAT_AAC);
        avi_set_audio_extra_info(p_avictx, p_rtsp->get_audio_config(), p_rtsp->get_audio_config_len());
    }

    avi_update_header(p_avictx);

    if (p_avictx->ctxf_video)
    {
        if (v_extra_len > 0)
        {
            recordVideo(v_extra, v_extra_len, 0, 0);
        }
    }
    
    return TRUE;
}




