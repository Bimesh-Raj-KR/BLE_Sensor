//******************************* Write Functions ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : writeFunctions.c
// Summary : Functions for writing to a file using shared memory and mutex
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 19/Dec/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "writeFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static pthread_mutex_t *spstMutex = NULL;

//***************************** Local Functions ********************************
static bool createFile(const int8 *pcFileName);

//***************************.createSharedMemory.*******************************
// Purpose : Function to create shared memory and initialize mutex
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool createSharedMemory(void)
{
    bool blCheck = false;
    key_t lKey = 0;
    int32 lMemoryId = 0;
    int8 *pMemory = NULL;

    createFile(FILENAME);

    if (true == assignMemory(&lKey, FILENAME))
    {
        if (true == getMemoryId(&lMemoryId, lKey, sizeof(pthread_mutex_t)))
        {
            if (true == attachMemory(&pMemory, lMemoryId))
            {
                spstMutex = (pthread_mutex_t*)pMemory;
                createMutex(&spstMutex);
                blCheck = true;
            }
        }
    }

    if (true != blCheck)
    {
        printf("Error in creating shared memory\n");
    }

    return blCheck;
}

//*******************************.writeToFile.**********************************
// Purpose : Function to write a new line to the file
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool writeToFile(void)
{
    bool blCheck = false;
    FILE *pstFile = NULL;

    if (NULL != spstMutex)
    {
        lockMutex(spstMutex);
        pstFile = fopen(FILENAME, APPEND_COMMAND);

        if (NULL != pstFile)
        {
            fwrite(NEWLINE, sizeof(int8), 1, pstFile);
            fclose(pstFile);
            blCheck = true;
        }
        else
        {
            printf("Error in opening file for writing\n");
        }

        unlockMutex(spstMutex);
    }
    else
    {
        printf("Mutex not created\n");
    }

    return blCheck;
}

//*************************.destroySharedMemory.********************************
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

//******************************.createFile.************************************
// Purpose : Function to create a file
// Inputs  : pcFileName - Name of the file to create
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
static bool createFile(const int8 *pcFileName)
{
    bool blCheck = false;
    FILE *pstFile = NULL;

    pstFile = fopen(pcFileName, WRITE_COMMAND);

    if (NULL != pstFile)
    {
        blCheck = true;
        fclose(pstFile);
    }

    if (true != blCheck)
    {
        printf("Error in creating file\n");
    }

    return blCheck;
}

// EOF