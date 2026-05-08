//******************************* Video Streaming ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : main.cpp
// Summary : Streams live video from a camera using onvifclient, Rtspclient and 
//           gstreamer libraries
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 16/Mar/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "videoStreamer.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//***********************************.main.*************************************
// Purpose : Function to start device discovery, get stream uri and start 
//           streaming
// Inputs  : None
// Outputs : None
// Return  : 0
// Notes   : None
//******************************************************************************
int main(void)
{
    discoverDevice();
    deviceStreamInit();
    videoStreamerInit();

    return 0;
}
