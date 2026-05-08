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

#include "sys_inc.h"
#include <QWidget>
#include "ui_FloatWidget.h"


class FloatWidget : public QWidget
{
    Q_OBJECT

public:
    FloatWidget(QWidget *parent = NULL, Qt::WindowFlags flags = Qt::Widget);
    ~FloatWidget();

    void showPause(bool show);
    void showMicphone(bool show);
    void showRecord(bool show);
    void showProgress(bool show);
    void setDuration(int64 duration);
    void setElapse(int64 elapse);
    
signals:
    void play();
    void pause();
    void stop();
    void micphone();
    void snapshot();
    void record();
    void volume();
    void volumeChanged(int);
    void progressChanged(int);
    void widgetSelecting();
    void doubleClicked();
    void contextMenu(QPoint);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent * event);
    
private:
    void initWidget();

public:
    Ui::FloatWidget ui;
};


