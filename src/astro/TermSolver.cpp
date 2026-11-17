/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/TermSolver.cpp
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

#include "TermSolver.h"

#include "astroconfig.h"
#include "Calculator.h"
#include "DateFormatter.h"
#include "Direction.h"
#include "enumserializer.h"
#include "Horoscope.h"
#include "logger.h"
#include "Term.h"

#include <math.h>

extern Astroconfig *astroconfig;

#define DEBUG_TERMSOLVER

/*****************************************************
**
**   TermSolver   ---   findRadixMatchDate
**
******************************************************/
double TermSolver::findRadixMatchDate( MTerm *term, const double &startjd, const double &targetlen )
{
#ifdef DEBUG_TERMSOLVER
	// debug output
#endif
	assert( term );
	assert( term->isIncarnated() );

	DateFormatter *formatter = DateFormatter::get();
	double diff = 1;
	double jd = startjd;

	Horoscope_ptr h2( new Horoscope());
	h2->dl.location = horoscope->dl.location;
	h2->setDate( jd );
	h2->update();

	// current loop
	int loop = 0;

	std::pair<double, double> r = term->evaluateSpeed( h2, viewpoint );
	
#ifdef DEBUG_TERMSOLVER
	debuglog << "FIND MATCHING DATE targetlen " << targetlen
		<< " termId " << (int)term->getTermId()
		<< " longitude " << r.first
		<< " orig date " << formatter->formatUTDatetime( startjd )
		<< " direction date " << formatter->formatUTDatetime( jd );
#endif

	diff = r.first - targetlen;
	if ( diff > 200 ) diff -= 360;
	if ( diff < -200 ) diff += 360;

	while( fabs( diff ) > targetdiff && loop < max_loop )
	{
		r = term->evaluateSpeed( h2, viewpoint );

#ifdef DEBUG_TERMSOLVER
		debuglog << " - loop " << loop
			<< " date " << formatter->formatUTDatetime(jd)
			<< " diff " << diff
			<< " longitude " << r.first
			<< " speed " <<  r.second;
#endif

		if ( r.second == 0 )
		{
			warnlog << "speed is zero for targetlen " << targetlen << " term " <<  *term
				<< " termlen " << r.first << " date " << formatter->formatUTDatetime( jd );
			r.second = 1;
		}
		diff = r.first - targetlen;
		if ( diff > 200 ) diff -= 360;
		if ( diff < -200 ) diff += 360;
		jd -= diff / r.second;

		h2->setDate( jd );
		h2->update();

#ifdef DEBUG_TERMSOLVER
		debuglog << " - ende loop " << loop << " diff " << diff << " diff/speed " << diff/r.second
			<< " date " << formatter->formatUTDatetime( jd );
#endif
		loop++;
	}
	if ( loop == max_loop )
	{
		warnlog << "TermSolver::findRadixMatchDate: Too many loops for term " << *term;
		return 0;
	}
	return jd;
}

/*****************************************************
**
**   TermSolver   ---   findRadixMatchDate
**
******************************************************/
double TermSolver::findRadixMatchDate( MTerm *t1, MTerm *t2, const double &startjd, const double &residuum )
{
	assert( t1 );
	assert( t1->isIncarnated() );
	assert( t2 );
	assert( t2->isIncarnated() );

	Horoscope_ptr h2( new Horoscope());
	h2->dl.location = horoscope->dl.location;

#ifdef DEBUG_TERMSOLVER
	debuglog << "TermSolver::findRadixMatchDate t1 " << *t1 << " t2 " << t2 << " residuum " << residuum;
#endif

	assert( t1 );
	DateFormatter *formatter = DateFormatter::get();
	double diff = 1;
	int loop = 0;
	std::pair<double, double> r1 = std::make_pair( 0, 0 );
	std::pair<double, double> r2 = std::make_pair( 0, 0 );

	double jd = startjd;

	while( fabs( diff ) > targetdiff && loop < max_loop )
	{
		h2->setDate( jd );
		h2->update();

		r1 = t1->evaluateSpeed( h2, viewpoint );
		if ( r1.second == 0 )
		{
			warnlog << "Term 1 speed is zero for residuum " << residuum << " term " <<  *t1
				<< " termlen " << r1.first << " date " << formatter->formatUTDatetime(jd);
			r1.second = 1;
		}
		if ( t2 )
		{
			r2 = t2->evaluateSpeed( h2, viewpoint );
			if ( r2.second == 0 )
			{
				warnlog << "Term2 speed is zero for residuum " << residuum << " term " <<  *t2
					<< " termlen " << r2.first << " date " << formatter->formatUTDatetime(jd);
				r2.second = 1;
			}
		}

		diff = r1.first - r2.first + residuum;
		if ( diff > 200 ) diff -= 360;
		if ( diff < -200 ) diff += 360;
		jd -= diff / ( r1.second - r2.second );

#ifdef DEBUG_TERMSOLVER
		debuglog << "TermSolver::findRadixMatchDate - loop " << loop
			<< " date " << formatter->formatUTDatetime(jd)
			<< " diff " << diff
			<< " longitude1 " << r1.first
			<< " speed1 " <<  r1.second
			<< " longitude2 " << r2.first
			<< " speed2 " <<  r2.second;
#endif

		++loop;
	}

#ifdef DEBUG_TERMSOLVER
	debuglog << " Finished TermSolver::findRadixMatchDate with " << loop << " loops and diff " << diff;
#endif
	return jd;
}

/*****************************************************
**
**  TermSolver   ---   findDeclinationMatchDate
**
******************************************************/
double TermSolver::findDeclinationMatchDate( const PlanetId &ob, const double &startjd, const double &targetd )
{
#ifdef DEBUG_TERMSOLVER
	debuglog << "TermSolver::findDeclinationMatchDate PlanetId " << (int)ob << " startJD " << startjd
		<< " target declination " << targetd;
#endif
	Calculator *calculator = CalculatorFactory().getCalculator();
	DateLocation dl;
	dl.location = astroconfig->standardLocation;
	PlanetPosition pos( 0 );
	DateFormatter *formatter = DateFormatter::get();

	double jd = startjd;
	double diff = 1;

	// current loop
	int loop = 0;

	dl.date.jd = startjd;

	while( fabs( diff ) > targetdiff && loop < max_loop )
	{
		pos = calculator->calcPositionSpeed( dl, ob, viewpoint );

#ifdef DEBUG_TERMSOLVER
		debuglog << " - loop " << loop
			<< " date " << formatter->formatUTDatetime(jd)
			<< " diff " << diff
			<< " declination " << pos.declination
			<< " speed " <<  pos.dspeed;
#endif

		if ( pos.dspeed == 0 )
		{
			warnlog << "speed is zero for targetd " << targetd << " PlanetId " << (int)ob
				<< " decl " << pos.declination << " date " << formatter->formatUTDatetime(jd);
			pos.dspeed = 1;
		}
		diff = pos.declination - targetd;
		jd -= diff / pos.dspeed;

		dl.date.jd = jd;

#ifdef DEBUG_TERMSOLVER
		debuglog << " - ende loop " << loop << " diff " << diff << " diff/speed " << diff/pos.dspeed
			<< " date " << formatter->formatUTDatetime(jd);
#endif
		loop++;
	}
	if ( loop == max_loop )
	{
		warnlog << "TermSolver::findDeclinationMatchDate: Too many loops for PlanetId " << (int)ob;
		return 0;
	}
	return jd;
}

/*****************************************************
**
**   TermSolver   ---   findInterProgressionMatchDate
**
******************************************************/
double TermSolver::findInterProgressionMatchDate( MTerm *term1, MTerm *term2, const double &startJD, const double &residuum )
{
	DirectionCalculator *calculator = DirectionCalculator::get();
	assert( horoscope );
	assert( term1 );
	assert( term2 );

	const double dirJD = calculator->transitToProgressiveJD( horoscope->getJD(), startJD );
	const double dirtargetJD = findRadixMatchDate( term1, term2, dirJD, residuum );
	return calculator->progressiveToTransitJD( horoscope->getJD(), dirtargetJD );
}

/*****************************************************
**
**   TermSolver   ---   findDirectionMatchDate
**
******************************************************/
double TermSolver::findDirectionMatchDate( MTerm *term, const HoroscopeRole &directionRole,
	const double &startjd, const double &ltarget )
{
	assert( term );
	assert( term->isIncarnated() );
	assert( startjd );

	DirectionCalculator *calculator = DirectionCalculator::get();
	DateFormatter *formatter = DateFormatter::get();
	constexpr double synodic_period = 29.53;

	double diff = 1;
	double jd = startjd;
	double speed = 0.0;
	double longitude = 0.0;

#ifdef DEBUG_TERMSOLVER
	debuglog << "TermSolver::findDirectionMatchDate term " << *term
		<< " role " << directionRole
		<< " startjd " << formatter->formatUTDatetime( startjd ) 
		<< " target longitude " << ltarget;
#endif

	Horoscope_ptr h1( new Horoscope( directionRole ));
	h1->dl.location = horoscope->dl.location;
	h1->setDate( jd );
	h1->update();

	// current loop number
	int loop = 0;

	longitude = term->evaluate( h1, viewpoint );
	diff = longitude - ltarget;
	if ( diff > 200 ) diff -= 360;
	if ( diff < -200 ) diff += 360;

#ifdef DEBUG_TERMSOLVER
	debuglog << "TermSolver::findDirectionMatchDate longitude " << longitude << " diff " << diff;
#endif

	while( fabs( diff ) > targetdiff && loop < max_loop )
	{
		h1->setTransitJD( jd );
		speed = calculator->calculateDirection( horoscope, h1, viewpoint );
		if ( speed )
		{
			longitude = term->evaluate( h1, viewpoint );
		}
		else
		{
			std::tie( longitude, speed ) = term->evaluateSpeed( h1, viewpoint );
		}
		if ( speed == 0 )
		{
			warnlog << "speed is zero for target lontigude " << ltarget << " term " <<  *term
				<< " longitude " << longitude << " date " << formatter->formatUTDatetime( jd );
			speed = 1;
		}
		if ( term->isAntiTerm())
		{
			speed = -speed;
		}

		diff = longitude - ltarget;
		if ( diff > 200 ) diff -= 360;
		if ( diff < -200 ) diff += 360;

		switch( directionRole )
		{
			case HoroscopeRole::Progressive:
				jd -= astroconfig->directions.yearLength.getValue() * diff / speed;
			break;

			case HoroscopeRole::Lunations:
				jd -= astroconfig->directions.yearLength.getValue() * diff / ( speed * synodic_period);
			break;

			default:
				jd -= diff / speed;
			break;
		}

#ifdef DEBUG_TERMSOLVER
		debuglog << "TermSolver::findDirectionMatchDate - loop " << loop
			<< " date " << formatter->formatUTDatetime( jd )
			<< " diff " << diff
			<< " longitude " << longitude
			<< " speed " <<  speed;
#endif

		loop++;
	}
	if ( loop == max_loop )
	{
		warnlog << "TermSolver::findDirectionMatchDate: Too many loops for term " << *term;
		return 0;
	}
	return jd;
}

/*****************************************************
**
**   TermSolver   ---   findJDForArc
**
******************************************************/
double TermSolver::findJDForArc( const HoroscopeRole &role, const double &startJD, const double &targetArc )
{
	DirectionCalculator *calculator = DirectionCalculator::get();
	assert( calculator->isShiftedRole( role ));
	MTerm term( PlanetId::Aries );
	return findDirectionMatchDate( &term, role, startJD, targetArc );
}

/*****************************************************
**
**   TermSolver   ---   fitTermPathToBoundaries
**
******************************************************/
void TermSolver::fitTermPathToBoundaries( TermPath &path, const int &divisor )
{
	assert( divisor );
	const double gk_total = 360.0 / divisor;

	std::list<TermStep> wrapsteps;

	std::list<TermStep>::iterator iter = path.steps.begin();
	if ( iter == path.steps.end()) return;
	std::list<TermStep>::iterator previous = iter;

	while( ++iter != path.steps.end())
	{
		// wrap is either at section change in divisional zodiac or step over 0 Aries
		const int step0 = (int)( previous->pos.longitude / gk_total );
		const int step1 = (int)( iter->pos.longitude / gk_total );
		bool isWrap = false;
		if (  step0 != step1
			|| ( iter->pos.lspeed > 0 && iter->pos.longitude < previous->pos.longitude )
			|| ( iter->pos.lspeed < 0 && iter->pos.longitude > previous->pos.longitude ))
		{
			isWrap = true;
#ifdef DEBUG_TERMSOLVER
			debuglog << "WRAP object " << path.term << " at " << step1 * gk_total;
#endif
		}

#ifdef DEBUG_TERMSOLVER
		debuglog << "TermSolver::fitTermPathToBoundaries Object " << path.term
			<< " l1 " << previous->pos.longitude << " speed " << previous->pos.lspeed << " step " << step0
			<< " l2 " << iter->pos.longitude << " speed " << iter->pos.lspeed << " step " << step1
			<< " WRAP " << isWrap;
#endif
		if ( isWrap )
		{
#ifdef DEBUG_TERMSOLVER
			debuglog << "TermSolver::fitTermPathToBoundaries WRAP Object " << path.term
				<< " " << step0 << " " << step1;
#endif
			const double jdm = .5 * ( previous->jd + iter->jd );
			horoscope->setDate( jdm );
			const double targetlen = step1 * gk_total;

			const double targetjd = findRadixMatchDate( &path.term, jdm, targetlen );
			if ( targetjd > iter->jd )
			{
				const double errorlen = targetjd - iter->jd;
				errorlog << "WRAP Object longitude too big " << path.term << " has error " << errorlen;
			}
			else if ( targetjd < previous->jd )
			{
				const double errorlen = previous->jd - targetjd;
				errorlog << "WRAP Object longitude too small " << path.term << " has error " << errorlen;
			}
			wrapsteps.push_back( TermStep( targetjd, TermStepType::Wrap, PlanetPosition( targetlen )));
#ifdef DEBUG_TERMSOLVER
			debuglog << "WRAP Object " << path.term << " at " << targetjd << " longitude " << targetlen;
#endif
		}
		previous = iter;
	}

	// insert wrap steps
	if ( wrapsteps.size() > 0 )
	{
		for( auto &step : wrapsteps )
		{
			path.steps.push_back( step );
		}
		path.sort();
	}
}

/*****************************************************
**
**   TermSolver   ---   insertDeclinationZeros
**
******************************************************/
void TermSolver::insertDeclinationZeros( TermPath &path )
{
#ifdef DEBUG_TERMSOLVER
	debuglog << "TermSolver::insertDeclinationZeros Term " << path.term << " with " << path.steps.size() << " steps";
#endif
	std::list<TermStep> wrapsteps;

	assert( path.term.isSingleTerm());
	const PlanetId ob = path.term.getFirstPlanet();

	std::list<TermStep>::iterator iter = path.steps.begin();
	if ( iter == path.steps.end()) return;
	std::list<TermStep>::iterator previous = iter;

	while( ++iter != path.steps.end())
	{
		if ( iter->pos.declination * previous->pos.declination < 0 )
		{
#ifdef DEBUG_TERMSOLVER
			debuglog << "TermSolver::insertDeclinationZeros WRAP step len " << ( iter->jd - previous->jd );
#endif
			const double jdm = .5 * ( previous->jd + iter->jd );
			const double targetjd = findDeclinationMatchDate( ob, jdm, 0 );

#ifdef DEBUG_TERMSOLVER
			const double portion = ( targetjd - previous->jd )  / ( iter->jd - previous->jd );
			debuglog << "TermSolver::insertDeclinationZeros WRAP with portion " << portion;
#endif
			wrapsteps.push_back( TermStep( targetjd, TermStepType::Zero, PlanetPosition( 0 )));
		}
		previous = iter;
	}

	// insert wrap steps
	if ( wrapsteps.size() > 0 )
	{
#ifdef DEBUG_TERMSOLVER
		debuglog << "TermSolver::insertDeclinationZeros inserting " << wrapsteps.size() << " new steps";
#endif
		for( auto &step : wrapsteps )
		{
			path.steps.push_back( step );
		}
		path.sort();
	}
}

