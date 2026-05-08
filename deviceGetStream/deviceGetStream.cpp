//***************************** Device Get Stream ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : deviceGetStream.cpp
// Summary : get the stream uri from the device inorder to start streaming
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 16/Mar/2025
//******************************************************************************

//****************************** Include Files *********************************
#include "deviceGetStream.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static int8 scStreamUri[ONVIF_URI_LEN] = {0};

//***************************** Local Functions ********************************
static bool printProfiles(ONVIF_PROFILE *pstFirstProfile);
static bool deviceFilterProfile(PROFILE_DETAILS* pstDetails, 
                                ONVIF_DEVICE* pstDevice);
static bool deviceGetStream(ONVIF_DEVICE *pstDevice);
static bool deviceInputValidity(int8* pcResolution);

//*********************************.getUri.*************************************
// Purpose : Function to give Uri to videoStreamer.
// Inputs  : pcUri - Pointer to store uri
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool getUri(int8* pcUri)
{
    bool blCheck = false;

    if (NULL != pcUri)
    {
        strncpy(pcUri, scStreamUri, ONVIF_URI_LEN);
        blCheck = true;
    }

    return blCheck;
}

//*****************************.deviceStreamInit.*******************************
// Purpose : Function to initialize all getStream functions to get Profiles and 
//           Get stream uri of a particular profile
// Inputs  : None
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
bool deviceStreamInit(void)
{
    bool blCheck = false;
    ONVIF_DEVICE stDevice = {0};

    printf("\n>>>LISTING PROBED DEVICES<<<\n\n");
    discoverPrint();
    printf("\n>>CHECKING FOR OUR PARTICULAR DEVICE<<\n\n");
    sleep(1);

    if (true == discoverCheck((int8*)DEVICE_IP))
    {
        sys_buf_init(BUFFER_SIZE);
        http_msg_buf_init(MSG_SIZE);
        sprintf(stDevice.binfo.XAddr.host, DEVICE_IP);
        stDevice.binfo.XAddr.https = FALSE;
        stDevice.binfo.XAddr.port = PORT;
        stDevice.timeout = TIMEOUT;

        if (TRUE == GetCapabilities(&stDevice))
        {
            printf("GETTING PROFILE......");
            sleep(1);

            if (TRUE != GetProfiles(&stDevice))
            {
                printf("\n>>>>>>>FAILED AUTHENTICATION<<<<<<<<\n");
                printf(">>>ENTERING USERNAME AND PASSWORD<<<\n\n");
                strcpy(stDevice.username, USERNAME);
                strcpy(stDevice.password, PASSWORD);

                if (TRUE == GetProfiles(&stDevice))
                {
                    deviceGetStream(&stDevice);
                    blCheck = true;
                }
            }
            else
            {
                deviceGetStream(&stDevice);
                blCheck = true;
            }

            if (NULL != stDevice.profiles)
            {
                onvif_free_profiles(&stDevice.profiles);
                stDevice.profiles = NULL;
            }
        }
    }

    return blCheck;
}

//*****************************.printProfiles.**********************************
// Purpose : Function to print all the device profiles
// Inputs  : pstFirstProfile - Pointer to profile linked list
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool printProfiles(ONVIF_PROFILE *pstFirstProfile) 
{
    bool blCheck = false;
    ONVIF_PROFILE *pstProfiles = NULL;
    uint16 unCount = 0;

    if (NULL != pstFirstProfile)
    {
        unCount = 1;
        pstProfiles = pstFirstProfile;

        while (NULL != pstProfiles) 
        {
            printf("\n================ [ PROFILE %d ] ================\n", 
                unCount++);
            printf("Name  : %s\n", pstProfiles->name);
            printf("Token : %s\n", pstProfiles->token);
            printf("Fixed : %s\n", pstProfiles->fixed ? "Yes" : "No");

            // --- VIDEO SOURCE (The Physical Lens) ---
            if (NULL != pstProfiles->v_src_cfg) 
            {
                printf("  [Video Source]\n");
                printf("    - Name/Token: %s / %s\n", pstProfiles->v_src_cfg->
                Configuration.Name, pstProfiles->v_src_cfg->
                Configuration.token);
                printf("    - Capturing Area: %dx%d at x:%d, y:%d\n", 
                        pstProfiles->v_src_cfg->Configuration.Bounds.width, 
                        pstProfiles->v_src_cfg->Configuration.Bounds.height,
                        pstProfiles->v_src_cfg->Configuration.Bounds.x, 
                        pstProfiles->v_src_cfg->Configuration.Bounds.y);
            }

            // --- VIDEO ENCODER (The Stream Settings) ---
            if (NULL != pstProfiles->v_enc_cfg) 
            {
                printf("  [Video Encoder]\n");
                printf("    - Encoding: ");

                switch (pstProfiles->v_enc_cfg->Configuration.Encoding)
                {
                    case VideoEncoding_Unknown:
                        printf("Unknown\n");
                        break;

                    case VideoEncoding_JPEG:
                        printf("JPEG\n");
                        break;

                    case VideoEncoding_MPEG4:
                        printf("MPEG4\n");
                        break;

                    case VideoEncoding_H264:
                        printf("H264\n");
                        break;

                    default:
                        // No other conditions
                        break;
                }

                printf("    - Resolution: %dx%d\n", pstProfiles->v_enc_cfg->
                    Configuration.Resolution.Width, pstProfiles->v_enc_cfg->
                    Configuration.Resolution.Height);
                
                if (1 == pstProfiles->v_enc_cfg->
                    Configuration.RateControlFlag) 
                {
                    printf("    - FPS Limit: %d | Bitrate: %d kbps\n", 
                    pstProfiles->v_enc_cfg->Configuration.RateControl.
                    FrameRateLimit, pstProfiles->v_enc_cfg->Configuration.
                    RateControl.BitrateLimit);
                }
            }

            // --- PTZ CONFIGURATION (Pan/Tilt/Zoom) ---
            if (NULL != pstProfiles->ptz_cfg) 
            {
                printf("  [PTZ Support]\n");
                printf("    - Config Token: %s\n", pstProfiles->ptz_cfg->
                    Configuration.token);
                printf("    - Node Token  : %s\n", pstProfiles->ptz_cfg->
                    Configuration.NodeToken);

                if (1 == pstProfiles->ptz_cfg->Configuration.
                        PanTiltLimitsFlag) 
                {
                    printf("    - Pan Range : Min %.1f to Max %.1f\n", 
                    pstProfiles->ptz_cfg->Configuration.PanTiltLimits.
                    XRange.Min, pstProfiles->ptz_cfg->Configuration.
                    PanTiltLimits.XRange.Max);
                }
            }

            // --- AUDIO SOURCE/ENCODER ---
            if ((NULL != pstProfiles->a_src_cfg) || 
                (NULL != pstProfiles->a_enc_cfg)) 
            {
                printf("  [Audio Support]\n");

                if (NULL != pstProfiles->a_enc_cfg) 
                {
                    printf("    - Format: %d | Sample Rate: %d kHz\n", 
                            pstProfiles->a_enc_cfg->Configuration.Encoding, 
                            pstProfiles->a_enc_cfg->Configuration.SampleRate);
                }
            }

            // Move to the next node in your linked list
            pstProfiles = pstProfiles->next; 
        }

        printf("\n==============================================\n");
        blCheck = true;
    }

    return blCheck;
}

//*****************************.deviceFilterProfile.****************************
// Purpose : Function to filter out the user selected profile.
// Inputs  : pstDetails - Profile details
//           pstDevice - Struct containing all details about the camera
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool deviceFilterProfile(PROFILE_DETAILS* pstDetails, 
                ONVIF_DEVICE* pstDevice)
{
    bool blCheck = false;
    ONVIF_PROFILE *pstFound = NULL;
    ONVIF_PROFILE *pstCurrent = NULL;
    ONVIF_PROFILE *pstPrevious = NULL;
    uint32 ulWidth = 0;
    uint32 ulHeight = 0;
    
    if ((NULL != pstDevice) && (NULL != pstDevice->profiles) && 
        (NULL != pstDetails))
    {
        sscanf(pstDetails->cResolution, "%dx%d", &ulWidth, &ulHeight);
        pstCurrent = pstDevice->profiles;

        while (NULL != pstCurrent)
        {
            if ((pstCurrent->v_enc_cfg->Configuration.Resolution.Width == 
                ulWidth) && (pstCurrent->v_enc_cfg->Configuration.Resolution.
                Height == ulHeight) && (pstCurrent->v_enc_cfg->Configuration.
                Encoding == pstDetails->eEncoding))
            {
                pstFound = pstCurrent;

                if (NULL != pstPrevious)
                {
                    pstPrevious->next = pstCurrent->next;
                }
                else
                {
                    pstDevice->profiles = pstCurrent->next;
                }

                pstFound->next = NULL;
                break;
            }

            pstPrevious = pstCurrent;
            pstCurrent = pstCurrent->next;
        }

        if (NULL != pstFound)
        {
            onvif_free_profiles(&pstDevice->profiles);
            pstDevice->profiles = NULL;
            pstDevice->profiles = pstFound;
            blCheck = true;
        }
        else
        {
            printf("Incorrect Inputs\n");
        }
    }
    else
    {
        printf("Failed Null check\n");
    }

    return blCheck;
}

//*****************************.deviceGetStream.********************************
// Purpose : Function to get stream uri
// Inputs  : pstDevice - Struct containing all details about the camera
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool deviceGetStream(ONVIF_DEVICE *pstDevice)
{
    bool blCheck = false;
    int8 cEncoding = 0;
    int8 cProfileToken[TOKEN_SIZE] = {0};
    PROFILE_DETAILS stDetails = {0};

    if (NULL != pstDevice)
    {
        printProfiles(pstDevice->profiles);
        printf("\n>>>>>>>ENTER RESOLUTION<<<<<<<<\n");
        printf(">>>>>>>FOLLOW FORMAT __x__<<<<<<<\n");
        scanf("%s", stDetails.cResolution);
        while('\n' != getchar());

        if (true == deviceInputValidity(stDetails.cResolution))
        {
            printf("\n>>>>>>>ENTER ENCODING<<<<<<<<\n");
            printf("|   Unknown             -1  |\n");
            printf("|   JPEG                 0  |\n");
            printf("|   MPEG4                1  |\n");
            printf("|   H264                 2  |\n");
            printf("=============================\n");
            scanf("%hhd", &cEncoding);
            stDetails.eEncoding = (onvif_VideoEncoding)cEncoding;

            if ((VideoEncoding_Unknown <= stDetails.eEncoding) && 
                (VideoEncoding_H264 >= stDetails.eEncoding))
            {
                if (true == deviceFilterProfile(&stDetails, pstDevice))
                {
                    if (TRUE == GetStreamUris(pstDevice, 
                            TransportProtocol_RTSP))
                    {
                        printf("\n>>>>>>THE STREAM URI OF THE PROFILE<<<<<<\n");
                        printf("\t\n %s\n\n", pstDevice->profiles->stream_uri);
                        strncpy(scStreamUri, pstDevice->profiles->stream_uri,
                            ONVIF_URI_LEN);
                        blCheck = true;
                    }
                }
            }
        }

        if (true != blCheck)
        {
            printf("Invalid Inputs\n"); 
        }
    }

    return blCheck;
}

//*****************************.deviceInputValidity.****************************
// Purpose : Function to validate the profile details(resolution) given by user
// Inputs  : pcResolution - Pointer to the resolution string
// Outputs : None
// Return  : true if there are no errors and false if any errors exist
//           during function execution
// Notes   : None
//******************************************************************************
static bool deviceInputValidity(int8* pcResolution)
{
    bool blCheck = false;
    int8* pcLocate = NULL;
    int8* pcReverseLocate = NULL;
    int8 cIndex = 0;

    if (NULL != pcResolution)
    {
        pcLocate = strchr(pcResolution, 'x');
        pcReverseLocate = strrchr(pcResolution, 'x');

        if ((NULL != pcLocate) && (NULL != pcReverseLocate) && 
            (pcReverseLocate == pcLocate))
        {
            if ((pcResolution != pcLocate) && ('\0' != *(pcLocate + 1)))
            {
                for (cIndex = 0;  cIndex < strlen(pcResolution); cIndex ++)
                {
                    if ('x' != pcResolution[cIndex])
                    {
                        if (0 != isdigit(pcResolution[cIndex]))
                        {
                            blCheck = true;
                        }
                        else
                        {
                            blCheck = false;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (true != blCheck)
    {
        printf("Resolution given in invalid format\n");
    }

    return blCheck;
}

// EOF