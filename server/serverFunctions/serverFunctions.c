//***************************** Server Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : serverFunctions.c
// Summary : Setups server to receive files from client
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 12/Dec/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "serverFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************
static bool serverReceiveFile(int16 nServerSocket);
static bool serverIgnorePath(int8 *pcFileName);
static bool serverGetIp(int8 *pcIpAddress);

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
    int16 nServerSocket = 0;
    struct sockaddr_in stServerAddr = {0};

    nServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (ERROR_CODE != nServerSocket) 
    {
        serverGetIp(cIpAddress);
        stServerAddr.sin_family = AF_INET;
        stServerAddr.sin_addr.s_addr = inet_addr(cIpAddress);
        stServerAddr.sin_port = htons(PORT);
        bind(nServerSocket, (struct sockaddr*)&stServerAddr, 
                             sizeof(stServerAddr));
        listen(nServerSocket, 1);
        printf("Server listening on port %d...\n", PORT);

        while (1) 
        {
           if (true == serverReceiveFile(nServerSocket))
           {
               printf("File received successfully.\n");
               blCheck = true;
           }
        }

        // The code is unlikely to reach here
        close(nServerSocket);
    }

    if(true != blCheck)
    {
        printf("Server side Failure\n");
    }

    return blCheck;
}

//**************************.serverReceiveFile.*********************************
// Purpose : Function to Receive a File from Client
// Inputs  : nServerSocket - The server socket descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverReceiveFile(int16 nServerSocket)
{
    bool blCheck = false;
    FILE *pstFile = NULL;
    int16 nClientSocket = 0;
    struct sockaddr_in stClientAddr = {0};
    uint32 ulAddressSize = 0;
    int8 cBuffer[BUFFER_SIZE] = {0};
    int8 cNameBuffer[FILE_NAME_SIZE] = {0};
    uint32 ulFileSize = 0;
    uint32 ulNameLength = 0;
    int8 cFileName[FILE_PATH_SIZE] = {0};
    uint32 ulTotalBytes = 0;
    int32 receivedBytes = 0;

    ulAddressSize = sizeof(stClientAddr);
    nClientSocket = accept(nServerSocket, 
        (struct sockaddr*)&stClientAddr, (socklen_t*)&ulAddressSize);

    if (-1 != nClientSocket)
    {
        // Receive filename
        ulNameLength = recv(nClientSocket, cNameBuffer, FILE_NAME_SIZE, 0);

        if (0 < ulNameLength) 
        {
            cNameBuffer[ulNameLength] = NULL_CHAR;
            serverIgnorePath(cNameBuffer);
            snprintf(cFileName, sizeof(cFileName), 
                    "%s/%s", RECEIVED_FOLDER, cNameBuffer);
            printf("Receiving: %s\n", cNameBuffer);

            // Receive file size
            if (0 < recv(nClientSocket, &ulFileSize, sizeof(ulFileSize), 0))
            {
                ulFileSize = ntohl(ulFileSize);

                // Receive and save file
                pstFile = fopen(cFileName, WRITE_BINARY);

                if (NULL != pstFile) 
                {
                    ulTotalBytes = 0;
                    receivedBytes = 0;

                    while ((ulTotalBytes < ulFileSize) && (0 < (receivedBytes = 
                        recv(nClientSocket, cBuffer, BUFFER_SIZE, 0)))) 
                    {
                        fwrite(cBuffer, 1, receivedBytes, pstFile);
                        ulTotalBytes += receivedBytes;
                    }

                    fclose(pstFile);
                    blCheck = true;
                }
            }
        }

        close(nClientSocket);
    }

    if(true != blCheck)
    {
        printf("Failed to receive file\n");
    }

    return blCheck;
}

//****************************.serverIgnorePath.********************************
// Purpose : Function to Ignore the Path in a File Name
// Inputs  : pcFileName - Pointer to the file name
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverIgnorePath(int8 *pcFileName)
{
    bool blCheck = false;
    uint32 ulIndex = 0;
    uint32 ulPosition = 0;
    uint32 ulNameLength = 0;
    uint8 ucFlag = 0;

    if (NULL != pcFileName)
    {
        while (NULL_CHAR != pcFileName[ulIndex])
        {
            if (PATH_SEPARATOR == pcFileName[ulIndex]) 
            {
                ulPosition = ulIndex;
                ucFlag = 1;
            }

            ulIndex ++;
        }

        if (1 == ucFlag) 
        {
            ulNameLength = strlen(pcFileName) - ulPosition;
            memmove(pcFileName, &pcFileName[ulPosition + 1], ulNameLength);
            pcFileName[ulNameLength - 1] = NULL_CHAR;
        }
        
        blCheck = true;
    }
    else
    {
        printf("Invalid file name\n");
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

// EOF