/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/UranianHelper.cpp
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

#include "UranianHelper.h"

#include "ContextProperties.h"
#include "MDocument.h"
#include "Formatter.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "Table.h"
#include "Term.h"
#include "Uranian.h"

#include <format>
#include <set>

extern Astroconfig *astroconfig;

#define DEBUG_URANIANHELPER

/*****************************************************
**
**   UranianHelper   ---   getAllDivisions
**
******************************************************/
std::vector<int> UranianHelper::getAllDivisions() const
{
	std::vector<int> r;

	for( const int d : calculateDivisionsForPrimes( astroconfig->uranian.divisionPrimes,
		astroconfig->uranian.maximumDivision ))
	{
		r.push_back( d );
	}
	return r;
}

/*****************************************************
**
**   UranianHelper   ---   getAllDivisionPrimes
**
******************************************************/
std::vector<int> UranianHelper::getAllDivisionPrimes() const
{
	return { 2, 3, 5, 7, 11, 13, 17, 19 };
}

/*****************************************************
**
**   UranianHelper   ---   calculateAllowedMultiples
**
******************************************************/
std::list<int> UranianHelper::calculateAllowedMultiples( const int &myPrime, const int &maxValue,
	const std::list<int> &multiples ) const
{
	//debuglog << "UranianHelper::calculateAllowedMultiples myPrime " << myPrime << " maxValue " << maxValue
		//<< " size " << multiples.size();
	int a;
	std::list<int> r;

	for( const int m : multiples )
	{
		//debuglog << "UranianHelper::calculateAllowedMultiples m " << m;
		a = m;
		while( a <= maxValue )
		{
			r.push_back( a );
			a *= myPrime;
		}
	}
	return r;
}

/*****************************************************
**
**   UranianHelper   ---   calculateDivisionsForPrimes
**
******************************************************/
std::list<int> UranianHelper::calculateDivisionsForPrimes( const std::set<int> &basePrimes, const int &maxValue ) const
{
	std::list<int> r = { 1 };
	for( const int p : basePrimes )
	{
		r = calculateAllowedMultiples( p, maxValue, r );
	}
	r.sort();
	return r;
}

/*****************************************************
**
**   UranianHelper   ---   getDivisors
**
******************************************************/
std::vector<int> UranianHelper::getDivisors( const int &division ) const
{
	assert( division > 0 );
	std::vector<int> ret;
	for( const int i : getAllDivisions())
	{
		//debuglog << "UranianHelper::getDivisors division " << division << " i " << i << " mod " << ( division % i );
		if ( ( division % i ) == 0 ) ret.push_back( i );
		if ( i >= division ) break;
	}
	//debuglog << "UranianHelper::getDivisors finished, size " << ret.size();
	return ret;
}

/**************************************************************
***
**   UranianHelper   ---   getDivisionResidua
***
***************************************************************/
std::list<DivisorResiduum> UranianHelper::getDivisionResidua( const int &base )
{
	assert( base > 0 );
	std::list<DivisorResiduum> l;
	const double steplen = 360.0 / base;

	/***
	* Example Base = 8 -> steplen = 45
	* Division Residuum
	*  1       0
	*  8       45
	*  4       90
	*  2       180
	*  8       135
	*  4       270
	*
	***/

#ifdef DEBUG_URANIANHELPER
	debuglog << "UranianHelper::getDivisionResidua base " << base << " steplen " << steplen;
#endif

	// division, residuum
	l.push_back( DivisorResiduum( 1, 0 )); // conj

	const std::vector<int> divisors = getDivisors( base );
#ifdef DEBUG_URANIANHELPER
	debuglog << "UranianHelper::getDivisionResidua ";
	for( const auto &a : divisors )
	{
		debuglog << "UranianHelper::getDivisionResidua divisor --> " << a;
	}
#endif

	for( int i = 1; i < base; ++i )
	{
		const double res = i * steplen;
		int g = base;
		for( const auto &a : divisors )
		{
			if ( a == 0 ) continue;
			if ( red_deg( a * res ) == 0 )
			{
				g = a;
				break;
			}
		}

		l.push_back( DivisorResiduum( g, res ));
	}

#ifdef DEBUG_URANIANHELPER
	debuglog << "UranianHelper::getDivisionResidua ";
	for( const auto &a : l )
	{
		debuglog << "UranianHelper::getDivisionResidua --> " << std::format( "{}: {}", a.division, a.residuum );
	}
#endif
	return l;
}

/*****************************************************
**
**   UranianHelper   ---   divisionDString
**
******************************************************/
wxString UranianHelper::divisionDString( const int &division )
{
	return std::format( "D-{}", division );
}

/*****************************************************
**
**   UranianHelper   ---   divisionDegreeString
**
******************************************************/
wxString UranianHelper::divisionDegreeString( const int &division )
{
	assert( division > 0 );
	const double dd = 360.0 / division;
	if ( dd == (int)dd )
	{
		return std::format( "{}", (int)dd );
	}
	else
	{
		return Formatter::get()->formatDegrees( dd, DegPrecision::FlexibleCutMinute );
	}
}

/*****************************************************
**
**   UranianHelper   ---   divisionFullString
**
******************************************************/
wxString UranianHelper::divisionFullString( const int &division )
{
	if( division > 0 )
	{
		return std::format( "D-{}: {}", division, divisionDegreeString( division ));
	}
	wxString s = std::format( "invalid: {}", division );
	warnlog << "UranianHelper::divisionFullString " << s;
	return s;
}

/*****************************************************
**
**   UranianHelper   ---   calculateUranianContext
**
******************************************************/
UranianContext UranianHelper::calculateUranianContext( const ContextProps_ptr props, const WidgetCategory &cat )
{
	const DataContext ctx = props->getDataContext();
	switch( cat )
	{
		case WidgetCategory::MatchesWidget:
			switch( ctx )
			{
				case DataContext::Radix:
					return UranianContext::RadixMatches;
				break;
				case DataContext::Direction:
					return UranianContext::DirectionMatches;
				break;
				case DataContext::MultiDirection:
					return UranianContext::MultiDirectionMatches;
				break;
				case DataContext::Solar:
					return UranianContext::SolarMatches;
				break;
				case DataContext::Partner:
					return UranianContext::PartnerMatches;
				break;
				default:
					assert( false );
				break;
			}
		break;
		case WidgetCategory::PpWidget:
			switch( ctx )
			{
				case DataContext::Radix:
					return UranianContext::RadixPp;
				break; 
				case DataContext::Direction:
					return UranianContext::DirectionPp;
				break; 
				case DataContext::Solar:
					return UranianContext::SolarPp;
				break;
				case DataContext::Partner:
					return UranianContext::PartnerPp;
				break;
				default:
					assert( false );
				break;
			}
		break;
		case WidgetCategory::UranianChartWidget:
			switch( ctx )
			{
				case DataContext::Radix:
					return UranianContext::RadixChart;
				break;
				case DataContext::Direction:
					return UranianContext::DirectionChart;
				break;
				case DataContext::MultiDirection:
					return UranianContext::MultiDirectionChart;
				break;
				case DataContext::Solar:
					return UranianContext::SolarChart;
				break;
				case DataContext::Partner:
					return UranianContext::PartnerChart;
				break;
				default:
					assert( false );
				break;
			}
		break;
		default:
			assert( false );
		break;
	}
	return UranianContext::None;
}

/*****************************************************
**
**   UranianHelper   ---   calculateMatch
**
******************************************************/
Match UranianHelper::calculateMatch( const double &len1, const double &len2, const double &orbis, const int &division )
{
	double circledeg = 360.0;
	double dist;
	double residuum;
	constexpr double very_small = .000000001;

	for( const int d : getDivisors( division ))
	{
		circledeg = 360.0 / d;
		dist = a_red( len2 - len1, circledeg );
		if ( dist > .5 * circledeg ) dist -= circledeg;

		/*
		debuglog << "UranianHelper::calculateMatch len1 " << len1
			<< " len2 " << len2
			<< " division " << d
			<< " dist " << dist
			<< " residuum " << residuum;
		*/

		if ( fabs ( dist ) <= orbis )
		{
			//residuum = red_deg( len1 - len2 + dist - .0000000001 );
			residuum = red_deg( len2 - len1 - dist + .0000000001 );
			if ( fabs( residuum ) < very_small ) residuum = 0;

			debuglog << "UranianHelper::calculateMatch MATCH len1 " << len1
				<< " len2 " << len2
				<< " division " << d
				<< " dist " << dist
				<< " residuum " << residuum;
			// params for ctor: divisor/orbis/residuum, add a small number in order to prevent cut on 360+x to 0
			return Match( d, dist, residuum );
		}
	}
	return Match( 0 );
}

/**************************************************************
***
**   UranianHelper   ---   getRoleTagForMatch
***
***************************************************************/
wxString UranianHelper::getRoleTagForMatch( const Match &m )
{
	assert( m.e1 );
	assert( m.e2 );
	Lang lang;

	return std::format( "{}/{}", lang.getHoroscopeRoleName( m.e1->role ), lang.getHoroscopeRoleName( m.e2->role ));
}

/**************************************************************
***
**   UranianHelper   ---   getFilterStringRepresentation
***
***************************************************************/
wxString UranianHelper::getFilterStringRepresentation( const MatchFilter &filter )
{
	Lang lang;
	constexpr Viewpoint viewpoint = Viewpoint::Western;

	const uint selPlanetSize = filter.selectedPlanets.size();
  if ( selPlanetSize == 0 )
  {
    return tr( "No filter" );
  }
  else if ( selPlanetSize == 1 )
  {
    return lang.getPlanetName( *filter.selectedPlanets.begin(), viewpoint, TextFormat::Medium );
  }
  else if ( selPlanetSize <  6 )
  {
    wxString s;
    uint i = 0;
    for( const auto &p : filter.selectedPlanets )
    {
      s += lang.getPlanetName( p, viewpoint, TextFormat::Short );
      if ( ++i < selPlanetSize ) s += ",";
    }
    return s;
  }

  //else
	return std::format( "{} {}", selPlanetSize, tr( "planets selected" ));
}

/**************************************************************
***
**   UranianHelper   ---   applyMatchFilter
***
***************************************************************/
bool UranianHelper::applyMatchFilter( const MatchFilter &filter, MTerm *lterm, MTerm *rterm ) const
{
	if ( filter.isPassThrough() ) return true;
	assert( lterm && rterm );

	switch( filter.side )
	{
		case 0: // left side
			return lterm->applyFilter( filter );
		break;
		case 1: // right side
			return rterm->applyFilter( filter );
		break;
		case 2: // both sides
		{
			switch( filter.rule )
			{
				case 0: // one must match
					return lterm->applyFilter( filter ) || rterm->applyFilter( filter );
				break;
				case 1: // all must be contained
				{
					std::set<PlanetId> obs; // contains the union of both terms' object id lists
					for( const PlanetId &ob : lterm->getPlanetIds()) obs.insert( ob );
					for( const PlanetId &ob : rterm->getPlanetIds()) obs.insert( ob );
					for( const PlanetId &ob : filter.selectedPlanets )
					{
						if ( obs.find( ob ) == obs.end() ) return false;
					}
					return true;
				}
				break;
				case 2: // only these in term
				{
					std::set<PlanetId> obs;
					for( const PlanetId &ob : lterm->getPlanetIds()) obs.insert( ob );
					for( const PlanetId &ob : rterm->getPlanetIds()) obs.insert( ob );
					for( const PlanetId &ob : obs )
					{
						if ( filter.selectedPlanets.contains( ob )) return false;
					}
					return true;
				}
				break;
				default:
					assert( false );
				break;
			}
			return lterm->applyFilter( filter ) || rterm->applyFilter( filter );
		}
		default:
			assert( false );
		break;
	}
	return true;
}

/**************************************************************
***
**   UranianHelper   ---   calculateHouses
***
***************************************************************/
std::array<double, 12> UranianHelper::calculateHouses( Horoscope_ptr horoscope, const Viewpoint &viewpoint,
	const PlanetId planetId )
{
	assert( horoscope );
	assert_valid_enum<Viewpoint>( viewpoint );
	assert_valid_enum<PlanetId>( planetId );

	std::array<double, 12 > houses( { 0.0 } );

	for( int i = 0; i < 12; i++ )
	{
		switch( planetId )
		{
			case PlanetId::Earth: // earth: cancer on 10th cusp means Libra is Ascendant
				houses[i] = red_deg( i * 30 + 180 );
			break;
			case PlanetId::Ascendant: // Ascendant is 1st cusp
				houses[i] = red_deg( i * 30 + horoscope->getLongitude( PlanetId::Ascendant, viewpoint ));
			break;
			case PlanetId::Moon: // Moon is 10th cusp
				houses[i] = red_deg( i * 30 + 90 + horoscope->getLongitude( PlanetId::Moon, viewpoint ));
			break;
			case PlanetId::AscNode: // Node is 1st cusp
				houses[i] = red_deg( i * 30 + horoscope->getLongitude( PlanetId::AscNode, viewpoint ));
			break;
			case PlanetId::Meridian: // meridian is 10th cusp, houses not equal
				houses[i] = horoscope->getHouseCusp( (House)i, viewpoint );
			break;
			case PlanetId::Sun: // Sun is 4th cusp, same for all other factors
			default:
				houses[i] = red_deg( i * 30 + 270 + horoscope->getLongitude( planetId, viewpoint ));
			break;
		}
	}

	return houses;
}

/**************************************************************
***
**   UranianHelper   ---   writeHouseReport
***
***************************************************************/
void UranianHelper::writeHouseReport( ContextProps_ptr props, MDocument_ptr doc, const Viewpoint &viewpoint )
{
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );
	Lang lang;
	Formatter *formatter = Formatter::get();

	const Planets lords = {
		PlanetId::Earth,
		PlanetId::Meridian,
		PlanetId::Sun,
		PlanetId::Ascendant,
		PlanetId::Moon,
		PlanetId::AscNode,
	};
	std::map<PlanetId, std::array<double, 12>> houses;

	// calculateHouses
	for ( const auto &lord : lords )
	{
		houses[lord] = calculateHouses( horoscope, viewpoint, lord );
	}

	const auto obs =  props->getPlanets( viewpoint );
	uint col = 1;

	{
		Table *table1 = new Table( lords.size() + 1, 13, tr( "Houses in Uranian Astrology" ));
		for( int h = 1; h <= 12; h++ )
		{
			table1->setEntry( 0, h, std::format( "{}", h ));
		}

		table1->setHeader( 0, tr( "House" ));
		for ( const auto &lord : lords )
		{
			table1->setHeader( col, lang.getPlanetName( lord, viewpoint ));
			for( int h = 0; h < 12; h++ )
			{
				table1->setEntry( col, h + 1, formatter->formatZodiacalLongitude( viewpoint, houses[lord][h] ));
			}
			col++;
		}
		doc->addItem( table1 );
	}

	{
		Table *table2 = new Table( lords.size() + 1, obs.size() + 1, tr( "Planets in Houses" ));
		table2->setHeader( 0, tr( "Planet" ));
		col = 1;
		for ( const auto &lord : lords )
		{
			table2->setHeader( col++, lang.getPlanetName( lord, viewpoint ));
		}

		uint row = 1;
		for( const auto &p : obs )
		{
			table2->setEntry( 0, row++, lang.getPlanetName( p, viewpoint ));
		}

		col = 1;
		row = 1;
		for ( const auto &lord : lords )
		{
			row = 1;
			for( const auto &p : obs )
			{
				for( int h = 0; h < 12; h++ )
				{
					const int h2 = h < 11 ? h + 1 : 0;
					if ( isLongitudeBetween( horoscope->getLongitude( p, viewpoint ), houses[lord][h], houses[lord][h2] ))
					{
						table2->setEntry( col, row, std::format( "{}", h+1 ));
						break;
					}
				}
				row++;
			}
			col++;
		}
		doc->addItem( table2 );
	}
}


