/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/mathbase.h
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
#ifndef _MATHBASE_H_
#define _MATHBASE_H_

#include "constants.h"
#include "mstring.h"

#include <chrono>
#include <ctime>
#include <list>
#include <wx/string.h>

/**************************************************************
***
**  Functions for circular periods
***
***************************************************************/
double a_red( const double &x, const double &a ); // mod a
double d_red( const double &x, const int &division );

double red_deg( const double &input );            // mod 360
double red_rad( const double &input );            // mod 2*PI

int red12( const int &input );                    // mod 12 (integer)
int red27( const int &input );                    // mod 27 (integer)
int red_sign( const Sign &r1, const Sign &r2 );
#define redRasi red12
#define redNakshatra red27

// Calculate the distance between 2 points
double planetDistance( const double &len1, const double &len2 );

bool isLongitudeBetween( const double &longitude, const double &l1, const double &l2 );

double getMidpoint( const double &len1, const double &len2 );

/**************************************************************
***
**  Variuos mathematical funtions
***
***************************************************************/
#define ASSERT_VALID_DEGREE( l ) assert( l >= 0 && l < 360 );

bool stringToInt( wxString, int &a );
bool stringToDouble( wxString, double &a );

wxString resolveCamelNotation( wxString );

template<typename E> requires std::is_scoped_enum_v<E> wxString resolveCamelNotation( const E &e )
{
	return resolveCamelNotation( enum_to_string( e ));
}

wxString getIntOrEmtpy( const int &a );

long currentTimestamp();
std::time_t jdToTime( const double& );
double timeToJD( const std::time_t& );

const wxString getHouseNumberFormatted( const House&, const HouseNumberFormat = HouseNumberFormat::Arabic );

// rounds a double to int
int a_rund( const double &v );

// Calculate the time for a given JD (0..24)
double getTimeFromJD( const double &jd );

int getYearForJD( const double& );
int getCurrentYear();
int getMonthLength( const int &year, const int &month );
int getNumberDaysInYear( const int &year );

//   weekday index for a given jd
int getWeekDay( const double &jd );

void trimTextFormat( TextFormat&, const TextFormat &maxTextFormat );

/**************************************************************
***
**     CLASS   ---   StopWatch
***
***************************************************************/
class StopWatch
{
public:
	StopWatch();

	int getElapsedMillisec() const;
	int millisec() const { return getElapsedMillisec(); }

	int getElapsedMicrosec() const;
	int microsec() const { return getElapsedMicrosec(); }
	int getElapsedNanosec() const;
	int nanosec() const { return getElapsedNanosec(); }

protected:

	std::chrono::system_clock::time_point starttime;

};


#endif


