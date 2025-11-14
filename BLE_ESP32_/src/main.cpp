//************************* Inter Board Communication **************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.cpp
// Summary : Establishes Inter board communication via UART and BLE
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 11/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "uartCommunication.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static const uint32 ulInterruptPin = INTERRUPT_PIN;
volatile bool blFlag = false;

//***************************** Local Functions ********************************

//*****************************.triggerInterrupt.*******************************
// Purpose : Function that executes during an interrupt
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void IRAM_ATTR triggerInterrupt() 
{
  blFlag = true;
}

//*********************************.setup.**************************************
// Purpose : Function to set up ESP32 board
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void setup() 
{
  uartSetup();
  bleSetup();
  attachInterrupt(digitalPinToInterrupt(ulInterruptPin), triggerInterrupt, 
                  RISING);
}

//**********************************.loop.**************************************
// Purpose : Function to run the code indefinitely
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void loop() 
{
  if (true == blFlag)
  {
    uartFailure();
  }
}

//*****************************.serialEventRun.*********************************
// Purpose : Function to run after each loop iteration if data is available 
//           over UART
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void serialEventRun()
{
  if (true == uartReceiveAndStore())
  {
    uartCommunication();
  }
}