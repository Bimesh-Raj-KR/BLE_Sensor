//******************************* App Functions ********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : appFunctions.c
// Summary : Functions for counting open files
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 24/Nov/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "appFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************
static bool appCheck(int8 *pcBuffer, uint16 unLength, int8 *pcAppName, 
                uint16 unSize);

//********************************.appCount.************************************
// Purpose : Function to count all files opened by the application
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
bool appCount(void)
{
    bool blCheck = false;
    uint16 unCount = 0;
    int8 cAppName[MAX_APP_SIZE] = APP_NAME;
    int8 cBuffer[MAX_BUFFER_SIZE] = {0};
    FILE *pstFile = NULL;

    pstFile = fopen(LIST_FILE_NAME, READ_COMMAND);

    if (NULL != pstFile)
    {
        while (NULL != fgets(cBuffer, sizeof(cBuffer), pstFile)) 
        {
            if (true == appCheck(cBuffer, strlen(cBuffer), cAppName, 
                strlen(cAppName)))
            {
                unCount ++;
            }
        }

        fclose(pstFile);
        openlog(LOG_NAME, LOG_PID|LOG_CONS, LOG_USER);
        syslog(LOG_INFO, "Open file count of application %s = %d\n", 
                cAppName, unCount);
        closelog();
        printf("Open file count of application %s = %d\n", cAppName, unCount);
        blCheck = true;
    }
    else
    {
        printf("All open files cannot be counted\n");
    }

    return blCheck;
}

//********************************.appCheck.************************************
// Purpose : Function to check the list for given application name
// Inputs  : pcBuffer - A single line from the open files list produced by lsof
//           unLength - Length of the buffer
//           pcAppName - The application name
//           unSize - Size of the application name
// Outputs : None
// Return  : true if there are no errors, else false
// Notes   : None
//******************************************************************************
static bool appCheck(int8 *pcBuffer, uint16 unLength, int8 *pcAppName, 
                uint16 unSize)
{
    bool blCheck = false;
    uint8 ucIterator = 0;
    int8 cSubBuffer[MAX_APP_SIZE] = {0};

    if ((NULL != pcBuffer) && (NULL != pcAppName))
    {
        for(ucIterator = 0; ucIterator < unLength - 1 - unSize; 
            ucIterator ++)
        {
            memcpy(cSubBuffer, &pcBuffer[ucIterator], unSize);

            if ((0 == strncmp(cSubBuffer, pcAppName, unSize)) && 
                (((PATH == pcBuffer[ucIterator - 1]) || (0 == ucIterator)) 
                && ((SPACE == pcBuffer[ucIterator + unSize]) || 
                (TAB == pcBuffer[ucIterator + unSize]))))
            {
                blCheck = true;
            }
        }
    }

    return blCheck;
}

