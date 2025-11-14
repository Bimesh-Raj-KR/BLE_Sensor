//******************************* UART Process *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all forward declarations needed for uartProcess.cpp
// Note    : None
//
//******************************************************************************
#ifndef UARTPROCESS_H
#define UARTPROCESS_H

//******************************* Include Files ********************************
#include "uartReception.h"

//******************************* Global Types *********************************

//***************************** Global Constants *******************************

//***************************** Global Variables *******************************
bool uartReceiveAndStore(void);
void uartClear(void);
bool uartTransmit(uint8 *pucDataBuffer, uint8 ucSize);
void uartDelay(uint16 ulDelay);

//**************************** Forward Declarations ****************************

//*********************** Inline Method Implementations ************************

#endif // UARTPROCESS_H
// EOF
