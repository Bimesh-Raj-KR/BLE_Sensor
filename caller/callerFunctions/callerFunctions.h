//***************************** Caller Functions *******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all type definitions, global constants and forward
//           declarations required for callerFunctions.c functions
// Note    : None
// 
//******************************************************************************
#ifndef CALLER_FUNCTIONS_H
#define CALLER_FUNCTIONS_H

//******************************* Include Files ********************************
#include <linphone/core.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//******************************* Global Types *********************************
typedef int32_t int32;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef char int8;

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
#define ITERATION_DELAY_US      (20000)
#define RECV_BUFFER_SIZE        (10240)
#define SAMPLE_RATE             (8000)
#define UDP_PORT                (5080)
#define DEBUG_PRINT_MODULO      (250)
#define OVERALL_SIZE_OFFSET     (36)
#define CLEANUP_ITERATIONS      (30)
#define HEADER_FMT_LEN          (16)
#define BITS_PER_SAMPLE         (8)
#define FORMAT_PCMU             (7)
#define SUCCESS_CODE            (0)
#define MONO_CHANNEL            (1)
#define BLOCK_ALIGN             (1)
#define ERROR_CODE              (-1)
#define DEFAULT_DESTINATION     "sip:127.0.0.1:5070"
#define FILE_ECHO_RCV           "echo_received.wav"
#define FILE_AUDIO_CLEAN        "/mnt/d/linCall/caller/audio_clean.wav"
#define FILE_SILENCE            "silence.wav"
#define WRITE_BINARY            "w+b"
#define MIME_TYPE_PCMU          "PCMU"
#define HEADER_X_TRANSFER       "X-Transfer-Complete"
#define HEADER_X_VAL_TRUE       "1"

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool callerEntry(int argc, char *argv[]);
extern void *linphone_call_get_stream(const void *call, int type);

//*********************** Inline Method Implementations ************************ 
#endif // CALLER_FUNCTIONS_H
// EOF