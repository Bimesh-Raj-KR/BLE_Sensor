//***************************** Client Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : clientFunctions.c
// Summary : Setups client to send file and memory range to server
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 14/Jan/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "clientFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************
static bool clientInputProcess(int32 lSocket);
static bool clientFileSend(int8 *pcFilePath, int32 lSocket);
static bool clientAddressSend(uint32 ulStartAddr, uint32 ulEndAddr, 
                                int32 lSocket);

//*******************************.clientSetup.**********************************
// Purpose : Function to setup client.
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool clientSetup(void)
{
    bool blCheck = false;
    struct sockaddr_in stServerAddr = {0};
    int32 lSocket = 0;
    uint32 ulEnable = 0;

    lSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (ERROR_CODE != lSocket) 
    {
        ulEnable = 1;
        setsockopt(lSocket, IPPROTO_TCP, TCP_NODELAY, &ulEnable, 
                    sizeof(ulEnable));
        stServerAddr.sin_family = AF_INET;
        stServerAddr.sin_addr.s_addr = inet_addr(HOST);
        stServerAddr.sin_port = htons(PORT);
        
        if (ERROR_CODE != connect(lSocket, (struct sockaddr*)&stServerAddr, 
                            sizeof(stServerAddr))) 
        {
            while (1)
            {
                if (true == clientInputProcess(lSocket)) 
                {
                    blCheck = true;
                }
                else
                {
                    break;
                }
            }
        }

        close(lSocket); 
    }  

    if (true != blCheck) 
    {
        printf("Client side failed\n");
    }

    return blCheck;
}

//**************************.clientInputProcess.********************************
// Purpose : Function to distinguish and process command line inputs
// Inputs  : lSocket - Socket descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool clientInputProcess(int32 lSocket)
{
    bool blCheck = false;
    int8 cChoice = 0;
    int8 cFilePath[PATH_SIZE] = {0};
    uint32 ulStartAddr = 0;
    uint32 ulEndAddr = 0;

    printf("|*******WHAT TO DO*******|\n");
    printf("|  F      -    Send File |\n");
    printf("|  M      -  Send Memory |\n");
    printf("|  E      -         Exit |\n");
    printf("|________________________|\n");
    scanf(" %c", &cChoice);
    printf("\033[6A\033[J");

    if ('F' == cChoice)
    {
        printf("Enter full file path: ");
        scanf(" %255[^\n]", cFilePath);

        if (ERROR_CODE != send(lSocket, FILE_FLAG, sizeof(FILE_FLAG), 0))
        {
            if (true == clientFileSend(cFilePath, lSocket))
            {
                blCheck = true;
            }
        } 

        printf("\033[2A\033[J");
    }
    else if ('M' == cChoice)
    {
        printf("Enter Starting Address: ");
        scanf("%x", &ulStartAddr);
        printf("Enter Ending Address: ");
        scanf("%x", &ulEndAddr);

        if (ERROR_CODE != send(lSocket, ADDRESS_FLAG, 
            sizeof(ADDRESS_FLAG), 0))
        {
            if (true == clientAddressSend(ulStartAddr, 
                                    ulEndAddr, lSocket))
            {
                blCheck = true;
            }
        }

        printf("\033[2A\033[J");
    }
    else if ('E' == cChoice)
    {
        blCheck = false;
    }
    else
    {
        // countinue
    }

    if (true != blCheck)
    {
        printf("Closing connection.....\n");
    }

    return blCheck;
}

//****************************.clientFileSend.**********************************
// Purpose : Function to send a file to the server
// Inputs  : pcFilePath - Path to the File
//           lSocket - Socket descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool clientFileSend(int8 *pcFilePath, int32 lSocket)
{
    bool blCheck = false;
    FILE *pstFile = {0};
    int8 cBuffer[BUFFER_SIZE] = {0};
    uint32 ulBytesRead = 0;
    uint32 ulFileSize = 0;
    uint32 ulNetSize = 0;

    pstFile = fopen(pcFilePath, READ_BINARY);

    if ((NULL != pstFile) && (NULL != pcFilePath)) 
    {
        fseek(pstFile, 0, SEEK_END);
        ulFileSize = ftell(pstFile);
        fseek(pstFile, 0, SEEK_SET);
        ulNetSize = htonl(ulFileSize);

        if (ERROR_CODE != send(lSocket, &ulNetSize, sizeof(ulNetSize), 0)) 
        {
            while (0 <  (ulBytesRead = fread(cBuffer, 1, 
                                        BUFFER_SIZE, pstFile))) 
            {
                if (ERROR_CODE != send(lSocket, cBuffer, ulBytesRead, 0)) 
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

    if (true != blCheck) 
    {
        printf("File transfer failed\n");
        printf("Closing connection...\n");
    }

    return blCheck;
}

//****************************.clientAddressSend.*******************************
// Purpose : Function to send memory range to the server
// Inputs  : pArgVector - Pointer to the file name argument
//           lSocket - Socket descriptor
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool clientAddressSend(uint32 ulStartAddr, uint32 ulEndAddr, 
                                int32 lSocket)
{
    bool blCheck = false;
    uint32 ulFormatAddr = 0;

    ulFormatAddr = htonl(ulStartAddr);
    usleep(MIN_DELAY);

    if (ERROR_CODE != send(lSocket, &ulFormatAddr, sizeof(ulFormatAddr), 0)) 
    {
        ulFormatAddr = htonl(ulEndAddr);
        usleep(MIN_DELAY);

        if (ERROR_CODE != send(lSocket, &ulFormatAddr, sizeof(ulFormatAddr), 0)) 
        {
            blCheck = true;
        }   
    }  
    
    if (true != blCheck)
    {
        printf("Address transfer failed\n");
        printf("Closing connection...\n");
    }

    return blCheck;
}

// EOF
