//******************************* UART Process *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : uartProcess.cpp
// Summary : Handles basic UART operations
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "uartProcess.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//***************************.uartReceiveAndStore.******************************
// Purpose : Function to receive and store data byte by byte
// Inputs  : None
// Outputs : None
// Return  : true if data is available, else false
// Notes   : None
//******************************************************************************
bool uartReceiveAndStore(void)
{
    bool blCheck = false;
    uint8 ucIterator = 0;

    if (MIN_SIZE <= Serial2.available())
    {
        while ((0 != Serial2.available()) && (MAX_SIZE > ucIterator))
        {
            uartReceiveByte();
            uartStoreData();
            ucIterator ++;
        }

        blCheck = true;
    }

    return blCheck;
}

//*******************************.uartClear.************************************
// Purpose : Function to clear received data and UART buffer
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void uartClear(void)
{
	uartClearBuffer();
    uartClearData();
}

//******************************.uartTransmit.**********************************
// Purpose : Function to transmit Data over UART
// Inputs  : None
// Outputs : None
// Return  : true if data to be transmitted is within limit else false
// Notes   : None
//******************************************************************************
bool uartTransmit(uint8 *pucDataBuffer, uint8 ucSize)
{
    bool blCheck = false;

    if (MAX_SIZE >= ucSize && (MIN_SIZE <= ucSize))
    {
        Serial2.write(pucDataBuffer, ucSize);
        blCheck = true;
    }

    return blCheck;
}

//*********************************.uartDelay.**********************************
// Purpose : Function to delay the code
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void uartDelay(uint16 ulDelay)
{
    delay(ulDelay);
}

// EOF