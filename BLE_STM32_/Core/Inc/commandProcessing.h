//**************************** Command Processing ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all forward declarations needed for commandProcessing.c
// Note    : None
//
//******************************************************************************
#ifndef COMMANDPROCESSING_H
#define COMMANDPROCESSING_H

//******************************* Include Files ********************************
#include "dataProcessing.h"

//******************************* Global Types *********************************

//***************************** Global Constants *******************************

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool processSetValue(UART_HandleTypeDef *pstHuart, osSemaphoreId_t *pSemHandle);
bool processPing();
bool processTelemetry();
bool processTimeout();

//*********************** Inline Method Implementations ************************

#endif // COMMANDPROCESSING_H
// EOF
