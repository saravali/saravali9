/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Ephemeris.cpp
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

#include "Ephemeris.h"

#include "astrobase.h"
#include "astroconfig.h"
#include "Calculator.h"
#include "ContextProperties.h"
#include "DasaTool.h"
#include "datelocation.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "gdiconfig.h"
#include "Lang.h"
#include "mathbase.h"
#include "MDocument.h"
#include "Table.h"
#include "Term.h"
#include "UranianHelper.h"

#include <algorithm>
#include <format>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   EphemExpert   ---   Constructor
**
******************************************************/
EphemExpert::EphemExpert( ContextProps_ptr props )
	: Expert( props, Viewpoint::None )
{
	dl.location = astroconfig->standardLocation;
	show_header = true;
	init();
}

/*****************************************************
**
**   EphemExpert   ---   init
**
******************************************************/
void EphemExpert::init()
{
	calc_coordinates = calc_ingress = calc_kp = calc_sunrise = calc_lunar = false;
	mydasa = DasaId::None;
	dstchange = false;
	in_current_month = false;
	start_jd = end_jd = 0;

	// reset variables
	for ( int i = 0; i < MAX_DAY; i++ )
	{
		weekday[i] = tithi[i] = 0;
		jd[i] = st[i] = sunrise[i] = sunset[i] = sunrise_angle[i]
			= sunrise_sunlen[i] = sunrise_moonlen[i] = 0;
	}
	data.clear();
}

/*****************************************************
**
**   EphemExpert   ---   prepareMonth
**
******************************************************/
void EphemExpert::prepareMonth( const int &y, const int &m )
{
	debuglog << "EphemExpert::prepareMonth year " << y << " month " << m << " isLocaltime " << props->isLocaltime;
	DateFormatter *df = DateFormatter::get();
	const Viewpoint viewpoint = getViewpoint();

	year = y;
	month = m;

	obs = props->getPlanets( viewpoint,
	{
	 FilterGroup::Angles,
	 FilterGroup::CardinalPoints,
	 FilterGroup::Upagrahas,
	 FilterGroup::Kalavelas,
	 FilterGroup::ArabicParts,
	 FilterGroup::Houses,
	});

	assert( month >= 0 && month <= 12 );
	assert( year >= -2000 && year <= 4000 );

	init();

	nb_days = getMonthLength( year, month );
	assert( nb_days > 27 && nb_days < MAX_DAY );

	std::tie( current_year, current_month, current_day ) = df->calculateDateIntegers( MDate::getCurrentJD());
	in_current_month = ( month == current_month && year == current_year );
	const double tzoffset = astroconfig->standardLocation.tz;
	double dstvalue = 0; 
	double lastdst = 0;

	dl.date.set( year, month, 1, 0.0 );
	start_jd = dl.date.jd;
	dl.date.addOneMonth();
	end_jd = dl.date.jd;

	// set jds and week days
	for ( int i = 0; i <= nb_days; i++ ) // calc JDs for this month and year
	{
		dl.date.set( year, month, i+1, 0 );
		jd[i] = dl.date.jd;
		weekday[i] = ((int)(jd[i] + 2 )) % 7;
		debuglog << "Ephem day " << (i+1) << " " << df->formatUTDatetime( jd[i] );

		if ( props->isLocaltime )
		{
			dstvalue = df->getConfiguredDstOffset( jd[i] );
			jd[i] -= ( tzoffset + dstvalue ) / 24.0;

			// test for dst change during month
			if ( i > 0 && lastdst != dstvalue )
			{
				dstchange = true;
			}
			lastdst = dstvalue;
			debuglog << "Ephem day " << (i+1) << " tzdelta " << tzoffset
				<< " dst " << dstvalue << std::boolalpha << " dst change " << dstchange;
		}
	}
}

/*****************************************************
**
**   EphemExpert   ---   calculateCoordinates
**
******************************************************/
int EphemExpert::calculateCoordinates()
{
	debuglog << "Start EphemExpert::calculateCoordinates";
	StopWatch sw;
	DateFormatter *formatter = DateFormatter::get();
	int ret = 0;
	Calculator *calculator = CalculatorFactory().getCalculator();
	const Viewpoint viewpoint = getViewpoint();

	data.clear();
	for ( const PlanetId p : obs )
	{
		data[p] = std::vector<PlanetPosition>();

		for ( int i = 0; i <= nb_days; i++ ) // calc longitude, declination and retrogression
		{
			dl.date.jd = jd[i];
			debuglog << "EphemExpert::calculateCoordinates " << (i+1) << " " << formatter->formatUTDatetime( jd[i] );
			data[p].push_back( calculator->calcPositionSpeed( dl, p, viewpoint ));
		}

		// count objects that could not be calculated (missing data files)
		if ( data[p][0].longitude == 0 ) ret++;
	}
	calc_coordinates = true;
	debuglog << "Finished EphemExpert::calculateCoordinates in " << sw.getElapsedMillisec() << " millisec "
		<< " data size " << data.size();
	return ret;
}

/*****************************************************
**
**   EphemExpert   ---   initTermPaths
**
******************************************************/
void EphemExpert::initTermPaths( std::list<TermPath> &paths )
{
	if ( ! calc_coordinates ) calculateCoordinates();

	for( const PlanetId &p : obs )
	{
		TermPath path( p );
		for ( int i = 0; i <= nb_days; i++ )
		{
			 path.steps.push_back( TermStep( jd[i],
			 	( data[p][i].lspeed < 0 ? TermStepType::Backward : TermStepType::Forward ),
				 data[p][i] ));
		}
		paths.push_back( path );
	}
}

/*****************************************************
**
**   EphemExpert   ---   getPosFormatted
**
******************************************************/
MString EphemExpert::getPosFormatted( const double &longitude, const MovingDirection dir,
	const DegPrecision precision, const int division )
{
	const Viewpoint viewpoint = getViewpoint();
	PlanetPropertyConfig cfg( astroconfig->getViewpointConfig( viewpoint ).planetProps );
	cfg.precision = precision;

	// Shorten sign names if necessary
	if ( cfg.textFormat == TextFormat::Medium || cfg.textFormat == TextFormat::Long )
	{
		cfg.textFormat = TextFormat::Short;
	}

	return Formatter::get()->formatZodiacalLongitude( cfg, longitude, dir, division );
}

/*****************************************************
**
**   EphemExpert   ---   calculateKP
**
******************************************************/
void EphemExpert::calculateKP( const DasaId &dasaId, DasaExpert_ptr expert )
{
	if ( ! expert->hasKpFeatures()) return;

	StopWatch sw;
	const Viewpoint viewpoint = getViewpoint();
	Calculator *calculator = CalculatorFactory().getCalculator();

	if ( ! calc_coordinates  ) calculateCoordinates();

	// calculate all lord/sublord events
	kpevents = expert->getKPEventList( data[PlanetId::Moon][0].longitude,
		data[PlanetId::Moon][nb_days].longitude, jd[0] );

	for ( KPEvent &e : kpevents )
	{
		dl.date.jd = e.jd;
		e.jd = calculator->calcPlanetaryEvent( dl, e.len, PlanetId::Moon, viewpoint );
	}
	calc_kp = true;
	mydasa = dasaId;
	debuglog << "EphemExpert::calcKP in " <<  sw.getElapsedMillisec() << " millisec";
}

/*****************************************************
**
**   EphemExpert   ---   writeKp
**
******************************************************/
void EphemExpert::writeKp( MDocument_ptr doc, const DasaId &dasaId )
{
	DasaLord lord;
	DateFormatter *dh = DateFormatter::get();
	DasaTool *tool = DasaTool::get();
	DasaExpert_ptr expert( DasaExpertFactory().getDasaExpert( dasaId ));
	StopWatch sw;
	const DegPrecision precision = astroconfig->getViewpointConfig( getViewpoint() ).planetProps.precision;

	writeHeaderInfo( doc );

	if ( ! expert->hasKpFeatures() )
	{
		doc->addLine( std::format( "K.P. events not supported for {} dasa", tool->getDasaName( dasaId )));
		return;
	}

	if ( ! calc_kp || mydasa != dasaId ) calculateKP( dasaId, expert );

	Table *table = nullptr;
	const std::list<wxString> thead =
	{
		tr( "Day" ),
		tr( "Time" ),
		tr( "Lord" ),
		tr( "Sublord" ),
		tr( "Lunar Position" )
	};

	int index = -1;
	for ( const KPEvent &event : kpevents )
	{
		// test if index has changed
		if ( event.dasaindex != index )
		{
			index = event.dasaindex;
			if ( table ) doc->addItem( table );
			table = new Table;
			table->addHeaderRow( thead );
		}
		assert( table );

		table->addRow(
		{
			MString( writeDayShortcut( event.jd ).first ),
			MString( dh->formatTime( event.jd, props->isLocaltime )),
			expert->getDasaLordName( event.lord ),
			expert->getDasaLordName( event.sublord ),
			getPosFormatted( event.len, MovingDirection::None, precision )
		});
		lord = event.lord;
		if ( table ) table->setTitle( expert->getDasaLordName( lord ));
	}
	if ( table ) doc->addItem( table );

	infolog << "FINISHED EphemExpert::writeKP " << sw.getElapsedMillisec() << " millisec";
}

/*****************************************************
**
**   EphemExpert   ---   calculateIngress
**
******************************************************/
int EphemExpert::calculateIngress()
{
	int ret = 0;
	double l1, l2;
	bool retro;
	if ( ! calc_coordinates ) ret = calculateCoordinates();

	const Viewpoint viewpoint = getViewpoint();
	ingressEvents.clear();

	// Achtung: das geht nicht immer für den Mond, weil der mehr als ein Event am Tag haben kann
	for ( int i = 0; i < nb_days; i++ )
	{
		dl.date.set( year, month, i+1, 12 );
		for ( const PlanetId &p : obs )
		{
			infolog << "EphemExpert::calculateIngress p " << (int)p;

			l1 = data[p][i].longitude;
			l2 = data[p][i+1].longitude;
			retro = data[p][i].lspeed < 0;

			if( getRasi( l1 ) != getRasi( l2 ))
			{
				testIngressEvent( (int)getRasi( l1 ), (int)getRasi( l2 ), p, 0, retro );
			}

			if ( viewpoint == Viewpoint::Vedic )
			{
				// Nakshatra changes
				if ( ::getNakshatra27( l1 ) != ::getNakshatra27( l2 ))
				{
					const int base_nak = (int)::getNakshatra27( l1 );
					const int diff_nak = red27( (int)::getNakshatra27( l2 )- (int)base_nak );
					switch( diff_nak )
					{
						case 1:
							testIngressEvent( base_nak, red27( base_nak + 1 ), p, 1, retro );
						break;
						case 2:
							testIngressEvent( base_nak, red27( base_nak + 1 ), p, 1, retro );
							testIngressEvent( red27( base_nak + 1 ), red27( base_nak + 2 ), p, 1, retro );
						break;
						default:
						break;
					}
				}
			}
		}
	}
	ingressEvents.sort( [] ( const auto &e1, const auto &e2 ) { return( e1.jd < e2.jd ); });

	calc_ingress = true;
	return ret;
}

/*****************************************************
**
**   EphemExpert   ---   testIngressEvent
**
******************************************************/
void EphemExpert::testIngressEvent( const int &t1, const int &t2, const PlanetId &planet, const int &type, const bool &r )
{
	const Viewpoint viewpoint = getViewpoint();

	if ( t1 != t2 )
	{
		Calculator *calculator = CalculatorFactory().getCalculator();
		const double targetlen = ( r ? t1 : t2 ) * ( type ? NAKSHATRA_LEN : 30 );
		const double tjd = calculator->calcPlanetaryEvent( dl, targetlen, planet, viewpoint );
		ingressEvents.push_back({ tjd, planet, t2, type });
	}
}

/*****************************************************
**
**   EphemExpert   ---   writeHeaderInfo
**
******************************************************/
void EphemExpert::writeHeaderInfo( MDocument_ptr doc )
{
	if ( ! show_header ) return;

	Lang lang;
	doc->addHeader2( std::format( "{} {}", lang.getMonthName( month-1 ), year ));
}

/*****************************************************
**
**   EphemExpert   ---   writeDayShortcut
**
******************************************************/
std::pair<wxString, bool> EphemExpert::writeDayShortcut( const double &jd )
{
	DateFormatter *dh = DateFormatter::get();

	const std::pair<int, int> wd = dh->getDayOfMonthAndWeekday( jd, props->isLocaltime );
	debuglog << "EphemExpert::writeDayShortcut date " << dh->formatDatetime( jd, props->isLocaltime )
		<< " day of month " << wd.first << " weekday " << wd.second;

	const wxString s = std::format( "{:02d} {}", wd.first, Lang().getWeekdayName( wd.second ).substr( 0, 2 ).ToStdString());

	return std::pair<wxString, bool >( s,
		month == current_month && year == current_year && wd.first == current_day );
}

/*****************************************************
**
**   EphemExpert   ---   writeIngress
**
******************************************************/
int EphemExpert::writeIngress( MDocument_ptr doc )
{
	int ret = 0;
	Lang lang;
	DateFormatter *dh = DateFormatter::get();
	std::pair<wxString, bool> day;
	const Viewpoint viewpoint = getViewpoint();

	infolog << "EphemExpert::writeIngress isLocaltime " << props->isLocaltime;

	if ( ! calc_ingress ) ret = calculateIngress();
	writeHeaderInfo( doc );

	Table *table = new Table;
	table->addHeaderRow(
	{
		tr( "Day" ),
		tr( "Time" ),
		tr( "Planet" ),
		viewpoint == Viewpoint::Vedic ? tr( "Sign/Nakshatra" ) : tr( "Sign" )
	});

	for ( const IngressEvent &event :ingressEvents )
	{
		day = writeDayShortcut( event.jd );

		TableRow row;
		row.addValue( day.first, day.second );
		row.addValue( dh->formatTime( event.jd, props->isLocaltime ));
		row.addValue( MToken( event.planet, viewpoint ));
		if ( event.type == 0 )
			row.addValue( MToken( (Sign)event.which, viewpoint ));
		else
			row.addValue( lang.getNakshatra27Name( (Nakshatra27)event.which ));

		table->addRow( row );
	}
	doc->addItem( table );
	return ret;
}

/*****************************************************
**
**   EphemExpert   ---   addLunarEvent
**
******************************************************/
void EphemExpert::addLunarEvent( const int &stdate, const double& target, const double& sdiff, const double &ediff )
{
	double slen, mlen, cjd;
	Calculator *calculator = CalculatorFactory().getCalculator();

	const Viewpoint viewpoint = getViewpoint();
	const CalculationConfig &cconfig = astroconfig->getViewpointConfig( viewpoint ).calculation;

	dl.date.jd = jd[stdate] + ( target - sdiff ) / ( ediff - sdiff );

	cjd = calculator->calcSunMoonEvent( dl, target, slen, mlen );
	const double aya = calculator->calcAyanamsa( cjd, cconfig.ayanamsa );

	lunarEvents.push_back({ target, cjd, red_deg( slen - aya ), red_deg( mlen - aya ) });
}


/*****************************************************
**
**   EphemExpert   ---   addLunarSpecialEvent
**
******************************************************/
void EphemExpert::addLunarSpecialEvent( const int &stdate, const double& target, const double& sdiff, const double &ediff )
{
	if ( sdiff < target && ediff >= target ) addLunarEvent( stdate, target, sdiff, ediff );
}

/*****************************************************
**
**   EphemExpert   ---   calculateLunar
**
******************************************************/
void EphemExpert::calculateLunar()
{
	int i, t, t2;
	double diff, diff2;
	lunarEvents.clear();

	if ( ! calc_coordinates ) calculateCoordinates();

	assert( data[PlanetId::Moon].size() >= (uint)nb_days );
	assert( data[PlanetId::Sun].size() >= (uint)nb_days );

	diff = red_deg( data[PlanetId::Moon][0].longitude - data[PlanetId::Sun][0].longitude );
	t = (int)(diff/12);
	for ( i = 1; i <= nb_days; i++ )
	{
		diff2 = red_deg( data[PlanetId::Moon][i].longitude - data[PlanetId::Sun][i].longitude );
		t2 = (int)(diff2/12);
		
		if ( t2 != t )
		{
			addLunarEvent( i-1, t2 * 12, diff, diff2 );

			// Happens once a month, i.e. double step
			// if ( t2 - t > 1 )
			// bugfix: t2 may loop immediately to 1 with t remaining high (!= 0)
			if ( t2 - t > 1 || ( t2 == 1 && t != 0 ) )
			{
				addLunarEvent( i-1, ( t + 1 ) * 12, diff, diff2 );
			}
		}
		// Squares
		addLunarSpecialEvent( i-1, 90, diff, diff2 );
		addLunarSpecialEvent( i-1, 270, diff, diff2 );

		// Semi Squares
		addLunarSpecialEvent( i-1, 45, diff, diff2 );
		addLunarSpecialEvent( i-1, 135, diff, diff2 );
		addLunarSpecialEvent( i-1, 225, diff, diff2 );
		addLunarSpecialEvent( i-1, 315, diff, diff2 );

		diff = diff2;
		t = t2;
	}

	lunarEvents.sort( [] ( const LunarEvent &e1, const LunarEvent &e2 ) { return( e1.jd < e2.jd ); });
	calc_lunar = true;
}

/*****************************************************
**
**   EphemExpert   ---   writeLunar
**
******************************************************/
void EphemExpert::writeLunar( MDocument_ptr doc )
{
	if ( ! calc_lunar ) calculateLunar();
	Lang lang;
	DateFormatter *dh = DateFormatter::get();
	std::pair<wxString, bool> day;
	wxString s;

	writeHeaderInfo( doc );
	Table *table = new Table;
	table->addHeaderRow(
	{
		tr( "Day" ),
		tr( "Time" ),
		tr( "Sun" ),
		tr( "Moon" ),
		tr( "Angle" ),
		tr( "Tithi" ),
		tr( "Western" )
	});

	for ( const LunarEvent &e : lunarEvents )
	{
		TableRow row;

		day = writeDayShortcut( e.jd );
		row.addValue( day.first, day.second );

		row.addValue( dh->formatTime( e.jd, props->isLocaltime ));

		row.addValue( getPosFormatted( e.slen, MovingDirection::None, DegPrecision::Minute ));
		row.addValue( getPosFormatted( e.mlen, MovingDirection::None, DegPrecision::Minute ));

		int angle = (int)( red_deg( e.mlen - e.slen ) + .00001 );
		if ( angle >= 360 ) angle -= 360;
		row.addInteger( angle );

		if ( ! ( angle % 12 ))
		{
			row.addValue( lang.getTithiName( angle / 12 ) );
		}
		else row.addValue();

		switch( (int)(angle + .1 ))
		{
			case 0:
				s = tr( "New Moon" );
			break;
			case 180:
				s = tr( "Full Moon" );
			break;
			case 60:
			case 300:
				s = tr( "Sextile" );
			break;
			case 90:
				s = tr( "Half Moon (Waxing)" );
			break;
			case 120:
			case 240:
				s = tr( "Trine" );
			break;
			case 270:
				s = tr( "Half Moon (Waning)" );
			break;
			case 45:
			case 135:
			case 225:
			case 315:
				s = tr( "Semi Square" );
			break;
			default:
				s.clear();
			break;
		}
		row.addValue( s );
		table->addRow( row );
	}
	doc->addItem( table );
}

/*****************************************************
**
**   EphemExpert   ---   calculateSunrise
**
******************************************************/
void EphemExpert::calculateSunrise()
{
	Calculator *calculator = CalculatorFactory().getCalculator();
	const Viewpoint viewpoint = getViewpoint();
	double mlen = 0, slen = 0;

	for ( int i = 0; i < nb_days; i++ )
	{
		dl.date.set( year, month, i+1, 0 ); // Must be 0, correct offset will follow in formatting

		// Only the time will be affected: location ist default! (not 0 - 0)
		st[i] = calculator->calcSiderealTime( dl.date.jd, dl.location.longitude );

		std::tie( sunrise[i], sunset[i] ) = calculator->calcSunRiseSunSet( dl );

		dl.date.jd = sunrise[i];

		sunrise_sunlen[i] = calculator->calcLongitude( dl, PlanetId::Sun, viewpoint );
		sunrise_moonlen[i] = calculator->calcLongitude( dl, PlanetId::Moon, viewpoint );
		sunrise_angle[i] = red_deg( sunrise_moonlen[i] - sunrise_sunlen[i] );
		tithi[i] = (int)( sunrise_angle[i] /12);
		debuglog << "EphemExpert::calculateSunrise day " << i << " mlen " << mlen << " slen " << slen
			<< " diff " << sunrise_angle[i] << " tithi " << tithi[i];
	
#if 1
		DateFormatter *dh = DateFormatter::get();
		Lang lang;
		infolog << "EphemExpert::calcSunrise day " << ( i + 1 ) << " day " << 
			lang.getWeekdayName( weekday[i] )
			<< " sunrise " << dh->formatDatetime( sunrise[i], props->isLocaltime )
			<< " sunset " << dh->formatDatetime( sunset[i], props->isLocaltime );
#endif
	}

	calc_sunrise = true;
}

/*****************************************************
**
**   EphemExpert   ---   getFormattedDate
**
******************************************************/
wxString EphemExpert::getFormattedDate( const double &jd )
{
	DateFormatter *dh = DateFormatter::get();
#if 1
	return dh->formatTime( jd, props->isLocaltime );
#else
	return dh->formatDatetimeWithOffset( jd, props->isLocaltime );
#endif
}

/*****************************************************
**
**   EphemExpert   ---   writeSunrise
**
******************************************************/
void EphemExpert::writeSunrise( MDocument_ptr doc )
{
	Formatter *formatter = Formatter::get();
	Lang lang;

	if ( ! calc_sunrise  ) calculateSunrise();
	writeHeaderInfo( doc );

	// location data
	{
		const Location &loc = dl.location;
		doc->addParagraph( std::format( "{}: {}. {}: {}, {}: {}",
			tr( "Location" ), loc.locName.IsEmpty() ? tr( "None" ) : loc.locName,
			tr( "Longitude" ), formatter->formatLongitude( loc.longitude ),
			tr( "Latitude" ), formatter->formatLatitude( loc.latitude ))
		);
	}

	const std::list<wxString> theader = {
		tr( "Day" ),
		tr( "Sunrise" ),
		tr( "Sunset" ),
		tr( "Tithi" ),
		tr( "Sun at Sunrise" ),
		tr( "Moon at Sunrise" ),
		tr( "Angle" ),
		tr( "Day" ),
	};
	Table *table = new Table;
	table->addHeaderRow( theader );

	for ( int i = 0; i < nb_days; i++ )
	{
		// blank line on weekend
		if ( i > 0 && weekday[i] == 0 ) table->addEmptyRow( true );

		TableRow row;
		const bool is_current = ( in_current_month && i == current_day - 1 );
		wxString day = std::format( "{} {}", ( i + 1 ), lang.getWeekdayName( weekday[i] ).substr( 0, 2 ));
		row.addValue( day, is_current );

		row.addValue( getFormattedDate( sunrise[i] ));
		row.addValue( getFormattedDate( sunset[i] ));

		row.addValue( lang.getTithiName( tithi[i] ));

		row.addValue( getPosFormatted( sunrise_sunlen[i] ));
		row.addValue( getPosFormatted( sunrise_moonlen[i] ));

		row.addValue( std::format( "{:.1f}", sunrise_angle[i] ));

		row.addValue( day, is_current );
		table->addRow( row );
	}
	table->addHeaderRow( theader );
	doc->addItem( table );
}

/*****************************************************
**
**   EphemExpert   ---   writeCoordinates
**
******************************************************/
int EphemExpert::writeCoordinates( MDocument_ptr doc, const CoordinateMode coordMode, const int division )
{
	int ret = 0;
	assert( division );

	Lang lang;
	MovingDirection dir;
	Formatter *formatter = Formatter::get();
	const Viewpoint viewpoint = getViewpoint();
	const LunarNodeMode nodeMode = astroconfig->getViewpointConfig( viewpoint ).calculation.lunarNodeMode;

	if ( ! calc_coordinates  ) ret = calculateCoordinates();

	std::list<MString> headerRow = { MString( tr( "Day" )) };
	for ( const PlanetId &p : obs )
	{
		headerRow.push_back( MToken( p, viewpoint ));
	}
	headerRow.push_back( MString( tr( "Day" )));

	Table *table = new Table;
	table->addHeaderRow( headerRow );

	for ( int i = 0; i < nb_days; i++ )
	{
		if ( i > 0 && weekday[i] == 0 ) table->addEmptyRow();

		TableRow row;
		const bool is_current = ( in_current_month && i == current_day - 1 );
		wxString day = std::format( "{} {}", ( i + 1 ), lang.getWeekdayName( weekday[i] ).substr( 0, 2 ));
		row.addValue( day, is_current );

		for ( const PlanetId &p : obs )
		{
			MString f;
			switch( coordMode )
			{
				case CoordinateMode::Declination:
					f = formatter->formatLatitude( data[p][i].declination, DegPrecision::Minute );
				break;
				case CoordinateMode::AbsDeclination:
					f = formatter->formatDegrees( fabs( data[p][i].declination ), DegPrecision::Minute );
				break;
				case CoordinateMode::Longitude:
				{
					if ( division == 1 )
					{
						dir = MovingDirection::None;
						if ( data[p][i].lspeed < 0 )
						{
							 if ( ! PlanetHelper::isNode( p ) || nodeMode == LunarNodeMode::True ) 
							 {
								 dir = MovingDirection::Retrograde;
							 }
						}

						// is not retrograde, but the previous was retrograde -> write "D" symbol
						else if ( i > 0 && data[p][i-1].lspeed < 0 ) { dir = MovingDirection::Direct; }

						f = getPosFormatted( data[p][i].longitude, dir, DegPrecision::Minute, division );
						if ( viewpoint == Viewpoint::Vedic )
						{
							f.add( MToken( lang.getNakshatra27Name( ::getNakshatra27( data[p][i].longitude ), TextFormat::Short )));
						}
					}
					else
					{
						f = formatter->formatDegrees( a_red( data[p][i].longitude, 360.0 / division ), DegPrecision::Minute );
					}
				}
				break;
				default:
					fatallog << "wrong CoordinateMode " << (int)coordMode;
					assert( false );
				break;
			}
			row.addValue( f );
		}
		row.addValue( day, is_current );
		table->addRow( row );
	}
	
	table->addHeaderRow( headerRow );
	doc->addItem( table );
	return ret;
}


