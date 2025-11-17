//**************************** UART Communication ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : uartCommunication.cpp
// Summary : Receives ping and transmits back acknowledgement
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "uartCommunication.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static DATA_PACKET stData = {0};
static uint8 ucReceiverFlag = 0;

//***************************** Local Functions ********************************

//********************************.uartSetup.***********************************
// Purpose : Function to setup UART and necessary interrupts
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
void uartSetup(void)
{
    pinMode(GPIO_LED, OUTPUT);
    pinMode(INTERRUPT_PIN, INPUT);
    Serial.begin(BAUD_RATE);
    Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
    setValue(&stData);
}

//***************************.uartCommunication.********************************
// Purpose : Function to receive ping, send telemetry, receive sensor readings, 
//           transmit the readings over BLE and send Timeout over UART
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool uartCommunication(void)
{
    bool blCheck = false;
    uint8 ucErrorFlag = 0;
    uint8 ucDelayFlag = 0;

    dataParser(&stData);
    setUid(stData.ulUid);

    if (true == dataVerifyChecksum(stData))
    {
        switch(stData.ucCmd)
        {
            case CMD_PING:

                processPing();

                break;

            case CMD_TELM:

                processTelemetry();

                break;

            case CMD_TIME:

                processTimeout();
                ucDelayFlag = 1;

                break;

            default:

                Serial.println("Invalid data");
                ucErrorFlag = 1;

                break;
        }
    }
    else
    {
        ucErrorFlag = 1;
    }

    if (NULL != stData.pucData)
    {
        free(stData.pucData);
        stData.pucData = NULL;
    }

    if (1 == ucDelayFlag)
    {
        delayProcess();
    }

    if (0 == ucErrorFlag)
    {
        blCheck = true;
    }
    else
    {
        Serial.println("Failed to process data packet");
    }

    return blCheck;
}

//******************************.uartFailure.***********************************
// Purpose : Function to reset ESP32
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void uartFailure(void)
{
    uint8 ucIterator = 0;

    Serial.println("Resetting ESP32");

    for (ucIterator = 0; ucIterator < MAX_LIMIT; ucIterator ++)
    {
      digitalWrite(GPIO_LED, HIGH);
      delay(MIN_SLEEP);
      digitalWrite(GPIO_LED, LOW);
      delay(MIN_SLEEP);
    }

    ESP.restart();
}

// EOF

