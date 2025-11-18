//**************************** Command Processing ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : commandProcessing.c
// Summary : Handle the different process commands
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "commandProcessing.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static UART_HandleTypeDef *pstHuartOne = NULL;
static osSemaphoreId_t *pReceiverHandle = NULL;

//***************************** Local Functions ********************************

//******************************.processSetValue.*******************************
// Purpose : Function to set value across different source files
// Inputs  : pSemHandle - The receiver semaphore handle value
//			 pstHuart -  USART1 handle value
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
bool processSetValue(UART_HandleTypeDef *pstHuart, osSemaphoreId_t *pSemHandle)
{
	bool blCheck = false;

	if ((NULL != pstHuart) && (NULL != pSemHandle))
	{
		pstHuartOne = pstHuart;
		pReceiverHandle = pSemHandle;
		blCheck = true;
	}

	return blCheck;
}

//*********************************.processPing.********************************
// Purpose : Function to process ping command
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool processPing(void)
{
	bool blCheck = false;
	DATA_PACKET stPing = {0};

	if (true == sendPingRequest(&stPing))
	{
		semaphoreWait(pReceiverHandle);

		if (true == receivePingResponse(&stPing))
		{
			blCheck = true;
		}
	}

	milliDelay(MAX_SLEEP);

	if (true != blCheck)
	{
		printf("Failed to process ping\r\n");
	}

	return blCheck;
}

//*****************************.processTelemetry.*******************************
// Purpose : Function to process telemetry command
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool processTelemetry(void)
{
	bool blCheck = false;
	DATA_PACKET stTelemetry = {0};

	semaphoreWait(pReceiverHandle);

	if (true == receiveTelemetryRequest(&stTelemetry))
	{
		if (true == sendTelemetryResponse(&stTelemetry))
		{
			blCheck = true;
		}
	}

	milliDelay(MAX_SLEEP);

	if (true != blCheck)
	{
		printf("Failed to process telemetry\r\n");
	}

	return blCheck;
}

//*******************************.processTimeout.*******************************
// Purpose : Function to process timeout command
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool processTimeout(void)
{
	bool blCheck = false;
	DATA_PACKET stTimeout = {0};

	if (true == sendTimeoutRequest(&stTimeout))
	{
		semaphoreWait(pReceiverHandle);

		if (true == receiveTimeoutResponse(&stTimeout))
		{
			blCheck = true;
		}
	}

	milliDelay(MAX_SLEEP);

	if (true != blCheck)
	{
		printf("Failed to process timeout\r\n");
	}

	return blCheck;
}

//*******************************.processVersion.*******************************
// Purpose : Function to process version command
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool processVersion(void)
{
	bool blCheck = false;
	DATA_PACKET stVersion = {0};

	semaphoreWait(pReceiverHandle);

	if (true == receiveVersionRequest(&stVersion))
	{
		if (true == sendVersionResponse(&stVersion))
		{
			blCheck = true;
		}
	}

	milliDelay(MAX_SLEEP);

	if (true != blCheck)
	{
		printf("Failed to process version\r\n");
	}

	return blCheck;
}

//EOF
