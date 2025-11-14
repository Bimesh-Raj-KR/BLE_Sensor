//********************************* Thread Operations **************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : threadOperations.c
// Summary : Wrapper for different thread operations
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "threadOperations.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//********************************.threadCreate.********************************
// Purpose : Function to Create a thread
// Inputs  : pTaskName - Name of the thread to be created
//			 pParameter -  Function parameter of the thread
//			 pThreadAttributes - Attributes of thread
//			 pThreadHandle - Thread Handle
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool threadCreate(osThreadFunc_t pTaskName, uint32 *pParameter,
		const osThreadAttr_t *pThreadAttributes, osThreadId_t *pThreadHandle)
{
	bool blCheck = false;

	if (NULL != pThreadAttributes && NULL != pThreadHandle)
	{
		*pThreadHandle = osThreadNew(pTaskName, (void*)pParameter,
				pThreadAttributes);
		blCheck = true;
	}
	else
	{
		perror("Unable to create task\r\n");
	}

	return blCheck;
}

//*****************************.semaphoreCreate.********************************
// Purpose : Function to Create a semaphore
// Inputs  : pSemAttributes - Attribute of the semaphore
// 			 pSemaphoreHandle - Semaphore Handle
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool semaphoreCreate(const osSemaphoreAttr_t *pSemaphoreAttributes,
						   osSemaphoreId_t *pSemaphoreHandle)
{
	bool blCheck = false;

	if ((NULL != pSemaphoreAttributes) && (NULL != pSemaphoreHandle))
	{
		*pSemaphoreHandle = osSemaphoreNew(1, 0, pSemaphoreAttributes);
		blCheck = true;
	}
	else
	{
		perror("Unable to create Semaphore\r\n");
	}

	return blCheck;
}

//******************************.semaphoreWait.*********************************
// Purpose : Function to acquire a semaphore
// Inputs  : pSemaphoreHandle - Semaphore Handle
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool semaphoreWait(osSemaphoreId_t *pSemaphoreHandle)
{
	bool blCheck = false;

	if (NULL != pSemaphoreHandle)
	{
		osSemaphoreAcquire(*pSemaphoreHandle, osWaitForever);
		blCheck = true;
	}
	else
	{
		perror("Semaphore acquiring failed\r\n");
	}

	return blCheck;
}

//******************************.semaphorePost.*********************************
// Purpose : Function to release a semaphore
// Inputs  : pSemaphoreHandle - Semaphore Handle
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool semaphorePost(osSemaphoreId_t *pSemaphoreHandle)
{
	bool blCheck = false;

	if (NULL != pSemaphoreHandle)
	{
		osSemaphoreRelease(*pSemaphoreHandle);
		blCheck = true;
	}
	else
	{
		perror("Semaphore releasing failed\r\n");
	}

	return blCheck;
}

//EOF
