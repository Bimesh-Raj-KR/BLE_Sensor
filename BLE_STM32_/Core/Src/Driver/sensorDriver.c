//**************************** Sensor Driver ***********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : sensorDriver.c
// Summary : Driver functions for starting and getting data from DHT11 sensor
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "sensorDriver.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static uint8 ucCount = 0;
static uint8 ucPosition = 0;
static uint8 ucData[MAX_DATA] = {0};

//********************************.sensorStart.*********************************
// Purpose : Function to ask the sensor for reading
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void sensorStart(void)
{
	modeChange(GPIO_MODE_OUTPUT_PP);
	gpioWrite(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
	milliDelay(START_DELAY);
	gpioWrite(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);
	ucPosition = 0;
	ucCount = 0;
	memset(ucData, 0, sizeof(ucData));
	modeChange(GPIO_MODE_IT_FALLING);
}


//********************************.sensorOutput.********************************
// Purpose : Function to get the output response of the sensor
// Inputs  : ulDifference - time difference between two falling edges
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool sensorOutput(uint32 ulDifference)
{
	bool blCheck = false;
	uint8 ucFlag = 0;

	if ((LOW_LIMIT < ulDifference) && (MID_LIMIT > ulDifference))
	{
		ucData[ucPosition] <<= 1;
	}
	else if ((MID_LIMIT < ulDifference) && (HIGH_LIMIT > ulDifference))
	{

		ucData[ucPosition] <<= 1;
		ucData[ucPosition] += 1;
	}
	else
	{
		ucFlag = 1;
	}

	ucCount ++;

	if (COUNT_LIMIT == ucCount)
	{
		ucCount = 0;
		ucPosition ++;
	}

	if (0 == ucFlag)
	{
		blCheck = true;
	}

	return blCheck;
}

//*******************************.sensorReadings.*******************************
// Purpose : Function to get the sensor readings
// Inputs  : pstReadings - Stores the sensor readings
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool sensorReadings(BLE_SENSOR *pstReadings)
{
	bool blCheck = false;
	uint32 ulSum;

	if (NULL != pstReadings)
	{
		pstReadings->ucHumidInt = ucData[FIRST_DATA];
		pstReadings->ucHumidDec = ucData[SECOND_DATA];
		pstReadings->ucTempInt = ucData[THIRD_DATA];
		pstReadings->ucTempDec = ucData[FOURTH_DATA];
		pstReadings->ucCheckSum = ucData[FIFTH_DATA];

		ulSum = pstReadings->ucHumidInt + pstReadings->ucHumidDec +
				pstReadings->ucTempInt + pstReadings->ucTempDec;

		if (pstReadings->ucCheckSum == ulSum)
		{
			blCheck = true;
		}
	}
	else
	{
		perror("Null Check Failed\r\n");
	}

	if (true != blCheck)
	{
		printf("Unable to get sensor readings\r\n");
	}
	return blCheck;
}

// EOF
