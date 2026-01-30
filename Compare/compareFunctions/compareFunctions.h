//****************************** Compare Functions *****************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all global constants and forward declarations required 
//           for compareFunctions.c functions
// Note    : None
// 
//******************************************************************************
#ifndef COMPARE_FUNCTIONS_H
#define COMPARE_FUNCTIONS_H

//******************************* Include Files ********************************
#include "sharedMemory.h"
#include <sys/stat.h>

//******************************* Global Types *********************************


//***************************** Global Constants *******************************
#define PORT                    (5000)
#define WRITE_BIN               "wb"
#define FILE_NAME               "shmOut.mp3"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool compareShm(uint32 ulSize);
void deleteShm(uint32 ulSize);
bool fileShm(uint32 ulSize);

//*********************** Inline Method Implementations ************************ 

#endif // COMPARE_FUNCTIONS_H
// EOF 