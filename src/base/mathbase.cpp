/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/mathbase.cpp
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

#include "mathbase.h"
#include "Calculator.h"
#include "datelocation.h"
#include "logger.h"
#include "Planet.h"

#include <locale.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <format>

//#define DEBUG_MATHBASE

/**************************************************************
***
**     currentTimestamp
***
***************************************************************/
long currentTimestamp()
{
  time_t tt;
  time( &tt ); 
	return (long)tt;
}

/**************************************************************
***
**     jdToTime
***
***************************************************************/
std::time_t jdToTime( const double &jd )
{
	/*
	return 0;
	jd = 2440587.5 + (double)tt / 86400.0;
	jd - 2440587.5 = (double)tt / 86400.0;
	tt = ( jd - 2440587.5 ) * 86400.0;
	*/
	return ( jd - 2440587.5 ) * 86400.0;
}

/**************************************************************
***
**     jdToTime
***
***************************************************************/
double timeToJD( const std::time_t &tt )
{
	return( 2440587.5 + (double)tt / 86400.0 );
}

/*****************************************************
**
**   getIntOrEmtpy
**
******************************************************/
wxString getIntOrEmtpy( const int &a )
{
	if ( a ) return  std::format( "{}", a );
	else return "";
}

/**************************************************************
***
**  convert wxString to int. return true if successful
***
***************************************************************/
bool stringToInt( wxString s, int &a )
{
	 long l;
	 const bool b = s.ToLong( &l );
	 if ( b )
	 {
		 a = (int)l;
	 }
	 return b;
}

/**************************************************************
***
**  convert wxString to double.
**  The method is portable because it accepts both '.' and ',' as delimiters
**
**  return true if successful
***
***************************************************************/
bool stringToDouble( wxString s, double &a )
{
	// avoid problems with locale specifi delimiter, e.g. is a file is saved and opened again in another locale
	s.Replace( ",", "." );
	return s.ToCDouble( &a );
}

/**************************************************************
***
**  resolveCamelNotation
***
***************************************************************/
wxString resolveCamelNotation( wxString s )
{
	wxString r;
	for( size_t i = 0; i < s.Length(); i++ )
	{
		if ( i < s.Length() - 2
			&& isupper( s.GetChar( i ))
			&& islower( s.GetChar( i + 1 ))
		)
		{
			r << " ";
		}
		r << s.GetChar( i );
	}
	return r;
}

/**************************************************************
***
**  prints a house number in Roman format
***
***************************************************************/
const wxString getHouseNumberFormatted( const House &house, const HouseNumberFormat style )
{
	assert_valid_enum<House>( house );
	const static char* rnumber[12] =
	{
		"I", "II", "III",
		"IV", "V", "VI",
		"VII", "VIII", "IX",
		"X", "XI", "XII"
	};
	switch( style )
	{
		case HouseNumberFormat::None:
		case HouseNumberFormat::Nothing:
			return "";
		break;
		case HouseNumberFormat::Roman:
			return rnumber[(int)house];
		break;
		case HouseNumberFormat::Arabic:
		default:
			return std::format( "{}", (int)house + 1 );
		break;
	}
}
	
/**************************************************************
***
**  Reduces a given double value x modulo the double a (should be positive).
**  The return value is between 0 and a.
***
***************************************************************/
double a_red( const double &x, const double &a )
{
	//assert( a > 0 );
	return( x - floor( x / a ) * a );
}

/**************************************************************
***
**
***
***************************************************************/
double d_red( const double &x, const int &division )
{
	assert( division > 0 );
	return a_red( x, 360.0 / division );
}

/**************************************************************
***
**   Reduces a given double value modulo 360.
**   The return value is between 0 and 360.
***
***************************************************************/
double red_deg( const double &input )
{
	return a_red( input, 360 );
}

/**************************************************************
***
**  Reduces a given integer modulo 12.
**  The return value is between 0 and 11.
***
***************************************************************/
int red12( const int &input )
{
	int ret = input %12;
	return ret >= 0 ? ret : ret + 12;
}
int red_sign( const Sign &r1, const Sign &r2 ) { return red12( (int)r1 - (int)r2 ); }

/**************************************************************
***
**  Reduces a given integer modulo 27.
**  The return value is between 0 and 26.
***
***************************************************************/
int red27( const int &input )
{
	int ret = input % 27;
	return ret >= 0 ? ret : ret + 27;
}

/**************************************************************
***
**     round a double
***
***************************************************************/
int a_rund( const double &v )
{
	return v >= 0 ? (int)(v+.5) : (int)(v-.5);
}

/**************************************************************
***
**
***
***************************************************************/
double planetDistance( const double &len1, const double &len2 )
{
	double d = red_deg( fabs( len2 - len1 ));
	if ( d > 180 ) return( 360 - d );
	return d;
}

/**************************************************************
***
**     point between two objects
***
***************************************************************/
double getMidpoint( const double &len1, const double &len2 )
{
	if ( len1 == len2 ) return len1;

	double midpoint =  red_deg( .5 * ( len1 + len2 ));
	if ( planetDistance( len1, midpoint ) > 90 ) midpoint = red_deg( midpoint + 180 );
	return midpoint;
}

/**************************************************************
***
 * Returns the Greenwich mean time for a given julian date.
 * the return value is between 0 and 24. There's no formating like [hhmmss], etc.
***
***************************************************************/
double getTimeFromJD( const double &jd )
{
	double jjd = jd - .5;
	return a_red( ( jjd - (double)(int)jjd ) * 24, 24 );
}

/*****************************************************
**
**   year for a given jd
**
******************************************************/
int getYearForJD( const double &jd )
{
	const auto[ year, month, day, ut ] = CalculatorFactory().getCalculator()->getDateIntsFromJD( jd );
	return year;
}

/*****************************************************
**
**   current year
**
******************************************************/
int getCurrentYear()
{
	return getYearForJD( MDate::getCurrentJD());
}

/*****************************************************
**
**   number of days in a given month
**
******************************************************/
int getMonthLength( const int &year, const int &month )
{
	switch ( month )
	{
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	break;
	case 2: // Feb
		if ( year % 4 ) return 28;
		else
		{
			int nb_days = 29;
			if ( year % 100 == 0 ) nb_days = 28; // 1900, 1800 etc. have 28 days
			if ( year % 400 == 0 ) nb_days = 29; // BUT Feb 2000 has 29 days
			return nb_days;
		}
	break;
	default:
		return 31;
	break;
	}
}

/*****************************************************
**
**   number of days in a given year
**
******************************************************/
int getNumberDaysInYear( const int &year )
{
	if ( year % 4 ) return 365;
	if ( year % 400 == 0 ) return 366; // Feb 2000, 1600 etc. have 29 days
	if ( year % 100 == 0 ) return 365; // 1900, 1800 etc. have 28 days
	return 366;
}

/*****************************************************
**
**   weekday index for a given jd
**
******************************************************/
int getWeekDay( const double &jd )
{
	return (int)(jd + 1.5 ) % 7;
}

/*****************************************************
**
**   isLongitudeBetween
**
******************************************************/
bool isLongitudeBetween( const double &longitude, const double &l1, const double &l2 )
{
#ifdef DEBUG_MATHBASE
	debuglog << "isLongitudeBetween longitude " << longitude << " l1 " << l1 << " l2 " << l2;
#endif

	assert( longitude >= 0 && longitude < 360 );
	assert( l1 >= 0 && l1 < 360 );
	assert( l2 >= 0 && l2 < 360 );

	if ( l1 <= l2 )
	{
		return l1 <= longitude && longitude < l2;
	}
	// else
	return l1 <= longitude ||  longitude < l2;
}

/*****************************************************
**
**   trimTextFormat
**
******************************************************/
void trimTextFormat( TextFormat &tf, const TextFormat &maxTextFormat )
{
  switch( tf )
  {
    case TextFormat::Long:
      tf = maxTextFormat;
    break;

    case TextFormat::Medium:
      if ( maxTextFormat != TextFormat::Short && maxTextFormat != TextFormat::Symbol ) tf = TextFormat::Medium;
    break;

    case TextFormat::Short:
    case TextFormat::Symbol:
    default:
    break;
  }
}

/**************************************************************
***
**     StopWatch   ---   Constructor
***
***************************************************************/
StopWatch::StopWatch()
{
	starttime = std::chrono::system_clock::now();
}

/**************************************************************
***
**     StopWatch   ---   getElapsed methods
***
***************************************************************/
int StopWatch::getElapsedMillisec() const
{
	const std::chrono::system_clock::time_point endtime = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>( endtime - starttime ).count();
}
int StopWatch::getElapsedMicrosec() const
{
	const std::chrono::system_clock::time_point endtime = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>( endtime - starttime ).count();
}
int StopWatch::getElapsedNanosec() const
{
	const std::chrono::system_clock::time_point endtime = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::nanoseconds>( endtime - starttime ).count();
}



