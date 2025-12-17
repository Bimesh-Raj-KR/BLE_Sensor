//***************************** Server Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for server functions
//           serverFunctions.c functions
// Note    : None
// 
//******************************************************************************
#ifndef SERVERFUNCTIONS_H
#define SERVERFUNCTIONS_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

//******************************* Global Types *********************************
typedef unsigned char uint8;
typedef char int8;
typedef short int16;
typedef int int32;
typedef unsigned int uint32;
typedef unsigned long uint64;

//***************************** Global Constants *******************************
#define PORT                    (9078)
#define BUFFER_SIZE             (1024)
#define PERMISSION              (0755)
#define FILE_PATH_SIZE          (512)
#define FILE_NAME_SIZE          (256)
#define MAX_IP_SIZE             (128)
#define ERROR_CODE              (-1)
#define GET_IP                  "hostname -I | awk '{print $1}'"
#define RECEIVED_FOLDER         "Received_Files"
#define WRITE_BINARY            "wb"
#define READ_COMMAND            "r"
#define PATH_SEPARATOR          '/'
#define NEWLINE_CHAR            '\n'
#define NULL_CHAR               '\0'

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool serverSetup(void);

//*********************** Inline Method Implementations ************************ 

#endif // SERVERFUNCTIONS_H
// EOF 