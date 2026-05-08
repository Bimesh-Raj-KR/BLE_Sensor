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

#include "SystemSetting.h"
#include "utils.h"
#include "video_decoder.h"
#include <QFile>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFileDialog>

#if __WINDOWS_OS__
#include "audio_capture_win.h"
#elif defined(IOS)
#include "audio_capture_mac.h"
#elif defined(ANDROID)
#include "audio_capture_android.h"
#elif __LINUX_OS__
#include "audio_capture_linux.h"
#endif

SystemSetting::SystemSetting(SysConfig &config, QWidget *parent, Qt::WindowFlags flags)
: DialogTitleBar(parent, flags)
, m_syscfg(config)
{
    ui.setupUi(this);

    initDialog();
    connSignalSlot();
}

SystemSetting::~SystemSetting()
{
}

void SystemSetting::initDialog()
{
    ui.cmbLogLevel->addItem(tr("TRACE"));
    ui.cmbLogLevel->addItem(tr("DEBUG"));
    ui.cmbLogLevel->addItem(tr("INFO"));
    ui.cmbLogLevel->addItem(tr("WARNING"));
    ui.cmbLogLevel->addItem(tr("ERROR"));
    ui.cmbLogLevel->addItem(tr("FATAL"));

    ui.cmbLanguage->addItem(tr("System"));
    ui.cmbLanguage->addItem(tr("English"));
    ui.cmbLanguage->addItem(tr("Chinese"));
    
    ui.cmbVideoRenderMode->addItem(tr("Keep the original aspect ratio"));
    ui.cmbVideoRenderMode->addItem(tr("Fill the whole window"));

    ui.cmbHWDecoding->addItem(tr("Automatic"), HW_DECODING_AUTO);
#if __WINDOWS_OS__
    ui.cmbHWDecoding->addItem(tr("Direct3D11 Video Acceleration"), HW_DECODING_D3D11);
    ui.cmbHWDecoding->addItem(tr("DirectX Video Acceleration (DXVA) 2.0"), HW_DECODING_DXVA);
#elif defined(IOS)
    ui.cmbHWDecoding->addItem(tr("Video Tool Box"), HW_DECODING_VIDEOTOOLBOX);
    ui.cmbHWDecoding->addItem(tr("OPENCL"), HW_DECODING_OPENCL);
#elif defined(ANDROID)
    ui.cmbHWDecoding->addItem(tr("Media Codec"), HW_DECODING_MEDIACODEC);
#elif __LINUX_OS__
    ui.cmbHWDecoding->addItem(tr("VAAPI"), HW_DECODING_VAAPI);
    ui.cmbHWDecoding->addItem(tr("OPENCL"), HW_DECODING_OPENCL);
#endif
    ui.cmbHWDecoding->addItem(tr("Disable"), HW_DECODING_DISABLE);

    int i, count;
    char name[256] = {'\0'};

    ui.cmbAudioDevice->addItem("Default device");
    
#if __WINDOWS_OS__
    count = CWAudioCapture::getDeviceNums();

    for (i = 0; i < count; i++)
    {
        if (CWAudioCapture::getDeviceName(i, name, sizeof(name)-1))
        {
            ui.cmbAudioDevice->addItem(QString::fromLocal8Bit(name));
        }
    }
#elif defined(IOS)
    count = CMAudioCapture::getDeviceNums();

    for (i = 0; i < count; i++)
    {
        if (CMAudioCapture::getDeviceName(i, name, sizeof(name)-1))
        {
            ui.cmbAudioDevice->addItem(QString::fromLocal8Bit(name));
        }
    }
#elif __LINUX_OS__
    count = CLAudioCapture::getDeviceNums();

    for (i = 0; i < count; i++)
    {
        if (CLAudioCapture::getDeviceName(i, name, sizeof(name)-1))
        {
            ui.cmbAudioDevice->addItem(QString::fromLocal8Bit(name));
        }
    }
#endif

    ui.chkMetadataChannel->setChecked(m_syscfg.metadataChannel);
    ui.editSnapshotPath->setText(m_syscfg.snapshotPath);
    ui.editRecordPath->setText(m_syscfg.recordPath);
    ui.chkEnableLog->setChecked(m_syscfg.enableLog);
    ui.chkRtpMulticast->setChecked(m_syscfg.rtpMulticast);
    ui.chkRtpOverUdp->setChecked(m_syscfg.rtpOverUdp);
    ui.chkRtspOverHttp->setChecked(m_syscfg.rtspOverHttp);
    ui.spinHttpPort->setValue(m_syscfg.rtspOverHttpPort);
    ui.chkRtspOverWs->setChecked(m_syscfg.rtspOverWs);
    ui.spinWsPort->setValue(m_syscfg.rtspOverWsPort);
    ui.cmbVideoRenderMode->setCurrentIndex(m_syscfg.videoRenderMode);
    ui.cmbLogLevel->setCurrentIndex(m_syscfg.logLevel);
    ui.cmbLanguage->setCurrentIndex(m_syscfg.sysLang);
    ui.timeRecordTime->setTime(QTime(0,0,0,0).addSecs(m_syscfg.recordTime));
    ui.spinRecordSize->setValue(m_syscfg.recordSize);

    for (i = 0; i < ui.cmbHWDecoding->count(); i++)
    {
        if (ui.cmbHWDecoding->itemData(i).toInt() == m_syscfg.hwDecoding)
        {
            ui.cmbHWDecoding->setCurrentIndex(i);
            break;
        }
    }

    for (i = 0; i < ui.cmbAudioDevice->count(); i++)
    {
        if (ui.cmbAudioDevice->itemText(i) == m_syscfg.audioDevice)
        {
            ui.cmbAudioDevice->setCurrentIndex(i);
            break;
        }
    }
}

void SystemSetting::connSignalSlot()
{
    connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(close()));    
    connect(ui.btnConfirm, SIGNAL(clicked()), this, SLOT(slotConfirm()));
    connect(ui.btnOpenSnapshotPath, SIGNAL(clicked()), this, SLOT(slotOpenSnapshotPath()));    
    connect(ui.btnOpenRecordPath, SIGNAL(clicked()), this, SLOT(slotOpenRecordPath()));
    connect(ui.btnBrowseSnapshotPath, SIGNAL(clicked()), this, SLOT(slotBrowseSnapshotPath()));    
    connect(ui.btnBrowseRecordPath, SIGNAL(clicked()), this, SLOT(slotBrowseRecordPath()));
    connect(ui.btnSaveLayout, SIGNAL(clicked()), this, SLOT(slotSaveLayout()));
    connect(ui.btnLoadLayout, SIGNAL(clicked()), this, SLOT(slotLoadLayout()));
}

void SystemSetting::slotOpenSnapshotPath()
{
    QString path = ui.editSnapshotPath->text();
    if (path.isEmpty())
    {
        return;
    }

    if (!QFile::exists(path))
    {
        QMessageBox::information(this, tr("Tips"), tr("Snapshot path not exist"));
        return;
    }
    
    QDesktopServices::openUrl(QUrl("file:///" + path, QUrl::TolerantMode));
}

void SystemSetting::slotOpenRecordPath()
{
    QString path = ui.editRecordPath->text();
    if (path.isEmpty())
    {
        return;
    }

    if (!QFile::exists(path))
    {
        QMessageBox::information(this, tr("Tips"), tr("Record path not exist"));
        return;
    }
    
    QDesktopServices::openUrl(QUrl("file:///" + path, QUrl::TolerantMode));
}

void SystemSetting::slotBrowseSnapshotPath()
{
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString path = ui.editSnapshotPath->text();
    
    path = QFileDialog::getExistingDirectory(this, tr("Select snapshot save path"), path, options);
    if (!path.isEmpty())
    {
        ui.editSnapshotPath->setText(path);
    }
}

void SystemSetting::slotBrowseRecordPath()
{
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString path = ui.editRecordPath->text();
    
    path = QFileDialog::getExistingDirectory(this, tr("Select record save path"), path, options);
    if (!path.isEmpty())
    {
        ui.editRecordPath->setText(path);
    }
}

void SystemSetting::slotSaveLayout()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save layout"), QString(), "xml file (*.xml)");
    if (!filename.isEmpty())
    {
        emit saveLayout(filename);
    }
}

void SystemSetting::slotLoadLayout()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Load layout"), QString(), "xml file (*.xml)");
    if (!filename.isEmpty())
    {
        emit loadLayout(filename);
    }
}

void SystemSetting::slotSaveLayoutResult(int ret, QString err)
{
    QMessageBox::information(this, tr("Save layout"), err);
}

void SystemSetting::slotLoadLayoutResult(int ret, QString err)
{
    QMessageBox::information(this, tr("Load layout"), err);
}

void SystemSetting::slotConfirm()
{
    m_syscfg.recordPath = ui.editRecordPath->text();
    m_syscfg.snapshotPath = ui.editSnapshotPath->text();
    m_syscfg.audioDevice = ui.cmbAudioDevice->currentText();
    m_syscfg.enableLog = ui.chkEnableLog->isChecked();
    m_syscfg.logLevel = ui.cmbLogLevel->currentIndex();
    m_syscfg.rtpMulticast = ui.chkRtpMulticast->isChecked();
    m_syscfg.rtpOverUdp = ui.chkRtpOverUdp->isChecked();
    m_syscfg.rtspOverHttp = ui.chkRtspOverHttp->isChecked();
    m_syscfg.rtspOverHttpPort = ui.spinHttpPort->value();
    m_syscfg.rtspOverWs = ui.chkRtspOverWs->isChecked();
    m_syscfg.rtspOverWsPort = ui.spinWsPort->value();
    m_syscfg.videoRenderMode = ui.cmbVideoRenderMode->currentIndex();
    m_syscfg.sysLang = ui.cmbLanguage->currentIndex();
    m_syscfg.recordTime = QTime(0, 0, 0, 0).secsTo(ui.timeRecordTime->time());
    m_syscfg.recordSize = ui.spinRecordSize->value();
    m_syscfg.hwDecoding = ui.cmbHWDecoding->currentData().toInt();
    m_syscfg.metadataChannel = ui.chkMetadataChannel->isChecked();
    
    if (!QFile::exists(m_syscfg.snapshotPath))
    {
        QMessageBox::information(this, tr("Tips"), tr("Please specify the correct snapshot path"));
        ui.editSnapshotPath->setFocus();
        return;
    }

    if (!QFile::exists(m_syscfg.recordPath))
    {
        QMessageBox::information(this, tr("Tips"), tr("Please specify the correct video recording path"));
        ui.editRecordPath->setFocus();
        return;
    }

    saveSnapshotPath(m_syscfg.snapshotPath);
    saveRecordPath(m_syscfg.recordPath);
    saveAudioDevice(m_syscfg.audioDevice);
    saveEnableLogFlag(m_syscfg.enableLog);
    saveLogLevel(m_syscfg.logLevel);
    saveRtpMulticast(m_syscfg.rtpMulticast);
    saveRtpOverUdp(m_syscfg.rtpOverUdp);
    saveRtspOverHttp(m_syscfg.rtspOverHttp);
    saveRtspOverHttpPort(m_syscfg.rtspOverHttpPort);
    saveRtspOverWs(m_syscfg.rtspOverWs);
    saveRtspOverWsPort(m_syscfg.rtspOverWsPort);
    saveVideoRenderMode(m_syscfg.videoRenderMode);
    saveSysLang(m_syscfg.sysLang);
    saveRecordTime(m_syscfg.recordTime);
    saveRecordSize(m_syscfg.recordSize);
    saveHWDecoding(m_syscfg.hwDecoding);
    saveMetadataChannel(m_syscfg.metadataChannel);
    
    accept();
}



