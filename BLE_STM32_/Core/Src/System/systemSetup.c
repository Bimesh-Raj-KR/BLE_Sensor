//***************************** System Setup ***********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : systemSetup.c
// Summary : Sets up STM32 board for code execution
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "systemSetup.h"
#include "taskOperation.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static TIM_HandleTypeDef *pstHtimTwo = NULL;
static osSemaphoreId_t receiverHandle = 0;
static osThreadId_t syncHandle = 0;

//***************************** Local Functions ********************************

//*******************************.handleCreate.*********************************
// Purpose : Function to create handles for task and semaphore
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool handleCreate(void)
{
	bool blCheck = false;
	uint8 ucCount = 0;

	const osThreadAttr_t syncAttributes =
	{
	  .name = "sync",
	  .stack_size = STACK_WORDS * STACK_BYTE,
	  .priority = (osPriority_t) osPriorityNormal,
	};

	const osSemaphoreAttr_t receiverSemAttributes =
	{
	  .name = "receiver"
	};

	if (true == semaphoreCreate(&receiverSemAttributes, &receiverHandle))
	{
		ucCount ++;
	}

	if (true == threadCreate(taskSync, NULL, &syncAttributes, &syncHandle))
	{
		ucCount ++;
	}

	if (HANDLE_COUNT == ucCount)
	{
		blCheck = true;
	}
	else
	{
		printf("Failed to create all Handles\r\n");
	}

	return blCheck;
}

//**********************************.setValue.**********************************
// Purpose : Function to share handle value across different source files
// Inputs  : pstHuartOne - UART1 handle
//           pstHtim - TIM handle
//			 pstHiwdg - Watchdog handle
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool setValue(UART_HandleTypeDef *pstHuartOne, TIM_HandleTypeDef *pstHtim,
		IWDG_HandleTypeDef *pstHiwdg)
{
	bool blCheck = false;

	if ((NULL != pstHuartOne) && (NULL != pstHtim) && (NULL != pstHiwdg))
	{
		systemSetValue(pstHtim, &receiverHandle);
		processSetValue(pstHuartOne, &receiverHandle);
		dataSetValue(pstHiwdg);
		receiverSetValue(pstHuartOne);
		pstHtimTwo = pstHtim;
		blCheck = true;
	}

	return blCheck;
}

//*****************************.timerBaseSetup.*********************************
// Purpose : Function to start TIM Base generation in interrupt mode
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool timerBaseSetup(void)
{
	bool blCheck = false;

	if (HAL_OK == HAL_TIM_Base_Start_IT(pstHtimTwo))
	{
		blCheck = true;
	}

	return blCheck;
}

//***************************.watchdogTriggerCheck.*****************************
// Purpose : Function to allocate memory
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool watchdogTriggerCheck(void)
{
	bool blCheck = false;
	uint8 ucIterator = 0;
	uint8 ucFlag = 0;

	if (true == checkReset())
	{
		clearReset();
		printf("\nRefreshing STM32\r\n");
		gpioWrite(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
		gpioWrite(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);

		for (ucIterator = 0; ucIterator < MAX_LIMIT; ucIterator ++)
		{
			gpioToggle(GPIOA, GPIO_PIN_5);

			if (true != milliDelay(MIN_SLEEP))
			{
				perror("Unable to delay execution");
				ucFlag = 1;
			}
		}
	}

	if (0 == ucFlag)
	{
		blCheck = true;
	}
	else
	{
		printf("Failed to set up Watchdog\r\n");
	}

	return blCheck;
}

// EOF
