/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Varga.cpp
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

#include "Varga.h"


#include "astrobase.h"
#include "astroconfig.h"
#include "constants.h"
#include "ContextProperties.h"
#include "Dignity.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "MDocument.h"
#include "Table.h"

#include <math.h>
#include <format>

extern Astroconfig *astroconfig;

/*****************************************************
**
**   VargaData   ---   Constructor
**
******************************************************/
VargaData::VargaData( const Varga &d )
	:	division( d )
{
	rasi = Sign::None;
	dignity = Dignity::None;
	friendship = PlanetaryFriendship::None;
	vargaViswa = saptaVargajaBala = -1;
}

/*****************************************************
**
**   VargaExpert   ---   getVargaDivision
**
******************************************************/
int VargaExpert::getVargaDivision( const Varga &i )
{
  assert_valid_enum<Varga>( i );
  return vargaDivisions[(int)i];
}

/*****************************************************
**
**   VargaExpert   ---   getVargaIndexByDivision
**
******************************************************/
Varga VargaExpert::getVargaIndexByDivision( const int &div )
{
  int j = (int)Varga::First;

  // nonsense or non-initialized
  assert(( div >= 0 ) || ( div <= 144 ));

  while ( j <= (int)Varga::Last )
  {
    if ( vargaDivisions[j] == div ) return (Varga)j;
    j++;
  }
	fatallog << "VargaExpert::getVargaIndexByDivision invalid division " << (int)div;
  assert( false );
  return Varga::None;
}

/**************************************************************
***
**  VargaExpert   ---   getVargaLabel
***
***************************************************************/
wxString VargaExpert::getVargaLabel( const Varga &varga, const TextFormat &format )
{
  wxString s;
  if ( varga == Varga::Bhava )
  {
    s = Lang().getVargaName( Varga::Bhava );
  }
  else
  {
    switch( format )
    {
			case TextFormat::Short:
				s += std::format( "D-{}", getVargaDivision( varga ));
			break;
			case TextFormat::Long:
				s += std::format( "{} (D-{})", Lang().getVargaName( varga ).ToStdString(), getVargaDivision( varga ));
			break;
			case TextFormat::Medium:
			default:
				s += Lang().getVargaName( varga );
			break;
    }
  }
  return s;
}

/*****************************************************
**
**   VargaExpert   ---   getDvadasamsaLongitude
**
******************************************************/
double VargaExpert::getDvadasamsaLongitude( const double& len )
{
	return red_deg( (int)getRasi( len ) * 30 + getRasiLen( len ) * 12 );
}

/*****************************************************
**
**   VargaExpert   ---   getVargaIdList
**
******************************************************/
std::list<Varga> VargaExpert::getVargaIdList( const VargaGroup &g )
{
  switch( g )
  {
    case VargaGroup::Shadvarga:
          return {
        Varga::Rasi,
        Varga::Hora,
        Varga::Drekkana,
        Varga::Navamsa,
        Varga::Dvadasamsa,
        Varga::Trimsamsa
      };
    break;
    case VargaGroup::Varga2:
      return {
        Varga::Chaturtamsa,
        Varga::Saptamamsa,
        Varga::Dasamsa,
        Varga::Shodasamsa,
        Varga::Vimsamsa,
        Varga::Siddhamsa
      };
    case VargaGroup::Varga3:
      return {
        Varga::Bhamsa,
        Varga::Chatvarimsamsa,
        Varga::Akshavedamsa,
        Varga::Shastiamsa,
        Varga::Ashtottaramsa,
        Varga::DvadasDvadasamsa
      };
    break;
    default:
      assert( false );
    break;
  }
	return {};
}

/*****************************************************
**
**   VargaExpert   ---   calcVarga
**
******************************************************/
Sign VargaExpert::calcVarga( const double& l, const Varga &division )
{
	//debuglog << "calcVarga division " << (int)division;
	assert_valid_enum<Varga>( division );
	double len = l;
	double ret = 0;
	double basepos, rs;
	Sign rasi;
	//assert( horoscope );

	switch ( division )
	{
		// D1
		case Varga::Rasi:
			ret = len;
		break;

		// D9
		case Varga::Navamsa:
			ret =  9 * len;
		break;

		// D2
		case Varga::Hora:
			switch ( astroconfig->varga.vargaHoraMode )
			{
				case 0: // Normal mode: Parasara
					ret = a_red( len - 15, 60 ) + 90;
				break;
				case 1: // Continuous
					ret = 2 * len;
				break;
				default:
					assert( false );
				break;
			}
		break;

		// D3
		case Varga::Drekkana:
			switch ( astroconfig->varga.vargaDrekkanaMode )
			{
				case 0: // normal Mode: Parasara
					ret = ( floor( getRasiLen( len ) / 10 )  * 120 + (int)getRasi( len ) * 30 + getRasiLen( 3 * len ));
				break;
				case 1: // Continuous
					ret = 3 * len;
				break;
				default:
					assert( false );
				break;
			}
		break;

		// D4
		case Varga::Chaturtamsa:
			switch ( astroconfig->varga.vargaChaturtamsaMode )
			{
			case 0: // Normal mode: Parasara
				ret = ( floor( getRasiLen( len ) / 7.5 )  * 90 + (int)getRasi( len ) * 30 + getRasiLen( 4 * len ));
				break;
			case 1: // Continuous
				ret = 2 * len;
				break;
			default:
				assert( false );
				break;
			}
		break;

		// D6
		case Varga::Sastamsa:
			ret =  6 * len;
		break;

		// D7
		case Varga::Saptamamsa:
			basepos = (int)getRasi( len ) * 30 + getRasiLen( len ) * 7;
			if ( isOddRasi( len ) == 1 ) ret = basepos;
			else ret = basepos + 180;
		break;

		// D8
		case Varga::Ashtamsa:
			ret = 8 * len;
		break;

		// D10
		case Varga::Dasamsa:
			basepos = (int)getRasi( len ) * 30 + getRasiLen( len ) * 10;
			if ( isOddRasi( len ) == 1 ) ret = basepos;
			else ret = basepos + 240;
		break;

		// D12
		case Varga::Dvadasamsa:
			ret = getDvadasamsaLongitude( len );
		break;

		// D16
		case Varga::Shodasamsa:
			ret = 16 * len;
		break;

		// D20
		case Varga::Vimsamsa:
			ret = 20 * len;
		break;

		// D24
		case Varga::Siddhamsa:
			basepos = getRasiLen( len ) * 24;
			if ( isOddRasi( len ) == 1 ) ret = basepos + 120;
			else ret = basepos + 90;
		break;

		// D27
		case Varga::Bhamsa:
			ret =  27 * len;
		break;

		// D30
		case Varga::Trimsamsa:
			rs = getRasiLen( len );
			if ( isOddRasi( len ) == 1 )
			{
				// Ar, Aq, Sa, Ge, Li
				if ( rs < 5 )
					ret = 30 * (int)Sign::Aries + rs*6;
				else if (( rs >=  5 ) && ( rs <= 10 ))
					ret = 30 * (int)Sign::Aquarius + (rs - 5)*6;
				else if (( rs >= 10 ) && ( rs <= 18 ))
					ret = 30 * (int)Sign::Sagittarius + (rs - 10)/4 * 15;
				else if (( rs >=  18 ) && ( rs <= 25 ))
					ret = 30 * (int)Sign::Gemini + ( rs - 18 )/7 * 30;
				else if ( rs > 25 )
					ret = 30 * (int)Sign::Libra + (rs - 25)*6;
			}
			else
			{
				// Ta, Vi, Pi, Cp, Sc
				if ( rs < 5 ) ret = 30 * (int)Sign::Taurus + (5-rs)*6;
				else if (( rs >=  5 ) && ( rs <= 10 )) ret = 30 * (int)Sign::Virgo + (10-rs)*6;
				else if (( rs >= 10 ) && ( rs <= 18 )) ret = 30 * (int)Sign::Pisces + ( 18-rs)/4 * 15;
				else if (( rs >= 18 ) && ( rs <= 25 )) ret = 30 * (int)Sign::Capricorn + (25-rs)/7 * 30;
				else if ( rs > 25 ) ret = 30 * (int)Sign::Scorpio + ( 30 -rs)*6;
			}
		break;

		// D40
		case Varga::Chatvarimsamsa:
			basepos = getRasiLen( len ) * 40;
			if ( isOddRasi( len ) == 1 ) ret = basepos;
			else ret = basepos + 180;
		break;

		// D45
		case Varga::Akshavedamsa:
			basepos = getRasiLen( len ) * 45;
			if ( inMovableSign( len )) ret = basepos;
			else if ( inFixedSign( len )) ret = basepos + 120;
			else ret = basepos + 240;
		break;

		// D60
		case Varga::Shastiamsa:
			ret = 60 * getRasiLen( len ) + (int)getRasi( len ) * 30;
		break;

		// D108
		case Varga::Ashtottaramsa:
			ret = getDvadasamsaLongitude( 9 * len );
		break;

		// D144
		case Varga::DvadasDvadasamsa:
			ret = getDvadasamsaLongitude( getDvadasamsaLongitude ( len ));
		break;

		// Bhava
		case Varga::Bhava:
			assert( horoscope );
			ret = red_deg( horoscope->getLongitude( PlanetId::Ascendant, viewpoint )
				+ 30 * (int)horoscope->getHouse4Longitude( len, viewpoint ));
		break;

		default: // invalid varga
			assert( 0 );
		break;
	}

	// ret has degree value, so get rasi
	rasi = getRasi( red_deg( ret ));
	//debuglog << "rasi " << (int)rasi << " ret " << ret << " varga " << (int)division << " len " << len;
	assert_valid_enum<Sign>( rasi );
	return rasi;
}

/*****************************************************
**
**   VargaExpert   ---   write
**
******************************************************/
void VargaExpert::write( ContextProps_ptr props, MDocument_ptr doc, const bool /*show_header*/ )
{
	assert( horoscope );

	int gv;
	Lang lang;
	wxString s;
  PlanetPropertyConfig cfg( astroconfig->getViewpointConfig( viewpoint ).planetProps );

	// shorten signs
	if ( cfg.textFormat == TextFormat::Long || cfg.textFormat == TextFormat::Medium )
	{
		cfg.textFormat = TextFormat::Short;
	}

	const auto obsp = PlanetHelper::createVedicOnlyPlanets();

	Table *t1 = new Table( enum_count<Varga>(), 1 );
	Table *t2 = new Table( enum_count<Varga>(), 1 );
	Table *t3 = new Table( enum_count<Varga>(), 8 );

	t1->setTitle(  tr( "Vargas" ));
	t1->setHeader( 0, "" );
	for ( Varga v : Enum<Varga>() )
	{
		if ( v == Varga::Bhava ) continue;

		s = "D-" + std::format( "{}", getVargaDivision( v ));

		t1->setHeader( (int)v + 1, s );
		t2->setHeader( (int)v + 1, s );
		t3->setHeader( (int)v + 1, s );
	}

	for ( const auto p : props->getPlanets( viewpoint ))
	{
		TableRow row;
		row.addValue( MToken( p, viewpoint ));

		for ( Varga v : Enum<Varga>() )
		{
			if ( v == Varga::Bhava ) continue;
			row.addValue( MToken( horoscope->getVargaData( p, v, viewpoint ).getRasi(), cfg ));
		}
		t1->addRow( row );
	}
	doc->addItem( t1 );

	/*
	* Dignity
	*/
	t2->setTitle(  tr( "Dignities" ));
	t2->setHeader( 0, "" );

	for ( const PlanetId p : obsp )
	{
		TableRow row;
		row.addValue( MToken( p, viewpoint ));

		for ( Varga v : Enum<Varga>() )
		{
			if ( v == Varga::Bhava ) continue;
			row.addValue( lang.getDignityName( horoscope->getVargaData( p, v, viewpoint ).getDignity(),
				cfg.textFormat, cfg.vedicSignNames ));
		}
		t2->addRow( row );
	}
	doc->addItem( t2 );

	/*
	* Varga Viswa
	*/
	t3->setTitle(  tr( "Varga Viswa" ));
	t3->setHeader( 0, "" );

	int line = 1;
	for ( const PlanetId p : obsp )
	{
		t3->setEntry( 0, line, MToken( p, viewpoint  ));

		for ( Varga v : Enum<Varga>() )
		{
			if ( v == Varga::Bhava ) continue;
			t3->setEntry( (int)v + 1, line,
				std::format( "{}", horoscope->getVargaData( p, v, viewpoint ).getVargaViswa()));
		}
		line++;
	}
	doc->addItem( t3 );

	Table *t4 = new Table( tr( "Vimsopaka Bala" ));
	t4->addHeaderRow(
	{
		"",
		"V-6",
		tr( "Own Vargas" ),
		"",
		"V-7",
		tr( "Own Vargas" ),
		"",
		"V-10",
		tr( "Own Vargas" ),
		"",
		"V-16",
		tr( "Own Vargas" ),
		"",
		tr( "Average" ),
		tr( "Percent" )
	});

	for ( const PlanetId p : obsp )
	{
		TableRow row;
		row.addValue( MToken( p, viewpoint ));
		const VedicPlanetData &pdata = horoscope->getVedicPlanetData( p, viewpoint );

		row.addValue( std::format( "{:.1f}", pdata.vimsopakaBala[0] ));

		gv = pdata.vimsopakaBalaGoodVargas[0];
		s = std::format( "{}", gv );
		if ( gv > 1 )
		{
			s += std::format( "{}", gv ) + " - " + lang.getVimsopakaDignityName( 0, gv );
		}
		row.addValue( s );
		row.addValue();

		row.addValue( std::format( "{:.1f}", pdata.vimsopakaBala[1] ));

		gv = pdata.vimsopakaBalaGoodVargas[1];
		s = std::format( "{}", gv );
		if ( gv > 1 )
		{
			s += " - " + lang.getVimsopakaDignityName( 1, gv );
		}
		row.addValue( s );
		row.addValue();

		row.addValue( std::format( "{:.1f}", pdata.vimsopakaBala[2] ));

		gv = pdata.vimsopakaBalaGoodVargas[2];
		s = std::format( "{}", gv );
		if ( gv > 1 )
		{
			s += " - " + lang.getVimsopakaDignityName( 2, gv );
		}
		row.addValue( s );
		row.addValue();

		row.addValue( std::format( "{:.1f}", pdata.vimsopakaBala[3] ));

		gv = pdata.vimsopakaBalaGoodVargas[3];
		s = std::format( "{}", gv );
		if ( gv > 1 )
		{
			s += " - " + lang.getVimsopakaDignityName( 3, gv );
		}
		row.addValue( s );
		row.addValue();

		row.addValue( std::format( "{:.1f}", pdata.averageVimsopakaBala ));
		row.addValue( std::format( "{:.1f}%", 5.0 * pdata.averageVimsopakaBala ));

		line++;
		t4->addRow( row );
	}
	doc->addItem( t4 );
}



