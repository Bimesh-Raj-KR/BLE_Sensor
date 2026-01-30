//***************************** Receiver Functions *****************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all global constants and forward declarations required 
//           for receiverFunctions.c functions
// Note    : None
// 
//******************************************************************************
#ifndef RECEIVER_FUNCTIONS_H
#define RECEIVER_FUNCTIONS_H

//******************************* Include Files ********************************
#include "sharedMemory.h"
#include <gst/gst.h>
#include <sys/stat.h>

//******************************* Global Types *********************************


//***************************** Global Constants *******************************
#define PORT                    (5000)

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
void receiverThreadCreate(uint32 ulSize);

//*********************** Inline Method Implementations ************************ 

#endif // RECEIVER_FUNCTIONS_H
// EOF 