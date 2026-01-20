//******************************** Parse S Record*******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for parseSRecord.c functions
// Note    : None
// 
//******************************************************************************
#ifndef PARSE_S_RECORD_H
#define PARSE_S_RECORD_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <elf.h>

//******************************* Global Types *********************************
typedef unsigned char uint8;
typedef char int8;
typedef unsigned int uint32;

typedef enum {
    TYPE_ONE_ADDR = 4,
    TYPE_TWO_ADDR = 6,
    TYPE_THREE_ADDR = 8
}ADDR_LENGTH;

typedef enum {
    TYPE_ONE_START = 8,
    TYPE_TWO_START = 10,
    TYPE_THREE_START = 12
}DATA_START;

typedef enum {
    DECR_THREE = 3,
    DECR_FOUR,
    DECR_FIVE
}DECR_VALUE;

typedef enum {
    LOWER_BITS = 8,
    UPPER_BITS = 16
}ADDR_BITS;

//***************************** Global Constants *******************************
#define MAX_COMMAND_SIZE        (256)
#define TAIL_SIZE               (100)    
#define TEMP_SIZE               (20)
#define HEX_BASE                (16)
#define ADDR_START              (4)
#define BYTE_LENGTH             (2)
#define BYTE_SKIP               (2)
#define COUNT_POS               (2)
#define BIN_FILE_NAME           "convert"
#define HEX                     "0x" 
#define NULL_CHAR               '\0'

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool parseTypeOne(int8* pcReadBuffer, uint32 ulStartAddr, uint32 ulEndAddr);
bool parseTypeTwo(int8* pcReadBuffer, uint32 ulStartAddr, uint32 ulEndAddr);
bool parseTypeThree(int8* pcReadBuffer, uint32 ulStartAddr, uint32 ulEndAddr);
void parseCheckRange(void);

//*********************** Inline Method Implementations ************************ 

#endif // PARSE_S_RECORD_H
// EOF 