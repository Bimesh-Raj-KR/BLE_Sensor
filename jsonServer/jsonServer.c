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
static bool serverSendTime(int16 nServerSocket);

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
    int16 nServerSocket = 0;
    struct sockaddr_in stServerAddr = {0};

    nServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (ERROR_CODE != nServerSocket) 
    {
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
                    if (true == serverSendTime(nServerSocket))
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

//******************************.serverSendTime.********************************
// Purpose : Function to send current date and time to the client
// Inputs  : nServerSocket - Server socket file descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool serverSendTime(int16 nServerSocket)
{
    bool blCheck = false;
    struct sockaddr_in stClientAddr = {0};
    struct tm *pstFormattedTime = NULL;
    socklen_t nAddrLen = sizeof(stClientAddr);
    time_t lCurrentTime = 0;
    int16 nClientSocket = 0;
    int8 httpResponse[HTTP_SIZE] = {0};
    int8 cCurrentTime[MAX_SIZE] = {0};
    int8 cCurrentDate[MAX_SIZE] = {0};
    int8 *pcJsonString = NULL;
    cJSON *pstRoot = NULL;

    nClientSocket = accept(nServerSocket, (struct sockaddr*)&stClientAddr, 
                    &nAddrLen);

    if (ERROR_CODE != nClientSocket) 
    {
        lCurrentTime = time(NULL);
        pstFormattedTime = localtime(&lCurrentTime);
        strftime(cCurrentDate, sizeof(cCurrentDate), "%d-%m-%Y", 
        pstFormattedTime);
        strftime(cCurrentTime, sizeof(cCurrentTime), "%H:%M:%S", 
        pstFormattedTime);

        // Dyanamic JSON Creation
        pstRoot = cJSON_CreateObject();
        cJSON_AddStringToObject(pstRoot, "Date", cCurrentDate);
        cJSON_AddStringToObject(pstRoot, "Time", cCurrentTime);
        pcJsonString = cJSON_Print(pstRoot);
        
        // Send HTTP Response with JSON
        snprintf(httpResponse, sizeof(httpResponse),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n\r\n"
            "%s",
            strlen(pcJsonString), pcJsonString);
        send(nClientSocket, httpResponse, strlen(httpResponse), 0);

        // Cleanup
        free(pcJsonString);
        cJSON_Delete(pstRoot);

        // Close Client Socket
        close(nClientSocket);
        blCheck = true;
    } 
    else 
    {
        printf("Failed to accept client connection\n");
    }

    return blCheck;
}
