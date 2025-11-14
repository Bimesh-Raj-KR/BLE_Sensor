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
typedef enum
{
	TYPE_TEMP = 0x00,
	TYPE_HUMD = 0x01,
	TYPE_TIME = 0x02
}TLV_TYPES;

typedef struct _BLE_SENSOR_
{
	uint8 ucHumidInt;
	uint8 ucHumidDec;
	uint8 ucTempInt;
	uint8 ucTempDec;
	uint8 ucCheckSum;
}BLE_SENSOR;

//***************************** Global Constants *******************************
#define TEMPERATURE_UUID 		(0x2A1C)
#define HUMIDITY_UUID 			(0x2A6F)
#define MAX_DATA				(2)

//***************************** Global Variables ******************************* 

//**************************** Forward Declarations ****************************
bool bleSetup();
bool bleTransmit(uint32 *pulReadings, uint8 *pucType);

//*********************** Inline Method Implementations ************************ 

#endif // BLECOMMUNICATION_H
// EOF 