//******************************** JSON Server *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : jsonServer.c
// Summary : Setups server to receive GET, POST and PUT request and give 
//           appropriate response
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "jsonServer.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static uint16 sunCount = 0;

//***************************** Local Functions ********************************
static bool serverConnectWeb(int16 nServerSocket);
static bool serverReceiveRequest(int16 nClientSocket);
static bool serverGetCount(int8* pcResponseBuffer);
static bool serverSetCount(int8* pcRequestBuffer, int8* pcResponseBuffer);

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
//           json data and send back response
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
    socklen_t nAddrLen = 0;
    int16 nClientSocket = 0;

    nAddrLen = sizeof(stClientAddr);
    nClientSocket = accept(nServerSocket, (struct sockaddr*)&stClientAddr, 
                    &nAddrLen);

    if (ERROR_CODE != nClientSocket) 
    {
        if (true == serverReceiveRequest(nClientSocket))
        {
            blCheck = true;
        }

        close(nClientSocket);
    } 
    else 
    {
        printf("Failed to accept client connection\n");
    }

    return blCheck;
}

//***************************.serverReceiveRequest.*****************************
// Purpose : Function to receive request from browser
// Inputs  : nClientSocket - Client socket file descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverReceiveRequest(int16 nClientSocket)
{
    bool blCheck = false;
    bool blFlag = true;
    int16 nReceived = 0;
    int8 cRequestBuffer[BUFFER_SIZE] = {0};
    int8 cResponseBuffer[BUFFER_SIZE] = {0};

    nReceived = recv(nClientSocket, cRequestBuffer, BUFFER_SIZE - 1, 0);

    if (0 < nReceived)
    {
        cRequestBuffer[nReceived] = NULL_CHAR;

        if (NULL != strstr(cRequestBuffer, GET))
        {
            serverGetCount(cResponseBuffer);
        }
        else if ((NULL != strstr(cRequestBuffer, POST)) || 
                 (NULL != strstr(cRequestBuffer, PUT)))
        {
            serverSetCount(cRequestBuffer, cResponseBuffer);
        }
        else
        {
            printf("Invalid request\n");
            blFlag = false;
        }

        if (true == blFlag)
        {
            send(nClientSocket, cResponseBuffer, strlen(cResponseBuffer), 0);
            blCheck = true;
        }
    }

    if (true != blCheck)
    {
        printf("Receiving request failed\n");
    }

    return blCheck;
}

//*********************************.serverGetCount.*****************************
// Purpose : Function to get current count and send it to browser in JSON format
// Inputs  : pcResponseBuffer - Buffer to store server response
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverGetCount(int8* pcResponseBuffer)
{
    bool blCheck = false;
    int8 *pcJsonString = NULL;
    cJSON *pstRoot = NULL;

    if (NULL != pcResponseBuffer)
    {
        pstRoot = cJSON_CreateObject();

        if (NULL != pstRoot)
        {
            cJSON_AddNumberToObject(pstRoot, COUNT, sunCount);
            pcJsonString = cJSON_Print(pstRoot);

            if (NULL != pcJsonString)
            {
                snprintf(pcResponseBuffer, BUFFER_SIZE, "%s", pcJsonString);
                free(pcJsonString);
                blCheck = true;
            }

            cJSON_Delete(pstRoot);
            sunCount ++;
        }
    }

    if (true != blCheck)
    {
        printf("Failed to get current count\n");
    }

    return blCheck;
}

//*******************************.serverSetCount.*******************************
// Purpose : Function for the browser to update count
// Inputs  : pcRequestBuffer - Buffer to store received request
//           pcResponseBuffer - Buffer to store generated response
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverSetCount(int8* pcRequestBuffer, int8* pcResponseBuffer)
{
    bool blCheck = false;
    int8 *pcCountPointer = NULL;
    cJSON *pstRoot = NULL;
    cJSON *pstValue = NULL;

    if ((NULL != pcRequestBuffer) && (NULL != pcResponseBuffer))
    {
        pcCountPointer = strchr(pcRequestBuffer, PIPE);

        if (NULL != pcCountPointer)
        {
            pcCountPointer += 1;
            pstRoot = cJSON_Parse(pcCountPointer);

            if (NULL != pstRoot)
            {
                pstValue = cJSON_GetObjectItem(pstRoot, COUNT);

                if (NULL != pstValue)
                {
                    if (true == cJSON_IsNumber(pstValue))
                    {
                        sunCount = (uint16)pstValue->valueint;
                        sprintf(pcResponseBuffer, "SUCCESS");
                        blCheck = true;
                    }
                }

                cJSON_Delete(pstRoot);
            }
        }
    }

    if (true != blCheck)
    {
        printf("Failed to set current count\n");
    }

    return blCheck;
}

// EOF