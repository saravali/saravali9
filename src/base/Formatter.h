/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Formatter.h
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

#ifndef _FORMATTER_H_
#define _FORMATTER_H_

#include "bootstrap.h"
#include "constants.h"
#include "Viewpoint.h"
#include "mstring.h"

#include <wx/string.h>

/*************************************************//**
*
*   Formatting of date, time and degree values
*
******************************************************/
class Formatter : public Singleton<Formatter>
{
	friend class Singleton<Formatter>;

public:

	MString formatZodiacalLongitude( PlanetPropertyConfig, const double &longitude, const MovingDirection = MovingDirection::None,
		const int division = 1 );

	MString formatZodiacalLongitude( const Viewpoint&, const double &longitude, const MovingDirection = MovingDirection::None,
		const int division = 1 );

	const wxString formatDegreeStringBounded( double t, const DegPrecision &format,
		const double &minvalue, const double &maxvalue );

	const wxString formatSignedDegreeString( const double &t, const DegPrecision format = DegPrecision::FlexibleCutSecond );

	// formats a latitude like DD:MM:SS  [N|S]
	const wxString formatLatitude( const double&, const DegPrecision = DegPrecision::FlexibleCutSecond );

	// formats a longitude like DD:MM:SS  [E|W]
	const wxString formatLongitude( const double&, const DegPrecision = DegPrecision::FlexibleCutSecond );

	// formats a time value like HH:MM:SS
	const wxString formatTime( const double &t, const DegPrecision format = DegPrecision::FlexibleCutSecond )
		{ return formatDegreeStringBounded( t, format, 0.0, 24.0 ); }

	// formats a degree value like DD:MM:SS
	const wxString formatDegrees( const double &t, const DegPrecision format = DegPrecision::FlexibleCutSecond )
		{ return formatDegreeStringBounded( t, format, 0.0, 360.0 ); }

	bool parseDegreeString( const wxString&, double &value, const int &maxvalue, wxString &err );

	bool parseDoubleString( const wxString &s, double &value, const double &minvalue, const double &maxvalue, wxString &err );

	wxString formatDoubleString( const double &t, const DegPrecision format = DegPrecision::FlexibleCutSecond );

private:

	bool parseDegreeStringInternal( wxString value, int &deg, int &minute, double &second, const int &maxvalue, wxString &err );

	// calculates degrees, minutes and seconds from a given length from a (double) degree value
	std::tuple<int, int, double> splitDegMinSec( const double& );

	const wxString formatDegreeStringInternal( double t, const DegPrecision& );
};

#endif // _FORMATTER_H_

