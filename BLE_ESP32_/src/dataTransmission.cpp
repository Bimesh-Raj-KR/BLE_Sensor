//**************************** Data Transmission *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : dataTransmission.cpp
// Summary : Contains function to format, build and parse received data
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "dataTransmission.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//*********************************.dataTlv.************************************
// Purpose : Function to convert data to TLV format
// Inputs  : ulData - Data to be converted
//			 pucTlvBuffer - Buffer to store TLV data
//			 ucType - type of data
//			 ucLength - size of data
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool dataTlv(size_t ulData, uint8 *pucTlvBuffer,
                uint8 ucType, uint8 ucLength)
{
    bool blCheck = false;

    if ((NULL != pucTlvBuffer) && (0 < ucLength))
    {
        pucTlvBuffer[DATA_TYPE] = ucType;
        pucTlvBuffer[DATA_LENGTH] = ucLength;
        memcpy(&pucTlvBuffer[DATA_VALUE], &ulData, ucLength);
        blCheck = true;
    }

    if (true != blCheck)
	{
		Serial.println("Failed to convert data to TLV");
	}

    return blCheck;
}

//****************************.dataBuildPacket.*********************************
// Purpose : Function to build data packet
// Inputs  : pstData - struct to store packet
//			 ucCmdType - Command type
//			 ucCmd - Command name
//			 ulUid - Transmission Uid
//			 unLength - Length of data transmission buffer
//			 pucTlvBuffer - Data in TLV format
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool dataBuildPacket(DATA_PACKET *pstData, uint8 ucCmdType, uint8 ucCmd,
					 uint32 ulUid, uint16 unLength, uint8 *pucTlvBuffer)
{
    bool blCheck = false;
    uint8 ucIterator = 0;
	uint8 ucFlag = 0;


	if ((0 < unLength) && (NULL == pucTlvBuffer))
	{
		Serial.println("TLV data mismatch");
	}
	else
	{
		if (NULL != pstData)
		{
			pstData->ucCmdType = ucCmdType;
			pstData->ucCmd = ucCmd;
			pstData->ulUid = ulUid;
			pstData->unLength = unLength;
			pstData->unChecksum = ucCmdType + ucCmd + ulUid + unLength;

			if (0 < unLength)
			{
				pstData->pucData = (uint8*)malloc(unLength);

				if (NULL != pstData->pucData)
				{
					memcpy(pstData->pucData, pucTlvBuffer, unLength);
				}
				else
				{
					pstData->pucData = NULL;
					ucFlag = 1;
				}

				for (ucIterator = 0; ucIterator < unLength; ucIterator ++)
				{
					pstData->unChecksum += pucTlvBuffer[ucIterator];
				}
			}

			if (0 == ucFlag)
			{
				blCheck = true;
			}
		}

	}

	if (true != blCheck)
	{
		Serial.println("Failed to build TLV packet");
	}

    return blCheck;
}

//********************************.dataExtract.*********************************
// Purpose : Function to extract meaningful data from TLV buffer
// Inputs  : pulData - Stores data after extraction
//			 pucType - Stores the type of data
//			 pucTlvBuffer - The TLV buffer from which data is extracted
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool dataExtract(size_t *pulData, uint8 *pucType, uint8 *pucTlvBuffer)
{
    bool blCheck = false;
    uint8 ucLength = 0;

    if ((NULL != pulData) && (NULL != pucType) && (NULL != pucTlvBuffer))
    {
        *pucType = pucTlvBuffer[DATA_TYPE];
        ucLength = pucTlvBuffer[DATA_LENGTH];

        if (0 < ucLength)
        {
			memcpy(pulData, &pucTlvBuffer[DATA_VALUE], ucLength);
			blCheck = true;
        }
    }

    if (true != blCheck)
	{
		Serial.println("Failed to extract data");
	}

    return blCheck;
}

//********************************.dataBuilder.*********************************
// Purpose : Function to build data for transmission over UART
// Inputs  : pucBuffer - Buffer to transmit data over UART
//			 stData - Data packet
//			 ucSize - Size of the data buffer
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool dataBuilder(uint8 *pucBuffer, DATA_PACKET stData, uint8 ucSize)
{
	bool blCheck = false;

	if ((NULL != pucBuffer) && (MIN_BUFFER_SIZE <= ucSize))
	{
		pucBuffer[BUFFER_CMD_TYPE] = stData.ucCmdType;
		pucBuffer[BUFFER_CMD] = stData.ucCmd;
		memcpy(&pucBuffer[BUFFER_UID], &stData.ulUid, sizeof(uint32));
		memcpy(&pucBuffer[BUFFER_LENGTH], &stData.unLength, sizeof(uint16));
		memcpy(&pucBuffer[BUFFER_CHECKSUM], &stData.unChecksum, sizeof(uint16));

		if ((0 < stData.unLength) && (NULL != stData.pucData) &&
			(BUFFER_DATA + stData.unLength <= ucSize))
		{
			memcpy(&pucBuffer[BUFFER_DATA], stData.pucData, stData.unLength);
		}

		blCheck = true;
	}
	else
	{
		Serial.println("Failed to build data for transmission");
	}

	return blCheck;
}

//*******************************.dataParser.***********************************
// Purpose : Function to parse received data buffer
// Inputs  : pstData - Stores the received data as a packet
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool dataParser(DATA_PACKET *pstData)
{
	bool blCheck = false;
	uint8 *pucBuffer = NULL;

	uartGetData(&pucBuffer);

	if ((NULL != pucBuffer) && (NULL != pstData))
	{
		pstData->ucCmdType = pucBuffer[BUFFER_CMD_TYPE];
		pstData->ucCmd = pucBuffer[BUFFER_CMD];
		memcpy(&pstData->ulUid, &pucBuffer[BUFFER_UID], sizeof(uint32));
		memcpy(&pstData->unLength, &pucBuffer[BUFFER_LENGTH], sizeof(uint16));
		memcpy(&pstData->unChecksum, &pucBuffer[BUFFER_CHECKSUM], 
			sizeof(uint16));

		if ((0 < pstData->unLength) && (NULL == pstData->pucData) &&
				(MIN_BUFFER_SIZE + pstData->unLength <= MAX_SIZE))
		{
			pstData->pucData = (uint8*)malloc(pstData->unLength);

			if (NULL != pstData->pucData)
			{
				memcpy(pstData->pucData, &pucBuffer[BUFFER_DATA], 
					pstData->unLength);
			}
		}

		blCheck = true;
	}
	else
	{
		Serial.println("Unable to parse received data");
	}

	return blCheck;
}

//***************************.dataVerifyChecksum.*******************************
// Purpose : Function to verify checksum of received data
// Inputs  : stData - Data packet
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool dataVerifyChecksum(DATA_PACKET stData)
{
	bool blCheck = false;
	uint8 ucIterator = 0;
	uint32 ulSum = 0;

	ulSum = stData.ucCmdType + stData.ucCmd + stData.ulUid + stData.unLength;

	if ((0 < stData.unLength) && (NULL != stData.pucData))
	{
		for (ucIterator = 0; ucIterator < stData.unLength; ucIterator ++)
		{
			ulSum += stData.pucData[ucIterator];
		}
	}

	if (ulSum == stData.unChecksum)
	{
		blCheck = true;
	}
	else
	{
		Serial.println("Failed checksum");
	}

	return blCheck;
}

// EOF