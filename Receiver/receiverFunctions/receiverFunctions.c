//***************************** Receiver Functions *****************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : receiverFunctions.c
// Summary : Implements receiver gstreamer pipeline setup and initialization
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 28/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "receiverFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************
static void* bridgeInit(void* pArg);
static void* consumerInit(void* pArg);

//*********************************.receiverInit.*******************************
// Purpose : Function to initialize receiver pipelines
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void receiverInit(void)
{
    pthread_t ulBridge = 0;
    pthread_t ulConsumer = 0;

    gst_init(NULL, NULL);
    unlink(TARGET_SOCKET);

    // Create threads
    pthread_create(&ulBridge, NULL, bridgeInit, NULL);
    usleep(THREAD_DELAY);
    pthread_create(&ulConsumer, NULL, consumerInit, NULL);

    // Join threads
    pthread_join(ulBridge, NULL);
    pthread_join(ulConsumer, NULL);
}

//*********************************.receiverInit.*******************************
// Purpose : Function to initialize bridge pipeline
// Inputs  : None
// Outputs : None
// Return  : NULL
// Notes   : None
//******************************************************************************
static void* bridgeInit(void* pArg)
{
    GstElement *pstPipeline = NULL;
    GstElement *pstSource = NULL;
    GstElement *pstDepay = NULL;
    GstElement *pstEnforce = NULL;
    GstElement *pstSink = NULL;
    GstCaps *pstCaps = NULL;
    GstBus *pstBus = NULL;
    GstMessage *pstMessage = NULL;

    // Silence warnings
    (void)pArg;

    // Setup elements
    pstPipeline = gst_pipeline_new("udp-to-shm");
    pstSource = gst_element_factory_make("udpsrc", "udp-source");
    pstDepay = gst_element_factory_make("rtpL16depay", "depayloader");
    pstEnforce = gst_element_factory_make("capsfilter", "enforcer");
    pstSink = gst_element_factory_make("shmsink", "shm-sink");

    if ((NULL != pstPipeline) && (NULL != pstSource) && (NULL != pstDepay) && 
        (NULL != pstEnforce) && (NULL != pstSink))
    {
        g_object_set(G_OBJECT(pstSource), "port", PORT, NULL);

        // Define incoming RTP caps
        pstCaps = gst_caps_from_string("application/x-rtp,media=audio," 
                "clock-rate=44100,encoding-name=L16,channels=2,payload=96");
        g_object_set(G_OBJECT(pstSource), "caps", pstCaps, NULL);
        gst_caps_unref(pstCaps);

        // Define output raw caps for SHM (S16BE is native for RTP L16)
        pstCaps = gst_caps_from_string("audio/x-raw,format=S16BE,channels=2,"
                                       "rate=44100,layout=interleaved");
        g_object_set(G_OBJECT(pstEnforce), "caps", pstCaps, NULL);
        gst_caps_unref(pstCaps);
        g_object_set(G_OBJECT(pstSink), "socket-path", TARGET_SOCKET, NULL);
        g_object_set(G_OBJECT(pstSink), "shm-size", MEM_SIZE, NULL);
        g_object_set(G_OBJECT(pstSink), "wait-for-connection", FALSE, NULL);
        gst_bin_add_many(GST_BIN(pstPipeline), pstSource, pstDepay, 
                         pstEnforce, pstSink, NULL);

        if (TRUE == gst_element_link_many(pstSource, pstDepay, pstEnforce, 
                                          pstSink, NULL))
        {
            gst_element_set_state(pstPipeline, GST_STATE_PLAYING);
            pstBus = gst_element_get_bus(pstPipeline);
            pstMessage = gst_bus_timed_pop_filtered(pstBus, GST_CLOCK_TIME_NONE,
                                        GST_MESSAGE_EOS | GST_MESSAGE_ERROR);

            if (NULL != pstMessage)
            {
                if (GST_MESSAGE_EOS == GST_MESSAGE_TYPE(pstMessage))
                {
                    g_usleep(EOS_WAIT);
                }

                gst_message_unref(pstMessage);
            }

            gst_object_unref(pstBus);
        }
    }

    gst_element_set_state(pstPipeline, GST_STATE_NULL);
    gst_object_unref(pstPipeline);

    return NULL;
}

//*********************************.consumerInit.*******************************
// Purpose : Function to initialize consumer pipeline
// Inputs  : None
// Outputs : None
// Return  : NULL
// Notes   : None
//******************************************************************************
static void* consumerInit(void* pArg)
{
    GstElement *pstPipeline = NULL;
    GstElement *pstSource = NULL;
    GstElement *pstEnforce = NULL;
    GstElement *pstConvert = NULL;
    GstElement *pstQueue = NULL;
    GstElement *pstEncode = NULL;
    GstElement *pstSink = NULL;
    GstCaps *pstCaps = NULL;
    GstBus *pstBus = NULL;
    GstMessage *pstMessage = NULL;

    // Silence warnings
    (void)pArg;

    pstPipeline = gst_pipeline_new("shm-to-wav");
    pstSource = gst_element_factory_make("shmsrc", "shm-source");
    pstEnforce = gst_element_factory_make("capsfilter", "enforcer");
    pstConvert = gst_element_factory_make("audioconvert", "converter");
    pstQueue = gst_element_factory_make("queue", "buffer-queue");
    pstEncode = gst_element_factory_make("wavenc", "encoder");
    pstSink = gst_element_factory_make("filesink", "file-sink");

    if ((NULL != pstPipeline) && (NULL != pstSource) && (NULL != pstEnforce) && 
        (NULL != pstConvert)&& (NULL != pstQueue) && (NULL != pstEncode) && 
        (NULL != pstSink))
    {
        g_object_set(G_OBJECT(pstSource), "socket-path", TARGET_SOCKET, NULL);
        g_object_set(G_OBJECT(pstSource), "is-live", TRUE, NULL);
        g_object_set(G_OBJECT(pstSource), "do-timestamp", TRUE, NULL);

        // Define input caps from SHM
        pstCaps = gst_caps_from_string("audio/x-raw,format=S16BE,channels=2,"
                                       "rate=44100,layout=interleaved");
        g_object_set(G_OBJECT(pstEnforce), "caps", pstCaps, NULL);
        gst_caps_unref(pstCaps);
        g_object_set(G_OBJECT(pstSink), "location", FILE_NAME, NULL);
        gst_bin_add_many(GST_BIN(pstPipeline), pstSource, pstEnforce, 
                            pstConvert, pstQueue, pstEncode, pstSink, NULL);

        if (TRUE == gst_element_link_many(pstSource, pstEnforce, pstConvert, 
                                          pstQueue, pstEncode, pstSink, NULL))
        {
            gst_element_set_state(pstPipeline, GST_STATE_PLAYING);
            pstBus = gst_element_get_bus(pstPipeline);
            pstMessage = gst_bus_timed_pop_filtered(pstBus, GST_CLOCK_TIME_NONE, 
                                        GST_MESSAGE_EOS | GST_MESSAGE_ERROR);
            if (NULL != pstMessage)
            {
                gst_message_unref(pstMessage);
            }

            gst_object_unref(pstBus);
        }
    }

    gst_element_set_state(pstPipeline, GST_STATE_NULL);
    gst_object_unref(pstPipeline);

    return NULL;
}

// EOF