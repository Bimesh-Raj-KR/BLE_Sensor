//***************************** BLE Communication ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : bleCommunication.cpp
// Summary : Facilitates BLE communication with between server and client
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "bleCommunication.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static BLECharacteristic *pHumidCharacteristics = NULL;
static BLECharacteristic *pTempCharacteristics = NULL;
static BLECharacteristic *pDatarateCharacteristics = NULL;

//***************************** Local Functions ********************************

//*********************************.bleSetup.***********************************
// Purpose : Function to set up BLE communication
// Inputs  : None
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleSetup() 
{
    bool blCheck = false;
    BLEServer *pServer = NULL;
    BLEService *pService = NULL;
    BLEAdvertising *pAdvertising = NULL;

    // Initialize BLE
    bleInit();

    // Create server and service
    bleServer(&pServer);
    bleService(&pService, &pServer);

    // Create characteristics
    bleCharacteristics(&pHumidCharacteristics, &pService, 
                                (uint16) HUMIDITY_UUID);
    bleCharacteristics(&pTempCharacteristics, &pService, 
                                (uint16) TEMPERATURE_UUID);
    bleWriteCharacteristics(&pDatarateCharacteristics, &pService);

    // Create callback
    bleCallback(&pDatarateCharacteristics);

    // Start service
    bleServiceStart(&pService);

    // Advertise to BLE clients
    bleAdvertising(&pAdvertising);
    bleAddService(&pAdvertising);
    bleStartAdvertising(&pAdvertising);

    if ((NULL != pServer) && (NULL != pService) && (NULL != pAdvertising))
    {
        blCheck = true;
    }
    else
    {
        Serial.println("Failed to setup BLE");
    }

    return blCheck;
}

//******************************.bleTransmit.***********************************
// Purpose : Function to set characteristics value and transmit it to the client
// Inputs  : pstReadings - Pointer to the struct containing readings
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleTransmit(uint32 *pulReadings, uint8 *pucType)
{
    bool blCheck = false;
    uint8 ucIterator = 0;
    uint32 ulHumidity = 0;
    uint32 ulTemperature = 0;
    BLECharacteristic *pCharacteristicsArray[] = {pTempCharacteristics, 
        pHumidCharacteristics};


    if ((TYPE_TEMP == pucType[0]) && (TYPE_HUMD == pucType[1]))
    {
        for (ucIterator = 0; ucIterator < MAX_DATA; ucIterator ++)
        {
            bleValueSet(&pCharacteristicsArray[ucIterator], 
                        pulReadings[ucIterator],  ucIterator);
            bleNotify(&pCharacteristicsArray[ucIterator]);
        }

        blCheck = true;
    }
    else
    {
        Serial.println("Failed to transmit over BLE");
    }

    return blCheck;
}