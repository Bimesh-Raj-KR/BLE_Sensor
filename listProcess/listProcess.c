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

// EOF
