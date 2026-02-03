//***************************** Port Structure *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : portStruct.c
// Summary : Port a source struct into a destination struct
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 03/Feb/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "portStruct.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//*******************************.portStruct.***********************************
// Purpose : Function to port the values of a source struct into a destination 
//           struct of different datatypes
// Inputs  : pstSource - Pointer to the source struct
//           pstDestination - Pointer to the destination struct
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool portStruct(FROM_PORT* pstSource, TO_PORT* pstDestination)
{
    bool blCheck = false;

    if ((NULL != pstSource) && (NULL != pstDestination))
    {
        pstDestination->lInteger = (int32)pstSource->fFloat;
        pstDestination->nShort = (int16)pstSource->lLong;
        blCheck = true;
    }

    return blCheck;
}

// EOF