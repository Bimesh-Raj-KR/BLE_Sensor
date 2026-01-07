//******************************** JSON Server *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : jsonServer.c
// Summary : Setups server to send current date and time to client in 
//           JSON format
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 29/Dec/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "jsonServer.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************
static bool serverConnectWeb(int16 nServerSocket);
static bool serverSendFile(int16 nClientSocket, int8* pcFileName);
static bool serverRequestParse(int8* pcRequestBuffer, int8* pcApi, 
                            int8* pcFileName);
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
    bool blFlag = true;
    struct sockaddr_in stClientAddr = {0};
    struct tm *pstFormattedTime = NULL;
    socklen_t nAddrLen = sizeof(stClientAddr);
    time_t lCurrentTime = 0;
    int16 nClientSocket = 0;
    int8 cRequestBuffer[HTTP_SIZE] = {0};
    int8 cApi[API_SIZE] = {0};
    int8 cFileName[FILENAME_SIZE] = {0};
    int8 cCurrentTime[MAX_SIZE] = {0};
    int8 cCurrentDate[MAX_SIZE] = {0};
    int8 *pcJsonString = NULL;
    cJSON *pstRoot = NULL;
    FILE *pstFile = NULL;

    nClientSocket = accept(nServerSocket, (struct sockaddr*)&stClientAddr, 
                    &nAddrLen);

    if (ERROR_CODE != nClientSocket) 
    {
        serverReceiveRequest(nClientSocket , cRequestBuffer);
        serverRequestParse(cRequestBuffer, cApi, cFileName);

        if (0 == strcmp(WEB_TITLE, cApi))
        {
            pstFile = fopen(cFileName, WRITE_MODE);

            if (NULL != pstFile)
            {
                pstRoot = cJSON_CreateObject();
                cJSON_AddStringToObject(pstRoot, "Title", "Server Time ");
                cJSON_AddStringToObject(pstRoot, "Brackets", 
                                        "(updates every second)");
                pcJsonString = cJSON_Print(pstRoot);
                fprintf(pstFile, "%s", pcJsonString);
                fclose(pstFile);

                // Cleanup
                free(pcJsonString);
                cJSON_Delete(pstRoot);
            }
        }
        else if (0 == strcmp(DATE_AND_TIME, cApi))
        {
            pstFile = fopen(cFileName, WRITE_MODE);

            if (NULL != pstFile)
            {
                lCurrentTime = time(NULL);
                pstFormattedTime = localtime(&lCurrentTime);
                strftime(cCurrentDate, sizeof(cCurrentDate), "%d-%m-%Y", 
                pstFormattedTime);
                strftime(cCurrentTime, sizeof(cCurrentTime), "%H:%M:%S", 
                pstFormattedTime);

                // Dynamic JSON Creation
                pstRoot = cJSON_CreateObject();
                cJSON_AddStringToObject(pstRoot, "Date", cCurrentDate);
                cJSON_AddStringToObject(pstRoot, "Time", cCurrentTime);
                pcJsonString = cJSON_Print(pstRoot);
                fprintf(pstFile, "%s", pcJsonString);
                fclose(pstFile);

                // Cleanup
                free(pcJsonString);
                cJSON_Delete(pstRoot);
            }
        }
        else
        {
            blFlag = false;
            printf("Invalid request\n");
        }

        if (true == blFlag)
        {
            serverSendFile(nClientSocket, cFileName);
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
//           pcApi - Stores API
//           pcFileName - Stores filename
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverRequestParse(int8* pcRequestBuffer, int8* pcApi, 
                            int8* pcFileName)
{
    bool blCheck = false;
    int16 nPathLength = 0;
    int8 cPath[PATH_SIZE] = {0};
    int8 *pcPathStart = NULL;
    int8 *pcPathEnd = NULL;
    int8 *pcLastSlash = NULL;

    if ((NULL != pcRequestBuffer) && (NULL != pcApi))
    {
        pcPathStart = strstr(pcRequestBuffer, GET);
        pcPathEnd = strstr(pcRequestBuffer, HTTP);

        if ((NULL != pcPathStart) && (NULL != pcPathEnd))
        {
            pcPathStart += OFFSET;
            nPathLength = pcPathEnd - pcPathStart;

            if ((0 < nPathLength) && (PATH_SIZE - 1 > nPathLength))
            {
                strncpy(cPath, pcPathStart, nPathLength);
                cPath[nPathLength] = NULL_CHAR;
                pcLastSlash =  strrchr(cPath, BACKSLASH);

                if (NULL != pcLastSlash)
                {
                    strncpy(pcApi, cPath, pcLastSlash - cPath);
                    strncpy(pcFileName, pcLastSlash + 1, 
                        strlen(cPath) - (pcLastSlash - cPath + 1));
                    pcApi[pcLastSlash - cPath] = NULL_CHAR;
                    pcFileName[strlen(cPath) - (pcLastSlash - cPath + 1)]
                                 = NULL_CHAR;
                    blCheck = true;
                }
            }
        }
    }
    else
    {
        printf("Failed to parse API and Filename\n");
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
    uint16 unTotalReceived = 0;
    int16 nReceived = 0;

    if (NULL != pcRequestBuffer)
    {
        do
        {
            nReceived = recv(nClientSocket, pcRequestBuffer + unTotalReceived, 
                            HTTP_SIZE - unTotalReceived - 1, 0);
            if (0 >= nReceived)
            {
                break;
            }

            unTotalReceived += nReceived;
            pcRequestBuffer[unTotalReceived] = NULL_CHAR;

        } while ((HTTP_SIZE - 1 > unTotalReceived) && 
            (NULL == strstr(pcRequestBuffer, CRLF_STR)));

        blCheck = true;
    }
    else
    {
        printf("Receiving request failed\n");
    }

    return blCheck;
}

// EOF