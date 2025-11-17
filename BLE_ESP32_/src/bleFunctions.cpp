//********************************* BLE Functions ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : bleFunctions.cpp
// Summary : Facilitates BLE communication with between server and client
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "bleFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static uint16 unDelay = DEFAULT_DELAY;

//***************************** Local Functions ********************************

//******************************.DeviceCallback.********************************
// Description : Waits for write events to occur from the BLE client
// Usage       : Reads the written data and updates delay between transmissions 
//               with that data
// Notes       : Data should be written by BLE client using little endian 
//               (preferably uint16) for proper transmission
//******************************************************************************
class DeviceCallback : public BLECharacteristicCallbacks 
{
    public : 
    void onWrite(BLECharacteristic *pCharacteristics)
    {
        std::string cData = pCharacteristics->getValue();
        memcpy(&unDelay, cData.data(), sizeof(uint16));
        Serial.printf("Tranmission Delay Changed to %d", unDelay);
        Serial.println();
    }
};

//*********************************.bleInit.************************************
// Purpose : Function to initialize the device as a BLE device
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void bleInit()
{
    BLEDevice::init("BLE Device");
}

//*******************************.bleServer.************************************
// Purpose : Function to create a BLE server
// Inputs  : ppServer - Server handle
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleServer(BLEServer **ppServer)
{
    bool blCheck = false;

    if (NULL != ppServer)
    {
        *ppServer = BLEDevice::createServer();

        if (NULL != *ppServer)
        {
            blCheck = true;
        }
    }
    else
    {
        Serial.println("NULL check failed");
    }

    if (true != blCheck)
    {
        Serial.println("Failed to create a BLE server");
    }

    return blCheck;
}

//*******************************.bleService.***********************************
// Purpose : Function to create a BLE service in the server
// Inputs  : ppServer - Server handle
//           ppService - Service Handle
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleService(BLEService **ppService, BLEServer **ppServer)
{
    bool blCheck = false;

    if ((NULL != *ppServer) && (NULL != ppService))
    {
        *ppService = (*ppServer)->createService(SERVICE_UUID);

        if (NULL != *ppService)
        {
            blCheck = true;
        }
    }
    else
    {
        Serial.println("NULL check failed");
    }

    if (true != blCheck)
    {
        Serial.println("Failed to create a BLE service");
    }

    return blCheck;
}

//**************************.bleCharacteristics.********************************
// Purpose : Function to create a BLE characteristics and set its 
//           properties and descriptor
// Inputs  : ppCharacteristics - Characteristics handle
//           ppService - Service Handle
//           unUuid - UUID of the Characteristics
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleCharacteristics(BLECharacteristic **ppCharacteristics, 
                        BLEService **ppService, uint16 unUuid)
{
    bool blCheck = false;

    if ((NULL != *ppService) && (NULL != ppCharacteristics))
    {
        *ppCharacteristics = (*ppService)->createCharacteristic(unUuid, 
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
        (*ppCharacteristics)->addDescriptor(new BLE2902());

        if (NULL != *ppCharacteristics)
        {
            blCheck = true;
        }
    }
    else
    {
        Serial.println("NULL check failed");
    }

    if (true != blCheck)
    {
        Serial.println("Failed to create BLE characteristics");
    }

    return blCheck;
}

//**************************.bleCharacteristics.********************************
// Purpose : Function to create a BLE characteristics and set its 
//           properties and descriptor
// Inputs  : ppCharacteristics - Characteristics handle
//           ppService - Service Handle
//           unUuid - UUID of the Characteristics
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleWriteCharacteristics(BLECharacteristic **ppCharacteristics, 
                        BLEService **ppService)
{
    bool blCheck = false;

    if ((NULL != *ppService) && (NULL != ppCharacteristics))
    {
        *ppCharacteristics = (*ppService)->createCharacteristic(TIMEOUT_UUID, 
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
        (*ppCharacteristics)->addDescriptor(new BLE2902());

        if (NULL != *ppCharacteristics)
        {
            blCheck = true;
        }
    }
    else
    {
        Serial.println("NULL check failed");
    }

    if (true != blCheck)
    {
        Serial.println("Failed to create a BLE characteristics with write "
                        "property");
    }

    return blCheck;
}

//**************************.bleCharacteristics.********************************
// Purpose : Function to assign the class DeviceCallback as the callback handler 
//           for the characteristics
// Inputs  : ppCharacteristics - Characteristics handle
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleCallback(BLECharacteristic **ppCharacteristics)
{
    bool blCheck = false;

    if (NULL != ppCharacteristics)
    {
        (*ppCharacteristics)->setCallbacks(new DeviceCallback());
        blCheck = true;
    }
    else
    {
        Serial.println("Failed to set callback handler");
    }

    return blCheck;
}

//****************************.bleServiceStart.*********************************
// Purpose : Function to start a service
// Inputs  : ppService - Service Handle
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleServiceStart(BLEService **ppService)
{
    bool blCheck = false;

    if (NULL != *ppService)
    {
        (*ppService)->start();
        blCheck = true;
    }
    else
    {
        Serial.println("Failed to start service");
    }

    return blCheck;
}

//*****************************.bleAdvertising.*********************************
// Purpose : Function to setup advertising
// Inputs  : ppAdvertising - Advertising Handle
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleAdvertising(BLEAdvertising **ppAdvertising)
{
    bool blCheck = false;

    if (NULL != ppAdvertising)
    {
        *ppAdvertising = BLEDevice::getAdvertising();

        if (NULL != *ppAdvertising)
        {
            blCheck = true;
        }
    }
    else
    {
        Serial.println("NULL check failed");
    }

    if (true != blCheck)
    {
        Serial.println("Failed to setup advertising");
    }

    return blCheck;
}

//******************************.bleAddService.*********************************
// Purpose : Function to add service to advertise
// Inputs  : ppAdvertising - Advertising Handle
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleAddService(BLEAdvertising **ppAdvertising)
{
    bool blCheck = false;

    if (NULL != *ppAdvertising)
    {
        (*ppAdvertising)->addServiceUUID(SERVICE_UUID);
        blCheck = true;
    }
    else
    {
        Serial.println("Failed to add service to advertise");
    }

    return blCheck;
}

//*************************.bleStartAdvertising.********************************
// Purpose : Function to start advertising service
// Inputs  : ppAdvertising - Advertising Handle
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleStartAdvertising(BLEAdvertising **ppAdvertising)
{
    bool blCheck = false;

    if (NULL != *ppAdvertising)
    {
        (*ppAdvertising)->start();
        blCheck = true;
    }
    else
    {
        Serial.println("Failed to start advertising");
    }

    return blCheck;
}

//******************************.bleValueSet.***********************************
// Purpose : Function to set value of the BLE transmission
// Inputs  : ppCharacteristics - Characteristics Handle
//`          ulValue - The characteristics value to be transmitted
//           blFlag - Flag for distinguishing temperature and humidity readings
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleValueSet(BLECharacteristic **ppCharacteristics, uint32 ulValue, 
                 uint8 ucFlag)
{
    bool blCheck = false;
    IEEE_FORMAT stFormattedTemp = {0};

    if (NULL != *ppCharacteristics)
    {
        if (0 == ucFlag)
        {
            if (true != bleTempMeasure(&stFormattedTemp, ulValue))
            {
                Serial.println("Unable to encode temperature reading");
            }

            (*ppCharacteristics)->setValue((uint8 *)&stFormattedTemp, 
                                            MAX_LENGTH);
        }
        else
        {
            (*ppCharacteristics)->setValue(ulValue);
        }

        blCheck = true;
    }
    else
    {
        Serial.println("Failed to set value to be transmitted");
    }

    return blCheck;
}

//******************************.bleNotify.*************************************
// Purpose : Function to notify client that Transmitted signal was updated
// Inputs  : ppCharacteristics - Characteristics Handle
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleNotify(BLECharacteristic **ppCharacteristics)
{
    bool blCheck = false;

    if (NULL != *ppCharacteristics)
    {
        (*ppCharacteristics)->notify();
        blCheck = true;
    }
    else
    {
        Serial.println("Failed to Notify client");
    }

    return blCheck;
}

//***************************.bleTempMeasure.***********************************
// Purpose : Function to notify client that Transmitted signal was updated
// Inputs  : pstFormattedTemp - Pointer to the struct which stores temperature 
//           value in IEEE 11073 FLOAT32 format
//           ulMantissa -  Contains temperature value
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleTempMeasure(IEEE_FORMAT *pstFormattedTemp, uint32 ulMantissa)
{
    bool blCheck = false;

    if (NULL != pstFormattedTemp)
    {
        pstFormattedTemp->ucFlag = CELSIUS_FLAG;
        pstFormattedTemp->ucFirstMantissa = ulMantissa & MAX_HEX;
        pstFormattedTemp->ucMiddleMantissa = (ulMantissa >> MIDDLE_BITS) & 
                                                MAX_HEX;
        pstFormattedTemp->ucLastMantissa = (ulMantissa >> LAST_BITS) & 
                                                MAX_HEX;
        pstFormattedTemp->ucExponent = EXPONENT_VALUE;
        blCheck = true;
    }
    else
    {
        Serial.println("Failed to format temperature to IEEE FLOAT32 format");
    }

    return blCheck;
}

//******************************.bleGetDelay.***********************************
// Purpose : Function to get the delay transmitted by the client
// Inputs  : punValue - Stores the delay value
// Outputs : None
// Return  : true if no error, else false
// Notes   : None
//******************************************************************************
bool bleGetDelay(uint16 *punValue)
{
    bool blCheck = false;

    if (NULL != punValue)
    {
        if ((MAX_DELAY >= unDelay) && (MIN_DELAY <= unDelay))
        {
            *punValue = unDelay;
        }
        else
        {
            Serial.println("Delay too large or too small, transmitting "
                            "default delay");
            *punValue = DEFAULT_DELAY;
            unDelay = DEFAULT_DELAY;
        }

        blCheck = true;
    }
    else
    {
        Serial.println("Failed to get delay value");
    }

    return blCheck;
}