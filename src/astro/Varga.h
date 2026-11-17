/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Varga.h
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

#ifndef _VARGA_H_
#define _VARGA_H_

#include "bootstrap.h"
#include "constants.h"
#include "Viewpoint.h"

#include <list>

/*************************************************//**
*
*  \brief Varga properties of a single planet
*
******************************************************/
class VargaData
{
	friend class DignityExpert;

public:

	VargaData( const Varga& );

	Varga getDivision() const { return division; }

	Sign getRasi() const { return rasi; }

	int getVargaViswa() const { return vargaViswa; }

	int getSaptaVargajaBala() const { return saptaVargajaBala; }

	Dignity getDignity() const { return dignity; }

	PlanetaryFriendship getFriendship() const { return friendship; }

private:

	Varga division;

	Sign rasi;

	int vargaViswa, saptaVargajaBala;

	Dignity dignity;

	PlanetaryFriendship friendship;
};

/*************************************************//**
*
*  \brief Calculates Vargas and creates reports
*
******************************************************/
class VargaExpert
{
public:

	VargaExpert( Horoscope *h, const Viewpoint &viewpoint )
		: horoscope( h ),
		viewpoint( viewpoint )
	{}

	static int getVargaDivision( const Varga& );

  static Varga getVargaIndexByDivision( const int& );

	static wxString getVargaLabel( const Varga&, const TextFormat& );

	static std::list<Varga> getVargaIdList( const VargaGroup& );

	Sign calcVarga( const double& len, const Varga& );

	virtual void write( ContextProps_ptr, MDocument_ptr, const bool show_header = true );

private:

	// Dvadasamsa calculation plays a special role an is used by D-12, D-108 and D-144
	static double getDvadasamsaLongitude( const double& );

	Horoscope *horoscope = nullptr;

	const Viewpoint viewpoint = Viewpoint::None;
};

#endif


