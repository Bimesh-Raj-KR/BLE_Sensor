//******************************** JSON Server *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for jsonServer.c functions
// Note    : None
// 
//******************************************************************************
#ifndef JSONSERVER_H
#define JSONSERVER_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <cJSON.h>

//******************************* Global Types *********************************
typedef int int32;
typedef unsigned short uint16;
typedef short int16;
typedef char int8;

//***************************** Global Constants *******************************
#define PORT                    (9080)
#define HTTP_SIZE               (4096)
#define PATH_SIZE               (128)
#define FILENAME_SIZE           (50)
#define CONTENT_SKIP            (17)
#define REQUEST_SIZE            (10)
#define OFFSET                  (4)
#define PATH_OFFSET             (2)
#define ERROR_CODE              (-1)
#define LOCAL_HOST              "127.0.0.1"
#define CRLF_STR                "\r\n\r\n"
#define COUNT                   "count"
#define GET                     "GET"
#define POST                    "POST"
#define PUT                     "PUT"
#define WRITE_MODE              "w"
#define READ_MODE               "r"
#define NULL_CHAR               '\0'
#define SPACE                   ' '
#define BACKSLASH               '/'

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool serverSetup(void);

//*********************** Inline Method Implementations ************************ 

#endif // JSONSERVER_H
// EOF 