//****************************** Sender Functions ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : senderFunctions.c
// Summary : Implements sender gstreamer pipeline setup and initialization
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 21/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "senderFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//*********************************.senderInit.********************************
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
    GstElement *pstParser = NULL; 
    GstElement *pstPayloader = NULL;
    GstElement *pstSink = NULL;
    GstBus *pstBus = NULL;
    GstMessage *pstMessage = NULL;

    if (NULL != pcFilePath)
    { 
        gst_init(NULL, NULL);

        // Create pipeline elements
        pstPipeline = gst_pipeline_new("file-sender");
        pstSource = gst_element_factory_make("filesrc", "source");
        pstParser = gst_element_factory_make("mpegaudioparse", "parser");
        pstPayloader = gst_element_factory_make("rtpgstpay", "payloader");
        pstSink = gst_element_factory_make("udpsink", "sink");

        if ((NULL != pstPipeline) && (NULL != pstSource) &&
            (NULL != pstParser) && (NULL != pstPayloader) && (NULL != pstSink))
        {
            // Set element properties
            g_object_set(G_OBJECT(pstSource), "location", pcFilePath, NULL);
            g_object_set(G_OBJECT(pstPayloader), "ssrc", SSRC_VALUE, NULL);
            g_object_set(G_OBJECT(pstSink), "host", HOST_IP, NULL);
            g_object_set(G_OBJECT(pstSink), "port", PORT, NULL);
            g_object_set(G_OBJECT(pstSink), "sync", FALSE, NULL);

            // Add elements to the pipeline
            gst_bin_add_many(GST_BIN(pstPipeline), pstSource, pstParser,
                            pstPayloader, pstSink, NULL);

            // Link the elements together
            if (TRUE == gst_element_link_many(pstSource, pstParser,
                                            pstPayloader, pstSink, NULL))
            {
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
    }
    else
    {
        g_printerr("File path is NULL.\n");
    }

    return blCheck;
}

// EOF