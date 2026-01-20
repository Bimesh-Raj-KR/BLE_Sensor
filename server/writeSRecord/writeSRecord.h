//******************************** Write S Record ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all global constants and forward declarations required 
//           for writeSRecord.c functions
// Note    : None
// 
//******************************************************************************
#ifndef WRITE_S_RECORD_H
#define WRITE_S_RECORD_H

//******************************* Include Files ********************************
#include "parseSRecord.h"

//******************************* Global Types *********************************

//***************************** Global Constants *******************************
#define BUFFER_SIZE             (1024)
#define LINE_SIZE               (512)
#define MAX_DATA                (16)
#define TYPE_TWO_COUNT          (4)
#define TYPE_ONE_COUNT          (3)
#define MASKER                  (0xFF)
#define READ_COMMAND            "r"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool writeHeader(FILE *pstSrecFile);
bool writeTypeOne(FILE *pstBinFile, FILE *pstSrecFile, 
                            uint32 ulLineStartAddr, uint32 ulAddrLength);
bool writeTypeTwo(FILE *pstBinFile, FILE *pstSrecFile, 
                            uint32 ulLineStartAddr, uint32 ulAddrLength);
bool writeTerminator(FILE *pstSrecFile, uint8 ucByteCount);

//*********************** Inline Method Implementations ************************ 

#endif // WRITE_S_RECORD_H
// EOF 