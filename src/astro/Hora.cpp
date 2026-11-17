/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Hora.cpp
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

#include "Hora.h"

#include "astroconfig.h"
#include "Calculator.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "gdiconfig.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "mstring.h"
#include "MDocument.h"
#include "Table.h"

#include <format>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define HORA_DEB

/*****************************************************
**
**   HoraExpert   ---   Constructor
**
******************************************************/
HoraExpert::HoraExpert()
{
	location = astroconfig->standardLocation;
}

/*****************************************************
**
**   HoraExpert   ---   update
**
******************************************************/
void HoraExpert::update( const double &jd )
{
	// order of planets related to weekdays = natural order of planets in Vedic astrology
	const static PlanetId weekday_lords[7] =
	{
		PlanetId::Sun,
		PlanetId::Moon,
		PlanetId::Mars,
		PlanetId::Mercury,
		PlanetId::Jupiter,
		PlanetId::Venus,
		PlanetId::Saturn
	};

	Calculator *calculator = CalculatorFactory().getCalculator();
	DateFormatter *formatter = DateFormatter::get();

	DateLocation dl;
	dl.date.jd = jd;
	dl.location = location;

	const double sunrise = calculator->calcNextSolarEvent( SolarEventType::Sunrise, jd - 1,
		location.latitude, location.longitude );
	const double sunset = calculator->calcNextSolarEvent( SolarEventType::Sunset, sunrise,
		location.latitude, location.longitude );
	const double sunrise_next = calculator->calcNextSolarEvent( SolarEventType::Sunrise, sunset,
		location.latitude, location.longitude );

	int weekday = getWeekDay( sunrise );
	dinaLord = weekday_lords[ weekday ];

#ifdef HORA_DEB
	debuglog << "##################  DEBUG ######################\n";
	debuglog << location;
	debuglog << std::format( "jd: {:18.12f}", jd );
	debuglog << "jd: " << formatter->formatUTDatetime( jd );
	debuglog << "sunrise: " << formatter->formatUTDatetime( sunrise );
	debuglog << "sunset: " <<  formatter->formatUTDatetime( sunset );
	debuglog << "sunrise NEXT: " << formatter->formatUTDatetime( sunrise_next );
	debuglog << "WEEKDAY " <<  weekday;
	debuglog << "dinalord " << dinaLord;
	debuglog << "##################  END DEBUG  ######################";
#endif

	/*
		According to BPHS 27.13 next Hora lord is the lord of the 6th weekday (+5).
		So the order is:
		Sun, Venus, Mercury, Moon, Saturn, Jupiter, Mars, Sun etc.
	*/
	for ( int i = 0; i < 24; i++ )
	{
		horaLord[i] = weekday_lords[ weekday % 7 ];
		weekday += 5;
	}

	double daydur = sunset - sunrise;
	double daystep = daydur / 12;

	double nightdur = sunrise_next - sunset;
	double nightstep = nightdur / 12;

	horaStart[24] = sunrise_next;

	for ( int i = 0; i < 12; i++ )
	{
		horaStart[i] = sunrise + i * daystep;
	}
	for ( int i = 0; i < 12; i++ )
	{
		horaStart[i+12] = sunset + i * nightstep;
	}

	currentHoraLord = PlanetId::None;
	for ( int i = 0; i < 24; i++ )
	{
		debuglog << "CALC hora lord: i " << i << " start " << formatter->formatUTDatetime( horaStart[i] )
		<< horaStart[i] << " next " << horaStart[i+1];
		if ( jd >= horaStart[i] && jd <= horaStart[i + 1 ] )
		{
			currentHoraLord = horaLord[i];
			break;
		}
	}

	if ( currentHoraLord == PlanetId::None )
	{
		errorlog << std::format( "Cannot determine current hora lord for jd {:18.12f}", jd );
	}

	// calculate lords of month and year
	const auto[ year, month, day ] = formatter->calculateDateIntegers( sunrise );

	// lord of 1st day of year
	varshaLord = weekday_lords[ getWeekDay( calculator->calc_jd( year, 1, 1, 12 ))];

	// lord of 1st day of month
	masaLord = weekday_lords[ getWeekDay( calculator->calc_jd( year, month, 1, 12 ))];
}

/*****************************************************
**
**   HoraExpert   ---   getHoraLordByIndex
**
******************************************************/
PlanetId HoraExpert::getHoraLordByIndex( const int &i ) const
{
	assert( i >= 0 && i < 24 );
	return horaLord[i];
}

/*****************************************************
**
**   HoraExpert   ---   getHoraStart
**
******************************************************/
double HoraExpert::getHoraStartByIndex( const int &i ) const
{
	assert( i >= 0 && i < 25 );
	return horaStart[i];
}

/*****************************************************
**
**   HoraExpert   ---   write
**
******************************************************/
void HoraExpert::write( MDocument_ptr doc, const bool &isLocaltime )
{
	debuglog << "HoraExpert::write isLocaltime " << std::boolalpha << isLocaltime;

	DateFormatter *dh = DateFormatter::get();
	Lang lang;
	constexpr Viewpoint viewpoint = Viewpoint::Vedic;

	Table *table = new Table( gdiconfig->view.showTextViewHeaders ? tr( "Hora" ) : wxString( "" ));
	table->addHeaderRow(
	{
		tr( "No." ),
		tr( "Begin" ),
		tr( "Lord" ),
		tr( "End" ),
		wxString( "" ),
		tr( "No." ),
		tr( "Begin" ),
		tr( "Lord" ),
		tr( "End" )
	});

	for ( int i = 0; i < 12; i++ )
	{
		TableRow row;
		row.addInteger( i + 1 );
		row.addValue( dh->formatTime( horaStart[i], isLocaltime ));
		row.addValue( MToken( horaLord[i], viewpoint ));
		row.addValue( dh->formatTime( horaStart[i+1], isLocaltime ));
		row.addValue( "" );

		row.addInteger( i + 13 );
		row.addValue( dh->formatTime( horaStart[i+12], isLocaltime ));
		row.addValue( MToken( horaLord[i+12], viewpoint ));
		row.addValue( dh->formatTime( horaStart[i+13], isLocaltime ));

		table->addRow( row );
	}
	doc->addItem( table );

	doc->addLine( std::format( "{}: {}", tr( "Sunrise" ), dh->formatDatetime( horaStart[0], isLocaltime )));
	doc->addLine( std::format( "{}: {}", tr( "Sunset" ), dh->formatDatetime( horaStart[12], isLocaltime )));
	doc->addLine( std::format( "{}: {}", tr( "Next sunset" ), dh->formatDatetime( horaStart[24], isLocaltime )));
	doc->addLine( std::format( "{}: {}", tr( "Lord of the day" ), lang.getPlanetName( getDinaLord(), viewpoint )));
	doc->addLine( "" );

	doc->addLine( std::format( "{}: {}", tr( "Lord of the month" ), lang.getPlanetName( getMasaLord(), viewpoint )));
	doc->addLine( std::format( "{}: {}", tr( "Lord of the year" ), lang.getPlanetName( getVarshaLord(), viewpoint )));
}

