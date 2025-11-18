//********************************* BLE Functions ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and forward 
//           declarations needed for bleFunctions.cpp function
// Note    : None
// 
//******************************************************************************
#ifndef BLEFUNCTIONS_H
#define BLEFUNCTIONS_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <BLE2904.h>

//******************************* Global Types *********************************
typedef char int8;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef enum
{
	TYPE_TEMP = 0x00,
	TYPE_HUMD = 0x01,
	TYPE_TIME = 0x02,
	TYPE_VERS = 0x03
}TLV_TYPES;

#pragma pack(1)
typedef struct _IEEE_FORMAT_
{
    uint8 ucFlag;
    uint8 ucFirstMantissa;
    uint8 ucMiddleMantissa;
    uint8 ucLastMantissa;
    uint8 ucExponent;
}IEEE_FORMAT;
#pragma pack(0)

//***************************** Global Constants *******************************
#define EXPONENT_VALUE          (-2)
#define MAX_LENGTH              (5)
#define MIDDLE_BITS             (8)
#define LAST_BITS               (16)
#define CELSIUS_FLAG            (0x00)
#define MAX_HEX                 (0xFF)
#define DEFAULT_DELAY           (2000)
#define MIN_DELAY               (300)
#define MAX_DELAY               (24000)
#define SERVICE_UUID 			"76189799-6010-4043-99af-79d4ae6ea773"
#define TIMEOUT_UUID			"81e7dbec-e65a-4990-aeed-af4914868a2d"
#define VERSION_UUID			"8bd244e4-adc2-40c8-a691-5f006c651218"

//***************************** Global Variables ******************************* 

//**************************** Forward Declarations ****************************
void bleInit();
bool bleServer(BLEServer **ppServer);
bool bleService(BLEService **ppService, BLEServer **ppServer);
bool bleCharacteristics(BLECharacteristic **ppCharacteristics, 
                        BLEService **ppService, uint16 unUuid);
bool bleStringCharacteristics(BLECharacteristic **ppCharacteristics, 
                        BLEService **ppService);
bool bleWriteCharacteristics(BLECharacteristic **ppCharacteristics, 
                        BLEService **ppService);
bool bleCallback(BLECharacteristic **ppCharacteristics);
bool bleServiceStart(BLEService **ppService);
bool bleAdvertising(BLEAdvertising **ppAdvertising);
bool bleAddService(BLEAdvertising **ppAdvertising);
bool bleStartAdvertising(BLEAdvertising **ppAdvertising);
bool bleValueSet(BLECharacteristic **ppCharacteristics, uint32 ulValue, 
                 uint8 ucFlag);
bool bleStringSet(BLECharacteristic **ppCharacteristics, uint8 *pucBuffer, 
                uint16 unLength);
bool bleNotify(BLECharacteristic **ppCharacteristics);
bool bleTempMeasure(IEEE_FORMAT *pstFormattedTemp, uint32 ulMantissa);
bool bleGetDelay(uint16 *punValue);

//*********************** Inline Method Implementations ************************ 

#endif // BLEFUNCTIONS_H
// EOF 