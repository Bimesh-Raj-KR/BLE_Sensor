#-------------------------------------------------
#
# Project created by QtCreator 2014-02-17T15:51:28
#
#-------------------------------------------------

QT -= core gui
TARGET = rtspclient
TEMPLATE = lib

#If compiling the dynamic library, please comment out this line
CONFIG += staticlib

DEFINES += IOS
DEFINES += METADATA
DEFINES += REPLAY
DEFINES += OVER_HTTP
DEFINES += OVER_WEBSOCKET

INCLUDEPATH += .
INCLUDEPATH += bm
INCLUDEPATH += http
INCLUDEPATH += media
INCLUDEPATH += rtp
INCLUDEPATH += rtsp

SOURCES	+= \
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
    http/http_cln.cpp \
    http/http_parse.cpp \
    rtp/aac_rtp_rx.cpp \
    rtp/h264_rtp_rx.cpp \
    rtp/h265_rtp_rx.cpp \
    rtp/mjpeg_rtp_rx.cpp \
    rtp/mjpeg_tables.cpp \
    rtp/mpeg4.cpp \
    rtp/mpeg4_rtp_rx.cpp \
    rtp/pcm_rtp_rx.cpp \
    rtp/rtp_rx.cpp \
    rtsp/rtsp_cln.cpp \
    rtsp/rtsp_parse.cpp \
    rtsp/rtsp_rcua.cpp \
    rtsp/rtsp_util.cpp \

unix {
    target.path = /usr/lib
    INSTALLS += target
}


