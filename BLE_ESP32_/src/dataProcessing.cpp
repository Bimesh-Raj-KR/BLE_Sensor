//**************************** Data processing *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : dataProcessing.cpp
// Summary : Contains functions to process received data
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "dataProcessing.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static DATA_PACKET *pstData = NULL;
static uint32 ulCurrentUid = 0;
static uint16 unDelay = DEFAULT_DELAY;

//***************************** Local Functions ********************************
static bool sendSensorOverBle(DATA_PACKET *pstTelemetry);
static bool sendVersionOverBle(DATA_PACKET *pstVersion);

//*******************************.setValue.*************************************
// Purpose : Function to set value of received Data packet
// Inputs  : pstReceivedData - received data packet
// Outputs : None
// Return  : true if no error else false
// Notes   : None
//******************************************************************************
bool setValue(DATA_PACKET *pstReceivedData)
{
	bool blCheck = false;
	pstData = pstReceivedData;

	if (NULL != pstData)
	{
		blCheck = true;
	}

	return blCheck;
}

//***********************************.setUid.***********************************
// Purpose : Function to set Uid of transmission
// Inputs  : ulUid - transmission uid
// Outputs : None
// Return  : true if no error else false
// Notes   : None
//******************************************************************************
void setUid(uint32 ulUid)
{
	ulCurrentUid = ulUid;
}

//*************************.sendPingResponse.***********************************
// Purpose : Function to send ping response
// Inputs  : None
// Outputs : None
// Return  : true if no error else false
// Notes   : None
//******************************************************************************
bool sendPingResponse(void)
{
	bool blCheck = false;
	DATA_PACKET stPing = {0};
	uint8 ucPingBuffer[MIN_BUFFER_SIZE] = {0};

	// build data for transmission
	dataBuildPacket(&stPing, CMD_RESP, CMD_PING, ulCurrentUid, 0, NULL);

	if (true == dataBuilder(ucPingBuffer, stPing, MIN_BUFFER_SIZE))
	{
		Serial.printf("[%d]", ulCurrentUid);
		Serial.println("Received ping request, Sending ping response");

		// transmit data packet
		uartTransmit(ucPingBuffer, MIN_BUFFER_SIZE);
		blCheck = true;
	}
	else
	{
		Serial.println("Failed to send Ping response");
	}

	return blCheck;
}

//***********************.sendTelemetryRequest.*********************************
// Purpose : Function to send telemetry request
// Inputs  : None
// Outputs : None
// Return  : true if no error else false
// Notes   : None
//******************************************************************************
bool sendTelemetryRequest(void)
{
	bool blCheck = false;
	DATA_PACKET stTelemetry = {0};
	uint8 ucTelemetryBuffer[MIN_BUFFER_SIZE] = {0};

	// build data for transmission
	dataBuildPacket(&stTelemetry, CMD_REQ, CMD_TELM, ulCurrentUid, 0, NULL);

	if (true == dataBuilder(ucTelemetryBuffer, stTelemetry, MIN_BUFFER_SIZE))
	{
		// transmit data packet
		uartTransmit(ucTelemetryBuffer, MIN_BUFFER_SIZE);
		blCheck = true;
	}

	if (true != blCheck)
	{
		Serial.println("Failed to send Telemetry request");
	}

	return blCheck;
}

//**************************.processTelemetryResponse.**************************
// Purpose : Function to send process telemetry response
// Inputs  : None
// Outputs : None
// Return  : true if no error else false
// Notes   : None
//******************************************************************************
bool processTelemetryResponse(void)
{
	bool blCheck = false;

	if (NULL != pstData)
	{
		if (NULL != pstData->pucData)
		{
			sendSensorOverBle(pstData);
			blCheck = true;
		}
	}

	if (true != blCheck)
	{
		Serial.println("Failed to receive Telemetry response");
	}

	return blCheck;
}

//*************************.sendTimeoutResponse.********************************
// Purpose : Function to send timeout response
// Inputs  : None
// Outputs : None
// Return  : true if no error else false
// Notes   : None
//******************************************************************************
bool sendTimeoutResponse(void)
{
	bool blCheck = false;
	DATA_PACKET stTimeout = {0};
	uint8 ucTlvData[TIMEOUT_DATA_SIZE] = {0};
	uint8 ucTransmitBuffer[MIN_BUFFER_SIZE + TIMEOUT_DATA_SIZE] = {0};

	bleGetDelay(&unDelay);

	// build data for transmission
	dataTlv((uint8*)&unDelay, ucTlvData, TYPE_TIME, sizeof(uint16));
	dataBuildPacket(&stTimeout, CMD_RESP, CMD_TIME, 
					ulCurrentUid, TIMEOUT_DATA_SIZE, ucTlvData);
	dataBuilder(ucTransmitBuffer, stTimeout, MIN_BUFFER_SIZE +
				TIMEOUT_DATA_SIZE);

	Serial.printf("[%d]", ulCurrentUid);
	Serial.println("Received timeout request, Sending timeout");

	// transmit data packet
	if (true == uartTransmit(ucTransmitBuffer, MIN_BUFFER_SIZE + 
			TIMEOUT_DATA_SIZE))
	{
		blCheck = true;
	}

	if (true != blCheck)
	{
		Serial.println("Failed to send Timeout response");
	}

	return blCheck;
}

//*************************.sendVersionRequest.*********************************
// Purpose : Function to send version request
// Inputs  : None
// Outputs : None
// Return  : true if no error else false
// Notes   : None
//******************************************************************************
bool sendVersionRequest(void)
{
	bool blCheck = false;
	DATA_PACKET stVersion = {0};
	uint8 ucTelemetryBuffer[MIN_BUFFER_SIZE] = {0};

	// build data for transmission
	dataBuildPacket(&stVersion, CMD_REQ, CMD_VERS, ulCurrentUid, 0, NULL);

	if (true == dataBuilder(ucTelemetryBuffer, stVersion, MIN_BUFFER_SIZE))
	{
		// transmit data packet
		uartTransmit(ucTelemetryBuffer, MIN_BUFFER_SIZE);
		blCheck = true;
	}

	if (true != blCheck)
	{
		Serial.println("Failed to send Version request");
	}

	return blCheck;
}

//***************************.processVersionResponse.***************************
// Purpose : Function to send process version response
// Inputs  : None
// Outputs : None
// Return  : true if no error else false
// Notes   : None
//******************************************************************************
bool processVersionResponse(void)
{
	bool blCheck = false;

	if (NULL != pstData)
	{
		if (NULL != pstData->pucData)
		{
			sendVersionOverBle(pstData);
			blCheck = true;
		}
	}

	if (true != blCheck)
	{
		Serial.println("Failed to receive Telemetry response");
	}

	return blCheck;
}

//******************************.delayProcess.**********************************
// Purpose : Function to delay the process
// Inputs  : None
// Outputs : None
// Return  : true if delay is non zero else false
// Notes   : None
//******************************************************************************
bool delayProcess(void)
{
	bool blCheck = false;

	if (0 != unDelay)
	{
		Serial.printf("[%d]", ulCurrentUid);
		Serial.println("Delaying process");
		uartDelay(unDelay);
		blCheck = true;
	}
	else
	{
		Serial.println("Failed to delay process");
	}

	return blCheck;
}

//***************************.sendSensorOverBle.********************************
// Purpose : Function to send sensor readings over BLE
// Inputs  : pstTelemetry - Telemetry data packet
// Outputs : None
// Return  : true if no error else false
// Notes   : None
//******************************************************************************
static bool sendSensorOverBle(DATA_PACKET *pstTelemetry)
{
	bool blCheck = false;
	uint8 ucIterator = 0;
	uint8 ucType[MAX_DATA] = {0};
	uint16 unOffset = 0;
	uint32 ulReadings[MAX_DATA] = {0};

	if ((CMD_RESP == pstTelemetry->ucCmdType))
	{
		Serial.printf("[%d]", ulCurrentUid);
		Serial.println("Sensor Readings received successfully");

		for (ucIterator = 0; ucIterator < MAX_DATA; ucIterator ++)
		{
			unOffset = ucIterator * DATA_OFFSET;
			dataExtract((uint8*)&ulReadings[ucIterator], &ucType[ucIterator], 
						pstTelemetry->pucData + unOffset);
		}

		bleTransmit(ulReadings, ucType);
		blCheck = true;
	}
	else
	{
		Serial.println("Failed to send sensor data over BLE");
	}

	return blCheck;
}

//***************************.sendVersionOverBle.*******************************
// Purpose : Function to send version details over BLE
// Inputs  : pstVersion - version data packet
// Outputs : None
// Return  : true if no error else false
// Notes   : None
//******************************************************************************
static bool sendVersionOverBle(DATA_PACKET *pstVersion)
{
	bool blCheck = false;
	uint8 ucIterator = 0;
	uint8 ucType = 0;
	uint16 unOffset = 0;
	uint8 ucVersion[VERSION_SIZE] = {0};

	if ((CMD_RESP == pstVersion->ucCmdType))
	{
		Serial.printf("[%d]", ulCurrentUid);
		Serial.println("STM32 version received successfully");

		// Extract version
		dataExtract(ucVersion, &ucType, pstVersion->pucData);
		bleStringTransmit(ucVersion, ucType);
		blCheck = true;
	}
	else
	{
		Serial.println("Failed to send IDE version over BLE");
	}

	return blCheck;
}

// EOF