//******************************* App Functions ********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and forward 
//			 declarations needed for appFunctions.c function
// Note    : None
// 
//******************************************************************************
#ifndef APPFUNCTIONS_H
#define APPFUNCTIONS_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>

//******************************* Global Types ********************************* 
typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;

//***************************** Global Constants ******************************* 
#define LIST_FILE_NAME          "/tmp/fileOpenList.txt"
#define LOG_NAME                "countFiles"
#define APP_NAME                "node"
#define READ_COMMAND            "r"
#define SPACE                   ' '
#define TAB                     '\t'
#define PATH                    '/'
#define MAX_BUFFER_SIZE         (1054)
#define MAX_APP_SIZE            (256)


//***************************** Global Variables ******************************* 

//**************************** Forward Declarations ****************************
bool appCount(void);

//*********************** Inline Method Implementations ************************ 

#endif // APPFUNCTIONS_H
// EOF 
