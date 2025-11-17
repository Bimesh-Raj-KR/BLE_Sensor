//*************************** System Functions *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and forward
//			 declarations needed for systemFunctions.c
// Note    : None
//
//******************************************************************************
#ifndef SYSTEMFUNCTIONS_H
#define SYSTEMFUNCTIONS_H

//******************************* Include Files ********************************
#include "common.h"

//******************************* Global Types *********************************
//Data packet
#pragma pack(1)
typedef struct _DATA_PACKET_
{
	uint8 ucCmdType;
	uint8 ucCmd;
    uint32 ulUid;
    uint16 unLength;
    uint8 *pucData;
    uint8 ucChecksum;
}DATA_PACKET;
#pragma pack(0)

//***************************** Global Constants *******************************
#define RELOAD_ACCESS			(0xAAAA)
#define WRITE_ACCESS			(0x5555)
#define BAUD_RATE				(115200)
#define TIM_COUNTER				(65535)
#define MAX_RELOAD				(4095)
#define DELAY_BUFFER			(2000)
#define RELOAD_COUNTER			(780)
#define PRESCALER_VALUE			(256)
#define MAX_SLEEP               (100)
#define TIM_PRESCALER			(63)
#define FREQUENCY				(40)
#define WATCHDOG_WAIT			(5)
#define PRIORITY				(5)

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
void interruptEnable(void);
void gpioWrite(GPIO_TypeDef* pPeripheral, uint16 unGpioPin, uint16 unPinState);
void gpioToggle(GPIO_TypeDef* pPeripheral, uint16 gpioPin);
bool refreshWatchDog(IWDG_HandleTypeDef *pHiwdg);
bool checkReset(void);
void clearReset(void);
void modeChange(uint32 ulMode);
bool changeWatchdog(IWDG_HandleTypeDef *pstHiwdg, uint16 unDelay);
bool milliDelay(uint32 ulDelay);

//*********************** Inline Method Implementations ************************

#endif // SYSTEMFUNCTIONS_H
// EOF
