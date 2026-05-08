//***************************** Device Get Stream ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for deviceGetStream.cpp functions
// Note    : None
// 
//******************************************************************************
#ifndef DEVICE_GETSTREAM_H
#define DEVICE_GETSTREAM_H

//******************************* Include Files ********************************
#include "deviceDiscovery.h"
#include "http_parse.h"
#include "onvif_api.h"

//******************************* Global Types *********************************
typedef struct PROFILE_DETAILS
{
    int8 cResolution[RES_SIZE];
    onvif_VideoEncoding eEncoding;
}
_PROFILE_DETAILS_;

//***************************** Global Constants *******************************
#define BUFFER_SIZE             (1024)
#define TIMEOUT                 (1000)
#define PORT                    (80)
#define MSG_SIZE                (20)
#define TOKEN_SIZE              (20)
#define DEVICE_IP               "192.167.0.163"
#define USERNAME                "admin"
#define PASSWORD                "admin"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool getUri(int8* pcUri);
bool deviceStreamInit(void);

//*********************** Inline Method Implementations ************************ 

#endif // DEVICE_GETSTREAM_H
// EOF