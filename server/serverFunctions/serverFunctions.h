//***************************** Server Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for serverFunctions.c functions
// Note    : None
// 
//******************************************************************************
#ifndef SERVERFUNCTIONS_H
#define SERVERFUNCTIONS_H

//******************************* Include Files ********************************
#include "parseSRecord.h"

//******************************* Global Types *********************************
typedef short int16;
typedef int int32;

//***************************** Global Constants *******************************
#define PORT                    (9075)
#define BUFFER_SIZE             (1024)
#define MAX_IP_SIZE             (128)
#define FLAG_SIZE               (20)
#define MIN_DELAY               (10)
#define START_OFFSET            (17)
#define END_OFFSET              (15)
#define ERROR_CODE              (-1)
#define GET_IP                  "hostname -I | awk '{print $1}'"
#define FILE_FLAG               "File"
#define ADDRESS_FLAG            "Address"
#define FILE_NAME               "parser.srec"
#define START_ADDR              "Starting Address:"
#define END_ADDR                "Ending Address:"
#define START_ONE               "S1"
#define START_TWO               "S2"
#define START_THREE             "S3"
#define WRITE_BINARY            "wb"
#define READ_COMMAND            "r"
#define NEWLINE_CHAR            '\n'

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool serverSetup(void);

//*********************** Inline Method Implementations ************************ 

#endif // SERVERFUNCTIONS_H
// EOF 