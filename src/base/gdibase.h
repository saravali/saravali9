/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/gdibase.h
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

#ifndef _GDIBASE_H_
#define _GDIBASE_H_

#include "bootstrap.h"

#include <list>
#include <map>

#include<wx/brush.h>
#include <wx/colour.h>
#include<wx/gdicmn.h>
#include<wx/pen.h>
#include<wx/string.h>

MComplex findOrthogonalPoint( const MComplex&, const double length = 1.0 );
wxPoint pointToWxPoint( const MComplex& );

/*************************************************//**
*
*  MRect is a rectangle simliar to wxRect.
*
*  It is needed because wxRect's integer coordinates are not suffcient, e.g. for combined widgets or PDF output.
*
******************************************************/
struct MRect : public Jsonable
{
	MRect() { reset(); }

	MRect( const double &x0, const double &y0, const double &w, const double &h )
		: x( x0 ),
		y( y0 ),
		width( w ),
		height( h )
	{}

	MRect( const wxRect &r )
		: x( r.GetLeft() ),
		y( r.GetTop() ),
		width( r.GetWidth() ),
		height( r.GetHeight() )
	{}

	// points will be used as opposite corners
	MRect( const MComplex&, const MComplex& );

	virtual ~MRect() {}

	wxRect toWxRect() const { return wxRect( (int)x, (int)y, (int)width, (int)height ); }

	MComplex getCenter() const { return MComplex( x + .5 * width, y + .5 * height ); }

	MComplex getExtent() const { return MComplex( width, height ); }

	bool isNil() const { return width <= 0 || height <= 0; }
	void reset() { x = y = width = height = 0; }

	double getRight() const { return x + width; }
	double getBottom() const { return y + height; }

	bool intersects( const MRect& ) const;
	MRect& intersect( const MRect& );
	MRect intersect( const MRect& ) const;

	MRect join( const MRect& );

	MRect grow( const double& );
	MRect moveTo( const double &x0, const double &y0 );
	MRect translate( const double &x0, const double &y0 );

	bool containsPoint( const MComplex& ) const;
	bool containsPoint( const double &x, const double &y ) const { return containsPoint( MComplex( x, y )); }

	// json:start
	// json:ostream
	double x;
	double y;
	double width;
	double height;

	friend mostream& operator <<( mostream&, const MRect& );

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*  wxBrush is wrapped by this class because stipple filename and hue angle must be stored.
*
******************************************************/
struct MBrush : public Jsonable
{
	// json:noimpl
	MBrush() {}

 	MBrush( const wxColour &color, const wxBrushStyle style = wxBRUSHSTYLE_SOLID ) : color( color ), style( style ) {}

 	MBrush( const wxString &stippleFilename, const int rotateHue );

	MBrush( const MBrush& );

	virtual ~MBrush() {}

	MBrush& operator= ( const MBrush &t );

	bool isOk() const;

	wxColour color;
	wxBrushStyle style = wxBRUSHSTYLE_INVALID;
	wxString filename;
	int rotateHue = 0;

	friend mostream& operator <<( mostream&, const MBrush& );

	// impls hand-written in JsonHandler.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*  Overrrides pens attribute wise. Returns true if anything changed.
*
******************************************************/
bool softOverridePen( wxPen &pen, const wxPen &refPen );

/*************************************************//**
*
*  Available wxPenStyles and wxBrushStyles
*
******************************************************/
const static std::map<wxPenStyle, wxString> mapping_list_penstyle =
{
	{ wxPENSTYLE_INVALID, "None" },
	{ wxPENSTYLE_TRANSPARENT, "Transparent" },
	{ wxPENSTYLE_SOLID, "Solid" },
	{ wxPENSTYLE_DOT, "Dot" },
	{ wxPENSTYLE_LONG_DASH, "LongDash" },
	{ wxPENSTYLE_SHORT_DASH, "ShortDash" },
	{ wxPENSTYLE_DOT_DASH, "DotDash" },
};

const static std::map<wxBrushStyle, wxString> mapping_list_brushstyle =
{
	{ wxBRUSHSTYLE_INVALID, "None" },
	{ wxBRUSHSTYLE_TRANSPARENT, "Transparent" },
	{ wxBRUSHSTYLE_SOLID, "Solid" },
	{ wxBRUSHSTYLE_STIPPLE, "Stipple" },
	{ wxBRUSHSTYLE_BDIAGONAL_HATCH, "BackwardDiagonalHatch" },
	{ wxBRUSHSTYLE_CROSSDIAG_HATCH, "CrossDiagonalHatch" },
	{ wxBRUSHSTYLE_FDIAGONAL_HATCH, "ForwardDiagonalHatch" },
	{ wxBRUSHSTYLE_CROSS_HATCH, "CrossHatch" },
	{ wxBRUSHSTYLE_VERTICAL_HATCH, "VerticalHatch" },
	{ wxBRUSHSTYLE_HORIZONTAL_HATCH, "HorizontalHatch" },
};

mostream& operator <<( mostream&, const wxSize& );

enum class ConfletSet
{
	// json:enum
	None = -1,
	General,
	Calculation,
	Directions,
	Gui,
	Printout,
	First = General,
	Last = Printout
};

enum class DoNotShowAgainTopic
{
	// json:enum
	None = -1,
	LanguageChange,
	EphemConfigChange,
	DaylightSavingTime,
	ImageExportSuccessful,
	TextExportSuccessful,
	UranianMatchesExceeded,
	MissingEphemData,
	First = LanguageChange,
	Last = MissingEphemData
};

#endif

