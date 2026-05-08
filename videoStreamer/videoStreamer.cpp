//******************************* Video Streamer *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : videoStreamer.cpp
// Summary : Setups RtspClient and Gstreamer to stream vide using stream Uri
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 12/Mar/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "videoStreamer.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static HQUEUE *spQueue = NULL;
static int32 sFlag = 0;
static pthread_t sulThreadId = 0;
static int32 slSuffix = 0;

//***************************** Local Functions ********************************
static void *reconnectHandler(void);

//*****************************.StreamHandler.**********************************
// Purpose : Streamhandler constructor. Creates a file to sore stream
// Inputs  : lPort - Port to receive stream
//           cUrl - Stream uri
//           cUser - Username
//           cPass - Password
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
StreamHandler::StreamHandler(int32 lPort, const std::string& cUrl, 
        const std::string& cUser, const std::string& cPass)
    : m_lPort(lPort), m_cUrl(cUrl), m_cUser(cUser), m_cPass(cPass), 
      m_pstPipeline(nullptr), m_pstBus(nullptr), m_pCClient(nullptr), 
      m_blIsRunning(false) 
{
    m_cFilename = "record-" + std::to_string(m_lPort) + "-0.h264";
}

//*****************************.~StreamHandler.*********************************
// Purpose : Streamhandler destructor. Stops streaming
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
StreamHandler::~StreamHandler() 
{
    stop();
}

//***********************************.start.************************************
// Purpose : Function to request device to start streaming 
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool StreamHandler::start() 
{
    bool blCheck = false;
    GstElement *pstSource = NULL;
    GstElement *pstJitter = NULL;
    GstElement *pstDepay = NULL;
    GstElement *pstQueue = NULL;
    GstElement *pstParse = NULL;
    GstElement *pstSink = NULL;
    GstCaps *pstCaps = NULL;
    GstCaps *pstParseCaps = NULL;

    m_pCClient = new (std::nothrow) CRtspClient();

    if (NULL != m_pCClient)
    {
        m_pCClient->set_notify_cb(videoCheckCodec, (void*)this);
        m_pCClient->set_rx_timeout(MAX_TIME);
        m_pstPipeline = gst_pipeline_new(nullptr);

        pstSource = gst_element_factory_make("udpsrc", "source");
        pstJitter = gst_element_factory_make("rtpjitterbuffer", nullptr);
        pstDepay = gst_element_factory_make("rtph264depay", nullptr);
        pstQueue = gst_element_factory_make("queue", nullptr);
        pstParse = gst_element_factory_make("h264parse", nullptr);
        pstSink  = gst_element_factory_make("filesink", "sink");

        if ((NULL != m_pstPipeline) && (NULL != pstSource) && 
            (NULL != pstJitter) && (NULL != pstDepay) && (NULL != pstQueue) && 
            (NULL != pstParse) && (NULL != pstSink))
        {
            g_object_set(pstSource, "port", m_lPort, "address", "0.0.0.0", 
                "buffer-size", SOURCE_SIZE, nullptr);
            g_object_set(pstJitter, "mode", 1, "latency", LATENCY, 
                "drop-on-latency", TRUE, nullptr);
            g_object_set(pstQueue, "max-size-buffers", MAX_BUFFER_SIZE, 
                "leaky", LEAKY_MODE, nullptr);
            g_object_set(pstSink, "location", m_cFilename.c_str(), 
                "sync", FALSE, nullptr);

            // Caps for the udpsrc
            pstCaps = gst_caps_from_string("application/x-rtp,media=video,"
                            "clock-rate=90000,encoding-name=H264,payload=96");

            if (NULL != pstCaps)
            {
                g_object_set(pstSource, "caps", pstCaps, nullptr);
                gst_caps_unref(pstCaps);
            }

            gst_bin_add_many(GST_BIN(m_pstPipeline), pstSource, pstJitter, 
                            pstDepay, pstQueue, pstParse, pstSink, nullptr);
            gst_element_link_many(pstSource, pstJitter, pstDepay, pstQueue, 
                                    pstParse, nullptr);

            pstParseCaps = gst_caps_from_string("video/x-h264, stream-format="
                        "byte-stream");

            if (NULL != pstParseCaps)
            {
                if (TRUE != gst_element_link_filtered(pstParse, 
                                            pstSink, pstParseCaps))
                {
                    printf("Failed to link parse to sink with "
                                "byte-stream caps!\n");
                }

                gst_caps_unref(pstParseCaps);
            }

            // Get the bus
            if (NULL != m_pstBus)
            {
                gst_object_unref(m_pstBus);
                m_pstBus = nullptr;
            }

            m_pstBus = gst_element_get_bus(m_pstPipeline);

            if (GST_STATE_CHANGE_FAILURE != gst_element_set_state(m_pstPipeline,
                GST_STATE_PLAYING))
            {
                m_pCClient->set_rtp_over_udp(1);
                m_pCClient->rtsp_start((int8*)m_cUrl.c_str(), 
                            (int8*)m_cUser.c_str(), (int8*)m_cPass.c_str());
                m_blIsRunning = true;
                blCheck = true;
            }
        }
    }

    return blCheck;
}

//********************************.reconnect.***********************************
// Purpose : Function to reconnect to device if connection fails
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void StreamHandler::reconnect()
{
    GstElement *pstSource = NULL;
    GstElement *pstSink = NULL;
    char cNewFileName[NAME_SIZE] = {0};

    if ((0 != m_blIsRunning) && (NULL != m_pCClient) && (NULL != m_pstPipeline))
    {
        m_pCClient->rtsp_close();
        m_pCClient->set_rtp_over_udp(1);
        gst_element_set_state(m_pstPipeline, GST_STATE_NULL);
        m_lPort += NEXT_PORT; 
        
        snprintf(cNewFileName, sizeof(cNewFileName), "record-%d-%d.h264", 
                    m_lPort, ++slSuffix);
        printf("[Reconnect] New Port: %d | New File: %s\n", 
                    m_lPort, cNewFileName);

        pstSource = gst_bin_get_by_name(GST_BIN(m_pstPipeline), "source");

        if (NULL != pstSource)
        {
            g_object_set(pstSource, "port", (int32)m_lPort, NULL);
            gst_object_unref(pstSource);
        }

        pstSink = gst_bin_get_by_name(GST_BIN(m_pstPipeline), "sink");

        if (NULL != pstSink) 
        {
            g_object_set(pstSink, "location", cNewFileName, NULL);
            gst_object_unref(pstSink);
        }

        usleep(200000);
        gst_element_set_state(m_pstPipeline, GST_STATE_PLAYING);
        
        m_pCClient->rtsp_start((int8*)m_cUrl.c_str(), 
                               (int8*)m_cUser.c_str(), (int8*)m_cPass.c_str());
    }
}

//***********************************.stop.*************************************
// Purpose : Function to stop streaming
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void StreamHandler::stop() 
{
    GstMessage *pstMessage = NULL;

    m_blIsRunning = false;

    if (NULL != m_pCClient) 
    {
        m_pCClient->set_notify_cb(NULL, NULL);
        m_pCClient->rtsp_close();
        delete m_pCClient;
        m_pCClient = nullptr;
    }

    if (NULL != m_pstPipeline) 
    {
        gst_element_send_event(m_pstPipeline, gst_event_new_eos());
        pstMessage = gst_bus_timed_pop_filtered(m_pstBus, GST_SECOND * 2, 
                        (GstMessageType)(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));

        if (NULL != pstMessage)
        {
            gst_message_unref(pstMessage);
        }

        gst_element_set_state(m_pstPipeline, GST_STATE_NULL);
        gst_element_get_state(m_pstPipeline, nullptr, nullptr, GST_SECOND);

        if (NULL != m_pstBus)
        {
            gst_bus_set_flushing(m_pstBus, TRUE);

            while ((pstMessage = gst_bus_pop(m_pstBus)))
            {
                gst_message_unref(pstMessage);
            }

            gst_element_set_bus(m_pstPipeline, NULL);
            gst_object_unref(m_pstBus);
            m_pstBus = nullptr;
        }

        gst_object_unref(m_pstPipeline);
        m_pstPipeline = nullptr;
    }
}

//******************************.pauseStream.***********************************
// Purpose : Function to pause streaming
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void StreamHandler::pauseStream()
{
    if (m_pstPipeline && !m_blIsPaused)
    {
        printf("[Port %d] Pausing...\n", m_lPort);
        m_pCClient->rtsp_pause();
        gst_element_set_state(m_pstPipeline, GST_STATE_PAUSED);
        m_blIsPaused = true;
    }
}

//*********************************.resumeStream.*******************************
// Purpose : Function to resume streaming
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void StreamHandler::resumeStream()
{
    if (m_pstPipeline && m_blIsPaused)
    {
        printf("[Port %d] Resuming...\n", m_lPort);
        gst_element_set_state(m_pstPipeline, GST_STATE_PLAYING);
        m_pCClient->rtsp_play();
        m_blIsPaused = false;
    }
}

//**************************.videoStreamerInit.*********************************
// Purpose : Function that manages streaming. Creates manager for 
//           one media stream
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void videoStreamerInit(void) 
{
    bool blRunning = false;
    int8 cChoice = 0;
    int8 cStreamUri[ONVIF_URI_LEN] = {0};
    uint8 ucIterator = 0;
    int32 lIndex = 0;
    StreamHandler* pstHandler = NULL;
    EVENT_PARAMS stExitParams = {0};
    EVENT_PARAMS stCleanup = {0};
    std::vector<StreamHandler*> pCStreamers = {};
    std::string cCUrl = "";
    std::string cCUserName = "";
    std::string cCPassword = "";


    // System Level Initialization
    log_init("videoStreamer.log\n");
    log_set_level(HT_LOG_DBG);
    network_init();
    signal(SIGPIPE, SIG_IGN);
    
    // Initialize custom buffers for the RTSP stack
    sys_buf_init(RTSP_CONN_NUM * STACK_SIZE);
    rtsp_parse_buf_init(RTSP_CONN_NUM * STACK_SIZE);

    // Global GStreamer Initialization
    gst_init(NULL, NULL);

    // Setup the Event Queue for Reconnections
    spQueue = hqueue_create(NAME_SIZE, sizeof(EVENT_PARAMS), 
                        HQ_GET_WAIT | HQ_PUT_WAIT);

    if (NULL != spQueue)
    {
        sFlag = 1;
        // Start the background monitor thread
        sulThreadId = sys_os_create_thread((void *)reconnectHandler, NULL);
        
        if (0 < sulThreadId)
        {
            getUri(cStreamUri);
            cCUrl = cStreamUri;
            cCUserName = USERNAME;
            cCPassword = PASSWORD;

            printf(">>>> INITIALIZING %d STREAMS <<<<\n", RTSP_CONN_NUM);

            for (ucIterator = 0; ucIterator < RTSP_CONN_NUM; ucIterator++)
            {
                int currentPort = INITIAL_PORT + (ucIterator * NEXT_PORT);
                
                pstHandler = new(std::nothrow) StreamHandler(currentPort, cCUrl,
                            cCUserName, cCPassword);

                if (NULL != pstHandler)
                {
                    if (pstHandler->start())
                    {
                        pCStreamers.push_back(pstHandler);
                        printf("[Init] Stream %d started on Port %d\n", 
                            ucIterator + 1, currentPort);
                    }
                    else
                    {
                        printf("[Error] Failed to start Stream %d on "
                            "Port %d\n", ucIterator + 1, currentPort);
                        delete pstHandler;
                    }
                }
            }

            printf("\n>>>> ALL STREAMS ACTIVE <<<<\n");
            printf("Commands: [p] Pause [r] Resume [c] Reconnect [q] Quit\n");
            blRunning = true;

            while (blRunning) 
            {
                printf("\nEnter Command: ");
                cChoice = getchar();

                if ('\n' == cChoice)
                {
                    continue; // Skip newline characters
                }

                if ('q' == cChoice) 
                {
                    blRunning = false;
                    continue;
                }

                lIndex = 0;

                if (1 < RTSP_CONN_NUM) 
                {
                    printf("Stream Index (1-%d): ", RTSP_CONN_NUM);

                    if (1 != scanf("%d", &lIndex)) 
                    {
                        while(getchar() != '\n');
                        continue;
                    }

                    lIndex--;
                }

                if ((0 <= lIndex) && (lIndex < pCStreamers.size())) 
                {
                    switch (cChoice) 
                    {
                        case 'p':
                            pCStreamers[lIndex]->pauseStream();
                            break;
                        case 'r':
                            pCStreamers[lIndex]->resumeStream();
                            break;
                        case 'c':
                            pCStreamers[lIndex]->reconnect();
                            break;
                        default:
                            printf("Unknown command: %c\n", cChoice);
                    }
                } 
                else 
                {
                    printf("Invalid Stream Index!\n");
                }

                while(getchar() != '\n'); 
            }

            printf("Shutting down system...\n");

            sFlag = 0;
            stExitParams.lEvent = ERROR;
            hqueue_put(spQueue, (int8*)&stExitParams);
            sys_os_wait_thread(&sulThreadId);

            for (auto pstHandler : pCStreamers)
            {
                if (NULL != pstHandler)
                {
                    pstHandler->stop();
                    delete pstHandler;
                }
            }

            pCStreamers.clear();
        }

        // Delete the event queue
        while (TRUE != hqueue_is_empty(spQueue))
        {
            hqueue_get(spQueue, (int8*)&stCleanup);
        }

        hqueue_delete(spQueue);
        spQueue =nullptr;
    }

    rtsp_parse_buf_deinit();
    sys_buf_deinit();
    log_close();
    gst_deinit();

    printf("System Shutdown Complete. All memory freed.\n");
}

//**************************.reconnectHandler.**********************************
// Purpose : Function to handle reconnections. It triggers in case of connection
//           failure
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
static void *reconnectHandler(void)
{
    EVENT_PARAMS stParams = {0};
    StreamHandler *pstTarget = NULL;

    while (sFlag)
    {
        if (hqueue_get(spQueue, (int8*)&stParams))
        {
            if ((ERROR == stParams.lEvent) || (NULL == stParams.pCClient))
            {
                break; 
            }

            pstTarget = (StreamHandler*)stParams.pCClient;

            if (RTSP_EVE_CONNFAIL == stParams.lEvent || 
                RTSP_EVE_NOSIGNAL == stParams.lEvent || 
                RTSP_EVE_NODATA   == stParams.lEvent)
            {
                printf("[Monitor] Connection lost. Triggering Reconnect "
                        "for Handler: %p\n", pstTarget);
                pstTarget->reconnect(); 
                usleep(MAX_SLEEP);
            }
        }
    }

    printf("reconnectHandler: Thread exiting cleanly.\n");
    return NULL;
}

//*****************************.clientProcess.**********************************
// Purpose : Callback function that triggers in case of notify callback. Prints 
//           the video codec
// Inputs  : lEvent - Event that triggered the callback
//           pUser - User given data(Current Stream handler)
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************
int videoCheckCodec(int32 lEvent, void *pUser)
{
    EVENT_PARAMS stParams = {0};
    int32 lVcodec = 0;
    int8 cCodecString[MSG_SIZE] = {0};

    StreamHandler *pstHandler = (StreamHandler*)pUser;
    CRtspClient *pCClient = pstHandler->get_client(); 

    printf("[CALLBACK] Event = %d | Handler = %p\r\n", lEvent, pstHandler);

    if (lEvent == RTSP_EVE_CONNSUCC)
    {
         lVcodec = pCClient->video_codec();

        if (lVcodec != VIDEO_CODEC_NONE)
        {
            cCodecString[0] = '\0';

            switch (lVcodec)
            {
                case VIDEO_CODEC_H264:
                    strcpy(cCodecString, "H264");
                    break;

                case VIDEO_CODEC_H265:
                    strcpy(cCodecString, "H265");
                    break;

                case VIDEO_CODEC_MP4:
                    strcpy(cCodecString, "MP4");
                    break;

                case VIDEO_CODEC_JPEG:
                    strcpy(cCodecString, "JPEG");
                    break;   
                    
                default:
                    // default case
                    break;
            }

            printf("video codec is %s\r\n", cCodecString);
        }
    }

    stParams.lEvent = lEvent;
    stParams.pCClient = (void*)pstHandler; 

    if (FALSE == hqueue_put(spQueue, (int8*) &stParams))
    {
        printf("hqueue_put failed\r\n");
    }

    return 0;
}

// EOF