/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Uranian.cpp
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

#include "Uranian.h"

#include "astrobase.h"
#include "astroconfig.h"
#include "ContextProperties.h"
#include "Direction.h"
#include "DateFormatter.h"
#include "DoubleTermList.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "Formulary.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "constants.h"
#include "mathbase.h"
#include "TermSolver.h"
#include "UranianHelper.h"

#include <math.h>

extern Astroconfig *astroconfig;

#define DEBUG_URANIANEXPERT

/*****************************************************
**
**   MatchElement   ---   COPY Constructor
**
******************************************************/
MatchElement::MatchElement( const MatchElement &m )
	: role( m.role ),
	formula( m.formula ? new MTerm( *m.formula ) : nullptr ),
	longitude( m.longitude )
{
}

/*****************************************************
**
**   MatchElement   ---   Destructor
**
******************************************************/
MatchElement::~MatchElement()
{
	if ( formula ) delete formula;
}
	
/*****************************************************
**
**   MatchElement   ---   operator<<
**
******************************************************/
MString& operator<< ( MString &s, const MatchElement &e )
{
	Lang lang;
	s << *e.formula << MToken( lang.getRoleContextSubscriptum( e.role ), TokenFontPosition::Subscriptum );
	//s << *e.formula << "(" << MToken( lang.getRoleContextSubscriptum( e.role ), TokenFontPosition::Subscriptum ) << ")";
	return s;
}

/*****************************************************
**
**   MatchElement   ---   operator<<
**
******************************************************/
mostream& operator<< ( mostream &stream, const MatchElement &e )
{
	stream  << *e.formula << " -- ";
	return stream;
}

/*****************************************************
**
**   MatchElement   ---   debugDump
**
******************************************************/
wxString MatchElement:: debugDump()
{
	MString ms;
	Lang lang;
	ms << " role: " << enum_to_string( role )
		<< " (" <<  lang.getRoleContextSubscriptum( role )
		<< ") form: "
		<< *formula
		<< " longitude: " << std::format( "{}", longitude );
	return ms.toWxString();
}

/*****************************************************
**
**   Match   ---   COPY Constructor
**
******************************************************/
Match::Match( const Match &m )
{
	division = m.division;
	orbis = m.orbis;
	residuum = m.residuum;
	duedate = m.duedate;
	lspeed = m.lspeed;
	bspeed = m.bspeed;
	formulaId = m.formulaId;

	if ( m.e1 ) e1 = new MatchElement( *m.e1 );
	else e1 = nullptr;

	if ( m.e2 ) e2 = new MatchElement( *m.e2 );
	else e2 = nullptr;
}

/*****************************************************
**
**   UranianExpert   ---   clear
**
******************************************************/
void UranianExpert::clear()
{
	debuglog << "UranianExpert::clear, currently " << matches.size() << " elements";
	obs.clear();
	matches.clear();
	usection->matchCount = 0;
}

/*****************************************************
**
**   UranianExpert   ---   sort
**
******************************************************/
void UranianExpert::sort()
{
	debuglog << "UranianExpert::sort " << (int)usection->sortOrder ;
	matches.sort( MatchSorter( usection->sortOrder, obs ));
}

/*****************************************************
**
**   UranianExpert   ---   getCorrectedDirectionSpeed
**
******************************************************/
double UranianExpert::getCorrectedDirectionSpeed( const HoroscopeRole &role, const double &speed )
{
	constexpr double synodic_period = 29.53;

	switch( role )
	{
		case HoroscopeRole::Progressive:
			return speed / astroconfig->directions.yearLength.getValue();
		break;
		// TODO richtig?
		case HoroscopeRole::Lunations:
			return speed / synodic_period;
		break;
		default:
		break;
	}
	return speed;
}

typedef std::shared_ptr<const Horoscope> Horoscope_const_ptr;


/*****************************************************
**
**   UranianExpert   ---   calculateSpeed
**
******************************************************/
double UranianExpert::calculateSpeed( Horoscope_ptr horoscope1, MTerm *lterm, Horoscope_ptr horoscope2, MTerm *rterm,
	const Viewpoint &viewpoint )
{
	DirectionCalculator *calculator = DirectionCalculator::get();
	//const double speed_multiplicator = calculator->getSpeedMultiplicator( role1 );

	assert( horoscope1 );
	const HoroscopeRole role1 = horoscope1->getRole();

	assert( horoscope2 );
	const HoroscopeRole role2 = horoscope2->getRole();

	//Horoscope_const_ptr hh = horoscope1;
	//hh->update();

	assert( lterm );
	assert( rterm );

#ifdef DEBUG_URANIANEXPERT
	DateFormatter *dfmt = DateFormatter::get();
	debuglog << "UranianExpert::calculateSpeed terms " << *lterm << "=" << *rterm << " role1 " << role1 << " role2 " << role2;
#endif

	if ( role2 == HoroscopeRole::Radix )
	{
		if ( role1 == HoroscopeRole::Radix ) // radix:radix
		{
			const auto[ longitude1, speed1 ] = lterm->evaluateSpeed( horoscope1, viewpoint );
			const auto[ longitude2, speed2 ] = rterm->evaluateSpeed( horoscope2, viewpoint );

#ifdef DEBUG_URANIANEXPERT
	debuglog << "UranianExpert::calculateSpeed RADIX/RADIX " << *lterm << "=" << *rterm << " role " << role1
		<< " longitude1 " << longitude1 << " speed1 " << speed1
		<< " longitude2 " << longitude2 << " speed2 " << speed2
		<< " result speed " << ( speed1 - speed2 );
#endif

			return speed1 - speed2;
		}
		else if ( calculator->isShiftedRole ( role1 )) // shifted:radix
		{
			// ALT
			//const double dirJD = calculator->transitToProgressiveJD( horoscope2->getJD(), horoscope1->getTransitJD() );
			const double dirJD = calculator->transitToDirectionJD( horoscope2, viewpoint, horoscope1->getTransitJD(), role1 );
			const auto [ arc, speed ] = calculator->calcArcSpeed( horoscope2, viewpoint, horoscope1->getTransitJD(), dirJD, role1 );

#ifdef DEBUG_URANIANEXPERT
	debuglog << "UranianExpert::calculateSpeed/ARC " << *lterm << "=" << *rterm << " role " << role1
		<< " direction date " << dfmt->formatUTDatetime( dirJD )
		<< " arc " << arc << " speed " << speed;
#endif

			// TODO speed multiplicator
			return speed;
		}
		else if ( calculator->isDirectionRole ( role1 )) // direction(transit,progression,lunation):radix
		{
			calculator->calculateDirection( horoscope2, horoscope1, viewpoint );
			const auto[ longitude, speed ] = lterm->evaluateSpeed( horoscope1, viewpoint );
			const double speed_multiplicator = calculator->getSpeedMultiplicator( role1 );
			assert( speed_multiplicator );

#ifdef DEBUG_URANIANEXPERT
	debuglog << "UranianExpert::calculateSpeed/general direction " << *lterm << "=" << *rterm << " role " << role1
		<< " speed_multiplicator " << speed_multiplicator
		<< " longitude " << longitude << " speed " << speed;
#endif

			return speed / speed_multiplicator;
			//return speed;
		}
	}
	else if ( role2 == role1 && calculator->isDirectionRole ( role1 )) // direction:direction
	{
		calculator->calculateDirection( horoscope2, horoscope1, viewpoint );
		const auto[ longitude1, speed1 ] = lterm->evaluateSpeed( horoscope1, viewpoint );
		const auto[ longitude2, speed2 ] = rterm->evaluateSpeed( horoscope1, viewpoint );
		const double speed_multiplicator = calculator->getSpeedMultiplicator( role1 );
		assert( speed_multiplicator );

#ifdef DEBUG_URANIANEXPERT
	debuglog << "UranianExpert::calculateSpeed DIR/DIR " << *lterm << "=" << *rterm << " role " << role1
		<< " longitude1 " << longitude1 << " speed1 " << speed1
		<< " longitude2 " << longitude2 << " speed2 " << speed2
		<< " speed_multiplicator " << speed_multiplicator
		<< " result speed " << ( speed1 - speed2 ) / speed_multiplicator;
#endif

		return ( speed1 - speed2 ) / speed_multiplicator;
	}
	else
	{
		warnlog << "UranianExpert::calculateSpeed: unsupported role combination " << role1 << ":" << role2;
	}
	return 0.0;
}

/*****************************************************
**
**   UranianExpert   ---   findMatches
**
******************************************************/
std::list<Match> &UranianExpert::findMatches( Horoscope_ptr horoscope1, Horoscope_ptr horoscope2,
	const double &orbis, const MatchFilter *filter )
{
	Match match;
	double longitude1 = 0.0;
	double longitude2 = 0.0;
	const Viewpoint viewpoint = getViewpoint();

	UranianHelper *uh = UranianHelper::get();
	assert( usection );
	const std::list<DivisorResiduum> reslist = uh->getDivisionResidua( usection->matchDivision );

	assert( horoscope1 );
	HoroscopeRole role1 = horoscope1->getRole();

	assert( horoscope2 );
	HoroscopeRole role2 = horoscope2->getRole();

	const DataContext dataContext = props->getDataContext();
	double shift1Value = 0;
	double shift2Value = 0;
	//wxString subscriptum1 = lang.getRoleContextSubscriptum( horoscope1->getRole();

	if ( dataContext == DataContext::Partner )
	{
		role1 = HoroscopeRole::Partner1;
		role2 = HoroscopeRole::Partner2;

		if ( usection->enablePlanetShift )
		{
			const auto a1 = usection->shiftPlanetIds.find( HoroscopeRole::Partner1 );
			if ( a1 != usection->shiftPlanetIds.end())
			{
				shift1Value = horoscope1->getLongitude( a1->second, viewpoint );
			}

			const auto a2 = usection->shiftPlanetIds.find( HoroscopeRole::Partner2 );
			if ( a2 != usection->shiftPlanetIds.end())
			{
				shift2Value = horoscope2->getLongitude( a2->second, viewpoint );
			}
		}
	}

	obs = props->getPlanets( viewpoint );

#ifdef DEBUG_URANIANEXPERT
	StopWatch sw;
	debuglog << "UranianExpert::findMatches role1 " << role1 << " role2 " << role2 << " viewpoint " << viewpoint << " data context " << dataContext
		<< " shift1Value " << shift1Value << " shift2Value " << shift2Value;

	for( const auto &p : obs )
	{
		debuglog << "UranianExpert::findMatches planet " << p;
	}
#endif

	for( auto a : DoubleTermList( obs, usection, role1, role2, filter ? filter : &usection->filter ))
	{
		longitude1 = red_deg( a.lterm->evaluate( horoscope1, viewpoint ) + shift1Value );
		longitude2 = red_deg( a.rterm->evaluate( horoscope2, viewpoint ) + shift2Value );

		for( const auto &r : reslist )
		{
			if ( matches.size() >= astroconfig->uranian.maxMatchCount )
			{
				warnlog << "UranianExpert::findMatches: maximum number of matches ("	
					<< astroconfig->uranian.maxMatchCount << ") exceeded.";
				usection->matchCount = matches.size();
				return matches;
			}

			assert( r.division );
			if ( isLongitudeBetween( red_deg( longitude1 - r.residuum ), red_deg( longitude2 - orbis ), red_deg( longitude2 + orbis )))
			{
				double orbis = red_deg( longitude1 - longitude2 - r.residuum );

				if ( orbis > 180 )
				{
					orbis = orbis - 360.0;
				}
				Match match( r.division, orbis, r.residuum );

				match.formulaId = a.formulaId;

				if ( usection->matchesShowSpeed )
				{
					match.lspeed = calculateSpeed( horoscope1, a.lterm, horoscope2, a.rterm, viewpoint );
				}
				match.e1 = new MatchElement( role1, new MTerm( *a.lterm ), longitude1 );
				match.e2 = new MatchElement( role2, new MTerm( *a.rterm ), longitude2 );

#ifdef DEBUG_URANIANEXPERT
			debuglog << "MATCH" 
				<< " --- l1 " << longitude1
				<< " l2 " << longitude2
				<< " division " << match.division
				<< " RES " << match.residuum 
				<< " ORBIS " << match.orbis
				<< " formula " << *a.lterm << " = " << *a.rterm;
#endif
				matches.push_back( match );
				break;
			}
		}

	}
	usection->matchCount = matches.size();

#ifdef DEBUG_URANIANEXPERT
	debuglog << "FINISHED findMatches size " << matches.size() << " in " << sw.getElapsedMillisec() << " millisec";
#endif
	return matches;
}

/*****************************************************
**
**   UranianExpert   ---   calculateRadix
**
******************************************************/
void UranianExpert::calculateRadix()
{
#ifdef DEBUG_URANIANEXPERT
	debuglog << "Start UranianExpert::calculateRadix viewpoint " << getViewpoint();
	StopWatch sw;
#endif
	Horoscope_ptr radixHoroscope = props->getRadixHoroscope();
	assert( radixHoroscope );
	clear();

	findMatches( radixHoroscope, radixHoroscope, usection->orbis );

	if ( usection->matchesShowDuedate )
	{
		TermSolver solver( radixHoroscope, getViewpoint() );

		for( Match &m : matches )
		{
			assert( m.e1 );
			assert( m.e2 );
			m.duedate = solver.findRadixMatchDate( m.e1->formula, m.e2->formula, radixHoroscope->getJD(), m.residuum );
		}
	}
	sort();
#ifdef DEBUG_URANIANEXPERT
	debuglog << "FINISHED UranianExpert::calculateRadix " << sw.getElapsedMillisec() << " millisec";
#endif
}

/*****************************************************
**
**   UranianExpert   ---   calculateDirections
**
******************************************************/
void UranianExpert::calculateDirections( const double & jd )
{
	StopWatch sw;
	const auto roles = props->getActiveRoles();
	if ( roles.size() < 2 )
	{
		errorlog << "UranianExpert::calculateDirections has wrong active roles size " << roles.size();
		return;
	}
	if ( ! ( roles[1] >= HoroscopeRole::FirstDirection && roles[1] <= HoroscopeRole::LastDirection ))
	{
		errorlog << "UranianExpert::calculateDirections has wrong role at pos 1 " << roles[1];
		return;
	}

	Horoscope_ptr radixHoroscope = props->getRadixHoroscope();
	assert( radixHoroscope );
	Horoscope_ptr htransit = props->getHoroscope( roles[1] );
	assert( htransit );

#ifdef DEBUG_URANIANEXPERT
		DateFormatter *dfmt = DateFormatter::get();
		debuglog << "UranianExpert::calculateDirections startjd " << dfmt->formatUTDatetime( jd )
			<< " direction role " << htransit->getRole();
#endif 

	clear();

	if ( roles[1] == HoroscopeRole::Progressive )
	{
		switch( usection->progressionMode )
		{
			case 0: // Radix-Progression
				findMatches( htransit, radixHoroscope, usection->orbis );
			break;
			case 1: // Progression only
				findMatches( htransit, htransit, usection->orbis );
			break;
			default: // both
				findMatches( htransit, radixHoroscope, usection->orbis );
				findMatches( htransit, htransit, usection->orbis );
			break;
		}
	}
	else
	{
		findMatches( htransit, radixHoroscope, usection->orbis );
	}

	if ( usection->matchesShowDuedate )
	{
		calculateDirectionDueDates( htransit, jd );
	}
	sort();
	debuglog << "FINISHED UranianExpert::calculateDirections in " << sw.getElapsedMillisec() << " millisec";
}

/*****************************************************
**
**   UranianExpert   ---   calculateMultiDirections
**
******************************************************/
void UranianExpert::calculateMultiDirections( const double &jd, const std::list<RoleRestrictor> &restrictors )
{
	StopWatch sw;
	debuglog << "START UranianExpert::calculateMultiDirections";
	const auto roles = props->getActiveRoles();
	if ( roles.size() < 2 )
	{
		errorlog << "UranianExpert::calculateMultiDirections has wrong roles size " << roles.size();
		return;
	}
	Horoscope_ptr radixHoroscope = props->getRadixHoroscope();
	assert( radixHoroscope );
	clear();

	for( const auto &role : roles )
	{
		if ( role == HoroscopeRole::Radix ) continue;

		if ( ! ( role >= HoroscopeRole::FirstDirection && role <= HoroscopeRole::LastDirection ))
		{
			errorlog << "UranianExpert::calculateMultiDirections has wrong role " << role;
			return;
		}

		debuglog << "UranianExpert::calculateMultiDirections role " << role;
		const auto &r = std::find_if( restrictors.begin(), restrictors.end(),
			[&role] ( const RoleRestrictor &r ) { return r.role == role; } );

		if ( r == restrictors.end() || ! r->enabled ) continue;

		Horoscope_ptr h = props->getHoroscope( role );
		assert( h );
		findMatches( h, radixHoroscope, usection->orbis, &r->filter );
		if ( usection->matchesShowDuedate )
		{
			calculateDirectionDueDates( h, jd );
		}
		debuglog << "UranianExpert::calculateMultitransit role " << role << " finished, " << matches.size() << "matches";;
	}

	sort();
	debuglog << "FINISHED UranianExpert::calculateMultitransit in " << sw.getElapsedMillisec() << " millisec";
}

/*****************************************************
**
**   UranianExpert   ---   calculateSolar
**
******************************************************/
void UranianExpert::calculateSolar()
{
	StopWatch sw;
	const auto roles = props->getActiveRoles();
	if ( roles.size() < 2 )
	{
		errorlog << "UranianExpert::calculateSolar has wrong active roles size " << roles.size();
		return;
	}
	if ( roles[1] != HoroscopeRole::Solar )
	{
		errorlog << "UranianExpert::calculateSolar has wrong role at pos 1 " << roles[1];
		return;
	}

	Horoscope_ptr radixHoroscope = props->getRadixHoroscope();
	assert( radixHoroscope );
	Horoscope_ptr hsolar = props->getHoroscope( HoroscopeRole::Solar );
	assert( hsolar );

	clear();

	findMatches( hsolar, radixHoroscope, usection->orbis );

	sort();
	debuglog << "FINISHED UranianExpert::calculateSolar in " << sw.getElapsedMillisec() << " millisec";
}

/*****************************************************
**
**   UranianExpert   ---   calculatePartner
**
******************************************************/
void UranianExpert::calculatePartner()
{
	clear();

	Horoscope_ptr h1 = props->getHoroscope( HoroscopeRole::Partner2 );
	Horoscope_ptr h2 = props->getHoroscope( HoroscopeRole::Partner1 );
	if ( ! h1 || ! h2 ) return; // may happen on empty charts

	findMatches( h2, h1, usection->orbis );
	matches.sort( MatchSorter( usection->sortOrder, obs ));
}

#define IS_MOVABLE_ROLE( role ) ( role != HoroscopeRole::Radix )

/*****************************************************
**
**   UranianExpert   ---   calculateDirectionDueDates
**
******************************************************/
void UranianExpert::calculateDirectionDueDates( Horoscope_ptr, const double& /*startjd*/ )
{
	StopWatch sw;
	const Viewpoint viewpoint = getViewpoint();
	DirectionCalculator *calculator = DirectionCalculator::get();

	Horoscope_ptr radixHoroscope = props->getRadixHoroscope();
	assert( radixHoroscope );
	TermSolver solver( radixHoroscope, viewpoint );

	for( Match &m : matches )
	{
		assert( m.e1 );
		const HoroscopeRole role1 = m.e1->role;
		Horoscope_ptr horoscope1 = props->getHoroscope( role1 );
		assert( horoscope1 );

		assert( m.e2 );
		const HoroscopeRole role2 = m.e2->role;
		Horoscope_ptr horoscope2 = props->getHoroscope( role2 );
		assert( horoscope2 );

#ifdef DEBUG_URANIANEXPERT
		DateFormatter *dfmt = DateFormatter::get();

		debuglog << "MATCH TERM " << *m.e1->formula << " = " << *m.e2->formula;
		debuglog << "MATCH LEFT " << m.e1->debugDump();
		debuglog << "MATCH RIGHT " << m.e2->debugDump();
		debuglog << "MATCH division " << m.division << " res " << m.residuum << " orbis " << m.orbis;
		debuglog << "ROLE1 " << horoscope1->getRole();
		debuglog << "ROLE2 " << horoscope2->getRole();
		debuglog << "DATE1 " << dfmt->formatUTDatetime( horoscope1->getJD());
		debuglog << "DATE2 " << dfmt->formatUTDatetime( horoscope2->getJD());
#endif 

		if ( role1  == HoroscopeRole::Radix )
		{
			assert( calculator->isDirectionRole( role2 ));
			const double targetlen = red_deg( m.e1->longitude + m.residuum );
			m.duedate = solver.findDirectionMatchDate( m.e2->formula, role2, horoscope2->getTransitJD(), targetlen );
		}
		else if ( role2  == HoroscopeRole::Radix )
		{
			assert( calculator->isDirectionRole( role1 ));
			// target is on right side
			const double targetlen = red_deg( m.e2->longitude + m.residuum );
			m.duedate = solver.findDirectionMatchDate( m.e1->formula, role1, horoscope1->getTransitJD(), targetlen );
		}
		else if ( role1  == HoroscopeRole::Progressive && role2  == HoroscopeRole::Progressive )
		{
			m.duedate = solver.findInterProgressionMatchDate( m.e1->formula, m.e2->formula, horoscope1->getTransitJD(), m.residuum );
		}
		else
		{
			warnlog << "UranianExpert::calculateDirectionDueDates: Unsupported context combination " <<  role1 << " / " << role2;
		}

	}
	debuglog << "Finished UranianExpert::calculateDirectionDueDates in " << sw.getElapsedMillisec() << " millisec";
}

/*****************************************************
**
**   MatchSorter   ---   operator()
**
******************************************************/
bool MatchSorter::operator()( const Match& m1, const Match& m2 ) const
{
	switch ( order )
	{
		case EventSortOrder::LeftPlanet:
			if ( m1.e1 && m2.e1 && *m1.e1->formula != *m2.e1->formula )
				return TermComparator( obs )( *m1.e1->formula, *m2.e1->formula );
		break;
		case EventSortOrder::RightPlanet:
			if ( m1.e2 && m2.e2 && *m1.e2->formula != *m2.e2->formula )
				return TermComparator( obs )( *m1.e2->formula, *m2.e2->formula );
		break;
		case EventSortOrder::Orbis:
			return m1.orbis < m2.orbis;
		break;
		case EventSortOrder::OrbisReverse:
			return m1.orbis > m2.orbis;
		break;
		case EventSortOrder::OrbisAbsolute:
			return fabs( m1.orbis ) < fabs( m2.orbis );
		break;
		case EventSortOrder::Date:
			return m1.duedate < m2.duedate;
		break;
		case EventSortOrder::DateReverse:
			return m1.duedate > m2.duedate;
		break;
		case EventSortOrder::Type: // sort according to division
			return m1.division < m2.division;
		break;
		case EventSortOrder::TypeReverse:
			return m1.division > m2.division;
		break;
		case EventSortOrder::Speed:
			return m1.lspeed < m2.lspeed;
		break;
		case EventSortOrder::SpeedReverse:
			return m1.lspeed > m2.lspeed;
		break;
		case EventSortOrder::Score:
			// fallback
			return fabs( m1.orbis ) < fabs( m2.orbis );
		break;
		default:
			errorlog << "Invalid sort order " << (int)order << " in MatchSorter";
		break;
	}
	return false;
}


