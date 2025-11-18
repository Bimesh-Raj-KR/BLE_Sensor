//***************************** Command Processing *****************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
//
// Summary : Contains all global constants and forward declarations needed for 
//           commandProcessing.cpp
// Note    : None
//
//******************************************************************************
#ifndef COMMANDPROCESSING_H
#define COMMANDPROCESSING_H

//******************************* Include Files ********************************
#include "dataProcessing.h"

//******************************* Global Types *********************************

//***************************** Global Constants *******************************
#define MAX_SLEEP           (100)

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool processPing(void);
bool processTelemetry(void);
bool processTimeout(void);
bool processVersion(void);

//*********************** Inline Method Implementations ************************

#endif // COMMANDPROCESSING_H
// EOF
