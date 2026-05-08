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

#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include "sys_inc.h"
#include "hqueue.h"
#include "video_decoder.h"
#include "audio_decoder.h"
#include "video_render.h"
#include "audio_play.h"
#include "avi_write.h"
#include "media_util.h"
#include <QObject>
#include <QWidget>
#include <list>

typedef struct
{
    uint32          SyncTimestamp;
    struct timeval  SyncTime;
} HTCLOCK;

typedef std::list<AVFrame*> FRAMELIST;

class CVideoPlayer : public QObject
{
    Q_OBJECT

public:
    CVideoPlayer(QObject * parent = 0);
    virtual ~CVideoPlayer();

    virtual BOOL    open(QString fileName, WId hWnd);    
    virtual BOOL    play() = 0;
    virtual void    stop() = 0;
    virtual BOOL    pause() = 0;
    virtual void    close();
    virtual BOOL    seek(int pos) { return FALSE; }
    virtual BOOL    isPlaying() {return m_bPlaying;}
    virtual BOOL    isPaused() {return m_bPaused;}
    virtual void    setVolume(int volume);
    virtual void    snapshot(int videofmt);
    virtual BOOL    record(QString baseName);
    virtual void    stopRecord();
    virtual BOOL    isRecording() {return m_bRecording;}
    virtual BOOL    onRecord() {return FALSE;}
    virtual int     getVideoClock() {return 1000;}
    virtual int     getAudioClock() {return 1000;}
    virtual void    setWindowSize(QSize size);
    virtual int64   getElapse() {return 0;}
    virtual int64   getDuration() {return 0;}
    virtual int     getVideoCodec() {return m_nVideoCodec;}
    virtual int     getAudioCodec() {return m_nAudioCodec;}
    
    virtual void    setAuthInfo(QString acct, QString pass) {m_acct=acct; m_pass=pass;}
    virtual void    setRenderMode(int mode) {m_nRenderMode=mode;}
    virtual void    setHWDecoding(int mode) {m_nHWDecoding=mode;}
    virtual void    setRtpMulticast(BOOL flag) {}
    virtual void    setRtpOverUdp(BOOL flag) {}
    virtual void    setMetadataChannel(BOOL flag) {}

#ifdef OVER_HTTP
    virtual void    setRtspOverHttp(int flag, int port) {}
#endif

#ifdef OVER_WEBSOCKET
    virtual void    setRtspOverWs(int flag, int port) {}
#endif

#ifdef BACKCHANNEL
    virtual int     getBCFlag() {return 0;}
    virtual void    setBCFlag(int flag) {}
    virtual int     getBCDataFlag() {return 0;}
    virtual void    setBCDataFlag(int flag) {}
    virtual void    setAudioDevice(int index) {}
#endif

#ifdef REPLAY
    virtual int     getReplayFlag() {return 0;}
    virtual void    setReplayFlag(int flag) {}
    virtual void    setScale(double scale) {}
    virtual void    setRateControlFlag(int flag) {}
    virtual void    setImmediateFlag(int flag) {}
    virtual void    setFramesFlag(int flag, int interval) {}
    virtual void    setReplayRange(time_t start, time_t end) {}
#endif

    BOOL            openVideo(int codec, uint8 * extradata = NULL, int extradata_size = 0);
    BOOL            openVideo(enum AVCodecID codec, uint8 * extradata = NULL, int extradata_size = 0);
    void            closeVideo();
    BOOL            openAudio(int codec, int samplerate, int channels, int bitpersample = 0);
    BOOL            openAudio(enum AVCodecID codec, int samplerate, int channels, int bitpersample = 0);
    void            closeAudio();

    void            playVideo(uint8 * data, int len, uint32 ts, uint16 seq);
    void            playAudio(uint8 * data, int len, uint32 ts, uint16 seq);

    void            recordVideo(uint8 * data, int len, uint32 ts, uint16 seq);
    void            recordAudio(uint8 * data, int len, uint32 ts, uint16 seq);

    int             getVideoWidth();
    int             getVideoHeight();
    double          getFrameRate();
    int             getSampleRate() {return m_nSampleRate;}
    int             getChannel() {return m_nChannel;}
    
    void            onVideoFrame(AVFrame * frame);
    void            onAudioFrame(AVFrame * frame);
    void            audioPlayThread();
    void            videoPlayThread();
    
signals:
    void            notify(int);
    void            snapshoted(AVFrame *);

protected:
    void            updateClock(HTCLOCK * clock, uint32 ts, int frequency);
    BOOL            initFrame(AVFrame *& frame, int width, int height, AVPixelFormat pixfmt);
    BOOL            initVideoRender(int width, int height);
    BOOL            doSnapshot(AVFrame * srcframe);
    AVFrame *       convertFrame(AVFrame * srcframe, AVFrame * dstframe, BOOL updown);
    void            recordVideoEx(uint8 * data, int len, uint32 ts, uint16 seq);
    BOOL            recordSwitchCheck();
    void            recordFileSwitch();
    
protected:
    BOOL            m_bVideoInited;
    BOOL            m_bAudioInited;
    CVideoDecoder * m_pVideoDecoder;
    CAudioDecoder * m_pAudioDecoder;
    CVideoRender  * m_pVideoRender;
    CAudioPlay    * m_pAudioPlay;

    BOOL            m_bPlaying;
    BOOL            m_bPaused;
    
    QString         m_acct;
    QString         m_pass;
    QString         m_sFileName;
    QString         m_sBaseName;
    WId             m_nVideoWnd;
    QSize           m_size;
    BOOL            m_bSizeChanged;
    int             m_nRenderMode;
    int             m_nHWDecoding;
    AVPixelFormat   m_nDstVideoFmt;
    BOOL            m_bUpdown;
    BOOL            m_bSnapshot;
    int             m_nSnapVideoFmt;

    H26XParamSets   m_h26XParamSets;
    
    int             m_nVideoCodec;
    int             m_nAudioCodec;
    int             m_nSampleRate;
    int             m_nChannel;
    int             m_nBitPerSample;
    
    AVFrame       * m_pSnapFrame;
    AVFrame       * m_pRenderFrame;

    BOOL            m_bRecording;
    BOOL            m_bNalFlag;
    AVICTX        * m_pAviCtx;
    void          * m_pRecordMutex;

    HTCLOCK         m_audioClock;
    void          * m_pAudioListMutex;
    FRAMELIST       m_audioFrameList;
    BOOL            m_audioPlayFlag;
    pthread_t       m_audioPlayThread;

    HTCLOCK         m_videoClock;
    void          * m_pVideoListMutex;
    FRAMELIST       m_videoFrameList;
    BOOL            m_videoPlayFlag;
    pthread_t       m_videoPlayThread;

    uint64          m_nLastAudioPts;
    time_t          m_lastAudioTS;
};

#endif // end of VIDEO_PLAYER_H



