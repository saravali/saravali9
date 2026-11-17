/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Ashtakavarga.cpp
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

#include "Ashtakavarga.h"

#include "astroconfig.h"
#include "ContextProperties.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "constants.h"
#include "mathbase.h"
#include "MDocument.h"
#include "Table.h"

#include <format>

extern Astroconfig *astroconfig;

/*****************************************************
**
**   AshtakavargaExpert   ---   clear
**
******************************************************/
void AshtakavargaExpert::clear()
{
	int i, j;
	for ( i = 0; i < 8; i++ )
	{
		for ( j = 0; j < 12; j++ ) { rekha[i][j] = trikona[i][j] = ekadhi[i][j] = 0; }

		psarvaRekha[i] = psarvaTrikona[i] = psarvaEkadhi[i] = 0;
		rasiPinda[i] = grahaPinda[i] = yogaPinda[i] = 0;
	}
	for ( int i = 0; i < 12; i++ )
	{
		sarvaRekha[i] = sarvaTrikona[i] = sarvaEkadhi[i] = 0;
		planetCount[i] = 0;
	}
}

/*****************************************************
**
**   AshtakavargaExpert   ---   getAvTypeName
**
******************************************************/
wxString AshtakavargaExpert::getAvTypeName( const AvType &type ) const
{
	switch( type )
	{
		case AvType::Rekha:
			return tr( "Rekha" );
		break;
		case AvType:: Trikona:
			return tr( "Trikona Shodana" );
		break;
		case AvType:: Ekadhi:
			return tr( "Ekadhipatya Shodana" );
		break;
		case AvType:: RasiSama:
			return tr( "Rasi Sama" );
		break;
		case AvType:: NakshatraSama:
			return tr( "Nakshatra Sama" );
		break;
		default:
			assert( false );
		break;
	}
  return "<error>";
}

/*****************************************************
**
**   AshtakavargaExpert   ---   getPindaName
**
******************************************************/
wxString AshtakavargaExpert::getPindaName( const PindaType &type ) const
{
	switch( type )
	{
		case PindaType::Graha:
			return tr( "Graha Pinda" );
		break;
		case PindaType::Rasi:
			return tr( "Rasi Pinda") ;
		break;
		case PindaType::Yoga:
			return tr( "Yoga Pinda" );
		break;
		default:
			assert( false );
		break;
	}
  return "<error>";
}

/*****************************************************
**
**   AshtakavargaExpert   ---   update
**
******************************************************/
void AshtakavargaExpert::update()
{
	clear();

	calcRekha();
	calcTrikonaShodana();
	calcEkadhipatyaShodana();
	calcSarva();
	calcPinda();
}

/*****************************************************
**
**   AshtakavargaExpert   ---   getPinda
**
******************************************************/
int AshtakavargaExpert::getPinda( const PindaType &type, const PlanetId &p ) const
{
	assert( PlanetHelper::isAvPlanet( p ));
	const int i = objectIdToInt( p );
	switch ( type )
	{
	case PindaType::Graha:
		return grahaPinda[i];
		break;
	case PindaType::Rasi:
		return rasiPinda[i];
		break;
	case PindaType::Yoga:
		return yogaPinda[i];
		break;
	default:
		errorlog << "Error  AshtakavargaExpert::getPinda, type is " << (int)type;
		assert( false );
		break;
	}
	return 0;
}

/*****************************************************
**
**   AshtakavargaExpert   ---   objectIdToInt
**
******************************************************/
int AshtakavargaExpert::objectIdToInt( const PlanetId &p ) const
{
	assert( PlanetHelper::isAvPlanet( p ));
	return ( (int)p <= (int)PlanetId::Saturn ? (int)p : 7 );
}

/*****************************************************
**
**   AshtakavargaExpert   ---   getItem
**
******************************************************/
int AshtakavargaExpert::getItem( const AvType &type, const PlanetId &p, const Sign &rasi )
{
	const int i = objectIdToInt( p );
	switch ( type )
	{
		case AvType::Rekha:
			return rekha[i][(int)rasi];
		break;
		case AvType::Trikona:
			return trikona[i][(int)rasi];
		break;
		case AvType::Ekadhi:
			return ekadhi[i][(int)rasi];
		break;
		default:
			errorlog << "Error  AshtakavargaExpert::getItem, type is " << (int)type;
			assert( false );
		break;
	}
	return 0;
}

/*****************************************************
**
**   AshtakavargaExpert   ---   getSignSarva
**
******************************************************/
int AshtakavargaExpert::getSignSarva( const AvType &type, const Sign &rasi ) const
{
	assert_valid_enum<Sign>( rasi );
	switch ( type )
	{
		case AvType::Rekha:
			return sarvaRekha[(int)rasi];
		break;
		case AvType::Trikona:
			return sarvaTrikona[(int)rasi];
		break;
		case AvType::Ekadhi:
			return sarvaEkadhi[(int)rasi];
		break;
		default:
			errorlog << "AshtakavargaExpert::getSarva, type is "  << (int)type << " rasi " << (int)rasi;
			assert( false );
		break;
	}
	return 0;
}

/*****************************************************
**
**   AshtakavargaExpert   ---   getPlanetSarva
**
******************************************************/
int AshtakavargaExpert::getPlanetSarva( const AvType &type, const PlanetId &p ) const
{
	const int i = objectIdToInt( p );
	switch ( type )
	{
		case AvType::Rekha:
			return psarvaRekha[i];
		break;
		case AvType::Trikona:
			return psarvaTrikona[i];
		break;
		case AvType::Ekadhi:
			return psarvaEkadhi[i];
		break;
		default:
			errorlog << "AshtakavargaExpert::getPlanetSarva, type is " << (int)type;
			assert( false );
		break;
	}
	return 0;
}

/*****************************************************
**
**   AshtakavargaExpert   ---   write
**
******************************************************/
void AshtakavargaExpert::write( MDocument_ptr doc, const bool /*show_header*/ )
{
	writeSingleAv( doc, AvType::Rekha );
	writeSingleAv( doc, AvType::Trikona );
	writeSingleAv( doc, AvType::Ekadhi );

	Table *table = new Table( tr( "Pinda" ));
	table->addHeaderRow(
	{
		tr( "Planet" ),
		tr( "Rasi" ),
		tr( "Graha" ),
		tr( "Sodhya" )
	});

	for ( const PlanetId p : PlanetHelper::createAshtakaVargaPlanets())
	{
		//debuglog << "AV planet " << (int)p;
		TableRow row;
		row.addValue( MToken( p, getViewpoint() ), true );
		row.addValue( std::format( "{:02d}", getPinda( PindaType::Rasi, p )));
		row.addValue( std::format( "{:02d}", getPinda( PindaType::Graha, p )));
		row.addValue( std::format( "{:02d}", getPinda( PindaType::Yoga, p )));
		table->addRow( row );
	}
	doc->addItem( table );
}

/*****************************************************
**
**   AshtakavargaExpert   ---   writeSingleAv
**
******************************************************/
void AshtakavargaExpert::writeSingleAv( MDocument_ptr doc, const AvType &type )
{
	Table *table = new Table( 13 );
	table->setTitle( getAvTypeName( type ));

	{
		std::list<MString> headerRow = { MString() };
		for ( Sign i : Enum<Sign>() ) { headerRow.push_back( MToken( i, getViewpoint() )); }
		table->addHeaderRow( headerRow );
	}

	for ( const PlanetId p : PlanetHelper::createAshtakaVargaPlanets())
	{
		if ( ! PlanetHelper::isAvPlanet( p )) continue;

		TableRow row;
		row.addValue( MToken( p, getViewpoint() ), true );
		for ( Sign j : Enum<Sign>() )
		{
			row.addValue( std::format( "{:02d}", getItem( type, p, j )));
		}
		table->addRow( row );
	}

	// last row
	{
		TableRow row;
		row.addValue( tr( "Sarva" ), true );
		for ( const Sign j : Enum<Sign>() )
		{
			row.addValue( std::format( "{:02d}", getSignSarva( type, j )));
		}
		table->addRow( row );
	}

	doc->addItem( table );
}

/*****************************************************
**
**   AshtakavargaExpert   ---   getSingleRekha
**
******************************************************/
int AshtakavargaExpert::getSingleRekha( const int &i, const int &j, const int &k )
{
	assert( i >= 0 && i < 8 );
	assert( j >= 0 && j < 8 );
	assert( k >= 0 && k < 12 );

	static const char REKHA_MAP[8][8][12] =
	{
		{ // Sun
			{ 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0 }, // Sun
			{ 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0 }, // Moon
			{ 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1 }, // Mercury
			{ 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 }, // Venus
			{ 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0 }, // Mars
			{ 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0 }, // Jupiter
			{ 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0 }, // Saturn
			{ 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1 }  // Ascendant
		},
		{ // Moon
			{ 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0 }, // Sun
			{ 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0 }, // Moon
			{ 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0 }, // Mercury
			{ 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0 }, // Venus
			{ 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0 }, // Mars
			{ 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0 }, // Jupiter
			{ 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0 }, // Saturn
			{ 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0 }  // Ascendant
		},
		{ // Mercury
			{ 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1 }, // Sun
			{ 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0 }, // Moon
			{ 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1 }, // Mercury
			{ 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0 }, // Venus
			{ 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0 }, // Mars
			{ 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1 }, // Jupiter
			{ 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0 }, // Saturn
			{ 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0 }  // Ascendant
		},
		{  // Venus
			{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1 }, // Sun
			{ 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1 }, // Moon
			{ 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0 }, // Mercury
			{ 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0 }, // Venus
			{ 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1 }, // Mars
			{ 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0 }, // Jupiter
			{ 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0 }, // Saturn
			{ 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0 }  // Ascendant
		},
		{ // Mars
			{ 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0 }, // Sun
			{ 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0 }, // Moon
			{ 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0 }, // Mercury
			{ 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1 }, // Venus
			{ 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0 }, // Mars
			{ 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1 }, // Jupiter
			{ 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0 }, // Saturn
			{ 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0 }  // Ascendant
		},
		{ // Jupiter
			{ 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0 }, // Sun
			{ 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // Moon
			{ 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0 }, // Mercury
			{ 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0 }, // Venus
			{ 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0 }, // Mars
			{ 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0 }, // Jupiter
			{ 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1 }, // Saturn
			{ 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0 }  // Ascendant
		},
		{  // Saturn
			{ 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0 }, // Sun
			{ 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0 }, // Moon
			{ 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1 }, // Mercury
			{ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1 }, // Venus
			{ 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1 }, // Mars
			{ 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1 }, // Jupiter
			{ 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0 }, // Saturn
			{ 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0 }  // Ascendant
		},
		{  // Ascendant
			{ 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1 }, // Sun
			{ 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1 }, // Moon
			{ 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0 }, // Mercury
			{ 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 }, // Venus
			{ 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0 }, // Mars
			{ 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0 }, // Jupiter
			{ 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0 }, // Saturn
			{ 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0 }  // Ascendant
		}
	};
	// Varaha Mihira
	if ( astroconfig->ashtakavarga.ashtakavargaMode )
	{
		if ( i == (int)PlanetId::Moon )
		{
			if ( j == (int)PlanetId::Moon && k == (int)House::h9 ) return 0;     // Moon malefic in  9 from Moon
			if ( j == (int)PlanetId::Mars && k == (int)House::h9 ) return 1;     // Moon benefic in  9 from Mars
			if ( j == (int)PlanetId::Jupiter && k == (int)House::h2 ) return 0;  // Moon malefic in  2 from Jupiter
			if ( j == (int)PlanetId::Jupiter && k == (int)House::h12 ) return 1; // Moon benefic in 12 from Jupiter
		}
		else if ( i == (int)PlanetId::Venus )
		{
			if ( j == (int)PlanetId::Mars && k == (int)House::h4 ) return 0;     // Venus malefic in 4 from Mars
			if ( j == (int)PlanetId::Mars && k == (int)House::h5 ) return 1;     // Venus benefic in 5 from Mars
		}
	}
	return REKHA_MAP[i][j][k];
}

/*****************************************************
**
**   AshtakavargaExpert   ---   calcRekha
**
******************************************************/
void AshtakavargaExpert::calcRekha()
{
	int k, house;
	PlanetId p2;

	for ( int i = 0; i < 8; i++ )
	{
		//i < 7 ? p1 = i : p1 = PlanetId::Ascendant;
		for ( int j = 0; j < 8; j++ )
		{
			j < 7 ? p2 = (PlanetId)j : p2 = PlanetId::Ascendant;
			for ( k = 0; k < 12; k++ )
			{
				house = red12( (int)this->getRasi(p2) + k );
				if ( getSingleRekha( i, j, k ) ) rekha[i][house]++;
			}
		}
	}
}

/*****************************************************
**
**   AshtakavargaExpert   ---   calcSarva
**
******************************************************/
void AshtakavargaExpert::calcSarva()
{
	const int jmax = astroconfig->ashtakavarga.ashtakavargaSarva ? 8 : 7;
	for ( int i = 0; i < 12; i++ )
	{
		for ( int j = 0; j < jmax; j++ )
		{
			sarvaRekha[i] += rekha[j][i];
			sarvaTrikona[i] += trikona[j][i];
			sarvaEkadhi[i] += ekadhi[j][i];

			psarvaRekha[j] += rekha[j][i];
			psarvaTrikona[j] += trikona[j][i];
			psarvaEkadhi[j] += ekadhi[j][i];
		}
	}
}

/*****************************************************
**
**   AshtakavargaExpert   ---   calcPinda
**
******************************************************/
void AshtakavargaExpert::calcPinda()
{
	// Numbers for virgo and Capricorn are not clear: some say 5,  others 6, others vi 6 and cp 5
	// until 8.0
	//const int k_rasimana[12] = { 7, 10, 8, 4, 10, 6, 7, 8, 9, 5, 11, 12 };
	// 9.0
	const int k_rasimana[12] = { 7, 10, 8, 4, 10, 5, 7, 8, 9, 5, 11, 12 };

	// Grahamana: caution order
	// so=5 mo=5 ma=8 me=5 ju=10 ve=7 sa=5
	const int k_grahamana[7] = { 5, 5, 5, 7, 8, 10, 5 };

	for ( int planet = 0; planet < 8; planet++ )
	{
		for( int rasi = 0; rasi < 12; rasi++ )
		{
			rasiPinda[planet] += ekadhi[planet][rasi] * k_rasimana[rasi];

			// Sodhya Pinda:  0 = Parasara 1 = Mantreswar
			if ( astroconfig->ashtakavarga.ashtakavargaSodhyaPindaMode == 1 ) yogaPinda[planet] += ekadhi[planet][rasi];
		}
		for ( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
		{
			grahaPinda[planet] += ekadhi[planet][(int)this->getRasi(p)] * k_grahamana[(int)p];
		}

		// Sodhya Pinda: Parasara
		if ( astroconfig->ashtakavarga.ashtakavargaSodhyaPindaMode != 1 )
			yogaPinda[planet] = grahaPinda[planet] + rasiPinda[planet];
	}
}

/*****************************************************
**
**   AshtakavargaExpert   ---   calcTrikonaShodana
**
******************************************************/
void AshtakavargaExpert::calcTrikonaShodana()
{
	int minrekha;
	for ( int i = 0; i < 8; i++ )
	{
		for ( int j = 0; j < 4; j++ )
		{
			minrekha = std::min( rekha[i][j], rekha[i][j+4] );
			minrekha = std::min( rekha[i][j+8], minrekha );

			for ( int k = 0; k < 3; k++ ) trikona[i][j+4*k] = rekha[i][j+4*k] - minrekha;
		}
	}
}

/*****************************************************
**
**   AshtakavargaExpert   ---   calcEkadhipatyaPair
**
******************************************************/
void AshtakavargaExpert::calcEkadhipatyaPair( const Sign &r1, const Sign &r2 )
{
	const int rasi1 = (int)r1;
	const int rasi2 = (int)r2;
	for ( int p = 0; p < 8; p++ )
	{
		/*********************************************************
		*** (0)
		** one rasi has got a number in Trikona Shodana
		** and the other ist bereft of
		** any number -> nothing to be done
		***
		*********************************************************/
		if (( trikona[p][rasi1] > 0 && trikona[p][rasi2] == 0 ) ||
			( trikona[p][rasi1] == 0 && trikona[p][rasi2] > 0 ))
		{
		}

		/*********************************************************
		*** (1)
		** both rasis without a planet and Trikona Shodana different
		** -> both are given the smaller number
		***
		***  BUG FIX 5.0:
		***  -> No test for Trikona different, so condition (5) was never reached
		*********************************************************/
		else if (( ! planetCount[rasi1] ) && ( ! planetCount[rasi2] ) && ( trikona[p][rasi1] != trikona[p][rasi2]) )
		{
			ekadhi[p][rasi1] = ekadhi[p][rasi2] = std::min( trikona[p][rasi1], trikona[p][rasi2] );
		}

		/*********************************************************
		*** (2)
		** both rasis with planets -> no shodana
		***
		*********************************************************/
		else if (( planetCount[rasi1] ) && ( planetCount[rasi2] ))
		{
		}

		/*********************************************************
		*** (3)
		** one rasi is with a planet and a smaller trikona number
		** an the other is without a planet and a bigger trikona number
		** -> deduct the smaller number from the bigger number
		**  and the number of the rasi with planet should be kept unchanged
		***
		*********************************************************/
		else if (( planetCount[rasi1] ) && ( ! planetCount[rasi2] )
		         && ( trikona[p][rasi1] < trikona[p][rasi2]))
		{
			ekadhi[p][rasi2] -= trikona[p][rasi1];
		}

		else if (( planetCount[rasi2] ) && ( ! planetCount[rasi1] )
		         && ( trikona[p][rasi2] < trikona[p][rasi1]))
		{
			ekadhi[p][rasi1] -= trikona[p][rasi2];
		}

		/*********************************************************
		*** (4)
		** if the rasi with the planet has a bigger number than that
		** of the rasi without planet
		** -> shodana with the number of the rasi without planet
		** and the number of the rasi with planet schould be kept unchaged
		***
		*********************************************************/
		else if (( planetCount[rasi1] ) && ( ! planetCount[rasi2] )
		         && ( trikona[p][rasi1] > trikona[p][rasi2]))
		{
			ekadhi[p][rasi2] = 0;
		}
		else if (( planetCount[rasi2] ) && ( ! planetCount[rasi1] )
		         && ( trikona[p][rasi2] > trikona[p][rasi1]))
		{
			ekadhi[p][rasi1] = 0;
		}

		/*********************************************************
		*** (5)
		**  if both the rasis are without a planet and possess the
		**  same numbers
		**  -> shodana with both the numbers
		***
		*********************************************************/
		else if (( ! planetCount[rasi1] ) && ( ! planetCount[rasi2] )
		         && ( trikona[p][rasi1] == trikona[p][rasi2]))
		{
			ekadhi[p][rasi1] = ekadhi[p][rasi2] = 0;
		}

		/*********************************************************
		*** (6)
		** if one rasi is with an planet and the other is without
		** any planet
		** -> the number of the latter should be reduced to zero
		***
		*********************************************************/
		else if (( planetCount[rasi1] ) && ( ! planetCount[rasi2] ))
		{
			ekadhi[p][rasi2] = 0;
		}
		else if (( planetCount[rasi2] ) && ( ! planetCount[rasi1] ))
		{
			ekadhi[p][rasi1] = 0;
		}
	}
}

/*****************************************************
**
**   AshtakavargaExpert   ---   calcEkadhipatyaShodana
**
******************************************************/
void AshtakavargaExpert::calcEkadhipatyaShodana()
{
	// calculate number of planets in rasi
	for ( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
		planetCount[(int)this->getRasi(p)]++;

	for ( int i = 0; i < 8; i++ )
		for ( int j = 0; j < 12; j++ )
			ekadhi[i][j] = trikona[i][j];

	calcEkadhipatyaPair( Sign::Aries, Sign::Scorpio );
	calcEkadhipatyaPair( Sign::Taurus, Sign::Libra );
	calcEkadhipatyaPair( Sign::Gemini, Sign::Virgo );
	calcEkadhipatyaPair( Sign::Sagittarius, Sign::Pisces );
	calcEkadhipatyaPair( Sign::Capricorn, Sign::Aquarius );
}


