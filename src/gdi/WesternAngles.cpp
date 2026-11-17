/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/CeAngles.cpp
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

#include "CePlanetsBase.h"

#include "ContextProperties.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "mathbase.h"
#include "Painter.h"

/*************************************************//**
*
*   Chart element containing the angles of a Western chart.
*  
*  The angles of western chart are shown as arrows in the direction of
*  Ascendant and Meridian.
*  According to the configuration labels are shown for up to four angles.
*
*  Angles are only painted for the first chart (most frequently radix)
*
******************************************************/
class CeAngles : public CePlanetsBase
{
public:

	CeAngles( HybridChart *chart, CgPlanetScale *a ) : CePlanetsBase( chart, a )
	{
		drawStackPosition = 3;
		doPaintMarker = false;
	}

	/*****************************************************
	**
	**   CeAngles   ---   paint
	**
	******************************************************/
	virtual void paint( Painter *painter, const MRect& refreshRect ) override
	{
		Horoscope_ptr h = chart->getFirstHoroscope();
		if ( ! h ) return;

		setInheritedStyle( painter );
		const Viewpoint &viewpoint = chart->getViewpoint();
		const double aries = chart->getAries();

		CgLabelledAnnulus *annulus = getCg();
		if ( ! annulus->show ) return;
		setInheritedStyle( painter );
		double l;
		MComplex p1, p2, p3, p4;

		const std::list<PlanetId> angles = { PlanetId::Ascendant, PlanetId::Meridian };
		for( const PlanetId p : angles )
		{
			// arrow
			l = red_deg( aries + h->getLongitude( p, viewpoint ));
			p1 = discPointToXy( l, annulus->innerRadius, annulus->geometryType );
			p2 = discPointToXy( l, annulus );
			painter->drawArrow( p1, p2 );

			// opposite part of arrow
			p3 = discPointToXy( l + 180, annulus->innerRadius, annulus->geometryType );
			p4 = discPointToXy( l + 180, annulus );
			painter->drawLine( p3, p4 );
		}

		CePlanetsBase::paint( painter, refreshRect );
	}

protected:

	/*****************************************************
	**
	**   CeAngles   ---   onCoordinatesChanged
	**
	******************************************************/
	virtual void onCoordinatesChanged() override
	{
		debuglog << "ChartElementAngles::onCoordinatesChanged";
		clearItems();

		Horoscope_ptr h = chart->getFirstHoroscope();
		if ( ! h ) return;

		const double aries = chart->getAries();
		std::list<PlanetId> angles;
		double l;

		const Viewpoint &viewpoint = chart->getViewpoint();
		ContextProps_ptr props = chart->getProps();

		CgLabelledAnnulus *annulus = getCg();
		if ( ! annulus->show ) return;

		// text/symbol will will be written at position + delta
		const double deltaDeg = 3;

#if 0
		for( const auto &p : { PlanetId::Ascendant, PlanetId::Meridian, PlanetId::Descendant, PlanetId::ImumCoeli } )
		{
			if ( props->hasPlanetId( p, viewpoint )) angles.push_back( p );
		}
#else
		if ( props->hasPlanetId( PlanetId::Ascendant, viewpoint ))
			angles.push_back( PlanetId::Ascendant );
		if ( props->hasPlanetId( PlanetId::Meridian, viewpoint ))
			angles.push_back( PlanetId::Meridian );
		if ( props->hasPlanetId( PlanetId::Descendant, viewpoint ))
			angles.push_back( PlanetId::Descendant );
		if ( props->hasPlanetId( PlanetId::ImumCoeli, viewpoint ))
			angles.push_back( PlanetId::ImumCoeli );
#endif

		const HoroscopeRole role = chart->getFirstRole();
		for( const PlanetId &planet : angles )
		{
			l = red_deg( aries + h->getLongitude( planet, viewpoint ));
			const MComplex point = discPointToXy( red_deg( l + deltaDeg ), annulus );

			addItem( this, role, point, planet, l );
		}
	}

};

/*****************************************************
**
**   Grid element factory methods
**
******************************************************/
ChartElementBase *ChartElementFactory::createAngles( CgElement *cge ) const
{
	CgPlanetScale *a = dynamic_cast<CgPlanetScale*>( cge );
	assert( a );
	return new CeAngles( chart, a );
}

