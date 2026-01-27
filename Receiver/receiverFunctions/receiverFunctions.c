//***************************** Receiver Functions *****************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : receiverFunctions.c
// Summary : Implements receiver gstreamer pipeline setup and initialization
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 27/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "receiverFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//*********************************.receiverInit.********************************
// Purpose : Function to initialize receiver pipeline
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool receiverInit(void)
{
    bool blCheck = false;
    GstElement *pstPipeline = NULL;
    GstElement *pstSource = NULL;
    GstElement *pstJitterBuffer = NULL;
    GstElement *pstDepayloader = NULL;
    GstElement *pstParser = NULL;
    GstElement *pstMuxer = NULL;
    GstElement *pstSink = NULL;
    GstBus *pstBus = NULL;
    GstMessage *pstMessage = NULL;
    GstCaps *pstCaps = NULL;

    gst_init(NULL, NULL);

    // Create pipeline elements
    pstPipeline = gst_pipeline_new("mkv-receiver");
    pstSource = gst_element_factory_make("udpsrc", "udp-source");
    pstJitterBuffer = gst_element_factory_make("rtpjitterbuffer", 
                                "jitter-buffer");
    pstDepayloader = gst_element_factory_make("rtph264depay", "depayloader");
    pstParser = gst_element_factory_make("h264parse", "parser");
    pstMuxer = gst_element_factory_make("matroskamux", "muxer");
    pstSink = gst_element_factory_make("filesink", "file-sink");

    if ((NULL != pstPipeline) && (NULL != pstSource) && 
        (NULL != pstJitterBuffer) && (NULL != pstDepayloader) && 
        (NULL != pstParser) && (NULL != pstMuxer) && (NULL != pstSink))
    {
        // Set element properties
        g_object_set(G_OBJECT(pstSource), "port", PORT, NULL);
        g_object_set(G_OBJECT(pstJitterBuffer), "latency", MAX_LATENCY, NULL);
        g_object_set(G_OBJECT(pstSink), "location", FILE_NAME, NULL);
        pstCaps = gst_caps_from_string("application/x-rtp, media=video, "
                                       "clock-rate=90000, encoding-name=H264");
        g_object_set(G_OBJECT(pstSource), "caps", pstCaps, NULL);
        gst_caps_unref(pstCaps);

        // Add elements to the pipeline
        gst_bin_add_many(GST_BIN(pstPipeline), pstSource, pstJitterBuffer, 
                         pstDepayloader, pstParser, pstMuxer, pstSink, NULL);

        // Link the elements together
        if (TRUE == gst_element_link_many(pstSource, pstJitterBuffer, 
                                        pstDepayloader, pstParser, pstMuxer, 
                                        pstSink, NULL))
        {
            gst_element_set_state(pstPipeline, GST_STATE_PLAYING);
            pstBus = gst_element_get_bus(pstPipeline);
            pstMessage = gst_bus_timed_pop_filtered(pstBus, GST_CLOCK_TIME_NONE,
                                        GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

            if (NULL != pstMessage)
            {
                gst_message_unref(pstMessage);
                blCheck = true;
            }
            else
            {
                g_printerr("Message is NULL.\n");
            }

            gst_object_unref(pstBus);
            gst_element_set_state(pstPipeline, GST_STATE_NULL);
        }
        else
        {
            g_printerr("Elements could not be linked.\n");
        }

        gst_object_unref(pstPipeline);
    }
    else
    {
        g_printerr("Not all elements could be created.\n");
    }

    return blCheck;
}

// EOF