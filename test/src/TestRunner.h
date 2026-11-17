/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       test/src/TestRunner.h
 Release    9.0

 This is free and unencumbered software released into the public domain.

 Anyone is free to copy, modify, publish, use, compile, sell, or
 distribute this software, either in source code form or as a compiled
 binary, for any purpose, commercial or non-commercial, and by any
 means.

 In jurisdictions that recognize copyright laws, the author or authors
 of this software dedicate any and all copyright interest in the
 software to the public domain. We make this dedication for the benefit
 of the public at large and to the detriment of our heirs and
 successors. We intend this dedication to be an overt act of
 relinquishment in perpetuity of all present and future rights to this
 software under copyright law.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

 For more information, please refer to <http://unlicense.org/>

************************************************************************/

#ifndef _TESTRUNNER_H_
#define _TESTRUNNER_H_

#include "gtest/gtest.h"

enum class Sign;

#define TEST1_MTXFILE "../resources/data/Test_1.mtx"
#define TEST2_MTXFILE "../resources/data/Test_2.mtx"
#define AVTEST_MTXFILE "../resources/data/AV_Test.mtx"
#define CHUGH1_MTXFILE "../resources/data/Chugh1_Test.mtx"
#define CHUGH2_MTXFILE "../resources/data/Chugh2_Test.mtx"
#define CHUGH3_MTXFILE "../resources/data/Chugh3_Test.mtx"
#define CHUGH_SOLAR1_MTXFILE "../resources/data/Chugh_Solar1_Test.mtx"
#define WITTE1_MTXFILE "../resources/data/Wilhelm2.mtx"

#define DEG_TOL 1 / 60.0
#define SEC_TOL 1 / 3600.0

/**
* Generate double value from sign/deg/min/sec
*/
// vedic sign format
double gen_longitude( const int &sign, const int &deg, const int &min, const int sec = 0 );

double gen_longitude( const Sign &sign, const int &deg, const int &min, const int sec = 0 );

//inline double gen_longitude( const Sign &sign, const int &deg, const int &min, const int sec = 0 )
	//{ return gen_longitude( (int)sign, deg, min ); }


double gen_degree( const int &deg, const int &min, const int sec = 0 );

// negative degrees. required for values like -0:30
double neg_degree( const int &deg, const int &min, const int sec = 0 );

double gen_date( const int &year, const int &month, const int &day, const int hour = 0, const int minute = 0, const int second = 0 );

/**
* Fixture for single threaded tests. Required for ephem test because ephem config would damage other parallel tests
*/
class SingleThreadedTests : public ::testing::Test
{
protected:
 virtual void SetUp() override;

 virtual void TearDown() override;
};


#endif // _TESTRUNNER_H_
