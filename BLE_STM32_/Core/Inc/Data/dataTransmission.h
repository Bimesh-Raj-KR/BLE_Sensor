//**************************** Data Transmission *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and forward
//			 declarations needed for dataTransmission.c
// Note    : None
//
//******************************************************************************
#ifndef DATATRANSMISSION_H
#define DATATRANSMISSION_H

//******************************* Include Files ********************************
#include "uartReception.h"

//******************************* Global Types *********************************
typedef enum
{
	DATA_TYPE,
	DATA_LENGTH,
	DATA_VALUE

}DATA_FORMAT;

typedef enum
{
	BUFFER_CMD_TYPE = 0,
	BUFFER_CMD = 1,
	BUFFER_UID = 2,
	BUFFER_LENGTH = 6,
	BUFFER_DATA = 8
}BUFFER_VARIABLES;

//***************************** Global Constants *******************************
#define MAX_MULTIPLIER			(10)
#define MIN_BUFFER_SIZE			(9)

//***************************** Global Variables *******************************
bool getValue(void);
bool dataFormat(BLE_SENSOR *pstReadings, uint32 *pulReadings);
bool dataTlv(uint32 ulData, uint8 *ucTlvBuffer,
                uint8 ucType, uint8 ucLength);
bool dataBuildPacket(DATA_PACKET *pstData, uint8 ucCmdType, uint8 ucCmd,
					 uint32 ulUid, uint16 unLength, uint8 *ucTlvBuffer);
bool dataExtract(uint16 *punData, uint8 *pucType, uint8 *pucTlvBuffer);
bool dataBuilder(uint8 *pucBuffer, DATA_PACKET stData, uint8 ucSize);
bool dataParser(DATA_PACKET *pstData);
bool dataVerifyChecksum(DATA_PACKET stData);

//**************************** Forward Declarations ****************************

//*********************** Inline Method Implementations ************************

#endif // DATATRANSMISSION_H
// EOF
