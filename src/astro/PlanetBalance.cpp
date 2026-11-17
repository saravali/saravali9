/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/PlanetBalance.cpp
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

#include "PlanetBalance.h"

#include "astrobase.h"
#include "ContextProperties.h"
#include "Horoscope.h"
#include "gdiconfig.h"
#include "Lang.h"
#include "MDocument.h"
#include "Table.h"

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   PlanetBalanceExpert   ---   clear
**
******************************************************/
void PlanetBalanceExpert::clear()
{
	element_map.clear();
	quality_map.clear();
	chartside_map.clear();
}

/*****************************************************
**
**   PlanetBalanceExpert   ---   update
**
******************************************************/
void PlanetBalanceExpert::update()
{
	clear();
	const Viewpoint viewpoint = getViewpoint();
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );

	for( const auto &o : props->getPlanets( viewpoint, { FilterGroup::Houses } ))
	{
		if ( PlanetHelper::isCardinalPoint( o )) continue;
		const double longitude = getLongitude( o );

		const Element elem = getElement( longitude );
		element_map[elem].push_back( o );

		const SignQuality qual = getSignQuality( longitude );
		quality_map[qual].push_back( o );

		if ( PlanetHelper::isAngle( o )) continue;

		const House house = horoscope->getHousePos( o, viewpoint );
		const std::pair<ChartSide,ChartSide> sides = getChartSides( house );
		chartside_map[sides.first].push_back( o );
		chartside_map[sides.second].push_back( o );
	}
}

/*****************************************************
**
**   PlanetBalanceExpert   ---   createPlanetList
**
******************************************************/
MString PlanetBalanceExpert::createPlanetList( const Planets &obs )
{
	//const TextFormat format = gdiconfig->view.textFormat;
	MString ms;
	for( const PlanetId ob : obs )
	{
		if ( ! ms.isEmpty()) ms << ", ";
		ms << MToken( ob, planetCfg );
	}
	return ms;
}

/*****************************************************
**
**   PlanetBalanceExpert   ---   getItemValue
**
******************************************************/
std::pair<wxString, MString> PlanetBalanceExpert::getItemValue( const std::variant<Element,SignQuality,ChartSide> &a )
{
	Lang lang;
	MString ms;

	if ( std::holds_alternative<Element>( a ))
	{
		const Element elem = std::get<0>( a );
		return( std::make_pair( lang.getElementName( elem ), createPlanetList( element_map[elem] )));
	}
	else if ( std::holds_alternative<SignQuality>( a ))
	{
		const SignQuality qual = std::get<1>( a );
		return( std::make_pair( lang.getSignQualityName( qual ), createPlanetList( quality_map[qual] )));
	}
	else if ( std::holds_alternative<ChartSide>( a ))
	{
		const ChartSide side = std::get<2>( a );
		return( std::make_pair( lang.getChartSideName( side ), createPlanetList( chartside_map[side] )));
	}
	assert( false );
	return( std::make_pair( "<error>", MString( "<error>" )));
}

/*****************************************************
**
**   PlanetBalanceExpert   ---   write
**
******************************************************/
void PlanetBalanceExpert::write( MDocument_ptr doc, const bool /*show_header*/ )
{
	doc->addHeader1( tr( "Planet Balance" ));

	{
		Table *t = new Table( 2 );
		t->setTitle( tr( "Elements" ));
		for( const Element elem : Enum<Element>() )
		{
			const std::pair<wxString, MString> p = getItemValue( elem );
			t->addRow( { MToken( p.first ), p.second } );
		}
		doc->addItem( t );
	}
	{
		Table *t = new Table( 2 );
		t->setTitle( tr( "Sign Qualities" ));
		for( const SignQuality qual : Enum<SignQuality>() )
		{
			const std::pair<wxString, MString> p = getItemValue( qual );
			t->addRow( { MToken( p.first ), p.second } );
		}
		doc->addItem( t );
	}
	{
		Table *t = new Table( 2 );
		t->setTitle( tr( "Chart Sides" ));
		for( const ChartSide side : Enum<ChartSide>() )
		{
			const std::pair<wxString, MString> p = getItemValue( side );
			t->addRow( { MToken( p.first ), p.second } );
		}
		doc->addItem( t );
	}
}


