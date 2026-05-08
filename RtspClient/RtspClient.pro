#-------------------------------------------------
#
# Project created by QtCreator 2021-04-016T09:50:05
#
#-------------------------------------------------

QT += core gui multimedia widgets

CONFIG += c++11
macx:CONFIG += sdk_no_version_check

TEMPLATE = app
TARGET = RtspClient

DEFINES += HTTPS
DEFINES += SRTP
DEFINES += RTSPS
DEFINES += BACKCHANNEL
DEFINES += METADATA
DEFINES += REPLAY
DEFINES += OVER_HTTP
DEFINES += OVER_WEBSOCKET

unix {
    macx {
        DEFINES += IOS
    }
    else {
        DEFINES += EPOLL
    }
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += bm
INCLUDEPATH += formClass
INCLUDEPATH += http
INCLUDEPATH += media
INCLUDEPATH += rtp
INCLUDEPATH += rtsp
INCLUDEPATH += ui

unix {
    macx {
        INCLUDEPATH += /usr/local/include
    }
    else {
        INCLUDEPATH += ffmpeg/include
        INCLUDEPATH += openssl/include
        INCLUDEPATH += sdl/include
        INCLUDEPATH += zlib/include
        INCLUDEPATH += libsrtp/include
    }
}

SOURCES += \
    bm/base64.cpp \
    bm/hqueue.cpp \
    bm/linked_list.cpp \
    bm/net_util.cpp \
    bm/ppstack.cpp \
    bm/rfc_md5.cpp \
    bm/sha256.cpp \
    bm/sys_buf.cpp \
    bm/sys_log.cpp \
    bm/sys_os.cpp \
    bm/util.cpp \
    bm/word_analyse.cpp \
    bm/ws.cpp \
    formClass/CustomLayoutWidget.cpp \
    formClass/DialogTitle.cpp \
    formClass/FloatWidget.cpp \
    formClass/MediaInfo.cpp \
    formClass/RtspClient.cpp \
    formClass/SetRtspInfo.cpp \
    formClass/SystemSetting.cpp \
    formClass/UMSlider.cpp \
    formClass/VideoSubWidget.cpp \
    formClass/VideoWidget.cpp \
    formClass/WidgetManager.cpp \
    formClass/ZoneConfig.cpp \
    http/http_cln.cpp \
    http/http_parse.cpp \
    media/audio_capture.cpp \
    media/audio_decoder.cpp \
    media/audio_encoder.cpp \
    media/audio_play.cpp \
    media/avcodec_mutex.cpp \
    media/avi_write.cpp \
    media/media_codec.cpp \
    media/media_parse.cpp \
    media/media_util.cpp \
    media/rtsp_player.cpp \
    media/video_decoder.cpp \
    media/video_render.cpp \
    media/video_render_sdl.cpp \
    rtp/aac_rtp_rx.cpp \
    rtp/h264_rtp_rx.cpp \
    rtp/h264_util.cpp \
    rtp/h265_rtp_rx.cpp \
    rtp/h265_util.cpp \
    rtp/mjpeg_rtp_rx.cpp \
    rtp/mjpeg_tables.cpp \
    rtp/mpeg4.cpp \
    rtp/mpeg4_rtp_rx.cpp \
    rtp/pcm_rtp_rx.cpp \
    rtp/rtp_rx.cpp \
    rtsp/rtsp_backchannel.cpp \
    rtsp/rtsp_cln.cpp \
    rtsp/rtsp_parse.cpp \
    rtsp/rtsp_rcua.cpp \
    rtsp/rtsp_util.cpp \
    main.cpp \
    utils.cpp \

unix {
    macx {
    SOURCES += \
        media/audio_capture_mac.cpp \
        media/audio_capture_avf.mm \
        media/audio_play_mac.cpp \
        media/audio_play_avf.mm \
        media/video_player.mm \
    }
    else {
    SOURCES += \
        media/alsa.cpp \
        media/audio_capture_linux.cpp \
        media/audio_play_qt.cpp \
        media/video_player.cpp \
    }
}

HEADERS += \
    formClass/CustomLayoutWidget.h \
    formClass/DialogTitle.h \
    formClass/FloatWidget.h \
    formClass/MediaInfo.h \
    formClass/RtspClient.h \
    formClass/SetRtspInfo.h \
    formClass/SystemSetting.h \
    formClass/UMSlider.h \
    formClass/VideoSubWidget.h \
    formClass/VideoWidget.h \
    formClass/WidgetManager.h \
    formClass/ZoneConfig.h \
    media/rtsp_player.h \
    media/video_player.h \

unix {
    macx {
    }
    else {
    HEADERS += \
        media/audio_play_qt.h \
    }
}

FORMS += \
    ui/CustomLayoutWidget.ui \
    ui/FloatWidget.ui \
    ui/MediaInfo.ui \
    ui/RtspClient.ui \
    ui/SetRtspInfo.ui \
    ui/SystemSetting.ui \
    ui/VideoWidget.ui \

unix {
    macx {
        LIBS += -L/usr/local/lib
        LIBS += -lavformat
        LIBS += -lswscale
        LIBS += -lavcodec
        LIBS += -lswresample
        LIBS += -lavutil
        LIBS += -lopus
        LIBS += -lx264
        LIBS += -lx265
        LIBS += -lcrypto
        LIBS += -lssl
        LIBS += -lSDL2
        LIBS += -lsrtp2
        LIBS += -framework AudioToolbox
        LIBS += -framework AVFoundation
        LIBS += -framework CoreAudio
        LIBS += -framework CoreFoundation
        LIBS += -framework CoreMedia
        LIBS += -framework Foundation
    }
    else {
        LIBS += -L$$PWD/ffmpeg/lib/linux
        LIBS += -L$$PWD/openssl/lib/linux
        LIBS += -L$$PWD/zlib/lib/linux
        LIBS += -L$$PWD/sdl/lib/linux
        LIBS += -L$$PWD/libsrtp/lib/linux
        LIBS += -lavformat
        LIBS += -lswscale
        LIBS += -lavcodec
        LIBS += -lswresample
        LIBS += -lavutil
        LIBS += -lopus
        LIBS += -lx264
        LIBS += -lx265
        LIBS += -lcrypto
        LIBS += -lssl
        LIBS += -lz
        LIBS += -lSDL2
        LIBS += -lsrtp2
        LIBS += -lasound
    }
}

RESOURCES += RtspClient.qrc

TRANSLATIONS += rtspclient_zh.ts

