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

#ifndef VIDEO_SUB_WIDGET_H
#define VIDEO_SUB_WIDGET_H

#include <QWidget>


class VideoSubWidget : public QWidget
{
    Q_OBJECT

public:
    VideoSubWidget(QWidget *parent = 0, Qt::WindowFlags flags = Qt::Widget);
    ~VideoSubWidget();

    void setTips(QString tips);
    
signals:
    void widgetSelecting();
    void doubleClicked();
    void contextMenu(QPoint);
    void sizeChanged(QSize);

protected:
    void paintEvent(QPaintEvent * event);
    void mouseDoubleClickEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    
private:
    QString m_tips;
};

#endif

