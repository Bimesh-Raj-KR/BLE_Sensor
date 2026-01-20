//********************************* CGI Parser *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for cgiParser.c functions
// Note    : None
// 
//******************************************************************************
#ifndef CGIPARSER_H
#define CGIPARSER_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

//******************************* Global Types *********************************
typedef int int32;
typedef unsigned short uint16;
typedef short int16;
typedef char int8;

//***************************** Global Constants *******************************
#define PORT                    (9080)
#define BUFFER_SIZE             (1024)
#define PAYLOAD_SIZE            (512)
#define FILENAME_SIZE           (50)
#define FILE_OFFSET             (5)
#define ERROR_CODE              (-1)
#define TYPE_JSON               "application/json"
#define LOCAL_HOST              "127.0.0.1"
#define SUCCESS_CASE            "SUCCESS"
#define FILE_NAME               "file="
#define GET                     "GET"
#define POST                    "POST"
#define PUT                     "PUT"
#define NULL_CHAR               '\0'
#define BACKSLASH               '/'
#define AMPERSAND               '&'

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool cgiBridgeSetup(void);

//*********************** Inline Method Implementations ************************ 

#endif // CGIPARSER_H
// EOF 