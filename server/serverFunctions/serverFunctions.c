//***************************** Server Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : serverFunctions.c
// Summary : Setups server to receive files from client
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 15/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "serverFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static uint8 sucFlag = 0;

//***************************** Local Functions ********************************
static bool serverReceiveRequest(int32 lServerSocket);
static bool serverReceiveFile(int32 lClientSocket);
static bool serverReceiveRange(int32 lClientSocket);
static bool serverGetIp(int8 *pcIpAddress);
static bool serverRecordParse(uint32 ulStartAddr, uint32 ulEndAddr);

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
                        blCheck = true;
                    }
                }
                else if (0 == strcmp(cFlagBuffer, ADDRESS_FLAG))
                {
                    if (1 == sucFlag)
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

        remove(FILE_NAME);
        sucFlag = 0;
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
// Inputs  : lServerSocket - The server socket descriptor
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
        pstFile = fopen(FILE_NAME, WRITE_BINARY);

        if (NULL != pstFile) 
        {
            while ((ulTotalBytes < ulFileSize) && (0 < (lReceivedBytes = 
                recv(lClientSocket, cBuffer, BUFFER_SIZE, 0)))) 
            {
                fwrite(cBuffer, 1, lReceivedBytes, pstFile);
                ulTotalBytes += lReceivedBytes; 
            }

            fclose(pstFile);
            sucFlag = 1;
            blCheck = true;
        }
    }

    if (true != blCheck)
    {
        printf("Failed to open file\n");
    }

    return blCheck;
}

//**************************.serverReceiveRange.********************************
// Purpose : Function to Receive a memory range from client
// Inputs  : lServerSocket - The server socket descriptor
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
    int8 cCommand[MAX_IP_SIZE] = {0};
    uint8 ucLength = 0;

    if (NULL != pcIpAddress)
    {
        snprintf(cCommand, sizeof(cCommand), GET_IP);
        pstPipe = popen(cCommand, READ_COMMAND);

        if (NULL != pstPipe)
        {
            if (NULL != fgets(pcIpAddress, MAX_IP_SIZE, pstPipe)) 
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


    pstFile = fopen(FILE_NAME, READ_COMMAND);

    if (NULL != pstFile)
    {
        while (NULL != fgets(cReadBuffer, BUFFER_SIZE, pstFile))
        {
            if (NULL != strstr(cReadBuffer, START_ONE))
            {
                parseTypeOne(cReadBuffer, ulStartAddr, ulEndAddr);
            }
            else if (NULL != strstr(cReadBuffer, START_TWO))
            {
                parseTypeTwo(cReadBuffer, ulStartAddr, ulEndAddr);
            }
            else if (NULL != strstr(cReadBuffer, START_THREE))
            {
                parseTypeThree(cReadBuffer, ulStartAddr, ulEndAddr);
            }
            else
            {
                continue;
            }
        }

        fclose(pstFile);
        printf("\n");
        parseCheckRange();
        blCheck = true;
    }
    else
    {
        printf("File was not received first\n");
    }

    if (true != blCheck)
    {
        printf("Failed to parse S record Data\n");
    }

    return blCheck;
}

// EOF