/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       test/src/EphemTest.cpp
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

#include "Ashtakavarga.h"
#include "astroconfig.h"
#include "Calculator.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "TestRunner.h"

#include <wx/dir.h>
#include <wx/filename.h>

extern Astroconfig *astroconfig;


/**
* Uranian planets compared to Astrolog
*/
TEST( Ephemeris, UranianPlanets )
{
	constexpr Viewpoint viewpoint = Viewpoint::Western;

	Horoscope h;
	h.openFile( TEST2_MTXFILE );
	h.update();

	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Cupido ), gen_longitude( Sign::Libra, 10, 57, 47 ), SEC_TOL );
	EXPECT_EQ( h.getDirection( PlanetId::Cupido, viewpoint ), MovingDirection::Direct );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Hades ), gen_longitude( Sign::Taurus, 11, 45, 1 ), SEC_TOL );
	EXPECT_EQ( h.getDirection( PlanetId::Hades, viewpoint ), MovingDirection::Retrograde );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Zeus ), gen_longitude( Sign::Virgo, 4, 49, 7 ), SEC_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Kronos ), gen_longitude( Sign::Gemini, 2, 40, 13 ), SEC_TOL );

	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Apollon ), gen_longitude( Sign::Virgo, 27, 10, 28 ), SEC_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Admetos ), gen_longitude( Sign::Aries, 28, 23, 7 ), SEC_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Vulkanus ), gen_longitude( Sign::Cancer, 0, 45, 18 ), SEC_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Poseidon ), gen_longitude( Sign::Libra, 15, 37, 35 ), SEC_TOL );
}

/*
TEST( Ephemeris, Minors )
{
	Formatter *formatter = Formatter::get();
	constexpr Viewpoint viewpoint = Viewpoint::Western;

	Horoscope h;
	h.openFile( TEST2_MTXFILE );
	h.update();

	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Chiron ), gen_longitude( Sign::Pisces, 7, 43 ), DEG_TOL );
	//EXPECT_NEAR( h.getWesternLongitude( PlanetId::Pholus ), gen_longitude( Sign::Cancer, 2, 13 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Ceres ), gen_longitude( Sign::Leo, 13, 58 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Pallas ), gen_longitude( Sign::Cancer, 2, 13 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Juno ), gen_longitude( Sign::Leo, 20, 48 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Vesta ), gen_longitude( Sign::Leo, 19, 48 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Lilith ), gen_longitude( Sign::Libra, 5, 55 ), DEG_TOL );
}
*/

/**
* Houses compared to Astrolog
*/
TEST( Ephemeris, Houses )
{
	constexpr Viewpoint viewpoint = Viewpoint::Western;
	CalculationConfig &calccfg = astroconfig->getViewpointConfig( viewpoint ).calculation;

	Horoscope h;
	h.openFile( TEST2_MTXFILE );
	h.update();

  ASSERT_EQ( calccfg.houseSystem, HouseSystem::Placidus );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Ascendant ), gen_longitude( Sign::Scorpio, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Descendant ), gen_longitude( Sign::Taurus, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Meridian ), gen_longitude( Sign::Virgo, 6, 13 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::ImumCoeli ), gen_longitude( Sign::Pisces, 6, 13 ), DEG_TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h1, viewpoint ), gen_longitude( Sign::Scorpio, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h2, viewpoint ), gen_longitude( Sign::Sagittarius, 17, 3 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h3, viewpoint ), gen_longitude( Sign::Capricorn, 26, 4 ), DEG_TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h10, viewpoint ), gen_longitude( Sign::Virgo, 6, 13 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h11, viewpoint ), gen_longitude( Sign::Libra, 7, 20 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h12, viewpoint ), gen_longitude( Sign::Libra, 29, 55 ), DEG_TOL );

  calccfg.houseSystem = HouseSystem::Koch;
	h.update();
	EXPECT_NEAR( h.getHouseCusp( House::h1, viewpoint ), gen_longitude( Sign::Scorpio, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h2, viewpoint ), gen_longitude( Sign::Sagittarius, 11, 38 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h3, viewpoint ), gen_longitude( Sign::Capricorn, 11, 48 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h4, viewpoint ), gen_longitude( Sign::Pisces, 6, 13 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h5, viewpoint ), gen_longitude( Sign::Pisces, 29, 55 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h6, viewpoint ), gen_longitude( Sign::Aries, 23, 36 ), DEG_TOL );

  calccfg.houseSystem = HouseSystem::Regiomontanus;
	h.update();
	EXPECT_NEAR( h.getHouseCusp( House::h1, viewpoint ), gen_longitude( Sign::Scorpio, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h2, viewpoint ), gen_longitude( Sign::Sagittarius, 12, 7 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h3, viewpoint ), gen_longitude( Sign::Capricorn, 21, 15 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h4, viewpoint ), gen_longitude( Sign::Pisces, 6, 13 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h5, viewpoint ), gen_longitude( Sign::Aries, 6, 48 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h6, viewpoint ), gen_longitude( Sign::Aries, 27, 45 ), DEG_TOL );

  calccfg.houseSystem = HouseSystem::Campanus;
	h.update();
	EXPECT_NEAR( h.getHouseCusp( House::h1, viewpoint ), gen_longitude( Sign::Scorpio, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h2, viewpoint ), gen_longitude( Sign::Sagittarius, 27, 31 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h3, viewpoint ), gen_longitude( Sign::Aquarius, 7, 58 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h4, viewpoint ), gen_longitude( Sign::Pisces, 6, 13 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h5, viewpoint ), gen_longitude( Sign::Pisces, 27, 37 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h6, viewpoint ), gen_longitude( Sign::Aries, 18, 59 ), DEG_TOL );

  calccfg.houseSystem = HouseSystem::Porphyry;
	h.update();
	EXPECT_NEAR( h.getHouseCusp( House::h1, viewpoint ), gen_longitude( Sign::Scorpio, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h2, viewpoint ), gen_longitude( Sign::Sagittarius, 23, 30 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h3, viewpoint ), gen_longitude( Sign::Capricorn, 29, 52 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h4, viewpoint ), gen_longitude( Sign::Pisces, 6, 13 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h5, viewpoint ), gen_longitude( Sign::Pisces, 29, 52 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h6, viewpoint ), gen_longitude( Sign::Aries, 23, 30 ), DEG_TOL );

  calccfg.houseSystem = HouseSystem::Equal;
	h.update();
	EXPECT_NEAR( h.getHouseCusp( House::h1, viewpoint ), gen_longitude( Sign::Scorpio, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h2, viewpoint ), gen_longitude( Sign::Sagittarius, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h3, viewpoint ), gen_longitude( Sign::Capricorn, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h4, viewpoint ), gen_longitude( Sign::Aquarius, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h5, viewpoint ), gen_longitude( Sign::Pisces, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h6, viewpoint ), gen_longitude( Sign::Aries, 17, 9 ), DEG_TOL );

  calccfg.houseSystem = HouseSystem::MeridianAxialRotation; // default for uranian
	h.update();
	EXPECT_NEAR( h.getHouseCusp( House::h1, viewpoint ), gen_longitude( Sign::Sagittarius, 9, 39 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h2, viewpoint ), gen_longitude( Sign::Capricorn, 7, 20 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h3, viewpoint ), gen_longitude( Sign::Aquarius, 5, 37 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h4, viewpoint ), gen_longitude( Sign::Pisces, 6, 13 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h5, viewpoint ), gen_longitude( Sign::Aries, 8, 42 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h6, viewpoint ), gen_longitude( Sign::Taurus, 10, 24 ), DEG_TOL );

  calccfg.houseSystem = HouseSystem::Alcabitus;
	h.update();
	EXPECT_NEAR( h.getHouseCusp( House::h1, viewpoint ), gen_longitude( Sign::Scorpio, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h2, viewpoint ), gen_longitude( Sign::Sagittarius, 23, 4 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h3, viewpoint ), gen_longitude( Sign::Capricorn, 28, 7 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h4, viewpoint ), gen_longitude( Sign::Pisces, 6, 13 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h5, viewpoint ), gen_longitude( Sign::Aries, 0, 14 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h6, viewpoint ), gen_longitude( Sign::Aries, 24, 15 ), DEG_TOL );

  calccfg.houseSystem = HouseSystem::Morinus;
	h.update();
	EXPECT_NEAR( h.getHouseCusp( House::h1, viewpoint ), gen_longitude( Sign::Sagittarius, 6, 13 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h2, viewpoint ), gen_longitude( Sign::Capricorn, 8, 42 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h3, viewpoint ), gen_longitude( Sign::Aquarius, 10, 24 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h4, viewpoint ), gen_longitude( Sign::Pisces, 9, 39 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h5, viewpoint ), gen_longitude( Sign::Aries, 7, 20 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h6, viewpoint ), gen_longitude( Sign::Taurus, 5, 37 ), DEG_TOL );

  calccfg.houseSystem = HouseSystem::Krusinski;
	h.update();
	EXPECT_NEAR( h.getHouseCusp( House::h1, viewpoint ), gen_longitude( Sign::Scorpio, 17, 9 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h2, viewpoint ), gen_longitude( Sign::Sagittarius, 8, 3 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h3, viewpoint ), gen_longitude( Sign::Capricorn, 14, 26 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h4, viewpoint ), gen_longitude( Sign::Pisces, 6, 13 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h5, viewpoint ), gen_longitude( Sign::Aries, 10, 17 ), DEG_TOL );
	EXPECT_NEAR( h.getHouseCusp( House::h6, viewpoint ), gen_longitude( Sign::Taurus, 0, 20 ), DEG_TOL );

	// reset
  calccfg.houseSystem = HouseSystem::Placidus;
}

/**
* Arabic Parts: reference is Astrolog https://www.astrolog.org
*/
TEST( Ephemeris, ArabicParts )
{
	Horoscope h;
	h.openFile( TEST2_MTXFILE );
	h.update();

	// Fortune - Spirit - Victory
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::FirstArabic ), gen_longitude( Sign::Cancer, 3, 6 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( (PlanetId)((int)PlanetId::FirstArabic + 1 )), gen_longitude( Sign::Aries, 1, 12 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( (PlanetId)((int)PlanetId::FirstArabic + 2 )), gen_longitude( Sign::Libra, 21, 31 ), DEG_TOL );
}


TEST_F( SingleThreadedTests, AdvancedEphemTest )
{
	EphemConfig &epcfg = astroconfig->ephem;
	epcfg.ephemCoordinateSystem = EphemCoordinateMode::Topocentric;

	Horoscope h;
	h.openFile( TEST2_MTXFILE );
	h.update();

	// TODO: how to get reference data

	astroconfig->ephem = EphemConfig();
}

/**
* Test with Swiss Ephem data files. 2 tests are included:
* - Use case 1: Test for 1980-10-1 DE200, expect Sun and Moon to be at the given positions within 1 second
* - Use case 2: test against printed ephem for 1980-9-27. Source Astron Ephemeris 1950-2050 for Ceres, Pallas Juno, Cheiron.
*
*  The swiss ephem data files directory must be set and files must be available!
*
* Sample output of "ls -l" directory:

-rw-r--r--  1 mp  223002 16. Mai 2020  seas_18.se1
-rw-r--r--  1 mp 1304771 16. Mai 2020  semo_18.se1
-rw-r--r--  1 mp  484055 16. Mai 2020  sepl_18.se1

*/
TEST_F( SingleThreadedTests, EphemDatafileTest )
{
	const static char *errorText = "EphemConfig.sePath contains the path to Swiss Ephemeris Data files. It can be configured in"
	" the config dialog in the Ephemeris section. Required files for this test are: seas_18.se1, semo_18.se1, sepl_18.se1";
	// astroconfig is loaded because Swiss Ephem data file directory is required.
	astroconfig->load();
	EphemConfig &epcfg = astroconfig->ephem;
	epcfg.useEphemerisTime = 1;
	epcfg.sePath = "swe";

	ASSERT_FALSE( epcfg.sePath.empty()) << "Ephem path is empty. " << errorText;
	ASSERT_TRUE( wxFileName::DirExists( epcfg.sePath )) << "Ephem path " << epcfg.sePath << " not a directory. " << errorText;

	// see if at least one file *.se1 can be found
	const wxString filespec = "*.se1";
	wxArrayString filelist;
	const int filecount = wxDir::GetAllFiles( epcfg.sePath, &filelist, filespec );
	debuglog << "EphemDatafileTest filecount " << filecount;
	ASSERT_GT( filelist.size(), 0u ) << "Ephem path " << epcfg.sePath << " contains no matching files *.se1. " << errorText;

	Calculator *calculator = CalculatorFactory().getCalculator();
	calculator->init();

	Horoscope h;

	// Use case 1
	h.openFile( TEST1_MTXFILE );
	h.update();

	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Sun ), gen_longitude( Sign::Libra, 7, 57, 48 ), SEC_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Moon ), gen_longitude( Sign::Cancer, 6, 15, 57 ), SEC_TOL );

	// Use case 2
	h.dl.date.set( 1980, 9, 27, 0 );
	h.update();
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Ceres ), gen_longitude( Sign::Cancer, 17, 25 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Pallas ), gen_longitude( Sign::Taurus, 3, 3 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Juno ), gen_longitude( Sign::Virgo, 28, 17 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Vesta ), gen_longitude( Sign::Leo, 13, 59 ), DEG_TOL );
	EXPECT_NEAR( h.getWesternLongitude( PlanetId::Chiron ), gen_longitude( Sign::Taurus, 17, 41 ), DEG_TOL );

	// reset astroconfig and re-init Calculator
	*astroconfig = Astroconfig();
	calculator->init();
}



