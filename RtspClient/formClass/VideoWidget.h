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

#ifndef _VIDEO_WIDGET_H_
#define _VIDEO_WIDGET_H_

#include <QWidget>
#include "ui_VideoWidget.h"
#include "FloatWidget.h"
#include "video_player.h"
#include <QTimer>
#ifdef BACKCHANNEL
#include "rtsp_cln.h"
#endif

/******************************************************************/
class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    VideoWidget(QWidget *parent = 0, Qt::WindowFlags flags = Qt::Widget);
    ~VideoWidget();

    bool    isSelected() {return m_bSelected;}
    void    setSelected(bool flag);

    QString getUrl() { return m_url; }
    QString getAcct() { return m_acct; }
    QString getPass() { return m_pass; }

    void    setMediaInfo(QString url, QString acct, QString pass);
    void    setRenderMode(int mode);
    void    onBackChannelNotify(int evt);

public slots:
    void    slotCloseVideo();
    void    slotResumeNormal();
    void    slotFillWhole();
    void    slotKeepAspectRatio();
    void    slotMediaInfo();
    void    slotWidgetSelecting();
    void    slotFullscreen();
    void    slotPlayerNotify(int event);
    void    slotReconn();
    void    slotFloatWidget();
    void    slotProgress();
    void    slotPlay();
    void    slotPause();
    void    slotStop();
    void    slotMicphone();
    void    slotSnapshot();
    void    slotRecord();
    void    slotMute();
    void    slotVolume();
    void    slotVolumeChanged(int);
    void    slotProgressChanged(int action);
    void    slotContextMenu(QPoint);
    void    slotSnapshoted(AVFrame * frame);
    void    slotSizeChanged(QSize);
    void    slotBackChannelNotify(int);

signals:
    void    widgetSelecting(QWidget *);
    void    backChannelNotify(int);

protected:
    void    paintEvent(QPaintEvent * event);
    void    resizeEvent(QResizeEvent * event);
    void    keyPressEvent(QKeyEvent * event); 
    void    mousePressEvent(QMouseEvent * event);
    bool    event(QEvent * event);

    void    fullScreen();
    void    resumeWidget();
    void    closePlayer();
    void    closeVideo();
    void    makeCall();
    void    setVolume();
    void    showFloatWidget(bool show);
    
private:
    void    initVideoWidget();
    void    initActions();    
    bool    canMaximized();
    void    showParent(bool show);
    void    startRecord();
    void    stopRecord();
    void    setMicphone(int flag);
    QString getBaseName(QString &url);
    void    stopBackChannel();
    void    startBackChannel();
    
private:
    Ui::VideoWidget ui;
    bool            m_bSelected;
    bool            m_bFullScreen;
    bool            m_bRecording;
    bool            m_bMute;
    QWidget       * m_pParent;
    QRect           m_rect;

    QString         m_url;
    QString         m_acct;
    QString         m_pass;
    QString         m_base;

    CVideoPlayer  * m_pPlayer;
    FloatWidget   * m_pFloatWidget;
    int             m_nRenderMode;

    QTimer          m_timerReconn;
    QTimer          m_timerFloatWidget;
    QTimer          m_timerProgress;

    QAction       * m_actionCloseVideo;
    QAction       * m_actionFullScreen;
    QAction       * m_actionResumeNormal;
    QAction       * m_actionFillWhole;
    QAction       * m_actionKeepAspectRatio;
    QAction       * m_actionMediaInfo;

#ifdef BACKCHANNEL
    int             m_nBackChannel;
    CRtspClient   * m_pRtspClient;
#endif
};

#endif


