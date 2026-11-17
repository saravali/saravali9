/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/CeWesternPlanets.cpp
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
#include "ChartElementImpl.h"
#include "ContextProperties.h"
#include "Direction.h"
#include "enumserializer.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "Painter.h"

#define DEBUG_WESTERNPLANETS

/*************************************************//**
*
*   This class implements the display of planets, luminaries etc
*   and degree scales.
*
*   Depending on the mode there may be multiple (or even no) sets of planets
*
******************************************************/
class CeWesternPlanets : public CePlanetsBase
{
public:

	CeWesternPlanets( HybridChart *chart, CgPlanetScale *a ) : CePlanetsBase( chart, a )
	{
		drawStackPosition = 3;
	}

	/*****************************************************
	**
	**   CeWesternPlanets   ---   paint
	**
	******************************************************/
	virtual void paint( Painter *painter, const MRect &refreshRect ) override
	{
#ifdef DEBUG_WESTERNPLANETS
		debuglog << "CeWesternPlanets::paint roles size " << chart->getChartRoles().size();
#endif
		CePlanetsBase::paint( painter, refreshRect );
		paintDegreeScales( painter, refreshRect );
	}


protected:

	/*****************************************************
	**
	**   CeWesternPlanets   ---   onCoordinatesChanged
	**
	******************************************************/
	virtual void onCoordinatesChanged() override
	{
		const Viewpoint viewpoint = chart->getViewpoint();
		ContextProps_ptr props = chart->getProps();

#ifdef DEBUG_WESTERNPLANETS
		debuglog << "CeWesternPlanets::onCoordinatesChanged Start viewpoint " << viewpoint << " roles size " << chart->getChartRoles().size();
#endif
		if ( ! getCg()->show ) return;

		//const auto obs = props->getPlanets( viewpoint, { FilterGroup::Houses } );
		const auto obs = props->getPlanets( viewpoint );
#ifdef DEBUG_WESTERNPLANETS

		for( const auto &p : obs )
		{
			debuglog << "CeWesternPlanets::onCoordinatesChanged planet " << p;
		}
#endif

		clearItems();
		for( const auto &p : obs )
		{
			for( const auto &role : chart->getChartRoles() )
			{
				Horoscope_ptr h = chart->getHoroscope( role );
				if ( ! h ) return; // can happen on partner chart

				if ( ! drawAnglesRequired( role ) && PlanetHelper::isAngle( p )) continue;

				if ( ! DirectionCalculator::get()->isShiftedRole( h->getRole()) && PlanetHelper::isCardinalPoint( p )) continue;

				const double longitude = getModuloLongitude( h->getLongitude( p, viewpoint ));

#ifdef DEBUG_WESTERNPLANETS
				debuglog << "CeWesternPlanets::onCoordinatesChanged role " << role << " Ob: " << p << " longitude " << longitude;
#endif
				addItem( this, role, getCoordinatesForItem( role, longitude ), p, longitude );
			}
		}
	}
};

/*****************************************************
**
**   Chart element factory
**
******************************************************/
ChartElementBase *ChartElementFactory::createWesternPlanets( CgElement *cge ) const
{
	CgPlanetScale *a = dynamic_cast<CgPlanetScale*>( cge );
	assert( a );
	return new CeWesternPlanets( chart, a );
}

