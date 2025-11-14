//**************************** UART Communication ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and forward 
//			 declarations needed for uartCommunication.cpp function
// Note    : None
// 
//******************************************************************************
#ifndef UARTCOMMUNICATION_H
#define UARTCOMMUNICATION_H

//******************************* Include Files ********************************
#include "commandProcessing.h"

//******************************* Global Types ********************************* 

//***************************** Global Constants ******************************* 
#define GPIO_LED                (2)
#define MAX_LIMIT               (3)
#define MAX_PROCESS             (4)
#define INTERRUPT_PIN           (13)
#define RX_PIN                  (16)
#define TX_PIN                  (17)
#define MIN_SLEEP               (20)
#define BAUD_RATE               (115200)

//***************************** Global Variables ******************************* 

//**************************** Forward Declarations ****************************
void uartSetup(void);
bool uartCommunication(void);
void uartFailure(void);

//*********************** Inline Method Implementations ************************ 

#endif // UARTCOMMUNICATION_H
// EOF 