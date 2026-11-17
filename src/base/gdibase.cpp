/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/gdibase.cpp
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

#include "gdibase.h"

#include "gdiconfig.h"
#include "mathbase.h"

#include <format>

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   findOrthogonalPoint
**
******************************************************/
MComplex findOrthogonalPoint( const MComplex &p, const double length )
{
	if ( p == MComplex( 0, 0 )) return MComplex();
	if ( p.imag() == 0 ) return MComplex( 0, length );

	// p * i is orthogonal
	return ( length / abs( p ))  * p * MComplex( 0, 1 );
}

/*****************************************************
**
**   pointToWxPoint
**
******************************************************/
wxPoint pointToWxPoint( const MComplex &p )
{
  return wxPoint( a_rund( p.real() ), a_rund( p.imag() ));
}

/*****************************************************
**
**   MRect   ---   2 points constructor
**
******************************************************/
MRect::MRect( const MComplex &p, const MComplex &q )
{
	x = std::min( p.real(), q.real());
	y = std::min( p.imag(), q.imag());
	width = fabs( q.real() - p.real());
	height = fabs( q.imag() - p.imag() );
}

/*****************************************************
**
**   MRect   ---   grow
**
******************************************************/
MRect MRect::grow( const double &d )
{
	x -= d;
	y -= d;
	width += 2 * d;
	height += 2 * d;
	return *this;
}

/*****************************************************
**
**   MRect   ---   moveTo
**
******************************************************/
MRect MRect::moveTo( const double &x0, const double &y0 )
{
	x = x0;
	y = y0;
	return *this;
}

/*****************************************************
**
**   MRect   ---   translate
**
******************************************************/
MRect MRect::translate( const double &x0, const double &y0 )
{
	x += x0;
	y += y0;
	return *this;
}

/*****************************************************
**
**   MRect   ---   intersects
**
******************************************************/
bool MRect::intersects( const MRect &rect ) const
{
	MRect r = intersect( rect );
	return r.width != 0;
}

/*****************************************************
**
**   MRect   ---   intersect
**
******************************************************/
MRect MRect::intersect( const MRect& rect ) const
{
	MRect r = *this;
	r.intersect( rect );
	return r;
}

/*****************************************************
**
**   MRect   ---   intersect
**
******************************************************/
MRect& MRect::intersect( const MRect& rect )
{
	int x2 = getRight();
	int y2 = getBottom();

	if ( x < rect.x ) x = rect.x;
	if ( y < rect.y ) y = rect.y;
	if ( x2 > rect.getRight() ) x2 = rect.getRight();
	if ( y2 > rect.getBottom() ) y2 = rect.getBottom();

	width = x2 - x + 1;
	height = y2 - y + 1;

	if ( width <= 0 || height <= 0 )
	{
		width = 0;
		height = 0;
	}
	return *this;
}

/*****************************************************
**
**   MRect   ---   join
**
******************************************************/
MRect MRect::join( const MRect &r )
{
	if ( isNil()) return r;

	MRect rect( *this );
	if ( r.isNil()) return rect;

	rect.x = std::min( x, r.x );
	rect.width = std::max( getRight(), r.getRight()) - rect.x;
	rect.y = std::min( y, r.y );
	rect.height = std::max( getBottom(), r.getBottom()) - rect.y;
	//debuglog << "MRect::join this " << *this << " input rect " << r << " RESULT " << rect;
	return rect;
}

/*****************************************************
**
**   MRect   ---   containsPoint
**
******************************************************/
bool MRect::containsPoint( const MComplex &p ) const
{
	return (
		x <= p.real() && p.real() <= x + width
		&& y <= p.imag() && p.imag() <= y + height );
}

mostream& operator <<( mostream &o, const wxSize &s )
{
	o << "(" << s.GetWidth() << "/" << s.GetHeight() << ")";
	return o;
}

/*****************************************************
**
**   MBrush   ---   Constructor
**
******************************************************/
MBrush::MBrush( const wxString &filename, const int rotateHue )
	: style( wxBRUSHSTYLE_STIPPLE ), filename( filename ), rotateHue( rotateHue )
{
	if ( filename.IsEmpty())
	{
		warnlog << "invalid brush, stipple filename is empty: " << *this;
	}
}

/*****************************************************
**
**   MBrush   ---   Constructor
**
******************************************************/
MBrush::MBrush( const MBrush &tocopy )
{
	this->color = tocopy.color;
	this->style = tocopy.style;
	this->filename = tocopy.filename;
	this->rotateHue = tocopy.rotateHue;
}

/*****************************************************
**
**   MBrush   ---   operator=
**
******************************************************/
MBrush& MBrush::operator= ( const MBrush &t )
{
	this->color = t.color;
	this->style = t.style;
	this->filename = t.filename;
	this->rotateHue = t.rotateHue;
	return *this;
}

/*****************************************************
**
**   MBrush   ---   operator <<
**
******************************************************/
mostream& operator <<( mostream &o, const MBrush &b )
{
	wxString style;
	if ( mapping_list_brushstyle.find( b.style ) != mapping_list_brushstyle.end())
	{
		style = mapping_list_brushstyle.at( b.style );
	}
	else
	{
		style = std::format( "Unknown:{}", (int)b.style );
	}
	o << "Brush:{" << "{Style: " << style << "} ";

	if ( b.style == wxBRUSHSTYLE_STIPPLE )
	{
		o << "{Filename: " << b.filename << "} "
			<< "{Rotate Hue: " << b.rotateHue << "}}.";
	}
	else if ( b.style != wxBRUSHSTYLE_TRANSPARENT )
	{
		wxString color = b.color.IsOk() ?
			b.color.GetAsString( wxC2S_CSS_SYNTAX )
			: wxString( "<notokay>" );
		o << "{Color: " << color << "} ";
	}
	o << "}";
	return o;
}

/*****************************************************
**
**   MBrush   ---   isOk
**
******************************************************/
bool MBrush::isOk() const
{
	if ( style == wxBRUSHSTYLE_INVALID ) { return false; }
	else if ( style == wxBRUSHSTYLE_TRANSPARENT ) { return true; }
	else if ( style == wxBRUSHSTYLE_STIPPLE )
	{
		return ! filename.IsEmpty();
	}
	else
	{
		return color.IsOk();
	}
	assert( false );
}

/*****************************************************
**
**   softOverridePen
**
******************************************************/
bool softOverridePen( wxPen &pen, const wxPen &refPen )
{
	if ( ! pen.IsOk()) return false;
	if ( ! refPen.IsOk()) return false;

	bool isChanged = false;
	if ( refPen.GetColour().IsOk() )
	{
		isChanged = true;
		pen.SetColour( refPen.GetColour());
	}
  if ( refPen.GetWidth() > 0 )
	{
		isChanged = true;
		pen.SetWidth( refPen.GetWidth());
	}
  if ( refPen.GetStyle() >= 0 )
	{
		isChanged = true;
		pen.SetStyle( refPen.GetStyle());
	}
	return isChanged;
}

