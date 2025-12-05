//******************************* List Process *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contains all type definitions, global constants and forward 
//			 declarations needed for listFunctions.c function
// Note    : None
// 
//******************************************************************************
#ifndef LISTFUNCTIONS_H
#define LISTFUNCTIONS_H

//******************************* Include Files ********************************
#include "checkFunctions.h"

//******************************* Global Types ********************************* 

//***************************** Global Constants ******************************* 
#define LIST_FILE_NAME          "fileOpenList.txt"
#define LIST_OPEN_FILES         "lsof"
#define READ_COMMAND            "r"
#define WRITE_COMMAND           "w"
#define LINE_CHECK              "\n"
#define FILE_COMMAND            'f'
#define PID_COMMAND             'a'
#define FLAG_PREFIX             '-'
#define MAX_BUFFER_SIZE         (1054)
#define MAX_FILE_SIZE           (256)
#define MAX_PID_SIZE            (10)
#define MAX_COMMAND_SIZE        (5)
#define CORRECT_COUNT           (3)
#define INPUT_INDEX             (2)
#define MAX_FLAG_SIZE           (2)
#define COMMAND_INDEX           (1)

//***************************** Global Variables ******************************* 

//**************************** Forward Declarations ****************************
bool listData(void);
bool listFiles(int8 *pcProcessId);
bool listProcess(int8 *pcFileName);

//*********************** Inline Method Implementations ************************ 

#endif // LISTFUNCTIONS_H
// EOF 
