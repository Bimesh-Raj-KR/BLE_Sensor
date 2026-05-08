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

#include "MediaInfo.h"
#include "media_format.h"
#include <QCloseEvent>


MediaInfo::MediaInfo(QWidget *parent, Qt::WindowFlags flags)
: DialogTitleBar(parent, flags)
{
    ui.setupUi(this);

    initDialog();
    connSignalSlot();
}

MediaInfo::~MediaInfo()
{
}

void MediaInfo::initDialog()
{
}

void MediaInfo::connSignalSlot()
{
    connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(slotClose()));
}

void MediaInfo::setUrl(QString url)
{
    ui.editUrl->setText(url);
}

void MediaInfo::setVideoInfo(int codec, int width, int height, int framerate)
{
    switch(codec)
    {
    case VIDEO_CODEC_H264:
        ui.editVideoCodec->setText("H264");
        break;

    case VIDEO_CODEC_H265:
        ui.editVideoCodec->setText("H265");
        break;

    case VIDEO_CODEC_MP4:
        ui.editVideoCodec->setText("MP4");
        break;

    case VIDEO_CODEC_JPEG:
        ui.editVideoCodec->setText("MJPEG");
        break;

    case VIDEO_CODEC_NONE:
        ui.editVideoCodec->setText("N/A");
        break;
        
    default:
        ui.editVideoCodec->setText(tr("Unknow"));
        break;     
    }

    if (width > 0 && height > 0)
    {
        ui.editResolution->setText(QString("%1 x %2").arg(width).arg(height));
    }

    if (framerate > 1)
    {
        ui.editFrameRate->setText(QString("%1").arg(framerate));
    }
}

void MediaInfo::setAudioInfo(int codec, int samplerate, int channel)
{
    switch(codec)
    {
    case AUDIO_CODEC_AAC:
        ui.editAudioCodec->setText("AAC");
        break;

    case AUDIO_CODEC_G711A:
        ui.editAudioCodec->setText("G711A");
        break;

    case AUDIO_CODEC_G711U:
        ui.editAudioCodec->setText("G711U");
        break;

    case AUDIO_CODEC_G726:
        ui.editAudioCodec->setText("G726");
        break;

    case AUDIO_CODEC_G722:
        ui.editAudioCodec->setText("G722");
        break;

    case AUDIO_CODEC_OPUS:
        ui.editAudioCodec->setText("OPUS");
        break;

    case AUDIO_CODEC_NONE:
        ui.editAudioCodec->setText("N/A");
        break;
        
    default:
        ui.editAudioCodec->setText(tr("Unknow"));
        break;     
    }

    if (samplerate > 0)
    {
        ui.editSampleRate->setText(QString("%1 Hz").arg(samplerate));
    }

    if (channel == 1)
    {
        ui.editChannel->setText(tr("Mono"));
    }
    else if (channel == 2)
    {
        ui.editChannel->setText(tr("Stereo"));
    }
    else if (channel > 0)
    {
        ui.editChannel->setText(QString("%1").arg(channel));
    }
}

void MediaInfo::slotClose()
{
    close();
}



