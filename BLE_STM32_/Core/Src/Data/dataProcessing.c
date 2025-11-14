//**************************** Data Processing *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : dataProcessing.c
// Summary : Handles different processes to be executed
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "dataProcessing.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static IWDG_HandleTypeDef *pstHiwdg = NULL;
static BLE_SENSOR stReadings = {0};
static uint32 ulCurrentUid = 0;
static uint16 unSum = 0;
static uint16 unDelay = TIMER_DELAY;
static uint16 unTimeout = TIMER_DELAY;

//***************************** Local Functions ********************************
static bool readSensorData(BLE_SENSOR *pstReadings);
static bool sendSensorData(DATA_PACKET *pstTelemetry, uint32 *pulReadings);
static bool getTimeout(DATA_PACKET *pstTimeout);

//*****************************.dataSetValue.************************************
// Purpose : Function to set value of global variables
// Inputs  : pstWatchDogHandle - watchdog handle
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
bool dataSetValue(IWDG_HandleTypeDef *pstWatchDogHandle)
{
	bool blCheck = false;

	if (NULL != pstWatchDogHandle)
	{
		pstHiwdg = pstWatchDogHandle;
		blCheck = true;
	}

	return blCheck;
}


//****************************.sendPingRequest.*********************************
// Purpose : Function to send ping request
// Inputs  : pstPing - Ping data packet
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool sendPingRequest(DATA_PACKET *pstPing)
{
	bool blCheck = false;
	uint8 ucTransmitBuffer[MIN_BUFFER_SIZE] = {0};

	if (NULL != pstPing)
	{
		// build data for transmission
		dataBuildPacket(pstPing, CMD_REQ, CMD_PING, ulCurrentUid, 0, NULL);
		dataBuilder(ucTransmitBuffer, *pstPing, MIN_BUFFER_SIZE);

		// transmit data packet
		uartTransmit(ucTransmitBuffer, MIN_BUFFER_SIZE);
		printf("[%d]Transmitted ping\r\n", ulCurrentUid);
		blCheck = true;
	}
	else
	{
		printf("Failed to send ping\r\n");
	}

	return blCheck;
}

//**************************.receivePingResponse.*******************************
// Purpose : Function to receive ping response
// Inputs  : pstPing - Ping data packet
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool receivePingResponse(DATA_PACKET *pstPing)
{
	bool blCheck = false;

	if (NULL != pstPing)
	{
		// Parses data
		dataParser(pstPing, MIN_BUFFER_SIZE);

		if ((CMD_RESP == pstPing->ucCmdType) && (CMD_PING == pstPing->ucCmd)
			&& (true == dataVerifyChecksum(*pstPing)))
		{
			printf("[%d]Received Ping response, refreshing watchdog\r\n",
					ulCurrentUid);
			refreshWatchDog(pstHiwdg);
			blCheck = true;
		}
	}

	if (true != blCheck)
	{
		printf("Ping response not received\r\n");
	}

	return blCheck;
}
//***********************.receiveTelemetryRequest.******************************
// Purpose : Function to receive telemetry request
// Inputs  : pstTelemetry - Telemetry data packet
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool receiveTelemetryRequest(DATA_PACKET *pstTelemetry)
{
	bool blCheck = false;

	if (NULL != pstTelemetry)
	{
		// Parses data
		dataParser(pstTelemetry, MIN_BUFFER_SIZE);

		if ((CMD_REQ == pstTelemetry->ucCmdType) &&
				(CMD_TELM == pstTelemetry->ucCmd) &&
				(true == dataVerifyChecksum(*pstTelemetry)))
		{
			printf("[%d]Received telemetry request, sending sensor "
					"readings\r\n", ulCurrentUid);
			blCheck = true;
		}
	}

	if (true != blCheck)
	{
		printf("Telemetry request not received\r\n");
	}

	return blCheck;
}

//************************.sendTelemetryResponse.*******************************
// Purpose : Function to send telemetry response
// Inputs  : pstTelemetry - Telemetry data packet
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool sendTelemetryResponse(DATA_PACKET *pstTelemetry)
{
	bool blCheck = false;
	uint32 ulReadings[MAX_SENSOR_DATA] = {0};

	if (NULL != pstTelemetry)
	{
		if ((REQUIRED_DELAY <= unDelay) || (REQUIRED_DELAY <= unSum))
		{
			readSensorData(&stReadings);
			unSum = 0;
		}
		else
		{
			unSum += unDelay;
		}

		// format sensor reading to readable data
		dataFormat(&stReadings, ulReadings);

		// send data over UART
		sendSensorData(pstTelemetry, ulReadings);
		blCheck = true;
	}
	else
	{
		printf("Failed to send telemetry response\r\n");
	}

	if (NULL != pstTelemetry->pucData)
	{
		free(pstTelemetry->pucData);
	}

	return blCheck;

}
//***************************.readSensorData.***********************************
// Purpose : Function to read data from DHT11 sensor
// Inputs  : pstReadings - pointer to struct to store sensor readings
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
static bool readSensorData(BLE_SENSOR *pstReadings)
{
	bool blCheck = false;

	if (NULL != pstReadings)
	{
		sensorStart();

		// wait for STM32 to read data
		milliDelay(THREAD_WAIT);

		//get read data
		sensorReadings(pstReadings);

		blCheck = true;
	}
	else
	{
		perror("Failed to read sensor data\r\n");
	}

	return blCheck;
}

//***************************.sendSensorData.***********************************
// Purpose : Function to send sensor data to ESP32
// Inputs  : pstTelemetry - Telemetry data packet
//			 pulReadings - Temperature and humidity readings
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
static bool sendSensorData(DATA_PACKET *pstTelemetry, uint32 *pulReadings)
{
	bool blCheck = false;
	uint8 ucIterator = 0;
	uint8 *pucTlvData = NULL;
	uint8 ucTransmitBuffer[MIN_BUFFER_SIZE + SENSOR_DATA_SIZE] = {0};
	uint16 unOffset = 0;
	uint8 ucType[MAX_SENSOR_DATA] = {TYPE_TEMP, TYPE_HUMD};

	if ((NULL != pstTelemetry) && (NULL != pulReadings))
	{
		// allocate memory
		pucTlvData = malloc(SENSOR_DATA_SIZE);

		if (NULL != pucTlvData)
		{
			// convert data to TLV format
			for (ucIterator = 0; ucIterator < MAX_SENSOR_DATA; ucIterator ++)
			{
				unOffset = ucIterator * DATA_OFFSET;
				dataTlv(pulReadings[ucIterator], pucTlvData + unOffset,
						ucType[ucIterator], sizeof(uint32));
			}

			// build data for transmission
			dataBuildPacket(pstTelemetry, CMD_RESP, CMD_TELM,
					 ulCurrentUid, SENSOR_DATA_SIZE, pucTlvData);
			dataBuilder(ucTransmitBuffer, *pstTelemetry,
					MIN_BUFFER_SIZE + SENSOR_DATA_SIZE);

			// transmit data packet
			uartTransmit(ucTransmitBuffer, MIN_BUFFER_SIZE + SENSOR_DATA_SIZE);

			blCheck = true;
		}
		else
		{
			perror("NULL check failed");
		}
	}

	// free memory
	if (NULL != pucTlvData)
	{
		free(pucTlvData);
	}

	if (true != blCheck)
	{
		printf("Failed to send Sensor data\r\n");
	}

	return blCheck;
}

//************************.sendTimeoutRequest.**********************************
// Purpose : Function to send timeout request
// Inputs  : pstTimeout - Timeout data packet
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool sendTimeoutRequest(DATA_PACKET *pstTimeout)
{
	bool blCheck = false;
	uint8 ucTransmitBuffer[MIN_BUFFER_SIZE] = {0};

	if (NULL != pstTimeout)
	{
		// build data for transmission
		dataBuildPacket(pstTimeout, CMD_REQ, CMD_TIME, ulCurrentUid, 0, NULL);
		dataBuilder(ucTransmitBuffer, *pstTimeout, MIN_BUFFER_SIZE);
		printf("[%d]sending Timeout request\r\n", ulCurrentUid);

		// transmit data packet
		uartTransmit(ucTransmitBuffer, MIN_BUFFER_SIZE);
		blCheck = true;
	}
	else
	{
		printf("Failed to send Timeout request\r\n");
	}

	return blCheck;
}

//*************************.receiveTimeoutResponse.*****************************
// Purpose : Function to receive timeout response
// Inputs  : pstTimeout - Timeout data packet
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool receiveTimeoutResponse(DATA_PACKET *pstTimeout)
{
	bool blCheck = false;

	if (NULL != pstTimeout)
	{
		// Parses data
		dataParser(pstTimeout, MIN_BUFFER_SIZE + TIMEOUT_DATA_SIZE);

		if ((CMD_RESP == pstTimeout->ucCmdType) &&
				(CMD_TIME == pstTimeout->ucCmd) &&
			(true == dataVerifyChecksum(*pstTimeout)))
		{
			printf("[%d]Received Timeout response, delaying process\r\n",
					ulCurrentUid);
			getTimeout(pstTimeout);
			blCheck = true;
		}
	}

	if (NULL != pstTimeout->pucData)
	{
		free(pstTimeout->pucData);
	}

	if (true != blCheck)
	{
		printf("Timeout response not received\r\n");
	}

	return blCheck;
}

//******************************.getTimeout.************************************
// Purpose : Function to get Timeout value and change watchdog timeout
// Inputs  : pstTimeout - Timeout data packet
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
static bool getTimeout(DATA_PACKET *pstTimeout)
{
	bool blCheck = false;
	uint8 ucType = 0;

	if (NULL != pstTimeout)
	{
		dataExtract(&unDelay, &ucType, pstTimeout->pucData);

		if (TYPE_TIME == ucType)
		{
			if (unTimeout != unDelay)
			{
				changeWatchdog(pstHiwdg, unDelay);
				unTimeout = unDelay;
			}

			blCheck = true;
		}
		else
		{
			printf("Received Data is not correct\r\n");
		}
	}

	if (true != blCheck)
	{
		printf("Failed to get timeout value\r\n");
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
		milliDelay(unDelay);
		blCheck = true;
	}
	else
	{
		printf("Failed to delay process\r\n");
	}

	ulCurrentUid ++;

	return blCheck;
}

//EOF
