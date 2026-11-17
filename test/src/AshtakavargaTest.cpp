/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       test/src/AshtakavargaTest.cpp
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
#include "ContextProperties.h"
#include "enumserializer.h"
#include "Horoscope.h"
#include "Lang.h"
#include "mathbase.h"
#include "TestRunner.h"
#include "Varga.h"

extern Astroconfig *astroconfig;


/**
* Test chart according to the book 'Ashtakavarga' by Patel and Aiyar. House system is Porphyry.
*
* The values below are related to the sample chart and can be found in tabular form on p.12. 
*/
TEST_F( SingleThreadedTests, AshtakavargaBhavaTest )
{
	// the values in the book are manually calculated and 1 minute tolerance is too sharp
	constexpr double TOL = 1 / 45.0;
	constexpr Viewpoint viewpoint = Viewpoint::Vedic;

	CalculationConfig &calccfg = astroconfig->getViewpointConfig( viewpoint ).calculation;
	ASSERT_EQ( calccfg.houseSystem, HouseSystem::Placidus );
	calccfg.houseSystem = HouseSystem::Porphyry;

	Horoscope h;
	h.openFile( AVTEST_MTXFILE );
	h.update();

	EXPECT_NEAR( h.getHouseCusp( House::h1, viewpoint ), gen_longitude( 2, 4, 37 ), TOL );
	EXPECT_NEAR( h.getHouseSandhi( House::h1, viewpoint ), gen_longitude( 2, 18, 39 ), TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h2, viewpoint ), gen_longitude( 3, 2, 41 ), TOL );
	EXPECT_NEAR( h.getHouseSandhi( House::h2, viewpoint ), gen_longitude( 3, 16, 44 ), TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h3, viewpoint ), gen_longitude( 4, 0, 46 ), TOL );
	EXPECT_NEAR( h.getHouseSandhi( House::h3, viewpoint ), gen_longitude( 4, 14, 48 ), TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h4, viewpoint ), gen_longitude( 4, 28, 50 ), TOL );
	EXPECT_NEAR( h.getHouseSandhi( House::h4, viewpoint ), gen_longitude( 5, 14, 48 ), TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h5, viewpoint ), gen_longitude( 6, 0, 46 ), TOL );
	EXPECT_NEAR( h.getHouseSandhi( House::h5, viewpoint ), gen_longitude( 6, 16, 44 ), TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h6, viewpoint ), gen_longitude( 7, 2, 41 ), TOL );
	EXPECT_NEAR( h.getHouseSandhi( House::h6, viewpoint ), gen_longitude( 7, 18, 39 ), TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h7, viewpoint ), gen_longitude( 8, 4, 37 ), TOL );
	EXPECT_NEAR( h.getHouseSandhi( House::h7, viewpoint ), gen_longitude( 8, 18, 39 ), TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h8, viewpoint ), gen_longitude( 9, 2, 41 ), TOL );
	EXPECT_NEAR( h.getHouseSandhi( House::h8, viewpoint ), gen_longitude( 9, 16, 44 ), TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h9, viewpoint ), gen_longitude( 10, 0, 46 ), TOL );
	EXPECT_NEAR( h.getHouseSandhi( House::h9, viewpoint ), gen_longitude( 10, 14, 48 ), TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h10, viewpoint ), gen_longitude( 10, 28, 50 ), TOL );
	EXPECT_NEAR( h.getHouseSandhi( House::h10, viewpoint ), gen_longitude( 11, 14, 48 ), TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h11, viewpoint ), gen_longitude( 0, 0, 46 ), TOL );
	EXPECT_NEAR( h.getHouseSandhi( House::h11, viewpoint ), gen_longitude( 0, 16, 44 ), TOL );

	EXPECT_NEAR( h.getHouseCusp( House::h12, viewpoint ), gen_longitude( 1, 2, 41 ), TOL );
	EXPECT_NEAR( h.getHouseSandhi( House::h12, viewpoint ), gen_longitude( 1, 18, 39 ), TOL );

	// reset
	calccfg.houseSystem = HouseSystem::Placidus;
}

/**
* Test chart according to the book 'Ashtakavarga' by Patel and Aiyar. House system is Porphyry, Varga is Bhava.
*
* The values below are related to the sample chart and can be found on p. 32ff.
*/
TEST_F( SingleThreadedTests, AshtakavargaTest )
{
	constexpr Viewpoint viewpoint = Viewpoint::Vedic;
	constexpr Varga varga = Varga::Bhava;
	//AshtakavargaConfig &avcfg = astroconfig->ashtakavarga;
	// ashtakavargaMode = parasara/varahamihira

	CalculationConfig &calccfg = astroconfig->getViewpointConfig( viewpoint ).calculation;
	ASSERT_EQ( calccfg.houseSystem, HouseSystem::Placidus );
	calccfg.houseSystem = HouseSystem::Porphyry;

	ASSERT_EQ( calccfg.houseStart, HouseStartMode::Cusp );
	calccfg.houseStart = HouseStartMode::Middle;

	Horoscope_ptr h( new Horoscope());
	ContextProps_ptr props( new ContextProperties());

	props->setRadixHoroscope( h );
	props->setPreferredViewpoint( viewpoint );
	h->openFile( AVTEST_MTXFILE );
	h->update();

	AshtakavargaExpert expert( props, varga );
	expert.update();
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Sun, Sign::Aries ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Sun, Sign::Taurus ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Sun, Sign::Gemini ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Sun, Sign::Cancer ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Sun, Sign::Leo ), 6 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Sun, Sign::Virgo ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Sun, Sign::Libra ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Sun, Sign::Scorpio ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Sun, Sign::Sagittarius ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Sun, Sign::Capricorn ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Sun, Sign::Aquarius ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Sun, Sign::Pisces ), 6 );

	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Moon, Sign::Aries ), 7 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Moon, Sign::Taurus ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Moon, Sign::Gemini ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Moon, Sign::Cancer ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Moon, Sign::Leo ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Moon, Sign::Virgo ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Moon, Sign::Libra ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Moon, Sign::Scorpio ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Moon, Sign::Sagittarius ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Moon, Sign::Capricorn ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Moon, Sign::Aquarius ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Moon, Sign::Pisces ), 5 );

	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mars, Sign::Aries ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mars, Sign::Taurus ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mars, Sign::Gemini ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mars, Sign::Cancer ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mars, Sign::Leo ), 6 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mars, Sign::Virgo ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mars, Sign::Libra ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mars, Sign::Scorpio ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mars, Sign::Sagittarius ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mars, Sign::Capricorn ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mars, Sign::Aquarius ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mars, Sign::Pisces ), 5 );

	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mercury, Sign::Aries ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mercury, Sign::Taurus ), 6 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mercury, Sign::Gemini ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mercury, Sign::Cancer ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mercury, Sign::Leo ), 7 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mercury, Sign::Virgo ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mercury, Sign::Libra ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mercury, Sign::Scorpio ), 6 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mercury, Sign::Sagittarius ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mercury, Sign::Capricorn ), 6 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mercury, Sign::Aquarius ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Mercury, Sign::Pisces ), 4 );

	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Jupiter, Sign::Aries ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Jupiter, Sign::Taurus ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Jupiter, Sign::Gemini ), 7 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Jupiter, Sign::Cancer ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Jupiter, Sign::Leo ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Jupiter, Sign::Virgo ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Jupiter, Sign::Libra ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Jupiter, Sign::Scorpio ), 6 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Jupiter, Sign::Sagittarius ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Jupiter, Sign::Capricorn ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Jupiter, Sign::Aquarius ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Jupiter, Sign::Pisces ), 5 );

	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Venus, Sign::Aries ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Venus, Sign::Taurus ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Venus, Sign::Gemini ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Venus, Sign::Cancer ), 6 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Venus, Sign::Leo ), 6 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Venus, Sign::Virgo ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Venus, Sign::Libra ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Venus, Sign::Scorpio ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Venus, Sign::Sagittarius ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Venus, Sign::Capricorn ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Venus, Sign::Aquarius ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Venus, Sign::Pisces ), 1 );

	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Saturn, Sign::Aries ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Saturn, Sign::Taurus ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Saturn, Sign::Gemini ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Saturn, Sign::Cancer ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Saturn, Sign::Leo ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Saturn, Sign::Virgo ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Saturn, Sign::Libra ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Saturn, Sign::Scorpio ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Saturn, Sign::Sagittarius ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Saturn, Sign::Capricorn ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Saturn, Sign::Aquarius ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Saturn, Sign::Pisces ), 6 );

	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Ascendant, Sign::Aries ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Ascendant, Sign::Taurus ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Ascendant, Sign::Gemini ), 6 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Ascendant, Sign::Cancer ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Ascendant, Sign::Leo ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Ascendant, Sign::Virgo ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Ascendant, Sign::Libra ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Ascendant, Sign::Scorpio ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Ascendant, Sign::Sagittarius ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Ascendant, Sign::Capricorn ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Ascendant, Sign::Aquarius ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Rekha, PlanetId::Ascendant, Sign::Pisces ), 4 );

	// Trikona Shodana
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Sun, Sign::Aries ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Sun, Sign::Taurus ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Sun, Sign::Gemini ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Sun, Sign::Cancer ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Sun, Sign::Leo ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Sun, Sign::Virgo ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Sun, Sign::Libra ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Sun, Sign::Scorpio ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Sun, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Sun, Sign::Capricorn ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Sun, Sign::Aquarius ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Sun, Sign::Pisces ), 2 );

	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Moon, Sign::Aries ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Moon, Sign::Taurus ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Moon, Sign::Gemini ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Moon, Sign::Cancer ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Moon, Sign::Leo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Moon, Sign::Virgo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Moon, Sign::Libra ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Moon, Sign::Scorpio ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Moon, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Moon, Sign::Capricorn ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Moon, Sign::Aquarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Moon, Sign::Pisces ), 0 );

	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mars, Sign::Aries ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mars, Sign::Taurus ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mars, Sign::Gemini ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mars, Sign::Cancer ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mars, Sign::Leo ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mars, Sign::Virgo ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mars, Sign::Libra ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mars, Sign::Scorpio ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mars, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mars, Sign::Capricorn ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mars, Sign::Aquarius ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mars, Sign::Pisces ), 1 );

	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mercury, Sign::Aries ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mercury, Sign::Taurus ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mercury, Sign::Gemini ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mercury, Sign::Cancer ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mercury, Sign::Leo ), 6 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mercury, Sign::Virgo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mercury, Sign::Libra ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mercury, Sign::Scorpio ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mercury, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mercury, Sign::Capricorn ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mercury, Sign::Aquarius ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Mercury, Sign::Pisces ), 0 );

	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Jupiter, Sign::Aries ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Jupiter, Sign::Taurus ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Jupiter, Sign::Gemini ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Jupiter, Sign::Cancer ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Jupiter, Sign::Leo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Jupiter, Sign::Virgo ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Jupiter, Sign::Libra ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Jupiter, Sign::Scorpio ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Jupiter, Sign::Sagittarius ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Jupiter, Sign::Capricorn ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Jupiter, Sign::Aquarius ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Jupiter, Sign::Pisces ), 0 );

	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Venus, Sign::Aries ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Venus, Sign::Taurus ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Venus, Sign::Gemini ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Venus, Sign::Cancer ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Venus, Sign::Leo ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Venus, Sign::Virgo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Venus, Sign::Libra ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Venus, Sign::Scorpio ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Venus, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Venus, Sign::Capricorn ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Venus, Sign::Aquarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Venus, Sign::Pisces ), 0 );

	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Saturn, Sign::Aries ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Saturn, Sign::Taurus ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Saturn, Sign::Gemini ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Saturn, Sign::Cancer ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Saturn, Sign::Leo ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Saturn, Sign::Virgo ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Saturn, Sign::Libra ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Saturn, Sign::Scorpio ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Saturn, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Saturn, Sign::Capricorn ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Saturn, Sign::Aquarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Saturn, Sign::Pisces ), 4 );

	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Ascendant, Sign::Aries ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Ascendant, Sign::Taurus ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Ascendant, Sign::Gemini ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Ascendant, Sign::Cancer ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Ascendant, Sign::Leo ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Ascendant, Sign::Virgo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Ascendant, Sign::Libra ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Ascendant, Sign::Scorpio ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Ascendant, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Ascendant, Sign::Capricorn ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Ascendant, Sign::Aquarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Trikona, PlanetId::Ascendant, Sign::Pisces ), 0 );

	// Ekadipatya Shodana 
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Sun, Sign::Aries ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Sun, Sign::Taurus ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Sun, Sign::Gemini ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Sun, Sign::Cancer ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Sun, Sign::Leo ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Sun, Sign::Virgo ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Sun, Sign::Libra ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Sun, Sign::Scorpio ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Sun, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Sun, Sign::Capricorn ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Sun, Sign::Aquarius ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Sun, Sign::Pisces ), 2 );

	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Moon, Sign::Aries ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Moon, Sign::Taurus ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Moon, Sign::Gemini ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Moon, Sign::Cancer ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Moon, Sign::Leo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Moon, Sign::Virgo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Moon, Sign::Libra ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Moon, Sign::Scorpio ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Moon, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Moon, Sign::Capricorn ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Moon, Sign::Aquarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Moon, Sign::Pisces ), 0 );

	/* total falsch
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mars, Sign::Aries ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mars, Sign::Taurus ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mars, Sign::Gemini ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mars, Sign::Cancer ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mars, Sign::Leo ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mars, Sign::Virgo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mars, Sign::Libra ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mars, Sign::Scorpio ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mars, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mars, Sign::Capricorn ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mars, Sign::Aquarius ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mars, Sign::Pisces ), 3 );
	*/

	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mercury, Sign::Aries ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mercury, Sign::Taurus ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mercury, Sign::Gemini ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mercury, Sign::Cancer ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mercury, Sign::Leo ), 6 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mercury, Sign::Virgo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mercury, Sign::Libra ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mercury, Sign::Scorpio ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mercury, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mercury, Sign::Capricorn ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mercury, Sign::Aquarius ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Mercury, Sign::Pisces ), 0 );

	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Jupiter, Sign::Aries ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Jupiter, Sign::Taurus ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Jupiter, Sign::Gemini ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Jupiter, Sign::Cancer ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Jupiter, Sign::Leo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Jupiter, Sign::Virgo ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Jupiter, Sign::Libra ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Jupiter, Sign::Scorpio ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Jupiter, Sign::Sagittarius ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Jupiter, Sign::Capricorn ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Jupiter, Sign::Aquarius ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Jupiter, Sign::Pisces ), 0 );

	//EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Venus, Sign::Aries ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Venus, Sign::Taurus ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Venus, Sign::Gemini ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Venus, Sign::Cancer ), 5 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Venus, Sign::Leo ), 4 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Venus, Sign::Virgo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Venus, Sign::Libra ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Venus, Sign::Scorpio ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Venus, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Venus, Sign::Capricorn ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Venus, Sign::Aquarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Venus, Sign::Pisces ), 0 );

	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Saturn, Sign::Aries ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Saturn, Sign::Taurus ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Saturn, Sign::Gemini ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Saturn, Sign::Cancer ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Saturn, Sign::Leo ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Saturn, Sign::Virgo ), 3 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Saturn, Sign::Libra ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Saturn, Sign::Scorpio ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Saturn, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Saturn, Sign::Capricorn ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Saturn, Sign::Aquarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Saturn, Sign::Pisces ), 4 );

	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Ascendant, Sign::Aries ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Ascendant, Sign::Taurus ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Ascendant, Sign::Gemini ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Ascendant, Sign::Cancer ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Ascendant, Sign::Leo ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Ascendant, Sign::Virgo ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Ascendant, Sign::Libra ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Ascendant, Sign::Scorpio ), 1 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Ascendant, Sign::Sagittarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Ascendant, Sign::Capricorn ), 2 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Ascendant, Sign::Aquarius ), 0 );
	EXPECT_EQ( expert.getItem( AvType::Ekadhi, PlanetId::Ascendant, Sign::Pisces ), 0 );

	// Pindas
	EXPECT_EQ( expert.getPinda( PindaType::Rasi, PlanetId::Sun ), 142 );
	EXPECT_EQ( expert.getPinda( PindaType::Graha, PlanetId::Sun ), 62 );
	EXPECT_EQ( expert.getPinda( PindaType::Yoga, PlanetId::Sun ), 204 );

	EXPECT_EQ( expert.getPinda( PindaType::Rasi, PlanetId::Moon ), 62 );
	EXPECT_EQ( expert.getPinda( PindaType::Graha, PlanetId::Moon ), 32 );
	EXPECT_EQ( expert.getPinda( PindaType::Yoga, PlanetId::Moon ), 94 );

	EXPECT_EQ( expert.getPinda( PindaType::Rasi, PlanetId::Mars ), 88 );
	EXPECT_EQ( expert.getPinda( PindaType::Graha, PlanetId::Mars ), 65 );
	EXPECT_EQ( expert.getPinda( PindaType::Yoga, PlanetId::Mars ), 153 );

	EXPECT_EQ( expert.getPinda( PindaType::Rasi, PlanetId::Mercury ), 117 );
	EXPECT_EQ( expert.getPinda( PindaType::Graha, PlanetId::Mercury ), 53 );
	EXPECT_EQ( expert.getPinda( PindaType::Yoga, PlanetId::Mercury ), 170 );

	EXPECT_EQ( expert.getPinda( PindaType::Rasi, PlanetId::Jupiter ), 76 );
	EXPECT_EQ( expert.getPinda( PindaType::Graha, PlanetId::Jupiter ), 20 );
	EXPECT_EQ( expert.getPinda( PindaType::Yoga, PlanetId::Jupiter ), 96 );

	// TODO maitreya rechnet 120
	//EXPECT_EQ( expert.getPinda( PindaType::Rasi, PlanetId::Venus ), 106 );
	EXPECT_EQ( expert.getPinda( PindaType::Graha, PlanetId::Venus ), 42 );
	//EXPECT_EQ( expert.getPinda( PindaType::Yoga, PlanetId::Venus ), 148 );

	EXPECT_EQ( expert.getPinda( PindaType::Rasi, PlanetId::Saturn ), 107 );
	EXPECT_EQ( expert.getPinda( PindaType::Graha, PlanetId::Saturn ), 46 );
	EXPECT_EQ( expert.getPinda( PindaType::Yoga, PlanetId::Saturn ), 153 );

	EXPECT_EQ( expert.getPinda( PindaType::Rasi, PlanetId::Ascendant ), 51 );
	EXPECT_EQ( expert.getPinda( PindaType::Graha, PlanetId::Ascendant ), 40 );
	EXPECT_EQ( expert.getPinda( PindaType::Yoga, PlanetId::Ascendant ), 91 );

	// reset
	calccfg.houseSystem = HouseSystem::Placidus;
	calccfg.houseStart = HouseStartMode::Cusp;
}


