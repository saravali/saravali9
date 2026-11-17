/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/DateFormatter.h
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

#ifndef _DATEFORMAT_H_
#define _DATEFORMAT_H_

#include "bootstrap.h"
#include "constants.h"

#include <tuple>
#include <wx/string.h>

enum class DaylightSavingMode;

/*************************************************//**
*
*  
*
******************************************************/
class DateFormatter : public Singleton<DateFormatter>
{
	friend class Singleton<DateFormatter>;

public:

	/**
	*  Format date like YYYY-DD-MM (no timezone)
	*/
	wxString formatUTDate( const double &jd, const DateFormat dateFormat = DateFormat::ISO )
	{
		return formatDateInternal( jd, dateFormat );
	}

	/**
	*  Format date like YYYY-DD-MM HH:MM:SS (no timezone)
	*/
	wxString formatUTDatetime( const double &jd, const DateFormat dateFormat = DateFormat::ISO )
	{
		return formatDatetimeWithOffsetInternal( jd, 0, true, dateFormat );
	}

	/**
	*  Format date like YYYY-DD-MM +/-tz (using tz offset in hours)
	*/
	wxString formatDateOffset( const double &jd, const double &tzoffset, const DateFormat &dateFormat )
	{
		return formatDatetimeWithOffsetInternal( jd, tzoffset, false, dateFormat );
	}

	/**
	*  Format date and time like YYYY-DD-MM HH:MM:SS +/-tz (using tz offset in hours)
	*/
	wxString formatDatetimeOffset( const double &jd, const double &tzoffset, const DateFormat &dateFormat  )
	{
		return formatDatetimeWithOffsetInternal( jd, tzoffset, true, dateFormat );
	}

	/**
	*  Format date and time like YYYY-DD-MM HH:MM:SS +/-tz (using tz offset in hours). Timezone is set automatically
	*/
	wxString formatDatetime( const double &jd, const bool &isLocaltime );

	/**
	*  Format local date like YYYY-DD-MM HH:MM:SS ). Timezone is set automatically
	*/
	wxString formatDate( const double &jd, const bool &isLocaltime );

	/**
	* Formats the time like HH::MM:SS
	*
	* Used by ephemeris, hora, StandardLocationConflet
	*/
	wxString formatTime( const double &jd, const bool &isLocaltime );

	/**
	* Format date and time separately (first and second part of std::pair)
	*
	* Used by EcplipseExpert.
	*/
	std::pair<wxString, wxString> formatDateAndTimePair( const double &jd, const bool &isLocaltime );

	double getTotalConfiguredTzAndDstOffset( const double &jd );

	/**
	* Returns the DST value for a given JD according to configuration of standard location (i.e. fixed or from the system).
	*/
	double getConfiguredDstOffset( const double &jd );

	/**
	* Returns the DST value for given JD and dst mode.
	*/
	double getDstOffset( const double &jd, const DaylightSavingMode& );

	/**
	* Returns the DST value of the local system at a given JD.
	*/
	double getSystemDstOffset( const double &jd );

	std::pair<int, int> getDayOfMonthAndWeekday( const double &jd, const bool &isLocaltime );

	bool parseDateString( const wxString &value, int &year, int &month, int &day, double &jd, wxString &err );

	/**
	* 
	*/
	bool parseDateString( const wxString &value, double &jd, const DateFormat&, wxString &err );

	/**
	* Used by textclients (parse date from command line). Return true on success.
	*/
	bool parseDateAndTime( const wxString&, double &jd, wxString &err );

	std::tuple<int,int,int> calculateDateIntegers( const double &jd );

	int calculateYearFromJD( const double &jd )
	{
		return std::get<0>( calculateDateIntegers( jd ));
	}

protected:

	wxString formatDateInternal( const double &jd, const DateFormat& );

	wxString formatDatetimeWithOffsetInternal( const double &jd, const double &tzoffset, const bool &includeTime, const DateFormat& );


};

#endif // _DATEFORMAT_H_

