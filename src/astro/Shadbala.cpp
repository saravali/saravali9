/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Shadbala.cpp
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

#include "Shadbala.h"

#include "astrobase.h"
#include "Calculator.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "Hora.h"
#include "Horoscope.h"
#include "Formatter.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "MDocument.h"
#include "Table.h"
#include "VedicAspect.h"
#include "Varga.h"

#include <math.h>
#include <format>

// required shadbala in virupas
// su=390 mo=360 ma=300 me=420 ju=390 ve=330 sa=300
const int ShadbalaExpert::required_shadbala[7] = { 390, 360, 420, 330, 300, 390, 300 };

// circulation time of the objects in years, used by cheshta bala calculation
//su=1, mo=.082, ma=1.88, me=.24, ju=11.86, ve=.62, sa=29.46
const double ShadbalaExpert::circulation_time[7] = { 1, .082, .24, .62, 1.88, 11.86, 29.46 };

const std::array<Varga, 7> ShadbalaExpert::sapta_vargas =
{
	Varga::Rasi,
	Varga::Navamsa,
	Varga::Hora,
	Varga::Drekkana,
	Varga::Saptamamsa,
	Varga::Dvadasamsa,
	Varga::Trimsamsa
};

/*****************************************************
**
**   ShadbalaExpert   ---   clear
**
******************************************************/
void ShadbalaExpert::clear()
{
	for( int b = 0; b < enum_count<BalaId>(); ++b )
	{
		for( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
		{
			bala[b][(int)p] = 0.0;
		}
	}

	for ( int i = 0; i < 7; i++ )
	{
		varshalord = masalord = dinalord = horalord = PlanetId::None;
		cheshtabalatype[i] = CheshtaBalaId::None;
		for ( int j = 0; j < 7; j++ )
		{
			drigbala_pair[i][j] = 0;
			saptavargajabala[i][j] = 0;
		}
	}
	max_saptavargajabala = 0;
	sunriseStatusOk = true;
}

/*****************************************************
**
**   ShadbalaExpert   ---   getBala
**
******************************************************/
double ShadbalaExpert::getBala( const BalaId &type, const PlanetId &p )
{
	assert_valid_enum<BalaId>( type );
	assert( p >= PlanetId::Sun && p <= PlanetId::Saturn );
	return bala[(int)type][(int)p];
}

/*****************************************************
**
**   ShadbalaExpert   ---   getBalaIdTitle
**
******************************************************/
wxString ShadbalaExpert::getBalaIdTitle( const BalaId &type )
{
	static std::map<BalaId, wxString> bala_map =
	{
		{ BalaId::Sthaana, "Sthaana Summary" },
		{ BalaId::Uchcha, "Uchcha" },
		{ BalaId::Samasaptavargaja, "Saptavargaja" },
		{ BalaId::Ojhajugma, "Ojhajugma" },
		{ BalaId::Kendradi, "Kendradi" },
		{ BalaId::Drekkana, "Drekkana" },
		{ BalaId::Dig, "Dig" },
		{ BalaId::Kala, "Kala" },
		{ BalaId::Nathonatha, "Nathonatha" },
		{ BalaId::Paksha, "Paksha" },
		{ BalaId::Tribhaga, "Tribhaga" },
		{ BalaId::Varshamasadinahora, "Varsha-Masa-Dina-Hora" },
		{ BalaId::Ayana, "Ayana" },
		{ BalaId::Yudhdha, "Yudhdha" },
		{ BalaId::Drig, "Drig" },
		{ BalaId::Cheshta, "Cheshta" },
		{ BalaId::Naisargika, "Naisargika" },
		{ BalaId::ShadTotal, "Shadbala Total" },
		{ BalaId::ShadPercent, "Shadbala Percent" },
		{ BalaId::Speed, "Speed" },
		{ BalaId::SpeedPercent, "Speed (Percent)" }
	};
	return bala_map[type];
}

/*****************************************************
**
**   ShadbalaExpert   ---   getCheshtaBalaIdName
**
******************************************************/
wxString ShadbalaExpert::getCheshtaBalaIdName( const CheshtaBalaId &id )
{
	static std::map<CheshtaBalaId, wxString> cheshta_map =
	{
		{ CheshtaBalaId::None, tr( "None" ) },
		{ CheshtaBalaId::Vakra, tr( "Vakra" ) },
		{ CheshtaBalaId::Anuvakra, tr( "Anuvakra" ) },
		{ CheshtaBalaId::Vikala, tr( "Vikala" ) },
		{ CheshtaBalaId::Manda, tr( "Manda" ) },
		{ CheshtaBalaId::Mandatara, tr( "Mandatara" ) },
		{ CheshtaBalaId::Sama, tr( "Sama" ) },
		{ CheshtaBalaId::Chara, tr( "Chara" ) },
		{ CheshtaBalaId::Atichara, tr( "Atichara" ) }
	};
	return cheshta_map[id];
}

/*****************************************************
**
**   ShadbalaExpert   ---   updateAllBalas
**
******************************************************/
void ShadbalaExpert::updateAllBalas()
{
	clear();
	updateSthaanaBala();
	updateDigBala();
	updateKalaBala();
	updateNaisargikaBala();
	updateCheshtaBala();
	updateDrigBala();

	for( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		bala[(int)BalaId::Sthaana][(int)p] =
			getBala( BalaId::Uchcha, p )
			+ getBala( BalaId::Samasaptavargaja, p )
			+ getBala( BalaId::Ojhajugma, p )
			+ getBala( BalaId::Kendradi, p )
			+ getBala( BalaId::Drekkana, p );
		bala[(int)BalaId::Kala][(int)p] = 
			getBala( BalaId::Nathonatha, p )
			+ getBala( BalaId::Paksha, p )
			+ getBala( BalaId::Tribhaga, p )
			+ getBala( BalaId::Varshamasadinahora, p )
			+ getBala( BalaId::Ayana, p )
			+ getBala( BalaId::Yudhdha, p );
		bala[(int)BalaId::ShadTotal][(int)p] =
			getBala( BalaId::Sthaana, p )
			+ getBala( BalaId::Dig, p )
			+ getBala( BalaId::Kala, p )
			+ getBala( BalaId::Cheshta, p )
			+ getBala( BalaId::Naisargika, p )
			+ getBala( BalaId::Drig, p );
		bala[(int)BalaId::ShadPercent][(int)p] = getBala( BalaId::ShadTotal, p ) * 100.0 / required_shadbala[(int)p];
	}
}

/*****************************************************
**
**   ShadbalaExpert   ---   updateSthaanaBala
**
******************************************************/
void ShadbalaExpert::updateSthaanaBala()
{
	int drekkana;
	double a;
	Horoscope_ptr horoscope1 = props->getRadixHoroscope();
	assert( horoscope1 );

	// Exaltation points
	//su=10 mo=33 ma=298 me=165 ju=95 ve=357 sa=200 ra=285 ke=225
	const static int k_exaltation[9] = { 10, 33, 165, 357, 298, 95, 200, 285, 225 };

	// Uchcha Bala
	for( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		a = red_deg( k_exaltation[(int)p] - getLongitude( p ) - 180 );
		if ( a > 180 ) a = 360 - a;
		bala[(int)BalaId::Uchcha][(int)p] = a / 3;
	}

	// Sapta Vargaja Bala
	for ( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		for( int v = 0; v < 7; ++v )
		{
			saptavargajabala[(int)p][v] = getVargaData( p, sapta_vargas[v] ).getSaptaVargajaBala();
			bala[(int)BalaId::Samasaptavargaja][(int)p] += saptavargajabala[(int)p][v];
			max_saptavargajabala = std::max( max_saptavargajabala, saptavargajabala[(int)p][v] );
		}
	}

	auto rasiGetter = [&]( const PlanetId &p, const Varga &varga) ->int
	{
		return (int)getVargaData( p, varga ).getRasi();
	};

	// Ojhajugmariamsa Bala
	for( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		//double q = chart.getLongitude( p );
		if ( p == PlanetId::Venus || p == PlanetId::Moon )
		{
			if ( rasiGetter( p, Varga::Rasi ) % 2 ) bala[(int)BalaId::Ojhajugma][(int)p] += 15;
			if ( rasiGetter( p, Varga::Navamsa ) % 2 ) bala[(int)BalaId::Ojhajugma][(int)p] += 15;
		}
		else
		{
			if ( rasiGetter( p, Varga::Rasi ) % 2 > 0 ) bala[(int)BalaId::Ojhajugma][(int)p] += 15;
			if ( rasiGetter( p, Varga::Navamsa ) % 2 > 0 ) bala[(int)BalaId::Ojhajugma][(int)p] += 15;
		}
	}

	// Kendradi Bala
	const Sign lagna = getRasi(  PlanetId::Ascendant );
	for ( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		const Sign prasi = getRasi( p );
		switch ( red12( (int)prasi - (int)lagna ) % 3 )
		{
		case 0: // Kendra
			bala[(int)BalaId::Kendradi][(int)p] = 60;
			break;
		case 1:
			bala[(int)BalaId::Kendradi][(int)p] = 30;
			break;
		default:
			bala[(int)BalaId::Kendradi][(int)p] = 15;
			break;
		}
	}

	// Drekkana Bala
	for( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		a = getRasiLen( getLongitude( p )) / 10.0;
		drekkana = (int)a;
		if ( p == PlanetId::Sun || p == PlanetId::Mars || p == PlanetId::Jupiter )
		{
			if ( drekkana == 0 ) bala[(int)BalaId::Drekkana][(int)p] = 15;
		}
		else if ( p == PlanetId::Venus || p == PlanetId::Moon )
		{
			if ( drekkana == 1 ) bala[(int)BalaId::Drekkana][(int)p] = 15;
		}
		else if ( p == PlanetId::Saturn || p == PlanetId::Mercury )
		{
			if ( drekkana == 2 ) bala[(int)BalaId::Drekkana][(int)p] = 15;
		}
		else assert( false );
	}
}

/*****************************************************
**
**   ShadbalaExpert   ---   updateDigBala
**
******************************************************/
void ShadbalaExpert::updateDigBala()
{
	double weakestHouse = 0;;
	double housepos;
	const Viewpoint viewpoint = getViewpoint();
	Horoscope_ptr horoscope1 = props->getRadixHoroscope();
	assert( horoscope1 );

	for( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		switch ( p )
		{
		case PlanetId::Sun:
		case PlanetId::Mars:
			weakestHouse = (int)House::h4;
			break;
		case PlanetId::Jupiter:
		case PlanetId::Mercury:
			weakestHouse = (int)House::h7;
			break;
		case PlanetId::Moon:
		case PlanetId::Venus:
			weakestHouse = (int)House::h10;
			break;
		case PlanetId::Saturn:
			weakestHouse = (int)House::h1;
			break;
		default:
			break;
		}
		housepos = horoscope1->getHouse4LongitudeDouble( getLongitude( p ), viewpoint );

		// distance of planet from weakest house, range 0..6
		//int b = red12( (int)housepos - (int)weakestHouse );
		double b = a_red( housepos - weakestHouse, 12 );
		debuglog << "updateDigBala p " << p << " weakestHouse " << weakestHouse << " b " << b;
		if ( b > 6 ) b = 12 - b;

		bala[(int)BalaId::Dig][(int)p] = (int)((b * 10.0) + .5 );
	}
}

/*****************************************************
**
**   ShadbalaExpert   ---   updateKalaBala
**
******************************************************/
void ShadbalaExpert::updateKalaBala()
{
	double paskha_value, natabala, sunrise, sunset, d;
	double eps, tlen, lat, kranti;
	int daypart, nightpart;
	bool daybirth;

	Horoscope_ptr horoscope1 = props->getRadixHoroscope();
	assert( horoscope1 );

	Calculator *calculator = CalculatorFactory().getCalculator();
	const Viewpoint viewpoint = getViewpoint();

	// Nathonahta Bala
	bala[(int)BalaId::Nathonatha][(int)PlanetId::Mercury] = 60;

	natabala = horoscope1->dl.getLocalMeanTime();
	if ( natabala > 12 ) natabala = 24 - natabala;
	natabala *= 5;

	bala[(int)BalaId::Nathonatha][(int)PlanetId::Moon]
		= bala[(int)BalaId::Nathonatha][(int)PlanetId::Mars]
		= bala[(int)BalaId::Nathonatha][(int)PlanetId::Saturn]
		= natabala;
	bala[(int)BalaId::Nathonatha][(int)PlanetId::Sun]
		= bala[(int)BalaId::Nathonatha][(int)PlanetId::Jupiter]
		= bala[(int)BalaId::Nathonatha][(int)PlanetId::Venus]
		= 60 - natabala;

	// pakshabala
	paskha_value = planetDistance( getLongitude( PlanetId::Sun ),
		getLongitude( PlanetId::Moon )) / 3;

	bala[(int)BalaId::Paksha][(int)PlanetId::Sun]= 60 - paskha_value;
	bala[(int)BalaId::Paksha][(int)PlanetId::Moon]= ( horoscope1->isBenefic( PlanetId::Moon, viewpoint )
		? paskha_value : 60 - paskha_value );
	bala[(int)BalaId::Paksha][(int)PlanetId::Mars]= 60 - paskha_value;
	bala[(int)BalaId::Paksha][(int)PlanetId::Mercury]= ( horoscope1->isBenefic( PlanetId::Mercury, viewpoint )
		? paskha_value : 60 - paskha_value );
	bala[(int)BalaId::Paksha][(int)PlanetId::Jupiter]= paskha_value;
	bala[(int)BalaId::Paksha][(int)PlanetId::Venus]= paskha_value;
	bala[(int)BalaId::Paksha][(int)PlanetId::Saturn]= 60 - paskha_value;

	// tribhagabala
	sunrise = horoscope1->getSunrise();
	sunset = horoscope1->getSunset();
	sunriseStatusOk = ( sunrise != 0 && sunset != 0 );
	debuglog << "sunrise " << sunrise << " jd " << horoscope1->getJD() << " sunset " << sunset
		<< " status " << sunriseStatusOk;

	if ( sunriseStatusOk )
	{
		bala[(int)BalaId::Tribhaga][(int)PlanetId::Jupiter] = 60; // Jupiter has always Tribhaga Bala of 1 Rupa
		daybirth = horoscope1->getDayBirth();
		if ( daybirth )
		{
			assert( sunrise < horoscope1->getJD() && sunset > horoscope1->getJD());
			d =  ( horoscope1->getJD() - sunrise ) / ( sunset - sunrise );
			daypart = (int)( d * 3 );
			if ( daypart == 0 ) {
				bala[(int)BalaId::Tribhaga][(int)PlanetId::Mercury] = 60;
			}
			else if ( daypart == 1 )
			{
				bala[(int)BalaId::Tribhaga][(int)PlanetId::Sun] = 60;
			}
			else if ( daypart == 2 )
			{
				bala[(int)BalaId::Tribhaga][(int)PlanetId::Saturn] = 60;
			}
		}
		else
		{
			assert( sunrise > horoscope1->getJD() && sunset < horoscope1->getJD());
			d =  ( horoscope1->getJD() - sunset ) / ( sunrise - sunset );
			nightpart = (int)( d * 3 );
			if ( nightpart == 0 )
			{
				bala[(int)BalaId::Tribhaga][(int)PlanetId::Moon] = 60;
			}
			else if ( nightpart == 1 )
			{
				bala[(int)BalaId::Tribhaga][(int)PlanetId::Venus] = 60;
			}
			else if ( nightpart == 2 )
			{
				bala[(int)BalaId::Tribhaga][(int)PlanetId::Mars] = 60;
			}
		}
	}

	// Lords of year, month, day and hour
	HoraExpert hexpert;
	hexpert.location = horoscope1->dl.location;
	hexpert.update( horoscope1->getJD() );

	varshalord = hexpert.getVarshaLord();
	bala[(int)BalaId::Varshamasadinahora][(int)varshalord] += 15;

	masalord = hexpert.getMasaLord();
	bala[(int)BalaId::Varshamasadinahora][(int)masalord] += 30;

	dinalord = hexpert.getDinaLord();
	bala[(int)BalaId::Varshamasadinahora][(int)dinalord] += 45;

	horalord = hexpert.getHoraLord();
	bala[(int)BalaId::Varshamasadinahora][(int)horalord] += 60;

	// Ayana Bala
	calculator->calcEps( eps, horoscope1->getJD());
	for( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		bala[(int)BalaId::Ayana][(int)p] = eps;

		tlen = horoscope1->getTropicalLongitude( p );
		lat = horoscope1->getLatitude( p, viewpoint );
		kranti = lat + eps * sin( DEG2RAD * tlen );

		switch ( p )
		{
		case PlanetId::Mercury:
			bala[(int)BalaId::Ayana][(int)p] += fabs( kranti );
			break;
		case PlanetId::Moon:
		case PlanetId::Saturn:
			bala[(int)BalaId::Ayana][(int)p] -= kranti;
			break;
		default:
			bala[(int)BalaId::Ayana][(int)p] += kranti;
			break;
		}

		bala[(int)BalaId::Ayana][(int)p] *= 1.2793;
		bala[(int)BalaId::Ayana][(int)p] = std::max( getBala( BalaId::Ayana, p ), 0.0 );
	}

	// yudhdhabala 
	{
		// start with kalabala aquired so far
		// in case of planetary war the difference of both values must be added to the winner, substractcted from the looser
		double kb[7];
		for( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
		{
			kb[(int)p] = getBala( BalaId::Nathonatha, p )
				+ getBala( BalaId::Paksha, p )
				+ getBala( BalaId::Tribhaga, p )
				+ getBala( BalaId::Varshamasadinahora, p )
				+ getBala( BalaId::Ayana, p );
		}
		double dist, shadbaladiff;
		PlanetId pwinner, ploser;
		double len1, len2;

		for( const PlanetId p1 : Enum<PlanetId>( PlanetId::Mercury, PlanetId::Saturn ))
		{
			for( const PlanetId p2 : Enum<PlanetId>( p1, PlanetId::Saturn ))
			{
				if ( p1 == p2 ) continue;

				len1 = getLongitude( p1 );
				len2 = getLongitude( p2 );
				dist = planetDistance( len1, len2 );

				if ( dist < 1 )
				{
					shadbaladiff = fabs( kb[(int)p1] - kb[(int)p2] );

					// winner is the planet with higher longitude
					if (( len1 > len2 ) || ( len1 < 1 && len2 >= 359 ))
					{
						pwinner = p1;
						ploser = p2;
					}
					else
					{
						pwinner = p2;
						ploser = p1;
					}
					bala[(int)BalaId::Yudhdha][ (int)pwinner ] = shadbaladiff;
					bala[(int)BalaId::Yudhdha][ (int)ploser ] = -shadbaladiff;
				}
			}
		}
	}
}

/*****************************************************
**
**   ShadbalaExpert   ---   updateNaisargikaBala
**
******************************************************/
void ShadbalaExpert::updateNaisargikaBala()
{
	// Order: sun, moon, venus, jupiter, mercury, mars, saturn
	const int nbala_base[7] = { 0, 1, 4, 2, 5, 3, 6 };
	for( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		bala[(int)BalaId::Naisargika][(int)p] = (double)( 7 - nbala_base[(int)p] ) * 60.0 / 7.0;
	}
}

/*****************************************************
**
**   ShadbalaExpert   ---   updateCheshtaBala
**
******************************************************/
void ShadbalaExpert::updateCheshtaBala()
{
	int count;
	Sign rasi, baserasi;
	double len, lat, medium_speed, jd, speed1;
	Calculator *calculator = CalculatorFactory().getCalculator();
	const Viewpoint viewpoint = getViewpoint();

	Horoscope_ptr horoscope1 = props->getRadixHoroscope();
	assert( horoscope1 );

	DateLocation ds = horoscope1->dl;

	bala[(int)BalaId::Cheshta][(int)PlanetId::Sun] = getBala( BalaId::Ayana, PlanetId::Sun );
	bala[(int)BalaId::Cheshta][(int)PlanetId::Moon] = getBala( BalaId::Paksha, PlanetId::Moon );
	for( const PlanetId ob : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		const int p = (int)ob;
		bala[(int)BalaId::Speed][p] = horoscope1->getLSpeed( ob, viewpoint );
		medium_speed = 1 / circulation_time[p];
		bala[(int)BalaId::SpeedPercent][p] = 100 * getBala( BalaId::Speed, ob ) / medium_speed;
		if ( getBala( BalaId::Speed, ob ) > 0 ) // direct motion
		{
			if ( fabs( getBala( BalaId::SpeedPercent, ob ) ) < 10 ) // Vikala, devoid of motion
			{
				cheshtabalatype[p] = CheshtaBalaId::Vikala;
				bala[(int)BalaId::Cheshta][p] = 15;
			}
			else if ( getBala( BalaId::SpeedPercent, ob ) < 50 ) // Mandatara
			{
				cheshtabalatype[p] = CheshtaBalaId::Mandatara;
				bala[(int)BalaId::Cheshta][p] = 15;
			}
			else if ( getBala( BalaId::SpeedPercent, ob ) < 100 ) // Manda
			{
				cheshtabalatype[p] = CheshtaBalaId::Manda;
				bala[(int)BalaId::Cheshta][p] = 30;
			}
			else if ( getBala( BalaId::SpeedPercent, ob ) < 150 ) // Sama
			{
				cheshtabalatype[p] = CheshtaBalaId::Sama;
				bala[(int)BalaId::Cheshta][p] = 7.5;
			}
			else if ( getBala( BalaId::SpeedPercent, ob ) >= 150 ) // Chara
			{
				cheshtabalatype[p] = CheshtaBalaId::Chara;
				bala[(int)BalaId::Cheshta][p] = 45;

				// Atichara
				baserasi = getRasi( ob );
				count = 0;
				jd = horoscope1->getJD();
				speed1 = getBala( BalaId::Speed, ob );
				while ( speed1 > 0 && count++ < 90 )
				{
					jd += 3.0;
					ds.date.jd = jd;
					calculator->calcPositionSpeed( ds, ob, len, lat, speed1, viewpoint );
					rasi = ::getRasi( len );
					if ( rasi != baserasi )
					{
						cheshtabalatype[p] = CheshtaBalaId::Atichara;
						bala[(int)BalaId::Cheshta][p] = 30;
						break;
					}
				}
			}
		}
		else
		{
			cheshtabalatype[p] = CheshtaBalaId::Vakra;
			bala[(int)BalaId::Cheshta][p] = 60;

			// anuvakra
			baserasi = getRasi( ob );
			count = 0;
			jd = horoscope1->getJD();
			speed1 = getBala( BalaId::Speed, ob );
			while ( speed1 < 0 && count++ < 90 )
			{
				jd += 3.0;
				ds.date.jd = jd;
				calculator->calcPositionSpeed( ds, ob, len, lat, speed1, viewpoint );
				rasi = ::getRasi( len );
				if ( rasi != baserasi )
				{
					cheshtabalatype[p] = CheshtaBalaId::Anuvakra;
					bala[(int)BalaId::Cheshta][p] = 30;
					break;
				}
			}
		}
	}
}

/*****************************************************
**
**   ShadbalaExpert   ---   updateDrigBala
**
******************************************************/
void ShadbalaExpert::updateDrigBala()
{
	int rasidiff;
	const Viewpoint viewpoint = getViewpoint();
	Horoscope_ptr horoscope1 = props->getRadixHoroscope();
	assert( horoscope1 );

	for( const PlanetId aspectingPlanet : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		for( const PlanetId aspectedPlanet : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
		{
			if ( aspectingPlanet == aspectedPlanet ) continue;

			rasidiff = red_sign( getRasi( aspectedPlanet ),
				getRasi( aspectingPlanet ));

			drigbala_pair[(int)aspectedPlanet][(int)aspectingPlanet]
				= 60.0 * VedicAspectExpert::getGrahaDrishtiValue( aspectingPlanet, rasidiff );

			if ( drigbala_pair[(int)aspectedPlanet][(int)aspectingPlanet] > 0 )
			{
				const bool maleficAspect = ( aspectingPlanet == PlanetId::Sun
			        || ( aspectingPlanet == PlanetId::Moon && ! horoscope1->isBenefic( PlanetId::Moon, viewpoint ))
			        || aspectingPlanet == PlanetId::Mars
			        || ( aspectingPlanet == PlanetId::Mercury && ! horoscope1->isBenefic( PlanetId::Mercury, viewpoint ))
			        || aspectingPlanet == PlanetId::Saturn );

				if ( maleficAspect ) drigbala_pair[(int)aspectedPlanet][(int)aspectingPlanet] -= 15;
				else drigbala_pair[(int)aspectedPlanet][(int)aspectingPlanet] += 15;
			}

			bala[(int)BalaId::Drig][(int)aspectedPlanet] += drigbala_pair[(int)aspectedPlanet][(int)aspectingPlanet];
		}
	}
}

/*****************************************************
**
**   ShadbalaExpert   ---   write
**
******************************************************/
void ShadbalaExpert::write( MDocument_ptr doc )
{
	Formatter *formatter = Formatter::get();
	Lang lang;
	const Viewpoint viewpoint = getViewpoint();
	wxString s;

	// get correct order for planets
	const Planets obs = PlanetHelper::createVedicOnlyPlanets();

	Table *t0 = new Table( tr( "Six Balas" ));
	t0->addHeaderRow(
	{
		"", // planets
		tr( "Sthana" ),
		tr( "Dig" ),
		tr( "Kala" ),
		tr( "Cheshta" ),
		tr( "Naisargika" ),
		tr( "Drik" ),
		"", // empty
		tr( "Rupas" ),
		tr( "Virupas" ),
		tr( "Required" ),
		tr( "Strength (Percent)" ),
		"" // planets
	});

	for ( const PlanetId p : obs )
	{
		TableRow row;
		row.addValue( MToken( p, viewpoint ));

		row.addValue( std::format( "{}", (int)getBala( BalaId::Sthaana, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Dig, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Kala, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Cheshta, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Naisargika, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Drig, p )));

		row.addValue( "" );
		row.addValue( std::format( "{:.1f}", getBala( BalaId::ShadTotal, p ) / 60.0 ));
		row.addValue( std::format( "{}", (int)getBala( BalaId::ShadTotal, p )));
		row.addInteger( required_shadbala[(int)p] );

		s = std::format( "{:.1f}", getBala( BalaId::ShadPercent, p )) + "%";
		row.addValue( s );

		row.addValue( MToken( p, viewpoint ));
		t0->addRow( row );
	}
	doc->addItem( t0 );

	Table *t1 = new Table( tr( "Sthana Bala" ));
	t1->addHeaderRow(
	{
		"", // planets
		tr( "Uchcha" ),
		tr( "Saptavargaja" ),
		tr( "Ojhayugma" ),
		tr( "Kendradi" ),
		tr( "Drekkana" ),
		"",  // empty
		tr( "Rupas" ),
		tr( "Virupas" ),
		"" // planets
	});

	for ( const PlanetId p : obs )
	{
		TableRow row;
		row.addValue( MToken( p, viewpoint ));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Uchcha, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Samasaptavargaja, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Ojhajugma, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Kendradi, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Drekkana, p )));

		row.addValue( "" );
		row.addValue( std::format( "{:.1f}", getBala( BalaId::Sthaana, p ) / 60.0 ));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Sthaana, p )));

		row.addValue( MToken( p, viewpoint ));
		t1->addRow( row );
	}
	doc->addItem( t1 );

	Table *t2 = new Table( tr( "Saptavargaja Bala" ));
	t2->addHeaderRow(
	{
		"", // planets
		"D-1", "D-9", "D-2", "D-3", "D-7", "D-12", "D-30",
		"", // empty
		tr( "Total Virupas" ),
		"" // planets
	});

	for ( const PlanetId p : obs )
	{
		TableRow row;
		row.addValue( MToken( p, viewpoint ));

		for ( int v = 0; v < 7; v++ ) // 7 Vargas
		{
			row.addValue( std::format( "{}", (int)getSaptavargajaBala( p, v )));
		}
		row.addValue( "" );
		row.addValue( std::format( "{}", (int)getBala( BalaId::Samasaptavargaja, p )));
		row.addValue( MToken( p, viewpoint ));
		t2->addRow( row );
	}
	doc->addItem( t2 );

	Table *t3 = new Table( tr( "Kala Bala" ));
	t3->addHeaderRow(
	{
		"", // planets
		tr( "Nathonatha" ),
		tr( "Paksha" ),
		tr( "Tribhaga" ),
		tr( "Varsha-Masa-Dina-Hora" ),
		tr( "Ayana" ),
		tr( "Yudhdha" ),
		"", // empty
		tr( "Total Rupas" ),
		tr( "Virupas" ),
		"" // planets
	});

	for ( const PlanetId p : obs )
	{
		TableRow row;
		row.addValue( MToken( p, viewpoint ));

		row.addValue( std::format( "{}", (int)getBala( BalaId::Nathonatha, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Paksha, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Tribhaga, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Varshamasadinahora, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Ayana, p )));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Yudhdha, p )));
		row.addValue( "" );

		row.addValue( std::format( "{:.1f}", getBala( BalaId::Kala, p ) / 60.0 ));
		row.addValue( std::format( "{}", (int)getBala( BalaId::Kala, p )));
		row.addValue( MToken( p, viewpoint ));
		t3->addRow( row );
	}
	doc->addItem( t3 );

	if ( ! sunriseStatusOk ) doc->addLine( tr( "Remark: no Tribhaga Bala, sunrise and sunset not found." ));

	s = tr( "Lord of year" ) + ": " + lang.getPlanetName( varshalord, viewpoint );
	doc->addLine( s );

	s = tr( "Lord of month" ) + ": " + lang.getPlanetName( masalord, viewpoint );
	doc->addLine( s );

	s = tr( "Lord of day" ) + ": " + lang.getPlanetName( dinalord, viewpoint );
	doc->addLine( s );

	s = tr( "Lord of hour" ) + ": " + lang.getPlanetName( horalord, viewpoint );
	doc->addLine( s );

	Table *t4 = new Table( tr( "Cheshta Bala" ));
	t4->addHeaderRow(
	{
		"", // planets
		tr( "Speed (deg/day)" ),
		tr( "Medium Speed" ),
		tr( "Ratio" ),
		tr( "Type" ),
		tr( "Virupas" )
	});

	for ( const PlanetId p : obs )
	{
		if ( p == PlanetId::Sun || p == PlanetId::Moon ) continue; // no such Bala for Sun and Moon

		TableRow row;
		row.addValue( MToken( p, viewpoint ));
		row.addValue( formatter->formatDegrees( getBala( BalaId::Speed, p ), DegPrecision::Second ) );
		row.addValue( formatter->formatDegrees( 1 / circulation_time[(int)p], DegPrecision::Second ) );
		row.addValue( std::format( "{:.1f}", getBala( BalaId::SpeedPercent, p )));
		row.addValue( getCheshtaBalaIdName( cheshtabalatype[(int)p] ));
		row.addValue( std::format( "{:.1f}", bala[(int)BalaId::Cheshta][(int)p] ));
		t4->addRow( row );
	}
	doc->addItem( t4 );

	std::list<MString> headerContents = { MString( "" ) };
	for ( const PlanetId p : obs ) headerContents.push_back( MToken( p, viewpoint ));
	Table *t5 = new Table( tr( "Drig Bala" ));
	t5->addHeaderRow( headerContents );

	for ( const PlanetId p : obs )
	{
		TableRow row;
		row.addValue( MToken( p, viewpoint ) );
		for ( const PlanetId q : obs )
		{
			if ( p == q )
			{
				row.addValue( "" );
			}
			else
			{
				row.addValue( std::format( "{:02d}", (int)drigbala_pair[(int)q][(int)p ]));
			}
		}
		t5->addRow( row );
	}
	{ // last row containing the sums
		TableRow row;
		row.addValue( tr( "Sum" ));
		for ( const PlanetId p : obs )
		{
			row.addValue( std::format( "{:02d}", (int)getBala( BalaId::Drig, p )));
		}
		t5->addRow( row );
	}
	doc->addItem( t5 );
}

