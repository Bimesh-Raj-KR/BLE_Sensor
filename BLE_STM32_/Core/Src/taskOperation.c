//***************************** Task Operation *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : taskOperation.c
// Summary : Contains the task to be executed
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "taskOperation.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//********************************.taskSync.************************************
// Purpose : Function to establish Inter board communication via UART
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************

void taskSync(void *pArgument)
{
	timerBaseSetup();
	watchdogTriggerCheck();
	milliDelay(PROCESS_DELAY);

	while(1)
	{
		processPing();
		processTelemetry();
		processTimeout();
		delayProcess();
	}
}

//EOF

