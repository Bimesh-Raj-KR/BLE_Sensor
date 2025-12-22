//******************************* Write Functions ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all global constants and forward declarations required 
//           for writeFunctions.c functions
// Note    : None
// 
//******************************************************************************
#ifndef WRITE_FUNCTIONS_H
#define WRITE_FUNCTIONS_H

//******************************* Include Files ********************************
#include "mutexOperations.h"

//******************************* Global Types *********************************

//***************************** Global Constants *******************************
#define WRITE_SLEEP             (600000)
#define MAX_LIMIT               (300)
#define WRITE_COMMAND           "w"
#define APPEND_COMMAND          "a"
#define NEWLINE                 "\n"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool createSharedMemory(void);
bool writeToFile(void);
bool destroySharedMemory(void);

//*********************** Inline Method Implementations ************************ 

#endif // WRITE_FUNCTIONS_H
// EOF