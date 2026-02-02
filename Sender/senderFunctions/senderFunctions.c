//****************************** Sender Functions ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : senderFunctions.c
// Summary : Implements sender gstreamer pipeline setup and initialization
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 28/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "senderFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************
static void* sourceInit(void* pArg);
static void* bridgeInit(void* pArg);

//*********************************.senderInit.*********************************
// Purpose : Function to initialize sender pipelines
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void senderInit(void)
{
    pthread_t ulSource = 0;
    pthread_t ulBridge = 0;

    gst_init(NULL, NULL);

    //Clean up old sockets
    unlink(SOURCE_SOCKET);

    // Create threads
    pthread_create(&ulSource, NULL, sourceInit, NULL);
    usleep(THREAD_DELAY);
    pthread_create(&ulBridge, NULL, bridgeInit, NULL);

    // Join threads
    pthread_join(ulSource, NULL);
    pthread_join(ulBridge, NULL);
}

//**********************************.sourceInit.********************************
// Purpose : Function to initialize source pipeline
// Inputs  : None
// Outputs : None
// Return  : NULL
// Notes   : None
//******************************************************************************
static void* sourceInit(void* pArg)
{
    GstElement *pstPipeline = NULL;
    GstElement *pstSource = NULL;
    GstElement *pstParse = NULL;
    GstElement *pstDecode = NULL;
    GstElement *pstConvert = NULL;
    GstElement *pstEnforce = NULL;
    GstElement *pstSink = NULL;
    GstCaps *pstCaps = NULL;
    GstBus *pstBus = NULL;
    GstMessage *pstMessage = NULL;

    // Silence warnings
    (void)pArg;

    // Setup elements
    pstPipeline = gst_pipeline_new("file-to-shm");
    pstSource = gst_element_factory_make("filesrc", "file-source");
    pstParse = gst_element_factory_make("mpegaudioparse", "parser");
    pstDecode = gst_element_factory_make("mpg123audiodec", "decoder");
    pstConvert = gst_element_factory_make("audioconvert", "converter");
    pstEnforce = gst_element_factory_make("capsfilter", "enforcer");
    pstSink = gst_element_factory_make("shmsink", "shm-sink");

    if ((NULL != pstPipeline) && (NULL != pstSource) && (NULL != pstParse) &&
        (NULL != pstDecode) && (NULL != pstConvert) && (NULL != pstEnforce) && 
        (NULL != pstSink))
    {
        // Configure Elements
        g_object_set(G_OBJECT(pstSource), "location", FILE_NAME, NULL);
        pstCaps = gst_caps_from_string("audio/x-raw,format=S16LE,"
                                "channels=2,rate=44100,layout=interleaved");
        g_object_set(G_OBJECT(pstEnforce), "caps", pstCaps, NULL);
        gst_caps_unref(pstCaps);
        g_object_set(G_OBJECT(pstSink), "socket-path", SOURCE_SOCKET, NULL);
        g_object_set(G_OBJECT(pstSink), "shm-size", MEM_SIZE, NULL);
        g_object_set(G_OBJECT(pstSink), "wait-for-connection", FALSE, NULL);
        gst_bin_add_many(GST_BIN(pstPipeline), pstSource, pstParse, 
                         pstDecode, pstConvert, pstEnforce, pstSink, NULL);

        if (TRUE == gst_element_link_many(pstSource, pstParse, pstDecode, 
                                    pstConvert, pstEnforce, pstSink, NULL))
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

//*****************************.bridgeInit.*************************************
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
    GstElement *pstEnforce = NULL;
    GstElement *pstConvert = NULL;
    GstElement *pstPayload = NULL;
    GstElement *pstSink = NULL;
    GstCaps *pstCaps = NULL;
    GstBus *pstBus = NULL;
    GstMessage *pstMessage = NULL;

    // Silence warnings
    (void)pArg;

    pstPipeline = gst_pipeline_new("shm-to-udp");
    pstSource = gst_element_factory_make("shmsrc", "shm-source");
    pstEnforce = gst_element_factory_make("capsfilter", "enforcer");
    pstConvert = gst_element_factory_make("audioconvert", "converter");
    pstPayload = gst_element_factory_make("rtpL16pay", "payloader");
    pstSink = gst_element_factory_make("udpsink", "udp-sink");

    if ((NULL != pstPipeline) && (NULL != pstSource) && (NULL != pstEnforce) && 
        (NULL != pstConvert) && (NULL != pstPayload) && (NULL != pstSink))
    {

        g_object_set(G_OBJECT(pstSource), "socket-path", SOURCE_SOCKET, NULL);
        g_object_set(G_OBJECT(pstSource), "do-timestamp", TRUE, NULL);
        
        pstCaps = gst_caps_from_string("audio/x-raw,format=S16LE,channels=2,"
                                        "rate=44100,layout=interleaved");
        g_object_set(G_OBJECT(pstEnforce), "caps", pstCaps, NULL);
        gst_caps_unref(pstCaps);

        g_object_set(G_OBJECT(pstSink), "host", HOST_IP, NULL);
        g_object_set(G_OBJECT(pstSink), "port", PORT, NULL);
        g_object_set(G_OBJECT(pstSink), "sync", FALSE, NULL);

        gst_bin_add_many(GST_BIN(pstPipeline), pstSource, pstEnforce, 
                            pstConvert, pstPayload, pstSink, NULL);

        if (TRUE == gst_element_link_many(pstSource, pstEnforce, pstConvert, 
                                          pstPayload, pstSink, NULL))
        {
            gst_element_set_state(pstPipeline, GST_STATE_PLAYING);
            pstBus = gst_element_get_bus(pstPipeline);
            pstMessage = gst_bus_timed_pop_filtered(pstBus, GST_CLOCK_TIME_NONE,
                                        GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

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