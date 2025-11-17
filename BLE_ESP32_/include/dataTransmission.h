//**************************** Data Transmission *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and 
//			 forward declarations needed for dataTransmission.cpp
// Note    : None
//
//******************************************************************************
#ifndef DATATRANSMISSION_H
#define DATATRANSMISSION_H

//******************************* Include Files ********************************
#include "uartProcess.h"

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

#pragma pack(1)
typedef struct _DATA_PACKET_
{
	uint8 ucCmdType;
	uint8 ucCmd;
    uint32 ulUid;
    uint16 unLength;
    uint8 *pucData;
    uint8 ucChecksum;
}DATA_PACKET;
#pragma pack(0)

//***************************** Global Constants *******************************
#define MIN_BUFFER_SIZE			(9)

//***************************** Global Variables *******************************
bool dataTlv(size_t ulData, uint8 *ucTlvBuffer,
                uint8 ucType, uint8 ucLength);
bool dataBuildPacket(DATA_PACKET *pstData, uint8 ucCmdType, uint8 ucCmd,
					 uint32 ulUid, uint16 unLength, uint8 *ucTlvBuffer);
bool dataExtract(size_t *pulData, uint8 *pucType, uint8 *pucTlvBuffer);
bool dataBuilder(uint8 *pucBuffer, DATA_PACKET stData, uint8 ucSize);
bool dataParser(DATA_PACKET *pstData);
bool dataVerifyChecksum(DATA_PACKET stData);

//**************************** Forward Declarations ****************************

//*********************** Inline Method Implementations ************************

#endif // DATATRANSMISSION_H
// EOF
