/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/UranianPlanets.cpp
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

#include "ChartElementImpl.h"

#include "CePlanetsBase.h"
#include "ContextProperties.h"
#include "Direction.h"
#include "DocHelper.h"
#include "enumserializer.h"
#include "Formulary.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "mathbase.h"
#include "Painter.h"
#include "TermList.h"
#include "Uranian.h"

#define DEBUG_UPLANETS

/*************************************************//**
*
*   This class implements the display of planets, luminaries etc
*   and degree scales.
*
*   Depending on the mode there may be multiple (or even no) sets of planets
*
******************************************************/
class UranianPlanets : public CePlanetsBase
{
public:

	UranianPlanets( HybridChart *chart, CgPlanetScale *a ) : CePlanetsBase( chart, a )
	{
		usection = chart->getUranianSection();
		assert( usection );

		drawStackPosition = 3;
	}

	/*****************************************************
	**
	**   UranianPlanets   ---   paint
	**
	******************************************************/
	virtual void paint( Painter *painter, const MRect &refreshRect ) override
	{
#ifdef DEBUG_UPLANETS
		debuglog << "UranianPlanets::paint roles size " << chart->getChartRoles().size();
#endif
		CePlanetsBase::paint( painter, refreshRect );
		paintDegreeScales( painter, refreshRect, usection->chartDivision );
	}

protected:

	/*****************************************************
	**
	**   UranianPlanets   ---   onCoordinatesChanged
	**
	******************************************************/
	virtual void onCoordinatesChanged() override
	{
#ifdef DEBUG_UPLANETS
		debuglog << "UranianPlanets::onCoordinatesChanged Start roles size " << chart->getChartRoles().size();
		for( const auto &role : chart->getChartRoles() )
		{
			debuglog << "UranianPlanets::onCoordinatesChanged -> ROLE " << role;
		}
		for( const auto a : usection->shiftPlanetIds )
		{
			debuglog << "UranianPlanets::onCoordinatesChanged -> SHIFT ID " << a.first << "/" << a.second;
		}
#endif
		if ( ! getCg()->show ) return;

		const Viewpoint viewpoint = chart->getViewpoint();
		ContextProps_ptr props = chart->getProps();

		auto obs = props->getPlanets( viewpoint, { FilterGroup::Houses } );

		clearItems();

		std::list<TermId> terms;
		for( const auto &def : Formulary::get()->getFormDefPairs( usection->chartFormulae, usection->includeAntiscia ))
		{
			terms.push_back( def.second );
		}

		const int division = usection->chartDivision;

		// find out shift values for planets
		std::map<HoroscopeRole,double> shiftValues;
		for( const auto &role : chart->getChartRoles() )
		{
			shiftValues[ role ] = 0;
			if ( usection->enablePlanetShift )
			{
				const auto a = usection->shiftPlanetIds.find( role );
				if ( a != usection->shiftPlanetIds.end())
				{
					shiftValues[ role ] = chart->getHoroscope( role )->getLongitude( a->second, viewpoint );
				}
			}
			debuglog << "UranianPlanets::onCoordinatesChanged -> SHIFT ROLE " << role << " value " << shiftValues[ role ];
		}

		for( const TermId &termid : terms )
		{
			for( auto term : TermList( obs, termid ))
			{
				if ( term.applyFilter( usection->filter ))
				{
					for( const auto &role : chart->getChartRoles() )
					{
						Horoscope_ptr h = chart->getHoroscope( role );
						if ( ! h ) return; // can happen on partner chart

						// Antiscia only in chart #1
						if ( MTerm::isAntiTermId( termid ) && role != chart->getFirstRole()) continue;

						// skip earth horoscope angles if not relevant
						if ( term.isPrimitive()
							&& PlanetHelper::isCardinalPoint( term.getFirstPlanet())
							&& ( ! h->getLongitudeOffset() || role == chart->getFirstRole())
							)
						continue;

						const double longitude = getModuloLongitude( term.evaluate( h, viewpoint ) - shiftValues[ role ], division );

#ifdef DEBUG_UPLANETS
						debuglog << "UranianPlanets::onCoordinatesChanged role " << role
							<< " is-shifted " << DirectionCalculator::get()->isShiftedRole( h->getRole())
							<< " h offset " << h->getLongitudeOffset()
							<< " partner offset " << shiftValues[ role ]
							<< " primitive " << term.isPrimitive()
							<< " Term: " << term
							<< " longitude " << longitude;
#endif
						addItem( this, role, getCoordinatesForItem( role, longitude ), term, longitude );
					}
				}
			}
		}
	}

	UranianSection *usection = nullptr;

	ActiveTerm lastMatchTerm = std::make_pair( MTerm( TermId::None ), HoroscopeRole::None );
};

/*****************************************************
**
**   Chart element factory
**
******************************************************/
ChartElementBase *ChartElementFactory::createUranianPlanets( CgElement *cge ) const
{
	CgPlanetScale *a = dynamic_cast<CgPlanetScale*>( cge );
	assert( a );
	return new UranianPlanets( chart, a );
}

