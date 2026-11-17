/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/JsonConverter.cpp
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

#include "JsonConverter.h"

#include "gdibase.h"
#include "gdiconfig.h"

#include <wx/regex.h>

extern Gdiconfig *gdiconfig;

/**************************************************************
***
**   EXTRACT FROM JSON
***
***************************************************************/
void extractFromJson( const Json::Value &v, bool &b, JsonHandler &err )
{
	if ( v.isBool()) { b = v.asBool(); }
	else { err.addReadValueError( v, "is not boolean" ); }
}

void extractFromJson( const Json::Value &v, int &i, JsonHandler &err )
{
	if ( v.isInt()) { i = v.asInt(); }
	else { err.addReadValueError( v, "is not int" ); }
}

void extractFromJson( const Json::Value &v, uint &i, JsonHandler &err )
{
	if ( v.isUInt()) { i = v.asUInt(); }
	else { err.addReadValueError( v, "is not unsigned int" ); }
}

void extractFromJson( const Json::Value &v, double &d, JsonHandler &err )
{
	if ( v.isDouble()) { d = v.asDouble(); }
	else { err.addReadValueError( v, "is not double" ); }
}

void extractFromJson( const Json::Value &v, wxString &s, JsonHandler &err )
{
	if ( v.isString()) { s = v.asString(); }
	else { err.addReadValueError( v, "is not string" ); }
}

void extractFromJson( const Json::Value &v, wxSize &s, JsonHandler& )
{
	if ( v[ "x" ] && v[ "x" ].isInt() ) s.SetWidth( v[ "x" ].asInt());
	if ( v[ "y" ] && v[ "y" ].isInt() ) s.SetHeight( v[ "y" ].asInt());
}

void extractFromJson( const Json::Value &v, Jsonable &j, JsonHandler &err )
{
	j.fromJsonValue( v, err );
}

void extractFromJson( const Json::Value &v, DasaLord &lord, JsonHandler &jh )
{
	if ( v.isString())
	{
#ifdef CPP26
		const PlanetId o = string_to_enum<PlanetId>( v.asString());
#else
		PlanetId o = PlanetId::None;
		string_to_enum( v.asString(), o );
#endif
		if ( o != PlanetId::None )
		{
			lord = o;
		}
		else
		{
#ifdef CPP26
			const Sign sign = string_to_enum<Sign>( v.asString());
#else
			Sign sign;
			string_to_enum( v.asString(), sign );
#endif
			if ( sign != Sign::None ) lord = sign;
		}
	}
	else { jh.addReadValueError( v, "enum canno be converted to valid DasaLord" ); }
}

void extractFromJson( const Json::Value &v, wxColour &color, JsonHandler &jh )
{
	if ( v.isString())
	{
		// rgba is not properly converted (alpha missing), so do it manually
		const wxString s = v.asString();
		wxRegEx r("^\\s*rgba\\s*\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\)\\s*$");
		if ( r.Matches( s ))
		{
			assert( r.GetMatchCount() == 5 );
			long red, green, blue, alpha;
			r.GetMatch( s, 1 ).ToLong( &red );
			r.GetMatch( s, 2 ).ToLong( &green );
			r.GetMatch( s, 3 ).ToLong( &blue );
			r.GetMatch( s, 4 ).ToLong( &alpha );
			color.Set( (int)red, (int)green, (int)blue, (int)alpha );
		}
		else
		{
			color.Set( s );
		}
	}
	else { jh.addReadValueError( v, "is not string" ); }
}

void extractFromJson( const Json::Value &v, wxPen &pen, JsonHandler &jh )
{
  if ( v[ "color" ] )
	{
		wxColour color;
		extractFromJson( v[ "color" ], color, jh );
		if ( color.IsOk() ) pen.SetColour( color );
	}
  if ( v[ "width" ] )
	{
		int width = 0;
		extractFromJson( v[ "width" ], width, jh );
		pen.SetWidth( width );
	}

  if ( v[ "style" ] )
	{
		wxPenStyle style = wxPENSTYLE_INVALID;
		std::string s = v[ "style" ].asString();
		for( const auto &m : mapping_list_penstyle )
		{
			if ( m.second == s )
			{
				style = m.first;
				break;
			}
		}
		if ( style != wxPENSTYLE_INVALID ) pen.SetStyle( style );
	}
}

/**************************************************************
***
**  JSON OUTPUT
***
***************************************************************/
Json::Value writeToJson( const Jsonable &a, JsonHandler &err )
{
	return a.toJsonValue( err );
}

Json::Value writeToJson( const int &a, JsonHandler& )
{
	return Json::Value( a );
}

Json::Value writeToJson( const uint &a, JsonHandler& )
{
	return Json::Value( a );
}

Json::Value writeToJson( const bool &b, JsonHandler& )
{
	return Json::Value( b );
}

Json::Value writeToJson( const double &a, JsonHandler& )
{
	return Json::Value( a );
}

Json::Value writeToJson( const wxString &s, JsonHandler& )
{
	return Json::Value( s.ToStdString() );
}

Json::Value writeToJson( const wxSize &s, JsonHandler& )
{
	Json::Value v;
	v[ "x" ] = s.GetWidth();
	v[ "y" ] = s.GetHeight();
	return v;
}

Json::Value writeToJson( const DasaLord &lord, JsonHandler& )
{
	Json::Value v;
	if ( std::holds_alternative<PlanetId>( lord ))
	{
		v = std::string( enum_to_string( std::get<PlanetId>( lord )));
	}
	else if ( std::holds_alternative<Sign>( lord ))
	{
		v = std::string( enum_to_string( std::get<Sign>( lord )));
	}
	return v;
}

Json::Value writeToJson( const wxColour &color, JsonHandler& )
{
	Json::Value v;
	v = color.GetAsString( wxC2S_CSS_SYNTAX ).ToStdString();
	return v;
}

Json::Value writeToJson( const wxPen &pen, JsonHandler &jh )
{
	Json::Value v;

	if ( pen.GetColour().IsOk())
	{
		v[ "color" ] = writeToJson( pen.GetColour(), jh );
	}

  if ( pen.GetWidth() > 0 ) v[ "width" ] = writeToJson( pen.GetWidth(), jh );

	if ( pen.GetStyle() >= 0 )
	{
		for( const auto &m : mapping_list_penstyle )
		{
			if ( m.first == pen.GetStyle() )
			{
				v[ "style" ] = writeToJson( m.second, jh );
				break;
			}
		}
	}
	return v;
}

