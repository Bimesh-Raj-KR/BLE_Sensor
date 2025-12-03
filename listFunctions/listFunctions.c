//******************************* List Process *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : listFunctions.c
// Summary : Functions for listing open files and active processes
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 24/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "listFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

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

//*********************************.listFiles.**********************************
// Purpose : Function to list all files opened by a particular PID
// Inputs  : pcProcessId - Given Process ID
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool listFiles(int8 *pcProcessId)
{
    bool blCheck = false;
    int8 cCurrentId[MAX_PID_SIZE] = {0};
    int8 cBuffer[MAX_BUFFER_SIZE] = {0};
    FILE *pstFile = NULL;

    if (NULL != pcProcessId)
    {
       if (true == checkValidId(pcProcessId)) 
       {
            pstFile = fopen(LIST_FILE_NAME, READ_COMMAND);

            if (NULL != pstFile)
            {
                while (NULL != fgets(cBuffer, sizeof(cBuffer), pstFile)) 
                {
                    if (true == checkId(cBuffer, strlen(cBuffer), cCurrentId))
                    {
                        if (0 == strcmp(cCurrentId, pcProcessId))
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
        }
    }
    else
    {
        printf("Invalid PID\n");
    }

    return blCheck;
}

//*********************************.listProcess.********************************
// Purpose : Function to list all PIDs that opened a particular file
// Inputs  : pcFileName - Given file name
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool listProcess(int8 *pcFileName)
{
    bool blCheck = false;
    int8 cCurrentFile[MAX_FILE_SIZE] = {0};
    int8 cBuffer[MAX_BUFFER_SIZE] = {0};
    FILE *pstFile = NULL;

    if (NULL != pcFileName)
    {
        pstFile = fopen(LIST_FILE_NAME, READ_COMMAND);

        if (NULL != pstFile)
        {
            while (NULL != fgets(cBuffer, sizeof(cBuffer), pstFile)) 
            {
                if (true == checkFile(cBuffer, strlen(cBuffer), cCurrentFile))
                {
                    if (0 == strcmp(cCurrentFile, pcFileName))
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
    }
    else
    {
        printf("Invalid FileName\n");
    }

    return blCheck;
}

// EOF
