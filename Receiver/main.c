//*********************************** Receiver *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.c
// Summary : Manages receiver setup and initialization
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 30/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "receiverFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//***********************************.main.*************************************
// Purpose : Function to setup receiver
// Inputs  : lArgCount - Argument Count
//           pcArgVector - File path
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
                receiverThreadCreate(ulSize);
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