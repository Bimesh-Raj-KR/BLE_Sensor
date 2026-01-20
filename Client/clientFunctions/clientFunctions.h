//***************************** Client Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for clientFunctions.c functions
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
#include <sys/time.h>
#include <netinet/tcp.h>

//******************************* Global Types *********************************
typedef char int8;
typedef int int32;
typedef unsigned int uint32;

//***************************** Global Constants *******************************
#define PORT                    (9075)
#define BUFFER_SIZE             (1024)
#define PATH_SIZE               (256)
#define MIN_DELAY               (10)
#define TIMEOUT_SEC             (5)
#define ERROR_CODE              (-1)
#define HOST                    "172.29.91.196"
#define FILE_FLAG               "File"
#define ADDRESS_FLAG            "Address"
#define CONVERT_FLAG            "Convert"
#define SREC_FILE_NAME          "format.srec"
#define READ_BINARY             "rb"
#define WRITE_BINARY            "wb"
#define FILE_CHOICE             'F'
#define MEMORY_CHOICE           'M'
#define CONVERT_CHOICE          'C'
#define EXIT_CHOICE             'E'

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool clientSetup(void);

//*********************** Inline Method Implementations ************************ 

#endif // CLIENTFUNCTIONS_H
// EOF