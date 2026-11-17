/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       test/src/ArabicPartsTest.cpp
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

#include "ContextProperties.h"
#include "Horoscope.h"
#include "TestRunner.h"

PlanetId getPart( const int &i )
{
	// Astrolog count starts from 1 instead of 0
	return (PlanetId)( (int)PlanetId::FirstArabic + i - 1 );
}

TEST( WesternTest, ArabicPartsTest )
{
	constexpr Viewpoint viewpoint = Viewpoint::Western;
	ContextProps_ptr props( new ContextProperties );
	Horoscope_ptr h( new Horoscope());
	props->setRadixHoroscope( h );

	// house system is Placidus = Default

	// Daybirth
	h->openFile( TEST2_MTXFILE );
	h->update();

	EXPECT_NEAR( h->getLongitude( PlanetId::Sun, viewpoint ), gen_longitude( Sign::Virgo, 24, 2 ), DEG_TOL );
	EXPECT_NEAR( h->getLongitude( PlanetId::Ascendant, viewpoint ), gen_longitude( Sign::Scorpio, 17, 9 ), DEG_TOL );

	// Simple: Fortune
	EXPECT_NEAR( h->getLongitude( getPart( 1 ), viewpoint ), gen_longitude( Sign::Cancer, 3, 6 ), DEG_TOL );

	// Victory: Spirit included
	EXPECT_NEAR( h->getLongitude( getPart( 3 ), viewpoint ), gen_longitude( Sign::Libra, 21, 31 ), DEG_TOL );

	// Valor and Bravery: Fortune included
	EXPECT_NEAR( h->getLongitude( getPart( 4 ), viewpoint ), gen_longitude( Sign::Scorpio, 4, 8 ), DEG_TOL );

	// Properties and Goods: House Cusp and ruler included
	EXPECT_NEAR( h->getLongitude( getPart( 6 ), viewpoint ), gen_longitude( Sign::Leo, 28, 38 ), DEG_TOL );

	// Death of Siblings: House ruler and aditional 10 deg
	EXPECT_NEAR( h->getLongitude( getPart( 8 ), viewpoint ), gen_longitude( Sign::Pisces, 29, 11 ), DEG_TOL );

	// Boldness and Violence: House dispositor included
	EXPECT_NEAR( h->getLongitude( getPart( 47 ), viewpoint ), gen_longitude( Sign::Cancer, 16, 46 ), DEG_TOL );

	// Night birth
	h->openFile( AVTEST_MTXFILE );
	h->update();

	EXPECT_NEAR( h->getLongitude( PlanetId::Sun, viewpoint ), gen_longitude( Sign::Virgo, 21, 45 ), DEG_TOL );
	EXPECT_NEAR( h->getLongitude( PlanetId::Ascendant, viewpoint ), gen_longitude( Sign::Gemini, 26, 42 ), DEG_TOL );

	// Simple: Fortune
	EXPECT_NEAR( h->getLongitude( getPart( 1 ), viewpoint ), gen_longitude( Sign::Taurus, 12, 47 ), 2 * DEG_TOL );

	// Siblings: no flip
	EXPECT_NEAR( h->getLongitude( getPart( 7 ), viewpoint ), gen_longitude( Sign::Aquarius, 26, 5 ), 2 * DEG_TOL );
}


