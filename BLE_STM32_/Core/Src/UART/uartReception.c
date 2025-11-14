//******************************* UART Reception *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : uartReception.c
// Summary : Handles reception to UART from console
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "uartReception.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static UART_HandleTypeDef *pstHuartOne = NULL;
static uint8 ucTransmittedByte = 0;
static uint8 ucReceivedBuffer[MAX_SIZE] = {0};
static uint8 ucReceivedData[MAX_SIZE] = {0};
static uint8 ucIndex = 0;

//***************************** Local Functions ********************************

//********************************.setValue.************************************
// Purpose : Function to share the address of UART Handles
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool receiverSetValue(UART_HandleTypeDef *pstHuart1)
{
	bool blCheck = false;

	if (NULL != pstHuart1)
	{
		pstHuartOne = pstHuart1;
		blCheck = true;
	}

	return blCheck;
}

//********************************.setValue.************************************
// Purpose : Function to share the address of UART Handles
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool getBufferValue(uint8 **ppucDataBuffer)
{
	bool blCheck = false;

	*ppucDataBuffer = ucReceivedData;

	if (NULL != *ppucDataBuffer)
	{
		blCheck = true;
	}

	return blCheck;
}

//********************************.setValue.************************************
// Purpose : Function to share the address of UART Handles
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool uartReceiveByte(void)
{
	bool blCheck = false;

	if (HAL_OK == HAL_UART_Receive_IT(pstHuartOne, &ucTransmittedByte, 1))
	{
		blCheck = true;
	}

	return blCheck;
}

//********************************.setValue.************************************
// Purpose : Function to share the address of UART Handles
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
void uartStoreData(void)
{
	ucReceivedBuffer[ucIndex] = ucTransmittedByte;
	ucIndex ++;
	ucTransmittedByte = 0;
}

//********************************.setValue.************************************
// Purpose : Function to share the address of UART Handles
// Inputs  : None
// Outputs : None
// Return  : true if no received data is terminator else false
// Notes   : None
//******************************************************************************
bool uartCheckReception(void)
{
	bool blCheck = false;

	if (MAX_SIZE == ucIndex)
	{
		memset(ucReceivedData, 0, MAX_SIZE);
		memcpy(ucReceivedData, ucReceivedBuffer, MAX_SIZE);
		memset(ucReceivedBuffer, 0, MAX_SIZE);
		ucIndex = 0;
		blCheck = true;
	}

	return blCheck;
}

//********************************.setValue.************************************
// Purpose : Function to share the address of UART Handles
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool uartTransmit(uint8 *pucDataBuffer, uint8 ucSize)
{
	bool blCheck = false;
	uint8 ucTransmitBuffer[MAX_SIZE] = {0};

	if (NULL != pucDataBuffer)
	{
		memcpy(ucTransmitBuffer, pucDataBuffer, ucSize);

		if (HAL_OK ==  HAL_UART_Transmit(pstHuartOne, ucTransmitBuffer,
				MAX_SIZE, MAX_WAIT))
		{
			blCheck = true;
		}
	}

	if (true != blCheck)
	{
		printf("Failed to transmit data\r\n");
	}

	return blCheck;
}

// EOF
