/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       test/src/DE200Test.cpp
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

#include "astroconfig.h"
#include "ContextProperties.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "Ephemeris.h"
#include "Horoscope.h"
#include "TestRunner.h"

#include <iomanip>

extern Astroconfig *astroconfig;

class DE200BaseTest : public SingleThreadedTests {};

/**
* Test for comparision with old printed Rosicrucian Ephemeris (based on JPL DE200)
*
* Example date is 1980-10-01 0:00 UT
*
* The ephem uses ephemeris time, so the test must run in a single thread.
*
*/
TEST_F( DE200BaseTest, BasicDE200Test )
{
	//Formatter *formatter = Formatter::get();
	constexpr Viewpoint viewpoint = Viewpoint::Western;
	CalculationConfig &calccfg = astroconfig->getViewpointConfig( viewpoint ).calculation;
	EphemConfig &ephemcfg = astroconfig->ephem;
	ephemcfg.useEphemerisTime = 1;

	Horoscope h;
	const double jd = MDate( 1980, 10, 1, 0 ).jd;
	h.setDate( jd );
	EXPECT_DOUBLE_EQ( h.getJD(), 2444513.5 );

	h.openFile( TEST1_MTXFILE );
	EXPECT_DOUBLE_EQ( h.getJD(), 2444513.5 );
	h.update();

	EXPECT_NEAR( h.getSiderealTime(), gen_degree( 0, 39, 31 ), SEC_TOL );

	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Sun ), gen_longitude( Sign::Libra, 7, 57, 48 ), SEC_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Moon ), gen_longitude( Sign::Cancer, 6, 15, 57 ), SEC_TOL );

	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Mercury ), gen_longitude( Sign::Scorpio, 1, 14 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Venus ), gen_longitude( Sign::Leo, 25, 28 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Mars ), gen_longitude( Sign::Scorpio, 22, 04 ), DEG_TOL );

	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Jupiter ), gen_longitude( Sign::Virgo, 24, 33 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Saturn ), gen_longitude( Sign::Libra, 1, 11 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Uranus ), gen_longitude( Sign::Scorpio, 23, 6 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Neptune ), gen_longitude( Sign::Sagittarius, 20, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Pluto ), gen_longitude( Sign::Libra, 21, 9 ), DEG_TOL );

	ASSERT_EQ( calccfg.lunarNodeMode, LunarNodeMode::True );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::AscNode ), gen_longitude( Sign::Leo, 19, 3 ), DEG_TOL );

	calccfg.lunarNodeMode = LunarNodeMode::Mean;
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::AscNode ), gen_longitude( Sign::Leo, 17, 23 ), DEG_TOL );
	calccfg.lunarNodeMode = LunarNodeMode::True;

	// declinations
	EXPECT_NEAR( h.getDeclination( PlanetId::Sun, viewpoint ), gen_degree( -3, 10 ), DEG_TOL );
	EXPECT_NEAR( h.getDeclination( PlanetId::Moon, viewpoint ), gen_degree( 19, 48 ), DEG_TOL );
	EXPECT_NEAR( h.getDeclination( PlanetId::Mercury, viewpoint ), gen_degree( -13, 40 ), DEG_TOL );
	EXPECT_NEAR( h.getDeclination( PlanetId::Venus, viewpoint ), gen_degree( 13, 0 ), DEG_TOL );
	EXPECT_NEAR( h.getDeclination( PlanetId::Mars, viewpoint ), gen_degree( -18, 57 ), DEG_TOL );
	EXPECT_NEAR( h.getDeclination( PlanetId::Jupiter, viewpoint ), gen_degree( 3, 8 ), DEG_TOL );
	EXPECT_NEAR( h.getDeclination( PlanetId::Saturn, viewpoint ), gen_degree( 1, 26 ), DEG_TOL );
	EXPECT_NEAR( h.getDeclination( PlanetId::Uranus, viewpoint ), gen_degree( -18, 19 ), DEG_TOL );
	EXPECT_NEAR( h.getDeclination( PlanetId::Neptune, viewpoint ), gen_degree( -21, 44 ), DEG_TOL );
	EXPECT_NEAR( h.getDeclination( PlanetId::Pluto, viewpoint ), gen_degree( 7, 10 ), DEG_TOL );

	// reset
	ephemcfg.useEphemerisTime = 0;
}

/**
* Find Lunar phases events for October 1980 and compare them with Lunar phases table in Rosicrucian Ephemeris.
*
* The Ephemeris has only the 4 major events new/90/full/270
*/
TEST_F( DE200BaseTest, DE200LunarPhasesTest )
{
	constexpr Viewpoint viewpoint = Viewpoint::Western;
	constexpr double TOL = 1 / ( 24.0 * 60.0 ); // 1 minute of time

	//ViewpointConfig &vpcfg = astroconfig->getViewpointConfig( viewpoint );
	EphemConfig &ephemcfg = astroconfig->ephem;
	ephemcfg.useEphemerisTime = 1;

	ContextProps_ptr props( new ContextProperties );
	props->setPreferredViewpoint( viewpoint );
	EphemExpert expert( props );
	expert.prepareMonth( 1980, 10 );

	expert.calculateLunar();
	int index = 0;
	bool found_270_1 = false;
	bool found_new = false;
	bool found_90 = false;
	bool found_full = false;
	bool found_270_2 = false;
	for( const auto &event : expert.getLunarEvents())
	{
		std::cout << "Lunar Event Date " << index << " date " << DateFormatter::get()->formatUTDatetime( event.jd )
			<< " angle " << event.angle << std::endl;

		if ( event.angle == 270 && index == 0 ) // happens twice
		{
			found_270_1 = true;
			EXPECT_NEAR( event.jd, gen_date( 1980, 10, 1, 3, 19 ), TOL );
			EXPECT_NEAR( event.mlen, gen_longitude( Sign::Cancer, 8, 6 ), DEG_TOL );
		}
		else if ( event.angle == 0 )
		{
			found_new = true;
			EXPECT_NEAR( event.jd, gen_date( 1980, 10, 9, 2, 50 ), TOL );
			EXPECT_NEAR( event.mlen, gen_longitude( Sign::Libra, 15, 58 ), DEG_TOL );
		}
		else if ( event.angle == 90 )
		{
			found_90 = true;
			EXPECT_NEAR( event.jd, gen_date( 1980, 10, 17, 3, 48 ), TOL );
			EXPECT_NEAR( event.mlen, gen_longitude( Sign::Capricorn, 23, 56 ), DEG_TOL );
		}
		else if ( event.angle == 180 )
		{
			found_full = true;
			EXPECT_NEAR( event.jd, gen_date( 1980, 10, 23, 20, 53 ), TOL );
			EXPECT_NEAR( event.mlen, gen_longitude( Sign::Taurus, 0, 36 ), DEG_TOL );
		}
		else if ( event.angle == 270 ) // second
		{
			found_270_2 = true;
			EXPECT_NEAR( event.jd, gen_date( 1980, 10, 30, 16, 34 ), TOL );
			EXPECT_NEAR( event.mlen, gen_longitude( Sign::Leo, 7, 25 ), DEG_TOL );
		}
		++index;
	}
	EXPECT_TRUE( found_270_1 );
	EXPECT_TRUE( found_new );
	EXPECT_TRUE( found_90 );
	EXPECT_TRUE( found_full );
	EXPECT_TRUE( found_270_2 );

	// reset
	ephemcfg.useEphemerisTime = 0;
}

/**
* Find Ingress events for october 1980 and compare them with aspectarian and Lunar ingress table in Rosicrucian Ephemeris.
*/
TEST_F( DE200BaseTest, DE200IngressTest )
{
	constexpr Viewpoint viewpoint = Viewpoint::Western;
	constexpr double TOL = 1 / ( 24.0 * 60.0 ); // 1 minute of time

	//ViewpointConfig &vpcfg = astroconfig->getViewpointConfig( viewpoint );
	EphemConfig &ephemcfg = astroconfig->ephem;
	ephemcfg.useEphemerisTime = 1;

	ContextProps_ptr props( new ContextProperties );
	props->setPreferredViewpoint( viewpoint );
	EphemExpert expert( props );
	expert.prepareMonth( 1980, 10 );

	expert.calculateIngress();
	int index = 0;
	for( const auto &event : expert.getIngressEvents())
	{
		std::cout << "Ingress Date " << index << " date " << DateFormatter::get()->formatUTDatetime( event.jd ) << std::endl;
		switch( index )
		{
			case 0: // Moon -> Leo
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 4 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 2, 19, 58 ), TOL );
			break;
			case 1: // Venus -> Virgo
				EXPECT_EQ( event.planet, PlanetId::Venus );
				EXPECT_EQ( event.which, 5 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 4, 23, 8 ), TOL );
			break;
			case 2: // Moon -> Virgo
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 5 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 5, 6, 20 ), TOL );
			break;
			case 3: // Moon -> Libra
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 6 );
				std::cout << "DIFF " << std::fixed << std::setprecision( 10 ) << event.jd << " which  " << event.which << " TOL " << TOL << std::endl;
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 7, 18, 31 ), TOL );
			break;
			case 4: // Moon -> Scorpio
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 7 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 10, 7, 16 ), TOL );
			break;
			case 5: // Mars -> Sagittarius
				EXPECT_EQ( event.planet, PlanetId::Mars );
				EXPECT_EQ( event.which, 8 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 12, 6, 27 ), TOL );
			break;
			case 6: // Moon -> Sagittarius
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 8 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 12, 19, 38 ), TOL );
			break;
			case 7: // Moon -> Capricorn
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 9 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 15, 6, 37 ), TOL );
			break;
			case 8: // Moon -> Aquarius
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 10 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 17, 14, 54 ), TOL );
			break;
			case 9: // Moon -> Pisces
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 11 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 19, 19, 32 ), TOL );
			break;
			case 10: // Moon -> Aries
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 0 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 21, 20, 44 ), TOL );
			break;
			case 11: // Sun -> Scorpio
				EXPECT_EQ( event.planet, PlanetId::Sun );
				EXPECT_EQ( event.which, 7 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 23, 6, 18 ), TOL );
			break;
			case 12: // Moon -> Taurus
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 1 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 23, 19, 56 ), TOL );
			break;
			case 13: // Moon -> Gemini
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 2 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 25, 19, 18 ), TOL );
			break;
			case 14: // Jupiter -> Libra
				EXPECT_EQ( event.planet, PlanetId::Jupiter );
				EXPECT_EQ( event.which, 6 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 27, 10, 11 ), TOL );
			break;
			case 15: // Moon -> Cancer
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 3 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 27, 21, 1 ), TOL );
			break;
			case 16: // Moon -> Leo
				EXPECT_EQ( event.planet, PlanetId::Moon );
				EXPECT_EQ( event.which, 4 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 30, 2, 39 ), TOL );
			break;
			case 17: // Venus -> Libra
				EXPECT_EQ( event.planet, PlanetId::Venus );
				EXPECT_EQ( event.which, 6 );
				EXPECT_NEAR( event.jd, gen_date( 1980, 10, 30, 10, 39 ), TOL );
			break;
			default:
				EXPECT_FALSE( false );
			break;
		}
		++index;
	}

	// reset
	ephemcfg.useEphemerisTime = 0;
}

