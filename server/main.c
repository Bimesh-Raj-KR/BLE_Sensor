//*********************************** Server ***********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.c
// Summary : Manages server operations. Setup, receive files, and handle 
//           requests
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 12/Dec/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "serverFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//***********************************.main.*************************************
// Purpose : Function to create the received files directory and 
//           initiate server setup
// Inputs  : None
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************

int main() 
{
    mkdir(RECEIVED_FOLDER, PERMISSION);
    serverSetup();

    return 0;
}
