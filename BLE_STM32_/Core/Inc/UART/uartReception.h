//***************************** UART Reception *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all global constants and forward declarations needed for
//			 uartReception.c
// Note    : None
//
//******************************************************************************
#ifndef UARTRECEPTION_H
#define UARTRECEPTION_H

//******************************* Include Files ********************************
#include "sensorDriver.h"

//******************************* Global Types *********************************

//***************************** Global Constants *******************************
#define MAX_SIZE 				(128)
#define MAX_WAIT				(20)

//***************************** Global Variables *******************************
bool receiverSetValue(UART_HandleTypeDef *pstHuart1);
bool getBufferValue(uint8 **ppucDataBuffer);
bool uartReceiveByte(void);
void uartStoreData(void);
bool uartCheckReception(void);
bool uartTransmit(uint8 *pucDataBuffer, uint8 ucSize);

//**************************** Forward Declarations ****************************

//*********************** Inline Method Implementations ************************

#endif // UARTRECEPTION_H
// EOF
