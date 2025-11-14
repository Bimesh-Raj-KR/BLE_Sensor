//****************************** UART Reception ********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : uartReception.cpp
// Summary : Handles reception to UART
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "uartReception.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static uint8 ucTransmittedByte = 0;
static uint8 ucReceivedData[MAX_SIZE] = {0};
static uint8 ucIndex = 0;

//***************************** Local Functions ********************************

//*****************************.uartGetData.************************************
// Purpose : Function to share the address of received data
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool uartGetData(uint8 **ppucDataBuffer)
{
	bool blCheck = false;

	*ppucDataBuffer = ucReceivedData;

	if (NULL != *ppucDataBuffer)
	{
		blCheck = true;
	}

	return blCheck;
}

//***************************.uartReceiveByte.**********************************
// Purpose : Function to receive a single byte from UART
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void uartReceiveByte(void)
{
	ucTransmittedByte = Serial2.read();
}

//*****************************.uartStoreData.**********************************
// Purpose : Function to store the received byte in a buffer
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void uartStoreData(void)
{
	ucReceivedData[ucIndex] = ucTransmittedByte;
	ucIndex ++;
	ucTransmittedByte = 0;
}

//****************************.uartGetBuffer.***********************************
// Purpose : Function to get received received data from buffer
// Inputs  : pucDataBuffer - Stores the received data
//			 ucSize - Length of data to be stored
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool uartGetBuffer(uint8 *pucDataBuffer, uint8 ucSize)
{
	bool blCheck = false;

	if (MAX_SIZE >= ucSize)
	{
		memcpy(pucDataBuffer, ucReceivedData, ucSize);
		blCheck = true;
	}
	else
	{
		Serial.println("Failed to copy data into buffer");
	}

	return blCheck;
}

//*****************************.uartClearData.**********************************
// Purpose : Function to clear received data
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void uartClearData(void)
{
	memset(ucReceivedData, 0, MAX_SIZE);
	ucIndex = 0;
}

//****************************.uartClearBuffer.*********************************
// Purpose : Function to clear UART buffer
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void uartClearBuffer(void)
{
	while (0 != Serial2.available())
	{
		Serial2.read();
	}
}

//EOF
