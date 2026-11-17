/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Dignity.cpp
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

#include "Dignity.h"

#include "astrobase.h"
#include "astroconfig.h"
#include "logger.h"
#include "constants.h"
#include "Horoscope.h"
#include "Lang.h"
#include "Varga.h"

extern Astroconfig *astroconfig;

/*****************************************************
**
**   VedicPlanetData   ---   Constructor
**
******************************************************/
VedicPlanetData::VedicPlanetData()
{
	for( Varga v : Enum<Varga>() ) vargadata.emplace_back( v );
}

/*****************************************************
**
**   DignityExpert   ---   calcKpData
**
******************************************************/
KpData DignityExpert::calcKpData( const PlanetId &ob )
{
	const DasaId dasaindex = DasaId::Vimsottari;

	DasaExpert_ptr dexpert( DasaExpertFactory().getDasaExpert( dasaindex ));
	return dexpert->getKPLords( horoscope->getLongitude( ob, viewpoint ));
}

/*****************************************************
**
**   DignityExpert   ---   calcVargaData
**
******************************************************/
std::vector<VargaData> DignityExpert::calcVargaData( const PlanetId &ob )
{
	std::vector<VargaData> vdata;

	VargaExpert vexpert( horoscope, viewpoint );
	for ( Varga v : Enum<Varga>() )
	{
		VargaData data( v );
		data.rasi = vexpert.calcVarga( horoscope->getLongitude( ob, viewpoint ), v );
		vdata.push_back( data );
	}
	return vdata;
}

/*****************************************************
**
**   DignityExpert   ---   calculateDignities
**
******************************************************/
void DignityExpert::calculateDignities( const PlanetId &ob, std::vector<VargaData> &vargadata )
{
	if ( (int)ob > (int)PlanetId::Saturn ) return;

	assert( horoscope );
	VargaExpert expert( horoscope, viewpoint );
	Dignity dignity = Dignity::None;
	PlanetaryFriendship friendship = PlanetaryFriendship::None;
	int vargaViswa = 0;
	int bala = 0;
	PlanetaryFriendship tempFriend, planetaryFriend;
	Sign rasi, tfrasi, tflordrasi;

	for ( Varga varga : Enum<Varga>() )
	{
		//debuglog << "calculateDignities " << (int)varga;
		dignity = Dignity::None;
		rasi = horoscope->getVargaData( ob, varga, viewpoint ).getRasi();

		// tempFriendMode  0 = Sign, 1 = Varga
		tfrasi = horoscope->getVargaData( ob,
			astroconfig->varga.tempFriendMode == 1 ? varga : Varga::Rasi, viewpoint ).getRasi();

		tflordrasi = horoscope->getVargaData( getLord( rasi ),
			astroconfig->varga.tempFriendMode == 1 ? varga : Varga::Rasi, viewpoint ).getRasi();
		tempFriend = getRasiTemporaryFriendship( tfrasi, tflordrasi );
		planetaryFriend = getPlanetaryFriendship( ob, getLord( rasi));

		// Adhi Mitra
		if ( planetaryFriend == PlanetaryFriendship::Friend && tempFriend == PlanetaryFriendship::Friend )
		{
			friendship = PlanetaryFriendship::BestFriend;
			dignity = Dignity::BestFriend;
			vargaViswa = 18;
			bala = 20;
		}

		// Mitra
		else if ( planetaryFriend == PlanetaryFriendship::Neutral && tempFriend == PlanetaryFriendship::Friend )
		{
			friendship = PlanetaryFriendship::Friend;
			dignity = Dignity::Friend;
			vargaViswa = 15;
			bala = 15;
		}

		// Sama
		else if (( planetaryFriend == PlanetaryFriendship::Friend && tempFriend == PlanetaryFriendship::Enemy )
			|| ( planetaryFriend == PlanetaryFriendship::Enemy && tempFriend == PlanetaryFriendship::Friend ))
		{
			friendship = PlanetaryFriendship::Neutral;
			dignity = Dignity::Neutral;
			vargaViswa = 10;
			bala = 10;
		}

		// Satru
		else if ( planetaryFriend == PlanetaryFriendship::Neutral && tempFriend == PlanetaryFriendship::Enemy )
		{
			friendship = PlanetaryFriendship::Enemy;
			dignity = Dignity::Enemy;
			vargaViswa = 7;
			bala = 4;
		}

		// Adhi Satru
		else if ( planetaryFriend == PlanetaryFriendship::Enemy && tempFriend == PlanetaryFriendship::Enemy )
		{
			friendship = PlanetaryFriendship::SwornEnemy;
			dignity = Dignity::SwornEnemy;
			vargaViswa = 5;
			bala = 2;
		}

		/*
		* NB: Moon in Taurus is exalted AND in Moolatrikona (Varga Chart, e.g. D-9). So exaltation will prevail because of order below.
		* Same for Saturn in Aquarius (own and Moolatrikona): Moolatrikona will prevail.
		*/
		// Own sign
		if ( ob == getLord( rasi ))
		{
			dignity = Dignity::OwnSign;
			vargaViswa = 20;
		}

		if ( hasExaltationRasi( ob, rasi )) {
			dignity = Dignity::Exalted;
		}

		// Special handling for Moolatrikona: Sign depends on exact degree, other Vargas only on sign position
		if ( varga == Varga::Rasi )
		{
			if ( ::isInMulatrikona( ob, horoscope->getLongitude( ob, viewpoint )))
			{
				dignity = Dignity::Moolatrikona;
				bala = 45;
			}
		}
		else
		{
			if ( horoscope->getVargaData( ob, varga, viewpoint ).getRasi() == getMulatrikonaRasi( ob ))
			{
				dignity = Dignity::Moolatrikona;
				bala = 45;
			}
		}

		// Debilation
		if ( hasDebilationRasi( ob, rasi ))
		{
			dignity = Dignity::Debilated;
		}

		assert( dignity != Dignity::None );
		vargadata[(int)varga].dignity = dignity;
		vargadata[(int)varga].friendship = friendship;
		vargadata[(int)varga].vargaViswa = vargaViswa;
		vargadata[(int)varga].saptaVargajaBala = bala;
	}
}

/*****************************************************
**
**   DignityExpert   ---   calculateVimsopakaBala
**
******************************************************/
void DignityExpert::calculateVimsopakaBala( VedicPlanetData &planetdata )
{
	int i;
	const static Varga shadvarga_vargas[6] =
		{ Varga::Rasi, Varga::Hora, Varga::Drekkana, Varga::Navamsa, Varga::Dvadasamsa, Varga::Trimsamsa };
	const static Varga saptavarga_vargas[7] =
		{ Varga::Rasi, Varga::Hora, Varga::Drekkana, Varga::Navamsa, Varga::Dvadasamsa, Varga::Trimsamsa, Varga::Saptamamsa };
	const static Varga dasavarga_vargas[10] =
	{
		Varga::Rasi, Varga::Hora, Varga::Drekkana, Varga::Navamsa, Varga::Dvadasamsa, Varga::Trimsamsa,
		Varga::Saptamamsa, Varga::Dasamsa, Varga::Shodasamsa, Varga::Shastiamsa
	};
	const static Varga shodasavarga_vargas[16] =
	{ Varga::Rasi, Varga::Hora, Varga::Drekkana, Varga::Navamsa, Varga::Dvadasamsa, Varga::Trimsamsa,
		Varga::Saptamamsa, Varga::Dasamsa, Varga::Shodasamsa, Varga::Shastiamsa, Varga::Vimsamsa, Varga::Siddhamsa, Varga::Bhamsa,
		Varga::Chaturtamsa, Varga::Chatvarimsamsa, Varga::Akshavedamsa
	};

	const static double shadvarga_weight[6] = { 6, 2, 4, 5, 2, 1 };
	const static double saptavarga_weight[7] = { 5, 2, 3, 2.5, 4.5, 2, 1 };
	const static double dasavarga_weight[10] = { 3, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 5 };
	const static double shodasavarga_weight[16] = { 3.5, 1, 1, 3, .5, 1, .5, .5, 2, 4, .5, .5, .5, .5, .5, .5  };

	for ( i = 0; i < 4; i++ )
	{
		planetdata.vimsopakaBala[i] = 0;
		planetdata.vimsopakaBalaGoodVargas[i] = 0;
	}

	for ( i = 0; i < 6; i++ )  addVimsopakaValue( planetdata, shadvarga_weight[i], shadvarga_vargas[i], 0 );
	for ( i = 0; i < 7; i++ )  addVimsopakaValue( planetdata, saptavarga_weight[i], saptavarga_vargas[i], 1 );
	for ( i = 0; i < 10; i++ )	addVimsopakaValue( planetdata, dasavarga_weight[i], dasavarga_vargas[i], 2 );
	for ( i = 0; i < 16; i++ )	addVimsopakaValue( planetdata, shodasavarga_weight[i], shodasavarga_vargas[i], 3 );

	planetdata.averageVimsopakaBala = .25 * ( planetdata.vimsopakaBala[0] + planetdata.vimsopakaBala[1]
		+ planetdata.vimsopakaBala[2] + planetdata.vimsopakaBala[3] );
}

/*****************************************************
**
**   DignityExpert   ---   addVimsopakaValue
**
******************************************************/
void DignityExpert::addVimsopakaValue( VedicPlanetData &planetdata,
	const double &weight, const Varga& varga, const int &vindex )
{
	constexpr double minbala = 20.0;

	assert( vindex >= 0 && vindex < 4 );
	assert_valid_enum<Varga>( varga );
	assert( weight > 0 && weight < 10 );

	const double bala = weight * planetdata.vargadata[ (int)varga ].getVargaViswa();

	planetdata.vimsopakaBala[vindex] += .05 * bala;
	if ( planetdata.vargadata[ (int)varga ].getVargaViswa() >= minbala ) planetdata.vimsopakaBalaGoodVargas[vindex]++;
}


