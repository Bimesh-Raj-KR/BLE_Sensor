//******************************* Check Function *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and forward 
//			 declarations needed for checkFunctions.c function
// Note    : None
// 
//******************************************************************************
#ifndef CHECKFUNCTIONS_H
#define CHECKFUNCTIONS_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h> 

//******************************* Global Types ********************************* 
typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef int int32;

//***************************** Global Constants ******************************* 
#define DIVISOR                 (2)
#define MIN_SIZE                (17)
#define MIN_LIMIT               (48)
#define MAX_LIMIT               (57)
#define NULL_CHAR               '\0'
#define NEXT_LINE               '\n'
#define PATH                    '/'

//***************************** Global Variables ******************************* 

//**************************** Forward Declarations ****************************
bool checkId(int8 *pcBuffer, uint16 unLength, int8 *pcCurrentId);
bool checkFile(int8 *pcBuffer, uint16 unLength, int8 *pcCurrentFile);
bool checkValidId(int8 *pcProcessId);

//*********************** Inline Method Implementations ************************ 

#endif // CHECKFUNCTIONS_H
// EOF 