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

#ifndef RTSP_PLAYER_H
#define RTSP_PLAYER_H

#include "video_player.h"
#include "rtsp_cln.h"


class CRtspPlayer : public CVideoPlayer
{
    Q_OBJECT

public:
    CRtspPlayer(QObject * parent = 0);
    virtual ~CRtspPlayer();

    BOOL    open(QString fileName, WId hWnd);
    void    close();
    BOOL    play();
    void    stop();
    BOOL    pause();
    BOOL    seek(int pos);
    int64   getElapse();
    int64   getDuration();
    int     getVideoClock();
    int     getAudioClock();
    int     getVideoCodec();
    int     getAudioCodec();
    
    void    setRtpMulticast(BOOL flag);
    void    setRtpOverUdp(BOOL flag);
    void    setMetadataChannel(BOOL flag);

#ifdef OVER_HTTP
    void    setRtspOverHttp(int flag, int port);
#endif

#ifdef OVER_WEBSOCKET
    void    setRtspOverWs(int flag, int port);
#endif

#ifdef BACKCHANNEL
    int     getBCFlag();
    void    setBCFlag(int flag);
    int     getBCDataFlag();
    void    setBCDataFlag(int flag);
    void    setAudioDevice(int index);
#endif

#ifdef REPLAY
    int     getReplayFlag();
    void    setReplayFlag(int flag);
    void    setScale(double scale);
    void    setRateControlFlag(int flag);
    void    setImmediateFlag(int flag);
    void    setFramesFlag(int flag, int interval);
    void    setReplayRange(time_t start, time_t end);
#endif

    void    onNotify(int evt);
    void    onAudio(uint8 * pdata, int len, uint32 ts, uint16 seq);
    void    onVideo(uint8 * pdata, int len, uint32 ts, uint16 seq);
    void    onRtcp(uint8 * pdata, int len, int type);
    BOOL    onRecord();
    
private:
    char        m_ip[32];
    int         m_port;
    CRtspClient m_rtsp;
};

#endif



