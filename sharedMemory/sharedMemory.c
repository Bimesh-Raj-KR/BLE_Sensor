//******************************* Shared Memory ********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : sharedMemory.c
// Summary : Functions for managing shared memory
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 19/Dec/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "sharedMemory.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//******************************.assignMemory.**********************************
// Purpose : Function to create a unique key for shared memory
// Inputs  : plKey - Pointer to store the generated key
//           pcFileName - Name of the file to generate key from
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool assignMemory(key_t *plKey, int8 *pcFileName)
{
    bool blCheck = false;

    if ((NULL != plKey) && (NULL != pcFileName))
    {
        *plKey = ftok((const int8*)pcFileName, 1);

        if (ERROR != *plKey)
        {
            blCheck = true;
        }
    }

    if (true != blCheck)
    {
        printf("Error in creating unique key\n");
    }

    return blCheck;
}

//*****************************.getMemoryId.************************************
// Purpose : Function to get shared memory ID
// Inputs  : pMemoryId - Pointer to store the shared memory ID
//           lKey - Unique key for the shared memory
//           unSize - Size of the shared memory
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool getMemoryId(int32 *pMemoryId, key_t lKey, uint32 unSize)
{
    bool blCheck = false;

    if (NULL != pMemoryId)
    {
        *pMemoryId = shmget(lKey, unSize, PERMISSIONS | IPC_CREAT);

        if (ERROR != *pMemoryId)
        {
            blCheck = true;
        }
    }

    if (true != blCheck)
    {
        printf("Error in creating id for shared memory\n");
    }

    return blCheck;
}

//******************************.attachMemory.**********************************
// Purpose : Function to attach shared memory
// Inputs  : ppMemory - Pointer to the memory to be attached
//           lMemoryId - ID of the shared memory
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool attachMemory(int8 **ppMemory, int32 lMemoryId)
{
    bool blCheck = false;

    if (NULL != ppMemory)
    {
        *ppMemory = (int8*) shmat(lMemoryId, NULL, 0);

        if ((int8*)ERROR != *ppMemory)
        {
            blCheck = true;
        }
    }

    if (true != blCheck)
    {
        printf("Error in attaching shared memory\n");
    }

    return blCheck;
}

//*****************************.detachMemory.***********************************
// Purpose : Function to detach shared memory
// Inputs  : ppMemory - Pointer to the memory to be detached
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool detachMemory(int8 *pMemory)
{
    bool blCheck = false;

    if (NULL != pMemory)
    {
        if (ERROR != shmdt(pMemory))
        {
            blCheck = true;
        }
    }

    if (true != blCheck)
    {
        printf("Error in detaching shared memory\n");
    }

    return blCheck;
} 

//******************************.deleteMemory.**********************************
// Purpose : Function to delete shared memory
// Inputs  : lMemoryId - ID of the shared memory
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool deleteMemory(int32 lMemoryId)
{
    bool blCheck = false;

    if (ERROR != shmctl(lMemoryId, IPC_RMID, NULL))
    {
        blCheck = true;
    }

    if (true != blCheck)
    {
        printf("Error in deleting shared memory\n");
    }

    return blCheck;
}   

// EOF