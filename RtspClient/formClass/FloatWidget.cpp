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
#include "FloatWidget.h"
#include <QResizeEvent>
#include <QMessageBox>


FloatWidget::FloatWidget(QWidget *parent, Qt::WindowFlags flags)
: QWidget(parent, flags | Qt::FramelessWindowHint | Qt::Tool)
{
    ui.setupUi(this);

    initWidget();
}

FloatWidget::~FloatWidget()
{

}

void FloatWidget::initWidget()
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    QObject::connect(ui.btnPlay, SIGNAL(clicked()), this, SIGNAL(play()));
    QObject::connect(ui.btnPause, SIGNAL(clicked()), this, SIGNAL(pause()));
    QObject::connect(ui.btnStop, SIGNAL(clicked()), this, SIGNAL(stop()));
    QObject::connect(ui.btnMic, SIGNAL(clicked()), this, SIGNAL(micphone()));
    QObject::connect(ui.btnSnapshot, SIGNAL(clicked()), this, SIGNAL(snapshot()));
    QObject::connect(ui.btnRecord, SIGNAL(clicked()), this, SIGNAL(record()));
    QObject::connect(ui.btnVolume, SIGNAL(clicked()), this, SIGNAL(volume()));
    QObject::connect(ui.sliderVolume, SIGNAL(actionTriggered(int)), this, SIGNAL(volumeChanged(int)));
    QObject::connect(ui.sliderProgress, SIGNAL(actionTriggered(int)), this, SIGNAL(progressChanged(int)));
}

void FloatWidget::showPause(bool show)
{
    ui.btnPause->setVisible(show);
}

void FloatWidget::showMicphone(bool show)
{
    ui.btnMic->setVisible(show);
}

void FloatWidget::showRecord(bool show)
{
    ui.btnRecord->setVisible(show);
}

void FloatWidget::showProgress(bool show)
{
    ui.labElapse->setVisible(show);
    ui.labDuration->setVisible(show);
    ui.sliderProgress->setVisible(show);

    if (show)
    {
        ui.horizontalSpacer1->changeSize(10, 20, QSizePolicy::Ignored, QSizePolicy::Minimum);
        ui.horizontalSpacer2->changeSize(10, 20, QSizePolicy::Ignored, QSizePolicy::Minimum);
    }
    else
    {
        ui.horizontalSpacer1->changeSize(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        ui.horizontalSpacer2->changeSize(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    }
}

void FloatWidget::setDuration(int64 duration)
{
    int seconds = duration / 1000;
    int minutes = seconds / 60;
    int hour = minutes / 60;

    seconds %= 60;
    minutes %= 60;

    if (hour > 0)
    {
        ui.labDuration->setText(QString("%1:%2:%3")
            .arg(hour, 2, 10, QLatin1Char('0'))
            .arg(minutes, 2, 10, QLatin1Char('0'))
            .arg(seconds, 2, 10, QLatin1Char('0')));
    }
    else
    {
        ui.labDuration->setText(QString("%1:%2")
            .arg(minutes, 2, 10, QLatin1Char('0'))
            .arg(seconds, 2, 10, QLatin1Char('0')));
    }
}

void FloatWidget::setElapse(int64 elapse)
{
    int seconds = elapse / 1000;
    int minutes = seconds / 60;
    int hour = minutes / 60;

    seconds %= 60;
    minutes %= 60;

    if (hour > 0)
    {
        ui.labElapse->setText(QString("%1:%2:%3")
            .arg(hour, 2, 10, QLatin1Char('0'))
            .arg(minutes, 2, 10, QLatin1Char('0'))
            .arg(seconds, 2, 10, QLatin1Char('0')));
    }
    else
    {
        ui.labElapse->setText(QString("%1:%2")
            .arg(minutes, 2, 10, QLatin1Char('0'))
            .arg(seconds, 2, 10, QLatin1Char('0')));
    }
}

/* On linux platform, Qt::WA_TransparentForMouseEvents does not work and it needs to handle mouse events  */

void FloatWidget::mousePressEvent(QMouseEvent *event)
{
    emit widgetSelecting();

    if (event->button() == Qt::RightButton)
    {
        emit contextMenu(event->globalPos());
    }
}

void FloatWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
    emit doubleClicked();
}





