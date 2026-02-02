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
#include <gst/gst.h>
#include <pthread.h>
#include <unistd.h>

//******************************* Global Types *********************************

//***************************** Global Constants *******************************
#define MEM_SIZE                (2000000)
#define EOS_WAIT                (2000000)
#define THREAD_DELAY            (500000)
#define PORT                    (5000)
#define FILE_NAME               "shmOutput.wav"
#define HOST_IP                 "127.0.0.1"
#define TARGET_SOCKET           "/tmp/target_shm"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
void receiverInit(void);

//*********************** Inline Method Implementations ************************ 

#endif // RECEIVER_FUNCTIONS_H
// EOF 