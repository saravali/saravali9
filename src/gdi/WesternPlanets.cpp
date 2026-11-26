/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/WesternPlanets.cpp
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
#include "ChartGeometry.h"
#include "ContextProperties.h"
#include "Direction.h"
#include "enumserializer.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "Painter.h"
#include "PlanetRing.h"
#include "TermList.h"

#define DEBUG_WESTERNPLANETS

/*************************************************//**
*
*
*
******************************************************/
class WesternPlanetRing : public PlanetRing
{
public:

	/*****************************************************
	**
	**   WesternPlanetRing   ---   paint
	**
	******************************************************/
	WesternPlanetRing( HybridChart *chart, CgPlanetRing *ring )
	 : PlanetRing( chart, ring )
	{
	}

	/*****************************************************
	**
	**   WesternPlanetRing   ---   createTerms
	**
	******************************************************/
	virtual std::list<MTerm> createTerms( const HoroscopeRole & role ) override
	{
		std::list<MTerm> terms;
		const Viewpoint viewpoint = chart->getViewpoint();
		ContextProps_ptr props = chart->getProps();

		const ChartGeometry &geom = chart->getGeometry();
		const bool hasAnglesElement  = geom.containsElementType( CgElementType::Angles );

		const auto obs = props->getPlanets( viewpoint );
		for( auto term : TermList( obs, TermId::Single ))
		{
#ifdef DEBUG_WESTERNPLANETS
		debuglog << "WesternPlanets::createTerms term " << term << " hasAnglesElement " << std::boolalpha << hasAnglesElement
			<< " first " << term.getFirstPlanet();
#endif

			// ignore item if (1) role = radix and (2) planet is angle and (3) angle element is contained in chart
			if ( PlanetHelper::isAngle( term.getFirstPlanet()) && hasAnglesElement && role == chart->getFirstRole()) continue;
			terms.push_back( term );
		}
		return terms;
	}

protected:

};

/*****************************************************
**
**   Chart element factory
**
******************************************************/
ChartElementBase *ChartElementFactory::createWesternPlanetRing( CgElement *cge ) const
{
	CgPlanetRing *ring = dynamic_cast<CgPlanetRing*>( cge );
	assert( ring );
	return new WesternPlanetRing( chart, ring );
}

