/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Jaimini.h
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
#ifndef _JAIMINI_H_
#define _JAIMINI_H_

#include "bootstrap.h"
#include "constants.h"
#include "Expert.h"

enum class PlanetId;

/*************************************************//**
*
*  \brief Calculations and reports for Jaimini astrology
*
******************************************************/
class JaiminiExpert : public VedicExpert
{
public:

	JaiminiExpert( ContextProps_ptr props, const Varga &varga ) : VedicExpert( props, varga ) {}

	// update all
	void update();

	// update only padas
	int calcPada( const int& );

	// stronger of two rasis
	Sign getStrongerRasi( const Sign &rasi1, const Sign &rasi2 );

	// lord of a rasi (including rahu/ketu for aquarius/scorpio)
	PlanetId getJaiminiLord( const Sign& );

	// whether the nodes are stronger than saturn/mars for aq/sc
	bool isKetuStrongerThanMars();
	bool isRahuStrongerThanSaturn();

	// number of years for each rasi (0..12)
	int getRasiYears( const Sign& );

	void write( MDocument_ptr, const bool show_header = true );

	// calculation of chara karakas can be run standalone without update()
	void calcCharaKarakas();

	// which planet is atma karaka etc
	PlanetId getCharaKaraka( const CharaKaraka& );

	// chara karaka property for a planet
	CharaKaraka getCharaKarakaProperty( const PlanetId& );

private:

	double getRasiLongitude( const PlanetId& );

	int calcPlanetAspects( const PlanetId &planet );
	void calcYears();
	void calcNodeStrength();
	int calcSignAspects( const Sign &rasi );

	// number of planets per sign
	int nbplanets[12];

	// year assigned to a sign
	int rasiYears[12];

	// number of aspects on a certain sign
	int nbAspectsOnRasi[12];

	// Chara Karaka property per planet
	CharaKaraka p_chara_karaka[8];

	// PlanetId per Karaka, e.g. 0 -> Atma Karaka
	PlanetId chara_karaka[8];

	int aspSat, aspKet, aspMar, aspRah;
	bool ketuStrongerThanMars, rahuStrongerThanSaturn;
};

#endif

