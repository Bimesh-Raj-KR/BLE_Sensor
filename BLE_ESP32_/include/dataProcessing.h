//**************************** Data processing *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and 
//			 forward declarations needed for dataProcessing.cpp
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
#define MIN_SLEEP               (20)
#define SENSOR_DATA_SIZE		(12)
#define DATA_OFFSET				(6)
#define TIMEOUT_DATA_SIZE		(4)

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool setValue(DATA_PACKET *pstReceivedData);
void setUid(uint32 ulUid);
bool sendPingResponse(void);
bool sendTelemetryRequest(void);
bool processTelemetryResponse(void);
bool sendTimeoutResponse(void);
bool delayProcess(void);

//*********************** Inline Method Implementations ************************

#endif // DATAPROCESSING_H
// EOF
