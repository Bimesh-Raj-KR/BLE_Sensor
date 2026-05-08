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

#include "RtspClient.h"
#include "CustomLayoutWidget.h"
#include "SystemSetting.h"
#include "utils.h"
#include <QDesktopServices>
#include <QXmlStreamReader>
#include <QTranslator>
#include <QFile>

/***************************************************************************************/

QTranslator   g_translator;

/***************************************************************************************/

RtspClient::RtspClient(QWidget *parent, Qt::WindowFlags flags)
: DialogTitleBar(parent, flags)
{
    loadSystemConfig();

    setLanguage(m_syscfg.sysLang);

    ui.setupUi(this);

    initDialog();

    connSignalSlot();

    if (m_syscfg.enableLog)
    {
        log_time_init("rtspclient");
        log_set_level(m_syscfg.logLevel);
    }

#ifdef DEMO
    QDesktopServices::openUrl(QUrl("https://happytimesoft.com", QUrl::TolerantMode));
#endif
}

void RtspClient::initDialog()
{
    setWindowFlags(Qt::WindowSystemMenuHint|Qt::WindowTitleHint|
                   Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint);

    setWindowTitle(QString("Happytime rtsp client %1").arg(VERSION_STRING));

    setProperty("isTopWindow", 1);
    
    loadVideoWindowLayout();

#ifdef IOS
    resize(QSize(1080, 720));
#endif
}

void RtspClient::setLanguage(int newLang)
{
    if (newLang == LANG_SYS)
    {
        if (QLocale::system().language() == QLocale::Chinese)
        {
            if (g_translator.load(QCoreApplication::applicationDirPath() + "/rtspclient_zh.qm"))
            {
                QCoreApplication::installTranslator(&g_translator);
            }
        }
    }
    else if (newLang == LANG_ZH)
    {
        if (g_translator.load(QCoreApplication::applicationDirPath() + "/rtspclient_zh.qm"))
        {
            QCoreApplication::installTranslator(&g_translator);
        }
    }
}

void RtspClient::connSignalSlot()
{
    connect(ui.btnLayoutOne, SIGNAL(clicked()), this, SLOT(slotLayoutOne()));
    connect(ui.btnLayoutFour, SIGNAL(clicked()), this, SLOT(slotLayoutFour()));
    connect(ui.btnLayoutSix, SIGNAL(clicked()), this, SLOT(slotLayoutSix()));
    connect(ui.btnLayoutNine, SIGNAL(clicked()), this, SLOT(slotLayoutNine()));
    connect(ui.btnLayoutSixteen, SIGNAL(clicked()), this, SLOT(slotLayoutSixteen()));
    connect(ui.btnLayoutCustom, SIGNAL(clicked()), this, SLOT(slotLayoutCustom()));
    connect(ui.btnFullScreen, SIGNAL(clicked()), this, SLOT(slotFullScreen()));
    connect(ui.btnStopAll, SIGNAL(clicked()), this, SLOT(slotStopAll()));
    connect(ui.btnMuteAll, SIGNAL(clicked()), this, SLOT(slotMuteAll()));
    connect(ui.btnSystemSetting, SIGNAL(clicked()), this, SLOT(slotSystemSetting()));
    connect(ui.btnOnlineHelp, SIGNAL(clicked()), this, SLOT(slotOnlineHelp()));
}

/* one window layout */
void RtspClient::slotLayoutOne()
{
    ui.widgetManager->setLayoutMode(LAYOUT_MODE_1);
}

/* four window layout */
void RtspClient::slotLayoutFour()
{
    ui.widgetManager->setLayoutMode(LAYOUT_MODE_4);
}

/* six window layout */
void RtspClient::slotLayoutSix()
{
    ui.widgetManager->setLayoutMode(LAYOUT_MODE_6);
}

/* nine window layout */
void RtspClient::slotLayoutNine()
{
    ui.widgetManager->setLayoutMode(LAYOUT_MODE_9);
}

/* sixteen window layout */
void RtspClient::slotLayoutSixteen()
{
    ui.widgetManager->setLayoutMode(LAYOUT_MODE_16);
}

/* custom window layout */
void RtspClient::slotLayoutCustom()
{
    CustomLayoutWidget dlg(this);

    if (QDialog::Accepted == dlg.exec())
    {
        Layout layout = dlg.getLayout();

        ui.widgetManager->setLayout(layout.rows, layout.cols, layout.zones);
    }
}

void RtspClient::slotFullScreen()
{
    this->layout()->removeWidget(ui.widget);
    ui.widget->setParent(0);
    ui.widget->setWindowFlags(Qt::FramelessWindowHint);
    ui.widget->showMaximized();

    this->hide();
    this->grabKeyboard();
}

void RtspClient::slotStopAll()
{
    ui.widgetManager->closeAll();
}

void RtspClient::slotMuteAll()
{
    ui.widgetManager->muteAll();
}

void RtspClient::slotSystemSetting()
{
    SystemSetting dlg(m_syscfg, this);

    connect(&dlg, SIGNAL(saveLayout(QString)), this, SLOT(slotSaveLayoutToFile(QString)));
    connect(&dlg, SIGNAL(loadLayout(QString)), this, SLOT(slotLoadLayoutFromFile(QString)));
    connect(this, SIGNAL(saveLayoutResult(int,QString)), &dlg, SLOT(slotSaveLayoutResult(int,QString)));
    connect(this, SIGNAL(loadLayoutResult(int,QString)), &dlg, SLOT(slotLoadLayoutResult(int,QString)));
    
    if (QDialog::Accepted == dlg.exec())
    {
        SysConfig config = dlg.getSysConfig();

        /* apply system config parameter */

        if (m_syscfg.enableLog != config.enableLog)
        {
            if (m_syscfg.enableLog)
            {
                log_close();
            }
            else
            {
                log_time_init("rtspclient");
            }
        }

        log_set_level(config.logLevel);

        /* update system config parameter */
        m_syscfg.enableLog = config.enableLog;
        m_syscfg.recordPath = config.recordPath;
        m_syscfg.snapshotPath = config.snapshotPath;
        m_syscfg.audioDevice = config.audioDevice;
        m_syscfg.videoRenderMode = config.videoRenderMode;
        m_syscfg.rtpMulticast = config.rtpMulticast;
        m_syscfg.rtpOverUdp = config.rtpOverUdp;
        m_syscfg.rtspOverHttp = config.rtspOverHttp;
        m_syscfg.rtspOverHttpPort = config.rtspOverHttpPort;
        m_syscfg.rtspOverWs = config.rtspOverWs;
        m_syscfg.rtspOverWsPort = config.rtspOverWsPort;
        m_syscfg.logLevel = config.logLevel;
        m_syscfg.sysLang = config.sysLang;
        m_syscfg.recordTime = config.recordTime;
        m_syscfg.recordSize = config.recordSize;
        m_syscfg.hwDecoding = config.hwDecoding;
        m_syscfg.metadataChannel = config.metadataChannel;
    }
}

void RtspClient::slotOnlineHelp()
{
    QDesktopServices::openUrl(QUrl("https://www.happytimesoft.com/support/rtsp-client/index.html", QUrl::TolerantMode));
}

void RtspClient::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if (ui.widget->isMaximized())
        {
            resumeVideoWidget();
        }
    }
}

void RtspClient::closeEvent(QCloseEvent * event)
{
    saveVideoWindowLayout();

    ui.widgetManager->closeAll();

    event->accept();
}

void RtspClient::resumeVideoWidget()
{
    this->show();
    ui.widget->setParent(this);
    ui.widget->showNormal();

    this->layout()->removeItem(ui.layoutLayout);
    this->layout()->addWidget(ui.widget);
    this->layout()->addItem(ui.layoutLayout);

    releaseKeyboard();
}

/* load video window layout mode */
void RtspClient::loadVideoWindowLayout()
{
    Layout layout;

    getLayout(layout);

    if (layout.zones.size() > 0)
    {
        ui.widgetManager->setLayout(layout.rows, layout.cols, layout.zones);
    }
}

/* save video window layout mode */
void RtspClient::saveVideoWindowLayout()
{
    Layout layout;

    ui.widgetManager->getLayout(layout.rows, layout.cols, layout.zones);
    
    saveLayout(layout);
}

/* load system config information */
void RtspClient::loadSystemConfig()
{
    m_syscfg.snapshotPath = getSnapshotPath();
    m_syscfg.recordPath = getRecordPath();
    m_syscfg.audioDevice = getAudioDevice();
    m_syscfg.enableLog = getEnableLogFlag();
    m_syscfg.videoRenderMode = getVideoRenderMode();
    m_syscfg.rtpMulticast = getRtpMulticast();
    m_syscfg.rtpOverUdp = getRtpOverUdp();
    m_syscfg.rtspOverHttp = getRtspOverHttp();
    m_syscfg.rtspOverHttpPort = getRtspOverHttpPort();
    m_syscfg.rtspOverWs = getRtspOverWs();
    m_syscfg.rtspOverWsPort = getRtspOverWsPort();
    m_syscfg.logLevel = getLogLevel();
    m_syscfg.sysLang = getSysLang();
    m_syscfg.recordTime = getRecordTime();
    m_syscfg.recordSize = getRecordSize();
    m_syscfg.hwDecoding = getHWDecoding();
    m_syscfg.metadataChannel = getMetadataChannel();
}

void RtspClient::slotSaveLayoutToFile(QString filename)
{
    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        emit saveLayoutResult(0, tr("Save layout file failed!"));
        return;
    }

    int rows, cols;
    LayoutZoneList zones;
    QXmlStreamWriter xml;

    ui.widgetManager->getLayout(rows, cols, zones);

    xml.setAutoFormatting(true);
    xml.setDevice(&file);

    xml.writeStartDocument();

    xml.writeStartElement("Layout");
    xml.writeAttribute("rows", QString("%1").arg(rows));
    xml.writeAttribute("cols", QString("%1").arg(cols));

    for (int i = 0; i < zones.size(); i++)
    {
        xml.writeStartElement("zone");

        xml.writeAttribute("left", QString("%1").arg(zones[i].left));
        xml.writeAttribute("right", QString("%1").arg(zones[i].right));
        xml.writeAttribute("top", QString("%1").arg(zones[i].top));
        xml.writeAttribute("bottom", QString("%1").arg(zones[i].bottom));

        QString url;
        QString acct;
        QString pass;

        VideoWidget * pVideoWidget = ui.widgetManager->getWidget(i);
        if (pVideoWidget)
        {
            url = pVideoWidget->getUrl();
            acct = pVideoWidget->getAcct();
            pass = pVideoWidget->getPass();
        }

        xml.writeAttribute("url", QString("%1").arg(url));
        xml.writeAttribute("acct", QString("%1").arg(acct));
        xml.writeAttribute("pass", QString("%1").arg(pass));

        xml.writeEndElement();
    }

    xml.writeEndElement();

    xml.writeEndDocument();
}

void RtspClient::slotLoadLayoutFromFile(QString filename)
{
    QXmlStreamReader xml;

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        emit loadLayoutResult(0, tr("Load layout file failed!"));
        return;
    }

    xml.setDevice(&file);

    if (!xml.readNextStartElement())
    {
        emit loadLayoutResult(0, tr("Load layout file failed!"));
        return;
    }

    if (xml.name().toString() != "Layout")
    {
        emit loadLayoutResult(0, tr("Invalid file format!"));
        return;
    }

    int rows = xml.attributes().value("rows").toString().toInt();
    int cols = xml.attributes().value("cols").toString().toInt();
    LayoutZone zone;
    LayoutZoneList zones;

    if (rows < 0 || rows >= 64 || cols < 0 || cols >= 64)
    {
        emit loadLayoutResult(0, tr("Invalid file format!"));
        return;
    }

    while (xml.readNextStartElement())
    {
        if (xml.name().toString() != "zone")
        {
            xml.skipCurrentElement();
            continue;
        }

        zone.left = xml.attributes().value("left").toString().toInt();
        zone.right = xml.attributes().value("right").toString().toInt();
        zone.top = xml.attributes().value("top").toString().toInt();
        zone.bottom = xml.attributes().value("bottom").toString().toInt();

        if (zone.left < 0 || zone.left >= cols || zone.right < 0 || zone.right >= cols || zone.left > zone.right ||
            zone.top < 0 || zone.top >= rows || zone.bottom < 0 || zone.bottom >= rows || zone.top > zone.bottom)
        {
            xml.skipCurrentElement();
            continue;
        }

        zone.url = xml.attributes().value("url").toString();
        zone.acct = xml.attributes().value("acct").toString();
        zone.pass = xml.attributes().value("pass").toString();

        zones.push_back(zone);

        xml.skipCurrentElement();
    }

    if (zones.size() > 0)
    {
        ui.widgetManager->setLayout(rows, cols, zones);
    }
}


    

