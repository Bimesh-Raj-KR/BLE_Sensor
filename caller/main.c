//*********************************** Caller ***********************************
// Copyright (c) 2026 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : callerFunctions.c
// Summary : Handles the caller Logic, including SIP call setup, WAV file 
//           sending, and echo reception
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 01/Apr/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "callerFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//***********************************.main.*************************************
// Purpose : Starts the caller logic
// Inputs  : argc - Argument count
//           argv - Argument vector strings
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************
int main(int argc, char *argv[]) 
{
    if (SUCCESS_CODE == callerEntry(argc, argv)) 
    {
        printf("[CALLER] SUCCESS\n");
    }

    return 0;
}