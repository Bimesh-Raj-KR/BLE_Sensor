//*********************************** Sender ***********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.c
// Summary : Manages sender setup and initialization
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 27/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "senderFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//***********************************.main.*************************************
// Purpose : Function to receive file path and initiate sender setup
// Inputs  : ulArgCount  - Argument count
//           pcArgVector - Argument vector
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************
int main(int32 lArgCount, int8* pcArgVector[])
{
    if (1 < lArgCount)
    {
        senderInit(pcArgVector[1]);
    }

    return 0;
}