/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Planet.cpp
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

#include "Planet.h"
#include "astroconfig.h"
#include "enumserializer.h"
#include "Lang.h"
#include "logger.h"

/*************************************************//**
*
*   PlanetComparator   ---   compare
*
******************************************************/
mostream& operator <<( mostream &o, const DasaLord &lord )
{
	Lang lang;
	if ( std::holds_alternative<PlanetId>( lord ))
	{
		o << lang.getPlanetName( std::get<PlanetId>( lord ), Viewpoint::Vedic );
	}
	else if ( std::holds_alternative<Sign>( lord ))
	{
		o << lang.getSignName( std::get<Sign>( lord ), Viewpoint::Vedic );
	}
	else
	{
		assert( false );
	}
	return o;
}

/*************************************************//**
*
*   PlanetComparator   ---   compare
*
******************************************************/
bool PlanetComparator::operator()( const PlanetId &p1, const PlanetId &p2 ) const
{
	if ( p1 == PlanetId::None || p2 == PlanetId::None ) return false;

	const auto iter1 = std::find_if( obs.begin(), obs.end(), [p1]( const PlanetId &p ) { return p == p1; } );
	const size_t found1 = std::ranges::distance( obs.begin(), iter1 );

	const auto iter2 = std::find_if( obs.begin(), obs.end(), [p2]( const PlanetId &p ) { return p == p2; } );
	const size_t found2 = std::ranges::distance( obs.begin(), iter2 );

	if ( found1 == obs.size() || found2 == obs.size() )
	{
		errorlog << "PlanetComparator: invalid planet comparison, planets " << p1 << "/" << p2;
		return false;
	}
	return found1 < found2;
}

/*************************************************//**
*
*   PlanetHelper   ---  planetMatchesFilter
*
******************************************************/
bool PlanetHelper::planetMatchesFilter( const PlanetId &p, const std::set<FilterGroup> &fg )
{
	debuglog << "PlanetHelper::planetMatchesFilter planet " << p << " filter size " << fg.size();
	for( const auto &f : fg )
	{
		//debuglog << "PlanetFilter::matches filter group " << f << " planet " << p;
		switch( f )
		{
			case FilterGroup::Luminaries:
				switch( p )
				{
					case PlanetId::Sun:
					case PlanetId::Moon:
					case PlanetId::Earth:
						return true;
					break;
					default:
					break;
				}
			break;
			case FilterGroup::InnerPlanets:
				return ( p >= PlanetId::Mercury && p <= PlanetId::Saturn );
			break;
			case FilterGroup::OuterPlanets:
				return ( p >= PlanetId::Uranus && p <= PlanetId::Pluto );
			break;
			case FilterGroup::Nodes:
				return ( p >= PlanetId::FirstNode && p <= PlanetId::LastNode );
			break;

			case FilterGroup::Angles:
				switch( p )
				{
					case PlanetId::Ascendant:
					case PlanetId::Meridian:
					case PlanetId::Descendant:
					case PlanetId::ImumCoeli:
					case PlanetId::BhavaLagna:
					case PlanetId::HoraLagna:
					case PlanetId::GhatikaLagna:
					case PlanetId::D9Lagna:
						return true;
					break;
					default:
					break;
				}
			break;
			case FilterGroup::TNP:
				return ( p >= PlanetId::FirstTNP && p <= PlanetId::LastTNP );
			break;
			case FilterGroup::Houses:
				return ( p >= PlanetId::FirstHouse && p <= PlanetId::LastHouse );
				switch( p )
				{
					case PlanetId::AllHouses:
					case PlanetId::FourHouses:
					case PlanetId::SixHouses:
						return true;
					break;
					default:
					break;
				}
			break;

			case FilterGroup::ExtraPlanets:
				switch( p )
				{
					case PlanetId::Chiron:
					case PlanetId::Pholus:
					case PlanetId::Ceres:
					case PlanetId::Pallas:
					case PlanetId::Juno:
					case PlanetId::Vesta:
					case PlanetId::Lilith:
						return true;
					break;
					default:
					break;
				}
			break;

			case FilterGroup::CardinalPoints:
				return ( p >= PlanetId::FirstCardinalPoint && p <= PlanetId::LastCardinalPoint );
			break;

			case FilterGroup::Upagrahas:
				if ( p == PlanetId::Upagrahas ) return true;
				return ( p >= PlanetId::FirstUpagraha && p <= PlanetId::LastUpagraha );
			break;

			case FilterGroup::Kalavelas:
				if ( p == PlanetId::Kalavelas ) return true;
				return ( p >= PlanetId::FirstKalavela && p <= PlanetId::LastKalavela );
			break;

			case FilterGroup::ArabicParts:
				return ( p >= PlanetId::FirstArabic && p <= PlanetId::LastArabic );
			break;

			default:
			break;
		}
	}
	return false;
}

/*************************************************//**
*
*   PlanetHelper   ---  resolveNode
*
******************************************************/
PlanetId PlanetHelper::resolveNode( const PlanetId &p, const Viewpoint &viewpoint )
{
	extern Astroconfig *astroconfig;
	switch( p )
	{
		case PlanetId::AscNode:
			return astroconfig->getViewpointConfig( viewpoint ).calculation.lunarNodeMode == LunarNodeMode::True
				? PlanetId::TrueAscNode : PlanetId::MeanAscNode;
		break;
		case PlanetId::DescNode:
			return astroconfig->getViewpointConfig( viewpoint ).calculation.lunarNodeMode == LunarNodeMode::True
				? PlanetId::TrueDescNode : PlanetId::MeanDescNode;
		break;

		case PlanetId::TrueAscNode:
		case PlanetId::TrueDescNode:
		case PlanetId::MeanAscNode:
		case PlanetId::MeanDescNode:
			return p;
		break;
		default:
		break;
	}
	return p;
}

/*************************************************//**
*
*   PlanetHelper   ---  isExpanded
*
******************************************************/
bool PlanetHelper::isExpanded( const Planets &planets )
{
	for( const auto &p : planets )
	{
		if ( isPlanetGroup( p )) return false;
	}
	return true;
}

/*************************************************//**
*
*   PlanetHelper   ---  expandPlanets
*
******************************************************/
Planets PlanetHelper::expandPlanets( const Viewpoint &viewpoint )
{
	extern Astroconfig *astroconfig;
	Planets planets;
	for( const auto &item : astroconfig->getViewpointConfig( viewpoint ).planets.items )
	{
		if ( item.second ) planets.push_back( item.first );
	}
	return expandPlanets( planets );
}

/*************************************************//**
*
*   PlanetHelper   ---  expandPlanets
*
******************************************************/
Planets PlanetHelper::expandPlanets( const Planets &items, const std::list<int> arabicParts )
{
	Planets l;
	//extern Astroconfig *astroconfig;

	// houses: define them separately as set in order to avoid duplicates
	std::set<PlanetId> houses;
	for( const auto &p : items )
	{
		debuglog << "PlanetHelper::expandPlanets " << p;
		switch( p )
		{
			case PlanetId::Inner:
				l.push_back( PlanetId::Sun );
				l.push_back( PlanetId::Moon );
				l.push_back( PlanetId::Mercury );
				l.push_back( PlanetId::Venus );
				l.push_back( PlanetId::Mars );
				l.push_back( PlanetId::Jupiter );
				l.push_back( PlanetId::Saturn );
			break;

			case PlanetId::InnerVedic:
				l.push_back( PlanetId::Sun );
				l.push_back( PlanetId::Moon );
				l.push_back( PlanetId::Mars );
				l.push_back( PlanetId::Mercury );
				l.push_back( PlanetId::Jupiter );
				l.push_back( PlanetId::Venus );
				l.push_back( PlanetId::Saturn );
			break;

			case PlanetId::Outer:
				l.push_back( PlanetId::Uranus );
				l.push_back( PlanetId::Neptune );
				l.push_back( PlanetId::Pluto );
			break;

			case PlanetId::InnerTNP:
				l.push_back( PlanetId::Cupido );
				l.push_back( PlanetId::Hades );
				l.push_back( PlanetId::Zeus );
				l.push_back( PlanetId::Kronos );
			break;
			case PlanetId::OuterTNP:
				l.push_back( PlanetId::Apollon );
				l.push_back( PlanetId::Admetos );
				l.push_back( PlanetId::Vulkanus );
				l.push_back( PlanetId::Poseidon );
			break;
			case PlanetId::AllTNP:
				l.push_back( PlanetId::Cupido );
				l.push_back( PlanetId::Hades );
				l.push_back( PlanetId::Zeus );
				l.push_back( PlanetId::Kronos );
				l.push_back( PlanetId::Apollon );
				l.push_back( PlanetId::Admetos );
				l.push_back( PlanetId::Vulkanus );
				l.push_back( PlanetId::Poseidon );
			break;

			case PlanetId::Planetoids:
				l.push_back( PlanetId::Ceres );
				l.push_back( PlanetId::Pallas );
				l.push_back( PlanetId::Juno );
				l.push_back( PlanetId::Vesta );
			break;
			
			case PlanetId::Upagrahas:
				l.push_back( PlanetId::Dhuma );
				l.push_back( PlanetId::Vyatipata );
				l.push_back( PlanetId::Parivesha );
				l.push_back( PlanetId::Chapa );
				l.push_back( PlanetId::Upaketu );
			break;

			case PlanetId::Kalavelas:
				l.push_back( PlanetId::Kala );
				l.push_back( PlanetId::Mrityu );
				l.push_back( PlanetId::Ardhaprahara );
				l.push_back( PlanetId::Yamaghantaka );
				l.push_back( PlanetId::Gulika );
				l.push_back( PlanetId::Mandi );
			break;

			case PlanetId::SpecialLagnas:
				l.push_back( PlanetId::BhavaLagna );
				l.push_back( PlanetId::HoraLagna );
				l.push_back( PlanetId::GhatikaLagna );
			break;

			// Houses
			case PlanetId::AllHouses:
				for ( const PlanetId id : Enum<PlanetId>( PlanetId::House1, PlanetId::House12 )) houses.insert( id );
			break;
			case PlanetId::FourHouses:
			{
				houses.insert( PlanetId::House2 );
				houses.insert( PlanetId::House3 );
				houses.insert( PlanetId::House11 );
				houses.insert( PlanetId::House12 );
			}
			break;
			case PlanetId::SixHouses:
			{
				houses.insert( PlanetId::House1 );
				houses.insert( PlanetId::House2 );
				houses.insert( PlanetId::House3 );
				houses.insert( PlanetId::House10 );
				houses.insert( PlanetId::House11 );
				houses.insert( PlanetId::House12 );
			}
			break;

			case PlanetId::ArabicParts:
				for( const auto &part : arabicParts )
				{
					l.push_back( (PlanetId)( (int)PlanetId::FirstArabic + part ));
				}
			break;

			default:
				l.push_back( p );
			break;
		}
	}
	for( const auto &h : houses )
	{
		l.push_back( h );
	}
	return l;
}

