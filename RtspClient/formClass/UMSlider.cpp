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
#include "UMSlider.h"


UMSlider::UMSlider(QWidget *parent) : QSlider(parent)
{
    setMouseTracking(true);
}

UMSlider::UMSlider(Qt::Orientation orientation, QWidget * parent) : QSlider(orientation, parent)
{
    setMouseTracking(true);
}

void UMSlider::mousePressEvent(QMouseEvent *event) 
{
    blockSignals(true);
    QSlider::mousePressEvent(event);
    blockSignals(false);
    
    double ro = event->x() / (double)width();
    setSliderPosition(ro * (maximum() - minimum()) + minimum());
}

void UMSlider::mouseMoveEvent(QMouseEvent *event)
{
    int ro = event->x() * 100 / (double)width();

    setToolTip(QString("%1%").arg(ro));

    QSlider::mouseMoveEvent(event);
}



