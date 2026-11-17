/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/PdfPainter.cpp
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
#include "logger.h"
#include "mathbase.h"

#include <wx/dcmemory.h>
#include <wx/graphics.h>
#include <wx/image.h>
#include <wx/pdfdoc.h>

#include <format>

extern Gdiconfig *gdiconfig;

#define PDF_TEXT_ZOOMFACTOR .004
#define PDF_DEFAULT_FGCOLOR *wxBLACK
#define PDF_DEFAULT_BGCOLOR *wxWHITE
#define PDF_PEN_PENWIDTH_FACTOR .5
#define IMAGE_HASH_SIZE 10

#define DEBUG_PDFPAINTER

/*************************************************//**
*
*  Painter implementation for wx device context (GUI and file export via memory DC)
*
******************************************************/
class PdfPainter: public Painter
{
public:

	/*****************************************************
	**
	**   PdfPainter   ---   Constructor
	**
	******************************************************/
	PdfPainter( wxPdfDocument *pdfdoc ) : Painter( TextAnchorPoint::BottomLeft ), pdf( pdfdoc ) {}

	/*****************************************************
	**
	**   PdfPainter   ---   setBrush
	**
	******************************************************/
	virtual void setBrush( const MBrush &b ) override
	{
		#ifdef DEBUG_PDFPAINTER
		debuglog << "PdfPainter::setBrush " << b;
		#endif

		useStyleFill = false;
		switch( b.style )
		{
			case wxBRUSHSTYLE_SOLID:
				if ( b.color.IsOk() )
				{
					useStyleFill = true;
					pdf->SetFillColour( b.color );
				}
				else errorlog << "PdfPainter::setBrush color is not okay: " << b;
			break;
			case wxBRUSHSTYLE_STIPPLE:
			{
				useStyleFill = true;
				const wxImage image = ImageProvider::get()->getImage( b.filename, b.rotateHue );
				if ( image.IsOk())
				{
					double width = image.GetWidth() / 3;
					double height = image.GetHeight() / 3;

					// setup a hash value for the image
					const wxString hash = std::format( "{}_{}", b.filename.ToStdString(), b.rotateHue );
					debuglog << "PdfPainter::setBrush: image hash for " << b.filename << "/" << b.rotateHue << " is " << hash;
					if ( std::find( patterns.begin(), patterns.end(), hash ) == patterns.end())
					{
						pdf->AddPattern( hash, image, width, height );
						patterns.push_back( hash );
					}
					pdf->SetFillPattern( hash );
				}
				else errorlog << "bitmap in PdfPainter::setBrush not okay for brush " << b;
			}
			break;
			case wxBRUSHSTYLE_TRANSPARENT:
					useStyleFill = false;
			break;
			case wxBRUSHSTYLE_BDIAGONAL_HATCH:
				setHatchPattern( b.style, wxPDF_PATTERNSTYLE_BDIAGONAL_HATCH, b.color );
			break;
			case wxBRUSHSTYLE_CROSSDIAG_HATCH:
				setHatchPattern( b.style, wxPDF_PATTERNSTYLE_FDIAGONAL_HATCH, b.color );
			break;
			case wxBRUSHSTYLE_FDIAGONAL_HATCH:
				setHatchPattern( b.style, wxPDF_PATTERNSTYLE_CROSSDIAG_HATCH, b.color );
			break;
			case wxBRUSHSTYLE_CROSS_HATCH:
				setHatchPattern( b.style, wxPDF_PATTERNSTYLE_CROSS_HATCH, b.color );
			break;
			case wxBRUSHSTYLE_VERTICAL_HATCH:
				setHatchPattern( b.style, wxPDF_PATTERNSTYLE_VERTICAL_HATCH, b.color );
			break;
			case wxBRUSHSTYLE_HORIZONTAL_HATCH:
				setHatchPattern( b.style, wxPDF_PATTERNSTYLE_HORIZONTAL_HATCH, b.color );
			break;
			default:
			break;
		}
		if ( b.color.IsOk())
		{
			fillAlpha = b.color.Alpha();
		}
		else
		{
			fillAlpha = 255;
		}
		setAlphaValue();
	}

	/*****************************************************
	**
	**   PdfPainter   ---   setPen
	**
	******************************************************/
	virtual void setPen( const wxPen &p ) override
	{
		#ifdef DEBUG_PDFPAINTER
		debuglog << "PdfPainter::setPen OK " << p.IsOk()  << " width " << p.GetWidth() << " style " << pen.GetStyle()
			<< " color " << pen.GetColour().GetAsString();
		#endif
		if ( ! p.IsOk())
		{
			warnlog << "PdfPainter::setPen: cannot set pen";
		}
		double width = p.GetWidth() * PDF_PEN_PENWIDTH_FACTOR;

		wxPdfLineStyle style;
		switch ( p.GetStyle() )
		{
			case wxPENSTYLE_DOT:
			{
				wxPdfArrayDouble dash1;
				dash1.Add( .7 );
				style = wxPdfLineStyle( width, wxPDF_LINECAP_NONE, wxPDF_LINEJOIN_MITER, dash1, 10.);
			}
			break;
			case wxPENSTYLE_LONG_DASH:
			{
				wxPdfArrayDouble dash2;
				dash2.Add( 4 );
				style  = wxPdfLineStyle( width, wxPDF_LINECAP_BUTT, wxPDF_LINEJOIN_MITER, dash2, 0.);
			}
			break;
			case wxPENSTYLE_SHORT_DASH:
			{
				wxPdfArrayDouble dash3;
				dash3.Add( 2 );
				style  = wxPdfLineStyle( width, wxPDF_LINECAP_BUTT, wxPDF_LINEJOIN_MITER, dash3, 0.);
			}
			break;
			case wxPENSTYLE_DOT_DASH:
			{
				wxPdfArrayDouble dash4;
				dash4.Add( 1.5 );
				dash4.Add( 5 );
				dash4.Add( 1 );
				style  = wxPdfLineStyle( width, wxPDF_LINECAP_BUTT, wxPDF_LINEJOIN_MITER, dash4, 0.);
			}
			break;
			case wxPENSTYLE_SOLID:
			default:
			// Nothing to do
			break;
		}

		style.SetWidth( width );
		if ( p.IsOk() && p.GetColour().IsOk() && p.GetStyle() != wxPENSTYLE_TRANSPARENT )
		{
			useStyleDraw = true;
			style.SetColour( p.GetColour() );
			fillAlpha = p.GetColour().Alpha();
			pdf->SetDrawColour( p.GetColour());
		}
		else
		{
			useStyleDraw = false;
			fillAlpha = 255;
		}
		setAlphaValue();
		pdf->SetLineStyle( style );
	}

	/*****************************************************
	**
	**   PdfPainter   ---   drawRectangle
	**
	******************************************************/
	virtual void drawRectangle( const double &x, const double &y, const double &w, const double &h,
		const double rnd = 0 ) const override
	{
		const double r = 1.0;
		if ( rnd )
		{
			pdf->RoundedRect( x, y, w, h, r * rnd, wxPDF_CORNER_ALL, getDrawMode() );
		}
		else
		{
			pdf->Rect( x, y, w, h, getDrawMode() );
		}
	}

	/*****************************************************
	**
	**   PdfPainter   ---   drawPolygon
	**
	******************************************************/
	virtual void drawPolygon( const std::vector<MComplex> &points ) const override
	{
		// TODO die waren bei rel8 params
		constexpr int xoffset = 0;
		constexpr int yoffset = 0;

		if ( points.size() < 3 ) return;
		wxPdfArrayDouble x, y;
		//for ( int i = 0; i < n; i++ )
		for( const auto &p : points )
		{
			x.Add( p.real() + xoffset );
			y.Add( p.imag() + yoffset );
		}
		pdf->Polygon( x, y, getDrawMode() );
	}

	/*****************************************************
	**
	**   PdfPainter   ---   drawLine
	**
	******************************************************/
	virtual void drawLine( const double &x1, const double &y1, const double &x2, const double &y2 ) const override
	{
		pdf->Line( x1, y1, x2, y2 );
	}

	/*****************************************************
	**
	**   PdfPainter   ---   drawEllipse
	**
	******************************************************/
	virtual void drawEllipse( const double &x0, const double &y0, const double &xmax, const double &ymax ) const override
	{
		pdf->Ellipse( x0 + xmax/2 , y0 + ymax/2, xmax/2, ymax/2, 0, 0, 360, getDrawMode() ); // Cannot be filled
	}

	/*****************************************************
	**
	**   PdfPainter   ---   drawBitmap
	**
	******************************************************/
	virtual void drawBitmap( const wxBitmap &bitmap, const double &x0, const double &y0 ) const override
	{
		pdf->Image( "image", bitmap.ConvertToImage(), x0, y0 );
	}

	/*****************************************************
	**
	**   PdfPainter   ---   rawTextOut
	**
	******************************************************/
	virtual void rawTextOut( const double &x, const double &y, const wxString &t ) const override
	{
		double oldx = pdf->GetX();
		double oldy = pdf->GetY();
		pdf->Text( x, y, t );
		pdf->SetXY( oldx, oldy );
	}

	/*****************************************************
	**
	**   PdfPainter   ---   getTextExtent
	**
	******************************************************/
	virtual MComplex getTextExtent( wxString s ) const override
	{
		const double w = pdf->GetStringWidth( s );
		const double h = pdf->GetFontSize();
		return MComplex( w, h );
	}

//#define DRAW_TEXT_DEBUG_FRAME
	/*****************************************************
	**
	**   PdfPainter   ---   drawRotatedText
	**
	******************************************************/
	virtual void drawRotatedText( wxString s, const double &x, const double &y, const double &alfa ) const override
	{
		MComplex p = getTextExtent( s );

		const double w = .5 * p.real();
		const double h = .5 * p.imag();

		// in pdf mode the anchor is located at the left sid of the text. y value is in the middle
		const double px = x - w * cos( alfa * DEG2RAD ) +  h * sin( alfa * DEG2RAD );
		const double py = y + w * sin( alfa * DEG2RAD ) + h * cos( alfa * DEG2RAD );

		/*
		const int rw = 1;
		setDefaultPen();
		//drawRectangle( x - rw, y - rw, 2 * rw, 2 * rw );
		drawRectangle( px - rw, py - rw, 2 * rw, 2 * rw );
		*/

		pdf->RotatedText( px, py, s, alfa );
	}

protected:

	/*****************************************************
	**
	**   PdfPainter   ---   setHatchPattern
	**
	******************************************************/
	void setHatchPattern( const wxBrushStyle &style, const wxPdfPatternStyle &pstyle, const wxColour &color )
	{
		assert( mapping_list_brushstyle.find( style ) != mapping_list_brushstyle.end());
		const wxString name = mapping_list_brushstyle.at( style );
		if ( std::find( patterns.begin(), patterns.end(), name ) == patterns.end())
		{
			pdf->AddPattern( name, pstyle, 1, 2, color );
			patterns.push_back( name );
		}
		pdf->SetFillPattern( name );
		useStyleFill = true;
	}

	/*****************************************************
	**
	**   PdfPainter   ---   setFontInternal
	**
	******************************************************/
	virtual void setFontInternal( const wxFont &f ) override
	{
#if 0
		wxFont f2( f );
		f2.SetPointSize( f.GetPointSize() * 2 );
		pdf->SetFont( f2 );
#else
		pdf->SetFont( f );
#endif
		if ( fontColor.IsOk())
		{
			pdf->SetTextColour( fontColor );
		}
		else
		{
			warnlog << "PdfPainter::setFontInternall: fontcolor " << fontColor.GetAsString() << " is not OK";
		}
	}

	/*****************************************************
	**
	**   PdfPainter   ---   getDrawMode
	**
	******************************************************/
	int getDrawMode() const
	{
		if ( useStyleDraw )
		{
			return ( useStyleFill ? wxPDF_STYLE_FILLDRAW : wxPDF_STYLE_DRAW );
		}
		else
		{
			return ( useStyleFill ? wxPDF_STYLE_FILL : wxPDF_STYLE_NOOP );
		}
	}

	/*****************************************************
	**
	**   PdfPainter   ---   setAlphaValue
	**
	******************************************************/
	void setAlphaValue()
	{
		pdf->SetAlpha( ( lineAlpha + 1 ) / 256.0, ( fillAlpha + 1 ) / 256.0 );
	/*
			wxALPHA_TRANSPARENT = 0
			wxALPHA_OPAQUE = 0xff
			virtual int 	SetAlpha (double lineAlpha=1, double fillAlpha=1,
			0 (transparent) to 1 (opaque)
	*/
	}

	bool useStyleDraw = true;

	bool useStyleFill = true;

	uint lineAlpha = 255;

	uint fillAlpha = 255;

	std::list<wxString> patterns;

	wxPdfDocument *pdf;
};

/*****************************************************
**
**   PainterFactory   ---   createPdfPainter
**
******************************************************/
Painter *PainterFactory::createPdfPainter( wxPdfDocument *pdfdoc )
{
	return new PdfPainter( pdfdoc );
}

