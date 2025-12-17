//***************************** Client Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : clientFunctions.c
// Summary : Setups client to send file to server
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 12/Dec/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "clientFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************
static bool clientFileSend(int8 *pArgVector[], int16 nSocket);
static bool clientIgnorePath(int8 *pcFilePath, int8 *pcFileName);

//*****************************.clientProcess.**********************************
// Purpose : Function to setup client.
// Inputs  : pArgVector - Pointer to the file name argument
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool clientSetup(int8 *pArgVector[])
{
    bool blCheck = false;
    struct sockaddr_in stServerAddr = {0};
    int16 nSocket = 0;
    uint32 ulEnable = 0;
    int8 cFileName[FILE_SIZE] = {0};
    int8 cFilePath[FILE_SIZE] = {0};

    nSocket = socket(AF_INET, SOCK_STREAM, 0);
  
    if (ERROR_CODE != nSocket) 
    {
        stServerAddr.sin_family = AF_INET;
        stServerAddr.sin_addr.s_addr = inet_addr(pArgVector[IP_ARG]);
        stServerAddr.sin_port = htons(PORT);

        if (ERROR_CODE != connect(nSocket, (struct sockaddr*)&stServerAddr, 
                            sizeof(stServerAddr))) 
        {
            ulEnable = 1;
            setsockopt(nSocket, IPPROTO_TCP, TCP_NODELAY, &ulEnable, 
                        sizeof(ulEnable));
            strcpy(cFilePath, pArgVector[1]);
            clientIgnorePath(cFilePath, cFileName);

            if (ERROR_CODE != send(nSocket, cFileName, 
                                    strlen(cFileName), 0))
            {
                if (true == clientFileSend(pArgVector, nSocket)) 
                {
                    blCheck = true;
                }
            }
        }

        close(nSocket);  
    }  

    if (true != blCheck) 
    {
        printf("Client side failed\n");
    }

    return blCheck;
}

//****************************.clientFileSend.**********************************
// Purpose : Function to send a file to the server
// Inputs  : pArgVector - Pointer to the file name argument
//           nSocket - Socket descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool clientFileSend(int8 *pArgVector[], int16 nSocket)
{
    bool blCheck = false;
    FILE *pstFile = {0};
    int8 cBuffer[BUFFER_SIZE] = {0};
    uint32 ulTotalSent = 0;
    uint32 ulBytesRead = 0;
    uint32 ulFileSize = 0;
    uint32 ulNetSize = 0;

    pstFile = fopen(pArgVector[1], READ_BINARY);

    if (NULL != pstFile) 
    {
        fseek(pstFile, 0, SEEK_END);
        ulFileSize = ftell(pstFile);
        ulNetSize = htonl(ulFileSize);

        if (ERROR_CODE != send(nSocket, &ulNetSize, sizeof(uint32), 0)) 
        {
            // Send file data
            printf("Uploading %s (%d Bytes)...\n", pArgVector[1], ulFileSize);
            fseek(pstFile, 0, SEEK_SET);

            while (0 <  (ulBytesRead = fread(cBuffer, 1, 
                                        BUFFER_SIZE, pstFile))) 
            {
                if (ERROR_CODE != send(nSocket, cBuffer, ulBytesRead, 0)) 
                {
                    ulTotalSent += ulBytesRead;
                    printf("Uploaded: %d/%d bytes\n", ulTotalSent, ulFileSize);
                    blCheck = true;
                    usleep(MIN_DELAY);
                }
            }

            printf("Upload complete: %s (%d bytes)\n", 
                    pArgVector[1], ulTotalSent);
        }

        fclose(pstFile);
    }

    if (true != blCheck) 
    {
        printf("File transfer failed\n");
    }

    return blCheck;
}

//****************************.clientIgnorePath.********************************
// Purpose : Function to Ignore the Path in a File Name
// Inputs  : pcFilePath - Pointer to the file path
//           pcFileName - Pointer to the file name
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool clientIgnorePath(int8 *pcFilePath, int8 *pcFileName)
{
    bool blCheck = false;
    uint32 ulIndex = 0;
    uint32 ulPosition = 0;
    uint32 ulNameLength = 0;
    uint8 ucFlag = 0;

    if ((NULL != pcFileName) && (NULL != pcFilePath))
    {
        while (NULL_CHAR != pcFilePath[ulIndex])
        {
            if (PATH_SEPARATOR == pcFilePath[ulIndex]) 
            {
                ulPosition = ulIndex;
                ucFlag = 1;
            }

            ulIndex ++;
        }

        if (1 == ucFlag) 
        {
            ulNameLength = strlen(pcFilePath) - ulPosition;
            memcpy(pcFileName, &pcFilePath[ulPosition + 1], ulNameLength);
            pcFileName[ulNameLength - 1] = NULL_CHAR;
        }
        else
        {
            strcpy(pcFileName, pcFilePath);
        }

        blCheck = true;
    }
    else
    {
        printf("Invalid file name\n");
    }

    return blCheck;
}

// EOF