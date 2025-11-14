//***************************** System Setup ***********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all global constants and forward declarations needed
//			 for systemSetup.c
// Note    : None
//
//******************************************************************************
#ifndef SYSTEMSETUP_H
#define SYSTEMSETUP_H

//******************************* Include Files ********************************
#include "systemCallback.h"

//******************************* Global Types *********************************

//***************************** Global Constants *******************************
#define STACK_WORDS				(128)
#define MIN_SLEEP               (20)
#define MAX_LIMIT               (6)
#define STACK_BYTE              (4)
#define MAX_COUNT 				(3)
#define HANDLE_COUNT 			(2)

//***************************** Global Variables *******************************
bool handleCreate(void);
bool setValue(UART_HandleTypeDef *pstHuartOne, TIM_HandleTypeDef *pstHtim,
		IWDG_HandleTypeDef *pstHiwdg);
bool timerBaseSetup(void);
bool watchdogTriggerCheck(void);

//**************************** Forward Declarations ****************************

//*********************** Inline Method Implementations ************************

#endif // SYSTEMSETUP_H
// EOF
