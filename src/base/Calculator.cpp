/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Calculator.cpp
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

#include "Calculator.h"

#include "astroconfig.h"
#include "DateFormatter.h"
#include "datelocation.h"
#include "enumserializer.h"
#include "logger.h"
#include "mathbase.h"

#include "swephexp.h"
#include "sweph.h"

#include <format>
#include <iomanip>

extern Astroconfig *astroconfig;

//#define PRINT_PEDANTIC	if ( err[0] != 0 ) { debugog << "Swe: "  << err; }
#define PRINT_PEDANTIC	{}

//#define DEBUG_CALCULATOR

/*************************************************//**
*
*  Swiss Ephemeris implementation of Calculator
*
******************************************************/
class CalculatorSwe : public Calculator
{
public:

	/*****************************************************
	**
	**   CalculatorSwe   ---   Constructor
	**
	******************************************************/
	CalculatorSwe()
	{
		init();
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   Destructor
	**
	******************************************************/
	virtual ~CalculatorSwe()
	{
		swe_close();
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   init
	**
	******************************************************/
	virtual void init() override
	{
		//if ( ! astroconfig->ephem.sePath.IsEmpty() )
		if ( true )
		{
			swe_set_ephe_path( (char*)astroconfig->ephem.sePath.ToAscii().data() );
#ifdef DEBUG_CALCULATOR
			debuglog << "Ephem path: " << astroconfig->ephem.sePath;
#endif
		}
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   getVersion
	**
	******************************************************/
	virtual wxString getVersion() override
	{
		char buf[ 256];
		swe_version( buf );
		return buf;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   getBuiltinAyanamsaName
	**
	******************************************************/
	virtual wxString getBuiltinAyanamsaName( const int &i ) const override
	{
		if ( i <= 0 || i > SE_NSIDM_PREDEF ) return std::format( "ERROR/{}", i );
		return swe_get_ayanamsa_name( i );
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   getBuiltinAyanamsaNames
	**
	******************************************************/
	virtual std::list<wxString> getBuiltinAyanamsaNames() const override
	{
		std::list<wxString> l;
		for( int32 i = 0; i < SE_NSIDM_PREDEF; i++ )
		{
			l.push_back( swe_get_ayanamsa_name( i ));
		}
		return l;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   Swe Sizes
	**
	******************************************************/
	virtual int getBuiltinAyanamsaSize() const override { return SE_NSIDM_PREDEF; }

	virtual int getBuiltinPrecessionModelsSize() const override { return SEMOD_NPREC; }

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcAyanamsa
	**
	******************************************************/
	virtual double calcAyanamsa( const double &jd, const int &type ) override
	{
		switch( type )
		{
			// tropical
			case 0:
				return 0;
			break;

			// custom
			case 1:
				return calcCustomAyanamsa( jd, astroconfig->customAyanamsa );
			break;

			// builtin
			default:
				swe_set_sid_mode( type - 2, 0, 0 );
				return swe_get_ayanamsa( calcJD( jd ));
			break;
		}
		return 0;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcCustomAyanamsa
	**
	******************************************************/
	virtual double calcCustomAyanamsa( const double &jd, const CustomAyanamsaConfig &cfg ) override
	{
		double t = 0.0;
		if ( cfg.hasConstantPeriod )
		{
			t = calcJD(jd ) - cfg.t0;
			const double years = t/365.25;

			assert( cfg.aya_constant_period != 0 );
			const double portion = years/ cfg.aya_constant_period;
			const double aya = portion * 360;

			return red_deg( cfg.ayan_t0 + aya );
		}
		else
		{
			swe_set_sid_mode( 255, cfg.t0, cfg.ayan_t0 );
			return swe_get_ayanamsa( calcJD( jd ));
		}
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   getPrecessionModeNames
	**
	******************************************************/
	virtual std::list<wxString> getPrecessionModeNames() const override
	{
		return {
			"NoPrecession",
			"IAU 1976",
			"Laskar 1986",
			"Will Eps Laskar",
			"Williams 1994",
			"Simon 1994",
			"IAU 2000",
			"Bretagnon 2003",
			"IAU 2006",
			"Vondrak 2011",
			"Owen 1990",
			"Newcomb",
		};
	/*
	#define SEMOD_NPREC   11
#define SEMOD_PREC_IAU_1976      1
#define SEMOD_PREC_LASKAR_1986   2
#define SEMOD_PREC_WILL_EPS_LASK 3
#define SEMOD_PREC_WILLIAMS_1994 4
#define SEMOD_PREC_SIMON_1994    5
#define SEMOD_PREC_IAU_2000      6
#define SEMOD_PREC_BRETAGNON_2003      7
#define SEMOD_PREC_IAU_2006      8
#define SEMOD_PREC_VONDRAK_2011  9
#define SEMOD_PREC_OWEN_1990     10
#define SEMOD_PREC_NEWCOMB       11
#define SEMOD_PREC_DEFAULT       SEMOD_PREC_VONDRAK_2011
	*/
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   getBuiltinAyanamsaParams
	**
	******************************************************/
	virtual CustomAyanamsaConfig getBuiltinAyanamsaParams( const int &type ) const override
	{
		assert( type >= 0 && type < SE_NSIDM_PREDEF );

		// static const struct aya_init ayanamsa[SE_NSIDM_PREDEF] 
		const struct aya_init &aya = ayanamsa[type]; 

		CustomAyanamsaConfig cfg;
		cfg.t0 = aya.t0;
		cfg.ayan_t0 = aya.ayan_t0;
		cfg.t0_ut = aya.t0_is_UT;
		cfg.precessionModel = aya.prec_offset;

		return cfg;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcSiderealTime
	**
	******************************************************/
	virtual double calcSiderealTime( const double &jd , const double &longitude ) override
	{
		return a_red( swe_sidtime( jd ) + longitude / 15, 24.0 );
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcEps
	**
	******************************************************/
	virtual void calcEps( double &eps, const double &jd ) override
	{
		char err[256];
		double x[6];

		// x[0] enthaelt eps
		if ( swe_calc( calcJD( jd ), SE_ECL_NUT, 0, x, err) == -1 )
		{
			assert( false );
		}
		PRINT_PEDANTIC
		eps = x[0];
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcDeclination
	**
	******************************************************/
	virtual double calcDeclination( const double &longitude, const double &latitude, const double &eps ) override
	{
		double xpn[3], xpo[3] = { longitude, latitude, 1 };
		swe_cotrans( xpo, xpn, -eps );
		return xpn[1];
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcNextSolarEvent
	**
	******************************************************/
	virtual double calcNextSolarEvent( const SolarEventType &type, const double &jd, const double &lat, const double &lon ) override
	{
		char err[256];
		double rsmi[3], tret[3];

		int flag = 0;
		if ( ! astroconfig->ephem.sunrise_def ) flag = SE_BIT_DISC_CENTER;
		if ( ! astroconfig->ephem.sunrise_refrac ) flag |= SE_BIT_NO_REFRACTION;

		switch ( type )
		{
		case SolarEventType::Sunrise:
			flag |= SE_CALC_RISE;
			break;
		case SolarEventType::Sunset:
			flag |= SE_CALC_SET;
			break;
		case SolarEventType::Noon:
			flag |= SE_CALC_ITRANSIT;
			break;
		case SolarEventType::Midnight:
			flag |= SE_CALC_MTRANSIT;
			break;
		default:
			assert( false );
			break;
		}

		rsmi[0] = lon;
		rsmi[1] = lat;
		rsmi[2] = 0;

		swe_rise_trans( calcJD( jd ), SE_SUN, 0, 0, flag, rsmi, 0, 0, tret, err );
		PRINT_PEDANTIC
		return tret[0];
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcSunRiseSunSet
	**
	******************************************************/
	virtual std::pair<double,double > calcSunRiseSunSet( const DateLocation &dl ) override
	{
		char err[256];
		double r[6];
		double cusp[13], ascmc[10];
		double rsmi[3], tret[3];
		int flag = 0;

		if ( ! astroconfig->ephem.sunrise_def ) flag = SE_BIT_DISC_CENTER;
		if ( ! astroconfig->ephem.sunrise_refrac ) flag |= SE_BIT_NO_REFRACTION;
		double jd = calcJD( dl.date.jd );

		swe_calc( jd, SE_SUN, 0, r, err );
		PRINT_PEDANTIC
		swe_houses( jd, dl.location.latitude, dl.location.longitude, 'P', cusp, ascmc );

		rsmi[0] = dl.location.longitude;
		rsmi[1] = dl.location.latitude;
		rsmi[2] = 0;

		swe_rise_trans( jd, SE_SUN, 0, 0, SE_CALC_RISE|flag, rsmi, 0, 0, tret, err );
		const double sunrise = tret[0];

		swe_rise_trans( jd, SE_SUN, 0, 0, SE_CALC_SET|flag, rsmi, 0, 0, tret, err );
		PRINT_PEDANTIC
		const double sunset = tret[0];

		return std::make_pair( sunrise, sunset );;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcPositionSpeed
	**
	******************************************************/
	virtual PlanetPosition calcPositionSpeed( const DateLocation &dl, const PlanetId &p,
		const Viewpoint &viewpoint ) override
	{
		double r[6];  // target in ecliptic coordinates: long, lat, dist + speed of long, lat and dist
		char err[256];

		const PlanetId planet = PlanetHelper::isNode( p ) ?  PlanetHelper::resolveNode( p, viewpoint ) : p;
#ifdef DEBUG_CALCULATOR
		debuglog << "CalculatorSwe::calcPositionSpeed p " << p << " planet " << planet;
#endif

		switch( planet )
		{
			case PlanetId::MeanDescNode:
			case PlanetId::TrueDescNode:
			case PlanetId::DescNode:
				swe_calc( calcJD( dl.date.jd ),  mapOToS( planet ), calcIFlag( dl ) | SEFLG_SPEED, r, err );
				r[0] += 180;
			break;
			default:
				swe_calc( calcJD( dl.date.jd ), mapOToS( planet ), calcIFlag( dl ) | SEFLG_SPEED, r, err );
			break;
		}
		PRINT_PEDANTIC

		if ( r[0] != 0.0 )
		{
			double ayanamsa = 0;
			if ( is_valid_enum( viewpoint ))
			{
				ayanamsa = calcAyanamsa( dl.date.jd, astroconfig->getViewpointConfig( viewpoint ).calculation.ayanamsa );
			}
			double eps;
			double xpn[6]; // target in equatorial coordinates: rectascension, decl, dist + speed x3
			double r2[6];
			for( int i = 0; i < 6; i++ ) r2[i] = r[i];

			calcEps( eps, dl.date.jd );
			swe_cotrans_sp( r2, xpn, -eps );
			return PlanetPosition( red_deg( r[0] - ayanamsa ), r[3], r[1], r[4], xpn[1], xpn[4] );
		}
		return PlanetPosition( 0.0 );
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcPositionSpeed
	**
	******************************************************/
	virtual void calcPositionSpeed( const DateLocation &dl, const PlanetId &p, double &slen, double &slat,
		double &sspeed, const Viewpoint &viewpoint ) override
	{
		const PlanetId planet = PlanetHelper::isNode( p ) ?  PlanetHelper::resolveNode( p, viewpoint ) : p;

		const PlanetPosition pos = calcPositionSpeed( dl, planet, viewpoint );
		slen = pos.longitude;
		slat = pos.latitude;
		sspeed = pos.lspeed;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcLongitude
	**
	******************************************************/
	virtual double calcLongitude( const DateLocation &dl, const PlanetId &planet, const Viewpoint &viewpoint ) override
	{
		double r[6];
		char err[256];

		switch( planet )
		{
			case PlanetId::MeanDescNode:
			case PlanetId::TrueDescNode:
			case PlanetId::DescNode:
				swe_calc( calcJD( dl.date.jd ),  mapOToS( planet ), calcIFlag( dl ) | SEFLG_SPEED, r, err );
				r[0] += 180;
			break;
			default:
				swe_calc( calcJD( dl.date.jd ), mapOToS( planet ), calcIFlag( dl ), r, err );
			break;
		}
		PRINT_PEDANTIC
		return red_deg( r[0] - calcAyanamsa( dl.date.jd, astroconfig->getViewpointConfig( viewpoint ).calculation.ayanamsa ));
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcAnglesMcByArmc
	**
	******************************************************/
	virtual std::pair<double, double> calcAnglesByArmc( const double &armc, const double &geolat, const double &eps ) override
	{
		double cusp[13], ascmc[10];
		swe_houses_armc( armc, geolat, eps, 'E', cusp, ascmc );
		return std::make_pair( ascmc[0], ascmc[1] );
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcAngles Speed
	**
	******************************************************/
	virtual std::array<double, 5> calcAnglesSpeed( const double &jd, const double &geolat, const double &geolon ) override
	{
		std::array<double, 5> a;

		double cusps[13];
		double ascmc[10];
		double cusp_speed[13];
		double ascmc_speed[10];
		char serr[256];

		const int ret = swe_houses_ex2( jd, 0, geolat, geolon, 'E', cusps, ascmc, cusp_speed, ascmc_speed, serr );
		if ( ! ret )
		{
			a[0] = ascmc[2]; // armc
			a[1] = ascmc[0]; // asc
			a[3] = ascmc[1]; // mc
			a[2] = ascmc_speed[0]; // asc speed
			a[4] = ascmc_speed[1]; // mc speed
		}
		else
		{
			errorlog << "Function swe_houses_ex2 returned error code " << ret << ", message was " << serr;
		}
#ifdef DEBUG_CALCULATOR
		debuglog << "CalculatorSwe::calcAngleSpeed asc " << a[0] << " ascspeed " << a[1]
			<< " mc " << a[2] << " mcspeed " << a[3];
#endif

		return a;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcHousesByArmc
	**
	******************************************************/
	virtual std::array<double, 12> calcHousesByArmc( const double &armc, const double &geolat,
		const double &eps, const HouseSystem &hsys ) override
	{
	  double cusp[13], ascmc[12];
		std::array<double, 12> a;

		swe_houses_armc( armc, geolat, eps, getHouseSystemCode( hsys ), cusp, ascmc );

		for( int i = 0; i < 12; ++i )
		{
			a[i] = cusp[i+1];
		}
		return a;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calc_jd
	**
	******************************************************/
	virtual double calc_jd( const int &year, const int &month, const int &day,
		const double &time, const bool gregflag = true ) override
	{
		return swe_julday( year, month, day, time, gregflag ? 1 : 0 );
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calc_and_check_jd
	**
	** returns 
	**  - true if okay
	**  - false if not okay
	**
	******************************************************/
	virtual bool calc_and_check_jd( const int &year, const int &month, const int &day,
		const double &time, double &jd ) override
	{
		const char c = isGregorianCalendarDate( year, month, day ) ? 'g' : 'j';
		const int ret = swe_date_conversion( year, month, day, time, c, &jd );
		if ( ret == ERR ) return true;
		if ( year < MIN_EPHEM_YEAR || year > MAX_EPHEM_YEAR )
		{
			return true;
		}
		else return false;
	}

#define GREG_CALENDAR_START_JD 2299160.5
	/*****************************************************
	**
	**   CalculatorSwe   ---   isGregorianCalendarDate
	**
	******************************************************/
	virtual bool isGregorianCalendarDate( const int &year, const int &month, const int &day ) override
	{
		// Oct 14 1582
		//if ((int32) jyear * 10000L + (int32) jmon * 100L + (int32) jday < 15821015L)
		return year > 1582 || month > 10 || day > 14;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   isGregorianCalendarDate
	**
	******************************************************/
	virtual bool isGregorianCalendarDate( const double &jd ) override
	{
		return jd > GREG_CALENDAR_START_JD;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcSolarReturnJD
	**
	******************************************************/
	virtual double calcSolarReturnJD( const DateLocation &dateloc, const int &tyear, const Viewpoint &viewpoint ) override
	{
		int count = 0;
		double diff, dummy1;
		int dummy2, dummy3, year;
		PlanetPosition pos( 0 );
		std::tie( year, dummy2, dummy3, dummy1 ) = CalculatorFactory().getCalculator()->getDateIntsFromJD( dateloc.date.jd );

		DateLocation dl( dateloc );
		const double birthlen = calcLongitude( dateloc, PlanetId::Sun, viewpoint );
		dl.date.jd = dateloc.date.jd + ( tyear - year ) * 365.25;
		do
		{
			pos = calcPositionSpeed( dl, PlanetId::Sun, viewpoint );

			diff = pos.longitude - birthlen;
			if ( diff > 200 ) diff -= 360;
			if ( diff < -200 ) diff += 360;
			dl.date.jd -= diff / pos.lspeed;
		}
		while (( fabs( diff ) > 0.0000001 ) && ( count++ < 20 ));

		if ( count > 19 ) errorlog << "Too many iterations for calculation of Sun longitude";
		return dl.date.jd;
	}


	/***************************************************************
	**
	** CalculatorSwe   ----  getDateIntsFromJD
	**
	***************************************************************/
	virtual std::tuple<int,int,int,double> getDateIntsFromJD( const double &jd ) override
	{
		int day = 0;
		int month= 0;
		int year = 0;
		double ut = 0.0;
		swe_revjul( jd, jd >= GREG_CALENDAR_START_JD ? 1 : 0, &year, &month, &day, &ut );
#ifdef DEBUG_CALCULATOR
		debuglog << "CalculatorSwe::getDateIntsFromJD jd " << std::setprecision( 12 ) << std::setw( 12 ) << jd
			<< " GREG_CALENDAR_START_JD " << GREG_CALENDAR_START_JD << " day " << day << " month " << month << " year " << year
			<< "ut " << ut;
#endif
		return std::make_tuple( year, month, day, ut );
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcAscendant
	**
	******************************************************/
	virtual double calcAscendant( const double &jd, const double &lat, const double &lon, const Viewpoint &viewpoint ) override
	{
		const CalculationConfig &cconfig = astroconfig->getViewpointConfig( viewpoint ).calculation;
		double cusp[13], ascmc[10];

		swe_houses( jd, lat, lon, 'A', cusp, ascmc );
		return red_deg( ascmc[0] - calcAyanamsa( jd, cconfig.ayanamsa ));
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcNextEclipse
	**
	******************************************************/
	virtual EclipseData calcNextEclipse( const double &jd, const PlanetId &planet,
		const int & /* type */, const Viewpoint &viewpoint ) override
	{
		EclipseData data;
		double tret[10];
		char err[256];
		int ret;
		DateLocation d;
		d.location = astroconfig->standardLocation;
		d.date.jd = jd;

		if ( planet == PlanetId::Sun ) ret = swe_sol_eclipse_when_glob( calcJD( jd ), 0, 0, tret, false, err );
		else ret = swe_lun_eclipse_when( calcJD( jd ), 0, 0, tret, false, err );
		PRINT_PEDANTIC
		data.jd = tret[0];
		d.date.jd = data.jd;
		data.type = ret;
		data.planet = planet;
		data.sunpos = calcLongitude( d, PlanetId::Sun, viewpoint );
		data.moonpos = calcLongitude( d, PlanetId::Moon, viewpoint );
		if ( ret & SE_ECL_TOTAL ) data.description = tr( "Total" );
		else if ( ret & SE_ECL_ANNULAR ) data.description = tr( "Annular" );
		else if ( ret & SE_ECL_PARTIAL ) data.description = tr( "Partial" );
		else if ( ret & SE_ECL_PENUMBRAL ) data.description = tr( "Penumbral" );
		else if ( ret & SE_ECL_ANNULAR_TOTAL ) data.description = tr( "Annular Total" );
		else data.description = tr( "unknown" );

		return data;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcPlanetaryEvent
	**
	******************************************************/
	virtual double calcPlanetaryEvent( const DateLocation &dateloc, const double &targetlen, const PlanetId &planet,
		const Viewpoint &viewpoint ) override
	{
		double diff, len, dummy, speed, tlen;
		DateLocation dl( dateloc );

		int count = 0;
		PlanetId pp = planet;

		tlen = targetlen;
		// map both rahu and ketu to the same node (depending on conf for inode)
		if ( planet == PlanetId::TrueDescNode )
		{
			pp = PlanetId::TrueAscNode;
			tlen = red_deg( tlen + 180 );
		}
		if ( planet == PlanetId::MeanDescNode )
		{
			pp = PlanetId::MeanAscNode;
			tlen = red_deg( tlen + 180 );
		}
		if ( planet == PlanetId::DescNode )
		{
			pp = PlanetId::AscNode;
			tlen = red_deg( tlen + 180 );
		}

		do
		{
			calcPositionSpeed( dl, pp, len, dummy, speed, viewpoint );

			// bugfix 6.0: found ingress event with speed=0 (Month 1/11, planet Jupiter, western ephemeris)
			if ( speed == 0 )
			{
				warnlog << "CalculatorSwe::calcPlanetaryEvent WARN: planetary speed is null: planet "
					<< (int)planet
					<< ", targetlen "
					<< targetlen 
					<< " date "
					<< dl.date.jd
					<< " calculated len "
					<< len;
				speed = .0000000001;
			}

			diff = len - tlen;
			if ( diff > 200 ) diff -= 360;
			if ( diff < -200 ) diff += 360;
			dl.date.jd = dl.date.jd - diff / speed;
		}
		while (( fabs( diff ) > 0.0000001 ) && ( count++ < 20 ));

		if ( count > 19 )
		{
			errorlog << "Too many iterations for calculation of planetary longitude";
		}
		return dl.date.jd;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcSunMoonEvent
	**
	******************************************************/
	virtual double calcSunMoonEvent( const DateLocation &dateloc, const double &angle, double &slen, double &mlen ) override
	{
		double mspeed, sspeed, dummy, a, diff;
		int count = 0;
		const double targettol = .000001;
		const Viewpoint viewpoint = Viewpoint::Astronomical;

		DateLocation dl( dateloc );

		do
		{
			calcPositionSpeed( dl, PlanetId::Sun, slen, dummy, sspeed, viewpoint );
			calcPositionSpeed( dl, PlanetId::Moon, mlen, dummy, mspeed, viewpoint );
			a = red_deg( mlen - slen );
			diff = angle - a;
			if ( diff > 200 ) diff -= 360;
			if ( diff < -200 ) diff += 360;
			dl.date.jd += diff / ( mspeed - sspeed );

			count++;
		}
		while ( fabs( diff ) > targettol && count < 20 );

		return dl.date.jd;
	}

private:

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcJF
	**
	******************************************************/
	double calcJD( const double &jd )
	{
		return( astroconfig->ephem.useEphemerisTime ? jd : jd + swe_deltat( jd ));
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   mapOToS
	**
	******************************************************/
	int mapOToS( const PlanetId &ob )
	{
		// must be resolved before
		assert( ob != PlanetId::AscNode);
		assert( ob != PlanetId::DescNode );

		const static std::map<PlanetId, int> objectMapping =
		{
			{ PlanetId::Sun, SE_SUN },
			{ PlanetId::Moon, SE_MOON },
			{ PlanetId::Mercury, SE_MERCURY },
			{ PlanetId::Venus, SE_VENUS },
			{ PlanetId::Mars, SE_MARS },
			{ PlanetId::Jupiter, SE_JUPITER },
			{ PlanetId::Saturn, SE_SATURN },
			{ PlanetId::Uranus, SE_URANUS },
			{ PlanetId::Neptune, SE_NEPTUNE },
			{ PlanetId::Pluto, SE_PLUTO },
			{ PlanetId::TrueAscNode, SE_TRUE_NODE },
			{ PlanetId::MeanAscNode, SE_MEAN_NODE },

			// longitude must be handled separately (add 180)
			{ PlanetId::TrueDescNode, SE_TRUE_NODE },
			{ PlanetId::MeanDescNode, SE_MEAN_NODE },

			{ PlanetId::Cupido, SE_CUPIDO },
			{ PlanetId::Hades, SE_HADES },
			{ PlanetId::Zeus, SE_ZEUS },
			{ PlanetId::Kronos, SE_KRONOS },
			{ PlanetId::Apollon, SE_APOLLON },
			{ PlanetId::Admetos, SE_ADMETOS },
			{ PlanetId::Vulkanus, SE_VULKANUS },
			{ PlanetId::Poseidon, SE_POSEIDON },
			{ PlanetId::Chiron, SE_CHIRON },
			{ PlanetId::Pholus, SE_PHOLUS },
			{ PlanetId::Ceres, SE_CERES },
			{ PlanetId::Pallas, SE_PALLAS },
			{ PlanetId::Juno, SE_JUNO },
			{ PlanetId::Vesta, SE_VESTA },
			{ PlanetId::Lilith, SE_MEAN_APOG }
		};

		const auto a = objectMapping.find( ob );
		if ( a != objectMapping.end() ) return a->second;
		else fatallog << "CalculatorSwe: wrong PlanetId " << ob << ", exit";
		assert( false );
		return -1;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   getHouseSystemCode
	**
	******************************************************/
	int getHouseSystemCode( const HouseSystem &hsys )
	{
		int c = 'P';
		switch ( hsys )
		{
		case HouseSystem::None:
		case HouseSystem::Placidus:
			c = 'P';
			break;
		case HouseSystem::Koch:
			c = 'K';
			break;
		case HouseSystem::Regiomontanus:
			c = 'R';
			break;
		case HouseSystem::Campanus:
			c = 'C';
			break;
		case HouseSystem::Porphyry:
			c = 'O';
			break;
		case HouseSystem::Equal:
			c = 'E';
			break;
		case HouseSystem::VehlowEqual:
			c = 'V';
			break;
		case HouseSystem::MeridianAxialRotation:
			c = 'X';
			break;
		case HouseSystem::AzimutalHorizontal:
			c = 'H';
			break;
		case HouseSystem::PolichPage:
			c = 'T';
			break;
		case HouseSystem::Alcabitus:
			c = 'B';
			break;
		case HouseSystem::Morinus:
			c = 'M';
			break;
		case HouseSystem::Krusinski:
			c = 'U';
			break;
		default:
			assert( false );
			break;
		}
		return c;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   CalcDateForAscendantPos
	**
	******************************************************/
	double CalcDateForAscendantPos( DateLocation *d, const double &dest, const Viewpoint &viewpoint )
	{
		double diff, alen, speed;
		int count = 0;
		DateLocation dataset( *d );
		const Location &loc = dataset.location;
		const double lat = loc.latitude;
		const double lon = loc.longitude;
		speed = 720;
		do
		{
			alen = calcAscendant( dataset.date.jd, lat, lon, viewpoint );

			diff = alen - dest;
			if ( diff > 200 ) diff -= 360;
			if ( diff < -200 ) diff += 360;
			dataset.date.jd -= diff / speed;
		}
		while (( fabs( diff ) > 0.0001 ) && ( count++ < 40 ));

		if ( count > 39 ) warnlog << "Too many iterations for calculation of ascecndant longitude";
		return dataset.date.jd;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   CalcDateForMoonPos
	**
	******************************************************/
	double CalcDateForMoonPos( DateLocation *d, const double &dest, const Viewpoint &viewpoint )
	{
		double diff, mlen, dummy, mspeed;
		int count = 0;
		DateLocation dl( *d );

		do
		{
			calcPositionSpeed( dl, PlanetId::Moon, mlen, dummy, mspeed, viewpoint );

			diff = mlen - dest;
			if ( diff > 200 ) diff -= 360;
			if ( diff < -200 ) diff += 360;
			dl.date.jd -=  diff / mspeed;
		}
		while (( fabs( diff ) > 0.0000001 ) && ( count++ < 20 ));

		if ( count > 19 ) warnlog << "Too many iterations for calculation of moon longitude";
		return dl.date.jd;
	}

	/*****************************************************
	**
	**   CalculatorSwe   ---   calcIFlag
	**
	******************************************************/
	long calcIFlag( const DateLocation &dl )
	{
		long iflag = 0;
		if ( astroconfig->ephem.useTruePositions ) iflag |= SEFLG_TRUEPOS;
		switch ( astroconfig->ephem.ephemCoordinateSystem )
		{
			case EphemCoordinateMode::Geocentric:
				// do nothing
			break;
			case EphemCoordinateMode::Topocentric:
				swe_set_topo( dl.location.longitude, dl.location.latitude, 0 );
				iflag |= SEFLG_TOPOCTR;
			break;
			case EphemCoordinateMode::Equatorial:
				iflag |= SEFLG_EQUATORIAL;
			break;
			case EphemCoordinateMode::Barycentric:
				iflag |= SEFLG_BARYCTR;
			break;
			case EphemCoordinateMode::Heliocentric:
				iflag |= SEFLG_HELCTR ;
			break;
			default:
				errorlog << "invalid ephemeris coordinate mode " << (int)astroconfig->ephem.ephemCoordinateSystem;
				assert( false );
			break;
		}
		return iflag;
	}

	// -3000-1-1
	virtual double getMinJD() const override { return 625307.0; }

	// 3000-12-31 -> jd  2817151
	virtual double getMaxJD() const override { return 2817151.0; }
};

Calculator *CalculatorFactory::calculator = nullptr;

/*****************************************************
**
**   CalculatorFactory   ---   getCalculator
**
******************************************************/
Calculator *CalculatorFactory::getCalculator()
{
	if ( calculator == 0 )
	{
		calculator = new CalculatorSwe();
		calculator->init();
	}
	return calculator;
}



