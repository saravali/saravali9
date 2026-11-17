/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Ashtakoota.cpp
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

#include "Ashtakoota.h"

#include "astrobase.h"
#include "ContextProperties.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "MDocument.h"
#include "Table.h"

#include <format>

/*****************************************************
**
**   AshtakootaExpert   ---   init()
**
******************************************************/
void AshtakootaExpert::init()
{
	pvarna = pyoni = pmaitri = prasi = pgana = pnadi = prajju = 0;
	pvashya = ptara = ptotal = 0.0;
	rasi1 = rasi2 = Sign::None;
}

/*****************************************************
**
**   AshtakootaExpert   ---   calcYoniValue
**
******************************************************/
int AshtakootaExpert::calcYoniValue( const Yoni &y1, const Yoni &y2 )
{
	assert( y1.id >= 0 && y1.id < 14 );
	assert( y2.id >= 0 && y2.id < 14 );
	const static int yoni_map[14][14] =
	{
		{ 4, 0, 1, 2, 3, 2, 2, 1, 3, 2, 3, 3, 1, 3 },
		{ 0, 4, 1, 2, 3, 2, 2, 2, 3, 2, 2, 2, 1, 3 },
		{ 1, 1, 4, 0, 1, 2, 2, 2, 1, 1, 2, 1, 2, 1 },
		{ 2, 3, 0, 4, 2, 2, 2, 2, 2, 2, 3, 2, 1, 2 },
		{ 3, 3, 1, 2, 4, 0, 3, 1, 3, 2, 3, 2, 1, 3 },
		{ 2, 2, 2, 2, 0, 4, 2, 1, 2, 2, 2, 2, 1, 2 },
		{ 2, 2, 2, 2, 3, 2, 4, 0, 2, 1, 2, 2, 2, 2 },
		{ 1, 2, 2, 2, 1, 1, 0, 4, 1, 1, 1, 1, 2, 1 },
		{ 3, 2, 1, 2, 3, 2, 2, 1, 4, 0, 2, 2, 1, 2 },
		{ 2, 2, 1, 2, 2, 2, 1, 1, 0, 4, 1, 1, 1, 1 },
		{ 3, 2, 2, 3, 3, 2, 2, 1, 2, 1, 4, 0, 2, 2 },
		{ 3, 2, 1, 2, 2, 2, 2, 1, 2, 1, 0, 4, 2, 2 },
		{ 1, 1, 2, 1, 1, 1, 2, 2, 1, 1, 2, 2, 4, 0 },
		{ 3, 3, 1, 2, 3, 2, 2, 1, 2, 1, 2, 2, 0, 4 }
	};
	return yoni_map[y1.id][y2.id];
}

/*****************************************************
**
**   AshtakootaExpert   ---   calcRajjuValue
**
******************************************************/
int AshtakootaExpert::calcRajjuValue( const Rajju &rajju1, const Rajju &rajju2 )
{
	 int pr = 0;
	 // different Rajjus in Aroha
	 if ( rajju1.aroha == 0 && rajju2.aroha == 0 && rajju1.type != rajju2.type )
	 {
	 	pr = 4;
	 }

	 // different Rajjus, not both in Avaroha
	 else if ( rajju1.type != rajju2.type && ( rajju1.aroha != 2 || rajju2.aroha != 2 ) )
	 {
	 	pr = 3;
	 }

	 // same Rajju, one in Aroha, other in Avaroha
	 else if ( rajju1.type != rajju2.type && rajju1.aroha != rajju2.aroha )
	 {
	 	pr = 2;
	 }

	 // different Rajjus in Avaroha
	 else if ( rajju1.type != rajju2.type && rajju1.aroha == 2 && rajju2.aroha == 2 )
	 {
	 	pr = 1;
	 }

	 // same Rajju 
	 //else if ( rajju1.type == rajju2.type && rajju1.aroha == rajju2.aroha )
	 else if ( rajju1.type == rajju2.type )
	 {
	 	pr = 0;
	 }

	 else
	 {
			errorlog << "Error RAJJU1 aroha "
				<< rajju1.aroha
				<< " type "
				<< rajju1.type
				<< " Rajju2 aroha "
				<< rajju2.aroha
				<< " type "
				<< rajju2.type;
			assert( false );
	 }

	return pr;
}

/*****************************************************
**
**   AshtakootaExpert   ---   update
**
******************************************************/
void AshtakootaExpert::update()
{
	double len1, len2;
	const Viewpoint viewpoint = getViewpoint();
	init();

	Horoscope_ptr horoscope1 = props->getHoroscope( HoroscopeRole::Partner1 );
	assert( horoscope1 );
	Horoscope_ptr horoscope2 = props->getHoroscope( HoroscopeRole::Partner2 );
	assert( horoscope2 );

	len1 = horoscope1->getLongitude( PlanetId::Moon, viewpoint );
	len2 = horoscope2->getLongitude( PlanetId::Moon, viewpoint );
	rasi1 = ::getRasi( len1 );
	nak1 = ::getNakshatra27( len1 );
	rasi2 = ::getRasi( len2 );
	nak2 = ::getNakshatra27( len2 );

	/*
	 * Varna
	 */
	varna1 = getVarna( len1 );
	varna2 = getVarna( len2 );
	if ( varna1 >= varna2 )
	{
		pvarna = 1;
	}

	/*
	 * Vashya
	 */
	vashya1 = getVashya( len1 );
	vashya2 = getVashya( len2 );
	assert_valid_enum<Vashya>( vashya1 );
	assert_valid_enum<Vashya>( vashya2 );

	const double vashya_map[5][5] =
	{
		{ 2, 1, .5, 0, 1 },
		{ 0, 2, 1, 0, 1 },
		{ 0, 1, 2, 0, 1 },
		{ .5, .5, 1, 2, 0 },
		{ 0, 1, 1, 0, 2 }
	};
	pvashya = vashya_map[(int)vashya1][(int)vashya2];

	/*
	 * Dina or Tara
	 */
	int dnak1 = (int)nak2 - (int)nak1;
	if ( dnak1 < 0 ) dnak1 += 27;

	// Fix 8.2 (bug #83)
	//dnak1++;
	tara1 = dnak1 % 9;
	if (( tara1 == 3 ) || ( tara1 == 5 ) || ( tara1 == 7 ))
	{
		ptara  = 0;
	}
	else ptara = 1.5;

	int dnak2 = (int)nak1 - (int)nak2;
	if ( dnak2 < 0 ) dnak2 += 27;

	// Fix 8.2 (bug #83)
	//dnak2++;
	tara2 = dnak2 % 9;
	if (( tara2 == 3 ) || ( tara2 == 5 ) || ( tara2 == 7 ))
	{
	}
	else ptara += 1.5;

	/*
	 * Yoni
	 */
	yoni1 = getYoni( PlanetId::Moon, HoroscopeRole::Partner1 );
	yoni2 = getYoni( PlanetId::Moon, HoroscopeRole::Partner2 );
	pyoni = calcYoniValue( yoni1, yoni2 );

	/*
	 * Graha Maitri
	 */
	mitra1 = getPlanetaryFriendship( getLord( rasi1 ), getLord( rasi2 ) );
	mitra2 = getPlanetaryFriendship( getLord( rasi2 ), getLord( rasi1 ) );

	if ( mitra1 == PlanetaryFriendship::Friend && mitra2 == PlanetaryFriendship::Friend ) pmaitri = 5;
	else if (( mitra1 == PlanetaryFriendship::Friend && mitra2 == PlanetaryFriendship::Neutral )
		|| ( mitra2 == PlanetaryFriendship::Friend && mitra1 == PlanetaryFriendship::Neutral )) pmaitri = 4;
	else if (( mitra1 == PlanetaryFriendship::Neutral && mitra2 == PlanetaryFriendship::Neutral )
	         || ( mitra1 == PlanetaryFriendship::Friend && mitra2 == PlanetaryFriendship::Enemy )
					 || ( mitra1 == PlanetaryFriendship::Enemy && mitra2 == PlanetaryFriendship::Friend )) pmaitri = 2;
	else if (( mitra1 == PlanetaryFriendship::Enemy && mitra2 == PlanetaryFriendship::Neutral )
		|| ( mitra2 == PlanetaryFriendship::Enemy && mitra1 == PlanetaryFriendship::Neutral )) pmaitri = 1;
	else if ( mitra1 == PlanetaryFriendship::Enemy && mitra2 == PlanetaryFriendship::Enemy ) pmaitri = 0;
	else assert( 0 );

	/*
	 * Gana
	 */
	const int gana_map[3][3] =
	{
		{ 6, 5, 1 },
		{ 6, 6, 0 },
		{ 0, 0, 6 }
	};

	gana1 = NakshatraExpert::getGana( PlanetId::Moon, HoroscopeRole::Partner1 );
	assert_valid_enum<Gana>( gana1 );
	gana2 = NakshatraExpert::getGana( PlanetId::Moon, HoroscopeRole::Partner2 );
	assert_valid_enum<Gana>( gana1 );
	pgana = gana_map[(int)gana1][(int)gana2];

	/*
	 * Sign Koota (or Bhakoota)
	 */
	int rasidiff = red_sign( rasi1, rasi2 );
	if (( rasidiff == (int)House::h1 )
	        || ( rasidiff == (int)House::h3 )
	        || ( rasidiff == (int)House::h4 )
	        || ( rasidiff == (int)House::h7 )
	        || ( rasidiff == (int)House::h10 )
	        || ( rasidiff == (int)House::h11 ))
		prasi = 7;

	/*
	 * Nadi
	 */
	nadi1 = getNadi( PlanetId::Moon, HoroscopeRole::Partner1 );
	nadi2 = getNadi( PlanetId::Moon, HoroscopeRole::Partner2 );
	if ( nadi1 != nadi2 ) pnadi = 8;

	/*
	 * Rajju
	 */
	 rajju1 = getRajju( PlanetId::Moon, HoroscopeRole::Partner1 );
	 rajju2 = getRajju( PlanetId::Moon, HoroscopeRole::Partner2 );
	 prajju = calcRajjuValue( rajju1, rajju2 );

	/*
	 * Final
	 */
	ptotal = pvarna + pvashya + ptara + pyoni + pmaitri + prasi + pgana + pnadi + prajju;
}

/*****************************************************
**
**   AshtakootaExpert   ---   writeComprehensive
**
******************************************************/
void AshtakootaExpert::writeComprehensive( MDocument_ptr doc )
{
	Lang lang;
	wxString s;
	const Viewpoint viewpoint = getViewpoint();

	Table *table = new Table( 7, 11 );
	table->setHeader( 0, tr( "No." ));
	table->setHeader( 1, tr( "Koota" ));
	table->setHeader( 2, tr( "Description" ));
	table->setHeader( 3, tr( "Criterium" ));
	table->setHeader( 4, tr( "Groom" ));
	table->setHeader( 5, tr( "Bride" ));
	table->setHeader( 6, tr( "Result" ));

	for( int i = 1; i <= 9; i++ ) table->setEntry( 0, i, std::format( "{}", i ));

	//varna
	table->setEntry( 1, 1, tr( "Varna" ));
	table->setEntry( 2, 1, tr( "Social matching" ));
	table->setEntry( 3, 1, tr( "Caste associated with the Moon's sign" ));

	if ( is_valid_enum( varna1 ))
	{
		s = lang.getSignName( rasi1, viewpoint ) + "/" + lang.getVarnaName( varna1 );
		table->setEntry( 4, 1, s );
	}

	if ( is_valid_enum( varna2 ))
	{
		s = lang.getSignName( rasi2, viewpoint ) + "/" + lang.getVarnaName( varna2 );
		table->setEntry( 5, 1, s );
	}

	table->setEntry( 6, 1, std::format( "{}/1", pvarna ));

	// vashya
	table->setEntry( 1, 2, tr( "Vashya" ));
	table->setEntry( 2, 2, tr( "control, amenability" ));
	table->setEntry( 3, 2, tr( "Vashya of Moon's sign" ));

	if ( is_valid_enum( vashya1 ))
	{
		s = lang.getSignName( rasi1, viewpoint ) + "/" + lang.getVashyaName( vashya1 );
		table->setEntry( 4, 2, s );
	}

	if ( is_valid_enum( vashya2 ))
	{
		s = lang.getSignName( rasi2, viewpoint ) + "/" + lang.getVashyaName( vashya2 );
		table->setEntry( 5,	2, s );
	}

	table->setEntry( 6, 2, std::format( "{:.1f}/2", pvashya ));

	// Dina
	table->setEntry( 1, 3, "Dina (Tara)" );
	table->setEntry( 2, 3, "health, family happiness" );
	table->setEntry( 3, 3, "Tara of difference between Moon's Nakshatras" );
	table->setEntry( 4, 3, lang.getTaraName( tara1 ) );
	table->setEntry( 5, 3, lang.getTaraName( tara2 ) );

	table->setEntry( 6, 3, std::format( "{:.1f}/3", ptara ));

	// yoni
	table->setEntry( 1, 4, tr( "Yoni" ));
	table->setEntry( 2, 4, tr( "sexual attraction" ));
	table->setEntry( 3, 4, tr( "Animal symbol of the Moon's Nakshatra" ));

	s = lang.getNakshatra27Name( nak1 ) + " - " + yoni1.name;
	table->setEntry( 4, 4, s );

	s = lang.getNakshatra27Name( nak2 ) + " - " + yoni2.name;
	table->setEntry( 5, 4, s );

	table->setEntry( 6, 4, std::format( "{}/4", pyoni ));

	// graha
	table->setEntry( 1, 5, tr( "Graha" ));
	table->setEntry( 2, 5, tr( "mental affection" ));
	table->setEntry( 3, 5, tr( "Mutual friendship of the Moon's sign lords" ));
	table->setEntry( 4, 5, lang.getPlanetName( ::getLord( rasi1 ), viewpoint ));
	table->setEntry( 5, 5, lang.getPlanetName( ::getLord( rasi2 ), viewpoint ));

	table->setEntry( 6, 5, std::format( "{}/5", pmaitri ));

	// gana
	table->setEntry( 1, 6, tr( "Gana" ));
	table->setEntry( 2, 6, tr( "character matching" ));
	table->setEntry( 3, 6, tr( "Moon's Nakshatra" ));

	s = lang.getNakshatra27Name( nak1 ) + " - " + lang.getGanaName( gana1 );
	table->setEntry( 4, 6, s );

	s = lang.getNakshatra27Name( nak2 ) + " - " + lang.getGanaName( gana2 );
	table->setEntry( 5, 6, s );

	table->setEntry( 6, 6, std::format( "{}/6", pgana ));

	// rasi / bha
	table->setEntry( 1, 7, tr( "Sign (Bha)" ));
	table->setEntry( 2, 7, tr( "general indications" ));
	table->setEntry( 3, 7, tr( "Difference between the Moon's signs" ));

	table->setEntry( 4, 7, lang.getSignName( rasi1, viewpoint ) );
	table->setEntry( 5, 7, lang.getSignName( rasi2, viewpoint ) );

	table->setEntry( 6, 7, std::format( "{}/7", prasi ));

	// nadi
	table->setEntry( 1, 8, tr( "Nadi" ));
	table->setEntry( 2, 8, tr( "physiological matching, offspring" ));
	table->setEntry( 3, 8, tr( "Moon's sign" ));

	s = lang.getSignName( rasi1, viewpoint ) + " - " + lang.getNadiName( nadi1 );
	table->setEntry( 4, 8, s );

	s = lang.getSignName( rasi2, viewpoint ) + " - " + lang.getNadiName( nadi2 );
	table->setEntry( 5, 8, s );

	table->setEntry( 6, 8, std::format( "{}/8", pnadi ));

	// rajju
	table->setEntry( 1, 9, tr( "Rajju" ));
	table->setEntry( 2, 9, tr( "general indications" ));
	table->setEntry( 3, 9, tr( "Moon's Nakshatra" ));

	s = lang.getNakshatra27Name( nak1 ) + " - " + lang.getRajjuName( rajju1.aroha, rajju1.type );
	table->setEntry( 4, 9, s );

	s = lang.getNakshatra27Name( nak2 ) + " - " + lang.getRajjuName( rajju2.aroha, rajju2.type );
	table->setEntry( 5, 9, s );

	table->setEntry( 6, 9, std::format( "{}/4", prajju ));

	table->setHeaderEntry( 0, 10, "" );
	table->setHeaderEntry( 1, 10, "" );
	table->setHeaderEntry( 2, 10, "" );
	table->setHeaderEntry( 3, 10, "" );
	table->setHeaderEntry( 4, 10, "Total" );

	table->setHeaderEntry( 5, 10, std::format( "{:.1f}%", .5 + 100.0 * ptotal / 40.0 ));
	table->setHeaderEntry( 6, 10, std::format( "{:.1f}/40", ptotal ));

	doc->addItem( table );
}

/*****************************************************
**
**   AshtakootaExpert   ---   write
**
******************************************************/
void AshtakootaExpert::write( MDocument_ptr doc )
{
	Lang lang;
	const Viewpoint viewpoint = getViewpoint();
	wxString s;

	Horoscope_ptr horoscope1 = props->getHoroscope( HoroscopeRole::Partner1 );
	assert( horoscope1 );
	Horoscope_ptr horoscope2 = props->getHoroscope( HoroscopeRole::Partner2 );
	assert( horoscope2 );

	int perc = (int)(100 * (double)ptotal / 40);

	//writer->addHeader( tr( "Vedic Partner Chart" ));

	s = tr( "Groom" ) + ": " + horoscope1->dl.name;
	doc->addLine( s );
	s = tr( "Bride" ) + ": " + horoscope2->dl.name;
	doc->addParagraph( s );

	Table *table = new Table( 2, 11 );
	table->setHeader( 0, tr( "Quality" ));
	table->setHeader( 1, tr( "Points" ));

	table->setEntry( 0, 1, tr( "Varna" ));
	table->setEntry( 1, 1, std::format( "{}/1", pvarna ));

	table->setEntry( 0, 2, tr( "Vashya" ));
	table->setEntry( 1, 2, std::format( "{:.1f}/2", pvashya ));

	table->setEntry( 0, 3, tr( "Tara" ));
	table->setEntry( 1, 3, std::format( "{:.1f}/3", ptara ));

	table->setEntry( 0, 4, tr( "Yoni" ));
	table->setEntry( 1, 4, std::format( "{}/4", pyoni ));

	table->setEntry( 0, 5, tr( "Graha Maitri" ));
	table->setEntry( 1, 5, std::format( "{}/5", pmaitri ));

	table->setEntry( 0, 6, tr( "Gana" ));
	table->setEntry( 1, 6, std::format( "{}/6", pgana ));

	table->setEntry( 0, 7, tr( "Bhakoota" ));
	table->setEntry( 1, 7, std::format( "{}/7", prasi ));

	table->setEntry( 0, 8, tr( "Nadi" ));
	table->setEntry( 1, 8, std::format( "{}/8", pnadi ));

	table->setEntry( 0, 9, tr( "Rajju" ));
	table->setEntry( 1, 9, std::format( "{}/4", prajju ));

	table->setHeaderEntry( 0, 10, tr( "Total" ));
	table->setHeaderEntry( 1, 10, std::format( "{:.1f} ({}%)/40", ptotal, perc ));

	doc->addItem( table );

	// Details about Moon #1
	doc->addHeader2( tr( "Groom" ));

	double mlen1 = horoscope1->getLongitude( PlanetId::Moon, viewpoint );
	Table *t1 = new Table( 2, 7 );
	t1->setHeader( 0, tr( "Quality" ));
	t1->setHeader( 1, tr( "Value" ));

	t1->setEntry( 0, 1, tr( "Nakshatra" ));
	t1->setEntry( 1, 1, lang.getNakshatra27Name( ::getNakshatra27( mlen1 )));


	t1->setEntry( 0, 2, tr( "Varna" ));
	t1->setEntry( 1, 2, lang.getVarnaName( getVarna( mlen1 )));

	Yoni yoni1 = getYoni( PlanetId::Moon, HoroscopeRole::Partner1 );
	t1->setEntry( 0, 3, tr( "Yoni" ));
	t1->setEntry( 1, 3, yoni1.name );

	t1->setEntry( 0, 4, tr( "Gana" ));
	t1->setEntry( 1, 4, lang.getGanaName( getGana( PlanetId::Moon, HoroscopeRole::Partner1 )));

	t1->setEntry( 0, 5, tr( "Nadi" ));
	t1->setEntry( 1, 5, lang.getNadiName( getNadi( PlanetId::Moon, HoroscopeRole::Partner1 )));

	t1->setEntry( 0, 6, tr( "Rajju" ));
	t1->setEntry( 1, 6, lang.getRajjuName( rajju1.aroha, rajju1.type ));

	doc->addItem( t1 );

	// Details about Moon #2
	doc->addHeader2( tr( "Bride" ));

	double mlen2 = horoscope2->getLongitude( PlanetId::Moon, viewpoint );
	Table *t2 = new Table( 2, 7 );
	t2->setHeader( 0, tr( "Quality" ));
	t2->setHeader( 1, tr( "Value" ));

	t2->setEntry( 0, 1, tr( "Nakshatra" ));
	t2->setEntry( 1, 1, lang.getNakshatra27Name( ::getNakshatra27( mlen2 )));

	t2->setEntry( 0, 2, tr( "Varna" ));
	t2->setEntry( 1, 2, lang.getVarnaName( getVarna( mlen2 )));

	Yoni yoni2 = getYoni( PlanetId::Moon, HoroscopeRole::Partner2 );
	t2->setEntry( 0, 3, tr( "Yoni" ));
	t2->setEntry( 1, 3, yoni2.name );

	t2->setEntry( 0, 4, tr( "Gana" ));
	t2->setEntry( 1, 4, lang.getGanaName( getGana( PlanetId::Moon, HoroscopeRole::Partner2  )));

	t2->setEntry( 0, 5, tr( "Nadi" ));
	t2->setEntry( 1, 5, lang.getNadiName( getNadi( PlanetId::Moon, HoroscopeRole::Partner2 )));

	t2->setEntry( 0, 6, tr( "Rajju" ));
	t2->setEntry( 1, 6, lang.getRajjuName( rajju2.aroha, rajju2.type ));

	doc->addItem( t2 );
}


