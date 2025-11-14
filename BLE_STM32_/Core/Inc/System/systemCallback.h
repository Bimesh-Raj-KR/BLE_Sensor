//***************************** System Callback ********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all global constant and forward declarations needed for
//			 systemCallback.c to run
// Note    : None
//
//******************************************************************************
#ifndef SYSTEMCALLBACK_H
#define SYSTEMCALLBACK_H

//******************************* Include Files ********************************
#include "commandProcessing.h"

//******************************* Global Types *********************************

//***************************** Global Constants *******************************
#define MAX_OUTPUT				(42)
#define OUTPUT_START            (2)

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool systemSetValue(TIM_HandleTypeDef *pstHtim, osSemaphoreId_t *pSemHandle);
void HAL_GPIO_EXTI_Callback(uint16 unGpioPin);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *pstHuart);

//*********************** Inline Method Implementations ************************

#endif // SYSTEMCALLBACK_H
// EOF
