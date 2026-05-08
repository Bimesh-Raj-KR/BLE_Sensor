//***************************** Device Get Stream ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for multiStreamer.cpp functions
// Note    : None
// 
//******************************************************************************
#ifndef MULTI_STREAMER_H
#define MULTI_STREAMER_H

//******************************* Include Files ********************************
#include <gst/gst.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <new>
#include <iostream>

//******************************* Global Types *********************************
typedef char int8;

class FileStreamer 
{
public:
    FileStreamer(const std::string& cFileName);
    ~FileStreamer();

    void setTargetIps(const std::vector<std::string>& vcIps);
    bool start();
    void stop();

private:
    std::string m_cFileName;
    std::vector<std::string> m_cTargetIps;

    // Store each independent pipeline
    std::vector<GstElement*> m_pvstPipelines;
    bool m_blIsRunning;
};

//***************************** Global Constants *******************************
#define MAX_SLEEP               (100000)

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
void videoFileStreamerInit(void);


//*********************** Inline Method Implementations ************************ 

#endif // MULTI_STREAMER_H
// EOF