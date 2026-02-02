//*********************************** Sender ***********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.c
// Summary : Manages sender setup and initialization
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 30/Jan/2026
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
    uint32 ulSize = 0;
    struct stat stFile = {0};

    if (MAX_ARG == lArgCount)
    {
        if (0 == stat(pcArgVector[1], &stFile))
        {
            ulSize = (uint32)stFile.st_size;

            if (0 < ulSize)
            {
                senderThreadCreate(pcArgVector[1], ulSize);
            }
        }
    }
    else
    {
        printf("Enter valid file path\n");
    }

    return 0;
}

// EOF