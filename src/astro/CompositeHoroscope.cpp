/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/CompositeHoroscope.cpp
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

#include "CompositeHoroscope.h"

#include "astrobase.h"
#include "astroconfig.h"
#include "Calculator.h"
#include "ContextProperties.h"
#include "Dignity.h"
#include "Formatter.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "MDocument.h"
#include "Nakshatra.h"
#include "Table.h"
#include "TextHelper.h"
#include "Varga.h"

#include <iostream>

extern Astroconfig *astroconfig;

/*****************************************************
**
**   CompositeHoroscope   ---   update
**
******************************************************/
void CompositeHoroscope::update()
{
	Horoscope::init_update();
}

/*****************************************************
**
**   CompositeHoroscope   ---   getLongitude
**
******************************************************/
double CompositeHoroscope::getLongitude( const PlanetId &p, const Viewpoint &viewpoint )
{
	assert( props );
	Horoscope_ptr h1 = props->getHoroscope( HoroscopeRole::Partner1 );
	Horoscope_ptr h2 = props->getHoroscope( HoroscopeRole::Partner2 );

	// must not happen and will cause endless loop
	assert( h1.get() != this );
	assert( h2.get() != this );

	if ( PlanetHelper::isAngle( p ))
	{
		if ( ! longitude[(int)p] ) calculate_angles();
		return red_deg( longitude[(int)p] - getAyanamsa( viewpoint ));
	}
	else if ( PlanetHelper::isHouse( p ))
	{
		return getHouseCusp( (House)((int)p - (int)House::h2), viewpoint );
	}
	else if ( PlanetHelper::isEphemPlanet( p ))
	{
		//debuglog << "PRE " << (int)p << " " << longitude[(int)p] ;
		if ( ! longitude[(int)p] ) longitude[(int)p] = getMidpoint( h1->getTropicalLongitude( p ), h2->getTropicalLongitude( p ));

#if 0
		debuglog << "CompositHorosocope::getLongitude p " << (int)p
			<< " h1 " << h1->getTropicalLongitude( p )
			<< " h2 " << h2->getTropicalLongitude( p )
			<< " result " << longitude[(int)p] ;
#endif
		return red_deg( longitude[(int)p] - getAyanamsa( viewpoint ));
	}
	return getMidpoint( h1->getLongitude( p, viewpoint ), h2->getLongitude( p, viewpoint ));
}

/*****************************************************
**
**   CompositeHoroscope   ---   getLSpeed
**
******************************************************/
double CompositeHoroscope::getLSpeed( const PlanetId &p, const Viewpoint &viewpoint )
{
	assert( props );
	Horoscope_ptr h1 = props->getHoroscope( HoroscopeRole::Partner1 );
	assert( h1 );
	Horoscope_ptr h2 = props->getHoroscope( HoroscopeRole::Partner2 );
	assert( h2 );

	return .5 * ( h1->getLSpeed( p, viewpoint ) + h2->getLSpeed( p, viewpoint ));
}

/*****************************************************
**
**   CompositeHoroscope   ---   calculate_angles
**
******************************************************/
void CompositeHoroscope::calculate_angles()
{
	assert( props );
	Horoscope_ptr h1 = props->getHoroscope( HoroscopeRole::Partner1 );
	if ( ! h1 ) return;
	Horoscope_ptr h2 = props->getHoroscope( HoroscopeRole::Partner2 );
	if ( ! h2 ) return;

	debuglog << "COMPOSITE CALCULATE angles";

	armc = .5 * ( h1->getArmc() + h2->getArmc());
	const std::pair<double, double>  a = calculator->calcAnglesByArmc( armc,
		dl.location.latitude,
		h1->getEps());

	longitude[(int)PlanetId::Ascendant] = a.first;
	longitude[(int)PlanetId::Descendant] = red_deg( a.first + 180 );
	longitude[(int)PlanetId::Meridian] = a.second;
	longitude[(int)PlanetId::ImumCoeli] = red_deg( a.second + 180 );
}


