//***************************** Mutex Operations *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : mutexOperations.c
// Summary : Functions for creating, locking, and unlocking mutexes
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 19/Dec/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "mutexOperations.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//******************************.createMutex.***********************************
// Purpose : Function to create a mutex
// Inputs  : ppstMutex - Pointer to the mutex to be created
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool createMutex(pthread_mutex_t **ppstMutex)
{
    bool blCheck = false;
    pthread_mutexattr_t stAttribute = {0};

    if ((NULL != ppstMutex) && (NULL != *ppstMutex))
    {
        pthread_mutexattr_init(&stAttribute);
        pthread_mutexattr_setpshared(&stAttribute, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(*ppstMutex, &stAttribute);
        blCheck = true;
    }
    else
    {
        printf("Error in creating mutex\n");
    }

    return blCheck;
}

//*********************************.lockMutex.**********************************
// Purpose : Function to lock a mutex
// Inputs  : pstMutex - Pointer to the mutex to be locked
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool lockMutex(pthread_mutex_t *pstMutex)
{
    bool blCheck = false;

    if (NULL != pstMutex)
    {
        if (0 == pthread_mutex_lock(pstMutex))
        {
            blCheck = true;
        }
    }

    if (true != blCheck)
    {
        printf("Error in locking mutex\n");
    }

    return blCheck;
}

//*******************************.unlockMutex.**********************************
// Purpose : Function to unlock a mutex
// Inputs  : pstMutex - Pointer to the mutex to be unlocked
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool unlockMutex(pthread_mutex_t *pstMutex)
{
    bool blCheck = false;

    if (NULL != pstMutex)
    {
        if (0 == pthread_mutex_unlock(pstMutex))
        {
            blCheck = true;
        }
    }

    if (true != blCheck)
    {
        printf("Error in unlocking mutex\n");
    }

    return blCheck;
}

// EOF