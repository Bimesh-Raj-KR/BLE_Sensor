//********************************* CGI Parser *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : cgiParser.c
// Summary : Act as a bridge between web server and backend to parse request 
//           headers and send back standard responses
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 13/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "cgiParser.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************
static bool cgiReceiveAndParse(int16 nClientSocket);
static bool cgiSendToWeb(int8* pcResponseBuffer, int8* pcFileName, int8 cFlag);

//******************************.cgiBridgeSetup.********************************
// Purpose : Function to setup bridge between nginx and backend
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool cgiBridgeSetup(void)
{
    bool blCheck = false;
    int16 nClientSocket = 0;
    int32 lOption = 0;
    struct sockaddr_in stServerAddr = {0};

    nClientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (ERROR_CODE != nClientSocket) 
    {
        lOption = 1;
        setsockopt(nClientSocket, SOL_SOCKET, SO_REUSEADDR, 
                    &lOption, sizeof(lOption));
        stServerAddr.sin_family = AF_INET;
        stServerAddr.sin_addr.s_addr = inet_addr(LOCAL_HOST); 
        stServerAddr.sin_port = htons(PORT);

        if (ERROR_CODE != connect(nClientSocket,
                                 (struct sockaddr*)&stServerAddr, 
                                 sizeof(stServerAddr))) 
        {
            if (true == cgiReceiveAndParse(nClientSocket))
            {
                blCheck = true;
            }
        }
        else 
        {
            fprintf(stderr, "Connection failed\n");
        }

        close(nClientSocket);
    }

    if(true != blCheck)
    {
        fprintf(stderr, "Server side Failure\n");
    }

    return blCheck;
}

//*****************************.cgiReceiveAndParse.*****************************
// Purpose : Function to receive and parse Browser request
// Inputs  : nClientSocket - Client socket file descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool cgiReceiveAndParse(int16 nClientSocket)
{
    bool blCheck = false;
    bool blFlag = false;
    uint16 unLength = 0;
    int16 nBytesReceived = 0;
    int8 cFlag = 0;
    int8 cPayload[BUFFER_SIZE] = {0};
    int8 cPayloadBody[PAYLOAD_SIZE] = {0};
    int8 cResponseBuffer[BUFFER_SIZE] = {0};
    int8 cFileName[FILENAME_SIZE] = {0};
    int8 *pcMethod = NULL;
    int8 *pcQueryString = NULL;
    int8 *pcContentType = NULL;
    int8 *pcFileName = NULL;
    int8 *pcContentLength = NULL;

    pcMethod = getenv("REQUEST_METHOD");
    pcQueryString = getenv("QUERY_STRING");

    if (NULL != pcMethod)
    {
        if (0 == strcmp(pcMethod, GET))
        {
            if ((NULL != pcQueryString) && (1 < strlen(pcQueryString)))
            {
                pcFileName = strstr(pcQueryString, FILE_NAME);

                if (NULL != pcFileName)
                {
                    pcFileName += FILE_OFFSET;

                    if ((NULL == strchr(pcFileName, AMPERSAND)) &&
                        NULL == strchr(pcFileName, BACKSLASH))
                    {
                        strcpy(cFileName, pcFileName);
                        snprintf(cPayload, sizeof(cPayload), "%s", 
                                pcMethod);
                        blFlag = true;
                    }
                }
            }
            else
            {
                fprintf(stderr, "File name not given\n");
            }
        }
        else if ((0 == strcmp(pcMethod, POST)) || (0 == strcmp(pcMethod, PUT)))
        {
            pcContentType = getenv("CONTENT_TYPE");

            if (NULL != pcContentType)
            {
                if (NULL != strstr(pcContentType, TYPE_JSON))
                {
                    pcContentLength = getenv("CONTENT_LENGTH");

                    if (NULL != pcContentLength)
                    {
                        unLength = atoi(pcContentLength);

                        if ((0 < unLength) && (BUFFER_SIZE - 1 > unLength))
                        {
                            fread(cPayloadBody, 1, unLength, stdin);
                            cPayloadBody[unLength] = NULL_CHAR;
                            snprintf(cPayload, sizeof(cPayload), "%s|%s", 
                            pcMethod, cPayloadBody);
                            cFlag = 1;
                            blFlag = true;
                        }
                        else
                        {
                            fprintf(stderr, "Invalid content Length\n");
                        }
                    }
                }
            }
        }
        else
        {
            fprintf(stderr, "Invalid request\n");
        }

        if (true == blFlag)
        {
            if (0 < strlen(cPayload))
            {
                send(nClientSocket, cPayload, strlen(cPayload), 0);

                nBytesReceived = recv(nClientSocket, 
                    cResponseBuffer, BUFFER_SIZE - 1, 0);

                if (0 < nBytesReceived)
                {
                    cgiSendToWeb(cResponseBuffer, cFileName, cFlag);
                    blCheck = true;
                }
            }
        }
    }

    if (true != blCheck)
    {
        fprintf(stderr, "Failed to receive and parse request\n");
    }

    return blCheck;
}

//******************************.cgiSendToWeb.**********************************
// Purpose : Function to send response back to web server
// Inputs  : pcResponseBuffer - Response given by backend
//           pcFileName - File name send by the browser
//           cFlag - Flag to distinguish GET,POST and PUT request
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool cgiSendToWeb(int8* pcResponseBuffer, int8* pcFileName, int8 cFlag)
{
    bool blCheck = false;
    uint16 unFileSize = 0;
    int16 nBytesRead = 0;
    int8 cReadBuffer[BUFFER_SIZE] = {0};
    FILE *pstFile = NULL;

    if ((NULL != pcResponseBuffer) && (NULL != pcFileName))
    {
        if (0 == cFlag)
        {
            pstFile = fopen(pcFileName, WRITE_MODE);

            if (NULL != pstFile)
            {
                fputs(pcResponseBuffer, pstFile);
                fclose(pstFile);
            }

            pstFile = fopen(pcFileName, READ_MODE);

            if (NULL != pstFile)
            {
                fseek(pstFile, 0, SEEK_END);
                unFileSize = ftell(pstFile);
                fseek(pstFile, 0, SEEK_SET);

                printf("Content-Type: application/octet-stream\r\n");
                printf("Content-Disposition: attachment; filename=\"%s\"\r\n", 
                        pcFileName);
                printf("Content-Length: %d\r\n", unFileSize);
                printf("Connection: close\r\n\r\n");

                while (0 < (nBytesRead = fread(cReadBuffer, 1, 
                    sizeof(cReadBuffer), pstFile)))
                {
                    fwrite(cReadBuffer, 1, nBytesRead, stdout);
                }

                fflush(stdout);
                fclose(pstFile);
                blCheck = true;
            }
        }
        else
        {
            if (0 == strcmp(pcResponseBuffer, SUCCESS_CASE))
            {
                printf("Content-Type: text/plain\r\n\r\n");
                printf("Connection was successful\n");
                blCheck = true;
            }
        }
    }

    if (true != blCheck)
    {
        fprintf(stderr, "Failed to send response to web");
    }

    return blCheck;
}

// EOF
