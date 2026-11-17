/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/DiamondChart.cpp
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

#include "CeFieldGrid.h"

#include "ChartConfig.h"
#include "ContextProperties.h"
#include "HybridChart.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "Painter.h"
#include "SymbolProvider.h"
#include "DocHelper.h"
#include "Varga.h"

#include <format>

/*************************************************//**
*
*   Chart implementation for North Indian (Diamond) charts.
*
******************************************************/
class DiamondChart : public VedicChartGrid
{
public:

	/*****************************************************
	**
	**   DiamondChart   ---   Constructor
	**
	******************************************************/
	DiamondChart( HybridChart *chart, CgVedicChartGrid *torus )
		: VedicChartGrid( chart, torus )
	{
		fields =
		{
			ChartField( chart, Align::South, FieldType::Diamond ),
			ChartField( chart, Align::South, FieldType::North ),
			ChartField( chart, Align::East, FieldType::West ),

			ChartField( chart, Align::East, FieldType::Diamond ),
			ChartField( chart, Align::East, FieldType::West ),
			ChartField( chart, Align::North, FieldType::South ),

			ChartField( chart, Align::North, FieldType::Diamond ),
			ChartField( chart, Align::North, FieldType::South ),
			ChartField( chart, Align::West, FieldType::East ),

			ChartField( chart, Align::West, FieldType::Diamond ),
			ChartField( chart, Align::West, FieldType::East ),
			ChartField( chart, Align::South, FieldType::North )
		};
	}

protected:
	/*****************************************************
	**
	**   DiamondChart   ---   setupFieldRectangles
	**
	******************************************************/
	virtual void setupFieldRectangles() override
	{
		Horoscope_ptr h = chart->getFirstHoroscope();
		debuglog << "DiamondChart::setupFieldRectangles";
		CgVedicChartGrid *cg = getCg();

		if ( h )
		{
			const Viewpoint viewpoint = chart->getViewpoint();
			VargaExpert expert( h.get(), viewpoint );
			ContextProps_ptr props = chart->getProps();
			positionOffset = (int)expert.calcVarga( h->getLongitude( PlanetId::Ascendant,
				props->getEffectiveViewpoint( Viewpoint::Vedic )),
				chart->getVarga() );
			debuglog << "DiamondChart::setupFieldRectangles offset " << positionOffset;
		}
		else
		{
			positionOffset = 0;
		}

		const MComplex center = chart->getCenter();
		const MComplex nw_outer = discPointToXy( 135, cg );
		const double xr = center.real() - nw_outer.real();
		const double yr = center.imag() - nw_outer.imag();
		const double xi = .5 * xr;
		const double yi = .5 * yr;
		debuglog << "DiamondChart::setupFieldRectangles offset " << positionOffset << " center " << center << " h " << h.get();

		const MRect rects[12] =
		{
			MRect( center.real() - xi, center.imag() - yr, xr, yr ),
			MRect( center.real() - xr, center.imag() - yr, xr, yi ),
			MRect( center.real() - xr, center.imag() - yr, xi, yr ),

			MRect( center.real() - xr, center.imag() - yi, xr, yr ),
			MRect( center.real() - xr, center.imag(), xi, yr ),
			MRect( center.real() - xr, center.imag() + yi, xr, yi ),

			MRect( center.real() - xi, center.imag(), xr, yr ),
			MRect( center.real(), center.imag() + yi, xr, yi ),
			MRect( center.real() + xi, center.imag(), xi, yr ),

			MRect( center.real(), center.imag() - yi, xr, yr ),
			MRect( center.real() + xi, center.imag() - yr, xi, yr ),
			MRect( center.real(), center.imag() - yr, xr, yi )
		};

		assert( fields.size() == 12 );
		for( uint i = 0; i < 12; i++ ) fields[i].rect = rects[i];

		pFieldSplit = MComplex( .25 * fields[1].rect.width, .5 * fields[1].rect.height );
	}

	/*****************************************************
	**
	**   DiamondChart   ---   paintGrid
	**
	******************************************************/
	virtual void paintGrid( Painter *painter, const MRect& /*refreshRect*/ ) override
	{
		// the rectangle around center item will be left out
		const MComplex center = chart->getCenter();
		const double r = .5 * std::min( centerItem.rect.width, centerItem.rect.height );
		debuglog << "DiamondChart::paintGrid rxymax " << chart->getXyMax() << " center rect" << r;

		const std::list<std::pair<MComplex,MComplex>> l = {
			{ p_north, p_east },
			{ p_east, p_south },
			{ p_south, p_west },
			{ p_west, p_north },
			{ p_northwest, MComplex( center.real() - r, center.imag() - r ) },
			{ p_northeast, MComplex( center.real() + r, center.imag() - r ) },
			{ p_southwest, MComplex( center.real() - r, center.imag() + r ) },
			{ p_southeast, MComplex( center.real() + r, center.imag() + r ) }
		};
		for( const std::pair<MComplex,MComplex> &p : l )
		{
			debuglog << "paintGrid  " << p.first << " /" << p.second;
			painter->drawLine( p.first, p.second );
		}

#if 0
		MPen tpen = getPen();
		tpen.style = PenStyle::ShortDash;
		tpen.style = PenStyle::LongDash;
		tpen.color = MColor ( "orange" );
		painter->setPen( tpen );
		for( int i = 1; i < 2; i++ )
		{
			painter->drawRectangle( fields[i].rect );
		}
		setInheritedStyle();
#endif

		if( chart->getChartConfig()->northIndianSignDisplayType == NorthIndianChartType::Symbol )
		{
			chart->setSymbolFont( painter );
		}
		else
		{
			chart->setGraphicFont( painter );
		}

		constexpr double rtol = 12;
		DocHelper spt( painter );
		MRect rect;

		// draw sign numbers resp names into the chart
		for( int i = 0; i < 12; i++ )
		{
			rect = fields[i].rect;
			rect.grow( -rtol );

			spt.formatAndDrawTextItem( getNorthIndianSignLabel( (Sign)red12( positionOffset + i )), rect, fields[i].gravitation );
		}
		chart->setGraphicFont( painter );
	}

	/*****************************************************
	**
	**   DiamondChart   ---   getNorthIndianSignLabel
	**
	******************************************************/
	MString getNorthIndianSignLabel( const Sign &rasi )
	{
		switch( chart->getChartConfig( WidgetCategory::VargaChart )->northIndianSignDisplayType )
		{
			case NorthIndianChartType::Ascendant:
				return MString( std::format( "{}", (int)rasi + 1 ));
			break;
			case NorthIndianChartType::Number:
				return MString( std::format( "{}", (int)rasi + 1 ));
			break;
			case NorthIndianChartType::Short:
				return MString( Lang().getSignName( rasi, chart->getViewpoint() ));
			break;
			default:
				return MToken( rasi, chart->getViewpoint());
			break;
		}
	}

};

/*****************************************************
**
**   Grid element factory methods
**
******************************************************/
ChartElementBase *ChartElementFactory::createDiamondChartGrid( CgElement *cge ) const
{
	CgVedicChartGrid *grid = dynamic_cast<CgVedicChartGrid*>( cge );
	assert( grid );
	return new DiamondChart( chart, grid );
}


