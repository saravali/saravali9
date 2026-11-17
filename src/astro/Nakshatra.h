/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Nakshatra.h
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

#ifndef _Nakshatra_H_
#define _Nakshatra_H_

#include "bootstrap.h"
#include "Expert.h"
#include "constants.h"

#include <list>
#include <map>
#include <vector>
#include <wx/string.h>

/*************************************************//**
*
* 
*
******************************************************/
class NakshatraProperties
{
public:

	// benefic, malefic, neutral (Abhijit)
	InfluenceType taraQuality;

	int tara; 

};


/*************************************************//**
*
* 
*
******************************************************/
class Rajju
{
public:

	Rajju() { aroha = type = 0; }

	int aroha;
	int type;
};

/*************************************************//**
*
* 
*
******************************************************/
class Yoni
{
public:

	Yoni() { id = 0; male = true; }
	Yoni( const int &yid, const bool &m, const wxString &s )
	 : id( yid ), male( m ), name( s ) {}

	int id;
	bool male;
	wxString name;
};

/*************************************************//**
*
*  Encapsulates calculation of Nakshatra releated details
*
******************************************************/
class NakshatraExpert : public VedicExpert
{
public:

	NakshatraExpert( ContextProps_ptr props ) : VedicExpert( props ) {}

	static int getVedhaNakshatra( const Nakshatra28 &nak );

	Yoni getYoni( const PlanetId&, const HoroscopeRole = HoroscopeRole::Radix );

	Nadi getNadi( const PlanetId&, const HoroscopeRole = HoroscopeRole::Radix );

	Gana getGana( const PlanetId&, const HoroscopeRole = HoroscopeRole::Radix );

	int getTara( const Nakshatra27& );

	Rajju getRajju( const PlanetId&, const HoroscopeRole = HoroscopeRole::Radix );

	/**
	*  Calculates the incfluence type (benefic etc.) and the Tara for a given birth Nakshatra.
	*
	*  Calculation is based on 28 Nakshatras.
	*/
	std::vector<NakshatraProperties> getSbcNakshatraProperties( const int &birthStar );

	std::list<Nakshatra28> getVedhaNakshatras( const Nakshatra28& );

	void write( MDocument_ptr, const bool show_header = true );
};

/*************************************************//**
*
*  Encapsulates astrological details for Sarvatobhadra charts
*
******************************************************/
class SarvatobhadraExpert : public VedicExpert
{
public:

	SarvatobhadraExpert( ContextProps_ptr props ) : VedicExpert( props ) {}

	void writeSbcText( MDocument_ptr );

protected:

};

#endif


