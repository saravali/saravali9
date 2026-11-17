/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Formatter.cpp
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

#include "Formatter.h"

#include "astroconfig.h"
#include "gdiconfig.h"
#include "Lang.h"
#include "enumserializer.h"
#include "constants.h"
#include "mathbase.h"

#include <wx/regex.h>
#include <format>

extern Gdiconfig *gdiconfig;
extern Astroconfig *astroconfig;

//#define DEBUG_FORMATTER

/*****************************************************
**
**   Formatter   ---   splitDegMinSec
**
******************************************************/
std::tuple<int, int, double> Formatter::splitDegMinSec( const double &l )
{
	if ( l < 0 )
	{
		errorlog << "Formatter::splitDegMinSec negative longitude not allowed " << l;
	}

	const double mylen = fabs( l );

	const int deg = (int)mylen;
	const double mind = ( mylen - (int)mylen ) * 60;
	const int min = (int)mind;
	double sec = ( mind - (int)mind ) * 60;

#ifdef DEBUG_FORMATTER
	debuglog << std::format( "Formatter::splitDegMinSec l {:.8f} result {}-{}-{}", l, deg, min, sec );
#endif	
	return std::make_tuple( deg, min, sec );
}

/*****************************************************
**
**   Formatter   ---   formatDegreeStringInternal
**
******************************************************/
const wxString Formatter::formatDegreeStringInternal( double t, const DegPrecision &format )
{
	const wxString sign = ( t >= 0 ? "" : "-" );

	// int, int, double
	auto[ deg, min, sec ] = splitDegMinSec( fabs( t ));

#ifdef DEBUG_FORMATTER
		debuglog << "Formatter::formatDegreeStringInternal t " << t << " format " << format << " deg " << deg << " min " << min << " sec " << sec;
#endif	

	if ( format == DegPrecision::More && t != 0.0 )
	{
		return std::format( "{}{:02d}:{:02d}:{:08.5f}", sign, deg, min, sec );
	}

	// else
	sec = a_rund( sec );
	if ( sec == 60 )
	{
		sec = 0;
		min++;
		if ( min == 60 )
		{
			min = 0;
			deg++;
		}
	}
	if ( format == DegPrecision::Second
		|| ( format == DegPrecision::FlexibleCutMinute && sec != 0 )
		|| ( format == DegPrecision::FlexibleCutSecond && sec != 0 )
	)
	{
		return std::format( "{}{:02d}:{:02d}:{:02d}", sign, deg, min, (int)sec );
	}
	else if ( format == DegPrecision::Minute
		|| ( format == DegPrecision::FlexibleCutSecond && sec == 0 )
		|| ( format == DegPrecision::FlexibleCutMinute && min != 0 )
	)
	{
		if ( sec >= 30 ) min++;
		if ( min == 60 )
		{
			min = 0;
			deg++;
		}
		return std::format( "{}{:02d}:{:02d}", sign, deg, min );
	}

	// else
	return std::format( "{}{}", sign, deg );
}

/*****************************************************
**
**   Formatter   ---   formatDegreeStringBounded
**
******************************************************/
const wxString Formatter::formatDegreeStringBounded( double t, const DegPrecision &format,
	const double &minvalue, const double &maxvalue )
{
	if ( t < minvalue || t > maxvalue )
	{
		warnlog << "Formatter::formatDegreeStringBounded invalid value t " << t << " minvalue " << minvalue 
			<< " maxvalue " <<  maxvalue;
		t = a_red( t, maxvalue );
	}
	return formatDegreeStringInternal( t, format );
}

/*****************************************************
**
**   Formatter   ---   formatSignedDegreeString
**
******************************************************/
const wxString Formatter::formatSignedDegreeString( const double &t, const DegPrecision format )
{
	return formatDegreeStringInternal( t, format );
}

/*****************************************************
**
**   Formatter   ---   formatLatitude
**
******************************************************/
const wxString Formatter::formatLatitude( const double &l, const DegPrecision format )
{
	wxString s = formatDegreeStringBounded( fabs( l ), format, 0.0, 90.0 );

	if ( l > 0 ) s += "N";
	else if ( l < 0 ) s += "S";
	return s;
}

/*****************************************************
**
**   Formatter   ---   formatLongitude
**
******************************************************/
const wxString Formatter::formatLongitude( const double &l, const DegPrecision format )
{
	wxString s = formatDegreeStringBounded( fabs( l ), format, 0.0, 180.0 );
	if ( l > 0 ) s += "E";
	else if ( l < 0 ) s += "W";
	return s;
}

/*****************************************************
**
**   Formatter   ---   parseDegreeString
**
******************************************************/
bool Formatter::parseDegreeString( const wxString &s, double &value, const int &maxvalue, wxString &err )
{
	// support simple double values like "1.5"
	if ( s.Find( ":" ) == wxNOT_FOUND )
	{
		double d = 0.0;
		//const bool b = stringToDouble( s, d );
		const bool b = s.ToDouble( &d );
#ifdef DEBUG_FORMATTER
		debuglog << "Formatter::parseDegreeString " << s << " stringToDouble returns " << std::boolalpha << b << " value " << d;
#endif	
		if ( b )
		{
			value = d;
		}
		return b;
	}

	int deg = 0;
	int min = 0;
	double sec = 0.0;

	const bool b = parseDegreeStringInternal( s, deg, min, sec, maxvalue, err );
	if ( b )
	{
		value = sec + min * 60 + deg * 3600;
		value /= 3600.0;
	}
#ifdef DEBUG_FORMATTER
		debuglog << "Formatter::parseDegreeString returns " << std::boolalpha << b;
#endif	
	return b;
}

/*****************************************************
**
**   Formatter   ---   parseDegreeStringInternal
**
******************************************************/
bool Formatter::parseDegreeStringInternal( wxString s, int &deg, int &minute, double &second, const int &maxvalue, wxString &err )
{
	deg = 0;
	minute = 0;
	second = 0.0;

#ifdef DEBUG_FORMATTER
		debuglog << "Formatter::parseDegreeStringInternal start string " << s << " maxvalue " << maxvalue;
#endif	

	wxRegEx r( "^\\s*([0-9]{1,3})(?::([0-9]{1,2}))?(?::([0-9\\.]+))?\\s*$" );
	if ( ! r.Matches( s ))
	{
		err = std::format( "Input {} does not match", s );
		warnlog << "Formatter::parseDegreeStringInternal no match, input " << s;
		return false;
	}

	const size_t matchcount = r.GetMatchCount();
#ifdef DEBUG_FORMATTER
	debuglog << "Formatter::parseDegreeString " << s << " matchcount " << matchcount;
	for( size_t i = 0; i < matchcount; i++ )
	{
		debuglog << "Formatter::parseDegreeStringInternal i " << i << " value " << r.GetMatch( s, i );
	}
#endif	

	assert( matchcount > 1 );
	if ( r.GetMatch( s, 1 ).length() )
	{
#ifdef DEBUG_FORMATTER
		debuglog << "Formatter::parseDegreeStringInternal deg value: " << r.GetMatch( s, 1 );
#endif	
		r.GetMatch( s, 1 ).ToInt( &deg );
		if ( deg < 0 || deg > maxvalue )
		{
			err = std::format( "Degree value must be between 0 and {}", (int)maxvalue );
			return false;
		}
	}
	if ( matchcount > 2 && r.GetMatch( s, 2 ).length() )
	{
#ifdef DEBUG_FORMATTER
		debuglog << "Formatter::parseDegreeStringInternal minute value: " << r.GetMatch( s, 2 );
#endif	
		r.GetMatch( s, 2 ).ToInt( &minute );
		if ( minute < 0 || minute > 60 )
		{
			err = "Minute value must be between 0 and 60";
			return false;
		}
	}
	if ( matchcount > 3 && r.GetMatch( s, 3 ).length() )
	{
#ifdef DEBUG_FORMATTER
		debuglog << "Formatter::parseDegreeStringInternal minute value: " << r.GetMatch( s, 3 );
#endif	
		//stringToDouble( r.GetMatch( s, 3 ), second );
		r.GetMatch( s, 3 ).ToDouble( &second );
		if ( second < 0 || second > 60 )
		{
			err = "Second value must be between 0 and 60";
			return false;
		}
	}

	// special case
	if ( deg == maxvalue &&  minute + second > 0 )
	{
#ifdef DEBUG_FORMATTER
		debuglog << "Formatter::parseDegreeStringInternal input error in value deg " << deg << " maxvalue " << maxvalue;
#endif	
		err = std::format( "Value {}:{}:{} exceeds maximum value", deg, minute, second );
		return false;
	}
	return true;
}

/*****************************************************
**
**   Formatter   ---   parseDoubleString
**
******************************************************/
bool Formatter::parseDoubleString( const wxString &s, double &value, const double &minvalue, const double &maxvalue, wxString &err )
{
	double v = 0;
	if ( ! stringToDouble( s, v ))
	{
		err = std::format( "String {} cannot be converted to floating point value", s );
		return false;
	}

	if ( v < minvalue || v > maxvalue )
	{
		err = std::format( "Floating point value must be between {} and {}", minvalue, maxvalue );
	} 
	value = v;
	return true;
}

/*****************************************************
**
**   Formatter   ---   formatDoubleString
**
******************************************************/
wxString Formatter::formatDoubleString( const double &t, const DegPrecision /*format*/  )
{
	return std::format( "{:.6f}", t );
}

/*****************************************************
**
**   Formatter   ---   formatZodiacalLongitude
**
******************************************************/
MString Formatter::formatZodiacalLongitude( const Viewpoint &viewpoint, const double &longitude, const MovingDirection dir,
	const int division )
{
	const PlanetPropertyConfig &pcfg = astroconfig->getViewpointConfig( viewpoint ).planetProps;
	return formatZodiacalLongitude( pcfg, longitude, dir, division );
}

/*****************************************************
**
**   Formatter   ---   formatZodiacalLongitude
**
******************************************************/
MString Formatter::formatZodiacalLongitude( PlanetPropertyConfig pcfg, const double &longitude, const MovingDirection dir,
	const int division )
{
	assert( division );
	if ( division != 1 )
	{
		return MString( formatDegrees( a_red( longitude, 360.0 / division ), pcfg.precision ));
	}

	MString t;
	if ( dir != MovingDirection::None )
	{
		switch( pcfg.retroFormat )
		{
			case 1: // text
				t << MToken( Lang().getMovingDirectionName( dir ),
					pcfg.precision == DegPrecision::Minute ? TokenFontPosition::Normal : TokenFontPosition::Subscriptum );
			break;
			case 2: // off
			break;
			case 0: // symbol
			default:
				t << MToken( dir, TextFormat::Symbol,
					pcfg.precision == DegPrecision::Minute ? TokenFontPosition::Normal : TokenFontPosition::Subscriptum );
			break;
		}
	}


	double mylen = red_deg( longitude );
	const Sign sign = (Sign)(int)( mylen / 30 );
	mylen -= 30 * (int)sign;

	auto[ deg, min, sec ] = splitDegMinSec( mylen );
#ifdef DEBUG_FORMATTER
		debuglog << "Formatter::formatZodiacalLongitude longitude " << longitude << " deg " << deg << " min " << min  << " sec " << sec;
#endif	

	// rounding stuff
	if ( pcfg.precision != DegPrecision::More )
	{
		sec = a_rund( sec );
		if ( sec == 60 && pcfg.precision != DegPrecision::More )
		{
			sec = 0;
			min++;
			if ( min == 60 )
			{
				min = 0;
				deg++;
			}
		}
	}

	wxString s_sec;
	switch( pcfg.precision )
	{
		case DegPrecision::Second:
			s_sec = std::format( "{:02d}", (int)sec );
		break;
		case DegPrecision::Minute:
			if ( sec > 30 )
			{
				min++;
			}
			if ( min == 60 )
			{
				min = 0;
				deg++;
			}
		break;
		case DegPrecision::More:
		{
			s_sec = std::format( "{:08.5f}", sec );
		}
		break;
		default:
		break;
	}

	const wxString s_deg = std::format( "{:02d}", deg );
	const wxString s_min = std::format( "{:02d}", min );

	switch( pcfg.positionStyle )
	{
		case PositionStyle::Vedic:
		{
			wxString s = std::format( "{:02d}", (int)sign ) + "-" + s_deg + "-" + s_min;
			if( pcfg.precision != DegPrecision::Minute )
			{
				s += "-" + s_sec;
			}
			t << s;
		}
		break;
		case PositionStyle::Compact:
		{
			if ( pcfg.textFormat != TextFormat::Symbol ) pcfg.textFormat = TextFormat::Short;
			t << s_deg << MToken( sign, pcfg );
			/*
			t << s_deg << MToken( sign,
				pcfg.textFormat == TextFormat::Symbol ?TextFormat::Symbol : TextFormat::Short,
				pcfg.capricornSymbol, pcfg.vedicSignNames );
			*/

			t << s_min << MToken( TokenId::MinuteSymbol );
			if ( pcfg.precision != DegPrecision::Minute )
			{
				t << s_sec << MToken( TokenId::SecondSymbol );
			}
			t << " ";
		}
		break;
		case PositionStyle::Colons:
		{
			t << s_deg << ":" << s_min;
			if ( pcfg.precision != DegPrecision::Minute )
			{
				t << ":" << s_sec;
			}
			t << " " << MToken( sign, pcfg );
		}
		break;
		case PositionStyle::Default:
		default:
		{
			t << s_deg << MToken( TokenId::DegreeSymbol ) << s_min << MToken( TokenId::MinuteSymbol );
			if ( pcfg.precision != DegPrecision::Minute )
			{
				t << s_sec << MToken( TokenId::SecondSymbol );
			}
			t << " " << MToken( sign, pcfg );
		}
		break;
	}
#ifdef DEBUG_FORMATTER
		debuglog << "FINISHED Formatter::formatZodiacalLongitude longitude " << longitude << " result " << t;
#endif	
	return t;
}

