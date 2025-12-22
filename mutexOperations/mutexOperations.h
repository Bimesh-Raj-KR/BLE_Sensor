//***************************** Mutex Operations *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all forward declarations required  for mutexOperations.c 
//           functions
// Note    : None
// 
//******************************************************************************
#ifndef MUTEX_OPERATIONS_H
#define MUTEX_OPERATIONS_H

//******************************* Include Files ********************************
#include "sharedMemory.h"
#include <pthread.h>
#include <unistd.h>

//******************************* Global Types *********************************

//***************************** Global Constants *******************************

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool createMutex(pthread_mutex_t **ppstMutex);
bool lockMutex(pthread_mutex_t *pstMutex);
bool unlockMutex(pthread_mutex_t *pstMutex);

//*********************** Inline Method Implementations ************************ 

#endif // MUTEX_OPERATIONS_H
// EOF