/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/EventFinder.cpp
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

#include "EventFinder.h"

#include "astrobase.h"
#include "astroconfig.h"
#include "Calculator.h"
#include "constants.h"
#include "ContextProperties.h"
#include "DateFormatter.h"
#include "Direction.h"
#include "DoubleTermList.h"
#include "enumserializer.h"
#include "Formulary.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "Term.h"
#include "TermSolver.h"
#include "UranianHelper.h"

#include <thread>

extern Astroconfig *astroconfig;

#define DEBUG_EVENT_FINDER

/*****************************************************
**
**   EventFinder   ---   Constructor
**
******************************************************/
EventFinder::EventFinder( ContextProps_ptr props, UranianSection *usection )
	: orig_props( props ),
	usection( usection )
{
	assert( usection );
}

/*****************************************************
**
**   EventFinder   ---   Destructor
**
******************************************************/
EventFinder::~EventFinder()
{
	if ( isCalculating.load())
	{
		cancelCalculation();
	}
}

/*****************************************************
**
**   EventFinder   ---   prepare
**
******************************************************/
void EventFinder::prepare()
{
	UranianHelper *uh = UranianHelper::get();
	reslist = uh->getDivisionResidua( usection->matchDivision );

	matches.clear();

	props = ContextProps_ptr( new ContextProperties( *orig_props ));
	obs = props->getPlanets( getViewpoint());

#ifdef DEBUG_EVENT_FINDER
		for( const auto &r : reslist )
		{
			debuglog << "EventFinder::prepare: division " << r.division << " residuum " << r.residuum;
		}
#endif

	if ( ! hradix )
	{
		hradix = Horoscope_ptr( new Horoscope() );
	}
	auto h1 = orig_props->getRadixHoroscope();
	assert( h1 );
	hradix->dl = h1->dl;
	hradix->update();
	props->setRadixHoroscope( hradix );

	for( const auto &r : astroconfig->directions.eventFinderItems )
	{
		Horoscope_ptr h = props->getHoroscope( r.role );
		if ( ! h )
		{
#ifdef DEBUG_EVENT_FINDER
		debuglog << "EventFinder::prepare: create Horoscope for role " << r.role;
#endif
			h = Horoscope_ptr( new Horoscope( r.role ));
			h->dl.location = hradix->dl.location;
		}
		props->setHoroscope( r.role, h );
	}
}

/*****************************************************
**
**   EventFinder   ---   sort
**
******************************************************/
void EventFinder::sort()
{
#ifdef DEBUG_EVENT_FINDER
	debuglog << "EventFinder::sort " << (int)usection->sortOrder ;
#endif
	if ( isCalculating.load())
	{
		errorlog << "EventFinder::sort: cannot sort while running";
	}
	else
	{
		matches.sort( MatchSorter( usection->sortOrder, obs ));
	}
}

/*****************************************************
**
**   EventFinder   ---   startCalculation
**
******************************************************/
void EventFinder::startCalculation( const int &year )
{ 
#ifdef DEBUG_EVENT_FINDER
	debuglog << "EventFinder::startCalculation year " << year;
#endif
	Calculator *calculator = CalculatorFactory().getCalculator();

	const double jd1 = calculator->calc_jd( year, 1, 1, 0.0 );
	const double jd2 = calculator->calc_jd( year + 1, 1, 1, 0.0 );
	startCalculation( jd1, jd2 );
}


/*****************************************************
**
**   EventFinder   ---   startCalculation
**
******************************************************/
void EventFinder::startCalculation( const double &jd1, const double &jd2 )
{ 
	isCalculating.store( true );
	prepare();

	matches.clear();
	usection->matchCount = 0;

	if ( jd1 == jd2 ) return;

	if ( jd1 < jd2 )
	{
		startJD = jd1;
		endJD = jd2;
	}
	else
	{
		startJD = jd2;
		endJD = jd1;
	}
	assert( startJD < endJD );

	duration = endJD - startJD;
	assert( duration > 0 );

#ifdef DEBUG_EVENT_FINDER
	DateFormatter *df = DateFormatter::get();
	debuglog << "EventFinder::startCalculation startJD " << df->formatUTDate( jd1 )
		<< " endJD " << df->formatUTDate( jd2 )
		<< " duration " << duration << " days "
		<< " viewpoint " << getViewpoint();
#endif

	t = std::thread( &EventFinder::calculate, this );
}

/*****************************************************
**
**   EventFinder   ---   cancelCalculation
**
******************************************************/
void EventFinder::cancelCalculation()
{
#ifdef DEBUG_EVENT_FINDER
	debuglog << "EventFinder::cancelCalculation";
#endif

	if ( ! isCalculating.load())
	{
		warnlog << "EventFinder::cancelCalculation: calculation is not running";
	}
	else
	{
		doCancel.store( true );
		t.join();
		matches.clear();
		usection->matchCount = 0;
		doCancel.store( false );
		isCalculating.store( false );
	}
}

/*****************************************************
**
**   EventFinder   ---   waitForCalculation
**
******************************************************/
void EventFinder::waitForCalculation()
{
#ifdef DEBUG_EVENT_FINDER
	debuglog << "EventFinder::waitForCalculation";
#endif

	//if ( isCalculating.load())
	if ( true )
	{
		t.join();
		isCalculating.store( false );
	}
}

/*****************************************************
**
**   EventFinder   ---   calculate
**
******************************************************/
void EventFinder::calculate()
{ 
#ifdef DEBUG_EVENT_FINDER
	debuglog << "Start EventFinder::calculate";
#endif
	StopWatch sw;

#if 1

	for( auto &r : astroconfig->directions.eventFinderItems )
	{

#ifdef DEBUG_EVENT_FINDER
		debuglog << "EventFinder::calculateEvents role:" << r.role
			<< " enabled " << std::boolalpha << r.enabled
			<< " filter select count " << r.filter.selectedPlanets.size();
#endif

		if ( ! r.enabled ) continue;
		switch( r.role )
		{
			case HoroscopeRole::Transit:
			case HoroscopeRole::Lunations:
				findIntervalls( r.role, HoroscopeRole::Radix, &r.filter );
			break;
			case HoroscopeRole::Progressive:
				switch( usection->progressionMode )
				{
					case 0: // Radix-Progression
						findIntervalls( r.role, HoroscopeRole::Radix, &r.filter );
					break;
					case 1: // Progression only
						findIntervalls( r.role, r.role, &r.filter );
					break;
					default: // both
						findIntervalls( r.role, HoroscopeRole::Radix, &r.filter );
						findIntervalls( r.role, r.role, &r.filter );
					break;
				}
			break;

			default:
				if ( DirectionCalculator::get()->isShiftedRole( r.role ))
				{
					//calculateEventsArc( r.role, &r.filter );
					findIntervalls( r.role, HoroscopeRole::Radix, &r.filter );
				}
				else
				{
					warnlog << "EventFinder::calculateEvents mode " << r.role << " not implemented";
				}
			break;
		}
	}

	matches.sort( MatchSorter( usection->sortOrder, obs ));
	usection->matchCount = matches.size();
	isCalculating.store( false );


#else
	// dummy
	while( sw.getElapsedMillisec() < 5000 )
	{
		if ( doCancel.load())
		{
			break;
		}
	}
#endif

	if ( doCancel.load())
	{
		infolog << "EventFinder::calculate canceled after " << sw.getElapsedMillisec() << " millisec";
	}
	else
	{
		infolog << "Finished EventFinder::calculate found " << matches.size() << " matches in " << sw.getElapsedMillisec() << " millisec";
	}
}

/*****************************************************
**
**   EventFinder   ---   findIntervalls
**
******************************************************/
void EventFinder::findIntervalls( const HoroscopeRole &role1, const HoroscopeRole &role2, const MatchFilter *filter )
{
	DirectionCalculator *calculator = DirectionCalculator::get();
	const Viewpoint viewpoint = getViewpoint();
	if ( doCancel.load()) return;
/*
	Left side: role1, e.g transit, progression, solar arc
	Right side: role2 is radix in most cases (e.g. transit = radix ) OR same as role1 e.g. progressive = progressive
*/
	assert( calculator->isDirectionRole( role1 ));
	assert( role2 == HoroscopeRole::Radix || role1 == role2 );

	const double speed_multiplicator = calculator->getSpeedMultiplicator( role1 );
	assert( speed_multiplicator );

#ifdef DEBUG_EVENT_FINDER
	DateFormatter *df = DateFormatter::get();
	int count = 0;

	debuglog << "EventFinder::findIntervalls role1 " << role1
		<< " role2 " << role2
		<< " startJD " << df->formatUTDate( startJD )
		<< " endJD " << df->formatUTDate( endJD )
		<< " duration " << duration
		<< " speed_multiplicator " << speed_multiplicator;
		;
#endif

	Horoscope_ptr h1 = props->getHoroscope( role1 );
	assert( h1 );
	const double middleJD = startJD + .5 * duration;
	h1->setTransitJD( middleJD );

	Horoscope_ptr h2 = props->getHoroscope( role2 );
	assert( h2 );

	for( auto a : DoubleTermList( props->getPlanets( viewpoint ), usection, role2, role1, filter ))
	{
		if ( doCancel.load()) return;

		double speed = 0;
		if ( role2 == HoroscopeRole::Radix )
		{
			if ( calculator->isShiftedRole( role1 ))
			{
				const double dirJD = calculator->transitToProgressiveJD( h2->getJD(), middleJD );
				const auto[ arc1, speed1 ] = calculator->calcArcSpeed( h2, viewpoint, middleJD, dirJD, role1 );
				speed = fabs( speed1 );
			}
			else
			{
				const auto [ l1, speed1 ] = a.lterm->evaluateSpeed( h1, viewpoint );
				speed = fabs( speed1 / speed_multiplicator );
			}
		}
		else if ( role1 == role2 )
		{
			const auto [ l1, speed1 ] = a.lterm->evaluateSpeed( h1, viewpoint );

			h2->setTransitJD( middleJD );
			const auto [ l2, speed2 ] = a.rterm->evaluateSpeed( h1, viewpoint );
			speed = fabs( ( speed1 - speed2 ) / speed_multiplicator );
		}
		else
		{
			errorlog << "EventFinder::findIntervalls has unsupported roles combination " << role1 << ":" << role2;
			exit( 1 );
		}
		assert( speed );

		//const double total_rounds = fabs( duration * speed / 365.25 );
		const double total_rounds = fabs( duration * speed );
		const int nb_steps = std::max( 1.0, total_rounds * ( usection->matchDivision + 1 ));
		double step_len = duration/nb_steps;

#ifdef DEBUG_EVENT_FINDER
		debuglog << "EventFinder::findIntervalls Step " << ++count << " "
			<< *a.lterm << "(" << role1 << ") = " << *a.rterm << "(" << role2 << ")"
			<< " speed " << speed
			<< " total_rounds " << total_rounds
			<< " nb_steps " << nb_steps
			<< " step len " << step_len;
#endif

		for( int i = 0; i < nb_steps; i++ )
		{
			if ( doCancel.load()) return;

			if ( role2 == HoroscopeRole::Radix )
			{
				const double lradix = a.rterm->evaluate( hradix, viewpoint );
				calculateDRPart( a, h1, lradix, startJD + i * step_len, startJD + ( i + 1 ) * step_len );
			}
			else
			{
				calculateDDPart( a, h1, startJD + i * step_len, startJD + ( i + 1 ) * step_len );
			}
			if ( matches.size() >= astroconfig->uranian.maxMatchCount ) return;
		}

	}
}

/*****************************************************
**
**   EventFinder   ---   calculateDDPart = direction to direction
**
******************************************************/
// nur direction
void EventFinder::calculateDDPart( DoubleTerm &a, Horoscope_ptr h1, const double &jd1, const double &jd2 )
{
	DirectionCalculator *calculator = DirectionCalculator::get();
	const Viewpoint viewpoint = getViewpoint();
	if ( doCancel.load()) return;

	assert( h1 );
	const HoroscopeRole &role = h1->getRole();
	assert ( calculator->isDirectionRole( role ));
	assert ( ! calculator->isShiftedRole( role ));

	h1->setTransitJD( jd1 );
	calculator->calculateDirection( hradix, h1, viewpoint );
	const double llstart = a.lterm->evaluate( h1, viewpoint );
	const double lrstart = a.rterm->evaluate( h1, viewpoint );
	//const double diff1 = llstart - lrstart;
	const double diff1 = red_deg( llstart - lrstart );

	h1->setTransitJD( jd2 );
	calculator->calculateDirection( hradix, h1, viewpoint );
	const double llend = a.lterm->evaluate( h1, viewpoint );
	const double lrend = a.rterm->evaluate( h1, viewpoint );
	const double diff2 = red_deg( llend - lrend );

#ifdef DEBUG_EVENT_FINDER
	const double ldiff = red_deg( llend - llstart );
	const double rdiff = red_deg( lrend - lrstart );

	DateFormatter *df = DateFormatter::get();
	debuglog << "EventFinder::calculateDDPart " 
		<< *a.lterm << "(" << role << ") = " << *a.rterm << "(" << role << ")"
		<< " jd1 " << df->formatUTDatetime( jd1 )
		<< " jd2 " << df->formatUTDatetime( jd2 )
		<< " duration " << ( jd2 - jd1 )
		<< " llstart " << llstart
		<< " lrstart " << lrstart
		<< " llend " << llend
		<< " lrend " << lrend
		<< " diff1 " << diff1
		<< " diff2 " << diff2
		<< " ldiff " << ldiff
		<< " rdiff " << rdiff
		;
#endif

	if ( diff1 * diff2 < 0 )
	//if ( ldiff * rdiff < 0 )
	{
		debuglog << "EventFinder::calculateDDPart MATCH ";
	}
}

/*****************************************************
**
**   EventFinder   ---   calculateDRPart = direction to radix
**
******************************************************/
// h1 ist direction, gegen hradix
void EventFinder::calculateDRPart( DoubleTerm &a, Horoscope_ptr h1, const double &lradix, const double &jd1,
	const double &jd2 )
{
	if ( doCancel.load()) return;

	DirectionCalculator *calculator = DirectionCalculator::get();
	const Viewpoint viewpoint = getViewpoint();

	assert( h1 );
	const HoroscopeRole &role = h1->getRole();
	assert ( calculator->isDirectionRole( role ));

	assert( hradix );
	TermSolver solver( hradix, viewpoint );

	assert( jd2 > jd1 );
	const double middleJD = jd1 + .5 * ( jd2 - jd1 );

	h1->setTransitJD( jd1 );
	calculator->calculateDirection( hradix, h1, viewpoint );
	const double lstart = a.lterm->evaluate( h1, viewpoint );

	h1->setTransitJD( jd2 );
	calculator->calculateDirection( hradix, h1, viewpoint );
	const double lend = a.lterm->evaluate( h1, viewpoint );

	double l1 = lstart;
	double l2 = lend;
	double span = lend - lstart;

	// retrogression: assume of maximun 30 deg per interval
	if ( span < 0 && span > -30 )
	{
		l1 = lend;
		l2 = lstart;
	}

#ifdef DEBUG_EVENT_FINDER
	DateFormatter *df = DateFormatter::get();
	debuglog << "EventFinder::calculateDRPart " 
		<< *a.lterm << "(" << role << ") = " << *a.rterm << "(radix)"
		<< " jd1 " << df->formatUTDatetime( jd1 )
		<< " jd2 " << df->formatUTDatetime( jd2 )
		<< " duration " << ( jd2 - jd1 )
		<< " lradix " << lradix
		<< " l1 " << l1
		<< " l2 " << l2
		;
#endif

	for( const auto &r : reslist )
	{
		if ( doCancel.load()) return;
		assert( r.division );
#ifdef DEBUG_EVENT_FINDER
		debuglog << "EventFinder::calculateDRPart res division " << r.division << " res " << r.residuum;
#endif

		if ( isLongitudeBetween( red_deg( lradix + r.residuum ), l1, l2 ))
		{
			Match match( r.division, 0, r.residuum );
			match.duedate = solver.findDirectionMatchDate( a.lterm, role, middleJD, red_deg( lradix + r.residuum ));

#ifdef DEBUG_EVENT_FINDER
		debuglog << "EventFinder::calculateDRPart MATCH " << *a.lterm << " = " << *a.rterm
			<< " values " << red_deg( lradix + r.residuum ) << ":" << l1 << ":" << l2
			<< " due " << df->formatUTDatetime( match.duedate );
#endif

			match.e1 = new MatchElement( role, new MTerm( *a.lterm ), red_deg( lradix + r.residuum ));
			match.e2 = new MatchElement( HoroscopeRole::Radix, new MTerm( *a.rterm ), lradix );
			matches.push_back( match );
			usection->matchCount++;
			break;
		}
		if ( matches.size() >= astroconfig->uranian.maxMatchCount ) return;
	}
}

/*****************************************************
**
**   EventFinder   ---   getViewpoint
**
******************************************************/
Viewpoint EventFinder::getViewpoint() const
{
	assert( props );
	return props->getEffectiveViewpoint( Viewpoint::Uranian );
}

/*****************************************************
**
**   EventFinder   ---   getMatches
**
******************************************************/
const std::list<Match> &EventFinder::getMatches() const
{
	if ( isCalculating.load())
	{
		errorlog << "EventFinder::getMatches: must wait until calculation is finished or canceled";
		return dummy_matches;
	}
	else return matches;
}

