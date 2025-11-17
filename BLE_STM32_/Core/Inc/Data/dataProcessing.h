//**************************** Data Processing *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and forward
//			 declarations needed for dataProcessing.c
// Note    : None
//
//******************************************************************************
#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

//******************************* Include Files ********************************
#include "dataTransmission.h"

//******************************* Global Types *********************************
typedef enum
{
	TYPE_TEMP = 0x00,
	TYPE_HUMD = 0x01,
	TYPE_TIME = 0x02
}TLV_TYPES;

typedef enum
{
	CMD_REQ = 0x00,
	CMD_RESP = 0x01
}CMD_TYPES;

typedef enum
{
	CMD_PING = 0x00,
	CMD_TELM = 0x01,
	CMD_TIME = 0x02
}CMD_NAMES;

//***************************** Global Constants *******************************
#define TIMER_DELAY             (2000)
#define REQUIRED_DELAY			(1500)
#define MAX_BUFFER_SIZE			(40)
#define TIMEOUT_DATA_SIZE		(13)
#define SENSOR_DATA_SIZE		(12)
#define DATA_OFFSET				(6)
#define THREAD_WAIT				(5)
#define MAX_SENSOR_DATA			(2)

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool dataSetValue(IWDG_HandleTypeDef *pstWatchDogHandle);
bool sendPingRequest(DATA_PACKET *pstPing);
bool receivePingResponse(DATA_PACKET *pstPing);
bool receiveTelemetryRequest(DATA_PACKET *pstTelemetry);
bool sendTelemetryResponse(DATA_PACKET *pstTelemetry);
bool sendTimeoutRequest(DATA_PACKET *pstTimeout);
bool receiveTimeoutResponse(DATA_PACKET *pstTimeout);
bool delayProcess(void);

//*********************** Inline Method Implementations ************************

#endif // DATAPROCESSING_H
// EOF
