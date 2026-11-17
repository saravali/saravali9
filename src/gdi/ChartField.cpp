/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/ChartField.cpp
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

#include "ChartField.h"

#include "astrobase.h"
#include "ColorConfig.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "Painter.h"
#include "DocHelper.h"
#include "Varga.h"

#include <math.h>

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   ChartField   ---   Constructor
**
******************************************************/
ChartField::ChartField( HybridChart *chart, const MRect &r, const Align &grav, const FieldType type )
	: chart( chart )
{
	rect = r;
	gravitation = grav;
	fieldtype = type;
	
	align = Align::Center;
	calculateAlign();
}

/*****************************************************
**
**   ChartField   ---   Constructor
**
******************************************************/
ChartField::ChartField( HybridChart *chart, const Align &grav, const FieldType type )
	: chart( chart )
{
	gravitation = grav;
	fieldtype = type;
	
	align = Align::Center;
	calculateAlign();
}

/*****************************************************
**
**   ChartField   ---   clear
**
******************************************************/
void ChartField::clear()
{
	contents.clear();
	tcontents.clear();
}

/*****************************************************
**
**   ChartField   ---   paint
**
******************************************************/
void ChartField::paint( Painter *painter, const bool doZoom )
{
	assert( painter );
	MRect rr = rect;

	if ( doZoom )
	{
		const double avgSize  = .5 * ( rr.width + rr.height );

		assert( chart );
		double newSize  = avgSize * chart->getChartConfig( chart->getWidgetCategory() )->zoomFactor;
		newSize /= 100.0;
		const double a = newSize - avgSize;
		rr.x -= .5 * a;
		rr.y -= .5 * a;
		rr.width += a;
		rr.height += a;
	}

	switch( fieldtype )
	{
		case FieldType::Full: // all fields in south indian chart, sbc, fixed rasis in east indian chart
			painter->drawRectangle( rr );
		break;

		case FieldType::NorthEast: // east: taurus and sag
		{
			std::vector<MComplex> mp =
			{
				MComplex( rr.x, rr.y ), 
				MComplex( rr.x + rr.width, rr.y ), 
				MComplex( rr.x  + rr.width, rr.y + rr.height ),
				//MComplex( rr.x, rr.y ) 
			}; 
			painter->drawPolygon( mp );
		}
		break;

		case FieldType::SouthWest: // east: gemini and scorpio
		{
			std::vector<MComplex> mp =
			{
				MComplex( rr.x, rr.y ), 
				MComplex( rr.x, rr.y + rr.height ), 
				MComplex( rr.x  + rr.width, rr.y + rr.height ),
				//MComplex( rr.x, rr.y )
			}; 
			painter->drawPolygon( mp );
		}
		break;

		case FieldType::NorthWest: // east: leo and pisces
		{
			std::vector<MComplex> mp =
			{
				MComplex( rr.x, rr.y ), 
				MComplex( rr.x, rr.y + rr.height ), 
				MComplex( rr.x  + rr.width, rr.y )
			}; 
			painter->drawPolygon( mp );
		}
		break;

		case FieldType::SouthEast: // east: virgo and aquarius
		{
			std::vector<MComplex> mp = {
				MComplex( rr.x, rr.y + rr.height ), 
				MComplex( rr.x + rr.width, rr.y ), 
				MComplex( rr.x  + rr.width, rr.y + rr.height )
			}; 
			painter->drawPolygon( mp );
		}
		break;

		case FieldType::North: // north: houses 2 and 12
		{
			std::vector<MComplex> mp = {
				MComplex( rr.x, rr.y ), 
				MComplex( rr.x + rr.width, rr.y ), 
				MComplex( rr.x  + 0.5 * rr.width, rr.y + rr.height )
			}; 
			painter->drawPolygon( mp );
		}
		break;

		case FieldType::West: // north: houses 3 and 5
		{
			std::vector<MComplex> mp = {
				MComplex( rr.x, rr.y ), 
				MComplex( rr.x, rr.y + rr.height ), 
				MComplex( rr.x  + rr.width, rr.y + 0.5 * rr.height )
			}; 
			painter->drawPolygon( mp );
		}
		break;

		case FieldType::South: // north: houses 6 and 8
		{
			std::vector<MComplex> mp = {
				MComplex( rr.x, rr.y + rr.height ), 
				MComplex( rr.x + rr.width, rr.y + rr.height ), 
				MComplex( rr.x  + 0.5 * rr.width, rr.y )
			}; 
			painter->drawPolygon( mp );
		}
		break;

		case FieldType::East: // north: houses 9 and 11
		{
			std::vector<MComplex> mp = {
				MComplex( rr.x + rr.width, rr.y ), 
				MComplex( rr.x + rr.width, rr.y + rr.height ), 
				MComplex( rr.x, rr.y + 0.5 * rr.height )
			}; 
			painter->drawPolygon( mp );
		}
		break;

		case FieldType::Diamond:// north: houses 1: 4: 7: 10
		{
			std::vector<MComplex> mp = {
				MComplex( rr.x + 0.5 * rr.width, rr.y ), 
				MComplex( rr.x + rr.width, rr.y + 0.5 * rr.height ), 
				MComplex( rr.x + 0.5 * rr.width, rr.y + rr.height ), 
				MComplex( rr.x, rr.y + 0.5 * rr.height )
			}; 
			painter->drawPolygon( mp );
		}
		break;

		default:
			errorlog << "invalid field type " << (int)fieldtype << " in ChartField::paintField";
			painter->drawRectangle( rr );
		break;
	}
}

/*****************************************************
**
**   ChartField   ---   drawText
**
******************************************************/
void ChartField::drawText( Painter *painter, const FieldPart part, const MComplex baseOuterMargin )
{

	const MString &tf = getContents( part == FieldPart::Outer ? chart->getSecondRole() : chart->getFirstRole() );
	if (! tf.isEmpty())
	{
		debuglog << "ChartField::drawText " << tf;
		MRect r = calculateModifiedRect( part, baseOuterMargin );

// DEBUG: draw border of the field rect
#if 0
		MColor color( part == FieldPart::Outer ? "yellow" : "green" );
		painter->setPenColor( color );
		painter->setTransparentBrush();
		painter->drawRectangle( r );
#endif

		ChartConfig *cfg = chart->getChartConfig( chart->getWidgetCategory());
		if ( cfg->showPlanetColors )
		{
			// TODO das geht gar nicht, weil die Planeten separate Farben brauchen.
			//painter->setTextColor( gdiconfig->chartColors.getPlanetColor( term.getFirstObject() )); }
		}
		constexpr MComplex marginExtent( 1, 1 );
		DocHelper( painter, marginExtent ).formatAndDrawTextItem( tf, r, align );
	}
}

/*****************************************************
**
**   ChartField   ---   drawLabel
**
******************************************************/
void ChartField::drawLabel( Painter *painter )
{
	debuglog << "ChartField::drawLabel " << label;
	if ( ! label.isEmpty() )
	{
		DocHelper( painter ).formatAndDrawTextItem( label, rect, Align::South );
	}
}

/*****************************************************
**
**   ChartField   ---   getContents
**
******************************************************/
MString &ChartField::getContents( const HoroscopeRole &role )
{
	assert( chart );
	return( role == chart->getFirstRole() ? contents : tcontents );
}

/*****************************************************
**
**   ChartContents   ---   isTriangle
**
******************************************************/
bool ChartField::isTriangle()
{
	return fieldtype == FieldType::NorthEast
		|| fieldtype == FieldType::SouthWest
		|| fieldtype == FieldType::NorthWest
		|| fieldtype == FieldType::SouthEast;
}

/*****************************************************
**
**   ChartField   ---   getCenter
**
******************************************************/
MComplex ChartField::getCenter( const FieldPart &part, const MComplex &pFieldSplit ) const
{
	if ( part == FieldPart::All )
	{
		return getCenter();
	}
	else
	{
		return calculateModifiedRect( part, pFieldSplit ).getCenter();
	}
}

/*****************************************************
**
**   ChartField   ---   getCenter
**
******************************************************/
MComplex ChartField::getCenter() const
{
	switch( fieldtype )
	{
		case FieldType::Full:
		case FieldType::Diamond:
			return MComplex( rect.x + 0.5 * rect.width, rect.y + 0.5 * rect.height );
		break;

		case FieldType::NorthEast:
			return MComplex( rect.x + 0.7 * rect.width, rect.y + 0.3 * rect.height );
		break;
		case FieldType::SouthWest:
			return MComplex( rect.x + 0.3 * rect.width, rect.y + 0.7 * rect.height );
		break;
		case FieldType::NorthWest:
			return MComplex( rect.x + 0.3 * rect.width, rect.y + 0.3 * rect.height );
		break;
		case FieldType::SouthEast:
			return MComplex( rect.x + 0.7 * rect.width, rect.y + 0.7 * rect.height );
		break;

		case FieldType::North:
		break;
		case FieldType::West:
		break;
		case FieldType::South:
		break;
		case FieldType::East: 
		break;
		default:
		break;
	}


	return MComplex( rect.x + 0.5 * rect.width, rect.y + 0.5 * rect.height );
}

/*****************************************************
**
**   ChartField   ---   containsPoint
**
******************************************************/
bool ChartField::containsPoint( const MComplex &p ) const
{
	if ( rect.isNil()) return false;
	//assert( rect.width );
	//assert( rect.height );

	if ( rect.containsPoint( p ))
	{
		const double x = ( p.real() - rect.x ) / rect.width;
		const double y = ( p.imag() - rect.y ) / rect.height;

		switch( fieldtype )
		{
			case ::FieldType::Full: // all fields in south indian chart, sbc, fixed rasis in east indian chart
				return true;
			break;

			case FieldType::NorthEast: // east: taurus and sag
				if ( x >= y ) return true;
			break;

			case FieldType::SouthWest: // east: gemini and scorpio
				if ( x <= y ) return true;
			break;

			case FieldType::NorthWest: // east: leo and pisces
				if ( 1 - x >= y ) return true;
			break;

			case FieldType::SouthEast: // east: virgo and aquarius
				if ( 1 - x <= y ) return true;
			break;

			case FieldType::North: // north: houses 2 and 12
				if ( fabs( x - .5 ) <= .5 * ( 1 - y )) return true;
			break;

			case FieldType::West: // north: houses 3 and 5
				if ( fabs( y - .5 ) <= .5 * ( 1 - x )) return true;
			break;

			case FieldType::South: // north: houses 6 and 8
				if ( fabs( x - .5 ) <= .5 * y ) return true;
			break;

			case FieldType::East: // north: houses 9 and 11
				if ( fabs( y - .5 ) <= .5 * x ) return true;
			break;

			case FieldType::Diamond:// north: houses 1: 4: 7: 10
				if ( .5 - fabs( x - .5 ) >= fabs( y - .5 )) return true;
			break;

			default:
				errorlog << "ChartField::containsPoint: invalid field type " << (int)fieldtype;
			break;
		}
	}
	return false;
}

/*****************************************************
**
**   ChartField   ---   calculateModifiedRect
**
******************************************************/
MRect ChartField::calculateModifiedRect( const FieldPart &mode, const MComplex dmargin ) const
{
	if ( mode == FieldPart::All ) return rect;

	MRect r = rect;

	switch( gravitation )
	{
		case Align::South: // e.g. diamond house 1, aries in east indian chart
			if ( mode == FieldPart::Outer )
			{
				r.height = dmargin.imag();
			}
			else // transit radix
			{
				r.y += dmargin.imag();
				r.height -= dmargin.imag();
			}
		break;
		case Align::SouthEast: // right lower, e.g. taurus and gemini in east indian chart
			if ( mode == FieldPart::Outer )
			{
				//r.width = dmargin.real();
				r.height = dmargin.imag();
				if ( fieldtype == FieldType::SouthWest )
				{
					r.y = dmargin.imag();
					r.width = dmargin.real();
				}
			}
			else // transit radix
			{
				r.x += dmargin.real();
				r.width -= dmargin.real();
				r.y += dmargin.imag();
				r.height -= dmargin.imag();
			}
		break;
		case Align::SouthWest: // left lower, e.g. aq and pisces in east indian chart, Gemini in south ind. chart
			if ( mode == FieldPart::Outer )
			{
				if ( fieldtype == FieldType::SouthEast )
				{
					r.y = dmargin.imag();
					r.height -= dmargin.imag();
					r.x = r.x + r.width - dmargin.real();
					r.width = dmargin.real();
				}
				else
				{
					r.height = dmargin.imag();
				}
			}
			else // transit radix
			{
				r.width -= dmargin.real();
				r.y += dmargin.imag();
				r.height -= dmargin.imag();
			}
		break;
		case Align::East: // left
			if ( mode == FieldPart::Outer )
			{
				r.width = dmargin.real();
			}
			else 
			{
				r.x += dmargin.real();
				r.width -= dmargin.real();
			}
		break;
		case Align::North:
			if ( mode == FieldPart::Outer )
			{
				r.y = r.y + r.height - dmargin.imag();
				r.height = dmargin.imag();
			}
			else // transit radix
			{
				r.height -= dmargin.imag();
			}
		break;
		case Align::NorthEast: // right upper: gemini in South Indian chart etc.
			/* einfach umgedreht: ist falsch
			if ( mode == FieldPart::Outer )
			{
				r.x += dmargin.real();
				r.width -= dmargin.real();
				r.height -= dmargin.imag();
			}
			else // transit radix
			{
				if ( fieldtype == FieldType::NorthWest )
				{
					r.height -= dmargin.imag();
					r.width = dmargin.real();
				}
				else
				{
					r.y += r.height - dmargin.imag();
					r.height = dmargin.imag();
				}
			}
			*/
			if ( mode == FieldPart::Outer )
			{
				if ( fieldtype == FieldType::NorthWest ) // east indian: taurus
				{
					r.height -= dmargin.imag();
					r.width = dmargin.real();
				}
				else
				{
					r.y += r.height - dmargin.imag();
					r.height = dmargin.imag();
				}
			}
			else // transit radix
			{
				r.x += dmargin.real();
				r.width -= dmargin.real();
				r.height -= dmargin.imag();
			}
		break;
		case Align::NorthWest: // left uuper
			if ( mode == FieldPart::Outer )
			{
				if ( fieldtype == FieldType::NorthEast )
				{
					r.x = r.x + r.width - dmargin.real();
					r.width = dmargin.real();
					r.height -= dmargin.imag();
				}
				else
				{
					r.y += r.height - dmargin.imag();
					r.height = dmargin.imag();
				}
			}
			else // transit radix
			{
				r.width -= dmargin.real();
				r.height -= dmargin.imag();
			}
		break;
		case Align::West:
			if ( mode == FieldPart::Outer )
			{
				r.x = r.x + r.width - dmargin.real();
				r.width = dmargin.real();
			}
			else // transit radix
			{
				r.width -= dmargin.real();
			}
		break;
		default:
			assert( false );
		break;
	}

	debuglog << "ChartField::calculateModifiedRect rect " << rect << " r " << r
		<< " gravitation " << (int)gravitation << " part " << (int)mode << " margin " << dmargin;
	return r;
}

/*****************************************************
**
**   ChartField   ---   calculateAlign
**
******************************************************/
void ChartField::calculateAlign()
{
	switch( fieldtype )
	{
		case FieldType::Full: // all fields in south indian chart, sbc, fixed rasis in east indian chart
			align = Align::Center;
		break;

		case FieldType::NorthEast: // east: taurus and sag
			align = Align::NorthEast;
		break;

		case FieldType::SouthWest: // east: gemini and scorpio
			align = Align::SouthWest;
		break;

		case FieldType::NorthWest: // east: leo and pisces
			align = Align::NorthWest;
		break;

		case FieldType::SouthEast: // east: virgo and aquarius
			align = Align::SouthEast;
		break;

		case FieldType::North: // north: houses 2 and 12
			align = Align::North;
		break;

		case FieldType::West: // north: houses 3 and 5
			align = Align::West;
		break;

		case FieldType::South: // north: houses 6 and 8
			align = Align::South;
		break;

		case FieldType::East: // north: houses 9 and 11
			align = Align::East;
		break;

		case FieldType::Diamond:// north: houses 1: 4: 7: 10
			align = Align::Center;
		break;

		default:
			warnlog << "invalid field type " << (int)fieldtype << " in ChartContents::calculateAlign";
			align = Align::Center;
		break;
	}
}



