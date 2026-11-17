/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/PlanetaryPicture.h
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
#ifndef _PLANETARYPICTURE_H_
#define _PLANETARYPICTURE_H_

#include "Expert.h"
#include "constants.h"
#include "Planet.h"
#include "Term.h"

#include <list>

struct UranianSection;

/*************************************************//**
*
*
*
******************************************************/
struct PpElement
{
	const MTerm term;

	const double longitude = 0.0;

	const int division = 1;

	const double orbis = 0.0;
};

/*************************************************//**
*
*
*
******************************************************/
class PlanetaryPicture
{
public:
	PlanetaryPicture( const MTerm &term, const double &longitude, const int &d, const HoroscopeRole &leadingRole )
		: leadingElem( term, longitude, d ),
		leadingRole( leadingRole ),
		longitude( longitude ),
		division( d )
	{
	}

	void calculateScore( const double &maxorbis );

	const PpElement leadingElem;

	const HoroscopeRole leadingRole = HoroscopeRole::None;

	const double longitude = 0.0;

	const int division = 1;

	double score = 0.0;

	std::list<PpElement> elements;
};

/*************************************************//**
*
*
*
******************************************************/
class PlanetaryPictureExpert : public Expert
{
public:

	PlanetaryPictureExpert( ContextProps_ptr props, UranianSection *usection )
		: Expert( props, Viewpoint::Uranian ),
		usection( usection )
	{
	}

	/**
	*  returns false if max number of matches exceeded (i.e. results are incomplete)
	*/
	bool createPictures();

	std::list<PlanetaryPicture> &getPictures() { return pictures; }

	void writePictureTable( MDocument_ptr );

	uint totalMatchCount = 0;

private:

	void writePictureTablePart( MDocument_ptr, const uint &istart, const uint &iend );

	void createPicturesForPair( Horoscope_ptr h1, Horoscope_ptr h2, const HoroscopeRole& );

	std::list<PlanetaryPicture> pictures;

	UranianSection *usection;

};

#endif // _PLANETARYPICTURE_H_

