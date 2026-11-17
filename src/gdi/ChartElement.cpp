/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/ChartElement.cpp
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

#include "ChartElement.h"

#include "ChartGeometry.h"
#include "ContextProperties.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "mathbase.h"
#include "Painter.h"

#include <math.h>

//#define DEBUG_CHARTELEMENT

/*****************************************************
**
**   ChartElementBase   ---  Constructor
**
******************************************************/
ChartElementBase::ChartElementBase( HybridChart *chart )
	: chart( chart )
{
	assert( chart );
	drawStackPosition = 0;
}

/*****************************************************
**
**   ChartElementBase   ---   getPen
**
******************************************************/
wxPen ChartElementBase::getPen() const
{
#ifdef DEBUG_CHARTELEMENT
	debuglog << "ChartElementBase::getPen ok1 " << getCgPen().IsOk();
#endif

	wxPen pen = chart->getSkinnedPen();
	softOverridePen( pen, getCgPen());
	return pen;
}

/*****************************************************
**
**   ChartElementBase   ---   getBrush
**
******************************************************/
MBrush ChartElementBase::getBrush( const bool inheritFromSkin ) const
{
	if ( getCgBrush().isOk()) return getCgBrush();
	else
	{
		if ( inheritFromSkin )
		{
			return chart->getSkinnedBrush();
		}	
		else
		{
			//return MBrush();
			return MBrush( wxNullColour, wxBRUSHSTYLE_TRANSPARENT );
		}
	}
}

/*****************************************************
**
**   ChartElementBase   ---   getTextColor
**
******************************************************/
wxColour ChartElementBase::getTextColor() const
{
	if ( getCgTextColor().IsOk() ) return getCgTextColor();
	else return chart->getSkinnedTextColor();
}

/*****************************************************
**
**   ChartElementBase   ---   setInheritedStyle
**
******************************************************/
void ChartElementBase::setInheritedStyle( Painter *painter, const bool inheritBrushFromSkin )
{
	painter->setPen( getPen() );
	painter->setBrush( getBrush( inheritBrushFromSkin ));
	painter->setTextColor( getTextColor());
}

/*****************************************************
**
**   ChartElementBase   ---   discPointToXy
**
******************************************************/
MComplex ChartElementBase::discPointToXy( const double &angle, const double &radius, const ChartShape &shape )
{
#ifdef DEBUG_CHARTELEMENT
	debuglog << "ChartElementBase::discPointToXy angle " << angle << " center " << chart->getCenter()
		<< " xymax " << chart->getXyMax();
#endif

	const double alfa = red_deg( angle );
	const double a = alfa * M_PI / 180;
	double lambda;

	const MComplex center = chart->getCenter();
	const MComplex xymax = chart->getXyMax();
	const double rmax = chart->getRMax();

	assert( chart );
	const ChartShape geom = ( shape == ChartShape::Inherited ? chart->getGeometry().geometryType : shape );

	switch( geom )
	{
		case ChartShape::Circle:
		{
			const double r = rmax * radius / 100.0;
			return MComplex( center.real() + r * cos( alfa * M_PI / 180 ),
				center.imag() - r * sin( alfa * M_PI / 180 ) );
		}
		break;
		case ChartShape::Ellipse:
		{
			const double xradius = xymax.real() * radius / 100.0;
			const double yradius = xymax.imag() * radius / 100.0;
			return MComplex( center.real() + xradius * cos( alfa * M_PI / 180 ),
				center.imag() - yradius * sin( alfa * M_PI / 180 ) );
		}
		break;
		case ChartShape::Square:
		{
			const double r = rmax * radius / 100.0;
			if ( alfa > 315 || alfa <= 45 )
			{
				return MComplex( center.real() + r, center.imag() - SQRT2 * r * sin( a ) );
			}
			else if ( alfa < 135 )
			{
				return MComplex( center.real() + SQRT2 * r * cos( a ), center.imag() - r );
			}
			else if ( alfa < 225 )
			{
				return MComplex( center.real() - r, center.imag() - SQRT2 * r * sin( a ) );
			}
			else // 225 < alfa < 315
			{
				return MComplex( center.real() + SQRT2 * r * cos( a ), center.imag() + r );
			}
		}
		break;

		case ChartShape::Rectangle:
		{
			const double xradius = xymax.real() * radius / 100.0;
			const double yradius = xymax.imag() * radius / 100.0;
			if ( alfa > 315 || alfa <= 45 ) // right part
			{
				return MComplex( center.real() + xradius, center.imag() - SQRT2 * yradius * sin( a ) );
			}
			else if ( alfa < 135 ) // upper part
			{
				return MComplex( center.real() + SQRT2 * xradius * cos( a ), center.imag() - yradius );
			}
			else if ( alfa < 225 ) // left part
			{
				return MComplex( center.real() - xradius, center.imag() - SQRT2 * yradius * sin( a ) );
			}
			else // 225 < alfa < 315 -> bottom part
			{
				return MComplex( center.real() + SQRT2 * xradius * cos( a ), center.imag() + yradius );
			}
		}
		break;
		
		case ChartShape::Rect2:
		{
			const double xradius = xymax.real() * radius / 100.0;
			const double yradius = xymax.imag() * radius / 100.0;
			if ( alfa > 315 || alfa <= 45 ) // right part
			{
				return MComplex( center.real() + xradius, center.imag() - SQRT2 * yradius * sin( a ) );
			}
			else if ( alfa < 135 ) // upper part
			{
				return MComplex( center.real() + SQRT2 * xradius * cos( a ), center.imag() - yradius );
			}
			else if ( alfa < 225 ) // left part
			{
				return MComplex( center.real() - xradius, center.imag() - SQRT2 * yradius * sin( a ) );
			}
			else // 225 < alfa < 315 -> bottom part
			{
				const double x0 = center.real() - xradius;
				const double xw = 2 * xradius;
				const double d = ( alfa - 225 ) / 45;
				return MComplex( x0 + d * xw, center.imag() + yradius );
			}
		}
		break;

		case ChartShape::Triangle:
		{
			const double xradius = xymax.real() * radius / 100.0;
			const double yradius = xymax.imag() * radius / 100.0;

			const MComplex pt( center.real(), center.imag() - yradius );
			const MComplex pl( center.real() - xradius / SQRT2, center.imag() + yradius / SQRT2 );
			const MComplex pr( center.real() + xradius / SQRT2, center.imag() + yradius / SQRT2 );

			if ( alfa > 330 || alfa < 90 )
			{
				lambda = red_deg( 90 - alfa ) / 120.0;
				return lambda * pr + ( 1 - lambda ) * pt;
			}
			else if ( alfa < 210 )
			{
				lambda = ( 210 - alfa ) / 120.0;
				return lambda * pt + ( 1 - lambda ) * pl;
			}
			else
			{
				lambda = red_deg( 330 - alfa ) / 120.0;
				return lambda * pl + ( 1 - lambda ) * pr;
			}
		}
		break;

		default:
			warnlog << "ChartElementBase::discPointToXy Invalid chart border types "
				<< (int)chart->getGeometry().geometryType << "/" << (int)shape;
			assert( false );
		break;
	}
	return 0.0;
}

/*****************************************************
**
**   ChartElementBase   ---   discPointToXy
**
******************************************************/
MComplex ChartElementBase::discPointToXy( const double &angle,  CgDisc *disc )
{
	assert( disc );
	return discPointToXy( angle, disc->radius, disc->geometryType );	
}

/*****************************************************
**
**   ChartElementBase   ---   paintDisc
**
******************************************************/
void ChartElementBase::paintDisc( Painter *painter, CgDisc *disc )
{
	paintDisc( painter, *disc );
}

/*****************************************************
**
**   ChartElementBase   ---   paintDisc
**
******************************************************/
void ChartElementBase::paintDisc( Painter *painter, CgDisc disc )
{
	if ( ! disc.show ) return;
	assert( painter );

	const MComplex center = chart->getCenter();
	const MComplex xymax = chart->getXyMax();
	const double rmax = chart->getRMax();
#ifdef DEBUG_CHARTELEMENT
	debuglog << "ChartElementBase::paintDisc center " << center << " yxmax "
		<< xymax << " rmax " << rmax << " disc.radius " << disc.radius;
#endif

	const ChartGeometry &parentGeom = chart->getGeometry();
	const ChartShape geom = ( disc.geometryType == ChartShape::Inherited ? parentGeom.geometryType : disc.geometryType );

	//painter->setTransparentBrush();
	//painter->setDefaults();

	switch( geom )
	{
		case ChartShape::Circle:
		{
			const double r = rmax * disc.radius / 100.0;
			painter->drawEllipse( center.real() - r, center.imag() - r, 2 * r, 2 * r );
			//debuglog << "->drawcircle( " << center.real() << " rad " <<  disc.radius << " " << center.imag() << " r " << r;
		}
		break;
		case ChartShape::Ellipse:
		{
			const double xradius = xymax.real() * disc.radius / 100;
			const double yradius = xymax.imag() * disc.radius / 100;
			painter->drawEllipse( center.real() - xradius, center.imag() - yradius, 2 * xradius, 2 * yradius );
			//debuglog << "->drawEllipse( " << center.real() << " " <<  xradius << " y " << yradius << " " << center.imag();
		}
		break;
		case ChartShape::Square:
		{
			const double r = rmax * disc.radius / 100.0;
			painter->drawRectangle( center.real() - r, center.imag() - r, 2 * r, 2 * r );
		}
		break;
		case ChartShape::Rect2:
		case ChartShape::Rectangle:
		{
			const double xradius = xymax.real() * disc.radius / 100;
			const double yradius = xymax.imag() * disc.radius / 100;
			painter->drawRectangle( center.real() - xradius, center.imag() - yradius, 2 * xradius, 2 * yradius );
		}
		break;
		case ChartShape::Triangle:
		{
			std::vector<MComplex> points;
			points.push_back( discPointToXy( 90, &disc ));
			points.push_back( discPointToXy( 210, &disc ));
			points.push_back( discPointToXy( 330, &disc ));
			painter->drawPolygon( points );
		}
		break;
		default:
			assert( false );
		break;
	}
}


/*****************************************************
**
**   ChartElementBase   ---   drawAnnulusSegment
**
******************************************************/
void ChartElementBase::drawAnnulusSegment( Painter *painter, CgAnnulus *annulus,
	const double &alfa0, const double &alfa1 )
{
	int i;
	const double a0 = red_deg( alfa0 );
	const double a1 = red_deg( alfa1 );

#ifdef DEBUG_CHARTELEMENT
	debuglog << "CeLabelledAnnulus::drawAnnulusSegment " << a0 << " " << a1;
#endif
	ASSERT_VALID_DEGREE( a0 );
	ASSERT_VALID_DEGREE( a1 );

	const ChartGeometry &parentGeom = chart->getGeometry();
	const ChartShape geom = ( annulus->geometryType == ChartShape::Inherited ? parentGeom.geometryType
		: annulus->geometryType );
	switch( geom )
	{
		case ChartShape::Circle:
		case ChartShape::Ellipse:
		{
			// number of steps depends upon the to total angle
			const double atotal = red_deg( a1 - a0 );
			const int nbsteps = atotal/5;
			const int ntotal =  2 * ( nbsteps + 1 );
			std::vector<MComplex> points;
			double a = a0;
			const double astep = red_deg( a1 - a0 ) / nbsteps;

			for ( i = 0; i <= nbsteps; i++ )
			{
				//points.push_back( discPointToXy( a, annulus->innerDisc() ));
				points.push_back( discPointToXy( a, annulus->innerRadius, annulus->geometryType ));
				a += astep;
			}
			a = a1;
			for ( i = nbsteps + 1; i < ntotal; i++ )
			{
				//points.push_back( discPointToXy( a, *annulus ));
				points.push_back( discPointToXy( a, annulus->radius, annulus->geometryType ));
				a -= astep;
			}
			painter->drawPolygon( points );
		}
		break;

		case ChartShape::Square:
		case ChartShape::Rectangle:
		case ChartShape::Rect2:
		{
			const int quadrant0 = (int)((a0 + 45 ) / 90 );
			const int quadrant1 = (int)((a1 + 45 ) / 90 );
			const bool qdrchange = (bool)(quadrant0 != quadrant1 );
			const double qdrcorner = 90 * (int)( std::max( a0, a1 ) / 90 ) + 45;

			std::vector<MComplex> points;

			points.push_back( discPointToXy( a0, annulus->innerRadius, annulus->geometryType ));
			if ( qdrchange ) points.push_back( discPointToXy( qdrcorner, annulus->innerRadius, annulus->geometryType ));
			points.push_back( discPointToXy( a1, annulus->innerRadius, annulus->geometryType ));
			points.push_back( discPointToXy( a1, annulus->radius, annulus->geometryType ));
			if ( qdrchange ) points.push_back( discPointToXy( qdrcorner, annulus->radius, annulus->geometryType ));
			points.push_back( discPointToXy( a0, annulus->radius, annulus->geometryType ));
			painter->drawPolygon( points );
		}
		break;
		case ChartShape::Triangle:
				warnlog << "CeLabelledAnnulus TODO: Invalid chart frame type " << (int)geom;
			break;
			default:
				warnlog << "CeLabelledAnnulus::drawChartFrameField Invalid chart frame type " << (int)geom;
				assert( false );
			break;
		}
	}

/*****************************************************
**
**   ChartElementBase   ---   paintAnnulusConnector
**
******************************************************/
void ChartElementBase::paintAnnulusConnector( Painter *painter, const double &angle, const CgDisc &disc, const double &r )
{
	paintAnnulusConnector( painter, angle, disc.radius, disc.radius + r, disc.geometryType );
	/*
	assert( painter );
	const MComplex p1 = discPointToXy( a, disc.radius + r, disc.geometryType );
	const MComplex p2 = discPointToXy( a, disc.radius, disc.geometryType );
	painter->drawLine( p1, p2 );
	*/
}

/*****************************************************
**
**   ChartElementBase   ---   paintAnnulusConnector
**
******************************************************/
void ChartElementBase::paintAnnulusConnector( Painter *painter, const double &angle, const double &r1,
	const double &r2, const ChartShape &geom )
{
	assert( painter ); 
	const MComplex p1 = discPointToXy( angle, r2, geom );
	const MComplex p2 = discPointToXy( angle, r1, geom );
#ifdef DEBUG_CHARTELEMENT
	debuglog << "ChartElementBase::paintAnnulusConnector angle " << angle << " r1 " << r1 << " r2 " << r2
		<< " p1 " << p1 << " p2 " << p2;
#endif
	painter->drawLine( p1, p2 );
}

/*****************************************************
**
**   ChartElementBase   ---   paintMarkerLine
**
******************************************************/
void ChartElementBase::paintMarkerLine( Painter *painter, const double &longitude, CgDisc disc )
{
	assert( painter );

	const MComplex p1 = discPointToXy( longitude, disc.radius - disc.markerLength, disc.geometryType );
	const MComplex p2 = discPointToXy( longitude, disc.radius + disc.markerLength, disc.geometryType );
	painter->drawLine( p1, p2 );
}

/*****************************************************
**
**   ChartElementFactory   ---   createElements
**
******************************************************/
std::list<ChartElementBase_ptr> ChartElementFactory::createElements( const ChartGeometry &geom ) const
{
	std::list<ChartElementBase_ptr> l;

	for( const auto &[ key, value ] : geom.elements )
	{
		auto elem = createChartElement( value );
		if ( elem ) l.push_back( elem );
	}

	l.sort(
		[] ( ChartElementBase_ptr e1, ChartElementBase_ptr e2 )
		{
			assert( e1 );
			assert( e2 );
			return e1->getDrawStackPosition() < e2->getDrawStackPosition();
		}
	);
	return l;
}

/*****************************************************
**
**   ChartElementFactory   ---   createChartElement
**
******************************************************/
ChartElementBase_ptr ChartElementFactory::createChartElement( CgElement *cge ) const
{
	assert( cge );
#ifdef DEBUG_CHARTELEMENT
	debuglog << "ChartElementFactory::createChartElement type " << (int)cge->getElementType();
#endif
	ChartElementBase *elem = nullptr;

	switch( cge->getElementType() )
	{
		using enum CgElementType;

		case Disc:
			elem = createDisc( cge );
		break;
		case Annulus:
			elem = createAnnulus( cge );
		break;
		case Aspects:
			elem = createAspects( cge );
		break;
		case Image:
			elem = createImage( cge );
		break;
		case Angles:
			elem = createAngles( cge );
		break;

		case WesternPlanets:
			elem = createWesternPlanets( cge );
		break;
		case UranianPlanets:
			elem = createUranianPlanets( cge );
		break;

		case UranianDial:
			elem = createUranianDial( cge );
		break;
		case UranianAspects:
			elem = createUranianAspects( cge );
		break;

		case Signs:
			elem = createSigns( cge );
		break;
		case Houses:
			elem = createHouses( cge );
		break;
		case Nakshatras:
			elem = createNakshatras( cge );
		break;
		case DasaLords:
			elem = createDasaLords( cge );
		break;

		case EastGrid:
			elem = createEastIndianChartGrid( cge );
		break;
		case SouthGrid:
			elem = createSouthIndianChartGrid( cge );
		break;
		case DiamondGrid:
			elem = createDiamondChartGrid( cge );
		break;
		case SbcGrid:
			elem = createSbcGrid( cge );
		break;
		case Error:
			elem = createError( cge );
		break;
		default:
			fatallog << "wrong Elem type " << (int)cge->getElementType() ;
			assert( false );
		break;
	}
	//assert( elem );
	return ChartElementBase_ptr( elem );
}


