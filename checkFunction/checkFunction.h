//******************************* Check Function *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and forward 
//			 declarations needed for checkFunction.c function
// Note    : None
// 
//******************************************************************************
#ifndef CHECKFUNCTION_H
#define CHECKFUNCTION_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//******************************* Global Types ********************************* 
typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;

//***************************** Global Constants ******************************* 
#define DIVISOR                 (2)
#define MIN_SIZE                (19)
#define MIN_LIMIT               (48)
#define MAX_LIMIT               (57)
#define SPACE                   (32)
#define NULL_CHAR               '\0'
#define NEXT_LINE               '\n'
#define PATH                    '/'

//***************************** Global Variables ******************************* 

//**************************** Forward Declarations ****************************
bool checkId(int8 *pcBuffer, uint16 unLength, int8 *pcCurrentId);
bool checkFile(int8 *pcBuffer, uint16 unLength, int8 *pcCurrentFile);
bool checkValidId(int8 *pcProcessId);
void clearRest(void);

//*********************** Inline Method Implementations ************************ 

#endif // CHECKFUNCTION_H
// EOF 