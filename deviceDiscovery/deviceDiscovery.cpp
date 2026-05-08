//***************************** Device Discovery *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : deviceDiscovery.cpp
// Summary : Discover all devices connected to the local network
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 16/Mar/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "deviceDiscovery.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static int8 scIpList[MAX_IPS][IP_LENGTH] = {0};
static int8 scIpCount = 0;

//***************************** Local Functions ********************************
static bool discoverAndStore(int8 *pcIp);

//****************************.discoverDevice.**********************************
// Purpose : Function to discover all devices by sending probe request
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void discoverDevice(void)
{
    uint8 ucChoice = 0;
    int32 lInterval = 0;
    int8 cIpAddr[IP_LENGTH] = {0};
    onvif_probe_cb callBackFunc = 0;

    lInterval = MAX_INTERVAL;
    strcpy(cIpAddr, IP_ADDR);

    callBackFunc = discoveryHandler;
    set_probe_cb(callBackFunc, NULL);
    set_monitor_cb(NULL, 0, callBackFunc, NULL);
    start_probe(cIpAddr, lInterval);
    sleep(MIN_DELAY);

    stop_probe();
}

//*****************************.discoveryHandler.*******************************
// Purpose : Callback function that triggers when a device is discovered.
// Inputs  : response - Info about the discovered device
//           messageType - Type of event that happened
//           Data - Pointer to userdata
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void discoveryHandler(DEVICE_BINFO * response, int messageType, void * Data)
{
    if (NULL != response) 
    {
        discoverAndStore(response->XAddr.host);
    }
}

//*****************************.discoverPrint.**********************************
// Purpose : Function to print IP of all discovered devices
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool discoverPrint(void)
{
    bool blCheck = false;
    int8 cIndex = 0;

    if (0 < scIpCount)
    {
        printf("Discovered devices are\n");

        for (cIndex = 0; cIndex < scIpCount; cIndex ++)
        {
            printf("%s\n", scIpList[cIndex]);
        }

        blCheck = true;
    }
    else
    {
        printf("No device was discovered\n");
    }

    return blCheck;
}

//*****************************.discoverCheck.**********************************
// Purpose : Function to Check if a particular device was discovered.
// Inputs  : pcIp - IP of the particular device
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool discoverCheck(int8 *pcIp)
{
    bool blCheck = false;
    int8 cIndex = 0;

    if (NULL != pcIp)
    {
        for (cIndex = 0; cIndex < scIpCount; cIndex ++)
        {
            if (0 == strcmp(scIpList[cIndex], pcIp))
            {
                blCheck = true;
                break;
            }
        }
    }

    if (true == blCheck)
    {
        printf("Device present in Discovered list\n");
    }
    else
    {
        printf("Device not present in Discovered list\n");
    }

    return blCheck;
}

//*****************************.discoverAndStore.*******************************
// Purpose : Function to store the discovered devices' IP
// Inputs  : pcIp - IP of the discovered device
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool discoverAndStore(int8 *pcIp)
{
    bool blCheck = false;
    int8 cIndex = 0;
    int8 ucFlag = 0;

    if ((NULL != pcIp) && (scIpCount < MAX_IPS))
    {
        if (0 < scIpCount)
        {
            for (cIndex = 0; cIndex < scIpCount; cIndex ++)
            {
                if (0 == strcmp(scIpList[cIndex], pcIp))
                {
                    ucFlag = 1;
                    break;
                }
            }

            if (0 == ucFlag)
            {
                strcpy(scIpList[scIpCount], pcIp);
                scIpCount ++;
            }
        }
        else
        {
            strcpy(scIpList[scIpCount], pcIp);
            scIpCount ++;
        }

        blCheck = true;
    }

    return blCheck;
}

// EOF