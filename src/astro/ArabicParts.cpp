/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/ArabicParts.cpp
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

#include "ArabicParts.h"
#include "ArabicPartBase.h"

#include "astrobase.h"
#include "constants.h"
#include "Formatter.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "MDocument.h"
#include "Table.h"

#include <format>

/*****************************************************
**
**   ArabicPartsExpert   ---   Constructor
**
******************************************************/
ArabicPartsExpert::ArabicPartsExpert( Horoscope* horoscope )
	: horoscope( horoscope )
{
	clear();
}

/*****************************************************
**
**   ArabicPartsExpert   ---   clear
**
******************************************************/
void ArabicPartsExpert::clear()
{
	pos.clear();
	dirty = true;
}

/*****************************************************
**
**   ArabicPartsExpert   ---   calculateDaybirth
**
******************************************************/
void ArabicPartsExpert::calculateDaybirth()
{
	assert( horoscope );

	daybirth = ( (int)horoscope->getHousePos( PlanetId::Sun, Viewpoint::Western ) >= (int)House::h7 );

	dirty = false;
}

/*****************************************************
**
**   ArabicPartsExpert   ---   getPartStep
**
******************************************************/
double ArabicPartsExpert::getPartStep( const ArabicPartSummand &sm, const Viewpoint& viewpoint )
{
	double len = 0;
	// longitude of element j
	switch ( sm.type )
	{
		// longitude of a planet (0..MAX_EPHEM_OBJECTS)
		case Arabic::LPlanet:
			assert( sm.getPlanetId() >= PlanetId::First && sm.getPlanetId() <= PlanetId::LastEphem );
			len = horoscope->getLongitude( sm.getPlanetId(), viewpoint );
		break;

		// longitude of other arabic part (only Fortune and Spirit used)
		case Arabic::LArabic:
			assert( sm.getPlanetId() >= PlanetId::FirstArabic && sm.getPlanetId() <= PlanetId::Last );
			len = horoscope->getLongitude( sm.getPlanetId(), viewpoint );
		break;

		// fixed zodiacal longitude (0..360) : ! NO Ayanamsa APPLIED!
		case Arabic::LFixed:
			ASSERT_VALID_DEGREE( sm.getLongitude() )
			len = (double)sm.getLongitude();
		break;

		// longitude of house cusp (0..11) = h
		case Arabic::LHousecusp:
			assert_valid_enum<House>( sm.getHouse() );
			len = horoscope->getHouseCusp( sm.getHouse(), viewpoint );
		break;

		// like above plus 10 deg (0..11) = j
		case Arabic::LHousecusp10:
			assert_valid_enum<House>( sm.getHouse() );
			len = horoscope->getHouseCusp( sm.getHouse(), viewpoint ) + 10;
		break;

		// longitude of lord of house cusp (0..11) = r
		case Arabic::LLord:
			assert_valid_enum<House>( sm.getHouse() );
			len = horoscope->getLongitude( getLord( getRasi( horoscope->getHouseCusp( sm.getHouse(), viewpoint ))), viewpoint );
		break;

		// longitude of Dispositor (0..MAX_EPHEM_OBJECTS) = D
		case Arabic::LDispositor:
		{
			assert( sm.getPlanetId() >= PlanetId::First && sm.getPlanetId() <= PlanetId::LastEphem );

			const double l = horoscope->getLongitude( sm.getPlanetId(), viewpoint );
			const Sign rasi = getRasi( l );
			const PlanetId lord = getLord( rasi, viewpoint );
			len = horoscope->getLongitude( lord, viewpoint );
		}
		break;

		// planet's house cusp (like Arabic::LHousecusp but reference to planet instead of house number),
		// (0..MAX_EPHEM_OBJECTS) = H
		case Arabic::LPlanetHousecusp:
			assert( sm.getPlanetId() >= PlanetId::First && sm.getPlanetId() <= PlanetId::LastEphem );
			len = horoscope->getHouseCusp(
				horoscope->getHouse4Longitude(
					horoscope->getLongitude( sm.getPlanetId(), viewpoint ),
				viewpoint ),
			viewpoint );
		break;

		// longitude of lord of a planet's house cusp (0..MAX_EPHEM_OBJECTS) = R
		case Arabic::LPlanetHousecuspLord:

			assert_valid_enum<PlanetId>( sm.getPlanetId() );
			len = horoscope->getLongitude(
				getLord(
					getRasi(
						horoscope->getHouseCusp(
							horoscope->getHouse4Longitude(
								horoscope->getLongitude( sm.getPlanetId(), viewpoint ),
							viewpoint ),
						viewpoint )),
					viewpoint ),
				viewpoint );
		break;
		default:
			assert( false );
		break;
	}
	return len;
}

/*****************************************************
**
**   ArabicPartsExpert   ---   updatePart
**
******************************************************/
double ArabicPartsExpert::updatePart( const PlanetId &p, const Viewpoint &viewpoint )
{
	assert( p >= PlanetId::FirstArabic && p <= PlanetId::LastArabic );
	const int index = (int)p - (int)PlanetId::FirstArabic;

	const ArabicPartDef def = ArabicPartsConfig::getPart( index );
	/*
	debuglog << "P " << (int)p << " index " << index << " objectid 1 " << (int)def.s1.getPlanetId();
	debuglog << "P " << (int)p << " index " << index << " objectid 2 " << (int)def.s2.getPlanetId();
	debuglog << "P " << (int)p << " index " << index << " objectid 3 " << (int)def.s3.getPlanetId();
	debuglog << "daybirth " << daybirth;
	*/

	const double l1 = getPartStep( def.s1, viewpoint );
	const double l2 = getPartStep( def.s2, viewpoint );
	const double l3 = getPartStep( def.s3, viewpoint );

	double l = 0;
	if ( def.flip && ! daybirth )
	{
		l = red_deg( l1 + l2 - l3 );
	}
	else
	{
		l = red_deg( l1 - l2 + l3 );
	}
	debuglog << "Arabic part " << index << " daybirth " << daybirth
		<< "l1 " << l1 << " l2 " << l2 << " l3 " << l3 << " result " << l;
	pos[ std::pair<Viewpoint, PlanetId>( viewpoint, p )] = l;
	return l;
}

/*****************************************************
**
**   ArabicPartsExpert   ---   getLongitude
**
******************************************************/
double ArabicPartsExpert::getLongitude( const PlanetId &p, const Viewpoint &viewpoint )
{
	assert( p >= PlanetId::FirstArabic && p <= PlanetId::LastArabic );

	if ( dirty ) calculateDaybirth();

	const auto l = pos.find( std::pair<Viewpoint, PlanetId>( viewpoint, p ) );
	if ( l != pos.end())
	{
		return l->second;
	}
	return updatePart( p, viewpoint );
}

/*****************************************************
**
**   ArabicPartsExpert   ---   getItemDescription
**
******************************************************/
wxString ArabicPartsExpert::getItemDescription( const ArabicPartSummand &sm, const Viewpoint &viewpoint )
{
	Lang lang;

	switch ( sm.type )
	{
		case Arabic::LPlanet:
		case Arabic::LArabic:
			return lang.getPlanetName( sm.getPlanetId(), viewpoint );
		break;
		case Arabic::LFixed:
			return std::format( "{} {}", (int)getRasiLen( (double)sm.getLongitude()  ),
				lang.getSignName( getRasi( (double)sm.getLongitude()), viewpoint ));
		break;

		case Arabic::LHousecusp:
			return std::format( "{} {}", tr( "Cusp" ), (int)sm.getHouse() +1 );
		break;
		case Arabic::LHousecusp10:
			return std::format( "({} {} +10)", tr( "Cusp" ), (int)sm.getHouse()+1 );
		break;
		case Arabic::LLord:
			return std::format( "{} {}", tr( "Lord of Cusp" ), (int)sm.getHouse()+1 );
		break;
		case Arabic::LDispositor:
			return std::format( "{} {}", tr( "Dispositor of" ), lang.getPlanetName( sm.getPlanetId(), viewpoint ));
		break;
		case Arabic::LPlanetHousecusp:
			return std::format( "{} {}", tr( "House Cusp" ), lang.getPlanetName( sm.getPlanetId(), viewpoint ));
		break;
		case Arabic::LPlanetHousecuspLord:
			return std::format( "{} {}", tr( "Lord of Cusp" ), lang.getPlanetName( sm.getPlanetId(), viewpoint ));
		break;

		default:
			assert( false );
		break;
	}
	return "";
}

/*****************************************************
**
**   ArabicPartsExpert   ---   write
**
******************************************************/
void ArabicPartsExpert::write( MDocument_ptr doc, const Viewpoint &viewpoint, const bool show_header )
{
	int index;
	double len;
	Formatter *formatter = Formatter::get();

	Table *table = new Table;
	table->addHeaderRow(
	{
		tr( "No." ),
		tr( "Name" ),
		tr( "Longitude" ),
		tr( "House" ),
		tr( "Rule" ),
		tr( "Flip" )
	});
	if ( show_header ) table->setTitle( tr("Arabic Parts" ));

#if 0
	for( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		debuglog << "OB " << (int)p;
	}
	for( const PlanetId p : Enum<PlanetId>( PlanetId::FirstArabic, PlanetId::LastArabic ))
	{
		debuglog << "ARA " << (int)p;
	}
#endif

	for( const PlanetId p : Enum<PlanetId>( PlanetId::FirstArabic, PlanetId::LastArabic ))
	{
		index = (int)p - (int)PlanetId::FirstArabic;
		const ArabicPartDef def = ArabicPartsConfig::getPart( index );

		TableRow row;
		row.addInteger( index + 1 );
		row.addValue( def.name );

		len = getLongitude( p, viewpoint );

		row.addValue( formatter->formatZodiacalLongitude( viewpoint, len ));
		row.addInteger( (int)horoscope->getHouse4Longitude( len, viewpoint ) + 1 );

		row.addValue( getItemDescription( def.s1, viewpoint )
			+ " - "
			+ getItemDescription( def.s2, viewpoint )
			+ " + "
			+ getItemDescription( def.s3, viewpoint ));

		if ( def.flip ) row.addValue( "x" );
		else row.addValue();
		table->addRow( row );
	}
	doc->addItem( table );
}


