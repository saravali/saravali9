/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/SouthIndianChart.cpp
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

/*************************************************//**
*
*   Chart implementation for South Indian charts.
*
******************************************************/
class SouthIndianChart : public VedicChartGrid
{
public:
	/*****************************************************
	**
	**   SouthIndianChart   ---   Constructor
	**
	******************************************************/
	SouthIndianChart( HybridChart *chart, CgVedicChartGrid *torus )
		: VedicChartGrid( chart, torus )
	{
		fields =
		{
			ChartField( chart, Align::South ),
			ChartField( chart, Align::South ),
			ChartField( chart, Align::SouthWest ),
			ChartField( chart, Align::West ),
			ChartField( chart, Align::West ),
			ChartField( chart, Align::NorthWest ),

			ChartField( chart, Align::North ),
			ChartField( chart, Align::North ),
			ChartField( chart, Align::NorthEast ),
			ChartField( chart, Align::East ),
			ChartField( chart, Align::East ),
			ChartField( chart, Align::SouthEast )
		};
	}

protected:
	/*****************************************************
	**
	**   SouthIndianChart   ---   setupFieldRectangles
	**
	******************************************************/
	virtual void setupFieldRectangles() override
	{
		debuglog << "SouthIndianChart::setupFieldRectangles";

		{
			constexpr double l0 = 20;
			CgVedicChartGrid *cg = getCg();

			// start of gemini etc. in clockwise order
			p_gemini = discPointToXy( 90 - l0, cg );

			// start of libra
			p_libra = discPointToXy( 270 + l0, cg );
			p_sagittarius = discPointToXy( 270 - l0, cg );
			p_aries = discPointToXy( 90 + l0, cg );

			p_cancer = discPointToXy( l0, cg );
			p_pisces = discPointToXy( 180 - l0, cg );
			p_virgo = discPointToXy( - l0, cg );
			p_capricorn = discPointToXy( 180 + l0, cg );
		}

		p_north_inner = MComplex( p_north.real(), p_pisces.imag());
		p_east_inner = MComplex( p_gemini.real(), p_east.imag());
		p_south_inner = MComplex( p_south.real(), p_virgo.imag());
		p_west_inner = MComplex( p_aries.real(), p_west.imag());

		const MRect rects[12] =
		{
			// aries etc
			MRect( p_aries, p_north_inner ),
			MRect( p_gemini, p_north_inner ),
			MRect( p_gemini, p_cancer ),

			// cancer etc
			MRect( p_cancer, p_east_inner ),
			MRect( p_east_inner, p_virgo ),
			MRect( p_virgo, p_libra ),

			// libra etc
			MRect( p_south_inner, p_libra ),
			MRect( p_south_inner, p_sagittarius ),
			MRect( p_capricorn, p_sagittarius ),

			// capricorn etc
			MRect( p_capricorn, p_west_inner ),
			MRect( p_pisces, p_west_inner ),
			MRect( p_pisces, p_aries )
		};
		assert( fields.size() == 12 );
		for( uint i = 0; i < 12; i++ )
		{
			fields[i].rect = rects[i];
			//fields[i].label = MToken( (Sign)i);
		}
		pFieldSplit = MComplex( .5 * fields[0].rect.width, .5 * fields[0].rect.height );
	}

	/*****************************************************
	**
	**   SouthIndianChart   ---   paintGrid
	**
	******************************************************/
	virtual void paintGrid( Painter *painter, const MRect& /*refreshRect*/ ) override
	{
		painter->drawLine( p_north, p_north_inner );
		painter->drawLine( p_south_inner, p_south );

		painter->drawLine( p_gemini, p_libra );
		painter->drawLine( p_sagittarius, p_aries );

		painter->drawLine( p_east, p_east_inner );
		painter->drawLine( p_west_inner, p_west );

		painter->drawLine( p_pisces, p_cancer );
		painter->drawLine( p_capricorn, p_virgo );

		// paint diagonal field for ascendant if required
		if( chart->getChartConfig()->southIndianAscendantMarkup != 0 )
		{
			const int asc = proxy->getPlanetField( PlanetId::Ascendant, chart->getFirstRole() );
			if ( asc != -1 )
			{
				assert( asc >= 0 && asc < 12 );
				const MRect &rr = fields[asc].rect;
				wxPen pen = getPen();
				if ( chart->getChartConfig()->southIndianAscendantMarkup == 2 ) 
				{
					pen.SetStyle( wxPENSTYLE_DOT );
					painter->setPen( pen );
				}
				painter->drawLine( MComplex( rr.x, rr.y + rr.height ), MComplex( rr.x + rr.width, rr.y ));
				setInheritedStyle( painter );
			}
		}
	}

	MComplex p_gemini, p_libra, p_sagittarius, p_aries;
	MComplex p_cancer, p_pisces, p_virgo, p_capricorn;

	MComplex p_north_inner, p_west_inner, p_south_inner, p_east_inner;
};

/*****************************************************
**
**   Grid element factory methods
**
******************************************************/
ChartElementBase *ChartElementFactory::createSouthIndianChartGrid( CgElement *cge ) const
{
	CgVedicChartGrid *grid = dynamic_cast<CgVedicChartGrid*>( cge );
	assert( grid );
	return new SouthIndianChart( chart, grid );
}

