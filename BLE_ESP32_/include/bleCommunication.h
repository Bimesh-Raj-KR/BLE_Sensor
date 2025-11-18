//***************************** BLE Communication ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and forward 
//           declarations needed for bleCommunication.cpp function
// Note    : None
// 
//******************************************************************************
#ifndef BLECOMMUNICATION_H
#define BLECOMMUNICATION_H

//******************************* Include Files ********************************
#include "bleFunctions.h"

//******************************* Global Types *********************************
typedef struct _BLE_SENSOR_
{
	uint8 ucHumidInt;
	uint8 ucHumidDec;
	uint8 ucTempInt;
	uint8 ucTempDec;
	uint8 ucCheckSum;
}BLE_SENSOR;

//***************************** Global Constants *******************************
#define MAX_DATA				(2)
#define VERSION_SIZE			(6)
#define TEMPERATURE_UUID 		(0x2A1C)
#define HUMIDITY_UUID 			(0x2A6F)
#define CHAR_NULL				('\n')

//***************************** Global Variables ******************************* 

//**************************** Forward Declarations ****************************
bool bleSetup();
bool bleTransmit(uint32 *pulReadings, uint8 *pucType);
bool bleStringTransmit(uint8 *pucReadings, uint8 ucType);

//*********************** Inline Method Implementations ************************ 

#endif // BLECOMMUNICATION_H
// EOF 