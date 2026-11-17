/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/VedicAspect.cpp
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

#include "VedicAspect.h"

#include "astrobase.h"
#include "astroconfig.h"
#include "constants.h"
#include "ChartConfig.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "logger.h"
#include "mathbase.h"
#include "Nakshatra.h"
#include "MDocument.h"
#include "Table.h"
#include "Varga.h"

#include <format>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   VedicAspectExpert   ---   Constructor
**
******************************************************/
VedicAspectExpert::VedicAspectExpert( ContextProps_ptr props, ChartConfig *chartConfig )
	: Expert( props, Viewpoint::Vedic ),
	myChartConfig( chartConfig )
{
	assert( props );
}

/*****************************************************
**
**   VedicAspectExpert   ---   getRasiDrishtis
**
******************************************************/
std::array<Sign, 3> VedicAspectExpert::getRasiDrishtis( const Sign &aspectingSign )
{
	std::array<Sign, 3> r { Sign::None };

	const static int rasi_drishtis[3][3] =
	{
		{ 4, 7, 10 }, // movable sign
		{ 2, 5, 8 }, // fixed sign
		{ 3, 6, 9 } // dual sign
	};
	const int sign_quality = (int)aspectingSign % 3;

	for( int i = 0; i < 3; i++ )
	{
		r[i] = (Sign)( red12( (int)aspectingSign + rasi_drishtis[sign_quality][i] ));
		debuglog << "VedicAspectExpert::getRasiDrishti aspectingSign " << aspectingSign << " index " << i << " value " << r[i];
	}
	return r;
}

/*****************************************************
**
**   VedicAspectExpert   ---   getSingleGrahaDrishti
**
******************************************************/
VedicAspectStrength VedicAspectExpert::getSingleGrahaDrishti( const PlanetId &aspectingPlanet, const int &rasidiff )
{
	if ( rasidiff == 6 ) return VedicAspectStrength::Full;
	else if ( rasidiff == 3 || rasidiff == 7 )
	{
		return ( aspectingPlanet == PlanetId::Mars ? VedicAspectStrength::Full : VedicAspectStrength::P75 );
	}
	else if ( rasidiff == 4 || rasidiff == 8 )
	{
		return ( aspectingPlanet == PlanetId::Jupiter ? VedicAspectStrength::Full : VedicAspectStrength::P50 );
	}
	else if ( rasidiff == 2 || rasidiff == 9 )
	{
		return ( aspectingPlanet == PlanetId::Saturn ? VedicAspectStrength::Full : VedicAspectStrength::P25 );
	}
	return VedicAspectStrength::None;
}

/*****************************************************
**
**   VedicAspectExpert   ---   getGrahaDrishtiValue
**
******************************************************/
double VedicAspectExpert::getGrahaDrishtiValue( const PlanetId &aspectingPlanet, const int &rasidiff )
{
	switch( getSingleGrahaDrishti( aspectingPlanet, rasidiff ))
	{
		case VedicAspectStrength::Full:
			return 1.0;
		break;
		case VedicAspectStrength::P75:
			return .75;
		break;
		case VedicAspectStrength::P50:
			return .5;
		break;
		case VedicAspectStrength::P25:
			return .25;
		break;

		default:
		return 0;
	}
}

/*****************************************************
**
**   VedicAspectExpert   ---   getGrahaDrishtis
**
******************************************************/
std::list<std::pair<Sign, VedicAspectStrength>> VedicAspectExpert::getGrahaDrishtis( const PlanetId &aspectingPlanet,
	const Sign &sign, const VedicAspectStrength &minimumStrength )
{
	std::list<std::pair<Sign, VedicAspectStrength>> l;

	// 7th house has always full aspect
	l.push_back( { (Sign)red12( (int)sign + 6 ), VedicAspectStrength::Full } );

	// Mars
	{
		VedicAspectStrength sMars = aspectingPlanet == PlanetId::Mars ? VedicAspectStrength::Full : VedicAspectStrength::P75;
		if ( sMars >= minimumStrength )
		{
			l.push_back({ (Sign)red12( (int)sign + 3 ), sMars } );
			l.push_back({ (Sign)red12( (int)sign + 7 ), sMars } );
		}
	}

	// Jupiter
	{
		VedicAspectStrength sJupiter = aspectingPlanet == PlanetId::Jupiter ? VedicAspectStrength::Full : VedicAspectStrength::P50;
		if ( sJupiter >= minimumStrength )
		{
			l.push_back({ (Sign)red12( (int)sign + 4 ), sJupiter } );
			l.push_back({ (Sign)red12( (int)sign + 8 ), sJupiter } );
		}
	}

	// Saturn
	{
		VedicAspectStrength sSaturn = aspectingPlanet == PlanetId::Saturn ? VedicAspectStrength::Full : VedicAspectStrength::P25;
		if ( sSaturn >= minimumStrength )
		{
			l.push_back({ (Sign)red12( (int)sign + 2 ), sSaturn } );
			l.push_back({ (Sign)red12( (int)sign + 9 ), sSaturn } );
		}
	}

	return l;
}

/*****************************************************
**
**   VedicAspectExpert   ---   getSputaDrishti
**
******************************************************/
double VedicAspectExpert::getSputaDrishti( const PlanetId& aspectingPlanet, const double &ldiff )
{
	assert( ldiff >= 0 && ldiff <= 360 );
	double ret = 0;
	const double rasildiff = a_red( ldiff, 30 );

	double rp = 2 * rasildiff;
	double ap = 2 * ( 30 - rasildiff );

	double fmars = .75;
	if ( aspectingPlanet == PlanetId::Mars ) fmars = 1;

	double fjupiter = .5;
	if ( aspectingPlanet == PlanetId::Jupiter ) fjupiter = 1;

	double fsaturn = .25;
	if ( aspectingPlanet == PlanetId::Saturn ) fsaturn = 1;

	// Default 180
	if ( ldiff >= 150 && ldiff < 180 ) {
		ret += rp;
	}
	else if ( ldiff >= 180 && ldiff < 210 ) {
		ret += ap;
	}

	// Mars 90 + 210
	if (( ldiff >= 60 && ldiff < 90 ) || ( ldiff >= 180 && ldiff < 210 )) {
		ret += fmars * rp;
	}
	else if (( ldiff >= 90 && ldiff < 120 ) || ( ldiff >= 210 && ldiff < 240 )) {
		ret += fmars * ap;
	}

	// jupiter 120 + 240
	if (( ldiff >= 90 && ldiff < 120 ) || ( ldiff >= 210 && ldiff < 240 )) {
		ret += fjupiter * rp;
	}
	else if (( ldiff >= 120 && ldiff < 150 ) || ( ldiff >= 240 && ldiff < 270 )) {
		ret += fjupiter * ap;
	}

	// Saturn 60 + 270
	if (( ldiff >= 30 && ldiff < 60 ) || ( ldiff >= 240 && ldiff < 270 )) {
		ret += fsaturn * rp;
	}
	else if (( ldiff >= 60 && ldiff < 90 ) || ( ldiff >= 270 && ldiff < 300 )) {
		ret += fsaturn * ap;
	}

	assert( ret >= 0 && ret <= 60 );
	return ret;
}

/*****************************************************
**
**   VedicAspectExpert   ---   calculateVargaChartAspects
**
******************************************************/
std::list<VedicAspect> VedicAspectExpert::calculateVargaChartAspects( const Varga &varga, Horoscope_ptr aspecting_h, 
	Horoscope_ptr aspected_h )
{
	std::list<VedicAspect> al;

	assert( props );
	assert( aspecting_h );
	assert( aspected_h );
	const Viewpoint viewpoint = getViewpoint();

	const ChartConfig *cfg = getChartConfig();
	debuglog << "VedicAspectExpert::calculateVargaChartAspects aspectType " << cfg->aspectType
		<< " myChartConfig " << myChartConfig;

	const VedicAspectId type = cfg->aspectType;

	for( const auto p : props->getPlanets( Viewpoint::Vedic ))
	{
		if ( ! cfg->includeAsMc && PlanetHelper::isAngle( p )) continue;
		if ( PlanetHelper::isHouse( p )) continue;

		const Sign rasi = aspecting_h->getVargaData( p, varga, viewpoint ).getRasi();
		const bool isBenefic = aspecting_h->isBenefic( p, viewpoint );

		switch( type )
		{
			case VedicAspectId::GrahaDrishti:
				for( const auto &drishti : getGrahaDrishtis( p, rasi, cfg->grahaDrishtiMinimumStrength ))
				{
					al.push_back({ (int)rasi, (int)drishti.first, VedicAspectId::GrahaDrishti, drishti.second, isBenefic });
				}
			break;
			case VedicAspectId::RasiDrishti:
			{
				const std::array<Sign, 3> rasiDrishtis = getRasiDrishtis( rasi );

				for( int i = 0; i < 3; i++ )
				{
					al.push_back({ (int)rasi, (int)rasiDrishtis[i], VedicAspectId::RasiDrishti,
						VedicAspectStrength::Full, isBenefic });
				}
			}
			break;
			default:
				fatallog << "Wrong aspect type " << type << " in AspectExpert::getVedicAspects";
				//assert( false );
			break;
		}
	}
	return al;
}

/*****************************************************
**
**   VedicAspectExpert   ---   calculateSbcAspects
**
******************************************************/
std::list<VedicAspect> VedicAspectExpert::calculateSbcAspects( Horoscope_ptr aspecting_h ) const
{
	std::list<VedicAspect> al;
	if ( ! aspecting_h )
	{
		warnlog << "VedicAspectExpert::calculateSbcAspects horoscope is NULL";
		return al;
	}
	assert( props );
	Nakshatra28 nak;
	const Viewpoint viewpoint = getViewpoint();

	// TODO that is not correct, should be part of sbc config
	const ChartConfig *cfg = getChartConfig();

	std::vector<Nakshatra28> vedhas;
	NakshatraExpert nexpert( props );

	for( const auto p : props->getPlanets( Viewpoint::Vedic ))
	{
		if ( ! cfg->includeAsMc && PlanetHelper::isAngle( p )) continue;

		nak = ::getNakshatra28( aspecting_h->getLongitude( p, viewpoint ));
		//vedhas = SarvatobhadraExpert::getVedhaNakshatras( nak );
		/*
		vedhas = NakshatraExpert::getVedhaNakshatras( nak );
		for ( uint j = 0; j < vedhas.size(); j++ )
		{
			if ( vedhas[j] == nak ) continue;
			al.push_back( VedicAspect( (int)nak, (int)vedhas[j], AspectType::Vedha,
				VedicAspectStrength::pFull, h->isBenefic( p, viewpoint ), swapCharts ));
		}
		*/
		for( const Nakshatra28 &vedha : nexpert.getVedhaNakshatras( nak ))
		{
			al.push_back({ (int)nak, (int)vedha, VedicAspectId::Vedha,
				VedicAspectStrength::Full, aspecting_h->isBenefic( p, viewpoint ) });
		}
	}
	return al;
}

/*****************************************************
**
**   VedicAspectExpert   ---   writeVedicAspectarium
**
******************************************************/
void VedicAspectExpert::writeVedicAspectarium( MDocument_ptr doc, Horoscope_ptr h, const bool show_header )
{
	const Viewpoint viewpoint = getViewpoint();
	const PlanetConfig &cfg = astroconfig->getViewpointConfig( viewpoint ).planets;

	if ( ! h )
	{
		doc->addHeader2( tr( "No horoscope set" ));
		return;
	}
	if ( show_header ) doc->addHeader1( tr( "Vedic Aspectarium" ));

	auto planets = props->getPlanets( viewpoint, 
		{
		 FilterGroup::Angles,
		 FilterGroup::CardinalPoints,
		 FilterGroup::Upagrahas,
		 FilterGroup::Kalavelas,
		 FilterGroup::Houses,
		});

	{
		Table *table1 = new Table( planets.size() + 1, planets.size() + 1 );
		table1->format.uniformLayout = true;
		table1->setTitle( tr( "Graha Drishti" ));
		table1->setHeader( 0, "" );

		for( int i = 1; auto p : planets )
		{
			table1->setHeader( i, MToken( p, cfg ));
			table1->setHeaderEntry( 0, i, MToken( p, cfg ));
			table1->setEntry( i, i, "-" );
			i++;
		}

		for( int i1 = 1; const auto p1 : planets )
		{
			for( int i2 = 1; const auto p2 : planets )
			{
				if ( p1 != p2 )
				{
					int rasidiff = red12(
						(int)h->getVargaData( p1, Varga::Rasi, getViewpoint() ).getRasi()
						- (int)h->getVargaData( p2, Varga::Rasi, getViewpoint() ).getRasi());

					int drishti = (int)(60.0 * getGrahaDrishtiValue( p2, rasidiff ));
					table1->setEntry( i1, i2, std::format( "{}", drishti ));
				}
				i2++;
			}
			i1++;
		}
		doc->addItem( table1 );
	}
	{
		Table *table2 = new Table( planets.size() + 1, planets.size() + 1 );
		table2->format.uniformLayout = true;
		table2->setTitle( tr( "Sputa Drishti" ));
		table2->setHeader( 0, "" );

		for( int i = 1; const auto p : planets )
		{
			table2->setHeader( i, MToken( p, cfg ));
			table2->setHeaderEntry( 0, i, MToken( p, cfg ));
			table2->setEntry( i, i, "-" );
			i++;
		}

		for( int i1 = 1; const auto p1 : planets )
		{
			for( int i2 = 1; const auto p2 : planets )
			{
				if ( p1 != p2 )
				{
					double lendiff = red_deg( getLongitude( p2 ) - getLongitude( p1 ));
					double drishti = getSputaDrishti( p2, lendiff );
					table2->setEntry( i1, i2, std::format( "{:.1f}", drishti ));
				}
				i2++;
			}
			i1++;
		}
		doc->addItem( table2 );
	}
}

/*****************************************************
**
**   VedicAspectExpert   ---   getChartConfig
**
******************************************************/
ChartConfig *VedicAspectExpert::getChartConfig() const
{
	return myChartConfig ? myChartConfig : &gdiconfig->chartVarga;
}

