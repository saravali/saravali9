/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Direction.cpp
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

#include "Direction.h"

#include "astroconfig.h"
#include "Calculator.h"
#include "ContextProperties.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "MDocument.h"
#include "Term.h"
#include "TextHelper.h"
#include "Viewpoint.h"

extern Astroconfig *astroconfig;

//#define DEBUG_DIRECTIONEXPERT

/*****************************************************
**
**   DirectionExpert   ---   Constructor
**
******************************************************/
DirectionExpert::DirectionExpert( ContextProps_ptr props )
	: Expert( props, Viewpoint::None  )
{
#ifdef DEBUG_DIRECTIONEXPERT
	showAllHoroscopeRoles();
#endif
}

/*****************************************************
**
**   DirectionExpert   ---   setDirectionModes
**
******************************************************/
void DirectionExpert::setDirectionModes( const std::list<HoroscopeRole>& roles )
{
	DirectionCalculator *calculator = DirectionCalculator::get();

	// Radix
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );
	assert( horoscope->getRole() == HoroscopeRole::Radix );

	for( const auto &role : roles )
	{
		assert_valid_enum<HoroscopeRole>( role );
		assert ( calculator->isDirectionRole( role ));

		Horoscope_ptr h1 = props->getHoroscope( role );
		if ( ! h1 )
		{
			h1 = Horoscope_ptr( new Horoscope( role ));
			h1->dl.location = horoscope->dl.location;
		}
		props->setHoroscope( role, h1 );
	}
	
	// cleanup old obsolete roles (if any)
	for( const auto &role : Enum<HoroscopeRole>( HoroscopeRole::FirstDirection, HoroscopeRole::LastDirection ))
	{
		if ( ! props->getHoroscope( role )) continue;

		const auto found = std::find( roles.begin(), roles.end(), role );
		if ( found == roles.end() )
		{
#ifdef DEBUG_DIRECTIONEXPERT
			debuglog << "DirectionExpert::setDirectionModes cleanup role " << role;
#endif
			props->setHoroscope( role, nullptr );
		}
	}
#ifdef DEBUG_DIRECTIONEXPERT
	showAllHoroscopeRoles();
#endif
}

/*****************************************************
**
**   DirectionExpert   ---   calculateDirections
**
******************************************************/
void DirectionExpert::calculateDirections( const double &transitJD, const HoroscopeRole &role )
{
#ifdef DEBUG_DIRECTIONEXPERT
	DateFormatter *dh = DateFormatter::get();
	debuglog << "DirectionExpert::calculateDirections direction date " << dh->formatUTDatetime( transitJD ) << " role " << role;
#endif
	assert ( DirectionCalculator::get()->isDirectionRole( role ));
	setDirectionModes( { role } );

	DirectionCalculator *calculator = DirectionCalculator::get();
	Horoscope_ptr radixHoroscope = props->getRadixHoroscope();
	assert(  radixHoroscope );
	assert(  radixHoroscope->getRole() == HoroscopeRole::Radix );

	Horoscope_ptr directionHoroscope = props->getHoroscope( role );
	assert( directionHoroscope );
	assert( radixHoroscope != directionHoroscope );

	directionHoroscope->setTransitJD( transitJD );

	calculator->calculateDirection( radixHoroscope, directionHoroscope, getViewpoint());
}

/*****************************************************
**
**   DirectionExpert   ---   calculateMultiDirections
**
******************************************************/
void DirectionExpert::calculateMultiDirections( const double &transitJD )
{
	Horoscope_ptr radixHoroscope = props->getRadixHoroscope();
	assert( radixHoroscope );
	DirectionCalculator *calculator = DirectionCalculator::get();
	const Viewpoint viewpoint = getViewpoint();

#ifdef DEBUG_DIRECTIONEXPERT
	showAllHoroscopeRoles();
#endif

	for( const auto &role : getValidDirectionRoles())
	{
		Horoscope_ptr directionHoroscope = props->getHoroscope( role );
		assert( directionHoroscope );
		directionHoroscope->setTransitJD( transitJD );
		calculator->calculateDirection( radixHoroscope, directionHoroscope, viewpoint );
	}
}

/*****************************************************
**
**   DirectionExpert   ---   describeRule
**
******************************************************/
MString DirectionExpert::describeRule( const HoroscopeRole& role, const Viewpoint &viewpoint )
{
	MString s;
	Lang lang;

	switch( role )
	{
		case HoroscopeRole::Transit:
			s << tr( "running planets" );
		break;

		case HoroscopeRole::Progressive:
			s << tr( "1year = 1day" );
		break;

		case HoroscopeRole::SolarArc:
			s << tr( "Arc" ) << " = Sun(p) - Sun(r)";
		break;

		case HoroscopeRole::ReverseSolarArc:
			s << tr( "Arc" ) << " = Sun(r) - Sun(p)";
		break;

		case HoroscopeRole::DoubleSolarArc:
			s << tr( "Arc" ) << " = 2 * ( Sun(p) - Sun(r))";
		break;

		case HoroscopeRole::HalfSolarArc:
			s << tr( "Arc" ) << " = ( Sun(p) - Sun(r)) / 2";
		break;

		case HoroscopeRole::LunarArc:
			s << tr( "Arc" ) << " = "
				<< MToken( PlanetId::Moon, viewpoint )
				<< MToken( lang.getRoleContextSubscriptum( HoroscopeRole::Progressive ), TokenFontPosition::Subscriptum )
				<< " - "
				<< MToken( PlanetId::Moon, viewpoint )
				<< MToken( lang.getRoleContextSubscriptum( HoroscopeRole::Radix ), TokenFontPosition::Subscriptum );
		break;
		case HoroscopeRole::ConstantArc:
			s << tr( "Arc" ) << " = " << tr( "number of years between transit date and birth date" );
		break;

		case HoroscopeRole::Lunations:
			s << tr( "Synodic Lunations" );
		break;
		case HoroscopeRole::Profections:
			s << tr( "Arc of Synodic Lunations" );
		break;

		default:
			s << "INVALID (" << enum_to_string( role ) << ")";
			warnlog << "invalid arc method " << role;
		break;
	}
	return s;
}

/*****************************************************
**
**   DirectionExpert   ---   writeDirectionHeader
**
******************************************************/
void DirectionExpert::writeDirectionHeader( MDocument_ptr doc )
{
	DateFormatter *dh = DateFormatter::get();
	Formatter *formatter = Formatter::get();
	const Viewpoint viewpoint = getViewpoint();

	const auto &validRoles = getValidDirectionRoles();
	if ( validRoles.size() == 0 )
	{
		errorlog << "DirectionExpert::writeDirectionHeader: no valid roles in context.";
		return;
	}
	double transitJD = 0.0;

	bool doReportProgressiveJD = false;
	bool doReportLunationJD = false;

	for( const auto &role : validRoles )
	{
		if ( ! transitJD ) // should be the same for all roles
		{
			Horoscope_ptr h = props->getHoroscope( role );
			assert( h );
			transitJD = h->getTransitJD();
		}
		switch( role )
		{
			case HoroscopeRole::Progressive:
			case HoroscopeRole::SolarArc:
			case HoroscopeRole::ReverseSolarArc:
			case HoroscopeRole::DoubleSolarArc:
			case HoroscopeRole::HalfSolarArc:
			case HoroscopeRole::LunarArc:
			case HoroscopeRole::ConstantArc:
				doReportProgressiveJD = true;
			break;

			case HoroscopeRole::Lunations:
			case HoroscopeRole::Profections:
				doReportLunationJD = true;
			break;
			default:
			break;
		}
	}
	assert( transitJD != 0.0 );
	doc->addLine( tr( "Direction date" ) + ": " + dh->formatDatetime( transitJD, props->isLocaltime ));

	if ( doReportProgressiveJD )
	{
		DirectionCalculator *calculator = DirectionCalculator::get();
		Horoscope_ptr radixHoroscope = props->getRadixHoroscope();
		assert( radixHoroscope );
		doc->addLine( tr( "Progressive date" ) + ": "
			+ dh->formatDatetime( calculator->transitToProgressiveJD( radixHoroscope->getJD(), transitJD ), props->isLocaltime ));
	}

	if ( doReportLunationJD )
	{
		DirectionCalculator *calculator = DirectionCalculator::get();
		Horoscope_ptr radixHoroscope = props->getRadixHoroscope();
		assert( radixHoroscope );
		doc->addLine( tr( "Lunation date" ) + ": "
			+ dh->formatDatetime( calculator->calculateLunationDate( radixHoroscope, viewpoint, transitJD ),
				props->isLocaltime ));
	}

	// multi direction
	if ( validRoles.size() > 1 )
	{
		doc->addLine( tr( "Rules" ));
	}

	std::map<HoroscopeRole,double> arcs;
	for( const auto &role : validRoles )
	{
		Horoscope_ptr h1 = props->getHoroscope( role );
		assert( h1 );


		{
			MString ms;
			if ( validRoles.size() > 1 )
			{
				ms << resolveCamelNotation( enum_to_string( role ));
			}
			else
			{
				ms << tr( "Rule" );
			}
			ms << ": " << describeRule( role, viewpoint );
			doc->addLine( ms );
		}

		if ( h1->getLongitudeOffset())
		{
			wxString s;
			s << tr( "Arc" ) << ": " << formatter->formatDegrees(h1->getLongitudeOffset());
			doc->addLine( s );
		}
	}
}

/*****************************************************
**
**   DirectionExpert   ---   writeDirectionReport
**
******************************************************/
void DirectionExpert::writeDirectionReport( MDocument_ptr doc )
{
	const Viewpoint viewpoint = getViewpoint();
#ifdef DEBUG_DIRECTIONEXPERT
	debuglog << "DirectionExpert::writeDirectionReport viewpoint " << viewpoint
		<< " isLocaltime " << std::boolalpha << props->isLocaltime;
#endif

	// this method makes only sense if exactly one direction horoscope is set
	const std::list<HoroscopeRole> roles = getValidDirectionRoles();
	assert( roles.size() == 1 );
	Horoscope_ptr h1 = props->getHoroscope( *roles.begin() );
	assert( h1 );

	writeDirectionHeader( doc );
	TextHelper( props, doc ).writePlanets( h1, viewpoint );
}

/*****************************************************
**
**   DirectionExpert   ---   writeMultiDirectionReport
**
******************************************************/
void DirectionExpert::writeMultiDirectionReport( MDocument_ptr doc )
{
	writeDirectionHeader( doc );
	TextHelper( props, doc ).writeMultiDirectionReport();
}

/*****************************************************
**
**   DirectionExpert   ---   getValidDirectionRoles
**
******************************************************/
std::list<HoroscopeRole> DirectionExpert::getValidDirectionRoles() const
{
  DirectionCalculator *calculator = DirectionCalculator::get();
	std::list<HoroscopeRole> roles;

	for( const auto &role : props->getActiveRoles())
	{
		if ( calculator->isDirectionRole( role ))
		{
			roles.push_back( role );
		}
	}
	return roles;
}

/*****************************************************
**
**   DirectionExpert   ---   showAllHoroscopeRoles
**
******************************************************/
void DirectionExpert::showAllHoroscopeRoles()
{
#ifdef DEBUG_DIRECTIONEXPERT
  debuglog << "DirectionExpert::showAllHoroscopeRoles START";
#endif
	for( const auto &role : Enum<HoroscopeRole>( HoroscopeRole::First, HoroscopeRole::Last ))
  {
    if ( props->getHoroscope( role ))
    {
#ifdef DEBUG_DIRECTIONEXPERT
      debuglog << "DirectionExpert::showAllHoroscopeRoles role SET: " << role;
#endif
    }
    else
    {
#ifdef DEBUG_DIRECTIONEXPERT
      debuglog << "DirectionExpert::showAllHoroscopeRoles role NULL: " << role;
#endif
    }
  }
#ifdef DEBUG_DIRECTIONEXPERT
  debuglog << "DirectionExpert::showAllHoroscopeRoles FINISHED";
#endif
}

/*****************************************************
**
**   DirectionCalculator   ---   Constructor
**
******************************************************/
DirectionCalculator::DirectionCalculator()
	: yearLength( astroconfig->directions.yearLength.getValue())
{
	assert( yearLength );
}

/*****************************************************
**
**   DirectionCalculator   ---   isDirectionRole
**
******************************************************/
bool DirectionCalculator::isDirectionRole( const HoroscopeRole &role )
{
	return ( role >= HoroscopeRole::FirstDirection && role <= HoroscopeRole::LastDirection );
}

/*****************************************************
**
**   DirectionCalculator   ---   isShiftedRole
**
******************************************************/
bool DirectionCalculator::isShiftedRole( const HoroscopeRole &role )
{
	return ( role >= HoroscopeRole::SolarArc && role <= HoroscopeRole::Lunations );
}

/*****************************************************
**
**   DirectionCalculator   ---   transitToProgressiveJD
**
******************************************************/
double DirectionCalculator::transitToProgressiveJD( const double &birthJD, const double &transitJD )
{
	return birthJD + ( transitJD - birthJD ) / yearLength;
}

/*****************************************************
**
**   DirectionCalculator   ---   transitToDirectionJD
**
******************************************************/
double DirectionCalculator::transitToDirectionJD( const Horoscope_ptr radixHoroscope, const Viewpoint &viewpoint,
	const double &transitJD, const HoroscopeRole &directionRole )
{
	switch( directionRole )
	{
		case HoroscopeRole::Transit:
			return transitJD;
		break;
		case HoroscopeRole::Lunations:
			return calculateLunationDate( radixHoroscope, viewpoint, transitJD );
		break;
		default:
			return transitToProgressiveJD( radixHoroscope->getJD(), transitJD );
		break;
	}
}

/*****************************************************
**
**   DirectionCalculator   ---   progressiveToTransitJD
**
******************************************************/
double DirectionCalculator::progressiveToTransitJD( const double &birthJD, const double &progressiveJD )
{
	return birthJD + yearLength * ( progressiveJD - birthJD );
}

/*****************************************************
**
**   DirectionCalculator   ---   calcArcSpeed
**
******************************************************/
std::pair<double, double> DirectionCalculator::calcArcSpeed( Horoscope_ptr radixHoroscope, const Viewpoint &viewpoint,
	const double &transitJD, const double &dirJD, const HoroscopeRole &directionRole )
{
	// return value with arc and arc speed
	std::pair<double, double> r( 0, 0 );
	constexpr double synodic_period = 29.53;

	Calculator *calculator = CalculatorFactory().getCalculator();
	double speed = 1.0;
	double lat = 0.0;
	double longitude = 0.0;

	DateLocation ds = radixHoroscope->dl;
	ds.date.jd = dirJD;

	switch( directionRole )
	{
		case HoroscopeRole::SolarArc: 
			calculator->calcPositionSpeed( ds, PlanetId::Sun, longitude, lat, speed, viewpoint );
			r.first = red_deg( longitude - radixHoroscope->getLongitude( PlanetId::Sun, viewpoint ));
			r.second = speed / yearLength;
		break;

		case HoroscopeRole::ReverseSolarArc:
			calculator->calcPositionSpeed( ds, PlanetId::Sun, longitude, lat, speed, viewpoint );
			r.first = red_deg( radixHoroscope->getLongitude( PlanetId::Sun, viewpoint ) - longitude );
			r.second = -speed / yearLength;
		break;

		case HoroscopeRole::DoubleSolarArc: 
			calculator->calcPositionSpeed( ds, PlanetId::Sun, longitude, lat, speed, viewpoint );
			r.first = red_deg( 2 * ( longitude - radixHoroscope->getLongitude( PlanetId::Sun, viewpoint )));
			r.second = 2 * speed / yearLength;
		break;

		case HoroscopeRole::HalfSolarArc: 
			calculator->calcPositionSpeed( ds, PlanetId::Sun, longitude, lat, speed, viewpoint );
			r.first = red_deg( .5 * ( longitude - radixHoroscope->getLongitude( PlanetId::Sun, viewpoint )));
			r.second = .5 * speed / yearLength;
		break;

		case HoroscopeRole::Profections: 
		{
			// könnte ok sein
			calculator->calcPositionSpeed( ds, PlanetId::Sun, longitude, lat, speed, viewpoint );
			r.first = red_deg( longitude - radixHoroscope->getLongitude( PlanetId::Sun, viewpoint ));
#ifdef DEBUG_DIRECTIONEXPERT
			debuglog << " DirectionCalculator::calcArcSpeed profection arc " << r.first;
#endif
			r.second = speed * synodic_period / yearLength;
		}
		break;

		case HoroscopeRole::LunarArc:
			calculator->calcPositionSpeed( ds, PlanetId::Moon, longitude, lat, speed, viewpoint );
			r.first = red_deg( longitude - radixHoroscope->getLongitude( PlanetId::Moon, viewpoint ));

			// TODO falsche konstante
			r.second = speed / yearLength;
		break;
		case HoroscopeRole::ConstantArc:
			r.first = red_deg( dirJD - radixHoroscope->getJD());

			// TODO
			r.second = 1.0 / yearLength;
		break;
		default:
			warnlog << "invalid arc method " << directionRole;
		break;
	}
	assert( r.second );

#ifdef DEBUG_DIRECTIONEXPERT
	debuglog << "DirectionCalculator::calcArcSpeed role " << directionRole
		<< " longitude " << longitude
		<< " speed " << r.second
		<< " arc " << r.first;
#endif

	return r;
}

/*****************************************************
**
**   DirectionCalculator   ---   calculateDirection
**
******************************************************/
double DirectionCalculator::calculateDirection( Horoscope_ptr radixHoroscope,
	Horoscope_ptr directionHoroscope, const Viewpoint &viewpoint )
{
	assert ( radixHoroscope );
	assert ( directionHoroscope );
	const HoroscopeRole directionRole = directionHoroscope->getRole();
	assert ( isDirectionRole( directionRole ));

	const double transitJD = directionHoroscope->getTransitJD();
	assert( transitJD );

#ifdef DEBUG_DIRECTIONEXPERT
	DateFormatter *formatter = DateFormatter::get();
	debuglog << "DirectionCalculator::calculateDirection direction role " << directionRole
		<< " direction date " << formatter->formatUTDatetime( transitJD );
#endif

	double arcJD = 0.0;

	// pre update
	switch( directionRole )
	{
		case HoroscopeRole::Progressive:
			directionHoroscope->setDate( transitToProgressiveJD( radixHoroscope->getJD(), transitJD ));
		break;

		case HoroscopeRole::SolarArc:
		case HoroscopeRole::ReverseSolarArc:
		case HoroscopeRole::DoubleSolarArc:
		case HoroscopeRole::HalfSolarArc:
		case HoroscopeRole::LunarArc:
		case HoroscopeRole::ConstantArc:
			arcJD = transitToProgressiveJD( radixHoroscope->getJD(), transitJD );
			directionHoroscope->setDate( radixHoroscope->getJD());
		break;

		case HoroscopeRole::Lunations:
			directionHoroscope->setDate( calculateLunationDate( radixHoroscope, viewpoint, transitJD ));
		break;
		case HoroscopeRole::Profections:
			arcJD = calculateLunationDate( radixHoroscope, viewpoint, transitJD );
			directionHoroscope->setDate( radixHoroscope->getJD());
		break;

		case HoroscopeRole::Transit:
		default:
			directionHoroscope->setDate( transitJD );
		break;
	}
	directionHoroscope->update();
#ifdef DEBUG_DIRECTIONEXPERT
		debuglog << "DirectionCalculator::calculateDirection direction horoscope updated with direction date "
			<< formatter->formatUTDatetime( directionHoroscope->getJD() );
#endif

	// post update
	//if ( isShiftedRole( directionRole ))
	if ( arcJD )
	{
		const std::pair<double, double> r = calcArcSpeed( radixHoroscope, viewpoint, transitJD, arcJD, directionRole );

#ifdef DEBUG_DIRECTIONEXPERT
		debuglog << "DirectionCalculator::calculateDirection arc " << r.first << " speed " << r.second;
#endif
		directionHoroscope->setLongitudeOffset( r.first );
		directionHoroscope->setUniformMovingDirection( MovingDirection::Direct );

		return r.second;
	}

	return 0.0;
}

/*****************************************************
**
**   DirectionCalculator   ---   getSpeedMultiplicator
**
******************************************************/
double DirectionCalculator::getSpeedMultiplicator( const HoroscopeRole &role ) const
{
	// TODO not complete
	switch( role )
	{
		case HoroscopeRole::Transit:
			return 1.0;
		break;

		case HoroscopeRole::SolarArc:
		case HoroscopeRole::Progressive:
			return yearLength;
		break;

		case HoroscopeRole::Lunations:
		case HoroscopeRole::Profections:
			return synodic_period;
		break;

		case HoroscopeRole::ReverseSolarArc:
			return -yearLength;
		break;
		case HoroscopeRole::DoubleSolarArc:
			return 2 * yearLength;
		break;
		case HoroscopeRole::HalfSolarArc:
			return .5 * yearLength;
		break;

		case HoroscopeRole::LunarArc:
		case HoroscopeRole::ConstantArc:

		default:
		assert( false );
		break;
	}
}

/*****************************************************
**
**   DirectionCalculator   ---   calculateLunationDate
**
******************************************************/
double DirectionCalculator::calculateLunationDate( Horoscope_ptr horoscope, const Viewpoint &viewpoint,
	const double &transitJD )
{
	constexpr double synodic_period = 29.53;
	double slen = 0;
	double mlen = 0;

	assert(  horoscope );
	Calculator *calculator = CalculatorFactory().getCalculator();

	// number of years after birth
	const double years = ( transitJD - horoscope->getJD()) / yearLength;

	// angle between moon and sun at birth
	const double radix_arc = red_deg( horoscope->getLongitude( PlanetId::Moon, viewpoint )
		- horoscope->getLongitude( PlanetId::Sun, viewpoint ));

	// angle between moon and sun at lunation
	const double target_moon_arc = red_deg( radix_arc + years * 360 );

	// approximate date of lunation = birth plus one year per synodic period
	const double approx_synodic_date = horoscope->getJD() + years * synodic_period;

	DateLocation ds = horoscope->dl;
	ds.date.jd = approx_synodic_date;

	const double exact_synodic_date = calculator->calcSunMoonEvent( ds, target_moon_arc, slen, mlen );

#ifdef DEBUG_DIRECTIONEXPERT
			DateFormatter *df = DateFormatter::get();

			debuglog << "DirectionCalculator::calculateLunations years " << years
				<< " radix_arc " << radix_arc
				<< " target moon arc " << target_moon_arc
				<< " approximate synodic date " << df->formatUTDatetime( approx_synodic_date )
				<< " exact synodic date " << df->formatUTDatetime( exact_synodic_date )
				<< " sun pos " << slen
				<< " moon pos " << mlen;
#endif

	return exact_synodic_date;
}



