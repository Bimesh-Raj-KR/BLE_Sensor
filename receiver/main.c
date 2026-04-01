//*********************************** Receiver *********************************
// Copyright (c) 2026 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.c
// Summary : Handles the receiver Logic, including SIP call setup, WAV file 
//           sending, and echo reception
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 01/Apr/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "receiverFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//***********************************.main.*************************************
// Purpose : Starts the receiver logic
// Inputs  : None
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************
int main(void) 
{
    if (SUCCESS_CODE == receiverEntry()) 
    {
        printf("[RECEIVER] SUCCESS\n");
    }

    return 0;
}