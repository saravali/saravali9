/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/Painter.h
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

#ifndef _PAINTER_H_
#define _PAINTER_H_

#include "gdibase.h"
#include "MDocument.h" // TextItemSubType
#include "mstring.h"

class MImage;
class wxBitmap;
class wxGraphicsContext;
class wxPdfDocument;

#include <vector>
#include <wx/font.h>

enum class TextAnchorPoint { TopLeft, BottomLeft };

/*************************************************//**
*
*  abstract class for painting graphical items and text on a GUI.
*
* There are two standard implementations:
* - GcPainter drawing on a wxGC context
* - PdfPainter using wxpdfdoc
*
******************************************************/
class Painter
{
public:

	Painter( const TextAnchorPoint& );

	virtual ~Painter() {}

	// get defaults
	static wxPen getDefaultPen();
	static MBrush getDefaultBrush();
	static wxColour getDefaultTextColor();

	// get defaults
	void setDefaultPen() { setPen( getDefaultPen()); }
	void setDefaultBrush() { setBrush( getDefaultBrush()); }
	void setDefaultTextColor() { setTextColor( getDefaultTextColor()); }

	// call all three methods above
	void setDefaults();

	// pen
	virtual void setPen( const wxPen& ) = 0;
	void setPen( const wxColour& );
	void setTransparentPen();
	void setPenColor( const wxColour& );
	void setPenWidth( const int& );
	wxColour getPenColor();

	// brush
	virtual void setBrush( const MBrush& ) = 0;
	void setTransparentBrush();
	virtual void setBrush( const wxColour& );

	// drawing methods
	virtual void drawRectangle( const double &x, const double &y, const double &w, const double &h, const double = 0 ) const = 0;
	void drawRectangle( const MRect&, const double = 0 ) const;

	virtual void drawPolygon( const std::vector<MComplex>& ) const = 0;
	virtual void drawLine( const double &x1, const double &y1, const double &x2, const double &y2 ) const = 0;
	void drawLine( const MComplex &p1, const MComplex &p2 ) const;

	virtual void drawEllipse( const double &x0, const double &y0, const double &xmax, const double &ymax ) const = 0;

	void drawArrow( const MComplex &p, const MComplex &q, const double arrowSize = 10, const bool fill = true );
	void drawCrossMarker( const MComplex &p, const double w = 5 ) const;

	// draw images
	virtual void drawBitmap( const wxBitmap&, const double &x, const double &y ) const = 0;

	// text
	virtual void rawTextOut( const double &x, const double &y, const wxString& ) const = 0;
	virtual void drawRotatedText( wxString /*text*/, const double& /*x*/,
		const double& /*y*/, const double& /*angle*/ ) const = 0;

	virtual MComplex getTextExtent( wxString ) const = 0;
	MComplex getTextExtent( const char ) const;

	// extent of sample character, internally stored until font is changes
	MComplex getCharacterExtent();

	// fonts and text properties
	void setTextColor( const wxColour& );
	void setFont( const TextItemSubType &type, const int &basicFontSize );

	[[deprecated]]
	void setFont( const wxFont& );

	int getFontSize() const { return currentFont.GetPointSize(); }
	wxFont getFont() const { return currentFont; }

	void setGraphicFont( const int pointSize = 0 );
	void setSymbolFont( const int pointSize = 0 );

	/**
	*  switch between graphic and symbol font e.g. in charts.
	* 
	* The size of the text will be kept.
	*/
	void switchToSymbolFont( const bool symbolMode = true, const double zoom = 1.0 );

	/**
	*  text anchor point determines whether the x/y coordinate for text output is located relative to the text item.
	*
	* Used values are
	* - TopLeft corner for wxGC painter
	* - BottomLeft not relevant anymore?
	*
	*/
	virtual TextAnchorPoint getTextAnchorPoint() const { return textAnchor; }

protected:

	virtual void setFontInternal( const wxFont& ) = 0;

	const TextAnchorPoint textAnchor;

	wxPen pen;

	wxColour fontColor;

	wxFont currentFont;

	wxFont graphicFont;

	MComplex characterExtent = 0;
};

/**************************************************************
***
**  Painter creation
***
***************************************************************/
class PainterFactory
{
public:

	// impl in PdfPainter.cpp
	Painter *createPdfPainter( wxPdfDocument* );

	// test for wxPdfDC
	Painter *createGcPainter( wxGraphicsContext* );

};

#endif

