//***************************** Receiver Functions *****************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : receiverFunctions.c
// Summary : Implements receiver gstreamer pipeline setup and initialization
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 30/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "receiverFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static int8 *spcSharedMem = NULL;
static uint32 sulCurrentOffset = 0;
static pthread_mutex_t sstDataMutex = PTHREAD_MUTEX_INITIALIZER;

//***************************** Local Functions ********************************
static void* createThreadFunc(void* pulSize);
static void* receiveThreadFunc(void* pulSize);
static bool receiverInit(uint32* pulSize);
static bool assignSharedMemory(uint32 ulSize);
static bool detachSharedMemory(uint32 ulSize);
static GstFlowReturn receiverFillMemory(GstElement *pstSink, gpointer pData);

//**************************.receiverThreadCreate.******************************
// Purpose : Function to create threads for receiver operations
// Inputs  : ulSize - Size of the SHM
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void receiverThreadCreate(uint32 ulSize)
{
    pthread_t ulCreator = 0;
    pthread_t ulSender = 0;

    // Initialize Mutex
    pthread_mutex_init(&sstDataMutex, NULL);

    /* Create threads */
    pthread_create(&ulCreator, NULL, createThreadFunc, &ulSize);
    pthread_create(&ulSender, NULL, receiveThreadFunc, &ulSize);

    /* Wait for execution to finish */
    pthread_join(ulCreator, NULL);
    pthread_join(ulSender, NULL);

    // Final Cleanup
    pthread_mutex_destroy(&sstDataMutex);
}

//*****************************.createThreadFunc.*******************************
// Purpose : Function to create SHM create thread
// Inputs  : pSize - Size of the SHM
// Outputs : None
// Return  : NULL
// Notes   : None
//******************************************************************************
static void* createThreadFunc(void* pSize)
{
    pthread_mutex_lock(&sstDataMutex);

    if (true == assignSharedMemory(*(uint32*)pSize))
    {
        detachSharedMemory(*(uint32*)pSize);
    }

    // Unlock to allow Sender thread to proceed
    pthread_mutex_unlock(&sstDataMutex);

    return NULL;
}

//****************************.receiveThreadFunc.*******************************
// Purpose : Function to create receiver initialization thread
// Inputs  : pSize - Size of the SHM
// Outputs : None
// Return  : NULL
// Notes   : None
//******************************************************************************
static void* receiveThreadFunc(void* pSize)
{
    printf("Press Enter to start the Receiver...\n");
    getchar(); 

    if (0 < *(uint32*)pSize)
    {
        pthread_mutex_lock(&sstDataMutex);

        if (true == assignSharedMemory(*(uint32*)pSize))
        {
            receiverInit((uint32*)pSize);
        }

        detachSharedMemory(*(uint32*)pSize);
        pthread_mutex_unlock(&sstDataMutex);
    }

    return NULL;
}

//*********************************.receiverInit.*******************************
// Purpose : Function to initialize receiver pipeline
// Inputs  : pulSize - Size of the SHM
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool receiverInit(uint32* pulSize)
{
    bool blCheck = false;
    GstElement *pstPipeline = NULL;
    GstElement *pstSource = NULL;
    GstElement *pstJitterBuffer = NULL;
    GstElement *pstDepayloader = NULL;
    GstElement *pstSink = NULL;
    GstBus *pstBus = NULL;
    GstMessage *pstMessage = NULL;
    GstCaps *pstCaps = NULL;

    gst_init(NULL, NULL);

    // Create pipeline elements
    pstPipeline = gst_pipeline_new("shm-receiver");
    pstSource = gst_element_factory_make("udpsrc", "udp-source");
    pstJitterBuffer = gst_element_factory_make("rtpjitterbuffer", 
                                "jitter-buffer");
    pstDepayloader = gst_element_factory_make("rtpgstdepay", "depayloader");
    pstSink = gst_element_factory_make("appsink", "sink");

    if ((NULL != pstPipeline) && (NULL != pstSource) && 
        (NULL != pstJitterBuffer) && (NULL != pstDepayloader) && 
        (NULL != pstSink))
    {
        // Set element properties
        g_object_set(G_OBJECT(pstSource), "port", PORT, NULL);
        g_object_set(G_OBJECT(pstSink), "emit-signals", TRUE, NULL);
        g_object_set(G_OBJECT(pstSink), "sync", FALSE, NULL);
        g_signal_connect(pstSink, "new-sample", 
                    G_CALLBACK(receiverFillMemory), pulSize);

        pstCaps = gst_caps_from_string("application/x-rtp, media=(string)"
                                        "application, clock-rate=(int)90000, "
                                        "encoding-name=(string)X-GST");
        g_object_set(G_OBJECT(pstSource), "caps", pstCaps, NULL);

        // Add elements to the pipeline
        gst_bin_add_many(GST_BIN(pstPipeline), pstSource, 
                         pstJitterBuffer, pstDepayloader, pstSink, NULL);

        if (TRUE == gst_element_link_many(pstSource, pstJitterBuffer, 
                                        pstDepayloader, pstSink, NULL))
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

        if (NULL != pstCaps)
        {
            gst_caps_unref(pstCaps);
        }

        gst_object_unref(pstPipeline);
    }
    else
    {
        g_printerr("Not all elements could be created.\n");
    }

    return blCheck;
}

//***************************.assignSharedMemory.*******************************
// Purpose : Function to assign SHM
// Inputs  : ulSize - Size of the SHM
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool assignSharedMemory(uint32 ulSize)
{
    bool blCheck = false;
    key_t lKey = 0;
    int32 lMemoryId = 0;
    int8 *pMemory = NULL;

    sulCurrentOffset = 0;

    if (true == assignMemory(&lKey, SHM_RECV_FILE))
    {
        if (true == getMemoryId(&lMemoryId, lKey, ulSize))
        {
            if (true == attachMemory(&pMemory, lMemoryId))
            {
                spcSharedMem = (int8*)pMemory;
                blCheck = true;
            }
        }
    }
    else
    {
        printf("File might not be created\n");
    }

    if (true != blCheck)
    {
        printf("Error in creating shared memory\n");
    }

    return blCheck;
}

//****************************.detachSharedMemory.******************************
// Purpose : Function to detach shared memory
// Inputs  : ulSize - Size of the SHM
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
static bool detachSharedMemory(uint32 ulSize)
{
    bool blCheck = false;
    key_t lKey = 0;
    int32 lMemoryId = 0;
    int8 *pMemory = NULL;

    if (true == assignMemory(&lKey, SHM_RECV_FILE))
    {
        if (true == getMemoryId(&lMemoryId, lKey, ulSize))
        {
            if (true == attachMemory(&pMemory, lMemoryId))
            {
                detachMemory(pMemory);
                blCheck = true;
            }
        }
    }

    if (true != blCheck)
    {
        printf("Failed to detach shared memory\n");
    }

    return blCheck;
}

//****************************.receiverFillMemory.******************************
// Purpose : Callback function that triggers when data arrives at udpsrc
// Inputs  : pstSink - sink element pointer
//           pData - Size of SHM
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static GstFlowReturn receiverFillMemory(GstElement *pstSink, gpointer pData)
{
    GstSample *pstSample = NULL;
    GstBuffer *pstBuffer = NULL;
    GstMapInfo stMap = {0};
    GstFlowReturn eStatus = 0;
    uint32 ulSize = 0;

    eStatus = GST_FLOW_ERROR;
    ulSize = *(uint32*)pData;

    if (NULL != pstSink)
    {
        g_signal_emit_by_name(pstSink, "pull-sample", &pstSample);

        if (NULL != pstSample)
        {
            pstBuffer = gst_sample_get_buffer(pstSample);

            if (TRUE == gst_buffer_map(pstBuffer, &stMap, GST_MAP_READ))
            {
                if (sulCurrentOffset + stMap.size <= ulSize)
                {
                    memcpy(spcSharedMem + sulCurrentOffset, 
                            stMap.data, stMap.size);
                    sulCurrentOffset += stMap.size;
                }

                gst_buffer_unmap(pstBuffer, &stMap);
            }

            gst_sample_unref(pstSample);
            eStatus = GST_FLOW_OK;
        }
    }

    return eStatus;
}

// EOF
