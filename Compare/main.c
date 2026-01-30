//************************************ Compare *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.c
// Summary : Interface for compare and delete options
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 30/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "compareFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//***********************************.main.*************************************
// Purpose : Function to select option
// Inputs  : lArgCount - Argument Count
//           pcArgVector - File path
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************
int main(int32 lArgCount, int8* pcArgVector[])
{
    uint32 ulSize = 0;
    int8 cChoice = 0;
    struct stat stFile = {0};

    if (MAX_ARG == lArgCount)
    {
        if (0 == stat(pcArgVector[1], &stFile))
        {
            ulSize = (uint32)stFile.st_size;

            printf("\tPRESS\n C - Compare\n D - Delete\n F - File\n");
            scanf("%c", &cChoice);

            if (0 < ulSize)
            {
                switch (cChoice)
                {
                    case 'C':
                        compareShm(ulSize);
                        break;

                    case 'D':
                        deleteShm(ulSize);
                        break;

                    case 'F':
                        fileShm(ulSize);
                        break;

                    default:
                        printf("Wrong choice\n");
                        break;
                }
            }
        }
    }

    return 0;
}

// EOF