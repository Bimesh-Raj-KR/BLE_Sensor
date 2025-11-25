//******************************* List Process *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and forward 
//			 declarations needed for listProcess.c function
// Note    : None
// 
//******************************************************************************
#ifndef LISTPROCESS_H
#define LISTPROCESS_H

//******************************* Include Files ********************************
#include "checkFunction.h"

//******************************* Global Types ********************************* 

//***************************** Global Constants ******************************* 
#define LIST_FILE_NAME          "fileOpenList.txt"
#define LIST_OPEN_FILES         "lsof"
#define READ_COMMAND            "r"
#define WRITE_COMMAND           "w"
#define LINE_CHECK              "\n"
#define MAX_BUFFER_SIZE         (1054)
#define MAX_FILE_SIZE           (256)
#define MAX_PID_SIZE            (10)
#define MAX_COMMAND_SIZE        (5)

//***************************** Global Variables ******************************* 

//**************************** Forward Declarations ****************************
bool listData(void);
bool listCommand(void);

//*********************** Inline Method Implementations ************************ 

#endif // LISTPROCESS_H
// EOF 
