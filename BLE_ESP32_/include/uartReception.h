//****************************** UART Reception ********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all global constants and forward declarations needed for 
//           uartReception.cpp
// Note    : None
//
//******************************************************************************
#ifndef UARTRECEPTION_H
#define UARTRECEPTION_H

//******************************* Include Files ********************************
#include "bleCommunication.h"

//******************************* Global Types *********************************

//***************************** Global Constants *******************************
#define MAX_SIZE 				(128)

//***************************** Global Variables *******************************
bool uartGetData(uint8 **ppucDataBuffer);
void uartReceiveByte(void);
void uartStoreData(void);
bool uartGetBuffer(uint8 *pucDataBuffer, uint8 ucSize);
void uartClearData(void);
void uartClearBuffer(void);

//**************************** Forward Declarations ****************************

//*********************** Inline Method Implementations ************************

#endif // UARTRECEPTION_H
// EOF
