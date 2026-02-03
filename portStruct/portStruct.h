//***************************** Port Structure *********************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//******************************************************************************
//
// Summary : Contain all typedefinitions, global constants and forward 
//           declarations required for portStruct.c functions
// Note    : None
// 
//******************************************************************************
#ifndef PORT_STRUCT_H
#define PORT_STRUCT_H

//******************************* Include Files ********************************
#include <stdio.h>
#include <stdbool.h>

//******************************* Global Types *********************************
typedef short int16;
typedef int int32;
typedef long int64;

typedef struct _FROM_PORT_
{
    float fFloat;
    int64 lLong;
}
FROM_PORT;

typedef struct _TO_PORT_
{
    int32 lInteger;
    int16 nShort;
}
TO_PORT;

//***************************** Global Constants *******************************
// All long constants
#define LONG_OVERFLOW           (40000L)
#define LONG_VERIFY             (5678L)
#define LONG_ROUND              (0L)
#define LONG_NEGATIVE           (-100L)

// All float constants
#define FLOAT_OVERFLOW          (3000000000.0f)
#define FLOAT_ROUND             (99.99f)
#define FLOAT_VERIFY            (12.34f)
#define FLOAT_NEGATIVE          (-10.5f)

// All short constants
#define SHORT_VERIFY            (5678)
#define SHORT_NEGATIVE          (-100)

// All int constants
#define INT_ROUND               (99)
#define INT_VERIFY              (12)
#define INT_NEGATIVE            (-10)
#define INT_OVERFLOW            (-2147483648)

//***************************** Global Variables *******************************

//**************************** Forward Declarations ****************************
bool portStruct(FROM_PORT* pstSource, TO_PORT* pstDestination);

//*********************** Inline Method Implementations ************************ 

#endif // PORT_STRUCT_H
// EOF 