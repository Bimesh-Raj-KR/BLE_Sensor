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

#ifndef MEDIA_INFO_DLG_H
#define MEDIA_INFO_DLG_H

#include "sys_inc.h"
#include <QDialog>
#include "ui_MediaInfo.h"
#include "DialogTitle.h"

class MediaInfo : public DialogTitleBar
{
    Q_OBJECT

public:
    MediaInfo(QWidget *parent = 0, Qt::WindowFlags flags = Qt::Widget);
    ~MediaInfo();

    void setUrl(QString url);
    void setVideoInfo(int codec, int width, int height, int framerate);
    void setAudioInfo(int codec, int samplerate, int channel);
    
private slots:
    void slotClose();

private:
    void initDialog();
    void connSignalSlot();
    
private:
    Ui::MediaInfo ui;
};

#endif




