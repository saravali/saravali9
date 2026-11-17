/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Ashtakavarga.h
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
#ifndef _ASHTAKAVARGA_H_
#define _ASHTAKAVARGA_H_

#include "bootstrap.h"
#include "Expert.h"
#include "constants.h"
#include <wx/string.h>

enum class PlanetId;
enum class Viewpoint;

enum class PindaType
{
	None = -1,
	Graha = 0,
	Rasi,
	Yoga,
	First = Graha,
	Last = Yoga
};

/*************************************************//**
*
*  Encapsulates calculation of Ashtaka Varga
*
******************************************************/
class AshtakavargaExpert : public VedicExpert
{
public:

	AshtakavargaExpert( ContextProps_ptr props, const Varga &varga ) : VedicExpert( props, varga ) {}

	void update();

	wxString getAvTypeName( const AvType& ) const;

	wxString getPindaName( const PindaType& ) const;

	// get Ashtakavarga value for rasi, value is one of AvType::Rekha, Trikona or Ekadhi
	int getItem( const AvType&, const PlanetId&, const Sign& );

	// pinda value per planet
	int getPinda( const PindaType&, const PlanetId& ) const;

	// sum for each rasi, value is one of AvType::Rekha, Trikona or Ekadhi
	int getSignSarva( const AvType&, const Sign& ) const;

	// sum for each planet, value is one of AvType::Rekha, Trikona or Ekadhi
	int getPlanetSarva( const AvType&, const PlanetId& ) const;

	void write( MDocument_ptr, const bool show_header = false );

	void writeSingleAv( MDocument_ptr, const AvType& );

private:

	int objectIdToInt( const PlanetId& ) const;

	void clear();

	void calcRekha();
	void calcSarva();
	void calcPinda();
	void calcTrikonaShodana();
	void calcEkadhipatyaPair( const Sign &b1, const Sign &b2 );
	void calcEkadhipatyaShodana();
	int getSingleRekha( const int &i, const int &j, const int &k );

	int rekha[8][12], trikona[8][12], ekadhi[8][12];

	// sums per sign
	int sarvaRekha[12], sarvaTrikona[12], sarvaEkadhi[12];

	// sums per planet (incl. lagna)
	int psarvaRekha[8], psarvaTrikona[8], psarvaEkadhi[8];

	// pinda (weighted sum) per planet
	int grahaPinda[8], rasiPinda[8], yogaPinda[8];

	// planets per sign
	int planetCount[12];
};

#endif

