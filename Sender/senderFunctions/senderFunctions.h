//****************************** Sender Functions ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all global constants and forward declarations required for 
//           senderFunctions.c functions
// Note    : None
// 
//******************************************************************************
#ifndef SENDER_FUNCTIONS_H
#define SENDER_FUNCTIONS_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <gst/gst.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

//******************************* Global Types *********************************

//***************************** Global Constants *******************************
#define MEM_SIZE                (2000000)
#define EOS_WAIT                (2000000)
#define THREAD_DELAY            (500000)
#define PORT                    (5000)
#define FILE_NAME               "/mnt/d/Gstream_ShareMem/Sender/Ringtone.mp3"
#define HOST_IP                 "127.0.0.1"
#define SOURCE_SOCKET           "/tmp/source_shm"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
void senderInit(void);

//*********************** Inline Method Implementations ************************ 

#endif // SENDER_FUNCTIONS_H
// EOF 