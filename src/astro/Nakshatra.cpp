/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Nakshatra.cpp
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

#include "Nakshatra.h"

#include "astrobase.h"
#include "ContextProperties.h"
#include "Formatter.h"
#include "Horoscope.h"
#include "Lang.h"
#include "mathbase.h"
#include "Table.h"

#include <math.h>
#include <assert.h>

/*****************************************************
**
**   NakshatraExpert   ---   getYoni
**
******************************************************/
Yoni NakshatraExpert::getYoni( const PlanetId &planetId, const HoroscopeRole role )
{
	assert( props );
	assert_valid_enum<PlanetId>( planetId );
	assert( props->getHoroscope( role ));

	const wxString k_yoni_name_sk[14] = { "Ashwa", "Mahish", "Simha", "Gaja",
		"Mesha", "Vanar", "Nakul", "Sarpa", "Mriga", "Swan",
		"Marjar", "Mushak", "Vyaghra"  "Gow" };
	const wxString k_yoni_name[14] = { tr( "Horse" ), tr( "Buffalo" ), tr( "Lion" ), tr( "Elephant" ),
		tr( "Goat" ), tr( "Monkey" ), tr( "Mongoose" ), tr( "Serpent" ), tr( "Deer" ), tr( "Dog" ), tr( "Cat" ),
		tr( "Rat" ), tr( "Tiger" ), tr( "Cow"  ) };

	const Nakshatra28 nakshatra = getNakshatra28( planetId, role );
	assert_valid_enum<Nakshatra28>( nakshatra );

	const static int yoni_map[28] =
	{
		0, 3, 4, 7, 7, 9, 10, 4, 10,
		11, 11, 13, 1, 12, 1, 12, 8, 8,
		9, 5, 6, 6, 5, 2, 0, 2, 13, 3
	};
	const int id = yoni_map[(int)nakshatra];
	const static int yoni_male[28] =
	{
		true, true, false, true, false, false, false, true, true,
		true, false, true, false, false, true, true, false, true,
		true, true, true,
		false, // Abhijit
		false, false, false, true, false, false
	};
	return Yoni( id, yoni_male[(int)nakshatra], std::format( "{} ({})", k_yoni_name[id], ( yoni_male[(int)nakshatra] ? "m" : "f" ) ));
}

/*****************************************************
**
**   NakshatraExpert   ---   getNadi
**
******************************************************/
Nadi NakshatraExpert::getNadi( const PlanetId &planetId, const HoroscopeRole role )
{
	assert( props );
	assert_valid_enum<PlanetId>( planetId );
	assert( props->getHoroscope( role ));
	const Nakshatra27 nakshatra = getNakshatra27( planetId, role );
	assert_valid_enum<Nakshatra27>( nakshatra );

	const int nadi_map[27] =
	{
		0, 1, 2, 2, 1, 0, 0, 1, 2,
		2, 1, 0, 0, 1, 2, 2, 1, 0,
		0, 1, 2, 2, 1, 0, 0, 1, 2
	};
	return (Nadi)nadi_map[(int)nakshatra];
}

/*****************************************************
**
**   NakshatraExpert   ---   getGana
**
******************************************************/
Gana NakshatraExpert::getGana( const PlanetId &planetId, const HoroscopeRole role )
{
	assert( props );
	assert_valid_enum<PlanetId>( planetId );
	assert( props->getHoroscope( role ));
	const Nakshatra27 nakshatra = getNakshatra27( planetId, role );
	assert_valid_enum<Nakshatra27>( nakshatra );

	const int gana_map[27] =
	{
		0, 1, 2, 1, 0, 1, 0, 0, 2,
		2, 1, 1, 0, 2, 0, 2, 0, 2,
		2, 1, 1, 0, 2, 2, 1, 1, 0
	};
	return (Gana)gana_map[(int)nakshatra];
}

/*****************************************************
**
**   NakshatraExpert   ---   getTara
**
******************************************************/
int NakshatraExpert::getTara( const Nakshatra27& nak )
{
	int n = (int)nak;
	if ( n < 0 ) n += 27;
	assert( n >= 0 && n < 27 );
	return n % 9;
}

/*****************************************************
**
**   NakshatraExpert   ---   getRajju
**
******************************************************/
Rajju NakshatraExpert::getRajju( const PlanetId &planetId, const HoroscopeRole role )
{
	assert_valid_enum<PlanetId>( planetId );
	Nakshatra27 nak = getNakshatra27( planetId, role );

	const int n1 = (int)nak % 9;
	Rajju rajju;

	if ( n1 < 4 )  // Aroha
	{
		rajju.aroha = 0;
		rajju.type = n1;
	}
	else if ( n1 == 4 )  // Siro
	{
		rajju.aroha = 1;
		rajju.type = 4;
	}
	else if ( n1 > 4 && n1 < 9 )  // Avaroha
	{
		rajju.aroha = 2;  
		rajju.type = 8 - n1;
	}
	else assert( false );

	return rajju;
}

/*****************************************************
**
**   NakshatraExpert   ---   getSbcNakshatraProperties
**
******************************************************/
std::vector<NakshatraProperties> NakshatraExpert::getSbcNakshatraProperties( const int &birthStar )
{
	std::vector<NakshatraProperties> nakprops;
	int nak27 = 0;  // Aswini

	for( int nak28 = 0; nak28 < 28; nak28++ )
	{
		NakshatraProperties p;
		if ( nak28 == ABHIJIT_INDEX )
		{
			p.tara = -1;
			p.taraQuality = InfluenceType::Neutral;
		}
		else
		{
			p.tara = getTara( Nakshatra27( nak27 - birthStar ));
			assert( p.tara >= 0 && p.tara < 9 );
			switch( p.tara )
			{
				case 1:
				case 3:
				case 5:
				case 7:
					p.taraQuality = InfluenceType::Benefic;
				break;
				case 0:
				case 2:
				case 4:
				case 6:
				case 8:
					p.taraQuality = InfluenceType::Malefic;
				break;
				default:
					assert( 0 );
				break;
			}
			nak27++;
		}
		nakprops.push_back( p );
	}
	return nakprops;
}


/*****************************************************
**
**   NakshatraExpert   ---   write
**
******************************************************/
void NakshatraExpert::write( MDocument_ptr doc, const bool show_header )
{
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );
	assert( doc );

	const Viewpoint viewpoint = getViewpoint();
	Formatter *formatter = Formatter::get();
	Lang lang;

	Table *table = new Table( show_header ? tr( "Nakshatras" ) : wxString() );
	table->addHeaderRow(
	{
		tr( "Planet" ),
		tr( "Longitude" ),
		tr( "Nakshatra" ),
		tr( "Yoni" ),
		tr( "Gana" ),
		tr( "Nadi" ),
		tr( "Tara" ),
		tr( "Rajju" )
	});

	for ( const PlanetId p : props->getPlanets( viewpoint, { FilterGroup::CardinalPoints, }))
	{
		const double len = getLongitude( p );
		TableRow row;
		row.addValue( MToken( p, viewpoint ));
		row.addValue( formatter->formatZodiacalLongitude( viewpoint, getLongitude( p ),
			isRetrograde( p ) ? MovingDirection::Retrograde : MovingDirection::None ));
		row.addValue( lang.getNakshatra27Name( ::getNakshatra27( len )));

		// This is not defined for 28 nakshatra mode, so leave it out
		const Yoni yoni = getYoni( p );
		row.addValue( yoni.name );
		row.addValue( lang.getGanaName( getGana( p )) );
		row.addValue( lang.getNadiName( getNadi( p )) );
		row.addValue( lang.getTaraName(
			getTara(  (Nakshatra27)((int)::getNakshatra27( len )
				- (int)::getNakshatra27( horoscope->getLongitude( PlanetId::Moon, viewpoint ))))));

		const Rajju rajju = getRajju( p );
		row.addValue( lang.getRajjuName( rajju.aroha, rajju.type ));
		table->addRow( row );
	}
	doc->addItem( table );
}

/*****************************************************
**
**   NakshatraExpert   ---   getVedhaNakshatras
**
******************************************************/
std::list<Nakshatra28> NakshatraExpert::getVedhaNakshatras( const Nakshatra28 &n )
{
	assert_valid_enum<Nakshatra28>( n );
	const static std::map<Nakshatra28, std::list<Nakshatra28>> nak_aspects =
	{
		{ Nakshatra28::Aswini, { Nakshatra28::Rohini, Nakshatra28::Poorvaphalguni, Nakshatra28::Jyeshta }},
		{ Nakshatra28::Bharani, { Nakshatra28::Krittika, Nakshatra28::Magha, Nakshatra28::Anuradha }},
		{ Nakshatra28::Krittika, { Nakshatra28::Visakha, Nakshatra28::Sravana, Nakshatra28::Bharani }},
		{ Nakshatra28::Rohini, { Nakshatra28::Swati, Nakshatra28::Abhijit, Nakshatra28::Aswini }},
		{ Nakshatra28::Mrigasira, { Nakshatra28::Chitra, Nakshatra28::Uttarashadha, Nakshatra28::Revati }},
		{ Nakshatra28::Ardra, { Nakshatra28::Hasta, Nakshatra28::Poorvashadha, Nakshatra28::Uttarabhadra }},
		{ Nakshatra28::Punarvasu, { Nakshatra28::Uttaraphalguni, Nakshatra28::Moola, Nakshatra28::Poorvabhadra }},
		{ Nakshatra28::Pushyami, { Nakshatra28::Poorvaphalguni, Nakshatra28::Jyeshta, Nakshatra28::Satabhisha }},
		{ Nakshatra28::Aslesha, { Nakshatra28::Magha, Nakshatra28::Anuradha, Nakshatra28::Dhanista }},
		{ Nakshatra28::Magha, { Nakshatra28::Sravana, Nakshatra28::Bharani, Nakshatra28::Aslesha }},
		{ Nakshatra28::Poorvaphalguni, { Nakshatra28::Abhijit, Nakshatra28::Aswini, Nakshatra28::Pushyami }},
		{ Nakshatra28::Uttaraphalguni, { Nakshatra28::Uttarashadha, Nakshatra28::Revati, Nakshatra28::Punarvasu }},
		{ Nakshatra28::Hasta, { Nakshatra28::Poorvashadha, Nakshatra28::Uttarabhadra, Nakshatra28::Ardra }},
		{ Nakshatra28::Chitra, { Nakshatra28::Moola, Nakshatra28::Poorvabhadra, Nakshatra28::Mrigasira }},
		{ Nakshatra28::Swati, { Nakshatra28::Jyeshta, Nakshatra28::Satabhisha, Nakshatra28::Rohini }},
		{ Nakshatra28::Visakha, { Nakshatra28::Anuradha, Nakshatra28::Dhanista, Nakshatra28::Krittika }},
		{ Nakshatra28::Anuradha, { Nakshatra28::Bharani, Nakshatra28::Aslesha, Nakshatra28::Visakha }},
		{ Nakshatra28::Jyeshta, { Nakshatra28::Aswini, Nakshatra28::Pushyami, Nakshatra28::Swati }},
		{ Nakshatra28::Moola, { Nakshatra28::Revati, Nakshatra28::Punarvasu, Nakshatra28::Chitra }},
		{ Nakshatra28::Poorvashadha, { Nakshatra28::Uttarabhadra, Nakshatra28::Ardra, Nakshatra28::Hasta }},
		{ Nakshatra28::Uttarashadha, { Nakshatra28::Poorvabhadra, Nakshatra28::Mrigasira, Nakshatra28::Uttaraphalguni }},
		{ Nakshatra28::Abhijit, { Nakshatra28::Satabhisha, Nakshatra28::Rohini, Nakshatra28::Poorvaphalguni }},
		{ Nakshatra28::Sravana, { Nakshatra28::Magha, Nakshatra28::Krittika, Nakshatra28::Dhanista }},
		{ Nakshatra28::Dhanista, { Nakshatra28::Aslesha, Nakshatra28::Visakha, Nakshatra28::Sravana }},
		{ Nakshatra28::Satabhisha, { Nakshatra28::Pushyami, Nakshatra28::Swati, Nakshatra28::Abhijit }},
		{ Nakshatra28::Poorvabhadra, { Nakshatra28::Punarvasu, Nakshatra28::Chitra, Nakshatra28::Uttarashadha }},
		{ Nakshatra28::Uttarabhadra, { Nakshatra28::Ardra, Nakshatra28::Hasta, Nakshatra28::Poorvashadha }},
		{ Nakshatra28::Revati, { Nakshatra28::Mrigasira, Nakshatra28::Uttaraphalguni, Nakshatra28::Moola }}
	};
	return nak_aspects.at( n );
}

/*****************************************************
**
**   SarvatobhadraExpert   ---   writeSbcText
**
******************************************************/
void SarvatobhadraExpert::writeSbcText( MDocument_ptr doc )
{
	Lang lang;
	//PlanetId i;
	double len;
	const Viewpoint viewpoint = getViewpoint();
	Formatter *formatter = Formatter::get();
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );

	const auto obs = props->getPlanets( viewpoint );
	Table *table = new Table( 3, obs.size() + 1 );
	table->setHeader( 0,  tr( "Planet" ));
	table->setHeader( 1,  tr( "Longitude" ));
	table->setHeader( 2,  tr( "Nakshatra" ));
	int line = 1;

	//for ( uint p = 0; p < obs.size(); p++ )
	for ( const PlanetId p : obs )
	{
		table->setEntry( 0, line, MToken( p, viewpoint ));
		len = getLongitude( p );
		table->setEntry( 1, line, formatter->formatZodiacalLongitude( viewpoint, len, getDirection( p )));
		table->setEntry( 2, line, lang.getNakshatra27Name( getNakshatra27( p )));
		line++;
	}
	doc->addItem( table );
}


