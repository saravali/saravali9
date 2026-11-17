/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Shadbala.h
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

#ifndef _SHADBALA_H_
#define _SHADBALA_H_

#include "bootstrap.h"
#include "constants.h"
#include "Expert.h"
#include "Planet.h"
#include "Viewpoint.h"

#include <array>

enum class BalaId
{
	None = -1,
	Sthaana,
	Uchcha,
	Samasaptavargaja,
	Ojhajugma,
	Kendradi,
	Drekkana,
	Dig,
	Kala,
	Nathonatha,
	Paksha,
	Tribhaga,
	Varshamasadinahora,
	Ayana,
	Yudhdha,
	Drig,
	Cheshta,
	Naisargika,
	ShadTotal,
	ShadPercent,
	Speed,
	SpeedPercent,
	First = Sthaana,
	Last = SpeedPercent
};

// types of cheshta bala
enum class CheshtaBalaId
{
	None = -1,
	Vakra,
	Anuvakra,
	Vikala,
	Manda,
	Mandatara,
	Sama,
	Chara,
	Atichara,
	First = Vakra,
	Last = Atichara
};

/*************************************************//**
*
*  Calculates the six Balas of Vedic astrology
*
******************************************************/
class ShadbalaExpert : public VedicExpert
{
public:

	ShadbalaExpert( ContextProps_ptr props ) : VedicExpert( props ) {}

	virtual ~ShadbalaExpert() {}

	static wxString getBalaIdTitle( const BalaId& );

	static wxString getCheshtaBalaIdName( const CheshtaBalaId& );

	double getBala( const BalaId&, const PlanetId& );

	virtual void write( MDocument_ptr );

	void updateAllBalas();

	double getSaptavargajaBala( const PlanetId &planet, const int &vargaid ) const
	{
		assert( planet >= PlanetId::First && planet <= PlanetId::Saturn );
		assert( vargaid >= 0 && vargaid < 7 );
		return saptavargajabala[(int)planet][vargaid];
	}
	double getMaximumSaptavargajaBala() const { return max_saptavargajabala; }

	static const std::array<Varga, 7> sapta_vargas;

private:

	double bala[enum_count<BalaId>()][7];

	double saptavargajabala[7][7];
	double max_saptavargajabala;

	PlanetId varshalord, masalord, dinalord, horalord;
	double drigbala_pair[7][7];
	CheshtaBalaId cheshtabalatype[7];
	bool sunriseStatusOk;

	void clear();
	void updateSthaanaBala();
	void updateDigBala();
	void updateKalaBala();
	void updateNaisargikaBala();
	void updateCheshtaBala();
	void updateDrigBala();

	static const int required_shadbala[7];
	static const double circulation_time[7];
};

#endif // _SHADBALA_H_

