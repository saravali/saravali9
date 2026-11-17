/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       test/src/DasaTest.cpp
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

#include "astrobase.h"
#include "astroconfig.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "Horoscope.h"
#include "KalachakraDasa.h"
#include "mathbase.h"
#include "TestRunner.h"

extern Astroconfig *astroconfig;

/**
* See Sumeet Chugh: Yogini and Kalachakra Dasa. Standard nativity p. 8 ff
*/
TEST( DasaTest, YoginiDasaTest )
{
	//constexpr Viewpoint viewpoint = Viewpoint::Vedic;
	constexpr double TOL = 1.0;
	ContextProps_ptr props( new ContextProperties );
	Horoscope_ptr h( new Horoscope());
	props->setRadixHoroscope( h );

	h->openFile( CHUGH1_MTXFILE );
	h->update();

	EXPECT_NEAR( h->getVedicLongitude( PlanetId::Ascendant ), gen_longitude( 9, 27, 20 ), 2 * DEG_TOL );
	EXPECT_NEAR( h->getVedicLongitude( PlanetId::Moon ), gen_longitude( 10, 19, 3 ), DEG_TOL );

	DasaExpert_ptr expert( DasaExpertFactory().getDasaExpert( DasaId::Yogini ));
	Dasa_ptr rootdasa = expert->createBlankRoot( expert );

	const std::list<Dasa_ptr> dasas = expert->getFirstLevel( props, rootdasa );

	EXPECT_GT( dasas.size(), 10u );

	int count = 0;
	Dasa_ptr dasa1;
	std::list<Dasa_ptr> children;
	for( auto dasa : dasas )
	{
		switch( count )
		{
			case 1:
				EXPECT_NEAR( dasa->getStartJD(), gen_date( 1967, 2, 7 ), TOL );
				EXPECT_EQ( dasa->getDasaLord(), DasaLord( PlanetId::Mars ));
			break;
			case 2:
				EXPECT_NEAR( dasa->getStartJD(), gen_date( 1971, 2, 7 ), TOL );
				EXPECT_EQ( dasa->getDasaLord(), DasaLord( PlanetId::Mercury ));
			break;
			case 3:
				EXPECT_NEAR( dasa->getStartJD(), gen_date( 1976, 2, 7 ), TOL );
				EXPECT_EQ( dasa->getDasaLord(), DasaLord( PlanetId::Saturn ));
			break;
			case 4:
				EXPECT_NEAR( dasa->getStartJD(), gen_date( 1982, 2, 7 ), TOL );
				EXPECT_EQ( dasa->getDasaLord(), DasaLord( PlanetId::Venus ));
			break;
			case 5:
				EXPECT_NEAR( dasa->getStartJD(), gen_date( 1989, 2, 7 ), TOL );
				EXPECT_EQ( dasa->getDasaLord(), DasaLord( PlanetId::Rahu ));
				dasa1 = dasa;
				children = expert->getNextLevel( props, dasa );
			break;
			case 6:
				EXPECT_NEAR( dasa->getStartJD(), gen_date( 1997, 2, 7 ), TOL );
				EXPECT_EQ( dasa->getDasaLord(), DasaLord( PlanetId::Moon ));
			break;
			default:
			break;
		}
		++count;
	}

	EXPECT_NE( dasa1, nullptr );
	EXPECT_EQ( children.size(), 8u );

	auto antardasa = children.begin();
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1989, 2, 7 ), TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Rahu ));  // Shankata

	EXPECT_NE( ++antardasa, children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1990, 11, 17 ), 2 * TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Moon )); // Mangala

	EXPECT_NE( ++antardasa, children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1991, 2, 7 ), TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Sun )); // Pingala

	EXPECT_NE( ++antardasa, children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1991, 7, 17 ), 3 * TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Jupiter )); // Dhanya

	EXPECT_NE( ++antardasa, children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1992, 3, 17 ), 3 * TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Mars ));  // Bhramari

	EXPECT_NE( ++antardasa, children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1993, 2, 7 ), TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Mercury ));  // Bhadrika

	EXPECT_NE( ++antardasa, children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1994, 3, 17 ), 3 * TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Saturn ));  // Ulka

	EXPECT_NE( ++antardasa, children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1995, 7, 17 ), 3 * TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Venus ));  // Siddha
}

/**
* See Sumeet Chugh: Yogini and Kalachakra Dasa. Standard nativity p. 57 ff
*/
TEST( DasaTest, KalachakraDasaTest1 )
{
	EXPECT_EQ( astroconfig->dasa.startOfLastMahadasa, 100 );

	ContextProps_ptr props( new ContextProperties );
	Horoscope_ptr h( new Horoscope());
	props->setRadixHoroscope( h );

	h->openFile( CHUGH1_MTXFILE );
	h->update();

	const double moonlen = h->getVedicLongitude( PlanetId::Moon );
	EXPECT_EQ( ::getNakshatra27( moonlen ), Nakshatra27::Satabhisha );

	DasaExpert_ptr expert( DasaExpertFactory().getDasaExpert( DasaId::Kalachakra ));

	{
		Dasa_ptr rootItem = expert->createBlankRoot( expert );
		const auto dasas = expert->getFirstLevel( props, rootItem );
		EXPECT_EQ( dasas.size(), 12u );
	}

	{
		// 50 years gives the dasas: Ar, Sa, Cp, Aq, Pi, Ar, Ta
		astroconfig->dasa.startOfLastMahadasa = 50;
		Dasa_ptr rootItem = expert->createBlankRoot( expert );
		const auto dasas = expert->getFirstLevel( props, rootItem );
		EXPECT_EQ( dasas.size(), 7u );
	}

	{
		Dasa_ptr rootItem = expert->createBlankRoot( expert );
		// set number of dasas explicitly
		const auto dasas = expert->getFirstLevel( props, rootItem, 8 );
		EXPECT_EQ( dasas.size(), 8u );
		KalachakraDasa *krootItem = dynamic_cast<KalachakraDasa*>( rootItem.get() );


		EXPECT_EQ( krootItem->getPada(), 3 );
		EXPECT_EQ( krootItem->getGroup(), KalachakraGroup::Apsavya );
		EXPECT_EQ( krootItem->getSubgroup(), KalachakraSubgroup::Mrigasira );
		EXPECT_EQ( krootItem->getParamayus(), 100 );
		EXPECT_EQ( krootItem->getAmsa(), Sign::Sagittarius );
		EXPECT_EQ( krootItem->getJeeva(), Sign::Sagittarius );
		EXPECT_EQ( krootItem->getDeha(), Sign::Aries );

		// quite large tolerance of one month, it is kalachakra 
		constexpr double BALANCE_TOL = 1 / 12.0;
		EXPECT_NEAR( krootItem->getPadaBalance(), 28.5, BALANCE_TOL );
		EXPECT_NEAR( krootItem->getFirstMdBalance(), 5.5, BALANCE_TOL );
	}

	// reset
	astroconfig->dasa.startOfLastMahadasa = 100;
}


/**
* See Sumeet Chugh: Yogini and Kalachakra Dasa. Nativity ch 7, first example (p.99)
*/
TEST( DasaTest, KalachakraDasaTest2 )
{
	EXPECT_EQ( astroconfig->dasa.startOfLastMahadasa, 100 );

	ContextProps_ptr props( new ContextProperties );
	Horoscope_ptr h( new Horoscope());
	props->setRadixHoroscope( h );

	h->openFile( CHUGH2_MTXFILE );
	h->update();

	const double moonlen = h->getVedicLongitude( PlanetId::Moon );
	EXPECT_NEAR( moonlen, gen_longitude( 4, 9, 15 ), DEG_TOL );
	EXPECT_EQ( ::getNakshatra27( moonlen ), Nakshatra27::Magha );

	DasaExpert_ptr expert( DasaExpertFactory().getDasaExpert( DasaId::Kalachakra ));
	Dasa_ptr rootItem = expert->createBlankRoot( expert );
	const auto dasas = expert->getFirstLevel( props, rootItem );
	KalachakraDasa *krootItem = dynamic_cast<KalachakraDasa*>( rootItem.get() );

	//EXPECT_EQ( dasas.size(), 9u );
	EXPECT_EQ( dasas.size(), 12u );
	EXPECT_EQ( krootItem->getPada(), 2 );
	EXPECT_EQ( krootItem->getGroup(), KalachakraGroup::Apsavya );
	EXPECT_EQ( krootItem->getSubgroup(), KalachakraSubgroup::Rohini );
	EXPECT_EQ( krootItem->getParamayus(), 85 );
	//EXPECT_EQ( krootItem->getAmsa(), Sign::Sagittarius );
	EXPECT_EQ( krootItem->getDeha(), Sign::Aquarius );
	EXPECT_EQ( krootItem->getJeeva(), Sign::Virgo );
}


