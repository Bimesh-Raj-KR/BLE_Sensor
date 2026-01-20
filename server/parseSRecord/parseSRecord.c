//******************************** Parse S Record*******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : parseSRecord.c
// Summary : Parses S record data of different format
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 15/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "parseSRecord.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static uint32 sulCount = 0; // Number of Bytes parsed
static uint32 sulPossibleCount = 0; // Possible number of Bytes to be parsed
static uint8 sucPartialFlag = 0; // Flag to check if End addr is outside range
static uint8 sucFullFlag = 0; // Flag to check if start and end addr are outside
                              // range
static uint8 sucStartFlag = 0; // Flag to check if start addr is outside range
static bool sblFlag = false; // Turns true after the first srec line arrives

//***************************** Local Functions ********************************
static bool parseVerifySum(int8* pcReadBuffer, uint8 ucByteCount);
static bool parseData(int8* pcReadBuffer, uint8 ucDataBytes, 
                uint32 ulLineStartAddr, uint32 ulStartAddr, uint32 ulEndAddr);

//******************************.parseTypeOne.**********************************
// Purpose : Function to parse S1 data
// Inputs  : pcReadBuffer - Data to be parsed
//           ulStartAddr - Memory range from which data needs to be parsed
//           ulEndAddr - Memory range upto which data needs to be parsed
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool parseTypeOne(int8* pcReadBuffer, uint32 ulStartAddr, uint32 ulEndAddr)
{
    bool blCheck = false;
    uint32 ulLineStartAddr = 0;
    uint8 ucByteCount = 0;
    uint8 ucDataBytes = 0;
    int8 cTempBuffer[TEMP_SIZE] = {0};

    if (NULL != pcReadBuffer)
    {
        // Get Byte Count
        strncpy(cTempBuffer, &pcReadBuffer[COUNT_POS], BYTE_LENGTH);
        cTempBuffer[BYTE_LENGTH] = NULL_CHAR;
        ucByteCount = (uint8)strtoul(cTempBuffer, NULL, HEX_BASE);

        if (true == parseVerifySum(pcReadBuffer, ucByteCount))
        {
            // Get Line's Starting Address
            strncpy(cTempBuffer, &pcReadBuffer[ADDR_START], TYPE_ONE_ADDR);
            cTempBuffer[TYPE_ONE_ADDR] = NULL_CHAR;
            ulLineStartAddr = (uint32)strtoul(cTempBuffer , NULL, HEX_BASE);

            // Calculate Data Bytes
            ucDataBytes = ucByteCount - DECR_THREE;

            if (true == parseData(pcReadBuffer + TYPE_ONE_START, ucDataBytes, 
                                  ulLineStartAddr, ulStartAddr, ulEndAddr))
            {
                blCheck = true;
            }
        }
    }

    if (true != blCheck)
    {
        printf("Failed to parse S1 data\n");
    }

    return blCheck;
}

//******************************.parseTypeTwo.**********************************
// Purpose : Function to parse S2 data
// Inputs  : pcReadBuffer - Data to be parsed
//           ulStartAddr - Memory range from which data needs to be parsed
//           ulEndAddr - Memory range upto which data needs to be parsed
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool parseTypeTwo(int8* pcReadBuffer, uint32 ulStartAddr, uint32 ulEndAddr)
{
    bool blCheck = false;
    uint32 ulLineStartAddr = 0;
    uint8 ucByteCount = 0;
    uint8 ucDataBytes = 0;
    int8 cTempBuffer[TEMP_SIZE] = {0};

    if (NULL != pcReadBuffer)
    {
        strncpy(cTempBuffer, &pcReadBuffer[COUNT_POS], BYTE_LENGTH);
        cTempBuffer[BYTE_LENGTH] = NULL_CHAR;
        ucByteCount = (uint8)strtoul(cTempBuffer, NULL, HEX_BASE);

        if (true == parseVerifySum(pcReadBuffer, ucByteCount))
        {
            strncpy(cTempBuffer, &pcReadBuffer[ADDR_START], TYPE_TWO_ADDR);
            cTempBuffer[TYPE_TWO_ADDR] = NULL_CHAR;
            ulLineStartAddr = (uint32)strtoul(cTempBuffer , NULL, HEX_BASE);
            ucDataBytes = ucByteCount - DECR_FOUR;

            if (true == parseData(pcReadBuffer + TYPE_TWO_START, ucDataBytes, 
                                  ulLineStartAddr, ulStartAddr, ulEndAddr))
            {
                blCheck = true;
            }
        }
    }

    if (true != blCheck)
    {
        printf("Failed to parse S2 data\n");
    }

    return blCheck;
}

//******************************.parseTypeThree.********************************
// Purpose : Function to parse S3 data
// Inputs  : pcReadBuffer - Data to be parsed
//           ulStartAddr - Memory range from which data needs to be parsed
//           ulEndAddr - Memory range upto which data needs to be parsed
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool parseTypeThree(int8* pcReadBuffer, uint32 ulStartAddr, uint32 ulEndAddr)
{
    bool blCheck = false;
    uint32 ulLineStartAddr = 0;
    uint8 ucByteCount = 0;
    uint8 ucDataBytes = 0;
    int8 cTempBuffer[TEMP_SIZE] = {0};

    if (NULL != pcReadBuffer)
    {
        strncpy(cTempBuffer, &pcReadBuffer[COUNT_POS], BYTE_LENGTH);
        cTempBuffer[BYTE_LENGTH] = NULL_CHAR;
        ucByteCount = (uint8)strtoul(cTempBuffer, NULL, HEX_BASE);

        if (true == parseVerifySum(pcReadBuffer, ucByteCount))
        {
            strncpy(cTempBuffer, &pcReadBuffer[ADDR_START], TYPE_THREE_ADDR);
            cTempBuffer[TYPE_THREE_ADDR] = NULL_CHAR;
            ulLineStartAddr = (uint32)strtoul(cTempBuffer , NULL, HEX_BASE);
            ucDataBytes = ucByteCount - DECR_FIVE;

            if (true == parseData(pcReadBuffer + TYPE_THREE_START, ucDataBytes, 
                                  ulLineStartAddr, ulStartAddr, ulEndAddr))
            {
                blCheck = true;
            }
        }
    }

    if (true != blCheck)
    {
        printf("Failed to parse S3 data\n");
    }

    return blCheck;
}

//**************************.parseCheckRange.***********************************
// Purpose : Function to check if memory range is within the bounds of the file
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
void parseCheckRange(void)
{
    if (0 == sucFullFlag)
    {
        printf("Data cannot be parsed\n");
        printf("Both Starting and Ending addresses are outside range\n");
    }
    else
    {

        if (1 == sucStartFlag)
        {
            printf("Starting Address is outside range\n");
        }

        if (1 == sucPartialFlag)
        {
            printf("Ending Address is outside range\n");
        }

        if (sulPossibleCount > sulCount)
        {
            printf("Only partial parsing was possible\n");
            printf("Only %u bytes of Data was within memory range\n", sulCount);
        }
    }

    sulCount = 0;
    sulPossibleCount = 0;
    sucFullFlag = 0;
    sucStartFlag = 0;
    sucPartialFlag = 0;
    sblFlag = false;
}

//***************************.parseVerifySum.***********************************
// Purpose : Function to verify checksum
// Inputs  : pcreadBuffer - Buffer containing data
//           ucByteCount - Total byte count in that line 
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool parseVerifySum(int8* pcReadBuffer, uint8 ucByteCount)
{
    bool blCheck = false;
    uint8 ucIterator = 0;
    uint8 ucCheckSum = 0;
    uint8 ucSum = 0;
    int8 cTempBuffer[TEMP_SIZE] = {0};

    if (NULL != pcReadBuffer)
    {
        ucSum += ucByteCount;

        for (ucIterator = 0; ucIterator < ucByteCount - 1; ucIterator ++)
        {
            // Sum all bytes except checksum byte
            strncpy(cTempBuffer, &pcReadBuffer[ADDR_START + 
                    (ucIterator * BYTE_SKIP)], BYTE_LENGTH);
            cTempBuffer[BYTE_LENGTH] = NULL_CHAR;
            ucSum += (uint8)strtoul(cTempBuffer, NULL, HEX_BASE);
        }

        // Calculate one's complement
        ucSum = (uint8)~ucSum;

        // Get Checksum byte from record
        strncpy(cTempBuffer, &pcReadBuffer[ADDR_START + 
                (ucByteCount - 1) * BYTE_SKIP], BYTE_LENGTH);
        cTempBuffer[BYTE_LENGTH] = NULL_CHAR;
        ucCheckSum = (uint8)strtoul(cTempBuffer, NULL, HEX_BASE);

        // Compare calculated checksum with record checksum
        if (ucSum == ucCheckSum)
        {
            blCheck = true;
        }
    }

    if (true != blCheck)
    {
        printf("Failed to validate Checksum\n");
    }

    return blCheck;
}

//*******************************.parseData.************************************
// Purpose : Function to parse data and print it
// Inputs  : pcReadBuffer - Data to be parsed
//           ucDataBytes - total data bytes
//           ulLineStartAddr - Address of the first data in the line
//           ulStartAddr - Memory range from which data needs to be parsed
//           ulEndAddr - Memory range upto which data needs to be parsed
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool parseData(int8* pcReadBuffer, uint8 ucDataBytes, 
                uint32 ulLineStartAddr, uint32 ulStartAddr, uint32 ulEndAddr)
{
    bool blCheck = false;
    uint32 ulCurrentAddr = 0;
    uint8 ucData = 0;
    uint8 ucIterator = 0;
    int8 cTempBuffer[TEMP_SIZE] = {0};

    if (NULL != pcReadBuffer)
    {
        sulPossibleCount = ulEndAddr - ulStartAddr + 1;

        // Checks if Start address is outside range
        if (true != sblFlag)
        {
            if ((ulLineStartAddr > ulStartAddr) && 
                (ulLineStartAddr <= ulEndAddr))
            {
                sucStartFlag = 1;
            }
        }

        for (ucIterator = 0; ucIterator < ucDataBytes; ucIterator ++)
        {
            ulCurrentAddr = ulLineStartAddr + ucIterator;

            if ((ulCurrentAddr >= ulStartAddr) && 
                (ulCurrentAddr <= ulEndAddr))
            {
                // Extract Data Byte within range
                strncpy(cTempBuffer, &pcReadBuffer[ucIterator * BYTE_SKIP], 
                        BYTE_LENGTH);
                cTempBuffer[BYTE_LENGTH] = NULL_CHAR;
                ucData = (uint8)strtoul(cTempBuffer , NULL, HEX_BASE);
                sucFullFlag = 1;
                printf("%02X ", ucData);
                sulCount ++;
            }
        }

        // Checks if End address is outside range
        if (ulCurrentAddr < ulEndAddr)
        {
            sucPartialFlag = 1;
        }
        else
        {
            sucPartialFlag = 0;
        }

        sblFlag = true;
        blCheck = true;
    }
    else
    {
        printf("Data parsing failed\n");
    }

    return blCheck;
}

// EOF