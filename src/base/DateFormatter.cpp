/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/DateFormatter.cpp
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

#include "DateFormatter.h"

#include "astroconfig.h"
#include "Calculator.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "logger.h"
#include "mathbase.h"

#include <wx/datetime.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
//#include <wx/uilocale.h>

#include <format>
#include <iomanip>

extern Astroconfig *astroconfig;

//#define DEBUG_DATEFORMATTER

/*****************************************************
**
**   DateFormatter   ---   formatDateInternal
**
******************************************************/
wxString DateFormatter::formatDateInternal( const double &jd, const DateFormat &dateFormat )
{
	wxString result;
#if 1
	if ( dateFormat != DateFormat::ISO )
	{
		warnlog << "DateFormatter::formatDateInternal dateformat " << dateFormat << " not supported";
	}
  const auto[ year, month, day, ut ] = CalculatorFactory().getCalculator()->getDateIntsFromJD( jd );
	result = std::format( "{:04d}-{:02d}-{:02d}", year, month, day );

#else
	const wxDateTime dt( jd );
	dt.MakeFromTimezone( wxDateTime::UTC );

	switch ( dateFormat )
	{
		case DateFormat::Local:
			result = dt.FormatDate();
		break;
		case DateFormat::ISO:
		default:
			result = dt.FormatISODate();
		break;
	}
#endif

#ifdef DEBUG_DATEFORMATTER
	debuglog << "DateFormatter::formatDateInternal jd " << std::setprecision( 12 ) << std::setw( 12 ) << jd << " dateFormat " << dateFormat
		<< " result " << result;
#endif
	return result;
}

/*****************************************************
**
**   DateFormatter   ---   formatDatetimeWithOffsetInternal
**
******************************************************/
wxString DateFormatter::formatDatetimeWithOffsetInternal( const double &jd, const double &tzoffset, const bool &includeTime, const DateFormat &dateFormat )
{
	if ( jd == 0 ) return "-";

	Formatter *tf = Formatter::get();

	const double reljd = jd + tzoffset / 24.0;

	wxString s;
	s << formatDateInternal( reljd, dateFormat );
	if ( includeTime )
	{
		s << " " << tf->formatTime( getTimeFromJD( reljd ))
			<< " (" << ( tzoffset < 0 ? '-' : '+' ) << Formatter::get()->formatTime( tzoffset ) << ")";
	}
	return s;
}

/*****************************************************
**
**   DateFormatter   ---   formatDatetime
**
******************************************************/
wxString DateFormatter::formatDatetime( const double &jd, const bool &isLocaltime )
{
	if ( jd == 0 ) return "-";

	Formatter *tf = Formatter::get();

	const double tzoffset = isLocaltime ? getTotalConfiguredTzAndDstOffset( jd ) : 0.0;
	const double reljd = jd + tzoffset / 24.0;

	wxString s = formatDateInternal( jd, DateFormat::ISO );

	s += " " + tf->formatTime( getTimeFromJD( reljd ))
		+ " (" + std::format( "{:.1f}", tzoffset ) + ")";
	return s;
}

/*****************************************************
**
**   DateFormatter   ---   formatDate
**
******************************************************/
wxString DateFormatter::formatDate( const double &jd, const bool &isLocaltime )
{
	if ( jd == 0 ) return "-";

	const double tzoffset = isLocaltime ? getTotalConfiguredTzAndDstOffset( jd ) : 0.0;
	const double reljd = jd + tzoffset / 24.0;

	return formatDateInternal( reljd, DateFormat::ISO );
}

/*****************************************************
**
**   DateFormatter   ---   formatDateAndTimePair
**
******************************************************/
std::pair<wxString, wxString> DateFormatter::formatDateAndTimePair( const double &jd, const bool &isLocaltime )
{
	if ( jd == 0 ) return std::make_pair( "-", "-" );

	wxString s;
	Formatter *tf = Formatter::get();

	const double tzoffset = isLocaltime ? getTotalConfiguredTzAndDstOffset( jd ) : 0.0;
	const double reljd = jd + tzoffset / 24.0;

	return std::make_pair( formatDateInternal( jd, DateFormat::ISO ),  tf->formatTime( getTimeFromJD( reljd )));
}

/*****************************************************
**
**   DateFormatter   ---   formatTime
**
******************************************************/
wxString DateFormatter::formatTime( const double &jd, const bool &isLocaltime )
{
	if ( jd == 0 ) return "-";

	Formatter *tf = Formatter::get();
	const double tzoffset = isLocaltime ? getTotalConfiguredTzAndDstOffset( jd ) : 0.0;
	const double reljd = jd + tzoffset / 24.0;
	return tf->formatTime( getTimeFromJD( reljd ));
}

/*****************************************************
**
**   DateFormatter   ---   getDayOfMonthAndWeekday
**
******************************************************/
std::pair<int, int> DateFormatter::getDayOfMonthAndWeekday( const double &jd, const bool &isLocaltime )
{
	const std::time_t tt = 86400.0 * ( jd - 2440587.5 );
	std::tm std_tm;

	if ( isLocaltime )
	{
		std_tm = *std::localtime( &tt );
	}
	else
	{
		std_tm = *std::gmtime( &tt );
	}

	const int day = std_tm.tm_mday;
	const int weekday = std_tm.tm_wday;
#ifdef DEBUG_DATEFORMATTER
	debuglog << "DateFormatter::getDayOfMonthAndWeekday isLocaltime " << isLocaltime
		<< " UT datetime " << formatUTDatetime( jd )
		<< " full datetime " << formatDatetime( jd, isLocaltime )
		<< " day " << day
		<< " weekday " << weekday;
#endif

	return std::make_pair( day, weekday );
}

#define DEBUG_CALC_TZINFO

/*****************************************************
**
**   DateFormatter   ---   getTotalConfiguredTzAndDstOffset
**
******************************************************/
double DateFormatter::getTotalConfiguredTzAndDstOffset( const double &jd )
{
	return astroconfig->standardLocation.tz + getConfiguredDstOffset( jd );
}

/*****************************************************
**
**   DateFormatter   ---   getConfiguredDstOffset
**
******************************************************/
double DateFormatter::getConfiguredDstOffset( const double &jd )
{
	return getDstOffset( jd, astroconfig->standardLocation.dstMode );
}

/*****************************************************
**
**   DateFormatter   ---   getDstOffset
**
******************************************************/
double DateFormatter::getDstOffset( const double &jd, const DaylightSavingMode &mode )
{
	switch( mode )
	{
		case DaylightSavingMode::Automatic:
			return getSystemDstOffset( jd );
		break;
		case DaylightSavingMode::FixedZero:
			return 0.0;
		break;
		case DaylightSavingMode::FixedOneHour:
			return 1.0;
		break;
		default:
			errorlog << "Wrong daylight saving mode " << (int)mode;
			assert( false );
		break;
	}
	return 0;
}

/*****************************************************
**
**   DateFormatter   ---   getSystemDstOffset
**
******************************************************/
double DateFormatter::getSystemDstOffset( const double &jd )
{
#if 1
	const time_t tt = 86400.0 * ( jd - 2440587.5 );
	struct tm *ttm = localtime( &tt );

	return( ttm->tm_isdst ? 1.0 : 0.0 );

#else
#endif
}

/*****************************************************
**
**   DateFormatter   ---   parseDateString
**
******************************************************/
bool DateFormatter::parseDateString( const wxString &s, int &year, int &month, int &day, double &jd, wxString &err )
{
	wxRegEx r( "^\\s*([-0-9]{1,5})-([0-9]{1,2})-([0-9]{1,2})\\s*$" );
	if ( ! r.Matches( s ))
	{
		err = std::format( "Date {} does not match the required format {}", s, "YYYY-DD-MM" );
		//warnlog << "DateFormatter::parseDateString no match, input " << s;
		return false;
	}
	assert( r.GetMatchCount() == 4 );

	r.GetMatch( s, 1 ).ToInt( &year );
	if ( year < MIN_EPHEM_YEAR || year > MAX_EPHEM_YEAR )
	{
		err = std::format( "Year must be between {} and {}", MIN_EPHEM_YEAR, MAX_EPHEM_YEAR );
		return false;
	}

	r.GetMatch( s, 2 ).ToInt( &month );
	if ( month < 1 || month > 12 )
	{
		err = "Month must be between 1 and 12";
		return false;
	}

	r.GetMatch( s, 3 ).ToInt( &day );
	if ( day < 1 || day > 31 )
	{
		err = "Day must be between 1 and 31";
		return false;
	}

	Calculator *calculator = CalculatorFactory().getCalculator();
	const bool isvalid = calculator->calc_and_check_jd( year, month, day, 0, jd ) != true;
	if ( ! isvalid )
	{
		err = std::format( "Date \"{}\" is not valid", s );
	}
	return isvalid;
}

/*****************************************************
**
**   DateFormatter   ---   parseDateString
**
******************************************************/
bool DateFormatter::parseDateString( const wxString &value, double &jd, const DateFormat &dateFormat, wxString &err )
{
#ifdef DEBUG_DATEFORMATTER
	debuglog << "DateFormatter::parseDateString value " << value << " DateFormat " << dateFormat;
#endif

	bool b = false;

#if 1
	if ( dateFormat != DateFormat::ISO )
	{
		warnlog << "DateFormatter::parseDateString dateformat " << dateFormat << " not supported";
	}

	int year = 0;
	int month = 0;
	int day = 0;

	return parseDateString( value, year, month, day, jd, err );

#else

	wxDateTime dt;
	wxString::const_iterator end;
	switch( dateFormat )
	{
		case DateFormat::Local:
			//b = dt.ParseDate( value, &end );
			b = dt.ParseFormat( value, "%d.%m.%Y", &end );
		break;
		case DateFormat::ISO:
			b = dt.ParseFormat( value, "%Y-%m-%d", &end );
		break;
		default:
			warnlog << "DateFormatter::parseDateString with invalid date format " << dateFormat << ", using default (ISO)";
			b = dt.ParseFormat( value, "%Y-%m-%d", &end );
		break;
	}
	const int year = dt.GetYear();
	if ( year < MIN_EPHEM_YEAR || year > MAX_EPHEM_YEAR )
	{
		warnlog << "DateFormatter::parseDateString: year is not valid, must be between " << MIN_EPHEM_YEAR << " and " << MAX_EPHEM_YEAR;
		b = false;
	}
	if ( b )
	{
		dt.MakeFromTimezone( wxDateTime::UTC );
		jd = dt.GetJDN();
	}
	else
	{
		warnlog << "DateFormatter::parseDateString date " << value << " is not valid";
	}

#ifdef DEBUG_DATEFORMATTER
	if ( b )
	{
		debuglog << "DateFormatter::parseDateString result date " << std::setprecision( 12 ) << std::setw( 12 ) << jd << " formatted " << formatUTDatetime( jd );
		debuglog << "DateFormatter::parseDateString result date (2) " << dt.Format( wxDefaultDateTimeFormat, wxDateTime::UTC ) << " local " << wxDateTime::Local;
		dt.ToTimezone( wxDateTime::Local );
		debuglog << "DateFormatter::parseDateString result date (3) " << dt.Format( wxDefaultDateTimeFormat, wxDateTime::Local ) << " local " << wxDateTime::Local
			<< " xx " << formatUTDatetime( dt.GetJDN() );
	}
	else
	{
		debuglog << "DateFormatter::parseDateString result: date " << value << " ist not valid";
	}
	//wxString s = wxUILocale::GetInfo( wxLOCALE_LONG_DATE_FMT );
	//debuglog << "DateFormatter::parseDateString date format " << s;

#endif // debug
#endif 
	return b;
}
 
/*****************************************************
**
**   DateFormatter   ---   parseDateAndTime
**
******************************************************/
bool DateFormatter::parseDateAndTime( const wxString &thedate, double &jd, wxString &err )
{
	wxString token;
	double t = 0;
	jd = 0.0;

	wxStringTokenizer tokenizer( thedate, " " );

	if ( tokenizer.HasMoreTokens() )
	{
    wxString token = tokenizer.GetNextToken();
#ifdef DEBUG_DATEFORMATTER
		debuglog<< "Date token " << token;
#endif

		if ( ! parseDateString( token, jd, DateFormat::ISO, err ))
		{
			errorlog << "invalid date string " << token << ", error message: " << err;
			return false;
		}
	}

	if ( tokenizer.HasMoreTokens() )
	{
    wxString token = tokenizer.GetNextToken();
#ifdef DEBUG_DATEFORMATTER
		debuglog<< "time token " << token;
#endif
		if ( ! Formatter::get()->parseDegreeString( token, t, 24.0, err ))
		{
			errorlog << "invalid time string " << token << " error was " << err;
			return false;
		}
		jd += t / 24.0;
	}
#ifdef DEBUG_DATEFORMATTER
		debuglog<< "DateFormatter::parseDateAndTime sucessfull, jd " << std::setprecision( 12 ) << jd;
#endif
	return true;
}

/*****************************************************
**
**   DateFormatter   ---   calculateDateIntegers
**
******************************************************/
std::tuple<int,int,int> DateFormatter::calculateDateIntegers( const double &jd )
{
	const auto [ year, month, day, ut ] = CalculatorFactory().getCalculator()->getDateIntsFromJD( jd );
	return std::make_tuple( year, month, day );
}



