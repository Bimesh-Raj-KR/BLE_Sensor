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

#pragma once

#include <QDialog>
#include "ui_RtspClient.h"
#include "DialogTitle.h"
#include "config.h"


class RtspClient : public DialogTitleBar
{
    Q_OBJECT

public:
    RtspClient(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::Widget);

signals:
    void saveLayoutResult(int, QString);
    void loadLayoutResult(int, QString);
    
private slots:
    void slotLayoutOne();
    void slotLayoutFour();
    void slotLayoutSix();
    void slotLayoutNine();
    void slotLayoutSixteen();
    void slotLayoutCustom();
    void slotFullScreen();
    void slotStopAll();
    void slotMuteAll();
    void slotSystemSetting();
    void slotOnlineHelp();
    void slotSaveLayoutToFile(QString);
    void slotLoadLayoutFromFile(QString);

private:
    void initDialog();
    void connSignalSlot();
    void setLanguage(int newLang);
    void resumeVideoWidget();
    void keyPressEvent(QKeyEvent * event);
    void closeEvent(QCloseEvent * event);
    void loadVideoWindowLayout();
    void saveVideoWindowLayout();
    void loadSystemConfig();
    
private:
    Ui::RtspClient  ui;
    SysConfig       m_syscfg;
};



