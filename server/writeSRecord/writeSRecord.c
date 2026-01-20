//******************************** Write S Record ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : writeSRecord.c
// Summary : Writes S record data of different format
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 20/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "writeSRecord.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//*********************************.writeHeader.********************************
// Purpose : Function to write SREC header
// Inputs  : pstSrecFile - Pointer to the SREC file
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool writeHeader(FILE *pstSrecFile)
{
    bool blCheck = false;

    if (NULL != pstSrecFile)
    {
        fprintf(pstSrecFile, "S00E0000666F726D61742E737265638D\n");
        blCheck = true;
    }

    if (true != blCheck)
    {
        printf("Failed to write SREC Header\n");
    }

    return blCheck;
}

//*******************************.writeTypeOne.*********************************
// Purpose : Function to write S1 data
// Inputs  : pstBinFile - Pointer to the binary file
//           pstSrecFile - Pointer to the SREC file
//           ulLineStartAddr - Start address of the line
//           ulAddrLength - Length of the address range
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool writeTypeOne(FILE *pstBinFile, FILE *pstSrecFile, 
                            uint32 ulLineStartAddr, uint32 ulAddrLength)
{
    bool blCheck = false;
    uint32 ulNextStartAddr = 0;
    uint8 ucReadBytes = 0;
    uint8 ucByteCount = 0;
    uint8 ucCheckSum = 0;
    uint8 ucIterator = 0;
    uint8 ucLineBuffer[LINE_SIZE] = {0};
    int8 cBuffer[BUFFER_SIZE] = {0};

    if ((NULL != pstBinFile) && (NULL != pstSrecFile))
    {
        while (0 < ulAddrLength)
        {
            ucCheckSum = 0;
            memset(ucLineBuffer, 0, LINE_SIZE);

            if (MAX_DATA < ulAddrLength)
            {
                ucReadBytes = MAX_DATA;
            }
            else
            {
                ucReadBytes = (uint8)ulAddrLength;
            }

            // Read data from binary file
            fread(ucLineBuffer, 1, ucReadBytes, pstBinFile);

            // Calculate Byte count, checksum and prepare S1 line
            ucByteCount = ucReadBytes + DECR_THREE;
            ulAddrLength -= ucReadBytes;
            ulNextStartAddr = ulLineStartAddr + ucReadBytes;

            // Calculate checksum
            for (ucIterator = 0; ucIterator < ucReadBytes; 
                    ucIterator ++)
            {
                ucCheckSum = ucCheckSum + ucLineBuffer[ucIterator];
            }

            ucCheckSum += (ucByteCount + 
                            ((ulLineStartAddr >> LOWER_BITS) & MASKER) 
                            + (ulLineStartAddr & MASKER)); 
            ucCheckSum = ~ucCheckSum;

            // Prepare S1 line
            sprintf(cBuffer, "S1%02X%04X", ucByteCount, 
                ulLineStartAddr);

            // Add data bytes to S1 line
            for (ucIterator = 0; ucIterator < ucReadBytes; 
                    ucIterator ++)
            {
                sprintf(&cBuffer[TYPE_ONE_START + 
                        (ucIterator * BYTE_SKIP)], "%02X", 
                        ucLineBuffer[ucIterator]);
            }

            // Add checksum and write to SREC file
            sprintf(&cBuffer[TYPE_ONE_START + 
                    (ucReadBytes * BYTE_SKIP)], 
                    "%02X\n", ucCheckSum);
            fwrite(cBuffer, 1, strlen(cBuffer), pstSrecFile);
            ulLineStartAddr = ulNextStartAddr;
            blCheck = true;
        }
    }

    if (true != blCheck)
    {
        printf("Failed to write S1 Data\n");
    }

    return blCheck;
}

//******************************.writeTypeTwo.**********************************
// Purpose : Function to write S2 data
// Inputs  : pstBinFile - Pointer to the binary file
//           pstSrecFile - Pointer to the SREC file
//           ulLineStartAddr - Start address of the line
//           ulAddrLength - Length of the address range
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool writeTypeTwo(FILE *pstBinFile, FILE *pstSrecFile, 
                            uint32 ulLineStartAddr, uint32 ulAddrLength)
{
    bool blCheck = false;
    uint32 ulNextStartAddr = 0;
    uint8 ucReadBytes = 0;
    uint8 ucByteCount = 0;
    uint8 ucCheckSum = 0;
    uint8 ucIterator = 0;
    uint8 ucLineBuffer[LINE_SIZE] = {0};
    int8 cBuffer[BUFFER_SIZE] = {0};

    if ((NULL != pstBinFile) && (NULL != pstSrecFile))
    {
        while (0 < ulAddrLength)
        {
            ucCheckSum = 0;
            memset(ucLineBuffer, 0, LINE_SIZE);

            if (MAX_DATA < ulAddrLength)
            {
                ucReadBytes = MAX_DATA;
            }
            else
            {
                ucReadBytes = (uint8)ulAddrLength;
            }

            fread(ucLineBuffer, 1, ucReadBytes, pstBinFile);
            ucByteCount = ucReadBytes + DECR_FOUR;
            ulAddrLength -= ucReadBytes;
            ulNextStartAddr = ulLineStartAddr + ucReadBytes;

            for (ucIterator = 0; ucIterator < ucReadBytes; 
                    ucIterator ++)
            {
                ucCheckSum = ucCheckSum + ucLineBuffer[ucIterator];
            }

            ucCheckSum += (ucByteCount + 
                            ((ulLineStartAddr >> UPPER_BITS) & MASKER) +
                            ((ulLineStartAddr >> LOWER_BITS) & MASKER) 
                            + (ulLineStartAddr & MASKER)); 
            ucCheckSum = ~ucCheckSum;
            sprintf(cBuffer, "S2%02X%06X", ucByteCount, 
                ulLineStartAddr);

            for (ucIterator = 0; ucIterator < ucReadBytes; 
                    ucIterator ++)
            {
                sprintf(&cBuffer[TYPE_TWO_START + 
                        (ucIterator * BYTE_SKIP)], "%02X", 
                        ucLineBuffer[ucIterator]);
            }

            sprintf(&cBuffer[TYPE_TWO_START + 
                    (ucReadBytes * BYTE_SKIP)], 
                    "%02X\n", ucCheckSum);
            fwrite(cBuffer, 1, strlen(cBuffer), pstSrecFile);
            ulLineStartAddr = ulNextStartAddr;
            blCheck = true;
        }
    }

    if (true != blCheck)
    {
        printf("Failed to write S2 Data\n");
    }

    return blCheck;
}

//******************************.writeTerminator.*******************************
// Purpose : Function to write SREC terminator
// Inputs  : pstSrecFile - Pointer to the SREC file
//           ucByteCount - Byte count for the terminator
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool writeTerminator(FILE *pstSrecFile, uint8 ucByteCount)
{
    bool blCheck = false;
    FILE *pstPipe = NULL;
    uint32 ulEntryAddress = 0;
    uint8 ucCheckSum = 0;
    int8 cCommand[MAX_COMMAND_SIZE] = {0};
    int8 cTailBuffer[TAIL_SIZE] = {0};
    int8 *pcAddress = NULL;

    if (NULL != pstSrecFile)
    {
        snprintf(cCommand, sizeof(cCommand), "readelf -h %s | grep \"Entry "
                 "point address\"", BIN_FILE_NAME);

        pstPipe = popen(cCommand, READ_COMMAND);

        if (NULL != pstPipe)
        {
            if (NULL != fgets(cTailBuffer, TAIL_SIZE, pstPipe)) 
            {
                pcAddress = strstr(cTailBuffer, HEX);

                if (NULL != pcAddress)
                {
                    // Extract Entry address
                    ulEntryAddress = (uint32)strtoul(pcAddress, NULL, HEX_BASE);

                    // Prepare and write Terminator line
                    if (TYPE_ONE_COUNT == ucByteCount)
                    {
                        ucCheckSum = ~((ucByteCount + 
                                    ((ulEntryAddress >> LOWER_BITS) & MASKER) + 
                                    (ulEntryAddress & MASKER)));
                        fprintf(pstSrecFile, "S903%04X%02X\n", 
                                ulEntryAddress, ucCheckSum);
                        blCheck = true;
                    }
                    else if (TYPE_TWO_COUNT == ucByteCount)
                    {
                        ucCheckSum = ~((ucByteCount + 
                                    ((ulEntryAddress >> UPPER_BITS) & MASKER) + 
                                    ((ulEntryAddress >> LOWER_BITS) & MASKER) + 
                                    (ulEntryAddress & MASKER)));
                        fprintf(pstSrecFile, "S804%06X%02X\n",  
                                ulEntryAddress, ucCheckSum);
                        blCheck = true;
                    }
                    else
                    {
                        printf("Invalid Address Type for Terminator\n");
                    }
                }
            }

            pclose(pstPipe);
        }
    }

    if (true != blCheck)
    {
        printf("Failed to write SREC Terminator\n");
    }

    return blCheck;
}

// EOF