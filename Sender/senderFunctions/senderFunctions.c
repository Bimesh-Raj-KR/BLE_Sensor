//****************************** Sender Functions ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : senderFunctions.c
// Summary : Implements sender gstreamer pipeline setup and initialization
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 30/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "senderFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static int8 *spcSharedMem = NULL;
static pthread_mutex_t sstDataMutex = PTHREAD_MUTEX_INITIALIZER;

//***************************** Local Functions ********************************
static void* writerThreadFunc(void* pParams);
static void* senderThreadFunc(void* pParams);
static bool senderInit(uint32 ulSize);
static bool assignSharedMemory(uint32 ulSize);
static bool detachSharedMemory(uint32 ulSize);

//*****************************.senderThreadCreate.*****************************
// Purpose : Function to create threads for sender operations
// Inputs  : pcFilePath - Pointer to the file path
//           ulSize - Size of the SHM
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool senderThreadCreate(int8* pcFilePath, uint32 ulSize)
{
    bool blCheck = false;
    pthread_t ulWriter = 0;
    pthread_t ulSender = 0;
    THREAD_PARAMS stParams = {0};

    if (NULL != pcFilePath)
    {
        stParams.pcFilePath = pcFilePath;
        stParams.ulSize = ulSize;

        // Initialize Mutex
        pthread_mutex_init(&sstDataMutex, NULL);

        /* Create threads */
        pthread_create(&ulWriter, NULL, writerThreadFunc, &stParams);
        pthread_create(&ulSender, NULL, senderThreadFunc, &stParams);

        /* Wait for execution to finish */
        pthread_join(ulWriter, NULL);
        pthread_join(ulSender, NULL);

        // Final Cleanup
        pthread_mutex_destroy(&sstDataMutex);
        blCheck = true;
    }
    else
    {
        printf("Failed to create threads and mutex");
    }

    return blCheck;
}

//*****************************.writerThreadFunc.*******************************
// Purpose : Function to create SHM write thread
// Inputs  : pParams - struct containing filepath and size
// Outputs : None
// Return  : NULL
// Notes   : None
//******************************************************************************
static void* writerThreadFunc(void* pParams)
{
    THREAD_PARAMS* pstParams = (THREAD_PARAMS*)pParams;
    FILE* pstFile = NULL;

    // Lock memory while writing
    pthread_mutex_lock(&sstDataMutex);

    if (true == assignSharedMemory(pstParams->ulSize))
    {
        pstFile = fopen(pstParams->pcFilePath, READ_BIN);

        if (NULL != pstFile)
        {
            // Copy file content to global spcSharedMem
            fread(spcSharedMem, 1, pstParams->ulSize, pstFile);
            fclose(pstFile);
            printf("Writer: SHM populated with file content.\n");
        }

        detachSharedMemory(pstParams->ulSize);
    }

    // Unlock to allow Sender thread to proceed
    pthread_mutex_unlock(&sstDataMutex);

    return NULL;
}

//****************************.senderThreadFunc.********************************
// Purpose : Function to create sender initialization thread
// Inputs  : pParams - struct containing filepath and size
// Outputs : None
// Return  : NULL
// Notes   : None
//******************************************************************************
static void* senderThreadFunc(void* pParams)
{
    THREAD_PARAMS* pstParams = (THREAD_PARAMS*)pParams;

    printf("Press Enter to start the Sender...\n");
    getchar(); 

    if (0 < pstParams->ulSize)
    {
        pthread_mutex_lock(&sstDataMutex);

        if (true == assignSharedMemory(pstParams->ulSize))
        {
            senderInit(pstParams->ulSize);
        }

        pthread_mutex_unlock(&sstDataMutex);
        printf("Sender: Shared memory ready. Starting GStreamer...\n");
    }

    return NULL;
}

//*********************************.senderInit.*********************************
// Purpose : Function to initialize sender pipeline
// Inputs  : ulSize - Size of the SHM
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool senderInit(uint32 ulSize)
{
    bool blCheck = false;
    GstElement *pstPipeline = NULL;
    GstElement *pstSource = NULL;
    GstElement *pstParser = NULL; 
    GstElement *pstPayloader = NULL;
    GstElement *pstSink = NULL;
    GstCaps *pstCaps = NULL;
    GstBus *pstBus = NULL;
    GstMessage *pstMessage = NULL;
    GstBuffer *pstBuffer = NULL;
    GstFlowReturn eStatus = 0;

    gst_init(NULL, NULL);

    // Create pipeline elements
    pstPipeline = gst_pipeline_new("shm-sender");
    pstSource = gst_element_factory_make("appsrc", "source");
    pstParser = gst_element_factory_make("mpegaudioparse", "parser");
    pstPayloader = gst_element_factory_make("rtpgstpay", "payloader");
    pstSink = gst_element_factory_make("udpsink", "sink");

    if ((NULL != pstPipeline) && (NULL != pstSource) &&
        (NULL != pstParser) && (NULL != pstPayloader) && (NULL != pstSink))
    {
        g_object_set(G_OBJECT(pstSource), "stream-type", 0, NULL);
        g_object_set(G_OBJECT(pstSource), "format", 2, NULL);
        g_object_set(G_OBJECT(pstPayloader), "ssrc", SSRC_VALUE, NULL);
        g_object_set(G_OBJECT(pstSink), "host", HOST_IP, NULL);
        g_object_set(G_OBJECT(pstSink), "port", PORT, NULL);
        g_object_set(G_OBJECT(pstSink), "sync", FALSE, NULL);

        pstCaps = gst_caps_from_string("audio/mpeg, mpegversion=(int)1, "
                                        "layer=(int)3");
        g_object_set(G_OBJECT(pstSource), "caps", pstCaps, NULL);
        g_object_set(G_OBJECT(pstSource), "format", GST_FORMAT_BYTES, NULL);
        gst_caps_unref(pstCaps);

        // Add elements to the pipeline
        gst_bin_add_many(GST_BIN(pstPipeline), pstSource, pstParser,
                        pstPayloader, pstSink, NULL);

        // Link the elements together
        if (TRUE == gst_element_link_many(pstSource, pstParser,
                                        pstPayloader, pstSink, NULL))
        {
            gst_element_set_state(pstPipeline, GST_STATE_PLAYING);

            // Wrap SHM pointer into a GStreamer buffer
            pstBuffer = gst_buffer_new_wrapped_full(GST_MEMORY_FLAG_READONLY, 
                    (gpointer)spcSharedMem, ulSize, 0, ulSize, NULL, NULL);

            if (NULL != pstBuffer)
            {
                // Push data to pipeline
                g_signal_emit_by_name(pstSource, "push-buffer", 
                                  pstBuffer, &eStatus);
                gst_buffer_unref(pstBuffer);
            }

            g_signal_emit_by_name(pstSource, "end-of-stream", &eStatus);
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

    return blCheck;
}

//**************************.assignSharedMemory.********************************
// Purpose : Function to assign SHM
// Inputs  : ulSize - Size of the SHM
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
static bool assignSharedMemory(uint32 ulSize)
{
    bool blCheck = false;
    key_t lKey = 0;
    int32 lMemoryId = 0;
    int8 *pMemory = NULL;

    if (true == assignMemory(&lKey, SHM_SEND_FILE))
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

//***************************.destroySharedMemory.******************************
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

    if (true == assignMemory(&lKey, SHM_SEND_FILE))
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

// EOF