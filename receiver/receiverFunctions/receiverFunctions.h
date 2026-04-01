//*************************** Receiver Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for receiverFunctions.c functions
// Note    : None
// 
//******************************************************************************
#ifndef RECEIVER_FUNCTIONS_H
#define RECEIVER_FUNCTIONS_H

//******************************* Include Files ********************************
#include <linphone/core.h>
#include <mediastreamer2/mediastream.h>
#include <ortp/ortp.h>
#include <ortp/rtp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

//******************************* Global Types *********************************
typedef int32_t int32;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef char int8;

typedef enum 
{ 
    PHASE_RECEIVING = 0, 
    PHASE_SENDING, 
    PHASE_DONE 
} ePhase;

typedef struct _AudioStream AudioStream;

typedef struct _stWavHeader 
{
    int8   cRiff[4];             // RIFF
    uint32 ulOverallSize;        // file_size - 8
    int8   cWave[4];             // WAVE
    int8   cFmtChunkMarker[4];   // fmt
    uint32 ulLengthOfFmt;        // 16 for PCM
    uint16 usFormatType;         // 7 for Mu-Law (PCMU)
    uint16 usChannels;           // 1
    uint32 ulSampleRate;         // 8000
    uint32 ulByteRate;           // 8000
    uint16 usBlockAlign;         // 1
    uint16 usBitsPerSample;      // 8
    int8   cDataChunkHeader[4];  // data
    uint32 ulDataSize;           // total bytes of audio
} stWavHeader;

//***************************** Global Constants *******************************
#define ONE_BILLION_NS          (1000000000L)
#define ONE_MILLION_NS          (1000L)
#define PACKET_INTERVAL_US      (20000)
#define MAIN_LOOP_DELAY_US      (20000)
#define SAMPLE_RATE             (8000)
#define UDP_PORT                (5070)
#define SLEEP_INTERVAL_US       (5000)
#define SAMPLES_PER_PACKET      (160)
#define PRINT_MODULO            (50)
#define OVERALL_SIZE_OFFSET     (36)
#define HEADER_FMT_LEN          (16)
#define RTP_FIXED_HEADER_SIZE   (12)
#define BITS_PER_SAMPLE         (8)
#define FORMAT_PCMU             (7)
#define CHUNK_ID_LEN            (4)
#define MONO_CHANNEL            (1)
#define BLOCK_ALIGN             (1)
#define PCMU_PAYLOAD_TYPE       (0)
#define SUCCESS_CODE            (0)
#define ERROR_CODE              (-1)
#define PORT_DISABLED           (-1)
#define FILE_RECEIVED           "received_audio.wav"
#define FILE_DEV_NULL           "/dev/null"
#define WRITE_BINARY            "w+b"
#define READ_BINARY             "rb"
#define MIME_TYPE_PCMU          "PCMU"
#define HEADER_X_TRANSFER       "X-Transfer-Complete"
#define HEADER_X_VAL_TRUE       "1"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool receiverEntry(void);
extern void *linphone_call_get_stream(const void *call, int type);

//*********************** Inline Method Implementations ************************ 
#endif // RECEIVER_FUNCTIONS_H
// EOF