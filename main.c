//**************************** Count Open Files *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.c
// Summary : Lists active processes or open files according to users choice
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 24/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "listProcess.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//************************************.main.************************************
// Purpose : Function to store lsof list inside a .txt file and show particular 
//           file or PID details according to user choice
// Inputs  : None
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************
int main()
{
    listData();
    appCount();

    return 0;
}

// EOF