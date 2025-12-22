//******************************* Read Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : readFunctions.c
// Summary : Functions for reading data from a file using shared memory and 
//           mutex
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 19/Dec/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "readFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static pthread_mutex_t *spstMutex = NULL;
static uint32 sulPreviousCount = 0;
static uint32 sulCount = 0;

//***************************** Local Functions ********************************
static bool countCheck(uint32 ulCurrentCount);

//**************************.assignSharedMemory.********************************
// Purpose : Function to get shared memory and initialize mutex
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool assignSharedMemory(void)
{
    bool blCheck = false;
    key_t lKey = 0;
    int32 lMemoryId = 0;
    int8 *pMemory = NULL;

    if (true == assignMemory(&lKey, FILENAME))
    {
        if (true == getMemoryId(&lMemoryId, lKey, sizeof(pthread_mutex_t)))
        {
            if (true == attachMemory(&pMemory, lMemoryId))
            {
                spstMutex = (pthread_mutex_t*)pMemory;
                blCheck = true;
            }
        }
    }
    else
    {
        printf("File might not be created\n");
    }

    if (true != blCheck)
    {
        printf("Error in creating shared memory\n");
    }

    return blCheck;
}

//********************************.readFromFile.********************************
// Purpose : Function to read number of lines from the file
// Inputs  : None
// Outputs : Total number of lines read from the file
// Return  : false if there are errors or if count is unchanged for five reads, 
//           else true
// Notes   : None
//******************************************************************************
bool readFromFile(void)
{
    bool blCheck = false;
    FILE *pstFile = NULL;
    uint32 ulLineCount = 0;
    int32 lCharacter = 0;
    

    if (NULL != spstMutex)
    {
        if (true == lockMutex(spstMutex))
        {
            pstFile = fopen(FILENAME, READ_COMMAND);

            if (NULL != pstFile)
            {
                while (EOF != (lCharacter = fgetc(pstFile)))
                {
                    if (NEWLINE_CHAR == (int8)lCharacter)
                    {
                        ulLineCount ++;
                    }
                }

                printf("Total lines: %u\n", ulLineCount);

                if (true == countCheck(ulLineCount))
                {
                    sulPreviousCount = ulLineCount;
                    blCheck = true;
                }

                fclose(pstFile);
            }
            else
            {
                printf("Error in opening file for reading\n");
            }

            unlockMutex(spstMutex);
        }
        else
        {
            printf("Mutex not initialized in sharedMemory\n");
        }
    }
    else
    {
        printf("Mutex not created\n");
    }

    return blCheck;
}

//***************************.destroySharedMemory.******************************
// Purpose : Function to detach and delete shared memory
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool destroySharedMemory(void)
{
    bool blCheck = false;
    key_t lKey = 0;
    int32 lMemoryId = 0;
    int8 *pMemory = NULL;

    if (true == assignMemory(&lKey, FILENAME))
    {
        if (true == getMemoryId(&lMemoryId, lKey, sizeof(pthread_mutex_t)))
        {
            if (true == attachMemory(&pMemory, lMemoryId))
            {
                detachMemory(pMemory);
                deleteMemory(lMemoryId);
                blCheck = true;
            }
        }
    }

    if (true != blCheck)
    {
        printf("Failed to destroy shared memory\n");
    }

    return blCheck;
}

//*********************************.countCheck.*********************************
// Purpose : Function to check if the count is same across five reads
// Inputs  : None
// Outputs : None
// Return  : true if count is changing, else false
// Notes   : None
//******************************************************************************
static bool countCheck(uint32 ulCurrentCount)
{
    bool blCheck = false;

    if (ulCurrentCount == sulPreviousCount)
    {
        sulCount ++;
    }
    else
    {
        sulCount = 0;
    }

    if (MAX_COUNT > sulCount)
    {
        blCheck = true;
    }
    else
    {
        printf("No new lines added in last 5 reads. Exiting app...\n");
    }

    return blCheck;
}

// EOF