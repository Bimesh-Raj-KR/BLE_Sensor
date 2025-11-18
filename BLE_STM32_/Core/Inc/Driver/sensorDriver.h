//**************************** Sensor Driver ***********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and forward
//			 declarations needed for sensorDriver.c
// Note    : None
//
//******************************************************************************
#ifndef SENSORDRIVER_H
#define SENSORDRIVER_H

//******************************* Include Files ********************************
#include "threadOperations.h"

//******************************* Global Types *********************************
// enum for buffer array elements
typedef enum
{
	FIRST_DATA,
	SECOND_DATA,
	THIRD_DATA,
	FOURTH_DATA,
	FIFTH_DATA
}DATA_SIZE;

// Struct for storing sensor readings
#pragma pack(1)
typedef struct _BLE_SENSOR_
{
	uint8 ucHumidInt;
	uint8 ucHumidDec;
	uint8 ucTempInt;
	uint8 ucTempDec;
	uint8 ucCheckSum;
}BLE_SENSOR;
#pragma pack(0)

//***************************** Global Constants *******************************
#define MAX_DATA			(5)
#define COUNT_LIMIT			(8)
#define START_DELAY			(20)
#define LOW_LIMIT			(30)
#define MID_LIMIT			(90)
#define HIGH_LIMIT			(140)

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
void sensorStart(void);
bool sensorOutput(uint32 ulDifference);
bool sensorReadings(BLE_SENSOR *pstReadings);

//*********************** Inline Method Implementations ************************

#endif // SENSORDRIVER_H
// EOF
