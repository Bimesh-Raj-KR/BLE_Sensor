//*********************************** Sender ***********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.c
// Summary : Manages sender setup and initialization
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

//***********************************.main.*************************************
// Purpose : Function to port two structs and print the resultant struct
// Inputs  : None
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************
int main(void)
{
    FROM_PORT stSource = {0};
    TO_PORT stDestination = {0};

    printf("Enter float value\n");
    scanf("%f", &stSource.fFloat);
    printf("Enter long value\n");
    scanf("%ld", &stSource.lLong);

    portStruct(&stSource, &stDestination);
    printf("Int value: %d\nShort value: %d\n", stDestination.lInteger, 
                                                stDestination.nShort);

    return 0;
}

// EOF