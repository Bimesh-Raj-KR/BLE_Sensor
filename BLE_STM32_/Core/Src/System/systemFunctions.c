//*************************** System Functions *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : systemFunctions.c
// Summary : Wrapper for necessary STM32 functions
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "systemFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//****************************.interruptEnable.*********************************
// Purpose : Function to enable and set priority of the interrupt
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void interruptEnable(void)
{
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, PRIORITY, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

//********************************.gpioWrite.***********************************
// Purpose : Function to write GPIO pins
// Inputs  : pPeripheral - The Port that should be written to
//			 unGpioPin - The pin within the port that should be written to
//			 unPinState - The state of the pin after writing to it
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void gpioWrite(GPIO_TypeDef* pPeripheral, uint16 unGpioPin, uint16 unPinState)
{
	HAL_GPIO_WritePin(pPeripheral, unGpioPin, unPinState);
}

//********************************.gpioToggle.**********************************
// Purpose : Function to toggle GPIO pins
// Inputs  : pPeripheral - The Port that should be toggled
//			 unGpioPin - The pin within the port that should be toggled
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void gpioToggle(GPIO_TypeDef* pPeripheral, uint16 unGpioPin)
{
	HAL_GPIO_TogglePin(pPeripheral, unGpioPin);
}

//******************************.refreshWatchDog.*******************************
// Purpose : Function to refresh watchdog timer
// Inputs  : pstHiwdg - The watchDog Timer to refresh
// Outputs : None
// Return  : true if there are no errors in the code else false
// Notes   : None
//******************************************************************************
bool refreshWatchDog(IWDG_HandleTypeDef *pstHiwdg)
{
	bool blCheck = false;

	if(NULL != pstHiwdg)
	{
		HAL_IWDG_Refresh(pstHiwdg);
		blCheck = true;
	}
	else
	{
		printf("Failed to refresh watchdog\r\n");
	}

	return blCheck;
}

//********************************.checkReset.**********************************
// Purpose : Function to check if last reset was triggered by watchdog
// Inputs  : None
// Outputs : None
// Return  : true if watchdog triggered reset else false
// Notes   : None
//******************************************************************************
bool checkReset(void)
{
	bool blCheck = false;

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
	{
		blCheck = true;
	}
	else
	{
		printf("Reset was not due to watchDog timer\r\n");
	}

	return blCheck;
}

//********************************.clearReset.**********************************
// Purpose : Function to clear reset flag
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void clearReset(void)
{
	__HAL_RCC_CLEAR_RESET_FLAGS();
}

//********************************.modeChange.**********************************
// Purpose : Function to change mode of the GPIO pin PC11
// Inputs  : ulMode - The mode to which the pin must be configured
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void modeChange(uint32 ulMode)
{
	GPIO_InitTypeDef stGpioInitStruct = {0};

	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

	stGpioInitStruct.Pin = GPIO_PIN_11;
	stGpioInitStruct.Mode = ulMode;
	stGpioInitStruct.Pull = GPIO_NOPULL;
	stGpioInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &stGpioInitStruct);

	if (ulMode == GPIO_MODE_IT_FALLING)
	{
		interruptEnable();
	}
}

//*****************************.changeWatchdog.*********************************
// Purpose : Function to change watchdog reload value
// Inputs  : pstHiwdg - watchdog handle
//			 unDelay - task delay value
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool changeWatchdog(IWDG_HandleTypeDef *pstHiwdg, uint16 unDelay)
{
	bool blCheck = false;
	uint16 unReload = 0;

	if (NULL != pstHiwdg)
	{
		unReload = ((unDelay + DELAY_BUFFER) * FREQUENCY) / PRESCALER_VALUE - 1;

		if (MAX_RELOAD > unReload)
		{
			pstHiwdg->Instance->KR = WRITE_ACCESS;
			pstHiwdg->Instance->RLR = unReload;
			milliDelay(WATCHDOG_WAIT);

			if (0 == pstHiwdg->Instance->SR)
			{
				pstHiwdg->Instance->KR = IWDG_KEY_RELOAD;
			}

			pstHiwdg->Instance->KR = RELOAD_ACCESS;
			blCheck = true;
		}

	}
	else
	{
		printf("Unable to change watchdog timeout\r\n");
	}

	return blCheck;
}

//********************************.milliDelay.**********************************
// Purpose : Function to run a delay of milliseconds
// Inputs  : ulDelay - The Delay for which execution must be halted
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool milliDelay(uint32 ulDelay)
{
	bool blCheck = false;

	if (osOK == osDelay(ulDelay))
	{
		blCheck = true;
	}
	else
	{
		printf("Failed to Start Delay\r\n");
	}

	return blCheck;
}

// EOF
