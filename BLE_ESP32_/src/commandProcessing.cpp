//***************************** Command Processing *****************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : commandProcessing.cpp
// Summary : Contains different actions taken after receiving data
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "commandProcessing.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//*******************************.processPing.**********************************
// Purpose : Function to process ping command
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool processPing(void)
{
	bool blCheck = false;

	if (true == sendPingResponse())
	{
		uartDelay(MAX_SLEEP);

		if (true == sendTelemetryRequest())
		{
			blCheck = true;
		}
	}

	if (true != blCheck)
	{
		Serial.println("Failed to process ping");
	}

	return blCheck;
}

//****************************.processTelemetry.********************************
// Purpose : Function to process telemetry command
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool processTelemetry(void)
{
	bool blCheck = false;

	if (true == processTelemetryResponse())
	{
		uartDelay(MAX_SLEEP);
		blCheck = true;
	}
	else
	{
		Serial.println("Failed to process telemetry");
	}

	return blCheck;
}

//*****************************.processTimeout.*********************************
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

	if (true == sendTimeoutResponse())
	{
		uartDelay(MAX_SLEEP);
		blCheck = true;
	}
	else
	{
		Serial.println("Failed to process timeout");
	}

	return blCheck;
}

// EOF