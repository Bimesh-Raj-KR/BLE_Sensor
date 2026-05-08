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

#include "RtspClient.h"
#include "rtsp_parse.h"
#include "http_parse.h"
#include <QFile>
#include <QApplication>
#if __LINUX_OS__
#include <SDL2/SDL.h>
#endif
#ifdef SRTP
#include "srtp2/srtp.h"
#endif

/**************************************************************************************/


#if defined(_DEBUG) || defined(DEBUG)
static void av_log_callback(void* ptr, int level, const char* fmt, va_list vl)   
{
    int htlv = HT_LOG_INFO;
    char buff[4096];

    if (av_log_get_level() < level)
    {
        return;
    }
    
    vsnprintf(buff, sizeof(buff), fmt, vl);

    if (AV_LOG_TRACE == level || AV_LOG_VERBOSE == level)
    {
        htlv = HT_LOG_TRC;
    }
    else if (AV_LOG_DEBUG == level)
    {
        htlv = HT_LOG_DBG;
    }
    else if (AV_LOG_INFO == level)
    {
        htlv = HT_LOG_INFO;
    }
    else if (AV_LOG_WARNING == level)
    {
        htlv = HT_LOG_WARN;
    }
    else if (AV_LOG_ERROR == level)
    {
        htlv = HT_LOG_ERR;
    }
    else if (AV_LOG_FATAL == level || AV_LOG_PANIC == level)
    {
        htlv = HT_LOG_FATAL;
    }
    
    log_print(htlv, "%s", buff);
}
#endif

void setAppStyleSheet(QApplication & app)
{
    QFile file(QCoreApplication::applicationDirPath() + "/" + CSS_FILE);

    file.open(QFile::ReadOnly);
    
    app.setStyleSheet(QLatin1String(file.readAll()));
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    QCoreApplication::setApplicationName("rtspclient");
    QCoreApplication::setOrganizationName("happytimesoft");

    setAppStyleSheet(a);

    network_init();

#if __LINUX_OS__
    // Ignore broken pipes
    signal(SIGPIPE, SIG_IGN);

    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
#endif

#if defined(_DEBUG) || defined(DEBUG)
    av_log_set_callback(av_log_callback);
    av_log_set_level(AV_LOG_QUIET);
#endif

#ifdef SRTP
    if (srtp_init() != srtp_err_status_ok) 
    {
        log_print(HT_LOG_ERR, "libsrtp init failed\r\n");
    }
#endif

    sys_buf_init(200);
    rtsp_parse_buf_init(200);
    http_msg_buf_init(200);

    RtspClient w(0, Qt::WindowSystemMenuHint|Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint);

#ifdef IOS
    w.show();
#else    
    w.showMaximized();
#endif

    return a.exec();
}



