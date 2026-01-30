//******************************* Shared Memory ********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for sharedMemory.c functions
// Note    : None
// 
//******************************************************************************
#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//******************************* Global Types *********************************
typedef char int8;
typedef int int32;
typedef unsigned int uint32;

//***************************** Global Constants *******************************
#define PERMISSIONS             (0644)
#define MAX_ARG                 (2)
#define ERROR                   (-1)
#define SHM_RECV_FILE           "/mnt/d/GStream_shm/shmRecvKey.txt"
#define SHM_SEND_FILE           "/mnt/d/GStream_shm/shmSendKey.txt"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool assignMemory(key_t *plKey, int8 *pcFileName);
bool getMemoryId(int32 *pMemoryId, key_t lKey, uint32 unSize);
bool attachMemory(int8 **ppMemory, int32 lMemoryId);
bool detachMemory(int8 *pMemory);
bool deleteMemory(int32 lMemoryId);

//*********************** Inline Method Implementations ************************ 

#endif // SHARED_MEMORY_H
// EOF