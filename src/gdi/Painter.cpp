/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/Painter.cpp
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

#include "Painter.h"

#include "FontTool.h"
#include "gdiconfig.h"

extern Gdiconfig *gdiconfig;

//#define DEBUG_PAINTER

/*****************************************************
**
**   Painter   ---   Constructor
**
******************************************************/
Painter::Painter( const TextAnchorPoint &anchor )
	: textAnchor( anchor )
{
	pen = gdiconfig->colors.defaultPen;
	graphicFont = FontTool().getWxFontForItem( TextItemSubType::Line, gdiconfig->fonts.graphicFontSize );
	fontColor = gdiconfig->colors.textFgColor;
}

/*****************************************************
**
**   Painter   ---   getPenColor
**
******************************************************/
wxColour Painter::getPenColor() 
{
	return pen.GetColour();
}

/*****************************************************
**
**   Painter   ---   setPenColor
**
******************************************************/
void Painter::setPenColor( const wxColour &c )
{
	if ( ! c.IsOk() )
	{
		warnlog <<  "pen color not okay, using default pen" << c.GetAsString();
		setDefaultPen();
	}
	else
	{
		pen.SetColour( c );
		if ( pen.GetStyle() != wxPENSTYLE_STIPPLE )
		{
			pen.SetStyle( wxPENSTYLE_SOLID );
		}
		setPen( pen );
	}
}

/*****************************************************
**
**   Painter   ---   setPenWidth
**
******************************************************/
void Painter::setPenWidth( const int &w )
{
	pen.SetWidth( w );
	setPen( pen );
}

/*****************************************************
**
**   Painter   ---   setPen
**
******************************************************/
void Painter::setPen( const wxColour &color )
{
	setPen( wxPen( color ));
}

/*****************************************************
**
**   Painter   ---   getDefaultPen
**
******************************************************/
wxPen Painter::getDefaultPen()
{
#if 1
	return gdiconfig->colors.getDefaultPen();
#else
	if ( gdiconfig->colors.fgColor.IsOk() )
	{
		return wxPen( gdiconfig->colors.fgColor, 1, wxPENSTYLE_SOLID );
	}
	warnlog << "Painter::getDefaultPen: foreground color is not okay, using fallback (black)";
	return *wxBLACK_PEN;
#endif
}

/*****************************************************
**
**   Painter   ---   setTransparentPen
**
******************************************************/
void Painter::setTransparentPen()
{
	setPen( *wxTRANSPARENT_PEN );
}

/*****************************************************
**
**   Painter   ---   Brush
**
******************************************************/
void Painter::setBrush( const wxColour &color )
{
	setBrush( MBrush( color ));
}

void Painter::setTransparentBrush()
{
	setBrush( MBrush ( wxNullColour, wxBRUSHSTYLE_TRANSPARENT ));
}

MBrush Painter::getDefaultBrush()
{
	if ( gdiconfig->colors.bgColor.IsOk() )
	{
		return MBrush( gdiconfig->colors.bgColor, wxBRUSHSTYLE_SOLID );
	}
	warnlog << "Painter::getDefaultBrush: background color is not okay, using fallback (white)";
	return MBrush( *wxWHITE );
}

/*****************************************************
**
**   Painter   ---   setDefaults
**
******************************************************/
void Painter::setDefaults()
{
	setDefaultPen();
	setDefaultBrush();
	setTextColor( getDefaultTextColor());
}

/*****************************************************
**
**   Painter   ---   drawRectangle
**
******************************************************/
void Painter::drawRectangle( const MRect &r, const double rnd ) const
{
	drawRectangle( r.x, r.y, r.width, r.height, rnd );
}

/*****************************************************
**
**   Painter   ---   drawLine
**
******************************************************/
void Painter::drawLine( const MComplex &p1, const MComplex &p2 ) const
{
	drawLine( p1.real(), p1.imag(), p2.real(), p2.imag() );
}

/*****************************************************
**
**   Painter   ---   drawArrow
**
******************************************************/
void Painter::drawArrow( const MComplex &p, const MComplex &q, const double arrowSize, const bool /*fill*/ )
{
	if ( p == q ) return;

	drawLine( p, q  );

	// length factor for the intersection point of arrow and line
	const double lambda = arrowSize / abs( q - p );

	// point on the line a root for the arrow
	const MComplex a =  lambda * p + ( 1 - lambda ) * q;

	const MComplex b = findOrthogonalPoint( a - q, arrowSize );

  std::vector<MComplex> mp = { q, a + b, a - b, q };

	// TODO fill

	setBrush( MBrush( getPenColor()));

	//const MBrush oldbrush = getBrush();
	//if ( fill ) setBrush( getPen().color );
	drawPolygon( mp );
	setTransparentBrush();
	//setBrush( oldbrush );
}

/*****************************************************
**
**   Painter   ---   drawCrossMarker
**
******************************************************/
void Painter::drawCrossMarker( const MComplex &p, const double w ) const
{
	drawLine( p.real() - w, p.imag() - w, p.real() + w, p.imag() + w );
	drawLine( p.real() - w, p.imag() + w, p.real() + w, p.imag() - w );
}

/*****************************************************
**
**   Painter   ---   setTextColor
**
******************************************************/
void Painter::setTextColor( const wxColour& c )
{
	fontColor = c;
	setFontInternal( currentFont );
}

/*****************************************************
**
**   Painter   ---   getDefaultTextColor
**
******************************************************/
wxColour Painter::getDefaultTextColor()
{
	if ( gdiconfig->colors.textFgColor.IsOk() )
	{
		return gdiconfig->colors.textFgColor;
	}
	warnlog << "Painter::getDefaultTextColor: color is not okay, using fallback (black)";
	return *wxBLACK;
}

/*****************************************************
**
**   Painter   ---   switchToSymbolFont
**
******************************************************/
void Painter::switchToSymbolFont( const bool symbolMode, const double zoom )
{
#ifdef DEBUG_PAINTER
	debuglog << "Painter::switchToSymbolFont " << symbolMode;
#endif

	if ( symbolMode )
	{
		wxFont symbolFont;
		symbolFont.SetFaceName( gdiconfig->fonts.symbolFontFacename );
		symbolFont.SetPointSize( (int)( graphicFont.GetPointSize() * zoom ));
		currentFont = symbolFont;
		setFontInternal( symbolFont );
	}
	else
	{
		currentFont = graphicFont;
		setFontInternal( graphicFont );
	}
}

/*****************************************************
**
**   Painter   ---   setGraphicFont
**
******************************************************/
void Painter::setGraphicFont( const int pointSize )
{
	const int size = pointSize ? pointSize : gdiconfig->fonts.graphicFontSize;
#ifdef DEBUG_PAINTER
	debuglog << "Painter::setGraphicFont pointSize " << pointSize << " size " << size
		<< " facename " << gdiconfig->fonts.graphicFontFacename;
#endif
	wxFont font;
	font.SetFaceName( gdiconfig->fonts.graphicFontFacename );
	font.SetPointSize( size );
	currentFont = font;
	graphicFont = font;
	characterExtent = 0;
	setFontInternal( font );
}

/*****************************************************
**
**   Painter   ---   setSymbolFont
**
******************************************************/
void Painter::setSymbolFont( const int pointSize )
{
	const int size = pointSize ? pointSize : gdiconfig->fonts.symbolFontSize;
	wxFont font;
	font.SetFaceName( gdiconfig->fonts.symbolFontFacename );
	font.SetPointSize( size );
	currentFont = font;
	setFontInternal( font );
}

/*****************************************************
**
**   Painter   ---   setFont
**
******************************************************/
void Painter::setFont( const TextItemSubType &type, const int &basicFontSize ) 
{
	//wxFont font = FontTool().getWxFontForItem( type, gdiconfig->fonts.graphicFontSize );
	characterExtent = 0;
	wxFont font = FontTool().getWxFontForItem( type, basicFontSize );
	graphicFont = font;
	currentFont = font;
	setFontInternal( font );
}

/*****************************************************
**
**   Painter   ---   setFont
**
******************************************************/
void Painter::setFont( const wxFont &font )
{
	graphicFont = font;
	currentFont = font;
	characterExtent = 0;
	setFontInternal( font );
}

/*****************************************************
**
**   Painter   ---   getCharacterExtent
**
******************************************************/
MComplex Painter::getCharacterExtent()
{
	if ( characterExtent == MComplex( 0 ))
	{
		characterExtent = getTextExtent( "a" );
	}
	return characterExtent;
}

/*****************************************************
**
**   Painter   ---   getTextExtent
**
******************************************************/
MComplex Painter::getTextExtent( const char c ) const
{
	wxString sc( 1, c );
	return getTextExtent( sc );
}


