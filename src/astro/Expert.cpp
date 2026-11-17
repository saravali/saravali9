/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Expert.cpp
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

#include "Expert.h"

#include "astrobase.h"
#include "ContextProperties.h"
#include "Horoscope.h"

/*****************************************************
**
**   Expert   ---   Constructor
**
******************************************************/
Expert::Expert( ContextProps_ptr props, const Viewpoint &stickyVp )
	: props( props ),
	stickyVp( stickyVp )
{
	assert( props );

	// TextHelper needs neutral Viewpoint
	//assert_valid_enum<Viewpoint>( stickyVp );
}

/*****************************************************
**
**   Expert   ---   getViewpoint
**
******************************************************/
Viewpoint Expert::getViewpoint() const
{
	assert( props );
	return props->getEffectiveViewpoint( stickyVp );
}

// ACHTUNG SCHROTT

/*****************************************************
**
**   Expert   ---   getLongitude
**
******************************************************/
double Expert::getLongitude( const PlanetId &planetId, const HoroscopeRole role ) const
{
	assert( props );
	Horoscope_ptr horoscope = props->getHoroscope( role );
	assert( horoscope );
	return horoscope->getLongitude( planetId, getViewpoint() );
}

/*****************************************************
**
**   Expert   ---   getNakshatra27
**
******************************************************/
Nakshatra27 Expert::getNakshatra27( const PlanetId &planetId, const HoroscopeRole role ) const
{
	return ::getNakshatra27( this->getLongitude( planetId, role ));
}

/*****************************************************
**
**   Expert   ---   getNakshatra28
**
******************************************************/
Nakshatra28 Expert::getNakshatra28( const PlanetId &planetId, const HoroscopeRole role ) const
{
	return ::getNakshatra28( this->getLongitude( planetId, role ));
}

/*****************************************************
**
**   Expert   ---   getDirection
**
******************************************************/
MovingDirection Expert::getDirection( const PlanetId &planetId, const HoroscopeRole role ) const
{
	assert( props );
	Horoscope_ptr horoscope = props->getHoroscope( role );
	assert( horoscope );
	return horoscope->getDirection( planetId, getViewpoint() );
}

/*****************************************************
**
**   Expert   ---   isRetrograde
**
******************************************************/
bool Expert::isRetrograde( const PlanetId &planetId, const HoroscopeRole role ) const
{
	assert( props );
	Horoscope_ptr horoscope = props->getHoroscope( role );
	assert( horoscope );
	return horoscope->isRetrograde( planetId, getViewpoint() );
}

/*****************************************************
**
**   VedicExpert   ---   getVargaData
**
******************************************************/
VargaData VedicExpert::getVargaData( const PlanetId &planetId, const HoroscopeRole role ) const
{
	return this->getVargaData( planetId, this->varga, role );
}

/*****************************************************
**
**   VedicExpert   ---   getVargaData
**
******************************************************/
VargaData VedicExpert::getVargaData( const PlanetId &planetId, const Varga &v, const HoroscopeRole role ) const
{
	assert( props );
	Horoscope_ptr horoscope = props->getHoroscope( role );
	assert( horoscope );
	assert_valid_enum<Varga>( v );
	return horoscope->getVargaData( planetId, v, getViewpoint() );
}

/*****************************************************
**
**   VedicExpert   ---   getRasi
**
******************************************************/
Sign VedicExpert::getRasi( const PlanetId &planetId, const HoroscopeRole role ) const
{
	return this->getVargaData( planetId, role ).getRasi();
}

