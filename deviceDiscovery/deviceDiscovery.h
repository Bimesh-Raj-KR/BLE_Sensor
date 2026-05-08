//***************************** Device Discovery *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for deviceDiscovery.cpp functions
// Note    : None
// 
//******************************************************************************
#ifndef DEVICE_DISCOVERY_H
#define DEVICE_DISCOVERY_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "onvif_probe.h"

//******************************* Global Types *********************************
typedef unsigned char uint8;
typedef char int8;
typedef short int16;
typedef int int32;
typedef unsigned int uint32;

//***************************** Global Constants *******************************
#define IP_LENGTH               (128)
#define MAX_IPS                 (20)
#define RES_SIZE                (15)
#define MAX_INTERVAL            (7)
#define MIN_DELAY               (10)
#define IP_ADDR                 "192.167.0.48"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
void discoverDevice(void);
void discoveryHandler(DEVICE_BINFO * response, int messageType, void * Data) ;
bool discoverPrint(void);
bool discoverCheck(int8 *pcIp);

//*********************** Inline Method Implementations ************************ 

#endif // DEVICE_DISCOVERY_H
// EOF