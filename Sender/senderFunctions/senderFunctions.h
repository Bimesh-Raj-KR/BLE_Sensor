//****************************** Sender Functions ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for senderFunctions.c functions
// Note    : None
// 
//******************************************************************************
#ifndef SENDER_FUNCTIONS_H
#define SENDER_FUNCTIONS_H

//******************************* Include Files ********************************
#include "sharedMemory.h"
#include <gst/gst.h>
#include <sys/stat.h>
#include <pthread.h>

//******************************* Global Types *********************************
typedef struct _THREAD_PARAMS_
{
    int8* pcFilePath;
    uint32 ulSize;
} THREAD_PARAMS;

//***************************** Global Constants *******************************
#define SSRC_VALUE              (11111)
#define PORT                    (5000)
#define READ_BIN                "rb"
#define HOST_IP                 "127.0.0.1"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool senderThreadCreate(int8* pcFilePath, uint32 ulSize);

//*********************** Inline Method Implementations ************************ 

#endif // SENDER_FUNCTIONS_H
// EOF 