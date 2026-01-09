//******************************** JSON Server *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : jsonServer.c
// Summary : Setups server to receive GET, POST and PUT request and give 
//           appropriate response
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 09/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "jsonServer.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static uint16 sunCount = 0;

//***************************** Local Functions ********************************
static bool serverConnectWeb(int16 nServerSocket);
static bool serverSendFile(int16 nClientSocket, int8* pcFileName);
static bool serverRequestParse(int8* pcRequestBuffer, int8* pcRequest, 
                            int8* pcFilename, int8* pcFlag);
static bool serverReceiveRequest(int16 nClientSocket , int8* pcRequestBuffer);

//******************************.serverSetup.***********************************
// Purpose : Function to setup server to receive requests from client
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool serverSetup(void)
{
    bool blCheck = false;
    int16 nServerSocket = 0;
    int32 lOption = 0;
    struct sockaddr_in stServerAddr = {0};

    nServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (ERROR_CODE != nServerSocket) 
    {
        lOption = 1;
        setsockopt(nServerSocket, SOL_SOCKET, SO_REUSEADDR, 
                    &lOption, sizeof(lOption));
        stServerAddr.sin_family = AF_INET;
        stServerAddr.sin_addr.s_addr = inet_addr(LOCAL_HOST); 
        stServerAddr.sin_port = htons(PORT);

        if (ERROR_CODE != bind(nServerSocket, (struct sockaddr*)&stServerAddr, 
                             sizeof(stServerAddr))) 
        {
            if (ERROR_CODE != listen(nServerSocket, 1))
            {
                printf("Server listening on port %d...\n", PORT);

                while(1) 
                {
                    if (true == serverConnectWeb(nServerSocket))
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
        close(nServerSocket);
    }

    if(true != blCheck)
    {
        printf("Server side Failure\n");
    }

    return blCheck;
}

//****************************.serverConnectWeb.********************************
// Purpose : Function to connect to the web browser, receive request, process 
//           json data and send it in a file
// Inputs  : nServerSocket - Server socket file descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverConnectWeb(int16 nServerSocket)
{
    bool blCheck = false;
    struct sockaddr_in stClientAddr = {0};
    socklen_t nAddrLen = sizeof(stClientAddr);
    int16 nClientSocket = 0;
    int8 cFlag = 0;
    int8 cRequestBuffer[HTTP_SIZE] = {0};
    int8 cRequest[REQUEST_SIZE] = {0};
    int8 cFileName[FILENAME_SIZE] = {0};
    int8 *pcJsonString = NULL;
    cJSON *pstRoot = NULL;
    FILE *pstFile = NULL;

    nClientSocket = accept(nServerSocket, (struct sockaddr*)&stClientAddr, 
                    &nAddrLen);

    if (ERROR_CODE != nClientSocket) 
    {
        serverReceiveRequest(nClientSocket , cRequestBuffer);
        serverRequestParse(cRequestBuffer, cRequest, cFileName, &cFlag);

        if (0 == cFlag)
        {
            pstFile = fopen(cFileName, WRITE_MODE);

            if (NULL != pstFile)
            {
                pstRoot = cJSON_CreateObject();
                cJSON_AddNumberToObject(pstRoot, "count", sunCount);
                pcJsonString = cJSON_Print(pstRoot);
                fprintf(pstFile, "%s", pcJsonString);
                fclose(pstFile);

                // Cleanup
                free(pcJsonString);
                cJSON_Delete(pstRoot);
                serverSendFile(nClientSocket, cFileName);
                sunCount ++;
                blCheck = true;
            }
        }
        else
        {
            blCheck = true;
        }

        // Close Client Socket
        close(nClientSocket);
    } 
    else 
    {
        printf("Failed to accept client connection\n");
    }

    return blCheck;
}

//******************************.serverSendFile.********************************
// Purpose : Function to send a file over to browser
// Inputs  : nClientSocket - Client socket file descriptor
//           pcFileName - Stores filename
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverSendFile(int16 nClientSocket, int8* pcFileName)
{
    bool blCheck =  false;
    uint16 unFileSize = 0;
    uint16 unBytesRead = 0;
    int8 cHttpResponse[HTTP_SIZE] = {0};
    int8 cDataBuffer[HTTP_SIZE] = {0};
    FILE *pstFile = NULL;

    pstFile = fopen(pcFileName, READ_MODE);

    if (NULL != pstFile)
    {
        fseek(pstFile, 0, SEEK_END);
        unFileSize = ftell(pstFile);
        fseek(pstFile, 0, SEEK_SET);

        snprintf(cHttpResponse, sizeof(cHttpResponse),
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: application/octet-stream\r\n"
                    "Content-Disposition: attachment; filename =\"%s\"\r\n"
                    "Content-Length: %d\r\n"
                    "Connection: close\r\n\r\n",
                    pcFileName, unFileSize);

        send(nClientSocket, cHttpResponse, strlen(cHttpResponse), 0);

        while (0 < (unBytesRead = fread(cDataBuffer, 1, 
                                        sizeof(cDataBuffer), pstFile)))
        {
            send(nClientSocket, cDataBuffer, unBytesRead, 0);
        }

        fclose(pstFile);
        blCheck = true;
    }
    else
    {
        printf("Failed to send file as response\n");
    }

    return blCheck;
}

//*****************************.serverRequestParse.*****************************
// Purpose : Function to parse request
// Inputs  : pcRequestBuffer - Buffer containing received request
//           pcRequest - Stores the request method
//           pcFileName - Stores filename
//           pcFlag - Distinguishes different request
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverRequestParse(int8* pcRequestBuffer, int8* pcRequest, 
                            int8* pcFilename, int8* pcFlag)
{
    bool blCheck = false;
    int16 nRequestLength = 0;
    int16 nFilenameLength = 0;
    int8 *pcFirstSpace = NULL;
    int8 *pcBodyStart = NULL;
    int8 *pcLastSlash = NULL;
    int8 *pcLastSpace = NULL;
    cJSON *pstRoot = NULL;
    cJSON *pstValue = NULL;

    if ((NULL != pcRequestBuffer) && (NULL != pcRequest) && (NULL != pcFlag))
    {
        pcFirstSpace = strchr(pcRequestBuffer, SPACE);

        if (NULL != pcFirstSpace)
        {
            nRequestLength = pcFirstSpace - pcRequestBuffer;
            strncpy(pcRequest, pcRequestBuffer, nRequestLength);
            pcRequest[nRequestLength] = NULL_CHAR;
            printf("%s\n", pcRequest);
        }

        if ((0 == strcmp(pcRequest, POST)) || (0 == strcmp(pcRequest, PUT)))
        {
            pcBodyStart = strstr(pcRequestBuffer, CRLF_STR);

            if (NULL != pcBodyStart)
            {
                pcBodyStart += OFFSET;
                pstRoot = cJSON_Parse(pcBodyStart);

                if (NULL != pstRoot)
                {
                    pstValue = cJSON_GetObjectItem(pstRoot, COUNT);

                    if (true == cJSON_IsNumber(pstValue))
                    {
                        sunCount = (uint16)pstValue->valueint;
                        *pcFlag = 1;
                        blCheck = true;
                    }

                    cJSON_Delete(pstRoot);
                }
            }
        }
        else if (0 == strcmp(pcRequest, GET))
        {
            if (0 < nRequestLength)
            {
                pcLastSlash = strchr(pcRequestBuffer + nRequestLength + 
                                     PATH_OFFSET, BACKSLASH);

                if (NULL != pcLastSlash)
                {
                    pcLastSpace = strchr(pcLastSlash, SPACE);

                    if (NULL != pcLastSpace)
                    {
                        nFilenameLength = pcLastSpace - pcLastSlash - 1;
                        strncpy(pcFilename, pcLastSlash + 1, nFilenameLength);
                        pcFilename[nFilenameLength] = NULL_CHAR;
                        *pcFlag = 0;
                        blCheck = true;
                    }
                }
            }
        }
        else
        {
            printf("Invalid Request\n");
        }
    }
    else
    {
        printf("Failed to parse request\n");
    }

    return blCheck;
}

//***************************.serverReceiveRequest.*****************************
// Purpose : Function to receive request from browser
// Inputs  : nClientSocket - Client socket file descriptor
//           pcRequestBuffer - Buffer to store received request
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverReceiveRequest(int16 nClientSocket , int8* pcRequestBuffer)
{
    bool blCheck = false;
    bool blFlag = true;
    uint16 unTotalReceived = 0;
    uint16 unBodyLength = 0;
    uint16 unBytesReceived = 0;
    int16 nReceived = 0;
    int16 nBodyRemaining = 0;
    int8 *pcHeaderEnd = 0;
    int8 *pcBodyStart = 0;
    int8 *pcLengthPointer = 0;

    do
    {
        if (NULL != pcRequestBuffer)
        {
            while (HTTP_SIZE - 1 > unTotalReceived)
            {
                nReceived = recv(nClientSocket, pcRequestBuffer + 
                        unTotalReceived, HTTP_SIZE - unTotalReceived - 1, 0);

                if (0 >= nReceived)
                {
                    blFlag = false;
                    break;
                }

                unTotalReceived += nReceived;
                pcRequestBuffer[unTotalReceived] = NULL_CHAR;
                pcHeaderEnd = strstr (pcRequestBuffer, CRLF_STR);

                if (NULL != pcHeaderEnd)
                {
                    break;
                }
            }

            if (NULL == pcHeaderEnd)
            {
                break;
            }

            pcLengthPointer = strstr(pcRequestBuffer, "Content-Length: ");

            if (NULL != pcLengthPointer)
            {
                pcLengthPointer += CONTENT_SKIP;
                unBodyLength = atoi(pcLengthPointer);
            }

            pcBodyStart = pcHeaderEnd + OFFSET;
            unBytesReceived = pcRequestBuffer + unTotalReceived - pcBodyStart;
            nBodyRemaining = unBodyLength - unBytesReceived;

            if ((0 < nBodyRemaining) && (HTTP_SIZE >
                 unTotalReceived + nBodyRemaining))
            {
                while (0 < nBodyRemaining)
                {
                    nReceived = recv(nClientSocket, pcRequestBuffer + 
                        unTotalReceived, nBodyRemaining, 0);

                    if (0 >= nReceived)
                    {
                        blFlag = false;
                        break;
                    }

                    unTotalReceived += nReceived;
                    nBodyRemaining -= nReceived;
                    pcRequestBuffer[unTotalReceived] = NULL_CHAR;
                }
            }

            if (true == blFlag)
            {
                blCheck = true;
            }
        }

    } while (0);

    printf("%s\n", pcRequestBuffer);

    if (true != blCheck)
    {
        printf("Receiving request failed\n");
    }

    return blCheck;
}

// EOF