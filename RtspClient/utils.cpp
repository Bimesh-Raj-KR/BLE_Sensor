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
#include "config.h"
#include "utils.h"
#include <QFile>
#include <QCoreApplication>
#include <QSettings>
#include <QDir>
#include <QDateTime>


QString getSnapshotPath()
{
    QSettings setting;

    QString path = setting.value("snapshotPath").toString();
    if (path.isEmpty() || !QFile::exists(path))
    {
        path = QCoreApplication::applicationDirPath() + "/snapshot";
        if (!QFile::exists(path))
        {
            QDir temppath(path);
            temppath.mkdir(path);
        }
    }

    return path;
}

QString getRecordPath()
{
    QSettings setting;

    QString path = setting.value("recordPath").toString();
    if (path.isEmpty() || !QFile::exists(path))
    {
        path = QCoreApplication::applicationDirPath() + "/record";
        if (!QFile::exists(path))
        {
            QDir temppath(path);
            temppath.mkdir(path);
        }
    }

    return path;
}

QString getAudioDevice()
{
    QSettings setting;

    return setting.value("audioDevice").toString();
}

BOOL getEnableLogFlag()
{
    QSettings setting;

    return setting.value("enableLog", 1).toInt();
}

int getVideoRenderMode()
{
    QSettings setting;

    return setting.value("videoRenderMode", 0).toInt();
}

BOOL getRtpMulticast()
{
    QSettings setting;

    return setting.value("rtpMulticast", 0).toInt();
}

BOOL getRtpOverUdp()
{
    QSettings setting;

    return setting.value("rtpOverUdp", 0).toInt();
}

BOOL getRtspOverHttp()
{
    QSettings setting;

    return setting.value("rtspOverHttp", 0).toInt();
}

int getRtspOverHttpPort()
{
    QSettings setting;

    return setting.value("rtspOverHttpPort", 80).toInt();
}

BOOL getRtspOverWs()
{
    QSettings setting;

    return setting.value("rtspOverWs", 0).toInt();
}

int getRtspOverWsPort()
{
    QSettings setting;

    return setting.value("rtspOverWsPort", 80).toInt();
}

int getLogLevel()
{
    QSettings setting;

    return setting.value("logLevel", 2).toInt();
}

int getSysLang()
{
    QSettings setting;

    return setting.value("sysLang", 0).toInt();
}

int getRecordTime()
{
    QSettings setting;

    return setting.value("recordTime", 0).toInt();
}

int getRecordSize()
{
    QSettings setting;

    return setting.value("recordSize", 0).toInt();
}

int getHWDecoding()
{
    QSettings setting;

    return setting.value("HWDecoding", 0).toInt();
}

int getMetadataChannel()
{
    QSettings setting;

    return setting.value("metadataChannel", 0).toInt();
}

void getLayout(Layout & layout)
{
    QSettings setting;
    
    layout.rows = setting.value("layout_Rows", 0).toInt();
    layout.cols = setting.value("layout_Cols", 0).toInt();

    int size = setting.beginReadArray("zones");
    
    for (int i = 0; i < size; ++i) 
    {
        LayoutZone zone;
        
        setting.setArrayIndex(i);
        zone.left = setting.value("left").toInt();
        zone.right = setting.value("right").toInt();
        zone.top = setting.value("top").toInt();
        zone.bottom = setting.value("bottom").toInt();
        zone.url = setting.value("url").toString();
        zone.acct = setting.value("acct").toString();
        zone.pass = setting.value("pass").toString();
        layout.zones.append(zone);
    }
    
    setting.endArray();
}

QString getTempFile(QString prefix, QString extName)
{
    QDateTime tm = QDateTime::currentDateTime();
    QString time;
    QString fileName;

    time = QString::asprintf("%04d%02d%02d_%02d%02d%02d%03d",
        tm.date().year(), tm.date().month(), tm.date().day(),
        tm.time().hour(), tm.time().minute(), tm.time().second(),
        tm.time().msec()); 
        
    fileName = prefix + "_" + time + extName; 

    return fileName;
}

void saveSnapshotPath(QString path)
{
    QSettings setting;

    setting.setValue("snapshotPath", path);
}

void saveRecordPath(QString path)
{
    QSettings setting;

    setting.setValue("recordPath", path);
}

void saveAudioDevice(QString str)
{
    QSettings setting;

    setting.setValue("audioDevice", str);
}

void saveEnableLogFlag(BOOL flag)
{
    QSettings setting;

    setting.setValue("enableLog", flag);
}

void saveVideoRenderMode(int mode)
{
    QSettings setting;

    setting.setValue("videoRenderMode", mode);
}

void saveRtpMulticast(BOOL flag)
{
    QSettings setting;

    setting.setValue("rtpMulticast", flag);
}

void saveRtpOverUdp(BOOL flag)
{
    QSettings setting;

    setting.setValue("rtpOverUdp", flag);
}

void saveRtspOverHttp(BOOL flag)
{
    QSettings setting;

    setting.setValue("rtspOverHttp", flag);
}

void saveRtspOverHttpPort(int port)
{
    QSettings setting;

    setting.setValue("rtspOverHttpPort", port);
}

void saveRtspOverWs(BOOL flag)
{
    QSettings setting;

    setting.setValue("rtspOverWs", flag);
}

void saveRtspOverWsPort(int port)
{
    QSettings setting;

    setting.setValue("rtspOverWsPort", port);
}

void saveLogLevel(int level)
{
    QSettings setting;

    setting.setValue("logLevel", level);
}

void saveSysLang(int lang)
{
    QSettings setting;

    setting.setValue("sysLang", lang);
}

void saveRecordTime(int value)
{
    QSettings setting;

    setting.setValue("recordTime", value);
}

void saveRecordSize(int value)
{
    QSettings setting;

    setting.setValue("recordSize", value);
}

void saveHWDecoding(int value)
{
    QSettings setting;

    setting.setValue("HWDecoding", value);
}

void saveMetadataChannel(int value)
{
    QSettings setting;

    setting.setValue("metadataChannel", value);
}

void saveLayout(Layout & layout)
{
    QSettings settings;

    settings.setValue("layout_Rows", layout.rows);
    settings.setValue("layout_Cols", layout.cols);
    
    settings.beginWriteArray("zones");
    
    for (int i = 0; i < layout.zones.size(); ++i) 
    {
        settings.setArrayIndex(i);
        settings.setValue("left", layout.zones[i].left);
        settings.setValue("right", layout.zones[i].right);
        settings.setValue("top", layout.zones[i].top);
        settings.setValue("bottom", layout.zones[i].bottom);
        settings.setValue("url", layout.zones[i].url);
        settings.setValue("acct", layout.zones[i].acct);
        settings.setValue("pass", layout.zones[i].pass);
    }
    
    settings.endArray();
}


