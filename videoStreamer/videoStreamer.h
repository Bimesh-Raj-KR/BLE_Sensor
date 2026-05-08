//******************************* Video Streamer *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for videoStreamer.cpp functions
// Note    : None
// 
//******************************************************************************
#ifndef VIDEO_STREAMER_H
#define VIDEO_STREAMER_H

//******************************* Include Files ********************************
#include <gst/gst.h>
#include <string>
#include <vector>
#include <new>
#include "deviceGetStream.h"
#include "sys_inc.h"
#include "rtsp_cln.h"
#include "hqueue.h"
#include "http.h"
#include "http_parse.h"

//******************************* Global Types *********************************
typedef char int8;
typedef short int16;

typedef struct EVENT_PARAMS
{
    int32 lEvent;
    void *pStreamHandler;
}
_EVENT_PARAMS_;

//*********************************.StreamHandler.******************************
// Description : Handles full streaming process from request to response
// Usage       : Constructor create file to store stream and start stream 
//               using start() and destructor stops streaming using stop()
// Note        : Also include pause, play and reconnect functionality.
//******************************************************************************
class StreamHandler 
{
public:
    StreamHandler(int32 lPort, const std::string& cUrl, 
        const std::string& cUser, const std::string& cPass);
    ~StreamHandler();

    bool start();
    void stop();
    void pauseStream();
    void resumeStream();
    void reconnect();
    void setTargetIps(const std::vector<std::string>& cIps);
    CRtspClient* get_client() { return m_pCClient; }

private:
    int32 m_lPort;
    std::string m_cUrl;
    std::string m_cUser;
    std::string m_cPass;
    std::string m_cFilename;
    std::vector<std::string> m_cTargetIps;
    GstElement *m_pstPipeline;
    GstBus *m_pstBus;
    CRtspClient *m_pCClient;
    pthread_t m_ulMonitorThread;
    bool m_blIsRunning;
    bool m_blIsPaused = false;
};

//***************************** Global Constants *******************************
#define SOURCE_SIZE             (212992)
#define MAX_SLEEP               (100000)
#define INITIAL_PORT            (40000)
#define BRANCH_PORT             (5000)
#define NAME_SIZE               (512)
#define LATENCY                 (500)
#define MAX_BUFFER_SIZE         (100)
#define MAX_TIME                (10)
#define STACK_SIZE              (8)
#define MAX_INTERVAL            (7)
#define LEAKY_MODE              (2)
#define NEXT_PORT               (2)
#define RTSP_CONN_NUM           (1)
#define ERROR                   (-1)

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
void videoStreamerInit(void);
int videoCheckCodec(int32 lEvent, void *pUser);

//*********************** Inline Method Implementations ************************ 

#endif // VIDEO_STREAMER_H
// EOF