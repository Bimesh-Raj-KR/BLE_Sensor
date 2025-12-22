//****************************** Write New Line ********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.c
// Summary : Functions for writing a new line to a file periodically using
//           shared memory and mutex for synchronization
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 19/Dec/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "writeFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//**********************************.main.**************************************
// Purpose : Function to write a new line to a file periodically
// Inputs  : None
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************
int main()
{
    uint32 ulCount = 0;

    if (true == createSharedMemory())
    {
        while (MAX_LIMIT > ulCount ++)
        {
            if (true != writeToFile())
            {
                break;
            }

            usleep(WRITE_SLEEP);
        }
    }

    destroySharedMemory();

    return 0;

}

// EOF