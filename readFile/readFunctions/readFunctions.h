//******************************* Read Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all global constants and forward declarations required 
//           for readFunctions.c functions
// Note    : None
// 
//******************************************************************************
#ifndef READ_FUNCTIONS_H
#define READ_FUNCTIONS_H

//******************************* Include Files ********************************
#include "mutexOperations.h"

//******************************* Global Types *********************************

//***************************** Global Constants *******************************
#define READ_SLEEP              (900000)
#define MAX_COUNT               (4)
#define READ_COMMAND            "r"
#define NEWLINE_CHAR           '\n'

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool assignSharedMemory(void);
bool readFromFile(void);
bool destroySharedMemory(void);

//*********************** Inline Method Implementations ************************ 

#endif // READ_FUNCTIONS_H
// EOF