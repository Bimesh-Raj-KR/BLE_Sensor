//****************************** Thread Operations *****************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contain all forward declaration required for threadOperations.c
// Note    : None
//
//******************************************************************************
#ifndef THREAD_OPERATIONS_H
#define THREAD_OPERATIONS_H

//******************************* Include Files ********************************
#include "systemFunctions.h"

//******************************* Global Types *********************************

//***************************** Global Constants *******************************

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool threadCreate(osThreadFunc_t pTaskName, uint32 *pParameter,
		const osThreadAttr_t *pthreadAttributes, osThreadId_t *pThreadHandle);
bool semaphoreCreate(const osSemaphoreAttr_t *pSemaphoreAttributes,
						   osSemaphoreId_t *pSemaphoreHandle);
bool semaphoreWait(osSemaphoreId_t *pSemHandle);
bool semaphorePost(osSemaphoreId_t *pSemHandle);

//*********************** Inline Method Implementations ************************

#endif // THREAD_OPERATIONS_H
// EOF
