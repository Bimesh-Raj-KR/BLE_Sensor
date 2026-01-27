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
#include <stdbool.h>
#include <gst/gst.h>

//******************************* Global Types *********************************
typedef int int32;
typedef char int8;

//***************************** Global Constants *******************************
#define SSRC_VALUE              (11111)
#define PORT                    (5000)
#define HOST_IP                 "127.0.0.1"
#define LOCAL_FILE              "backup.mp3"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool senderInit(int8* pcFilePath);

//*********************** Inline Method Implementations ************************ 

#endif // SENDER_FUNCTIONS_H
// EOF 