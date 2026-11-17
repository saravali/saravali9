/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ArabicPartBase.h
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

#ifndef _ARABICPARTBASE_H_
#define _ARABICPARTBASE_H_

#include "constants.h"
#include "Planet.h"

#include <wx/string.h>

/**********************************************************
***
*
* Arabic parts calculation parameters.
* "Planets" may be Ascendant/Meridian or Nodes.
*
* Each parameter must have a reference object
* (planet, house, other Arabic part or fixed longitude).
*
* Attention: lords are different in Vedic and western
* astrology (Uranus, Neptune and Pluto for Aq, Pi and Sc)
*
***
***********************************************************/
enum Arabic
{
	None = -1,

  // longitude of a planet (0..MAX_EPHEM_OBJECTS)
  LPlanet = 0,

  // longitude of other arabic part (only Fortune and Spirit used)
  LArabic,

  // fixed zodiacal longitude (0..360) : ! NO Ayanamsa APPLIED!
  LFixed,

  // longitude of house cusp (0..11) = h
  LHousecusp,

  // like above plus 10 deg (0..11) = j
  LHousecusp10,

  // longitude of lord of house cusp (0..11) = r
  LLord,

  // longitude of Dispositor (0..MAX_EPHEM_OBJECTS) = D
  LDispositor,

  // planet's house cusp (like LHousecusp but reference to planet instead of house number), (0..MAX_EPHEM_OBJECTS) = H
  LPlanetHousecusp,

  // longitude of lord of a planet's house cusp (0..MAX_EPHEM_OBJECTS) = R
  LPlanetHousecuspLord
};

/*************************************************//**
*
* 
*
******************************************************/
class ArabicPartSummand
{
public:

	ArabicPartSummand( const Arabic &t, const PlanetId &ob ) : type( t )
	{
		refob.objectId = ob;
	}

	ArabicPartSummand( const Arabic &t, const int &l ) : type( t )
	{
		refob.longitude = l;
	}

	ArabicPartSummand( const Arabic &t, const House &h ) : type( t )
	{
		refob.house = h;
	}

	PlanetId getPlanetId() const { return refob.objectId; }

	House getHouse() const { return refob.house; }

	int getLongitude() const { return refob.longitude; }

	const Arabic type;
	
private:

	union
	{
		int longitude;
		House house;
		PlanetId objectId;
	} refob;
};

/*************************************************//**
*
*  names and rules of arabis parts
*
******************************************************/
struct ArabicPartDef
{
	wxString name;
	ArabicPartSummand s1, s2, s3;
	bool flip;  // flip addition+subtraction for night birth
};

/*************************************************//**
*
*  encapsulates calculation of Arabic Parts
*
******************************************************/
class ArabicPartsConfig
{
public:

	static wxString getPlanetName( const PlanetId &p, const TextFormat &format );

	static ArabicPartDef getPart( const int &i );

};

#endif


