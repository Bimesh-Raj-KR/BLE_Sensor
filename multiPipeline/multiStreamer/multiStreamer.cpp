//******************************** Multi Streamer ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : multiStreamer.cpp
// Summary : Foward the received stream to multiple IPs using multiple pipelines
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 16/Mar/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "multiStreamer.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//******************************.FileStreamer.**********************************
// Purpose : FileStreamer constructor. Initialize streamer with a specific file
// Inputs  : cFileName - Path to the file
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
FileStreamer::FileStreamer(const std::string& cFileName) 
{
    m_cFileName = cFileName;
    m_blIsRunning = false;
    m_pvstPipelines.clear();
}

//******************************.~FileStreamer.*********************************
// Purpose : FileStreamer destructor. Stops streaming
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
FileStreamer::~FileStreamer() 
{
    if (true == m_blIsRunning) 
    {
        stop();
    }

    printf("[Cleanup] FileStreamer object destroyed.\n");
}

//***********************************.start.************************************
// Purpose : Creates a separate, independent pipeline for each target IP
// Inputs  : None
// Outputs : None
// Return  : true if at least one pipeline started, else false
// Notes   : None
//******************************************************************************
bool FileStreamer::start() 
{
    bool blCheck = false;
    size_t ulIterator = 0;
    GstElement *pstPipe  = NULL;
    GstElement *pstSrc   = NULL;
    GstElement *pstParse = NULL;
    GstElement *pstPay   = NULL;
    GstElement *pstSink  = NULL;
    std::string cPipeName = "";

    for (ulIterator = 0; ulIterator < m_cTargetIps.size(); ulIterator++)
    {
        // Create a unique name for each pipeline
        cPipeName = "pipe_" + std::to_string(ulIterator);
        pstPipe = gst_pipeline_new(cPipeName.c_str());
        pstSrc   = gst_element_factory_make("filesrc", NULL);
        pstParse = gst_element_factory_make("h264parse", NULL);
        pstPay   = gst_element_factory_make("rtph264pay", NULL);
        pstSink  = gst_element_factory_make("udpsink", NULL);

        if ((NULL != pstPipe) && (NULL != pstSrc) && (NULL != pstParse) && 
            (NULL != pstPay) && (NULL != pstSink))
        {
            // Configure elements
            g_object_set(pstSrc, "location", m_cFileName.c_str(), NULL);
            g_object_set(pstPay, "config-interval", 1, NULL);
            g_object_set(pstSink, "host", m_cTargetIps[ulIterator].c_str(), 
                         "port", 5000, "sync", TRUE, NULL);

            // Add and Link
            gst_bin_add_many(GST_BIN(pstPipe), pstSrc, pstParse, 
                             pstPay, pstSink, NULL);
            gst_element_link_many(pstSrc, pstParse, pstPay, pstSink, NULL);

            // Start this specific pipeline
            if (GST_STATE_CHANGE_FAILURE != gst_element_set_state(pstPipe, 
                GST_STATE_PLAYING))
            {
                m_pvstPipelines.push_back(pstPipe);
                printf("[Consumer] Pipeline for IP %s started.\n", 
                        m_cTargetIps[ulIterator].c_str());
                blCheck = true;
            }
            else
            {
                gst_object_unref(pstPipe);
            }
        }
    }

    m_blIsRunning = blCheck;
    return blCheck;
}

//***********************************.stop.*************************************
// Purpose : Function to stop all independent streaming pipelines
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void FileStreamer::stop() 
{
    m_blIsRunning = false;
    GstBus *pstBus = NULL;
    GstMessage *pstMsg = NULL;

    for (GstElement *pstPipe : m_pvstPipelines)
    {
        if (NULL != pstPipe)
        {
            gst_element_send_event(pstPipe, gst_event_new_eos());
            pstBus = gst_element_get_bus(pstPipe);

            if (NULL != pstBus)
            {
                pstMsg = gst_bus_timed_pop_filtered(pstBus, GST_SECOND, 
                         (GstMessageType)(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));

                if (NULL != pstMsg)
                {
                    gst_message_unref(pstMsg);
                }

                gst_object_unref(pstBus);
                pstBus = NULL;
            }

            gst_element_set_state(pstPipe, GST_STATE_NULL);
            gst_object_unref(pstPipe);
        }
    }

    m_pvstPipelines.clear();
    printf("[Consumer] All streaming pipelines stopped and cleaned up.\n");
}


//**************************.setTargetIps.**************************************
// Purpose : Sets the list of destination IP addresses for streaming
// Inputs  : cTargetIps - A vector of strings containing the IP addresses
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void FileStreamer::setTargetIps(const std::vector<std::string>& cTargetIps)
{
    m_cTargetIps.clear();
    m_cTargetIps = cTargetIps;
}

//**************************.videoFileStreamerInit.*****************************
// Purpose : System level entry point for the file-to-UDP streaming app
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void videoFileStreamerInit(void)
{
    bool blLoop = true;
    int8 cChoice = 0;
    FileStreamer *pCStreamer = NULL;
    std::string cFileName = "";
    std::vector<std::string> cTargetIps = {};

    gst_init(NULL, NULL);

    printf("Enter the file path to stream\n");
    std::getline(std::cin, cFileName);
    cTargetIps = {"192.168.14.25", "192.168.14.35"};
    printf(">>>> INITIALIZING FILE CONSUMER SYSTEM <<<<\n");
    pCStreamer = new (std::nothrow) FileStreamer(cFileName);

    if (NULL != pCStreamer)
    {
        pCStreamer->setTargetIps(cTargetIps);

        if (pCStreamer->start())
        {
            printf("[Init] Successfully started streaming from: %s\n", 
                        cFileName.c_str());
            printf("Commands: [q] Quit\n");

            while (blLoop)
            {
                cChoice = getchar();

                if ('q' == cChoice)
                {
                    blLoop = false;
                }

                usleep(MAX_SLEEP);
            }

            pCStreamer->stop();
        }
        else
        {
            printf("[Error] Failed to start file streamer.\n");
        }

        delete pCStreamer;
    }

    gst_deinit();
    printf("Consumer System Shutdown Complete.\n");
}

// EOF