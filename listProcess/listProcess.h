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
#include "appFunctions.h"

//******************************* Global Types ********************************* 

//***************************** Global Constants ******************************* 
#define LIST_OPEN_FILES         "lsof"
#define WRITE_COMMAND           "w"
#define MAX_COMMAND_SIZE        (5)

//***************************** Global Variables ******************************* 

//**************************** Forward Declarations ****************************
bool listData(void);

//*********************** Inline Method Implementations ************************ 

#endif // LISTPROCESS_H
// EOF 
