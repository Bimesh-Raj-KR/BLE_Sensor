//***************************** Client Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for client functions
//           clientFunctions.c functions
// Note    : None
// 
//******************************************************************************
#ifndef CLIENTFUNCTIONS_H
#define CLIENTFUNCTIONS_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

//******************************* Global Types *********************************
typedef char int8;
typedef short int16;
typedef int int32;
typedef unsigned int uint32;

//***************************** Global Constants *******************************
#define PORT                    (9078)
#define BUFFER_SIZE             (4096)
#define MAX_ARG                 (3)
#define IP_ARG                  (2)
#define ERROR_CODE              (-1)
#define READ_BINARY             "rb"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool clientSetup(int8 *pArgVector[]);

//*********************** Inline Method Implementations ************************ 

#endif // CLIENTFUNCTIONS_H
// EOF