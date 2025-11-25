//******************************* List Process *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : listProcess.c
// Summary : Functions for listing open files and active processes
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 24/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "listProcess.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************
static bool listFiles(void);
static bool listProcess(void);

//********************************.listData.************************************
// Purpose : Function to list all PIDs and open files using lsof command 
//           and copies it to a file
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool listData(void)
{
    bool blCheck = false;
    int8 cBuffer[MAX_BUFFER_SIZE] = {0};
    int8 cCommand[MAX_COMMAND_SIZE] = {0};
    FILE *pstFile = NULL;
    FILE *pstPipe = NULL;

    snprintf(cCommand, sizeof(cCommand), LIST_OPEN_FILES);
    pstPipe = popen(cCommand, READ_COMMAND);

    if (NULL != pstPipe)
    {
        pstFile = fopen(LIST_FILE_NAME, WRITE_COMMAND);

        if (NULL != pstFile)
        {
            while (NULL != fgets(cBuffer, sizeof(cBuffer), pstPipe)) 
            {
                fputs(cBuffer, pstFile);
                blCheck = true;
            }

            fclose(pstFile);
        }
        else
        {
            perror("fopen failed\n");
        }

        pclose(pstPipe);
    } 
    else
    {
        perror("popen failed\n");
    }

    return blCheck;
}

//********************************.listCommand.*********************************
// Purpose : Function for user to choose between listing PIDs or open files
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool listCommand(void)
{
    bool blCheck = false;
    uint8 ucChoice = 0;
    uint8 ucFlag = 0;

    while(1)
    {
        printf("********ENTER********\n0 -------------- EXIT\n1 -------- "
        "LIST FILES\n2 --------- LIST PIDs\n*********************\n");

        if (1 != scanf("%hhd", &ucChoice))
        {
            printf("scanf failed, Exiting code\n");
            ucChoice = 0;
        }

        clearRest();

        switch(ucChoice)
        {
            case 0: 
                ucFlag = 1;
                break;

            case 1:
                listFiles();
                break;

            case 2:
                listProcess();
                break;

            default:
                printf("Invalid Choice\n");
                break;
        }

        if (1 == ucFlag)
        {
            blCheck = true;
            remove(LIST_FILE_NAME);
            break;
        }
    }

    return blCheck;
}

//*********************************.listFiles.**********************************
// Purpose : Function to list all files opened by a particular PID
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
static bool listFiles(void)
{
    bool blCheck = false;
    int8 cProcessId[MAX_PID_SIZE] = {0};
    int8 cCurrentId[MAX_PID_SIZE] = {0};
    int8 cBuffer[MAX_BUFFER_SIZE] = {0};
    FILE *pstFile = NULL;

    printf("Enter process ID ");
    scanf("%9s", cProcessId);
    clearRest();
    checkValidId(cProcessId);
    pstFile = fopen(LIST_FILE_NAME, READ_COMMAND);

    if (NULL != pstFile)
    {
        while (NULL != fgets(cBuffer, sizeof(cBuffer), pstFile)) 
        {
            if (true == checkId(cBuffer, strlen(cBuffer), cCurrentId))
            {
                if (0 == strcmp(cCurrentId, cProcessId))
                {
                    printf("%s", cBuffer);
                }
            }
        }

        fclose(pstFile);
        blCheck = true;
    }
    else
    {
        printf("Files cannot be listed\n");
    }

    return blCheck;
}

//*********************************.listProcess.********************************
// Purpose : Function to list all PIDs that opened a particular file
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
static bool listProcess(void)
{
    bool blCheck = false;
    int8 cFileName[MAX_FILE_SIZE] = {0};
    int8 cCurrentFile[MAX_FILE_SIZE] = {0};
    int8 cBuffer[MAX_BUFFER_SIZE] = {0};
    FILE *pstFile = NULL;

    printf("Enter File name ");

    if (NULL != fgets(cFileName, sizeof(cFileName), stdin))
    {
        cFileName[strcspn(cFileName, LINE_CHECK)] = NULL_CHAR;
    }

    pstFile = fopen(LIST_FILE_NAME, READ_COMMAND);

    if (NULL != pstFile)
    {
        while (NULL != fgets(cBuffer, sizeof(cBuffer), pstFile)) 
        {
            if (true == checkFile(cBuffer, strlen(cBuffer), cCurrentFile))
            {
                if (0 == strcmp(cCurrentFile, cFileName))
                {
                    printf("%s", cBuffer);
                }
            }
        }

        fclose(pstFile);
        blCheck = true;
    }
    else
    {
        printf("All process IDs cannot be listed\n");
    }

    return blCheck;
}

// EOF
