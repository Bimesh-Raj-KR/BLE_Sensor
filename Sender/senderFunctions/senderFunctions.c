//****************************** Sender Functions ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : senderFunctions.c
// Summary : Implements sender gstreamer pipeline setup and initialization
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 27/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "senderFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************
static void senderPadAdded(GstElement *pstElement, GstPad *pstPad, 
                           gpointer pData);
static bool senderLinkDemux(GstPad *pstSourcePad, GstElement *pstQueue);

//*********************************.senderInit.*********************************
// Purpose : Function to initialize sender pipeline
// Inputs  : pcFilePath - Pointer to the file path
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool senderInit(int8* pcFilePath)
{
    bool blCheck = false;
    GstElement *pstPipeline = NULL;
    GstElement *pstSource = NULL;
    GstElement *pstDemux = NULL;
    GstElement *pstQueue = NULL; 
    GstElement *pstParser = NULL; 
    GstElement *pstPayloader = NULL;
    GstElement *pstSink = NULL;
    GstBus *pstBus = NULL;
    GstMessage *pstMessage = NULL;

    if (NULL != pcFilePath)
    { 
        gst_init(NULL, NULL);

        // Create pipeline elements
        pstPipeline = gst_pipeline_new("mp4-sender");
        pstSource = gst_element_factory_make("filesrc", "source");
        pstDemux = gst_element_factory_make("qtdemux", "demuxer");
        pstQueue = gst_element_factory_make("queue", "Timing-queue");
        pstParser = gst_element_factory_make("h264parse", "parser");
        pstPayloader = gst_element_factory_make("rtph264pay", "payloader");
        pstSink = gst_element_factory_make("udpsink", "sink");

        if ((NULL != pstPipeline) && (NULL != pstSource) && (NULL != pstParser) 
            && (NULL != pstPayloader) && (NULL != pstSink)
            && (NULL != pstDemux) && (NULL != pstQueue))
        {
            // Set element properties
            g_object_set(G_OBJECT(pstPipeline), "async-handling", TRUE, NULL);
            g_object_set(G_OBJECT(pstSource), "location", pcFilePath, NULL);
            g_object_set(G_OBJECT(pstPayloader), "config-interval", 1, NULL);
            g_object_set(G_OBJECT(pstSink), "host", HOST_IP, NULL);
            g_object_set(G_OBJECT(pstSink), "port", PORT, NULL);
            g_object_set(G_OBJECT(pstSink), "sync", TRUE, NULL);

            // Add elements to the pipeline
            gst_bin_add_many(GST_BIN(pstPipeline), pstSource, pstDemux,
                            pstQueue, pstParser, pstPayloader, pstSink, NULL);

            // Link the elements together
            gst_element_link(pstSource, pstDemux);
            gst_element_link_many(pstQueue, pstParser, pstPayloader, 
                                  pstSink, NULL);
            g_signal_connect(pstDemux, "pad-added", 
                             G_CALLBACK(senderPadAdded), pstQueue);

            // Start playing the pipeline
            gst_element_set_state(pstPipeline, GST_STATE_PLAYING);
            pstBus = gst_element_get_bus(pstPipeline);
            pstMessage = gst_bus_timed_pop_filtered(pstBus, 
                GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

            if (NULL != pstMessage)
            {
                // Free the message
                gst_message_unref(pstMessage);
                blCheck = true;
            }
            else
            {
                g_printerr("Message is NULL.\n");
            }

            gst_object_unref(pstBus);
            gst_element_set_state(pstPipeline, GST_STATE_NULL);
            gst_object_unref(pstPipeline);
        }
        else
        {
            g_printerr("Not all elements could be created.\n");
        }
    }
    else
    {
        g_printerr("File path is NULL.\n");
    }

    return blCheck;
}

//*******************************.senderPadAdded.*******************************
// Purpose : Function to handle dynamic pad addition from demuxer
// Inputs  : pstElement - Pointer to the element
//           pstPad     - Pointer to the newly created pad
//           pData      - Pointer to user data (queue element)
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
static void senderPadAdded(GstElement *pstElement, GstPad *pstPad, 
                           gpointer pData)
{
    GstElement *pstQueue = NULL;
    GstCaps *pstCaps = NULL;
    GstStructure *pstStruct = NULL;
    const gchar *pcName = NULL;
    
    pstQueue = (GstElement*)pData;
    (void)pstElement;

    pstCaps = gst_pad_get_current_caps(pstPad);
    pstStruct = gst_caps_get_structure(pstCaps, 0);
    pcName = gst_structure_get_name(pstStruct);

    if (TRUE == g_str_has_prefix(pcName, "video/x-h264")) 
    {
        if (true != senderLinkDemux(pstPad, pstQueue))
        {
            g_printerr("Dynamic link from Demuxer to Queue failed.\n");
        }
    }

    if (NULL != pstCaps) 
    {
        gst_caps_unref(pstCaps);
    }
}

//****************************.senderLinkDemux.*********************************
// Purpose : Function to link demuxer pad to queue element
// Inputs  : pstSourcePad - Pointer to the source pad
//           pstQueue     - Pointer to the queue element
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool senderLinkDemux(GstPad *pstSourcePad, GstElement *pstQueue) 
{
    bool blCheck = false;
    GstPad *pstSinkPad = NULL;

    if ((NULL != pstSourcePad) && (NULL != pstQueue)) 
    {
        pstSinkPad = gst_element_get_static_pad(pstQueue, "sink");

        if (NULL != pstSinkPad) 
        {
            if (TRUE != gst_pad_is_linked(pstSinkPad)) 
            {
                if (GST_PAD_LINK_OK == gst_pad_link(pstSourcePad, 
                                                    pstSinkPad)) 
                {
                    blCheck = true;
                }
            }

            gst_object_unref(pstSinkPad);
        }
    }

    if (true != blCheck) 
    {
        g_printerr("Demuxer to Queue link failed.\n");
    }

    return blCheck;
}

// EOF