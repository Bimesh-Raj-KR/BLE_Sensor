//*********************************** Client ***********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.c
// Summary : Client program to send files to server
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 12/Dec/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "clientFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//***********************************.main.*************************************
// Purpose : Function to Accept File Name as argument and initiate client setup
// Inputs  : lArgCount - Number of command line arguments passed by the user
//           pArgVector - An array of character pointers listing all arguments
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************
int main(int32 lArgCount, int8 *pArgVector[])
{
    if (2 == lArgCount) 
    {
        clientSetup(pArgVector);        
    }
    else 
    {
        printf("Invalid arguments\r\n");
    }

    return 0;
}

// EOF