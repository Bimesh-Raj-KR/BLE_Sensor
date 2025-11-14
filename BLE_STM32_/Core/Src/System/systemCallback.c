//***************************** System Callback ********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : systemCallback.c
// Summary : Contains all callback functions
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "systemCallBack.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static TIM_HandleTypeDef *pstHtimTwo = NULL;
static osSemaphoreId_t *pReceiverHandle = NULL;
static uint32 ulCount = 0;

//***************************** Local Functions ********************************

//**************************.systemSetValue.************************************
// Purpose : sets value of global variables needed for the functions
// Inputs  : pstHtim - TIM handle
//			 pSemHandle - Handle of the receiver semaphore
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
bool systemSetValue(TIM_HandleTypeDef *pstHtim, osSemaphoreId_t *pSemHandle)
{
	bool blCheck = false;

	if ((NULL != pstHtim) && (NULL != pSemHandle))
	{
		pstHtimTwo = pstHtim;
		pReceiverHandle = pSemHandle;
		blCheck = true;
	}

	return blCheck;
}

//**********************.HAL_GPIO_EXTI_Callback.********************************
// Purpose : Interrupt function that triggers on every falling edge
// Inputs  : unGpioPin - The GPIO pin that triggered the interrupt
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void HAL_GPIO_EXTI_Callback(uint16 unGpioPin)
{
	uint32 ulTime = 0;

    if(unGpioPin == GPIO_PIN_11)
    {
    	ulCount ++;
    	ulTime = __HAL_TIM_GET_COUNTER(pstHtimTwo);
    	__HAL_TIM_SET_COUNTER(pstHtimTwo, 0);

    	if (OUTPUT_START < ulCount)
    	{
			if (true == sensorOutput(ulTime))
			{
				// Success case
			}
    	}

    	if (MAX_OUTPUT == ulCount)
    	{
    		ulCount = 0;
    	}
    }
}

//***********************.HAL_UART_RxCpltCallback.******************************
// Purpose : Interrupt function that triggers when data is received over UART
// Inputs  : pstHuart - UART handle that triggered callback
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *pstHuart)
{
    if (pstHuart->Instance == USART1)
    {
    	uartStoreData();
    	uartReceiveByte();

    	if (true == uartCheckReception())
    	{
    		semaphorePost(pReceiverHandle);
    	}
    }
}

//EOF
