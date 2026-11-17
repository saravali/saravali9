/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/WesternAspect.cpp
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

#include "WesternAspect.h"

#include "astroconfig.h"
#include "constants.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "logger.h"
#include "mathbase.h"
#include "MDocument.h"
#include "SymbolProvider.h"
#include "Table.h"

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

//#define DEBUG_ASPECTEXPERT

/*****************************************************
**
**   WesternAspectExpert   ---   calculate
**
******************************************************/
std::list<WesternAspectEvent> WesternAspectExpert::calculate( const bool isCompositeChart )
{
	const DataContext charttype = props->getDataContext();
	const Viewpoint viewpoint = getViewpoint();
	std::list<WesternAspectEvent> l;

#ifdef DEBUG_ASPECTEXPERT
	debuglog << "WesternAspectExpert::calculate active roles " << props->getActiveRoles().size()
		<< " composite " << std::boolalpha << isCompositeChart;
	for( const auto &role : props->getActiveRoles())
	{
		debuglog << "WesternAspectExpert::calculate role " << role;
	}
#endif

	Horoscope_ptr horoscope;
	Horoscope_ptr h2;

	switch( charttype )
	{
		using enum DataContext;

		case Partner:
			if ( isCompositeChart )
			{
				horoscope = props->getHoroscope( HoroscopeRole::Composite );
				h2 = horoscope;
				if ( ! horoscope ) return l;
			}
			else
			{
				horoscope = props->getHoroscope( HoroscopeRole::Partner1 );
				h2 = props->getHoroscope( HoroscopeRole::Partner2 );
			}
		break;
		default:
		{
			const auto roles = props->getActiveRoles();
			if ( roles.size() > 0 ) horoscope = props->getRadixHoroscope();
			if ( roles.size() > 1 ) h2 = props->getHoroscope( roles[1] );
			else h2 = horoscope;
		}
		break;
	}

#ifdef DEBUG_ASPECTEXPERT
	debuglog << "WesternAspectExpert::calculate horoscope  " << horoscope << " h2 " << h2;
#endif
	if ( ! horoscope || ! h2 ) return l;
	double ldiff = 0.0;

	planets = props->getPlanets( viewpoint, { FilterGroup::Houses, });

#ifdef DEBUG_ASPECTEXPERT
	for( auto p : planets )
	{
		debuglog <<  "WesternAspectExpert::calculate planet " << p;
	}
	debuglog <<  "WesternAspectExpert::calculate viewpoint " << viewpoint << " object size " << planets.size();
#endif

	const bool useFullTable = ( charttype != DataContext::Radix );

	for( auto i1 = planets.begin(); i1 != planets.end(); i1++ )
	{
		const PlanetId p1 = *i1;

		for( auto i2 = ( useFullTable ? planets.begin() : i1 ); i2 != planets.end(); i2++ )
		{
			const PlanetId p2 = *i2;
			if ( charttype == DataContext::Radix &&  p1 == p2 ) continue;

			// no aspects if both are Lunar Nodes
			if ( PlanetHelper::isNode( p1 ) && PlanetHelper::isNode( p2 )) continue;

			// no aspects if one of them is a house cusps
			if ( PlanetHelper::isHouse( p1 )) continue;
			if ( PlanetHelper::isHouse( p2 )) continue;

			// aries excluded - NO
			//if ( p1 == PlanetId::Aries || p2 == PlanetId::Aries ) continue;

			// do not compare ascendent etc items that are alwys in opposition
			if ( p1 == PlanetId::Ascendant && p2 == PlanetId::Descendant ) continue;
			if ( p1 == PlanetId::Descendant && p2 == PlanetId::Ascendant ) continue;
			if ( p1 == PlanetId::Meridian && p2 == PlanetId::ImumCoeli ) continue;
			if ( p1 == PlanetId::ImumCoeli && p2 == PlanetId::Meridian ) continue;

			const double l1 = horoscope->getLongitude( p1, viewpoint );
			const double l2 = h2->getLongitude( p2, viewpoint );
			ldiff = fabs( l1 - l2 );
			if ( ldiff > 180 ) ldiff = 360 - ldiff;
			if ( ldiff < -180 ) ldiff = 360 + ldiff;

#ifdef DEBUG_ASPECTEXPERT
	debuglog <<  "WesternAspectExpert::calculate p1 " << p1 << " p2 " << p2 << " ldiff " << ldiff;
#endif

			// longitude base aspects
			const auto match = this->getMatch( ldiff );
			if ( match.first.first != WesternAspectId::None )
			{
				l.push_back({ p1, l1, p2, l2, match.first, match.second });
			}

			// declination based
			for ( const auto &id : { WesternAspectId::Parallele, WesternAspectId::Contraparallele } )
			{
				const auto &aspcfg = gdiconfig->westernAspects.getConfig( id );
				if ( ! aspcfg.active ) continue;

				double orbis = 0.0;
				switch( id )
				{
					case WesternAspectId::Parallele:
						orbis = fabs( horoscope->getDeclination( p1, viewpoint ) - h2->getDeclination( p2, viewpoint ));
					break;
					case WesternAspectId::Contraparallele:
						orbis = fabs( horoscope->getDeclination( p1, viewpoint ) + h2->getDeclination( p2, viewpoint ));
					break;
					default:
						assert( false );
					break;
				}
				if ( orbis < aspcfg.orbis )
				{
					l.push_back({ p1, l1, p2, l2, { id, 1 }, orbis });
				}
			}
		}
	}
#ifdef DEBUG_ASPECTEXPERT
	debuglog <<  "WesternAspectExpert::calculate has " << l.size() << " matches";
#endif
	return l;
}

/*****************************************************
**
**   WesternAspectExpert   ---   getMatch
**
******************************************************/
std::pair<WesternAspectType, double> WesternAspectExpert::getMatch( const double &ldiff ) const
{
	const static std::list<std::pair<WesternAspectType, double>> aspectAngles = {
		{{ WesternAspectId::Conjunction, 1 }, 0 },
		{{ WesternAspectId::Opposition, 1 }, 180 },
		{{ WesternAspectId::Trine, 1 }, 120 },
		{{ WesternAspectId::Square, 1 }, 90 },
		{{ WesternAspectId::Sextile, 1 }, 60 },
		{{ WesternAspectId::Quincunx, 5 }, 150 },
		{{ WesternAspectId::Semisquare, 1 }, 45 },
		{{ WesternAspectId::Sesquisquare, 3 }, 135 },
		{{ WesternAspectId::Semisextile, 1 }, 30 },

		{{ WesternAspectId::Quintile, 1 }, 72 },
		{{ WesternAspectId::Septile, 1 }, 360.0 / 7.0 },
		{{ WesternAspectId::Septile, 1 }, 360.0 / 7.0 },
		{{ WesternAspectId::Novile, 1 }, 40 },
	};

	for( const auto &a : aspectAngles )
	{
		const WesternAspectId id = a.first.first;
		const int nominator = a.first.second;
		const double angle = a.second;

		const auto &aspcfg = gdiconfig->westernAspects.getConfig( id );
		if ( ! aspcfg.active ) continue;

		const double orbis = fabs( ldiff - angle );
		if ( orbis < aspcfg.orbis )
		{
			//return std::make_tuple( type, orbis, std::get<2>( a ));
			return {{ id, nominator }, orbis };
		}

		// Biquintil etc.
		// TODO testen ob das richtig ist
		if ( id == WesternAspectId::Quintile || id == WesternAspectId::Septile || id == WesternAspectId::Novile )
		{
			const double orbis = fabs( ldiff - 2 * angle );
			if ( orbis < aspcfg.orbis )
			{
				//return std::make_tuple( type, orbis, 2 );
				return {{ id, 2 }, orbis };
			}
		}
		// Triseptile
		// TODO testen ob das richtig ist
		if ( id == WesternAspectId::Septile )
		{
			const double orbis = fabs( ldiff - 3 * angle );
			if ( orbis < aspcfg.orbis )
			{
				//return std::make_tuple( WesternAspectId::Septile, orbis, 3 );
				return {{ WesternAspectId::Septile, 3 }, orbis };
			}
		}

		// Quadnovile
		// TODO testen ob das richtig ist
		if ( id == WesternAspectId::Novile )
		{
			const double orbis = fabs( ldiff - 4 * angle );
			if ( orbis < aspcfg.orbis )
			{
				//return std::make_tuple( WesternAspectId::Novile, orbis, 4 );
				return {{ WesternAspectId::Novile, 4 }, orbis };
			}
		}
	}
	//return std::make_tuple( WesternAspectId::None, 0, 1 );
	return {{ WesternAspectId::None, 1 }, 0 };
}

/*****************************************************
**
**   WesternAspectExpert   ---   sort
**
******************************************************/
void WesternAspectExpert::sort( std::list<WesternAspectEvent> &al, const EventSortOrder &order )
{
	al.sort( [&] ( const auto &e1, const auto &e2 )
	{
		switch ( order )
		{
		case EventSortOrder::RightPlanet:
			if ( e1.planet2 != e2.planet2 ) return ( e1.planet2 < e2.planet2 );
			return ( e1.planet1 < e2.planet1 );
		break;
		case EventSortOrder::Orbis:
			return( e1.orbis < e2.orbis );
		break;
		case EventSortOrder::OrbisAbsolute:
			return( fabs( e1.orbis ) < fabs( e2.orbis ));
		break;
		case EventSortOrder::Type:
			if ( e1.type != e2.type ) return( e1.type < e2.type );
			return( fabs( e1.orbis ) < fabs( e2.orbis ));
		break;
		case EventSortOrder::TypeReverse:
			if ( e1.type != e2.type ) return( e1.type > e2.type );
			return( fabs( e1.orbis ) > fabs( e2.orbis ));
		break;
		case EventSortOrder::LeftPlanet:
		default:
			if ( e1.planet1 != e2.planet1 ) return ( e1.planet1 < e2.planet1 );
			return ( e1.planet2 < e2.planet2 );
		break;
		}
		return false;
	});
}

/*****************************************************
**
**   WesternAspectExpert   ---   writeAspectariumTable
**
******************************************************/
void WesternAspectExpert::writeAspectariumTable( Table *table, const uint &colOffset, const uint &rowOffset,
	const std::list<WesternAspectEvent> &al, PlanetConfig planetCfg )
{
#ifdef DEBUG_ASPECTEXPERT
	debuglog <<  "WesternAspectExpert::writeAspectariumTable colOffset " << colOffset << " rowOffset " << rowOffset
		<< " number of aspects " << al.size() << " number of planets: " << planets.size();
#endif

	trimTextFormat( planetCfg.textFormat, TextFormat::Short );
  
	std::map<PlanetId, int> reverse_obs;
	for( uint i = 0; const auto p : PlanetHelper::expandPlanets( planets ))
  {
    reverse_obs[ p ] = i;
		if ( i < planets.size() - 1 )
		{
			table->setHeaderEntry( colOffset + i, rowOffset + i, MToken( p, planetCfg ));
		}
		i++;
  }

	for( const auto &e : al )
	{
#ifdef DEBUG_ASPECTEXPERT
	debuglog <<  "WesternAspectExpert::writeAspectariumTable aspect " << e.planet1 << ":" << reverse_obs[e.planet1] << " / "
		<< e.planet2 << ":" << reverse_obs[e.planet2] << " type " << (int)e.type.first << "/" << e.type.second;
#endif
		DocItemText &item = table->getTextItem( reverse_obs[e.planet1] + colOffset, reverse_obs[e.planet2] + rowOffset );
		if ( ! item.tf.isEmpty() )
		{
			item.tf << ", ";
		}
		item.tf << MToken( e.type, TextFormat::Symbol );
	}
}

/*****************************************************
**
**   WesternAspectExpert   ---   createAspectariumTable
**
******************************************************/
Table *WesternAspectExpert::createAspectariumTable( const std::list<WesternAspectEvent> &al, PlanetConfig planetCfg,
	const bool show_header )
{
	Table *table = new Table( planets.size(), planets.size());
	if ( show_header ) table->setTitle( tr( "Western Aspectarium" ));

	for( uint i = 0; const auto p : planets )
  {
		if ( i > 0 )
		{
			table->setHeaderEntry( 0, i, MToken( p, planetCfg ));
		}
		i++;
	}

	writeAspectariumTable( table, 1, 0, al, planetCfg );
	return table;
}

/*****************************************************
**
**   WesternAspectExpert   ---   writeAspectList
**
******************************************************/
void WesternAspectExpert::writeAspectList( MDocument_ptr doc, const std::list<WesternAspectEvent> &l,
	const bool show_header )
{
	Formatter *formatter = Formatter::get();
	const Viewpoint viewpoint = getViewpoint();
	const PlanetConfig &cfg = astroconfig->getViewpointConfig( viewpoint ).planets;

	std::list<wxString> headerRow = { tr( "Type" ) };
	switch( props->getDataContext())
	{
		case DataContext::Direction:
			headerRow.push_back( tr( "Direction" ) );
			headerRow.push_back( tr( "Symbol" ) );
			headerRow.push_back( tr( "Radix" ) );
		break;

		case DataContext::Partner:
		{
			Horoscope_ptr ch = props->getHoroscope( HoroscopeRole::Composite );
			if ( ch )
			{
				headerRow.push_back( tr( "Aspecting" ) );
			}
			else
			{
				Horoscope_ptr ph1 = props->getHoroscope( HoroscopeRole::Partner1 );
				headerRow.push_back( ph1 ? ph1->dl.name : wxString( "N.N" ));
			}
			headerRow.push_back( tr( "Symbol" ) );
			if ( ch )
			{
				headerRow.push_back( tr( "Aspected" ) );
			}
			else
			{
				Horoscope_ptr ph2 = props->getHoroscope( HoroscopeRole::Partner2 );
				headerRow.push_back( ph2 ? ph2->dl.name : wxString( "N.N" ));
			}
		}
		break;

		case DataContext::Radix:
		default:
			headerRow.push_back( tr( "Aspecting" ) );
			headerRow.push_back( tr( "Symbol" ) );
			headerRow.push_back( tr( "Aspected" ) );
		break;
	}
	headerRow.push_back( tr( "Orbis" ) );

	Table *table = new Table( show_header ? tr( "Aspects" ) : wxString() );
	table->addHeaderRow( headerRow );

#if 1
	for ( const auto &event : l )
	{
		TableRow row;
		row.addValue( enum_to_string( event.type.first ));
		row.addValue( MToken( event.planet1, cfg ));
		row.addValue( MToken( event.type, TextFormat::Symbol ));

		row.addValue( MToken( event.planet2, cfg ));
		row.addValue( formatter->formatDegrees( event.orbis ));
		table->addRow( row );
	}
#endif
	doc->addItem( table );
}

