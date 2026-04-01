//*************************** Receiver Functions *******************************
// Copyright (c) 2026 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : receiverFunctions.c
// Summary : Handles RTP reception, WAV saving, and echo transmission
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 01/Apr/2026
//******************************************************************************

//****************************** Include Files *********************************
#include "receiverFunctions.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************
static volatile int32 slQuitFlag = 0;
static ePhase sePhase = PHASE_RECEIVING;
static RtpSession *spstRtp = NULL;
static LinphoneCall *spstCall = NULL;
static FILE *spstRecF = NULL;
static FILE *spstPlayF = NULL;
static time_t stLastRecvTime = 0;
static uint16 susLastSeq = 0xFFFF;
static int32 slSeqInitialized = 0;
static volatile int32 slTransferComplete = 0;
static uint32 sulCallerSsrc = 0;
static int32 slSsrcInitialized = 0;
static uint32 sulLastRecvTs = 0;
static uint32 sulCurrentTs = 0;
static uint16 susGlobalSeq = 0;
static struct timespec sstNextSendTime = {0};

//***************************** Local Functions ********************************
static void receiverOnSignal(int lSignal);
static bool receiverFindWavDataOffset(FILE *pstFile, int32 *plOffset);
static void receiverOnInfoRcvd(LinphoneCore *pstLc, LinphoneCall *pstCall, 
                               const LinphoneInfoMessage *pstMsg);
static bool receiverWriteWavHeader(FILE *pstFile, uint32 ulDataLen);
static void receiverProcessRtpToFileDirect(void);
static bool receiverProcessFileToRtpDirect(void);
static void receiverOnCallState(LinphoneCore *pstLc, LinphoneCall *pstCall, 
                                LinphoneCallState eState, const char *pcMsg);

//*****************************.receiverOnSignal.*******************************
// Purpose : Signal handler to trigger a clean exit
// Inputs  : lSignal - The received signal code
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
static void receiverOnSignal(int lSignal) 
{
    (void)lSignal;
    slQuitFlag = 1;
}

//************************.receiverFindWavDataOffset.***************************
// Purpose : Locates the "data" chunk offset within a WAV file
// Inputs  : pstFile - The open WAV file
//           plOffset - Pointer to store the result
// Outputs : plOffset is updated with the byte offset
// Return  : true if found, false otherwise
// Notes   : None
//******************************************************************************
static bool receiverFindWavDataOffset(FILE *pstFile, int32 *plOffset) 
{
    bool blCheck = false;
    bool blFound = false;
    bool blEof = false;
    int8 cChunkId[CHUNK_ID_LEN] = {0};
    uint32 ulChunkSize = 0;

    if ((NULL != pstFile) && (NULL != plOffset)) 
    {
        fseek(pstFile, RTP_FIXED_HEADER_SIZE, SEEK_SET); 

        while ((false == blFound) && (false == blEof)) 
        {
            if ((CHUNK_ID_LEN == fread(cChunkId, 1, CHUNK_ID_LEN, pstFile)) &&
                (1 == fread(&ulChunkSize, 4, 1, pstFile))) 
            {
                if (0 == memcmp(cChunkId, "data", CHUNK_ID_LEN)) 
                {
                    *plOffset = ftell(pstFile); 
                    blFound = true;
                    blCheck = true;
                } 
                else 
                {
                    fseek(pstFile, ulChunkSize, SEEK_CUR);
                }
            }
            else
            {
                blEof = true;
            }
        }
    }

    return blCheck;
}

//***************************.receiverOnInfoRcvd.*******************************
// Purpose : Callback to process SIP INFO messages (EOF signaling)
// Inputs  : pstLc - The linphone core instance
//           pstCall - The call instance
//           pstMsg - The received info message
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
static void receiverOnInfoRcvd(LinphoneCore *pstLc, LinphoneCall *pstCall, 
                               const LinphoneInfoMessage *pstMsg) 
{
    const char *pcHdr = NULL;

    (void)pstLc; 
    (void)pstCall;

    if (NULL != pstMsg)
    {
        pcHdr = linphone_info_message_get_header(pstMsg, HEADER_X_TRANSFER);

        if ((NULL != pcHdr) && (0 == strcmp(pcHdr, HEADER_X_VAL_TRUE))) 
        {
            printf("[RECEIVER] Got transfer complete signal.\n");
            slTransferComplete = 1;
        }
    }
}

//*************************.receiverWriteWavHeader.*****************************
// Purpose : Writes a PCMU WAV header to the specified file pointer
// Inputs  : pstFile - File pointer to write to
//           ulDataLen - The total bytes of audio data
// Outputs : None
// Return  : true if written successfully, false otherwise
// Notes   : None
//******************************************************************************
static bool receiverWriteWavHeader(FILE *pstFile, uint32 ulDataLen) 
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

//*********************.receiverProcessRtpToFileDirect.*************************
// Purpose : Processes incoming RTP packets directly into a file
// Inputs  : None
// Outputs : None
// Return  : None
// Notes   : Modifies global RTP queue and file pointer directly
//******************************************************************************
static void receiverProcessRtpToFileDirect(void) 
{
    mblk_t *pstPkt = NULL;
    mblk_t *pstNextPkt = NULL;
    rtp_header_t *pstRtpIn = NULL;
    uint32 ulIncomingTs = 0;
    uint32 ulSsrc = 0;
    uint16 usSeq = 0;
    uint8 *pucPayload = NULL;
    uint16 usExtLen = 0;
    int32 lLen = 0;

    if ((NULL != spstRtp) && (NULL != spstRecF)) 
    {
        pstPkt = qfirst(&spstRtp->rtp.rq);    

        while ((NULL != pstPkt) && (pstPkt != (mblk_t*)&spstRtp->rtp.rq)) 
        {
            pstNextPkt = pstPkt->b_next;
            pstRtpIn = (rtp_header_t*)pstPkt->b_rptr;

            if ((NULL == pstRtpIn) || 
                (pstPkt->b_wptr < pstPkt->b_rptr + RTP_FIXED_HEADER_SIZE) || 
                (pstPkt->b_rptr + sizeof(rtp_header_t) > pstPkt->b_wptr)) 
            {
                remq(&spstRtp->rtp.rq, pstPkt);
                freemsg(pstPkt);
                pstPkt = pstNextPkt;
                continue;
            }

            ulIncomingTs = ntohl(pstRtpIn->timestamp);
            sulLastRecvTs = ulIncomingTs;

            ulSsrc = ntohl(pstRtpIn->ssrc);
            usSeq = ntohs(pstRtpIn->seq_number);

            if (0 == slSsrcInitialized) 
            {
                sulCallerSsrc = ulSsrc;
                slSsrcInitialized = 1;
                printf("[RTP] Locked onto SSRC=0x%08X\n", sulCallerSsrc);
            }
            else if (ulSsrc != sulCallerSsrc) 
            {
                printf("[RTP] Rejecting SSRC=0x%08X (expected 0x%08X)\n", 
                       ulSsrc, sulCallerSsrc);
            }

            if (ulSsrc != sulCallerSsrc) 
            {
                remq(&spstRtp->rtp.rq, pstPkt);
                freemsg(pstPkt);
                pstPkt = pstNextPkt;
                continue;  
            }

            if ((1 == slSeqInitialized) && (usSeq == susLastSeq)) 
            {
                remq(&spstRtp->rtp.rq, pstPkt);
                freemsg(pstPkt);
                pstPkt = pstNextPkt;
                continue;
            }

            susLastSeq = usSeq;
            slSeqInitialized = 1;

            if (PCMU_PAYLOAD_TYPE == pstRtpIn->paytype) 
            {
                pucPayload = (uint8 *)pstRtpIn + RTP_FIXED_HEADER_SIZE + 
                             (pstRtpIn->cc * 4);
                
                if (pstRtpIn->extbit) 
                {
                    usExtLen = ntohs(((uint16_t *)pucPayload)[1]);
                    pucPayload += 4 + (usExtLen * 4);
                }
                
                lLen = (int32)(pstPkt->b_wptr - pucPayload);
                
                if (0 < lLen) 
                {
                    fwrite(pucPayload, 1, lLen, spstRecF);
                    stLastRecvTime = time(NULL); 
                }
            }

            remq(&spstRtp->rtp.rq, pstPkt);
            freemsg(pstPkt);
            pstPkt = pstNextPkt;
        }

        fflush(spstRecF);
    }
}

//*********************.receiverProcessFileToRtpDirect.*************************
// Purpose : Transmits file contents as RTP packets
// Inputs  : None
// Outputs : None
// Return  : true if there is still data to send, false on EOF
// Notes   : None
//******************************************************************************
static bool receiverProcessFileToRtpDirect(void) 
{
    bool blCheck = false;
    static int32 slPacketCount = 0;
    struct timespec stNow = {0};
    long lDiffNs = 0;
    struct timespec stSleepTs = {0};
    uint8 ucBuffer[SAMPLES_PER_PACKET] = {0};
    int32 lSize = 0;
    mblk_t *pstM = NULL;
    rtp_header_t *pstRtp = NULL;

    if ((NULL != spstRtp) && (NULL != spstPlayF)) 
    {
        slPacketCount ++;

        if (0 == (slPacketCount % PRINT_MODULO))
        {
            printf("[RECEIVER] Sent %d packets so far\n", slPacketCount);
        }

        clock_gettime(CLOCK_MONOTONIC, &stNow);

        lDiffNs = (sstNextSendTime.tv_sec - stNow.tv_sec) * ONE_BILLION_NS + 
                  (sstNextSendTime.tv_nsec - stNow.tv_nsec);

        if (0 < lDiffNs) 
        {
            stSleepTs.tv_sec = lDiffNs / ONE_BILLION_NS;
            stSleepTs.tv_nsec = lDiffNs % ONE_BILLION_NS;
            nanosleep(&stSleepTs, NULL);
        }

        sstNextSendTime.tv_nsec += PACKET_INTERVAL_US * ONE_MILLION_NS;

        if (ONE_BILLION_NS <= sstNextSendTime.tv_nsec) 
        {
            sstNextSendTime.tv_sec ++; 
            sstNextSendTime.tv_nsec -= ONE_BILLION_NS;
        }

        lSize = fread(ucBuffer, 1, SAMPLES_PER_PACKET, spstPlayF);

        if (0 < lSize) 
        {
            blCheck = true; 
            pstM = allocb(lSize + RTP_FIXED_HEADER_SIZE, 0);

            if (NULL != pstM) 
            {
                pstM->b_rptr = pstM->b_wptr;
                pstRtp = (rtp_header_t *)pstM->b_rptr;

                if ((NULL != pstRtp) && (pstM->b_wptr + RTP_FIXED_HEADER_SIZE + 
                    lSize <= pstM->b_datap->db_lim)) 
                {
                    memset(pstRtp, 0, RTP_FIXED_HEADER_SIZE);
                    pstRtp->version = 2;
                    pstRtp->paytype = PCMU_PAYLOAD_TYPE;
                    pstRtp->ssrc = htonl(rtp_session_get_send_ssrc(spstRtp));
                    pstRtp->markbit = (0 == sulCurrentTs) ? 1 : 0;
                    pstRtp->timestamp = htonl(sulCurrentTs);  
                    pstRtp->seq_number = htons(susGlobalSeq ++);
                    pstM->b_wptr += RTP_FIXED_HEADER_SIZE;
                    memcpy(pstM->b_wptr, ucBuffer, lSize);
                    pstM->b_wptr += lSize;
                    rtp_session_sendm_with_ts(spstRtp, pstM, sulCurrentTs);
                    sulCurrentTs += lSize;
                }
                else
                {
                    freemsg(pstM);
                }
            }
        }
    }

    return blCheck;
}

//***************************.receiverOnCallState.******************************
// Purpose : Callback to process call state changes
// Inputs  : pstLc - The linphone core instance
//           pstCall - The call instance
//           eState - The updated call state
//           pcMsg - Informational message string
// Outputs : None
// Return  : None
// Notes   : None
//******************************************************************************
static void receiverOnCallState(LinphoneCore *pstLc, LinphoneCall *pstCall, 
                                LinphoneCallState eState, const char *pcMsg) 
{
    (void)pcMsg;

    if (LinphoneCallIncomingReceived == eState) 
    {
        linphone_core_accept_call(pstLc, pstCall);
    } 
    else if (LinphoneCallStreamsRunning == eState) 
    {
        spstCall = pstCall;
        spstRecF = fopen(FILE_RECEIVED, WRITE_BINARY);
        receiverWriteWavHeader(spstRecF, 0);
    } 
    else if ((LinphoneCallEnd == eState) || (LinphoneCallError == eState)) 
    {
        slQuitFlag = 1;
    }
}

//**********************************.main.**************************************
// Purpose : Main entry point for the receiver application
// Inputs  : None
// Outputs : None
// Return  : SUCCESS_CODE (0) if clean exit, ERROR_CODE (-1) on failure
// Notes   : None
//******************************************************************************
bool receiverEntry(void) 
{
    bool blCheck = false;
    int32 lRetVal = SUCCESS_CODE;
    LinphoneFactory *pstFactory = NULL;
    LinphoneCore *pstLc = NULL;
    LinphoneTransports *pstTrans = NULL;
    const bctbx_list_t *pstCodecs = NULL;
    const bctbx_list_t *pstIt = NULL;
    LinphonePayloadType *pstPt = NULL;
    LinphoneCoreCbs *pstCbs = NULL;
    AudioStream *pstAs = NULL;
    long lDataLen = 0;
    int32 lDataOffset = 0;

    signal(SIGINT, receiverOnSignal);
    remove(FILE_RECEIVED);
    pstFactory = linphone_factory_get();
    pstLc = linphone_factory_create_core(pstFactory, NULL, NULL, NULL);

    if (NULL != pstLc)
    {
        ortp_set_log_level_mask(NULL, ORTP_FATAL);
        pstTrans = linphone_factory_create_transports(pstFactory);

        if (NULL != pstTrans)
        {
            linphone_transports_set_udp_port(pstTrans, UDP_PORT);
            linphone_transports_set_tcp_port(pstTrans, PORT_DISABLED); 
            linphone_transports_set_tls_port(pstTrans, PORT_DISABLED);
            linphone_transports_set_dtls_port(pstTrans, PORT_DISABLED);
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
            linphone_core_cbs_set_call_state_changed(pstCbs, 
                                                     receiverOnCallState);
            linphone_core_cbs_set_info_received(pstCbs, receiverOnInfoRcvd);
            linphone_core_add_callbacks(pstLc, pstCbs);
            linphone_core_cbs_unref(pstCbs);
        }

        linphone_core_set_playback_device(pstLc, NULL);
        linphone_core_set_capture_device(pstLc, NULL);
        linphone_core_set_use_files(pstLc, TRUE);
        linphone_core_set_record_file(pstLc, FILE_DEV_NULL);
        linphone_core_start(pstLc);
        printf("[RECEIVER] Listening on port %d...\n", UDP_PORT);

        while (0 == slQuitFlag) 
        {
            linphone_core_iterate(pstLc);

            if ((NULL != spstCall) && (NULL == spstRtp)) 
            {
                pstAs = (AudioStream *)linphone_call_get_stream(spstCall, 0);

                if (NULL != pstAs) 
                {
                    spstRtp = audio_stream_get_rtp_session(pstAs);
                    rtp_session_enable_jitter_buffer(spstRtp, TRUE);
                    rtp_session_set_scheduling_mode(spstRtp, 0);
                    rtp_session_set_blocking_mode(spstRtp, 0);
                    rtp_session_enable_rtcp(spstRtp, FALSE);
                    linphone_call_set_microphone_muted(spstCall, TRUE);

                    printf("[RECEIVER] RTP Session hijacked. "
                           "Awaiting data...\n");

                    stLastRecvTime = time(NULL);
                    susLastSeq = 0xFFFF;
                    slSeqInitialized = 0;
                    sulCallerSsrc = 0;
                    slSsrcInitialized = 0;
                }
            }

            if (NULL != spstRtp) 
            {
                if (PHASE_RECEIVING == sePhase) 
                {
                    receiverProcessRtpToFileDirect();

                    if ((1 == slTransferComplete) && (0 < ftell(spstRecF))) 
                    {
                        slTransferComplete = 0; 
                        printf("[RECEIVER] Silence detected. "
                               "Switching to ECHO phase.\n");

                        lDataLen = ftell(spstRecF) - sizeof(stWavHeader);
                        receiverWriteWavHeader(spstRecF, (uint32)lDataLen);

                        fclose(spstRecF); 
                        spstRecF = NULL;

                        spstPlayF = fopen(FILE_RECEIVED, READ_BINARY);

                        if (true == receiverFindWavDataOffset(spstPlayF, 
                                                              &lDataOffset)) 
                        {
                            printf("[RECEIVER] WAV data starts at offset %d\n", 
                                   lDataOffset);
                            fseek(spstPlayF, lDataOffset, SEEK_SET);
                        } 
                        else 
                        {
                            printf("[RECEIVER] ERROR: could not find data "
                                   "chunk in wav file\n");
                            slQuitFlag = 1;
                        }

                        susGlobalSeq = (uint16)(random() & 0xFFFF);
                        clock_gettime(CLOCK_MONOTONIC, &sstNextSendTime);
                        
                        printf("[RECEIVER] Switching to PHASE_SENDING, "
                               "play file size=%ld\n", ftell(spstPlayF));

                        sulCurrentTs = sulLastRecvTs + SAMPLES_PER_PACKET;
                        sePhase = PHASE_SENDING;
                    }

                    usleep(SLEEP_INTERVAL_US);
                } 
                else if (PHASE_SENDING == sePhase) 
                {
                    if (true != receiverProcessFileToRtpDirect()) 
                    {
                        printf("[RECEIVER] Echo complete.\n");
                        sePhase = PHASE_DONE;
                        linphone_call_terminate(spstCall);
                    }
                }
            } 
            else 
            {
                usleep(MAIN_LOOP_DELAY_US);
            }
        }

        if (NULL != spstRecF) 
        {
            fclose(spstRecF);
        }

        if (NULL != spstPlayF) 
        {
            fclose(spstPlayF);
        }

        linphone_core_unref(pstLc);
    }
    else
    {
        lRetVal = ERROR_CODE;
        printf("[RECEIVER] ERROR: Failed to create Linphone Core.\n");
    }

    if (SUCCESS_CODE == lRetVal) 
    {
        blCheck = true;
    }

    return blCheck;
}

// EOF