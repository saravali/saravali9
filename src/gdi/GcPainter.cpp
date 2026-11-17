/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/GcPainter.cpp
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

#include "gdiconfig.h"
#include "ImageProvider.h"
#include "constants.h"
#include "mathbase.h"

#include <wx/graphics.h>
#include <wx/image.h>
#include <wx/dcmemory.h>

extern Gdiconfig *gdiconfig;

//#define DEBUG_GCPAINTER

/*************************************************//**
*
*  Painter implementation for wx device context (GUI and file export via memory DC)
*
******************************************************/
class GcPainter: public Painter
{
public:

	/*****************************************************
	**
	**   GcPainter   ---   Constructors
	**
	******************************************************/
	GcPainter( wxGraphicsContext *ctx )
		: Painter( TextAnchorPoint::TopLeft ),
		gc( ctx )
	{
	}

	/*****************************************************
	**
	**   GcPainter   ---   setBrush
	**
	******************************************************/
	virtual void setBrush( const MBrush &brush ) override
	{
#ifdef DEBUG_GCPAINTER
		debuglog << "GcPainter::setBrush okay " << std::boolalpha << brush.isOk() << " params: " << brush;
#endif
		if ( ! brush.isOk() )
		{
			warnlog << "GcPainter::setBrush: brush " << brush << " not okay, using default brush";
			setDefaultBrush();
			return;
		}
		if ( brush.style == wxBRUSHSTYLE_STIPPLE )
		{
			ImageProvider *ip = ImageProvider::get();
			gc->SetBrush( wxBitmap( ip->getImage( brush.filename, brush.rotateHue )));
		}
		else
		{
			gc->SetBrush( wxBrush( brush.color, brush.style ));
		}
	}

	/*****************************************************
	**
	**   GcPainter   ---   setPen
	**
	******************************************************/
	virtual void setPen( const wxPen &pen ) override
	{
		gc->SetPen( pen );
	}

	/*****************************************************
	**
	**   GcPainter   ---   drawRectangle
	**
	******************************************************/
	virtual void drawRectangle( const double &x, const double &y, const double &w, const double &h,
		const double rnd = 0 ) const override
	{
		if ( rnd == 0 )
			gc->DrawRectangle( x,  y, w, h );
		else
			gc->DrawRoundedRectangle( x, y, w, h, rnd );
	}

	/*****************************************************
	**
	**   GcPainter   ---   drawPolygon
	**
	******************************************************/
	virtual void drawPolygon( const std::vector<MComplex> &points ) const override
	{
		if ( points.size() < 3 ) return;

		wxGraphicsPath path = gc->CreatePath();
		path.MoveToPoint( points[0]. real(), points[0].imag() );
		for( uint i = 1; i < points.size(); i++ )
		{
			path.AddLineToPoint( points[i].real(), points[i].imag());
		}
		gc->DrawPath( path );
	}

	/*****************************************************
	**
	**   GcPainter   ---   drawLine
	**
	******************************************************/
	virtual void drawLine( const double &x1, const double &y1, const double &x2, const double &y2 ) const override
	{
		gc->StrokeLine( x1, y1, x2, y2 );
	}

	/*****************************************************
	**
	**   GcPainter   ---   drawEllipse
	**
	******************************************************/
	virtual void drawEllipse( const double &x0, const double &y0, const double &xmax, const double &ymax ) const override
	{
		gc->DrawEllipse( x0, y0, xmax, ymax );
	}

	/*****************************************************
	**
	**   GcPainter   ---   drawBitmap
	**
	******************************************************/
	virtual void drawBitmap( const wxBitmap &bitmap, const double &x0, const double &y0 ) const override
	{
		gc->DrawBitmap( bitmap, x0, y0, bitmap.GetWidth(), bitmap.GetHeight() );
	}

	/*****************************************************
	**
	**   GcPainter   ---   rawTextOut
	**
	******************************************************/
	virtual void rawTextOut( const double &x, const double &y, const wxString &t ) const override
	{
#ifdef DEBUG_GCPAINTER
		debuglog << "GcPainter::rawTextOut x " << x << " y " << y << " font size " << currentFont.GetPointSize()
			<< " contents'" << t << "'";
#endif
		gc->DrawText( t, x, y );
	}

	/*****************************************************
	**
	**   GcPainter   ---   getTextExtent
	**
	******************************************************/
	virtual MComplex getTextExtent( wxString s ) const override
	{
		wxDouble width = 0;
		wxDouble height = 0;
		wxDouble descent = 0;
		wxDouble externalLeading = 0;

		gc->GetTextExtent( s, &width, &height, &descent, &externalLeading );
#ifdef DEBUG_GCPAINTER
		debuglog << "getTextExtent " << s
			<< " width " << width
			<< " height " << height
			<< " descent " << descent
			<< " externalLeading " << externalLeading;
#endif
		return MComplex( width, height - descent );
	}

//#define DRAW_TEXT_DEBUG_FRAME
	/*****************************************************
	**
	**   GcPainter   ---   drawRotatedText
	**
	******************************************************/
	virtual void drawRotatedText( wxString s, const double &x, const double &y, const double &alfa ) const override
	{
		MComplex p = getTextExtent( s );

	#ifdef DRAW_TEXT_DEBUG_FRAME
		debuglog << "GcPainter::drawRotatedText " << s << " p " << p;
		const double a = 2;
		gc->DrawRectangle( x - .5 * p.real(), y - .5 * p.imag(), p.real(), p.imag());
		gc->DrawRectangle( x - a, y - a, 2 * a, 2 * a );
	#endif

		const double w = .5 * p.real();
		const double h = .5 * p.imag();
		//const double newalfa = red_deg( alfa - radius / deg2pihalbe * w );

		/*
		TODO das ist falsch.
		Man muss auf dem bogen um die halbe textlänge zurückgehen um den Winkel zu bestimmen
		Doch woher weiss man den Radius??
		Den muss man von aussen mitgeben!

		*/

		const double px = x - w * cos( alfa * DEG2RAD ) -  h * sin( alfa * DEG2RAD );
		const double py = y + w * sin( alfa * DEG2RAD ) - h * cos( alfa * DEG2RAD );

	#ifdef DRAW_TEXT_DEBUG_FRAME
		gc->DrawRectangle( px - a, py - a, 2 * a, 2 * a );
	#endif
		//gc->DrawRotatedText( s, px, py, alfa );
		//gc->DrawText( s, px, py, alfa );

		gc->DrawText( s, px, py, DEG2RAD * alfa );
	}

protected:

	/*****************************************************
	**
	**   GcPainter   ---   setFontInternal
	**
	******************************************************/
	virtual void setFontInternal( const wxFont &f ) override
	{
#ifdef DEBUG_GCPAINTER
		debuglog << "GcPainter::setFontInternal " << f.GetFaceName() << " size " << f.GetPointSize();
#endif
		gc->SetFont( f, fontColor );
	}

private:

	wxGraphicsContext *gc;
};

/*****************************************************
**
**   PainterFactory   ---   createGcPainter
**
******************************************************/
Painter *PainterFactory::createGcPainter( wxGraphicsContext *ctx )
{
	return new GcPainter( ctx );
}


