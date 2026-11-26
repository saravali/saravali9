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
#include "PlanetRing.h"
#include "TermList.h"
#include "Uranian.h"

#define DEBUG_UPLANETS

/*************************************************//**
*
*
*
******************************************************/
class UranianPlanetRing : public PlanetRing
{
public:

	/*****************************************************
	**
	**   UranianPlanetRing   ---   paint
	**
	******************************************************/
	UranianPlanetRing( HybridChart *chart, CgPlanetRing *ring )
	 : PlanetRing( chart, ring )
	{
	}

	/*****************************************************
	**
	**   UranianPlanetRing   ---   createTerms
	**
	******************************************************/
	virtual std::list<MTerm> createTerms( const HoroscopeRole &role ) override
	{
		UranianSection *usection = chart->getUranianSection();
		assert( usection );

		const Viewpoint viewpoint = chart->getViewpoint();
		ContextProps_ptr props = chart->getProps();
		auto obs = props->getPlanets( viewpoint, { FilterGroup::Houses } );

		std::list<MTerm> terms;
		Horoscope_ptr h = chart->getHoroscope( role );
		if ( ! h ) return terms; // can happen on partner chart

		for( const auto &def : Formulary::get()->getFormDefPairs( usection->chartFormulae, usection->includeAntiscia ))
		{
			for( auto term : TermList( obs, def.second ))
			{
				if ( term.applyFilter( usection->filter ))
				{
						if ( MTerm::isAntiTermId( term.getTermId() ) && role != chart->getFirstRole()) continue;
						terms.push_back( term );
				}
			}
		}
		return terms;
	}
};

/*****************************************************
**
**   Chart element factory
**
******************************************************/
ChartElementBase *ChartElementFactory::createUranianPlanetRing( CgElement *cge ) const
{
	CgPlanetRing *a = dynamic_cast<CgPlanetRing*>( cge );
	assert( a );
	return new UranianPlanetRing( chart, a );
}

