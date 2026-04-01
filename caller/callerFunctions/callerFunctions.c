//***************************** Caller Functions *******************************
// Copyright (c) 2026 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : callerFunctions.c
// Summary : Handles the SIP headless caller functionality and WAV transfer
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 01/Apr/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "callerFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static volatile int32 slQuitFlag = 0;
static int32 slPlaybackFinished = 0;

//***************************** Local Functions ********************************
static void callerOnSignal(int lSignal);
static bool callerWriteWavHeader(FILE *pstFile, uint32 ulDataLen);
static void callerOnPlayerEof(LinphonePlayer *pstPlayer, void *pvUserData);
static void callerOnCallState(LinphoneCore *pstLc, LinphoneCall *pstCall, 
                              LinphoneCallState eState, const char *pcMsg);

//******************************.callerOnSignal.********************************
// Purpose : Signal handler to trigger a clean exit
// Inputs  : lSignal - The received signal code
// Outputs : None
// Return  : None
// Notes   : Signature intentionally matches standard OS signal callback
//******************************************************************************
static void callerOnSignal(int lSignal) 
{
    (void)lSignal;
    slQuitFlag = 1;
}

//***************************.callerWriteWavHeader.*****************************
// Purpose : Writes a PCMU WAV header to the specified file pointer
// Inputs  : pstFile - File pointer to write to
//           ulDataLen - The total bytes of audio data
// Outputs : None
// Return  : true if written successfully, false otherwise
// Notes   : None
//******************************************************************************
static bool callerWriteWavHeader(FILE *pstFile, uint32 ulDataLen) 
{
    bool blCheck = false;
    stWavHeader stHdr = {0};

    if (NULL != pstFile) 
    {
        memcpy(stHdr.cRiff, "RIFF", 4);
        stHdr.ulOverallSize = ulDataLen + OVERALL_SIZE_OFFSET;
        memcpy(stHdr.cWave, "WAVE", 4);
        memcpy(stHdr.cFmtChunkMarker, "fmt ", 4);
        stHdr.ulLengthOfFmt = HEADER_FMT_LEN;
        stHdr.usFormatType = FORMAT_PCMU; 
        stHdr.usChannels = MONO_CHANNEL;
        stHdr.ulSampleRate = SAMPLE_RATE;
        stHdr.ulByteRate = SAMPLE_RATE;
        stHdr.usBlockAlign = BLOCK_ALIGN;
        stHdr.usBitsPerSample = BITS_PER_SAMPLE;
        memcpy(stHdr.cDataChunkHeader, "data", 4);
        stHdr.ulDataSize = ulDataLen;

        fseek(pstFile, 0, SEEK_SET);

        if (1 == fwrite(&stHdr, sizeof(stHdr), 1, pstFile))
        {
            blCheck = true;
        }
    }

    return blCheck;
}

//****************************.callerOnPlayerEof.*******************************
// Purpose : Callback to know when the WAV file is done sending
// Inputs  : pstPlayer - Linphone player instance
//           pvUserData - Callback user data (ignored)
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
static void callerOnPlayerEof(LinphonePlayer *pstPlayer, void *pvUserData) 
{
    LinphoneCall *pstCall = NULL;
    LinphoneInfoMessage *pstInfo = NULL;

    (void)pvUserData;

    if (NULL != pstPlayer)
    {
        pstCall = (LinphoneCall *)linphone_player_get_user_data(pstPlayer);

        if (NULL != pstCall) 
        {
            pstInfo = linphone_core_create_info_message(
                          linphone_call_get_core(pstCall));

            if (NULL != pstInfo)
            {
                linphone_info_message_add_header(pstInfo, HEADER_X_TRANSFER, 
                                                 HEADER_X_VAL_TRUE);
                linphone_call_send_info_message(pstCall, pstInfo);
                linphone_info_message_unref(pstInfo);
                linphone_call_start_recording(pstCall);

                printf("[CALLER] File sent completely. "
                       "Now listening for Echo...\n");
                slPlaybackFinished = 1;
            }
        } 
        else 
        {
            printf("[CALLER] ERROR: no call in player user_data\n");
        }
    }
}

//****************************.callerOnCallState.*******************************
// Purpose : Callback to process call state changes
// Inputs  : pstLc - The linphone core instance
//           pstCall - The call instance
//           eState - The updated call state
//           pcMsg - Informational message string
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
static void callerOnCallState(LinphoneCore *pstLc, LinphoneCall *pstCall, 
                              LinphoneCallState eState, const char *pcMsg) 
{
    LinphonePlayer *pstPlayer = NULL;
    AudioStream *pstAs = NULL;
    RtpSession *pstRtp = NULL;
    LinphonePlayerCbs *pstCbs = NULL;

    (void)pstLc;
    (void)pcMsg;

    if (LinphoneCallStreamsRunning == eState) 
    {
        printf("[CALLER] Connected! Starting transmission and recording...\n");
        pstPlayer = linphone_call_get_player(pstCall);

        if (NULL != pstPlayer)
        {
            linphone_player_set_user_data(pstPlayer, pstCall);
            pstAs = (AudioStream *)linphone_call_get_stream(pstCall, 0);

            if (NULL != pstAs) 
            {
                pstRtp = audio_stream_get_rtp_session(pstAs);

                if (NULL != pstRtp)
                {
                    rtp_session_enable_jitter_buffer(pstRtp, FALSE);
                    rtp_session_set_recv_buf_size(pstRtp, RECV_BUFFER_SIZE); 
                }
            }

            pstCbs = linphone_factory_create_player_cbs(linphone_factory_get());

            if (NULL != pstCbs)
            {
                linphone_player_cbs_set_eof_reached(pstCbs, callerOnPlayerEof);
                linphone_player_add_callbacks(pstPlayer, pstCbs);
                linphone_player_cbs_unref(pstCbs);
            }

            linphone_player_open(pstPlayer, FILE_AUDIO_CLEAN);
            linphone_player_start(pstPlayer);
        }
    } 
    else if ((LinphoneCallEnd == eState) || (LinphoneCallError == eState)) 
    {
        slQuitFlag = 1;
    }
}

//***************************.callerEntry.**************************************
// Purpose : Main entry point for the caller application
// Inputs  : argc - Argument count
//           argv - Argument vector strings
// Outputs : None
// Return  : true if no errors, false otherwise
// Notes   : None
//******************************************************************************
bool callerEntry(int argc, char *argv[])
{
    bool blCheck = false;
    int32 lRetVal = SUCCESS_CODE;
    int32 lIndex = 0;
    static int32 slDebugCounter = 0;
    FILE *pstFile = NULL;
    const char *pcDest = NULL;
    LinphoneFactory *pstFactory = NULL;
    LinphoneCore *pstLc = NULL;
    LinphoneTransports *pstTrans = NULL;
    const bctbx_list_t *pstCodecs = NULL;
    const bctbx_list_t *pstIt = NULL;
    LinphonePayloadType *pstPt = NULL;
    LinphoneCoreCbs *pstCbs = NULL;
    LinphoneAddress *pstAddr = NULL;
    LinphoneCallParams *pstParams = NULL;
    LinphoneCall *pstCurCall = NULL;

    signal(SIGINT, callerOnSignal);
    pstFile = fopen(FILE_ECHO_RCV, WRITE_BINARY);

    if (NULL != pstFile) 
    {
        callerWriteWavHeader(pstFile, 0);
        fclose(pstFile);
    }

    if (2 <= argc) 
    {
        pcDest = argv[1];
    }
    else
    {
        pcDest = DEFAULT_DESTINATION;
    }

    pstFactory = linphone_factory_get();
    pstLc = linphone_factory_create_core(pstFactory, NULL, NULL, NULL);

    if (NULL != pstLc)
    {
        ortp_set_log_level_mask(NULL, ORTP_FATAL);
        linphone_core_enable_video_capture(pstLc, FALSE);
        linphone_core_enable_video_display(pstLc, FALSE);
        linphone_core_set_use_files(pstLc, TRUE); 
        pstTrans = linphone_factory_create_transports(pstFactory);

        if (NULL != pstTrans)
        {
            linphone_transports_set_udp_port(pstTrans, UDP_PORT); 
            linphone_core_set_transports(pstLc, pstTrans);
            linphone_transports_unref(pstTrans);
        }

        pstCodecs = linphone_core_get_audio_payload_types(pstLc);

        for (pstIt = pstCodecs; NULL != pstIt; pstIt = pstIt->next) 
        {
            pstPt = (LinphonePayloadType *)pstIt->data;
            linphone_payload_type_enable(pstPt,
                (0 == strcasecmp(linphone_payload_type_get_mime_type(pstPt), 
                                 MIME_TYPE_PCMU)));
        }

        pstCbs = linphone_factory_create_core_cbs(pstFactory);

        if (NULL != pstCbs)
        {
            linphone_core_cbs_set_call_state_changed(pstCbs, callerOnCallState);
            linphone_core_add_callbacks(pstLc, pstCbs);
            linphone_core_cbs_unref(pstCbs);
        }

        linphone_core_set_playback_device(pstLc, NULL);
        linphone_core_set_capture_device(pstLc, NULL);
        linphone_core_set_play_file(pstLc, FILE_SILENCE);
        linphone_core_set_record_file(pstLc, FILE_ECHO_RCV);
        linphone_core_enable_echo_cancellation(pstLc, FALSE);
        linphone_core_enable_echo_limiter(pstLc, FALSE);
        linphone_core_enable_agc(pstLc, FALSE);
        linphone_core_start(pstLc);

        pstAddr = linphone_core_interpret_url(pstLc, pcDest);
        pstParams = linphone_core_create_call_params(pstLc, NULL);

        if ((NULL != pstAddr) && (NULL != pstParams))
        {
            linphone_call_params_enable_audio(pstParams, TRUE);
            linphone_call_params_set_record_file(pstParams, FILE_ECHO_RCV);

            printf("[CALLER] Dialing %s...\n", pcDest);
            linphone_core_invite_address_with_params(pstLc, pstAddr, pstParams);
        }

        if (NULL != pstParams)
        {
            linphone_call_params_unref(pstParams);
        }

        if (NULL != pstAddr)
        {
            linphone_address_unref(pstAddr);
        }

        while (0 == slQuitFlag) 
        {
            linphone_core_iterate(pstLc);
            slDebugCounter ++;

            if (0 == (slDebugCounter % DEBUG_PRINT_MODULO)) 
            { 
                pstCurCall = linphone_core_get_current_call(pstLc);

                if (NULL != pstCurCall) 
                {
                    printf("[CALLER] Call active, is_recording=%d\n", 
                           linphone_call_is_recording(pstCurCall));
                }
            }

            usleep(ITERATION_DELAY_US);
        }

        pstCurCall = linphone_core_get_current_call(pstLc);

        if (NULL != pstCurCall) 
        {
            linphone_call_terminate(pstCurCall);
        }

        for (lIndex = 0; lIndex < CLEANUP_ITERATIONS; lIndex ++) 
        { 
            linphone_core_iterate(pstLc); 
            usleep(ITERATION_DELAY_US); 
        }

        linphone_core_unref(pstLc);
        printf("[CALLER] Clean exit. Received file is 'echo_received.wav'\n");
    }
    else
    {
        lRetVal = ERROR_CODE;
        printf("[CALLER] ERROR: Failed to create Linphone Core.\n");
    }

    if (SUCCESS_CODE == lRetVal) 
    {
        blCheck = true;
    }

    return blCheck;
}

// EOF