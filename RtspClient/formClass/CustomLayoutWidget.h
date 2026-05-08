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

#ifndef CUSTOM_LAYOUT_WIDGET_H
#define CUSTOM_LAYOUT_WIDGET_H

#include <QDialog>
#include "ui_CustomLayoutWidget.h"
#include "DialogTitle.h"

class CustomLayoutWidget : public DialogTitleBar
{
    Q_OBJECT

public:
    CustomLayoutWidget(QWidget *parent = 0, Qt::WindowFlags flags = Qt::Widget);
    ~CustomLayoutWidget();

    Layout getLayout();

public slots:
    void slotConfirm();
    void slotGridRowsChanged(int index);
    void slotGridColsChanged(int index);
    
private:
    void initCustomLayoutWidget();
    void connSignalSlot();
    
private:
    Ui::CustomLayoutWidget ui;
};

#endif


