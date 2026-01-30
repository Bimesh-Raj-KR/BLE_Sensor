//****************************** Compare Functions *****************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : compareFunctions.c
// Summary : Options for comparing and deleting SHMs 
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
static bool assignSharedMemory(int8** ppShared, uint32 ulSize, int8* pcMemPath);
static bool detachSharedMemory(uint32 ulSize, int8* pcMemPath);
static bool destroySharedMemory(uint32 ulSize, int8* pcMemPath);

//***********************************.compareShm.*******************************
// Purpose : Function to compare receiver and sender Shared memories
// Inputs  : ulSize - Size of the SHM
// Outputs : None
// Return  : true if both memories' content are same
// Notes   : None
//******************************************************************************
bool compareShm(uint32 ulSize)
{
    bool blCheck = false;
    int8 *pcSendShm = NULL;
    int8 *pcRecvShm = NULL;

    assignSharedMemory(&pcSendShm, ulSize, SHM_SEND_FILE);
    assignSharedMemory(&pcRecvShm, ulSize, SHM_RECV_FILE);

    if ((NULL != pcSendShm) && (NULL != pcRecvShm))
    {
        if (0 == memcmp(pcSendShm, pcRecvShm, ulSize))
        {
            printf("Both memories content are the same\n");
            blCheck = true;
        }
        else
        {
            printf("Both memories content are different\n");
        }
    }

    detachSharedMemory(ulSize, SHM_SEND_FILE);
    detachSharedMemory(ulSize, SHM_RECV_FILE);

    return blCheck;
}

//*********************************.deleteShm.**********************************
// Purpose : Function to delete receiver and sender Shared memories
// Inputs  : ulSize - Size of the SHM
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void deleteShm(uint32 ulSize)
{
    destroySharedMemory(ulSize, SHM_SEND_FILE);
    destroySharedMemory(ulSize, SHM_RECV_FILE);
}

//***********************************.fileShm.**********************************
// Purpose : Function to copy receiver SHM content into a file
// Inputs  : ulSize - Size of the SHM
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool fileShm(uint32 ulSize)
{
    bool blCheck = false;
    uint32 ulBytesWrote = 0;
    int8 *pcRecvShm = NULL;
    FILE *pstFile = NULL;

    if (true == assignSharedMemory(&pcRecvShm, ulSize, SHM_RECV_FILE))
    {
        pstFile = fopen(FILE_NAME, WRITE_BIN);

        if (NULL != pstFile)
        {
            ulBytesWrote = fwrite(pcRecvShm, 1, ulSize, pstFile);

            if (ulBytesWrote == ulSize)
            {
                printf("Succesfully wrote the full file\n");
                blCheck = true;
            }

            fclose(pstFile);
        }
    }

    detachSharedMemory(ulSize, SHM_RECV_FILE);

    return blCheck;
}

//***************************.assignSharedMemory.*******************************
// Purpose : Function to assign SHM
// Inputs  : ppShared - Double pointer to SHM
//           ulSize - Size of the SHM
//           pcMemPath - File path used to generate SHM key
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool assignSharedMemory(int8** ppShared, uint32 ulSize, int8* pcMemPath)
{
    bool blCheck = false;
    key_t lKey = 0;
    int32 lMemoryId = 0;
    int8 *pMemory = NULL;

    if ((NULL != ppShared) && (NULL != pcMemPath))
    {
        if (true == assignMemory(&lKey, pcMemPath))
        {
            if (true == getMemoryId(&lMemoryId, lKey, ulSize))
            {
                if (true == attachMemory(&pMemory, lMemoryId))
                {
                    *ppShared = (int8*)pMemory;
                    blCheck = true;
                }
            }
        }
        else
        {
            printf("File might not be created\n");
        }
    }

    if (true != blCheck)
    {
        printf("Error in creating shared memory\n");
    }

    return blCheck;
}

//***************************.destroySharedMemory.******************************
// Purpose : Function to detach shared memory
// Inputs  : ulSize - Size of the SHM
//           pcMemPath - File path used to generate SHM key
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
static bool detachSharedMemory(uint32 ulSize, int8* pcMemPath)
{
    bool blCheck = false;
    key_t lKey = 0;
    int32 lMemoryId = 0;
    int8 *pMemory = NULL;

    if (NULL != pcMemPath)
    {
        if (true == assignMemory(&lKey, pcMemPath))
        {
            if (true == getMemoryId(&lMemoryId, lKey, ulSize))
            {
                if (true == attachMemory(&pMemory, lMemoryId))
                {
                    detachMemory(pMemory);
                    blCheck = true;
                }
            }
        }
    }

    if (true != blCheck)
    {
        printf("Failed to detach shared memory\n");
    }

    return blCheck;
}

//***************************.destroySharedMemory.******************************
// Purpose : Function to detach and delete shared memory
// Inputs  : ulSize - Size of the SHM
//           pcMemPath - File path used to generate SHM key
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
static bool destroySharedMemory(uint32 ulSize, int8* pcMemPath)
{
    bool blCheck = false;
    key_t lKey = 0;
    int32 lMemoryId = 0;
    int8 *pMemory = NULL;

    if (NULL != pcMemPath)
    {
        if (true == assignMemory(&lKey, pcMemPath))
        {
            if (true == getMemoryId(&lMemoryId, lKey, ulSize))
            {
                if (true == attachMemory(&pMemory, lMemoryId))
                {
                    detachMemory(pMemory);
                    deleteMemory(lMemoryId);
                    blCheck = true;
                }
            }
        }
    }

    if (true != blCheck)
    {
        printf("Failed to destroy shared memory\n");
    }

    return blCheck;
}

// EOF