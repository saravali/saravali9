/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       test/src/VedicTest.cpp
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
#include "enumserializer.h"
#include "Exporter.h"
#include "Horoscope.h"
#include "Lang.h"
#include "mathbase.h"
#include "MDocument.h"
#include "SolarHoroscope.h"
#include "TestRunner.h"
#include "Varga.h"

extern Astroconfig *astroconfig;

TEST( VedicAstrology, VargaTest )
{
	constexpr Viewpoint viewpoint = Viewpoint::Vedic;

	Horoscope_ptr h( new Horoscope());
	h->openFile( TEST2_MTXFILE );
	h->update();

	EXPECT_EQ( h->getVargaData( PlanetId::Sun, Varga::Rasi, viewpoint ).getRasi(), Sign::Virgo );
	EXPECT_EQ( h->getVargaData( PlanetId::Moon, Varga::Rasi, viewpoint ).getRasi(), Sign::Aries );
	EXPECT_EQ( h->getVargaData( PlanetId::Mars, Varga::Rasi, viewpoint ).getRasi(), Sign::Gemini );
	EXPECT_EQ( h->getVargaData( PlanetId::Mercury, Varga::Rasi, viewpoint ).getRasi(), Sign::Virgo );
	EXPECT_EQ( h->getVargaData( PlanetId::Jupiter, Varga::Rasi, viewpoint ).getRasi(), Sign::Aquarius );
	EXPECT_EQ( h->getVargaData( PlanetId::Venus, Varga::Rasi, viewpoint ).getRasi(), Sign::Libra );
	EXPECT_EQ( h->getVargaData( PlanetId::Saturn, Varga::Rasi, viewpoint ).getRasi(), Sign::Capricorn );
	EXPECT_EQ( h->getVargaData( PlanetId::Rahu, Varga::Rasi, viewpoint ).getRasi(), Sign::Cancer );
	EXPECT_EQ( h->getVargaData( PlanetId::Ketu, Varga::Rasi, viewpoint ).getRasi(), Sign::Capricorn );
	EXPECT_EQ( h->getVargaData( PlanetId::Ascendant, Varga::Rasi, viewpoint ).getRasi(), Sign::Libra );

	// D9 Navamsa
	EXPECT_EQ( h->getVargaData( PlanetId::Sun, Varga::Navamsa, viewpoint ).getRasi(), Sign::Capricorn );
	EXPECT_EQ( h->getVargaData( PlanetId::Moon, Varga::Navamsa, viewpoint ).getRasi(), Sign::Leo );
	EXPECT_EQ( h->getVargaData( PlanetId::Mars, Varga::Navamsa, viewpoint ).getRasi(), Sign::Aries );
	EXPECT_EQ( h->getVargaData( PlanetId::Mercury, Varga::Navamsa, viewpoint ).getRasi(), Sign::Leo );
	EXPECT_EQ( h->getVargaData( PlanetId::Jupiter, Varga::Navamsa, viewpoint ).getRasi(), Sign::Capricorn );
	EXPECT_EQ( h->getVargaData( PlanetId::Venus, Varga::Navamsa, viewpoint ).getRasi(), Sign::Aquarius );
	EXPECT_EQ( h->getVargaData( PlanetId::Saturn, Varga::Navamsa, viewpoint ).getRasi(), Sign::Aries );
	EXPECT_EQ( h->getVargaData( PlanetId::Rahu, Varga::Navamsa, viewpoint ).getRasi(), Sign::Libra );
	EXPECT_EQ( h->getVargaData( PlanetId::Ketu, Varga::Navamsa, viewpoint ).getRasi(), Sign::Aries );
	EXPECT_EQ( h->getVargaData( PlanetId::Ascendant, Varga::Navamsa, viewpoint ).getRasi(), Sign::Taurus );

	// D12 Dvadasamsa
	EXPECT_EQ( h->getVargaData( PlanetId::Sun, Varga::Dvadasamsa, viewpoint ).getRasi(), Sign::Virgo );
	EXPECT_EQ( h->getVargaData( PlanetId::Moon, Varga::Dvadasamsa, viewpoint ).getRasi(), Sign::Libra );
	EXPECT_EQ( h->getVargaData( PlanetId::Mars, Varga::Dvadasamsa, viewpoint ).getRasi(), Sign::Pisces );
	EXPECT_EQ( h->getVargaData( PlanetId::Mercury, Varga::Dvadasamsa, viewpoint ).getRasi(), Sign::Cancer );
	EXPECT_EQ( h->getVargaData( PlanetId::Jupiter, Varga::Dvadasamsa, viewpoint ).getRasi(), Sign::Gemini );
	EXPECT_EQ( h->getVargaData( PlanetId::Venus, Varga::Dvadasamsa, viewpoint ).getRasi(), Sign::Aries );
	EXPECT_EQ( h->getVargaData( PlanetId::Saturn, Varga::Dvadasamsa, viewpoint ).getRasi(), Sign::Taurus );
	EXPECT_EQ( h->getVargaData( PlanetId::Rahu, Varga::Dvadasamsa, viewpoint ).getRasi(), Sign::Sagittarius );
	EXPECT_EQ( h->getVargaData( PlanetId::Ketu, Varga::Dvadasamsa, viewpoint ).getRasi(), Sign::Gemini );
	EXPECT_EQ( h->getVargaData( PlanetId::Ascendant, Varga::Dvadasamsa, viewpoint ).getRasi(), Sign::Cancer );

	// D30 Trimsamsa
	EXPECT_EQ( h->getVargaData( PlanetId::Sun, Varga::Trimsamsa, viewpoint ).getRasi(), Sign::Taurus );
	EXPECT_EQ( h->getVargaData( PlanetId::Moon, Varga::Trimsamsa, viewpoint ).getRasi(), Sign::Sagittarius );
	EXPECT_EQ( h->getVargaData( PlanetId::Mars, Varga::Trimsamsa, viewpoint ).getRasi(), Sign::Gemini );
	EXPECT_EQ( h->getVargaData( PlanetId::Mercury, Varga::Trimsamsa, viewpoint ).getRasi(), Sign::Scorpio );
	EXPECT_EQ( h->getVargaData( PlanetId::Jupiter, Varga::Trimsamsa, viewpoint ).getRasi(), Sign::Sagittarius );
	EXPECT_EQ( h->getVargaData( PlanetId::Venus, Varga::Trimsamsa, viewpoint ).getRasi(), Sign::Sagittarius );
	// TODO Virgo or Pisces?
	//EXPECT_EQ( h->getVargaData( PlanetId::Saturn, Varga::Trimsamsa, viewpoint ).getRasi(), Sign::Virgo );
	EXPECT_EQ( h->getVargaData( PlanetId::Rahu, Varga::Trimsamsa, viewpoint ).getRasi(), Sign::Pisces );
	EXPECT_EQ( h->getVargaData( PlanetId::Ketu, Varga::Trimsamsa, viewpoint ).getRasi(), Sign::Pisces );
	EXPECT_EQ( h->getVargaData( PlanetId::Ascendant, Varga::Trimsamsa, viewpoint ).getRasi(), Sign::Gemini );

}

double extractTimeFromDate( const double &jd, const int timezone )
{
	const double t = getTimeFromJD( jd + timezone / 24.0 );
	//std::cout << "JD " << std::fixed << std::setprecision( 6 ) << jd << " t " << t << std::endl;
	return t;
}

/**
* Compare several basic params of the standard nativity with JHora.
*
* Ayanamsa is set to Classic Lahiri (non default)
*
* The program uses the following params for Kalavelas
*
* Object        Lord    Portion
*---------------------------------- 
* Kala          Sun     Middle
* Mrityu        Mars    Middle
* Ardhaprahara  Mercury Middle
* Yamaghantaka  Jupiter Middle
* Gulika        Saturn  Beginning
* Mandi         Saturn  Middle
* 
* 
*/
TEST( VedicAstrology, BasicDataTest )
{
	constexpr Viewpoint viewpoint = Viewpoint::Vedic;

	CalculationConfig &calccfg = astroconfig->getViewpointConfig( viewpoint ).calculation;
	calccfg.lunarNodeMode = LunarNodeMode::True;

	astroconfig->kalavela.elements =
	 {
    { PlanetId::Kala, { KalavelaLord::Sun, KalavelaPortion::Middle }},
    { PlanetId::Mrityu, { KalavelaLord::Mars, KalavelaPortion::Middle }},
    { PlanetId::Ardhaprahara, { KalavelaLord::Mercury, KalavelaPortion::Middle }},
    { PlanetId::Yamaghantaka, { KalavelaLord::Jupiter, KalavelaPortion::Middle }},
    { PlanetId::Gulika, { KalavelaLord::Saturn, KalavelaPortion::Beginning }},
    { PlanetId::Mandi, { KalavelaLord::Saturn, KalavelaPortion::Middle }},
  };

	Horoscope h;
	h.openFile( TEST2_MTXFILE );
	h.update();

	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Sun ), gen_longitude( Sign::Virgo, 0, 42, 39 ), DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Moon ), gen_longitude( Sign::Aries, 16, 39, 26 ), DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Mars ), gen_longitude( Sign::Gemini, 22, 46, 49 ), DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Jupiter ), gen_longitude( Sign::Aquarius, 12, 14 ), DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Venus ), gen_longitude( Sign::Libra, 16, 0, 17 ), DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Saturn ), gen_longitude( Sign::Capricorn, 11, 49, 57 ), DEG_TOL );

	EXPECT_NEAR( h.getVedicLongitude( PlanetId::AscNode ), gen_longitude( Sign::Cancer, 14, 35, 41 ), DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::DescNode ), gen_longitude( Sign::Capricorn, 14, 35, 41 ), DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Ascendant ), gen_longitude( Sign::Libra, 23, 48, 59 ), DEG_TOL );

	EXPECT_NEAR( extractTimeFromDate( h.getSunrise(), 1 ), gen_degree( 6, 3, 30 ), DEG_TOL );
	EXPECT_NEAR( extractTimeFromDate( h.getSunset(), 1 ), gen_degree( 18, 27, 42 ), DEG_TOL );

	EXPECT_NEAR( h.getVedicLongitude( PlanetId::BhavaLagna ), gen_longitude( Sign::Scorpio, 17, 20, 7 ), DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::HoraLagna ), gen_longitude( Sign::Aquarius, 3, 57, 34 ), DEG_TOL );

	// TODO why 2 * TOL
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::GhatikaLagna ), gen_longitude( Sign::Virgo, 23, 49, 58 ), 2 * DEG_TOL );

	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Dhuma ), gen_longitude( Sign::Capricorn, 14, 2, 4 ), 2 * DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Vyatipata ), gen_longitude( Sign::Gemini, 15, 57, 21 ), 2 * DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Parivesha ), gen_longitude( Sign::Sagittarius, 15, 57, 21 ), 2 * DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Chapa ), gen_longitude( Sign::Cancer, 14, 2, 39 ), 2 * DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Upaketu ), gen_longitude( Sign::Leo, 0, 42, 39 ), 2 * DEG_TOL );

	// Config see above
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Kala ), gen_longitude( Sign::Aquarius, 3, 53, 25 ), 2 * DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Mrityu ), gen_longitude( Sign::Virgo, 24, 52, 2 ), 2 * DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Ardhaprahara ), gen_longitude( Sign::Libra, 11, 0, 0 ), 2 * DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Yamaghantaka ), gen_longitude( Sign::Libra, 27, 8, 52 ), 2 * DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Gulika ), gen_longitude( Sign::Scorpio, 22, 43, 22 ), 2 * DEG_TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Mandi ), gen_longitude( Sign::Sagittarius, 2, 21, 26 ), 2 * DEG_TOL );


	// reset
	*astroconfig = Astroconfig();
}

/**
* Compare kp values of the standard nativity with JHora.
*/
TEST( VedicAstrology, KpTest )
{
	constexpr Viewpoint viewpoint = Viewpoint::Vedic;

	CalculationConfig &calccfg = astroconfig->getViewpointConfig( viewpoint ).calculation;
	EXPECT_EQ( calccfg.lunarNodeMode, LunarNodeMode::Mean );
	calccfg.lunarNodeMode = LunarNodeMode::True;

	ContextProps_ptr props( new ContextProperties );
	Horoscope_ptr h( new Horoscope());
	props->setRadixHoroscope( h );

	h->openFile( TEST2_MTXFILE );
	h->update();

	EXPECT_EQ( h->getKPLords( PlanetId::Sun, viewpoint ), KpData( PlanetId::Sun, PlanetId::AscNode, PlanetId::Venus ));
	EXPECT_EQ( h->getKPLords( PlanetId::Moon, viewpoint ), KpData( PlanetId::Venus, PlanetId::Moon, PlanetId::Jupiter ));
	EXPECT_EQ( h->getKPLords( PlanetId::Mars, viewpoint ), KpData( PlanetId::Jupiter, PlanetId::Saturn, PlanetId::Venus ));
	EXPECT_EQ( h->getKPLords( PlanetId::Mercury, viewpoint ), KpData( PlanetId::Mars, PlanetId::Jupiter, PlanetId::Jupiter ));
	EXPECT_EQ( h->getKPLords( PlanetId::Jupiter, viewpoint ), KpData( PlanetId::Rahu, PlanetId::Saturn, PlanetId::Rahu ));
	EXPECT_EQ( h->getKPLords( PlanetId::Venus, viewpoint ), KpData( PlanetId::Rahu, PlanetId::Venus, PlanetId::Mars ));
	EXPECT_EQ( h->getKPLords( PlanetId::Saturn, viewpoint ), KpData( PlanetId::Moon, PlanetId::Mars, PlanetId::Moon ));
	EXPECT_EQ( h->getKPLords( PlanetId::Rahu, viewpoint ), KpData( PlanetId::Saturn, PlanetId::Rahu, PlanetId::Sun ));
	EXPECT_EQ( h->getKPLords( PlanetId::Ketu, viewpoint ), KpData( PlanetId::Moon, PlanetId::Jupiter, PlanetId::Mercury ));
	EXPECT_EQ( h->getKPLords( PlanetId::Ascendant, viewpoint ), KpData( PlanetId::Jupiter, PlanetId::Saturn, PlanetId::Jupiter ));

	calccfg.lunarNodeMode = LunarNodeMode::Mean;
}

/**
* See Sumeet Chugh: Varshphal. Standard nativity p. 5
*/
TEST( SolarTest, VedicSolarTest )
{
	constexpr Viewpoint viewpoint = Viewpoint::Vedic;
	constexpr double TOL = 1;

	ContextProps_ptr props( new ContextProperties );
	Horoscope_ptr h( new Horoscope());
	props->setRadixHoroscope( h );
	props->setPreferredViewpoint( viewpoint );

	h->openFile( CHUGH_SOLAR1_MTXFILE );
	h->update();

	EXPECT_NEAR( h->getVedicLongitude( PlanetId::Ascendant ), gen_longitude( 4, 12, 11 ), TOL );
	EXPECT_NEAR( h->getVedicLongitude( PlanetId::Sun ), gen_longitude( 6, 21, 4 ), TOL );
	EXPECT_NEAR( h->getVedicLongitude( PlanetId::Moon ), gen_longitude( 1, 9, 18 ), TOL );
	EXPECT_NEAR( h->getVedicLongitude( PlanetId::Mars ), gen_longitude( 5, 5, 9 ), TOL );
	EXPECT_NEAR( h->getVedicLongitude( PlanetId::Mercury ), gen_longitude( 6, 4, 28 ), TOL );
	EXPECT_NEAR( h->getVedicLongitude( PlanetId::Jupiter ), gen_longitude( 5, 5, 1 ), TOL );
	EXPECT_NEAR( h->getVedicLongitude( PlanetId::Venus ), gen_longitude( 7, 26, 4 ), TOL );
	EXPECT_NEAR( h->getVedicLongitude( PlanetId::Saturn ), gen_longitude( 11, 26, 58 ), TOL );
	EXPECT_NEAR( h->getVedicLongitude( PlanetId::Rahu ), gen_longitude( 11, 14, 6 ), TOL );

	SolarHoroscopeExpert expert( props );
  expert.setSolarReturnYear( 1993 );
  expert.update();

  Horoscope_ptr hsolar = expert.getSolarHoroscope();

	//MDocument *doc = new MDocument;
	MDocument_ptr doc( new MDocument );
  expert.writeSolarReturnData( doc );
	Exporter *exporter = ExporterFactory().getExporter( WidgetExportType::Text );
	mcout << exporter->exportDocument( doc ) << std::endl;
	delete exporter;

	EXPECT_NEAR( hsolar->getVedicLongitude( PlanetId::Ascendant ), gen_longitude( 8, 28, 46 ), 4 * TOL );
	EXPECT_NEAR( hsolar->getVedicLongitude( PlanetId::Sun ), gen_longitude( 6, 21, 4 ), TOL );
	EXPECT_NEAR( hsolar->getVedicLongitude( PlanetId::Moon ), gen_longitude( 3, 20, 48 ), TOL );
	EXPECT_NEAR( hsolar->getVedicLongitude( PlanetId::Saturn ), gen_longitude( 9, 29, 57 ), TOL );

/*
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Ascendant ), gen_longitude( 9, 27, 20 ), 2 * TOL );
	EXPECT_NEAR( h.getVedicLongitude( PlanetId::Moon ), gen_longitude( 10, 19, 3 ), TOL );

	DasaExpert *expert = DasaExpertFactory().getDasaExpert( DasaId::Yogini );
	Dasa *rootdasa = expert->createMahadasa( &props );

	EXPECT_GT( rootdasa->children.size(), 10u );

	int count = 0;
	Dasa *dasa1 = nullptr;
	for( Dasa *dasa : rootdasa->children )
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
				expert->createChildren( dasa1 );
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
	EXPECT_EQ( dasa1->children.size(), 8u );

	auto antardasa = dasa1->children.begin();
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1989, 2, 7 ), TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Rahu ));  // Shankata

	EXPECT_NE( ++antardasa, dasa1->children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1990, 11, 17 ), 2 * TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Moon )); // Mangala

	EXPECT_NE( ++antardasa, dasa1->children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1991, 2, 7 ), TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Sun )); // Pingala

	EXPECT_NE( ++antardasa, dasa1->children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1991, 7, 17 ), 3 * TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Jupiter )); // Dhanya

	EXPECT_NE( ++antardasa, dasa1->children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1992, 3, 17 ), 3 * TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Mars ));  // Bhramari

	EXPECT_NE( ++antardasa, dasa1->children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1993, 2, 7 ), TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Mercury ));  // Bhadrika

	EXPECT_NE( ++antardasa, dasa1->children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1994, 3, 17 ), 3 * TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Saturn ));  // Ulka

	EXPECT_NE( ++antardasa, dasa1->children.end());
	EXPECT_NEAR( (*antardasa)->getStartJD(), gen_date( 1995, 7, 17 ), 3 * TOL );
	EXPECT_EQ( (*antardasa)->getDasaLord(), DasaLord( PlanetId::Venus ));  // Siddha

	delete rootdasa;
*/
}

