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
#include "VideoWidget.h"
#include "rtsp_player.h"
#include "utils.h"
#include "SetRtspInfo.h"
#include "MediaInfo.h"
#include <QMenu>
#include <QPainter>
#include <QMimeData>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QMessageBox>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QtGlobal>

#ifdef BACKCHANNEL
#if __WINDOWS_OS__
#include "audio_capture_win.h"
#elif defined(IOS)
#include "audio_capture_mac.h"
#elif defined(ANDROID)
#include "audio_capture_android.h"
#elif __LINUX_OS__
#include "audio_capture_linux.h"
#endif
#endif

/**************************************************************************************/

int backChannelNotify(int evt, void * userdata)
{
    VideoWidget * pthis = (VideoWidget *) userdata;

    pthis->onBackChannelNotify(evt);
    return 0;
}

/**************************************************************************************/

VideoWidget::VideoWidget(QWidget *parent, Qt::WindowFlags flags)
: QWidget(parent, flags)
, m_bSelected(false)
, m_bFullScreen(false)
, m_bRecording(false)
, m_bMute(false)
, m_pParent(NULL)
, m_pPlayer(NULL)
, m_pFloatWidget(NULL)
, m_nRenderMode(RENDER_MODE_KEEP)
#ifdef BACKCHANNEL
, m_nBackChannel(0)
, m_pRtspClient(NULL)
#endif
{
    ui.setupUi(this);

    initVideoWidget();
    initActions();
}


VideoWidget::~VideoWidget()
{
    m_timerFloatWidget.stop();
    
    if (m_pFloatWidget)
    {
        delete m_pFloatWidget;
        m_pFloatWidget = NULL;
    }

    closeVideo();
}

void VideoWidget::initVideoWidget()
{
    setMouseTracking(true);
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(false);

    m_timerReconn.setSingleShot(true);

    m_timerFloatWidget.setParent(this);
    m_timerFloatWidget.start(1000);
    
    connect(&m_timerReconn, SIGNAL(timeout()), this, SLOT(slotReconn()));
    connect(&m_timerFloatWidget, SIGNAL(timeout()), this, SLOT(slotFloatWidget()));
    connect(&m_timerProgress, SIGNAL(timeout()), this, SLOT(slotProgress()));

    connect(ui.widgetVideo, SIGNAL(widgetSelecting()), this, SLOT(slotWidgetSelecting()));
    connect(ui.widgetVideo, SIGNAL(doubleClicked()), this, SLOT(slotFullscreen()));
    connect(ui.widgetVideo, SIGNAL(contextMenu(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    connect(ui.widgetVideo, SIGNAL(sizeChanged(QSize)), this, SLOT(slotSizeChanged(QSize)));

    connect(this, SIGNAL(backChannelNotify(int)), this, SLOT(slotBackChannelNotify(int)), Qt::QueuedConnection);
}

void VideoWidget::initActions()
{
    m_actionCloseVideo = new QAction(this);
    m_actionCloseVideo->setText(tr("Close Video"));
    m_actionCloseVideo->setStatusTip(tr("Close Video"));

    m_actionFullScreen = new QAction(this);
    m_actionFullScreen->setText(tr("Full Screen"));
    m_actionFullScreen->setStatusTip(tr("Full Screen"));

    m_actionResumeNormal = new QAction(this);
    m_actionResumeNormal->setText(tr("Exit Full Screen"));
    m_actionResumeNormal->setStatusTip(tr("Exit Full Screen"));

    m_actionFillWhole = new QAction(this);
    m_actionFillWhole->setText(tr("Fill the whole window"));
    m_actionFillWhole->setStatusTip(tr("Fill the whole window"));

    m_actionKeepAspectRatio = new QAction(this);
    m_actionKeepAspectRatio->setText(tr("Keep the original aspect ratio"));
    m_actionKeepAspectRatio->setStatusTip(tr("Keep the original aspect ratio"));

    m_actionMediaInfo = new QAction(this);
    m_actionMediaInfo->setText(tr("Media information ..."));
    m_actionMediaInfo->setStatusTip(tr("Media information"));
    
    connect(m_actionCloseVideo, SIGNAL(triggered()), this, SLOT(slotCloseVideo()));
    connect(m_actionFullScreen, SIGNAL(triggered()), this, SLOT(slotFullscreen()));
    connect(m_actionResumeNormal, SIGNAL(triggered()), this, SLOT(slotResumeNormal()));
    connect(m_actionFillWhole, SIGNAL(triggered()), this, SLOT(slotFillWhole()));
    connect(m_actionKeepAspectRatio, SIGNAL(triggered()), this, SLOT(slotKeepAspectRatio()));
    connect(m_actionMediaInfo, SIGNAL(triggered()), this, SLOT(slotMediaInfo()));
}

void VideoWidget::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);

    if (m_bSelected && !m_bFullScreen)
    {
        painter.fillRect(rect(), QBrush(QColor(170, 85, 255)));
    }
    else
    {
        painter.fillRect(rect(), QBrush(QColor(20, 20, 20)));
    }
}

void VideoWidget::resizeEvent(QResizeEvent * event)
{
    if (m_pFloatWidget)
    {
        QRect rect = ui.widgetVideo->geometry();
        QPoint pt = mapToGlobal(rect.topLeft());

        m_pFloatWidget->setGeometry(pt.x(), pt.y(), ui.widgetVideo->width(), ui.widgetVideo->height());
    }
}

void VideoWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "VideoWidget::mousePressEvent";

    slotWidgetSelecting();
}

void VideoWidget::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if (m_bFullScreen)
        {
            resumeWidget();
        }
    }
}

bool VideoWidget::canMaximized()
{
    // The parent window is full screen, it cannot be full screen

    QObject * pParent = parent();
    while (pParent)
    {
        if (pParent->isWidgetType())
        {
            if (pParent->property("isTopWindow").toInt())
            {
                break;
            }

            QWidget * pParentWidget = (QWidget *)pParent;

            if (pParentWidget->isMaximized())
            {
                return false;
            }
        }

        pParent = pParent->parent();
    }

    return true;
}

void VideoWidget::showParent(bool show)
{
    // show parent widget
    QWidget * pParentWidget;
    QObject * pParent = parent();
    while (pParent)
    {
        if (pParent->isWidgetType())
        {
            pParentWidget = (QWidget *)pParent;

            if (show)
                pParentWidget->show();
            else
                pParentWidget->hide();
        }

        pParent = pParent->parent();
    }
}

void VideoWidget::fullScreen()
{
    if (!canMaximized())
    {
        return;
    }

    // hide parent widget
    showParent(false);
    
    m_rect = geometry();
    m_pParent = (QWidget *)parent();
    setParent(NULL);
    setWindowFlags(Qt::FramelessWindowHint);
    
    showMaximized();

    if (m_pFloatWidget)
    {
        m_pFloatWidget->hide();
    }

    grabKeyboard();

    m_bFullScreen = true;
}

void VideoWidget::resumeWidget()
{
    setParent(m_pParent);

    // show parent widget
    showParent(true);
    
    showNormal();
    
    setGeometry(m_rect);

    if (m_pFloatWidget)
    {
        m_pFloatWidget->hide();
    }

    releaseKeyboard();

    m_bFullScreen = false;
}

void VideoWidget::slotBackChannelNotify(int evt)
{
#ifdef BACKCHANNEL
    if (evt == RTSP_EVE_CONNFAIL || 
        evt == RTSP_EVE_STOPPED || 
        evt == RTSP_EVE_AUTHFAILED)
    {
        if (m_nBackChannel)
        {
            m_nBackChannel = 0;
            setMicphone(0);
        }
        
        stopBackChannel();
    }
#endif
}

void VideoWidget::onBackChannelNotify(int evt)
{
    emit backChannelNotify(evt);
}

void VideoWidget::stopBackChannel()
{
#ifdef BACKCHANNEL
    if (m_pRtspClient)
    {
        m_pRtspClient->rtsp_stop();
        m_pRtspClient->rtsp_close();
    
        delete m_pRtspClient;
        m_pRtspClient = NULL;
    }
#endif
}

void VideoWidget::startBackChannel()
{
#ifdef BACKCHANNEL
    int index = 0;
    m_pRtspClient = new CRtspClient;

    m_pRtspClient->set_notify_cb(::backChannelNotify, this);
    
#if __WINDOWS_OS__
    index = CWAudioCapture::getDeviceIndex(getAudioDevice().toLocal8Bit());
#elif defined(IOS)
    index = CMAudioCapture::getDeviceIndex(getAudioDevice().toLocal8Bit());
#elif defined(ANDROID)
    index = CAAudioCapture::getDeviceIndex(getAudioDevice().toLocal8Bit());
#elif __LINUX_OS__
    index = CLAudioCapture::getDeviceIndex(getAudioDevice().toLocal8Bit());
#endif
    m_pRtspClient->set_audio_device(index);

    m_pRtspClient->set_bc_flag(1);         // enable audio backchannel
    m_pRtspClient->set_bc_data_flag(1);    // enable send audio data

    m_pRtspClient->set_rtp_over_udp(getRtpOverUdp());
    
#ifdef OVER_HTTP
    m_pRtspClient->set_rtsp_over_http(getRtspOverHttp(), getRtspOverHttpPort()); // rtsp over http
#endif
#ifdef OVER_WEBSOCKET
    m_pRtspClient->set_rtsp_over_ws(getRtspOverWs(), getRtspOverWsPort());  // rtsp over websocket
#endif
    m_pRtspClient->set_channel(AV_VIDEO_CH, 0);     // not setup video channel
    m_pRtspClient->set_channel(AV_AUDIO_CH, 0);     // not setup audio channel
    m_pRtspClient->set_channel(AV_METADATA_CH, 0);  // not setup metadata channel

    m_pRtspClient->rtsp_start(m_url.toStdString().c_str(), 
                              m_acct.toStdString().c_str(), 
                              m_pass.toStdString().c_str());
#endif
}

void VideoWidget::closePlayer()
{
    m_timerReconn.stop();
    m_timerProgress.stop();

    if (m_pPlayer)
    {
        stopRecord();

        delete m_pPlayer;
        m_pPlayer = NULL;
    }

#ifdef BACKCHANNEL
    if (m_nBackChannel)
    {
        m_nBackChannel = 0;
        setMicphone(0);
    }
    
    stopBackChannel();
#endif
}

void VideoWidget::closeVideo()
{
    closePlayer();

    m_url = "";
    m_acct = "";
    m_pass = "";
    m_bRecording = false;

    ui.widgetVideo->setTips("");

    if (m_pFloatWidget)
    {
        m_pFloatWidget->ui.btnPause->setIcon(QIcon(QString::fromUtf8(":/res/Resources/btn_pause.png")));
        m_pFloatWidget->ui.btnPause->setToolTip(tr("Pause"));

        m_pFloatWidget->setElapse(0);
        m_pFloatWidget->setDuration(0);
    }
}

void VideoWidget::slotCloseVideo()
{
    closeVideo();
}

void VideoWidget::slotResumeNormal()
{
    resumeWidget();
}

void VideoWidget::slotFillWhole()
{
    m_nRenderMode = RENDER_MODE_FILL;

    if (m_pPlayer)
    {
        m_pPlayer->setRenderMode(m_nRenderMode);
    }
}

void VideoWidget::slotKeepAspectRatio()
{
    m_nRenderMode = RENDER_MODE_KEEP;

    if (m_pPlayer)
    {
        m_pPlayer->setRenderMode(m_nRenderMode);
    }
}

void VideoWidget::slotMediaInfo()
{
    MediaInfo dlg;

    if (m_pPlayer)
    {
        dlg.setUrl(m_url);
        dlg.setVideoInfo(m_pPlayer->getVideoCodec(), m_pPlayer->getVideoWidth(), 
            m_pPlayer->getVideoHeight(), m_pPlayer->getFrameRate());
        dlg.setAudioInfo(m_pPlayer->getAudioCodec(), m_pPlayer->getSampleRate(),
            m_pPlayer->getChannel());
    }

    dlg.exec();
}

void VideoWidget::setSelected(bool flag)
{
    m_bSelected = flag;

    update();
}

void VideoWidget::setMediaInfo(QString url, QString acct, QString pass)
{
    if (m_url == url && m_acct == acct && m_pass == pass)
    {
        return;
    }

    closeVideo();

    m_url = url;
    m_acct = acct;
    m_pass = pass;
    m_base = getBaseName(url);

    if (!m_url.isEmpty())
    {
        makeCall();
    }
}

void VideoWidget::setRenderMode(int mode)
{
    m_nRenderMode = mode;
}

void VideoWidget::slotWidgetSelecting()
{
    if (!m_bSelected)
    {
        emit widgetSelecting(this);
    }
}

void VideoWidget::slotFullscreen()
{
    if (m_bFullScreen)
    {
        resumeWidget();
    }
    else
    {
        fullScreen();
    }
}

void VideoWidget::makeCall()
{
    if (m_url.isEmpty())
    {
        return;
    }
    
    m_pPlayer = new CRtspPlayer(this);
    if (NULL == m_pPlayer)
    {
        return;        
    }

    connect(m_pPlayer, SIGNAL(notify(int)), this, SLOT(slotPlayerNotify(int)), Qt::QueuedConnection);
    connect(m_pPlayer, SIGNAL(snapshoted(AVFrame*)), this, SLOT(slotSnapshoted(AVFrame*)), Qt::QueuedConnection);

    if (m_pPlayer->open(m_url, ui.widgetVideo->winId()))
    {
        m_pPlayer->setAuthInfo(m_acct, m_pass);
        m_pPlayer->setRenderMode(m_nRenderMode);
        m_pPlayer->setHWDecoding(getHWDecoding());
        m_pPlayer->setRtpOverUdp(getRtpOverUdp());
        m_pPlayer->setRtpMulticast(getRtpMulticast());
        m_pPlayer->setMetadataChannel(getMetadataChannel());

#ifdef OVER_HTTP
        m_pPlayer->setRtspOverHttp(getRtspOverHttp(), getRtspOverHttpPort());
#endif
#ifdef OVER_WEBSOCKET
        m_pPlayer->setRtspOverWs(getRtspOverWs(), getRtspOverWsPort());
#endif

        m_timerProgress.start(1000);
        
        m_pPlayer->play();
    }
    else
    {
        closePlayer();

        ui.widgetVideo->setTips("Invalid rtsp url");
    }
}

void VideoWidget::setVolume()
{
    if (m_pFloatWidget && m_pPlayer)
    {
        if (m_pFloatWidget->ui.btnVolume->property("mute").toBool())
        {
            m_pPlayer->setVolume(HTVOLUME_MIN);
        }
        else
        {
            m_pPlayer->setVolume(m_pFloatWidget->ui.sliderVolume->value());
        }
    }
}

void VideoWidget::showFloatWidget(bool show)
{
    if (show)
    {
        if (NULL == m_pFloatWidget)
        {
            m_pFloatWidget = new FloatWidget(this);

            if (m_bMute)
            {
                m_pFloatWidget->ui.btnVolume->setProperty("mute", true);
                m_pFloatWidget->ui.btnVolume->setIcon(QIcon(QString::fromUtf8(":/res/Resources/mute.png")));
            }

#ifndef BACKCHANNEL
            m_pFloatWidget->ui.btnMic->hide();
#endif

            QObject::connect(m_pFloatWidget, SIGNAL(play()), this, SLOT(slotPlay()));
            QObject::connect(m_pFloatWidget, SIGNAL(pause()), this, SLOT(slotPause()));
            QObject::connect(m_pFloatWidget, SIGNAL(stop()), this, SLOT(slotStop()));
            QObject::connect(m_pFloatWidget, SIGNAL(micphone()), this, SLOT(slotMicphone()));
            QObject::connect(m_pFloatWidget, SIGNAL(snapshot()), this, SLOT(slotSnapshot()));
            QObject::connect(m_pFloatWidget, SIGNAL(record()), this, SLOT(slotRecord()));
            QObject::connect(m_pFloatWidget, SIGNAL(volume()), this, SLOT(slotVolume()));
            QObject::connect(m_pFloatWidget, SIGNAL(volumeChanged(int)), this, SLOT(slotVolumeChanged(int)));

            QObject::connect(m_pFloatWidget, SIGNAL(widgetSelecting()), this, SLOT(slotWidgetSelecting()));
            QObject::connect(m_pFloatWidget, SIGNAL(doubleClicked()), this, SLOT(slotFullscreen()));
            QObject::connect(m_pFloatWidget, SIGNAL(contextMenu(QPoint)), this, SLOT(slotContextMenu(QPoint)));
            QObject::connect(m_pFloatWidget, SIGNAL(progressChanged(int)), this, SLOT(slotProgressChanged(int)));
        }

        QRect rect = ui.widgetVideo->geometry();
        QPoint pt = mapToGlobal(rect.topLeft());

        m_pFloatWidget->setGeometry(pt.x(), pt.y(), ui.widgetVideo->width(), ui.widgetVideo->height());

        if (m_pPlayer)
        {
            m_pFloatWidget->setDuration(m_pPlayer->getDuration());
        }

        m_pFloatWidget->showProgress(1);
        m_pFloatWidget->show();
    }
    else if (m_pFloatWidget)
    {
        QRect rect = m_pFloatWidget->geometry();

        if (!rect.contains(QCursor::pos()))
        {
            m_pFloatWidget->hide();
        }
    }
}

void VideoWidget::slotPlayerNotify(int event)
{
    QString tips;
    
    if (event == RTSP_EVE_CONNECTING)
    {
        tips = tr("Connecting");
    }
    else if (event == RTSP_EVE_CONNFAIL)
    {
        tips = tr("Connect failed");

        m_timerReconn.start(5 * 1000);
    }
    else if (event == RTSP_EVE_CONNSUCC)
    {
#ifdef IOS
        tips = m_url;
#else
        tips = "";
#endif

        setVolume();

        // Re-record after reconnect 
        if (m_bRecording)
        {
            startRecord();
        }
    }
    else if (event == RTSP_EVE_NOSIGNAL)
    {
        tips = tr("NO Signal");

        m_timerReconn.start(5 * 1000);
    }
    else if (event == RTSP_EVE_NODATA)
    {
        tips = tr("NO Data");

        m_timerReconn.start(5 * 1000);
    }
    else if (event == RTSP_EVE_RESUME)
    {
#ifdef IOS
        tips = m_url;
#else
        tips = "";
#endif

        m_timerReconn.stop();
    }
    else if (event == RTSP_EVE_STOPPED)
    {
#ifdef BACKCHANNEL
        if (m_nBackChannel)
        {
            m_nBackChannel = 0;
            setMicphone(0);
        }
        
        stopBackChannel();
#endif

        m_timerReconn.start(5 * 1000);

        return;
    }
    else if (event == RTSP_EVE_AUTHFAILED)
    {
        tips = tr("Authenticate failed");
    }

    ui.widgetVideo->setTips(tips);
}

void VideoWidget::slotReconn()
{
    closePlayer();

    makeCall(); 
}

void VideoWidget::slotPlay()
{
    SetRtspInfo dlg(m_url, m_acct, m_pass, NULL, Qt::WindowCloseButtonHint);

    if (QDialog::Accepted == dlg.exec())
    {
        setMediaInfo(dlg.getUrl(), dlg.getUser(), dlg.getPass());
    }
}

void VideoWidget::slotPause()
{
    if (m_pPlayer)
    {
        if (m_pPlayer->pause())
        {
            m_pFloatWidget->ui.btnPause->setIcon(QIcon(QString::fromUtf8(":/res/Resources/btn_play.png")));
            m_pFloatWidget->ui.btnPause->setToolTip(tr("Play"));
        }
        else
        {
            m_pFloatWidget->ui.btnPause->setIcon(QIcon(QString::fromUtf8(":/res/Resources/btn_pause.png")));
            m_pFloatWidget->ui.btnPause->setToolTip(tr("Pause"));
        }
    }
}

void VideoWidget::slotStop()
{
    closeVideo();
}

void VideoWidget::slotMicphone()
{
#ifdef BACKCHANNEL
    if (m_url.isEmpty())
    {
        return;
    }

    if (m_nBackChannel)
    {
        m_nBackChannel = 0;
    }
    else
    {
        m_nBackChannel = 1;
    }

    stopBackChannel();

    if (m_nBackChannel)
    {
        startBackChannel();
    }

    setMicphone(m_nBackChannel);
#endif
}

void VideoWidget::slotMute()
{
    if (NULL == m_pPlayer)
    {
        return;
    }

    m_bMute = true;
    m_pPlayer->setVolume(HTVOLUME_MIN);

    if (m_pFloatWidget && !m_pFloatWidget->ui.btnVolume->property("mute").toBool())
    {
        m_pFloatWidget->ui.btnVolume->setProperty("mute", true);
        m_pFloatWidget->ui.btnVolume->setIcon(QIcon(QString::fromUtf8(":/res/Resources/mute.png")));
    }
}

void VideoWidget::slotVolume()
{
    if (NULL == m_pPlayer || NULL == m_pFloatWidget)
    {
        return;
    }
    
    if (m_pFloatWidget->ui.btnVolume->property("mute").toBool())
    {
        m_bMute = false;
        m_pFloatWidget->ui.btnVolume->setProperty("mute", false);
        m_pFloatWidget->ui.btnVolume->setIcon(QIcon(QString::fromUtf8(":/res/Resources/volume.png")));

        m_pPlayer->setVolume(m_pFloatWidget->ui.sliderVolume->value());
    }
    else
    {
        m_bMute = true;
        m_pFloatWidget->ui.btnVolume->setProperty("mute", true);
        m_pFloatWidget->ui.btnVolume->setIcon(QIcon(QString::fromUtf8(":/res/Resources/mute.png")));

        m_pPlayer->setVolume(HTVOLUME_MIN);
    }
}

void VideoWidget::slotVolumeChanged(int action)
{
    int pos = m_pFloatWidget->ui.sliderVolume->sliderPosition();

    // not mute
    if (!m_pFloatWidget->ui.btnVolume->property("mute").toBool())
    {
        if (m_pPlayer)
        {
            m_pPlayer->setVolume(pos);
        }
    }
}

void VideoWidget::slotProgressChanged(int action)
{
    int pos = m_pFloatWidget->ui.sliderProgress->sliderPosition();

    if (m_pPlayer)
    {
        m_pPlayer->seek(pos);
    }
}

void VideoWidget::slotSnapshot()
{
    if (m_pPlayer)
    {
        m_pPlayer->snapshot(VIDEO_FMT_RGB24);
    }
}

void VideoWidget::slotSnapshoted(AVFrame * frame)
{
    QImage image = QImage(frame->data[0], frame->width, frame->height, frame->linesize[0], QImage::Format_RGB888);
    QString path = getSnapshotPath();
    QString file = getTempFile(m_base, ".jpg");
    
    if (!image.save(path + "/" + file, "JPG"))
    {
        QMessageBox::information(NULL, tr("Tips"), tr("Write file failed to check the file path, save path:\n") + path);
    }
    else
    {
        QMessageBox::information(NULL, tr("Tips"), tr("Snapshot successful. save path:\n") + path);
    }
}

void VideoWidget::slotRecord()
{
    if (NULL == m_pPlayer)
    {
        return;
    }

    if (m_pPlayer->isRecording())
    {
        stopRecord();
    }
    else
    {
        startRecord();
    }

    m_bRecording = m_pPlayer->isRecording();
}

void VideoWidget::startRecord()
{
    if (NULL == m_pPlayer)
    {
        return;
    }

    if (m_pPlayer->isRecording())
    {
        return;
    }
    
    if (m_pPlayer->record(m_base))
    {
        if (m_pFloatWidget)
        {
            QIcon icon;
            icon.addFile(QString::fromUtf8(":/res/Resources/stop_record.png"));
            m_pFloatWidget->ui.btnRecord->setIcon(icon);
            m_pFloatWidget->ui.btnRecord->setToolTip(tr("Stop video record"));
        }
    }
}

void VideoWidget::stopRecord()
{
    if (NULL == m_pPlayer)
    {
        return;
    }

    if (m_pPlayer->isRecording())
    {
        m_pPlayer->stopRecord();

        if (m_pFloatWidget)
        {
            QIcon icon;
            icon.addFile(QString::fromUtf8(":/res/Resources/video_record.png"));
            m_pFloatWidget->ui.btnRecord->setIcon(icon);
            m_pFloatWidget->ui.btnRecord->setToolTip(tr("Video record"));
        }
    }
}

void VideoWidget::setMicphone(int flag)
{
#ifdef BACKCHANNEL
    if (m_pFloatWidget)
    {
        if (flag)
        {
            m_pFloatWidget->ui.btnMic->setToolTip(tr("Stop micphone"));
            m_pFloatWidget->ui.btnMic->setIcon(QIcon(QString::fromUtf8(":/res/Resources/stopmic.png")));
        }
        else
        {
            m_pFloatWidget->ui.btnMic->setToolTip(tr("Micphone"));
            m_pFloatWidget->ui.btnMic->setIcon(QIcon(QString::fromUtf8(":/res/Resources/mic.png")));
        }
    }
#endif
}

void VideoWidget::slotSizeChanged(QSize size)
{
    if (m_pPlayer)
    {
        m_pPlayer->setWindowSize(size);
    }
}

void VideoWidget::slotContextMenu(QPoint pt)
{
    if (m_url.isEmpty())
    {
        return;
    }

    QMenu * popMenu = new QMenu();

    popMenu->addAction(m_actionCloseVideo);

    if (m_nRenderMode == RENDER_MODE_KEEP)
    {
        popMenu->addAction(m_actionFillWhole);
    }
    else
    {
        popMenu->addAction(m_actionKeepAspectRatio);
    }

    if (m_bFullScreen)
    {
        popMenu->addAction(m_actionResumeNormal);
    }
    else if (canMaximized())
    {
        popMenu->addAction(m_actionFullScreen);
    }

    popMenu->addAction(m_actionMediaInfo);

    popMenu->exec(pt);

    delete popMenu;
}

void VideoWidget::slotFloatWidget()
{
    if (NULL == m_pFloatWidget)
    {
        return;
    }
    
    QRect rect = m_pFloatWidget->geometry();

    if (!rect.contains(QCursor::pos()))
    {
        m_pFloatWidget->hide();
    }
}

void VideoWidget::slotProgress()
{
    if (m_pPlayer && m_pPlayer->isPlaying() && m_pFloatWidget)
    {
        int64 elapse = m_pPlayer->getElapse();
        int64 duration = m_pPlayer->getDuration();
        int progress = 0;
        
        if (duration != 0)
        {
            progress = elapse * 100 / duration;
        }

        m_pFloatWidget->ui.sliderProgress->setValue(progress);
        m_pFloatWidget->setElapse(elapse);
    }
}

bool VideoWidget::event(QEvent * event)
{
    QEvent::Type tp = event->type();

    switch (event->type())
    {
    case QEvent::Enter:
        showFloatWidget(true);
        break;

    case QEvent::Leave:
    case QEvent::Hide:
        showFloatWidget(false);
        break;

    case QEvent::Show:
        //showFloatWidget(true);
        break;

    default:
        break;
    }

    return QWidget::event(event);
}

QString VideoWidget::getBaseName(QString &url)
{
    char host[100] = {'\0'};
    
    url_split(url.toStdString().c_str(), NULL, 0, NULL, 0, NULL, 0, host, sizeof(host), NULL, NULL, 0);

    return QString(host);
}



