//******************************* List Process *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.c
// Summary : Lists active processes or open files according to the 
//           command line argument
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 24/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "listFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//************************************.main.************************************
// Purpose : Function to store lsof list inside a .txt file and show particular 
//           file or PID details according to the command line argument
// Inputs  : lArgCount - Number of command line arguments passed by the user
//           pArgVector - An array of character pointers listing all arguments
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************
int main(int32 lArgCount, int8 *pArgVector[])
{
    int8 pcFlag[MAX_FLAG_SIZE] = {0};

    listData();

    if ((CORRECT_COUNT == lArgCount) && 
        (MAX_FLAG_SIZE == strlen(pArgVector[COMMAND_INDEX])))
    {
        strcpy(pcFlag, pArgVector[COMMAND_INDEX]);

        if (FLAG_PREFIX == pcFlag[0])
        {
            switch (pcFlag[1])
            {
                case FILE_COMMAND:
                    listProcess(pArgVector[INPUT_INDEX]);
                    break;

                case PID_COMMAND:
                    listFiles(pArgVector[INPUT_INDEX]);
                    break;

                default:
                    printf("Unknown flag\n");
                    break;
            }
        }
    }
    else
    {
        printf("Invalid arguments\n");
    }

    return 0;
}
