/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/SolarHoroscope.cpp
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

#include "SolarHoroscope.h"

#include "astrobase.h"
#include "constants.h"
#include "Calculator.h"
#include "ContextProperties.h"
#include "DateFormatter.h"
#include "gdiconfig.h"
#include "GenericDasa.h"
#include "Horoscope.h"
#include "Lang.h"
#include "mathbase.h"
#include "MDocument.h"
#include "Table.h"
#include "TextHelper.h"

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   SolarHoroscopeExpert   ---   Constructor
**
******************************************************/
SolarHoroscopeExpert::SolarHoroscopeExpert( ContextProps_ptr props )
	: Expert( props, Viewpoint::None )
{
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );

	hsolar = Horoscope_ptr( new Horoscope( HoroscopeRole::Solar ));
	hsolar->dl.name = tr( "SolarChart" );
	hsolar->dl.location = horoscope->dl.location;
	props->setHoroscope( HoroscopeRole::Solar, hsolar );

	year = getCurrentYear();
}

/*****************************************************
**
**   SolarHoroscopeExpert   ---   Destructor
**
******************************************************/
SolarHoroscopeExpert::~SolarHoroscopeExpert()
{
	props->setHoroscope( HoroscopeRole::Solar, nullptr );
}

/*****************************************************
**
**   SolarHoroscopeExpert   ---   update
**
******************************************************/
void SolarHoroscopeExpert::update()
{
	Calculator *calculator = CalculatorFactory().getCalculator();
	Horoscope_ptr horoscope = props->getRadixHoroscope();

	double jd = calculator->calcSolarReturnJD( horoscope->dl, year, getViewpoint() );
	hsolar->setDate( jd );
	hsolar->update();
}

/*****************************************************
**
**   SolarHoroscopeExpert   ---   setSolarReturnYear
**
******************************************************/
void SolarHoroscopeExpert::setSolarReturnYear( const int &year )
{
	this->year = year;
}

/*****************************************************
**
**   SolarHoroscopeExpert   ---   createYearlyHoroscope
**
******************************************************/
Horoscope_ptr SolarHoroscopeExpert::createYearlyHoroscope( const int &year, const Location &location, const Viewpoint &viewpoint )
{
	Calculator *calculator = CalculatorFactory().getCalculator();

	Horoscope_ptr horoscope( new Horoscope());
	horoscope->dl.date.set( year - 1, 12, 21, 0 );
	horoscope->dl.location = location;
	horoscope->dl.name = std::format( "{} {}", tr( "Yearly Horoscope" ), year );

	const double jd = calculator->calcPlanetaryEvent( horoscope->dl, 270.0, PlanetId::Sun, viewpoint );
	horoscope->setDate( jd );
	horoscope->update();
	return horoscope;
}

/*****************************************************
**
**   SolarHoroscopeExpert   ---   writeSolarReturnData
**
******************************************************/
void SolarHoroscopeExpert::writeSolarReturnData( MDocument_ptr doc )
{
	DateFormatter *formatter = DateFormatter::get();
	constexpr bool isLocaltime = true;
	Horoscope_ptr horoscope = props->getRadixHoroscope();

	TextHelper( props, doc ).writePlanets( hsolar );

	const auto [ birthyear, dummy2, dummy3, dummy1 ] = CalculatorFactory().getCalculator()->getDateIntsFromJD( horoscope->getJD());
	if ( year < birthyear )
	{
		doc->addParagraph( "Year is before birth.");
	}
	else
	{
		doc->addParagraph( std::format( "Native is {} years old.", ( year - birthyear )));
	}

	doc->addLine( tr( "Return date" ) + ": " + formatter->formatDatetime( hsolar->getJD(), isLocaltime ));

	if ( const Viewpoint viewpoint = getViewpoint(); viewpoint == Viewpoint::Vedic )
	{
		Lang lang;
		const int numyears = year - getYearForJD( horoscope->getJD());
		const int muntha =  (int)a_red( numyears
			+ (int)getRasi( horoscope->getLongitude( PlanetId::Ascendant, viewpoint )),  12 );

		doc->addLine( "Muntha: " + lang.getSignName( (Sign)muntha, viewpoint ));

		DasaExpert_ptr dptr( DasaExpertFactory().getDasaExpert( DasaId::Vimsottari ));
		GenericNakshatraDasaExpert *expert =
			dynamic_cast<GenericNakshatraDasaExpert*>( dptr.get() );
		assert( expert );
		
		Table *table = new Table( tr( "Vimsottari" ));
		table->addHeaderRow(
		{
			tr( "Lord" ),
			tr( "Until" )
		});

		for( auto &child : expert->createSolarDasas( props, hsolar->getJD(), year ))
		{
			table->addRow(
			{
				lang.getPlanetName( std::get<PlanetId>( child->getDasaLord()), viewpoint ),
				formatter->formatDate( child->getEndJD(), isLocaltime )
			});
		}
		doc->addItem( table );
	}
}


