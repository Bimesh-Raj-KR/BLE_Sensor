//****************************** PORT STRUCT TEST ******************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : structTest.cpp
// Summary : Conducts Test of portStruct function
// Note    : None
// Author  : Bimesh Raj K R
// Date    : 03/Feb/2026
//******************************************************************************

//****************************** Include Files *********************************
#include <gtest/gtest.h>
#include "portStruct.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//***************************** Local Variables ********************************

//***************************** Local Functions ********************************

//**************************.Source Null Check Test.****************************
// Purpose : To test portStruct null verification by passing NULL instead of 
//           source struct
// Inputs  : None
// Outputs : None
// Outcome : flase
// Notes   : None
//******************************************************************************
TEST(TG_PORTSTRUCT, TC_NULL_CHECK_SRC)
{
    TO_PORT stDestination = {};

    EXPECT_FALSE(portStruct(NULL, &stDestination));
}

//***********************.Destination Null Check Test.**************************
// Purpose : To test portStruct null verification by passing NULL instead of 
//           destination struct
// Inputs  : None
// Outputs : None
// Outcome : flase
// Notes   : None
//******************************************************************************
TEST(TG_PORTSTRUCT, TC_NULL_CHECK_DEST)
{
    FROM_PORT stSource = {};

    EXPECT_FALSE(portStruct(&stSource, NULL));
}


//****************************.Both Null Check Test.****************************
// Purpose : To test portStruct null verification by passing NULL for both 
//           source and destination structs
// Inputs  : None
// Outputs : None
// Outcome : flase
// Notes   : None
//******************************************************************************
TEST(TG_PORTSTRUCT, TC_NULL_CHECK_BOTH)
{
    EXPECT_FALSE(portStruct(NULL, NULL));
}


//****************************.None Null Check Test.****************************
// Purpose : To test portStruct null verification by passing valid source and 
//           destination struct 
// Inputs  : None
// Outputs : None
// Outcome : true
// Notes   : None
//******************************************************************************
TEST(TG_PORTSTRUCT, TC_NULL_CHECK_NONE)
{
    FROM_PORT stSource = {};
    TO_PORT stDestination = {};

    EXPECT_TRUE(portStruct(&stSource, &stDestination));
}

//*************************.Int Data verification Test.*************************
// Purpose : To test portStruct processing by passing predetermined float value 
//           and checking if desired int value is stored in destination struct
// Inputs  : None
// Outputs : None
// Outcome : equal
// Notes   : None
//******************************************************************************
TEST(TG_PORTSTRUCT, TC_DATA_VERIFY_INT)
{
    FROM_PORT stSource = {};
    TO_PORT stDestination = {};

    stSource = {FLOAT_VERIFY, LONG_VERIFY};
    portStruct(&stSource, &stDestination);

    EXPECT_EQ(INT_VERIFY, stDestination.lInteger);
}

//************************.Short Data verification Test.************************
// Purpose : To test portStruct processing by passing predetermined long value 
//           and checking if desired short value is stored in destination struct
// Inputs  : None
// Outputs : None
// Outcome : equal
// Notes   : None
//******************************************************************************
TEST(TG_PORTSTRUCT, TC_DATA_VERIFY_SHORT)
{
    FROM_PORT stSource = {};
    TO_PORT stDestination = {};

    stSource = {FLOAT_VERIFY, LONG_VERIFY};
    portStruct(&stSource, &stDestination);

    EXPECT_EQ(SHORT_VERIFY, stDestination.nShort);
}

//******************************.Int Overflow Test.*****************************
// Purpose : To test portStruct processing by passing an overflow prone float 
//           value and checking if desired int value is stored in destination
// Inputs  : None
// Outputs : None
// Outcome : equal
// Notes   : None
//******************************************************************************
TEST(TG_PORTSTRUCT, TC_INT_OVERFLOW)
{
    FROM_PORT stSource = {};
    TO_PORT stDestination = {};

    stSource = {FLOAT_OVERFLOW, LONG_OVERFLOW}; 
    portStruct(&stSource, &stDestination);

    EXPECT_EQ(INT_OVERFLOW, stDestination.lInteger);
}

//****************************.Short Overflow Test.*****************************
// Purpose : To test portStruct processing by passing an overflow prone long 
//           value and checking if desired short value is stored in destination
// Inputs  : None
// Outputs : None
// Outcome : equal
// Notes   : None
//******************************************************************************
TEST(TG_PORTSTRUCT, TC_SHORT_OVERFLOW)
{
    FROM_PORT stSource = {};
    TO_PORT stDestination = {};

    stSource = {FLOAT_OVERFLOW, LONG_OVERFLOW}; 
    portStruct(&stSource, &stDestination);

    EXPECT_EQ((int16)LONG_OVERFLOW, stDestination.nShort);
}

//************************.Int Negative value Test.*****************************
// Purpose : To test portStruct processing by passing a negative float value 
//           and checking if desired int value is stored in destination
// Inputs  : None
// Outputs : None
// Outcome : equal
// Notes   : None
//******************************************************************************
TEST(TG_PORTSTRUCT, TC_NEGVAL_INT)
{
    FROM_PORT stSource = {};
    TO_PORT stDestination = {};

    stSource = {FLOAT_NEGATIVE, LONG_NEGATIVE};
    portStruct(&stSource, &stDestination);

    EXPECT_EQ(INT_NEGATIVE, stDestination.lInteger);
}

//************************.Short Negative value Test.***************************
// Purpose : To test portStruct processing by passing a negative long value 
//           and checking if desired short value is stored in destination
// Inputs  : None
// Outputs : None
// Outcome : equal
// Notes   : None
//******************************************************************************
TEST(TG_PORTSTRUCT, TC_NEGVAL_SHORT)
{
    FROM_PORT stSource = {};
    TO_PORT stDestination = {};

    stSource = {FLOAT_NEGATIVE, LONG_NEGATIVE};
    portStruct(&stSource, &stDestination);

    EXPECT_EQ(SHORT_NEGATIVE, stDestination.nShort);
}

//****************************.Int Rounding Test.*******************************
// Purpose : To test portStruct processing by passing a float value with a 
//           decimal part of value more than .5 value and checking if the 
//           integer value is stored after rounding the decimal point
// Inputs  : None
// Outputs : None
// Outcome : equal
// Notes   : None
//******************************************************************************
TEST(TG_PORTSTRUCT, TC_FLOAT_ROUNDING)
{
    FROM_PORT stSource = {};
    TO_PORT stDestination = {};

    stSource = {FLOAT_ROUND, LONG_ROUND};
    portStruct(&stSource, &stDestination);

    EXPECT_EQ(INT_ROUND, stDestination.lInteger);
}

// EOF