//******************************* Check Function *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : checkFunctions.c
// Summary : Functions for extracting data and checking validity
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 24/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "checkFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//*********************************.checkId.************************************
// Purpose : Function to extract the current PID for comparing with 
//           user given PID
// Inputs  : pcBuffer - A single line from the open files list produced by lsof
//           unLength - Length of the buffer
//           pcCurrentId - Stores the PID given inside the buffer
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool checkId(int8 *pcBuffer, uint16 unLength, int8 *pcCurrentId)
{
    bool blCheck = false;
    uint8 ucIterator = 0;
    uint8 ucIndex = 0;
    uint8 ucFlag = 0;

    if ((NULL != pcBuffer) && (NULL != pcCurrentId))
    {
        for(ucIterator = 0; ucIterator < unLength; ucIterator ++)
        {
            if ((MIN_LIMIT <= pcBuffer[ucIterator]) && 
                (MAX_LIMIT >= pcBuffer[ucIterator]))
            {
                pcCurrentId[ucIndex] = pcBuffer[ucIterator];
                ucIndex ++;
                ucFlag = 1;
            }
            else
            {
                if (1 == ucFlag)
                {
                    pcCurrentId[ucIndex] = NULL_CHAR;
                    blCheck = true;
                    break;
                }
            }
        }
    }

    return blCheck;
}

//*********************************.checkId.************************************
// Purpose : Function to extract the current open file for comparing with 
//           user given filename
// Inputs  : pcBuffer - A single line from the open files list produced by lsof
//           unLength - Length of the buffer
//           pcCurrentFile - Stores the open file name given inside the buffer
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool checkFile(int8 *pcBuffer, uint16 unLength, int8 *pcCurrentFile)
{
    bool blCheck = false;
    uint8 ucIterator = 0;
    uint8 ucIndex = 0;
    uint8 ucFlag = 0;

    if ((NULL != pcBuffer) && (NULL != pcCurrentFile))
    {
        for(ucIterator = MIN_SIZE; ucIterator < unLength - 1; 
            ucIterator ++)
        {
            if (PATH == pcBuffer[ucIterator] || 1 == ucFlag)
            {
                pcCurrentFile[ucIndex] = pcBuffer[ucIterator];
                ucIndex ++;
                ucFlag = 1;
                blCheck = true;
            }
        }
    }

    pcCurrentFile[ucIndex] = NULL_CHAR;

    return blCheck;
}

//*****************************.checkValidId.***********************************
// Purpose : Function to check if user given PID is valid or not
// Inputs  : pcProcessId - user given PID
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool checkValidId(int8 *pcProcessId)
{
    bool blCheck = false;
    uint8 ucIterator = 0;
    uint8 ucFlag = 0;

    if (NULL != pcProcessId)
    {
        while (NULL_CHAR != pcProcessId[ucIterator])
        {
            if ((MIN_LIMIT > pcProcessId[ucIterator]) || 
                (MAX_LIMIT < pcProcessId[ucIterator]))
            {
                ucFlag = 1;
                break;
            }

            ucIterator ++;
        }
    }
    else
    {
        ucFlag = 1;
    }

    if (0 == ucFlag)
    {
        blCheck = true;
    }
    else
    {
        printf("Invalid process ID\n");
    }

    return blCheck;
}

// EOF