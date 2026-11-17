/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       test/src/UranianTest.cpp
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
#include "Direction.h"
#include "Horoscope.h"
#include "Lang.h"
#include "mathbase.h"
#include "Term.h"
#include "TestRunner.h"
#include "Varga.h"

extern Astroconfig *astroconfig;

class UranianBaseTest : public SingleThreadedTests {};

/**
* Test with an article by Alfred Witte "Noch einmal ein oder das Horoskop des Kaisers" (German Kaiser Wilhelm II) containing
* many events, solar arc and term values.
*
* Source is the book "Witte/Sporner: Der Mensch, eine Empfangsstation kosmischer Suggestion" p108 ff (german)
* available at www.witte-verlag.com.
*
* Remarks:
* - Witte uses Placidus houses and mean lunar node (later he uses Meridian house system).
* - Some values are not precises, so the tolerance is sometimes multiplied
*
*/
TEST_F( UranianBaseTest, Witte1Test )
{
	constexpr Viewpoint viewpoint = Viewpoint::Uranian;
	CalculationConfig &calccfg = astroconfig->getViewpointConfig( viewpoint ).calculation;
  calccfg.houseSystem = HouseSystem::Placidus;
	constexpr double HOUSE_TOL = .5; // houses were calculated manually, so more orbis
	constexpr double DECL_TOL = .25; // 15 minutes for declinations

	ContextProps_ptr props( new ContextProperties );
	Horoscope_ptr h( new Horoscope());
	props->setRadixHoroscope( h );
	h->openFile( WITTE1_MTXFILE );
	h->update();

	EXPECT_NEAR( h->getUranianLongitude( PlanetId::AscNode ), gen_longitude( Sign::Pisces, 0, 43 ), 2 * DEG_TOL );
	EXPECT_NEAR( h->getUranianLongitude( PlanetId::Neptune ), gen_longitude( Sign::Pisces, 22, 57 ), DEG_TOL );
	EXPECT_NEAR( h->getUranianLongitude( PlanetId::Mars ), gen_longitude( Sign::Pisces, 26, 40 ), DEG_TOL );

	EXPECT_NEAR( h->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Aquarius, 6, 58 ), DEG_TOL );
	EXPECT_NEAR( h->getHouseCusp( House::h12, viewpoint ), gen_longitude( Sign::Aquarius, 23, 0 ), HOUSE_TOL );

	EXPECT_NEAR( h->getUranianLongitude( PlanetId::Meridian ), gen_longitude( Sign::Capricorn, 7, 15 ), DEG_TOL );
	EXPECT_NEAR( h->getUranianLongitude( PlanetId::Mercury ), gen_longitude( Sign::Capricorn, 13, 13 ), DEG_TOL );
	EXPECT_NEAR( h->getHouseCusp( House::h11, viewpoint ), gen_longitude( Sign::Capricorn, 25, 45 ), HOUSE_TOL );

	EXPECT_NEAR( h->getUranianLongitude( PlanetId::Venus ), gen_longitude( Sign::Sagittarius, 23, 52 ), 4 * DEG_TOL ); // it is 23:49
	EXPECT_NEAR( h->getHouseCusp( House::h9, viewpoint ), gen_longitude( Sign::Sagittarius, 19, 53 ), HOUSE_TOL );

	EXPECT_NEAR( h->getUranianLongitude( PlanetId::Moon ), gen_longitude( Sign::Scorpio, 24, 22 ), DEG_TOL );
	EXPECT_NEAR( h->getHouseCusp( House::h8, viewpoint ), gen_longitude( Sign::Scorpio, 29, 15 ), HOUSE_TOL );

	EXPECT_NEAR( h->getUranianLongitude( PlanetId::Descendant ), gen_longitude( Sign::Libra, 19, 25 ), DEG_TOL );

	EXPECT_NEAR( h->getUranianLongitude( PlanetId::Ascendant ), gen_longitude( Sign::Aries, 19, 25 ), DEG_TOL );

	EXPECT_NEAR( h->getUranianLongitude( PlanetId::Uranus ), gen_longitude( Sign::Taurus, 29, 34 ), 5 * DEG_TOL ); // it is 29:30
	EXPECT_NEAR( h->getHouseCusp( House::h2, viewpoint ), gen_longitude( Sign::Taurus, 29, 15 ), HOUSE_TOL );

	EXPECT_NEAR( h->getUranianLongitude( PlanetId::Jupiter ), gen_longitude( Sign::Gemini, 11, 41 ), DEG_TOL );
	EXPECT_NEAR( h->getHouseCusp( House::h3, viewpoint ), gen_longitude( Sign::Gemini, 19, 53 ), HOUSE_TOL );

	EXPECT_NEAR( h->getHouseCusp( House::h4, viewpoint ), gen_longitude( Sign::Cancer, 7, 15 ), HOUSE_TOL );
	EXPECT_NEAR( h->getHouseCusp( House::h5, viewpoint ), gen_longitude( Sign::Cancer, 25, 45 ), HOUSE_TOL );

	EXPECT_NEAR( h->getUranianLongitude( PlanetId::Saturn ), gen_longitude( Sign::Leo, 9, 3 ), DEG_TOL );
	EXPECT_NEAR( h->getHouseCusp( House::h6, viewpoint ), gen_longitude( Sign::Leo, 23, 0 ), HOUSE_TOL );

	EXPECT_NEAR( h->getUranianLongitude( PlanetId::DescNode ), gen_longitude( Sign::Virgo, 0, 43 ), 2 * DEG_TOL );

	// Declinations
	EXPECT_NEAR( h->getDeclination( PlanetId::Sun, viewpoint ), gen_degree( -18, 43 ), DECL_TOL );
	EXPECT_NEAR( h->getDeclination( PlanetId::Moon, viewpoint ), gen_degree( -23, 53 ), DECL_TOL );
	EXPECT_NEAR( h->getDeclination( PlanetId::Mercury, viewpoint ), gen_degree( -22, 26 ), DECL_TOL );
	EXPECT_NEAR( h->getDeclination( PlanetId::Venus, viewpoint ), gen_degree( -18, 16 ), DECL_TOL );
	EXPECT_NEAR( h->getDeclination( PlanetId::Mars, viewpoint ), gen_degree( -1, 47 ), DECL_TOL );
	EXPECT_NEAR( h->getDeclination( PlanetId::Jupiter, viewpoint ), gen_degree( 21, 45 ), DECL_TOL );
	EXPECT_NEAR( h->getDeclination( PlanetId::Saturn, viewpoint ), gen_degree( 18, 46 ), DECL_TOL );
	EXPECT_NEAR( h->getDeclination( PlanetId::Uranus, viewpoint ), gen_degree( 19, 55 ), DECL_TOL );
	EXPECT_NEAR( h->getDeclination( PlanetId::Neptune, viewpoint ), gen_degree( -3, 55 ), DECL_TOL );
	EXPECT_NEAR( h->getDeclination( PlanetId::AscNode, viewpoint ), gen_degree( -11, 13 ), DECL_TOL );
	EXPECT_NEAR( h->getDeclination( PlanetId::Meridian, viewpoint ), gen_degree( -23, 13 ), DECL_TOL );
	EXPECT_NEAR( h->getDeclination( PlanetId::Ascendant, viewpoint ), gen_degree( 7, 36 ), DECL_TOL );

	// Latitudes
	EXPECT_NEAR( h->getLatitude( PlanetId::Sun, viewpoint ), gen_degree( 0, 0 ), DECL_TOL );
	EXPECT_NEAR( h->getLatitude( PlanetId::Moon, viewpoint ), gen_degree( -5, 13 ), DECL_TOL );
	EXPECT_NEAR( h->getLatitude( PlanetId::Mercury, viewpoint ), gen_degree( 0, 21 ), DECL_TOL );
	EXPECT_NEAR( h->getLatitude( PlanetId::Venus, viewpoint ), gen_degree( 5, 2 ), DECL_TOL );
	EXPECT_NEAR( h->getLatitude( PlanetId::Mars, viewpoint ), neg_degree( 0, 30 ), DECL_TOL );
	EXPECT_NEAR( h->getLatitude( PlanetId::Jupiter, viewpoint ), neg_degree( 0, 28 ), DECL_TOL );
	EXPECT_NEAR( h->getLatitude( PlanetId::Saturn, viewpoint ), gen_degree( 0, 47 ), DECL_TOL );
	EXPECT_NEAR( h->getLatitude( PlanetId::Uranus, viewpoint ), neg_degree( 0, 9 ), DECL_TOL );
	EXPECT_NEAR( h->getLatitude( PlanetId::Neptune, viewpoint ), gen_degree( -1, 13 ), DECL_TOL );
	EXPECT_NEAR( h->getLatitude( PlanetId::AscNode, viewpoint ), gen_degree( 0, 0 ), DECL_TOL );
	EXPECT_NEAR( h->getLatitude( PlanetId::Meridian, viewpoint ), gen_degree( 0, 0 ), DECL_TOL );
	EXPECT_NEAR( h->getLatitude( PlanetId::Ascendant, viewpoint ), gen_degree( 0, 0 ), DECL_TOL );

	/*
	* Term section
	*/
	// Mars + Saturn = 4:43 Leo etc.
	EXPECT_NEAR( MTerm( PlanetId::Mars, PlanetId::Saturn, TermId::Sum ).evaluate( h, viewpoint ), gen_longitude( Sign::Leo, 5, 43 ), DEG_TOL );

	// Mars / Saturn = 2:42 Ge/Sag (10 Minutes diff)
	EXPECT_NEAR( MTerm( PlanetId::Mars, PlanetId::Saturn, TermId::Midpoint ).evaluate( h, viewpoint ),
		gen_longitude( Sign::Gemini, 2, 52 ), DEG_TOL );

	// Mars + Saturn - Sun
	EXPECT_NEAR( MTerm( PlanetId::Mars, PlanetId::Saturn, PlanetId::Sun, TermId::Triple ).evaluate( h, viewpoint ),
		gen_longitude( Sign::Virgo, 28, 45 ), DEG_TOL );

	// Mars + Saturn - IV
	EXPECT_NEAR( MTerm( PlanetId::Mars, PlanetId::Saturn, PlanetId::ImumCoeli, TermId::Triple ).evaluate( h, viewpoint ),
		gen_longitude( Sign::Aries, 28, 28 ), DEG_TOL );

	// Mars + Saturn - As: Witte has 6:16 Cancer
	EXPECT_NEAR( MTerm( PlanetId::Mars, PlanetId::Saturn, PlanetId::Ascendant, TermId::Triple ).evaluate( h, viewpoint ),
		gen_longitude( Sign::Cancer, 16, 18 ), 2 * DEG_TOL );

	// Saturn + Aries - Mars
	EXPECT_NEAR( MTerm( PlanetId::Saturn, PlanetId::Aries, PlanetId::Mars, TermId::Triple ).evaluate( h, viewpoint ),
		gen_longitude( Sign::Leo, 12, 23 ), DEG_TOL );

	// Mars + Aries - Saturn
	EXPECT_NEAR( MTerm( PlanetId::Mars, PlanetId::Aries, PlanetId::Saturn, TermId::Triple ).evaluate( h, viewpoint ),
		gen_longitude( Sign::Scorpio, 17, 37 ), DEG_TOL );

	// Saturn + Sun - Mars
	EXPECT_NEAR( MTerm( PlanetId::Saturn, PlanetId::Sun, PlanetId::Mars, TermId::Triple ).evaluate( h, viewpoint ),
		gen_longitude( Sign::Gemini, 19, 21 ), DEG_TOL );

	// Mars + Sun - Saturn
	EXPECT_NEAR( MTerm( PlanetId::Mars, PlanetId::Sun, PlanetId::Saturn, TermId::Triple ).evaluate( h, viewpoint ),
		gen_longitude( Sign::Virgo, 24, 35 ), DEG_TOL );

	// Saturn + IV - Mars
	EXPECT_NEAR( MTerm( PlanetId::Saturn, PlanetId::ImumCoeli, PlanetId::Mars, TermId::Triple ).evaluate( h, viewpoint ),
		gen_longitude( Sign::Scorpio, 19, 38 ), DEG_TOL );

	// Mars + IV - Saturn
	// Witte has 5:08 Taurus = a. -a is 24:52 Aq
	EXPECT_NEAR( MTerm( PlanetId::Mars, PlanetId::ImumCoeli, PlanetId::Saturn, TermId::Triple ).evaluate( h, viewpoint ),
		gen_longitude( Sign::Aquarius, 24, 52 ), DEG_TOL );

	// Saturn + As - Mars
	EXPECT_NEAR( MTerm( PlanetId::Saturn, PlanetId::Ascendant, PlanetId::Mars, TermId::Triple ).evaluate( h, viewpoint ),
		gen_longitude( Sign::Virgo, 1, 48 ), DEG_TOL );

	// Mars + As - Saturn
	EXPECT_NEAR( MTerm( PlanetId::Mars, PlanetId::Ascendant, PlanetId::Saturn, TermId::Triple ).evaluate( h, viewpoint ),
		gen_longitude( Sign::Sagittarius, 7, 2 ), 2 * DEG_TOL );

	/*
	* Direction section
	*/
	DirectionExpert expert( props );

	// Marriage
	double transitjd = gen_date( 1881, 2, 27 );
	{
		expert.calculateDirections( transitjd, HoroscopeRole::SolarArc );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::SolarArc );
		assert( ht );
		EXPECT_NEAR( ht->getLongitudeOffset(), gen_degree( 22, 22 ), DEG_TOL );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Aquarius, 29, 20 ), DEG_TOL );
	}
	{
		expert.calculateDirections( transitjd, HoroscopeRole::Transit );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::Transit );
		assert( ht );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Pisces, 9, 4 ), 1 );
	}

	// Father's death
	transitjd = gen_date( 1888, 6, 15 );
	{
		expert.calculateDirections( transitjd, HoroscopeRole::SolarArc );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::SolarArc );
		assert( ht );
		EXPECT_NEAR( ht->getLongitudeOffset(), gen_degree( 29, 42 ), DEG_TOL );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Pisces, 6, 40 ), DEG_TOL );
	}
	{
		expert.calculateDirections( transitjd, HoroscopeRole::Transit );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::Transit );
		assert( ht );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Gemini, 24, 44 ), 1 );
	}

	// Death Bismarck
	transitjd = gen_date( 1898, 7, 30 );
	{
		expert.calculateDirections( transitjd, HoroscopeRole::SolarArc );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::SolarArc );
		assert( ht );
		EXPECT_NEAR( ht->getLongitudeOffset(), gen_degree( 39, 51 ), DEG_TOL );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Pisces, 16, 49 ), DEG_TOL );
	}
	{
		expert.calculateDirections( transitjd, HoroscopeRole::Transit );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::Transit );
		assert( ht );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Leo, 7, 17 ), 1 );
	}

	// 1st assasination attempt
	transitjd = gen_date( 1900, 11, 16 );
	{
		expert.calculateDirections( transitjd, HoroscopeRole::SolarArc );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::SolarArc );
		assert( ht );
		EXPECT_NEAR( ht->getLongitudeOffset(), gen_degree( 42, 8 ), DEG_TOL );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Pisces, 19, 6 ), DEG_TOL );
	}
	{
		expert.calculateDirections( transitjd, HoroscopeRole::Transit );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::Transit );
		assert( ht );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Scorpio, 23, 42 ), 1 );
	}

	// 2nd assasination attempt
	transitjd = gen_date( 1901, 3, 6 );
	{
		expert.calculateDirections( transitjd, HoroscopeRole::SolarArc );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::SolarArc );
		assert( ht );
		EXPECT_NEAR( ht->getLongitudeOffset(), gen_degree( 42, 32 ), 10 * DEG_TOL ); // fuzzy
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Pisces, 19, 30 ), 10 * DEG_TOL );
	}
	{
		expert.calculateDirections( transitjd, HoroscopeRole::Transit );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::Transit );
		assert( ht );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Pisces, 15, 15 ), 1 );
	}

	// Mother's death
	transitjd = gen_date( 1901, 8, 5 );
	{
		expert.calculateDirections( transitjd, HoroscopeRole::SolarArc );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::SolarArc );
		assert( ht );
		EXPECT_NEAR( ht->getLongitudeOffset(), gen_degree( 42, 52 ), DEG_TOL );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Pisces, 19, 50 ), DEG_TOL );
	}
	{
		expert.calculateDirections( transitjd, HoroscopeRole::Transit );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::Transit );
		assert( ht );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Leo, 12, 20 ), 1 );
	}

	// Daughter's marriage
	transitjd = gen_date( 1913, 5, 24 );
	{
		expert.calculateDirections( transitjd, HoroscopeRole::SolarArc );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::SolarArc );
		assert( ht );
		EXPECT_NEAR( ht->getLongitudeOffset(), gen_degree( 54, 23 ), 15 * DEG_TOL );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Aries, 1, 21 ), 15 * DEG_TOL );
	}
	{
		expert.calculateDirections( transitjd, HoroscopeRole::Transit );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::Transit );
		assert( ht );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Gemini, 2, 41 ), 1 );
	}

	// declaration of war
	transitjd = gen_date( 1914, 8, 1 );
	{
		expert.calculateDirections( transitjd, HoroscopeRole::SolarArc );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::SolarArc );
		assert( ht );
		EXPECT_NEAR( ht->getLongitudeOffset(), gen_degree( 55, 46 ), DEG_TOL );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Aries, 2, 44 ), DEG_TOL );
	}
	{
		expert.calculateDirections( transitjd, HoroscopeRole::Transit );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::Transit );
		assert( ht );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Leo, 8, 22 ), 1 );
	}

	// Escape to Netherland
	transitjd = gen_date( 1918, 11, 9 );
	{
		expert.calculateDirections( transitjd, HoroscopeRole::SolarArc );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::SolarArc );
		assert( ht );
		EXPECT_NEAR( ht->getLongitudeOffset(), gen_degree( 60, 2 ), 3 * DEG_TOL );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Aries, 7, 0 ), 3 * DEG_TOL );
	}
	{
		expert.calculateDirections( transitjd, HoroscopeRole::Transit );
		Horoscope_ptr ht = props->getHoroscope( HoroscopeRole::Transit );
		assert( ht );
		EXPECT_NEAR( ht->getUranianLongitude( PlanetId::Sun ), gen_longitude( Sign::Scorpio, 16, 27 ), 1 );
	}

	// reset
	calccfg = astroconfig->getViewpointConfig( Viewpoint::Uranian ).calculation;
}

