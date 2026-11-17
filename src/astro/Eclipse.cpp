/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Eclipse.cpp
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

#include "Eclipse.h"

#include "Calculator.h"
#include "ContextProperties.h"
#include "DateFormatter.h"
#include "Formatter.h"
#include "constants.h"
#include "Table.h"

#include <wx/string.h>

/*****************************************************
**
**   EclipseExpert   ---   update
**
******************************************************/
void EclipseExpert::update( const double &startjd, const double &endjd,
	const int &planet, const int& /* type */ )
{
	Calculator *calculator = CalculatorFactory().getCalculator();
	EclipseData d;
	int count = 0;
	const Viewpoint viewpoint = getViewpoint();

	if ( startjd >= endjd ) return;

	data.clear();

	/*
	* int planet is the value of the choice box
	* 0 : sun and moon
	* 1 : sun
	* 2 : moon
	*/
	if ( planet == 0 || planet == 1 )
	{
		d.jd = startjd;
		while ( d.jd < endjd && count < 100 )
		{
			d = calculator->calcNextEclipse( d.jd, PlanetId::Sun, 0, viewpoint );
			d.planet = PlanetId::Sun;
			if ( d.jd <= endjd ) data.push_back( d );
			count++;
		}
	}
	if ( planet == 0 || planet == 2 )
	{
		count = 0;
		d.jd = startjd;
		while ( d.jd < endjd && count < 100 )
		{
			d = calculator->calcNextEclipse( d.jd, PlanetId::Moon, 0, viewpoint );
			d.planet = PlanetId::Moon;
			if ( d.jd <= endjd ) data.push_back( d );
			count++;
		}
	}
	data.sort( []( const auto &e1, const auto &e2 ) { return ( e1.jd < e2.jd ); });
}

/*****************************************************
**
**   EclipseExpert   ---   write
**
******************************************************/
void EclipseExpert::write( MDocument_ptr doc )
{
	Formatter *formatter = Formatter::get();
	DateFormatter *dh = DateFormatter::get();
	const Viewpoint viewpoint = getViewpoint();

	Table *table = new Table( tr( "Eclipses" ));
	table->addHeaderRow(
	{
		tr( "Type" ),
		tr( "Date" ),
		tr( "Time" ),
		tr( "Sun" ),
		tr( "Moon" )
	});

	for ( const auto &e : data )
	{
		TableRow row;
		row.addValue( std::format( "{} {}", ( e.planet == PlanetId::Sun ? tr( "Solar" ) : tr( "Lunar" )), e.description ));

		auto fd = dh->formatDateAndTimePair( e.jd, props->isLocaltime );
		row.addValue( fd.first );
		row.addValue( fd.second );

		row.addValue( formatter->formatZodiacalLongitude( viewpoint, e.sunpos ));
		row.addValue( formatter->formatZodiacalLongitude( viewpoint, e.moonpos));
		table->addRow( row );
	}
	doc->addItem( table );
}


