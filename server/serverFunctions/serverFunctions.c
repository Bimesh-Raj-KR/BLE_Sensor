//***************************** Server Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : serverFunctions.c
// Summary : Setups server to receive files and memory range from client
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 20/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "serverFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************
static bool serverReceiveRequest(int32 lServerSocket);
static bool serverReceiveFile(int32 lClientSocket);
static bool serverReceiveRange(int32 lClientSocket);
static bool serverGetIp(int8 *pcIpAddress);
static bool serverRecordParse(uint32 ulStartAddr, uint32 ulEndAddr);
static bool serverReceiveConvert(int32 lClientSocket);
static bool serverConvertToSrec(uint32 ulFileSize, int32 lClientSocket);
static bool serverCheckBinary(void);
static bool serverTypeConvert(uint32 ulFileSize);
static bool serverSendSrecFile(int32 lClientSocket);

//******************************.serverSetup.***********************************
// Purpose : Function to setup server to receive files from client
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool serverSetup(void)
{
    bool blCheck = false;
    int8 cIpAddress[MAX_IP_SIZE] = {0};
    int32 lServerSocket = 0;
    struct sockaddr_in stServerAddr = {0};

    lServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (ERROR_CODE != lServerSocket) 
    {
        serverGetIp(cIpAddress);
        stServerAddr.sin_family = AF_INET;
        stServerAddr.sin_addr.s_addr = inet_addr(cIpAddress);
        stServerAddr.sin_port = htons(PORT);
        if (ERROR_CODE != bind(lServerSocket, (struct sockaddr*)&stServerAddr, 
                             sizeof(stServerAddr))) 
        {
            if (ERROR_CODE != listen(lServerSocket, 1))
            {
                printf("Server listening on port %d...\n", PORT);

                while (1) 
                {
                    if (true == serverReceiveRequest(lServerSocket))
                    {
                        blCheck = true;
                    }
                }
            }
            else 
            {
                printf("Listen failed\n");
            }
        }
        else 
        {
            printf("Bind failed\n");
        }

        // The code is unlikely to reach here
        close(lServerSocket);
    }

    if(true != blCheck)
    {
        printf("Server side Failure\n");
    }

    return blCheck;
}

//**************************.serverReceiveRequest.******************************
// Purpose : Function to receive and process a request from Client
// Inputs  : lServerSocket - The server socket descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverReceiveRequest(int32 lServerSocket)
{
    bool blCheck = false;
    uint32 ulAddressSize = 0;
    int32 lClientSocket = 0;
    int16 nBytesReceived = 0;
    uint8 ucFlag = 0;
    int8 cFlagBuffer[FLAG_SIZE] = {0};
    struct sockaddr_in stClientAddr = {0};

    ulAddressSize = sizeof(stClientAddr);
    lClientSocket = accept(lServerSocket, 
        (struct sockaddr*)&stClientAddr, (socklen_t*)&ulAddressSize);

    if (ERROR_CODE != lClientSocket)
    {
        while (1)
        {
            memset(cFlagBuffer, 0, FLAG_SIZE);
            nBytesReceived = recv(lClientSocket, cFlagBuffer, FLAG_SIZE, 0);

            if (0 < nBytesReceived)
            {
                if (0 == strcmp(cFlagBuffer, FILE_FLAG))
                {
                    if (true == serverReceiveFile(lClientSocket))
                    {
                        ucFlag = 1;
                        blCheck = true;
                    }
                }
                else if (0 == strcmp(cFlagBuffer, ADDRESS_FLAG))
                {
                    if (1 == ucFlag)
                    {
                        if (true == serverReceiveRange(lClientSocket))
                        {
                            blCheck = true;
                        }
                    }
                    else
                    {
                        printf("File was not received first\n");
                    }
                }
                else if (0 == strcmp(cFlagBuffer, CONVERT_FLAG))
                {
                    if (true == serverReceiveConvert(lClientSocket))
                    {
                        blCheck = true;
                    }
                }
                else
                {
                    printf("Invalid request flag\n");
                }
            }
            else if (0 == nBytesReceived)
            {
                printf("Client closed connection. exiting...\n");
                break;
            }
            else
            {
                break;
            }
        }

        remove(PARSE_FILE_NAME);
        close(lClientSocket);
    }

    if (true != blCheck)
    {
        printf("Failed to receive client request\n");
    }

    return blCheck;
}

//**************************.serverReceiveFile.*********************************
// Purpose : Function to Receive file content from Client
// Inputs  : lClientSocket - The client socket descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverReceiveFile(int32 lClientSocket)
{
    bool blCheck = false;
    FILE *pstFile = NULL;
    uint32 ulFileSize = 0;
    uint32 ulTotalBytes = 0;
    int32 lReceivedBytes = 0;
    int8 cBuffer[BUFFER_SIZE] = {0};

    
    if (0 < recv(lClientSocket, &ulFileSize, sizeof(ulFileSize), 0))
    {
        ulFileSize = ntohl(ulFileSize);

        // Receive and save file
        pstFile = fopen(PARSE_FILE_NAME, WRITE_BINARY);

        if (NULL != pstFile) 
        {
            while ((ulTotalBytes < ulFileSize) && (0 < (lReceivedBytes = 
                recv(lClientSocket, cBuffer, BUFFER_SIZE, 0)))) 
            {
                fwrite(cBuffer, 1, lReceivedBytes, pstFile);
                ulTotalBytes += lReceivedBytes; 
            }

            fclose(pstFile);
            blCheck = true;
        }
    }

    if (true == blCheck)
    {
        printf("Successfully received file from client\n");
    }
    else
    {
        printf("Failed to open file\n");
    }

    return blCheck;
}

//**************************.serverReceiveRange.********************************
// Purpose : Function to Receive a memory range from client
// Inputs  : lClientSocket - The client socket descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverReceiveRange(int32 lClientSocket)
{
    bool blCheck = false;
    uint32 ulStartAddr = 0;
    uint32 ulEndAddr = 0;
    uint32 ulFormatAddr = 0;

    recv(lClientSocket, &ulFormatAddr, sizeof(ulFormatAddr), 0);
    ulStartAddr = ntohl(ulFormatAddr);
    recv(lClientSocket, &ulFormatAddr, sizeof(ulFormatAddr), 0);
    ulEndAddr = ntohl(ulFormatAddr);

    if (0 < ulEndAddr)
    {
        if (ulStartAddr < ulEndAddr)
        {
            if (true == serverRecordParse(ulStartAddr, ulEndAddr))
            {
                blCheck = true;
            }
        }
        else
        {
            printf("Start address should be smaller than End address\n");
        }
    }
    else
    {
        printf("Failed to receive memory range\n");
    }

    return blCheck;
}

//**************************.serverReceiveConvert.******************************
// Purpose : Function to Receive a file to convert from client
// Inputs  : lClientSocket - The client socket descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverReceiveConvert(int32 lClientSocket)
{
    bool blCheck = false;
    FILE *pstFile = NULL;
    uint32 ulFileSize = 0;
    uint32 ulTotalBytes = 0;
    int32 lReceivedBytes = 0;
    int8 cBuffer[BUFFER_SIZE] = {0};

    
    if (0 < recv(lClientSocket, &ulFileSize, sizeof(ulFileSize), 0))
    {
        ulFileSize = ntohl(ulFileSize);
        pstFile = fopen(BIN_FILE_NAME, WRITE_BINARY);

        if (NULL != pstFile) 
        {
            while ((ulTotalBytes < ulFileSize) && (0 < (lReceivedBytes = 
                recv(lClientSocket, cBuffer, BUFFER_SIZE, 0)))) 
            {
                fwrite(cBuffer, 1, lReceivedBytes, pstFile);
                ulTotalBytes += lReceivedBytes; 
            }

            fclose(pstFile);

            serverConvertToSrec(ulFileSize, lClientSocket);
            remove(SREC_FILE_NAME);
            remove(BIN_FILE_NAME);
            blCheck = true;
        }
    }

    if (true != blCheck)
    {
        printf("Failed to receive binary file\n");
    }

    return blCheck;
}

//********************************.serverGetIp.*********************************
// Purpose : Function to Get the IP Address of the Server
// Inputs  : pcIpAddress - Pointer to store the IP Address
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverGetIp(int8 *pcIpAddress)
{
    bool blCheck = false;
    FILE *pstPipe = NULL;
    int8 cCommand[MAX_COMMAND_SIZE] = {0};
    uint8 ucLength = 0;

    if (NULL != pcIpAddress)
    {
        snprintf(cCommand, sizeof(cCommand), GET_IP);
        pstPipe = popen(cCommand, READ_COMMAND);

        if (NULL != pstPipe)
        {
            if (NULL != fgets(pcIpAddress, MAX_COMMAND_SIZE, pstPipe)) 
            {
                ucLength = strlen(pcIpAddress);

                if ((0 < ucLength) && 
                    (NEWLINE_CHAR == pcIpAddress[ucLength - 1])) 
                {
                    pcIpAddress[ucLength - 1] = NULL_CHAR;
                    blCheck = true;
                }
            }

            pclose(pstPipe);
        }
    }

    if (true != blCheck)
    {
        printf("Failed to get IP Address\n");
    }

    return blCheck;
}

//****************************.serverRecordParse.*******************************
// Purpose : Function to parse an S record file
// Inputs  : ulStartAddr - Start of memory range
//           ulEndAddr - End of memory range
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverRecordParse(uint32 ulStartAddr, uint32 ulEndAddr)
{
    bool blCheck = false;
    FILE *pstFile = NULL;
    int8 cReadBuffer[BUFFER_SIZE] = {0};
    int8 ucFlag = 0;


    pstFile = fopen(PARSE_FILE_NAME, READ_COMMAND);

    if (NULL != pstFile)
    {
        while (NULL != fgets(cReadBuffer, BUFFER_SIZE, pstFile))
        {
            if (NULL != strstr(cReadBuffer, START_ONE))
            {
                parseTypeOne(cReadBuffer, ulStartAddr, ulEndAddr);
                ucFlag = 1;
            }
            else if (NULL != strstr(cReadBuffer, START_TWO))
            {
                parseTypeTwo(cReadBuffer, ulStartAddr, ulEndAddr);
                ucFlag = 1;
            }
            else if (NULL != strstr(cReadBuffer, START_THREE))
            {
                parseTypeThree(cReadBuffer, ulStartAddr, ulEndAddr);
                ucFlag = 1;
            }
            else
            {
                continue;
            }
        }

        if (1 == ucFlag)
        {
            printf("\n**********\n");
            parseCheckRange();
        }
        else
        {
            printf("The file is not in S record format\n");
        }

        fclose(pstFile);
        blCheck = true;

    }
    else
    {
        printf("Failed to open S record file\n");
    }

    if (true != blCheck)
    {
        printf("Failed to parse S record Data\n");
    }

    return blCheck;
}

//**************************.serverConvertToSrec.*******************************
// Purpose : Function to Receive a file to convert to srec from client
// Inputs  : ulFileSize - Size of the file
//           lClientSocket - The client socket descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverConvertToSrec(uint32 ulFileSize, int32 lClientSocket)
{
    bool blCheck = false;

    if (true == serverCheckBinary())
    {
        serverTypeConvert(ulFileSize);
        serverSendSrecFile(lClientSocket);
        blCheck = true;
    }
    else
    {
        printf("Failed to receive file to convert\n");
    }

    return blCheck;
}

//**************************.serverCheckBinary.*********************************
// Purpose : Function to check if the received file is a binary
// Inputs  : None
// Outputs : None
// Return  : true if the received file is a binary and false if it is not or if 
//           any errors exist during function execution
// Notes   : None
//******************************************************************************
static bool serverCheckBinary(void)
{
    bool blCheck = false;
    FILE *pstPipe = NULL;
    int8 cCommand[MAX_COMMAND_SIZE] = {0};
    int8 cResultBuffer[MAX_COMMAND_SIZE] = {0};

    snprintf(cCommand, sizeof(cCommand), "file %s", BIN_FILE_NAME);

    pstPipe = popen(cCommand, READ_COMMAND);

    if (NULL != pstPipe)
    {
        if (NULL != fgets(cResultBuffer, MAX_COMMAND_SIZE, pstPipe)) 
        {
            if (NULL != strstr(cResultBuffer, ELF))
            {
                blCheck = true;
            }
            else
            {
                printf("Received file is not a binary\n");
            }
        }

        pclose(pstPipe);
    }

    if (true != blCheck)
    {
        printf("Failed binary check\n");
    }

    return blCheck;
}

//***************************.serverTypeConvert.********************************
// Purpose : Function to convert binary to S record format
// Inputs  : ulFileSize - Size of the file
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverTypeConvert(uint32 ulFileSize)
{
    bool blCheck = false;
    uint32 ulLineStartAddr = 0;
    uint32 ulAddrLength = 0;
    uint8 ucIndex = 0;
    FILE *pstSrecFile = NULL;
    FILE *pstBinFile = NULL;
    Elf64_Ehdr stElfHeader = {0};
    Elf64_Shdr stSectionHeader = {0};

    pstBinFile = fopen(BIN_FILE_NAME, READ_BINARY);

    if (NULL != pstBinFile)
    {
        pstSrecFile = fopen(SREC_FILE_NAME, WRITE_BINARY);

        if (NULL != pstSrecFile)
        {
            writeHeader(pstSrecFile);
            fread(&stElfHeader, 1, sizeof(Elf64_Ehdr), pstBinFile);

            // Iterate through all section headers
            for (ucIndex = 0; ucIndex < stElfHeader.e_shnum; ucIndex++)
            {
                // Read section header
                fseek(pstBinFile, stElfHeader.e_shoff + 
                      (ucIndex * stElfHeader.e_shentsize), SEEK_SET);
                fread(&stSectionHeader, 1, sizeof(Elf64_Shdr), pstBinFile);

                // Check for allocated sections with data
                if ((ALLOCATED_FLAG == (stSectionHeader.sh_flags & SHF_ALLOC)) 
                    && (SHT_NOBITS != stSectionHeader.sh_type) && 
                    (0 < stSectionHeader.sh_size)) 
                {
                    // Set Start address and length
                    ulLineStartAddr = (uint32)stSectionHeader.sh_addr;
                    ulAddrLength = (uint32)stSectionHeader.sh_size;

                    // Move file pointer to section offset
                    fseek(pstBinFile, stSectionHeader.sh_offset, SEEK_SET);

                    if (SIZE_LIMIT > ulFileSize)
                    {
                        writeTypeOne(pstBinFile, pstSrecFile, 
                            ulLineStartAddr, ulAddrLength);
                    }
                    else
                    {
                        writeTypeTwo(pstBinFile, pstSrecFile, 
                            ulLineStartAddr, ulAddrLength);
                    }
                }
            }

            if (SIZE_LIMIT > ulFileSize)
            {
                writeTerminator(pstSrecFile, TYPE_ONE_COUNT);
            }
            else
            {
                writeTerminator(pstSrecFile, TYPE_TWO_COUNT);
            }

            fclose(pstSrecFile);
            blCheck = true;
        }

        fclose(pstBinFile);
    }

    if (true != blCheck)
    {
        printf("Failed to convert binary to S record format\n");
    }

    return blCheck;
}

//*************************.serverSendSrecFile.*********************************
// Purpose : Function to send S record file to client
// Inputs  : lClientSocket - The client socket descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverSendSrecFile(int32 lClientSocket)
{
    bool blCheck = false;
    FILE *pstFile = NULL;
    int8 cBuffer[BUFFER_SIZE] = {0};
    uint32 ulBytesRead = 0;
    uint32 ulFileSize = 0;
    uint32 ulNetSize = 0;

    pstFile = fopen(SREC_FILE_NAME, READ_BINARY);

    if ((NULL != pstFile)) 
    {
        fseek(pstFile, 0, SEEK_END);
        ulFileSize = ftell(pstFile);
        fseek(pstFile, 0, SEEK_SET);
        ulNetSize = htonl(ulFileSize);

        if (ERROR_CODE != send(lClientSocket, &ulNetSize, sizeof(ulNetSize), 0)) 
        {
            while (0 <  (ulBytesRead = fread(cBuffer, 1, 
                                        BUFFER_SIZE, pstFile))) 
            {
                if (ERROR_CODE != send(lClientSocket, cBuffer, ulBytesRead, 0)) 
                {
                    usleep(MIN_DELAY);
                    blCheck = true;
                }
                else
                {
                    blCheck = false;
                    break;
                }
            }
        }

        fclose(pstFile);
    }
    else
    {
        printf("Invalid file path\n");
    }

    if (true == blCheck) 
    {
        printf("Converted file sent successfully\n");
    }
    else
    {
        printf("Converted file transfer failed\n");
        printf("Closing connection...\n");
    }

    return blCheck;
}

// EOF

