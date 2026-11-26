/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/WesternAngles.cpp
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

#include "PlanetRing.h"

#include "ContextProperties.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "mathbase.h"
#include "Painter.h"

/*************************************************//**
*
*  Chart element containing the angles of a Western chart.
*  
*  The angles of western chart are shown as arrows in the direction of
*  Ascendant and Meridian.
*
*  According to the configuration labels are shown for up to four angles.
*
*  Angles are only painted for the first chart (radix)
*
*  This class inherits from PlanetRing because responsive event handling is identical.
*
******************************************************/
class WesternAngles : public PlanetRing
{
public:

	WesternAngles( HybridChart *chart, CgPlanetRing *a ) : PlanetRing( chart, a )
	{
		drawStackPosition = 3;
	}

	/*****************************************************
	**
	**   WesternAngles   ---   paint
	**
	******************************************************/
	virtual void paint( Painter *painter, const MRect& /*refreshRect*/ ) override
	{
		Horoscope_ptr h = chart->getFirstHoroscope();
		if ( ! h ) return;

		setInheritedStyle( painter );
		const Viewpoint viewpoint = chart->getViewpoint();
		const double aries = chart->getAries();

		CgLabelledAnnulus *annulus = getGeom();
		assert( annulus );
		if ( ! annulus->show ) return;
		setInheritedStyle( painter );
		MComplex p1, p2;

		for( const PlanetId p : { PlanetId::Ascendant, PlanetId::Meridian } )
		{
			// arrow
			const double l = red_deg( aries + h->getLongitude( p, viewpoint ));
			p1 = diskPointToXy( l, annulus->innerRadius, annulus->geometryType );
			p2 = diskPointToXy( l, annulus );
			painter->drawArrow( p1, p2 );

			// opposite part of arrow is a line
			p1 = diskPointToXy( l + 180, annulus->innerRadius, annulus->geometryType );
			p2 = diskPointToXy( l + 180, annulus );
			painter->drawLine( p1, p2 );
		}

		for( PlanetRingItem *item : items[ HoroscopeRole::Radix ] )
		{
			// geht nicht
			//item->correctedLongitude = red_deg( item->longitude - deltaDeg );
			if ( formatRequired )
			{
				item->format( painter );
			}
			item->paint( painter );
		}
		formatRequired = false;
		chart->setGraphicFont( painter );
	}

	/*****************************************************
	**
	**   WesternAngles   ---   createTerms
	**
	******************************************************/
	virtual std::list<MTerm> createTerms( const HoroscopeRole & role ) override
	{
		std::list<MTerm> terms;
		const Viewpoint &viewpoint = chart->getViewpoint();
		ContextProps_ptr props = chart->getProps();

		if ( role == HoroscopeRole::Radix )
		{
			if ( props->hasPlanetId( PlanetId::Ascendant, viewpoint ))
				terms.push_back( PlanetId::Ascendant );
			if ( props->hasPlanetId( PlanetId::Meridian, viewpoint ))
				terms.push_back( PlanetId::Meridian );
			if ( props->hasPlanetId( PlanetId::Descendant, viewpoint ))
				terms.push_back( PlanetId::Descendant );
			if ( props->hasPlanetId( PlanetId::ImumCoeli, viewpoint ))
				terms.push_back( PlanetId::ImumCoeli );
		}
		return terms;
	}

	/*****************************************************
	**
	**   WesternAngles   ---   adjustLongitudes
	**
	******************************************************/
	virtual void adjustLongitudes( const uint &roleId ) override
	{
		constexpr double deltaDeg = 3;
		if ( roleId != 0 || chartRoles.size() == 0 ) return;

		CgPlanetRing *geom = getGeom();
		assert( geom );
		const double radius = getLabelRadiusForRoleIndex( roleId );

		const auto a = items.find( chartRoles[roleId] );
		for( PlanetRingItem *item : a->second )
		{
			item->correctedLongitude = red_deg( item->longitude + deltaDeg );
			item->point = diskPointToXy( item->correctedLongitude, radius, geom->geometryType );
		}
	}

	virtual double getOuterRadius() override
	{
		CgPlanetRing *geom = getGeom();
		assert( geom );
		return geom->radius;
	}
};

/*****************************************************
**
**   Grid element factory methods
**
******************************************************/
ChartElementBase *ChartElementFactory::createAngles( CgElement *cge ) const
{
	auto *a = dynamic_cast<CgPlanetRing*>( cge );
	assert( a );
	return new WesternAngles( chart, a );
}

