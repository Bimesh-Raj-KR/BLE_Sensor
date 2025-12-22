//****************************** Read Line Count *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.c
// Summary : Functions for reading number of lines from a file using shared
//           memory and mutex for synchronization
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 19/Dec/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "readFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//************************************.main.************************************
// Purpose : Function to read number of lines from a file periodically
// Inputs  : None
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************
int main()
{
    if (true == assignSharedMemory())
    {
        while (1)
        {
            if (true != readFromFile())
            {
                break;
            }

            usleep(READ_SLEEP);
        }
    }

    destroySharedMemory();

    return 0;
}

// EOF