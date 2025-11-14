//****************************** Data Transmission *****************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : dataTransmission.c
// Summary : Wrapper for necessary STM32 functions
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "dataTransmission.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static uint8 *pucReceiverBuffer = NULL;

//***************************** Local Functions ********************************

//**********************************.getValue.**********************************
// Purpose : Function to get receiver buffer
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool getValue(void)
{
	bool blCheck = false;

	if (true == getBufferValue(&pucReceiverBuffer))
	{
		blCheck = true;
	}

	return blCheck;
}
//********************************.dataFormat.**********************************
// Purpose : Function to format sensor data to be send over UART
// Inputs  : pstReadings - Contains sensor readings
//			 pulReadings - Stores formatted data
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool dataFormat(BLE_SENSOR *pstReadings, uint32 *pulReadings)
{
    bool blCheck = false;
    uint8 ucSum = 0;

    if ((NULL != pstReadings) && (NULL != pulReadings))
    {
    	ucSum = pstReadings->ucHumidInt + pstReadings->ucHumidDec +
    			pstReadings->ucTempInt + pstReadings->ucTempDec;


        if ((pstReadings->ucCheckSum == ucSum) &&
        		(0 != pstReadings->ucCheckSum))
        {
        	pulReadings[0] = (((uint32)pstReadings->ucTempInt * MAX_MULTIPLIER)
        					+ (uint32)pstReadings->ucTempDec) * MAX_MULTIPLIER;
        	pulReadings[1] = (((uint32)pstReadings->ucHumidInt * MAX_MULTIPLIER)
        					+ (uint32)pstReadings->ucHumidDec) * MAX_MULTIPLIER;
            blCheck = true;
        }
    }
    else
    {
        perror("NULL check Failed\r\n");
    }

    if (true != blCheck)
	{
    	printf("Failed to format sensor readings\r\n");
	}

    return blCheck;
}

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
bool dataTlv(uint32 ulData, uint8 *ucTlvBuffer,
                uint8 ucType, uint8 ucLength)
{
    bool blCheck = false;

    if ((NULL != ucTlvBuffer) && (0 < ucLength))
    {
        ucTlvBuffer[DATA_TYPE] = ucType;
        ucTlvBuffer[DATA_LENGTH] = ucLength;
        memcpy(&ucTlvBuffer[DATA_VALUE], &ulData, ucLength);
        blCheck = true;
    }

    if (true != blCheck)
	{
    	printf("Failed to convert data to TLV\r\n");
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
		printf("TLV data mismatch\r\n");
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
		perror("Failed to build TLV packet");
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
bool dataExtract(uint16 *punData, uint8 *pucType, uint8 *pucTlvBuffer)
{
    bool blCheck = false;
    uint8 ucLength = 0;

    if ((NULL != punData) && (NULL != pucType) && (NULL != pucTlvBuffer))
    {
        *pucType = pucTlvBuffer[DATA_TYPE];
        ucLength = pucTlvBuffer[DATA_LENGTH];

        if (0 < ucLength)
        {
			memcpy(punData, &pucTlvBuffer[DATA_VALUE], ucLength);
			blCheck = true;
        }
    }

    if (true != blCheck)
	{
    	printf("Failed to extract data\r\n");
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

	if ((NULL != pucBuffer) && (9 <= ucSize))
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
		printf("Failed to build data for transmission\r\n");
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
bool dataParser(DATA_PACKET *pstData, uint8 ucSize)
{
	bool blCheck = false;

	if ((NULL != pucReceiverBuffer) && (NULL != pstData))
	{
		pstData->ucCmdType = pucReceiverBuffer[BUFFER_CMD_TYPE];
		pstData->ucCmd = pucReceiverBuffer[BUFFER_CMD];
		memcpy(&pstData->ulUid, &pucReceiverBuffer[BUFFER_UID], sizeof(uint32));
		memcpy(&pstData->unLength, &pucReceiverBuffer[BUFFER_LENGTH],
				sizeof(uint16));
		memcpy(&pstData->unChecksum, &pucReceiverBuffer[BUFFER_CHECKSUM],
						sizeof(uint16));

		if ((0 < pstData->unLength) && (NULL == pstData->pucData) &&
				(MIN_BUFFER_SIZE + pstData->unLength <= ucSize))
		{
			pstData->pucData = malloc(pstData->unLength);

			if (NULL != pstData->pucData)
			{
				memcpy(pstData->pucData, &pucReceiverBuffer[BUFFER_DATA],
						pstData->unLength);
			}
		}

		blCheck = true;
	}
	else
	{
		printf("Unable to parse received data\r\n");
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
		printf("Failed checksum\r\n");
	}

	return blCheck;
}
