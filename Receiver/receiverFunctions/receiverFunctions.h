//***************************** Receiver Functions *****************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions and forward declarations required 
//           for receiverFunctions.c functions
// Note    : None
// 
//******************************************************************************
#ifndef RECEIVER_FUNCTIONS_H
#define RECEIVER_FUNCTIONS_H

//******************************* Include Files ********************************
#include <stdbool.h>
#include <gst/gst.h>

//******************************* Global Types *********************************

//***************************** Global Constants *******************************
#define PORT                    (5000)
#define FILE_NAME               "ring.mp3"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool receiverInit(void);

//*********************** Inline Method Implementations ************************ 

#endif // RECEIVER_FUNCTIONS_H
// EOF 