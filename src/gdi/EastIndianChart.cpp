/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/EastIndianChart.cpp
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
*   Chart implementation for East Indian charts.
*
******************************************************/
class EastIndianChart : public VedicChartGrid
{
public:
	/*****************************************************
	**
	**   EastIndianChart   ---   Constructor
	**
	******************************************************/
	EastIndianChart( HybridChart *chart, CgVedicChartGrid *torus )
		: VedicChartGrid( chart, torus )
	{
		fields =
		{
			ChartField( chart, Align::South, FieldType::Full ),
			ChartField( chart, Align::SouthEast, FieldType::NorthEast ),
			ChartField( chart, Align::SouthEast, FieldType::SouthWest ),

			ChartField( chart, Align::East, FieldType::Full),
			ChartField( chart, Align::NorthEast, FieldType::NorthWest ),
			ChartField( chart, Align::NorthEast, FieldType::SouthEast ),

			ChartField( chart, Align::North, FieldType::Full),
			ChartField( chart, Align::NorthWest, FieldType::SouthWest ),
			ChartField( chart, Align::NorthWest, FieldType::NorthEast ),

			ChartField( chart, Align::West, FieldType::Full),
			ChartField( chart, Align::SouthWest, FieldType::SouthEast),
			ChartField( chart, Align::SouthWest, FieldType::NorthWest )
		};
	}

protected:
	/*****************************************************
	**
	**   EastIndianChart   ---   setupFieldRectangles
	**
	******************************************************/
	virtual void setupFieldRectangles() override
	{
		//debuglog << "EastIndianChart::setupFieldRectangles straightFrame " << cg->straightFrame;
		{
			constexpr double l0 = 12;
			CgVedicChartGrid *cg = getCg();

			p_aries = discPointToXy( 90 - l0, cg );
			p_taurus = discPointToXy( 90 + l0, cg );
			p_gemini = discPointToXy( 135, cg );
			p_cancer = discPointToXy( 180 - l0, cg );
			p_leo = discPointToXy( 180 + l0, cg );
			p_virgo = discPointToXy( 225, cg );
			p_libra = discPointToXy( 270 - l0, cg );
			p_scorpio = discPointToXy( 270 + l0, cg );
			p_sagittarius = discPointToXy( 315, cg );
			p_capricorn = discPointToXy( - l0, cg );
			p_aquarius = discPointToXy( l0, cg );
			p_pisces = discPointToXy( 45, cg );
		}

		cgemini = MComplex( p_taurus.real(), p_cancer.imag());
		cvirgo = MComplex( p_libra.real(), p_leo.imag());
		csagittarius = MComplex( p_scorpio.real(), p_capricorn.imag());
		cpisces = MComplex( p_aries.real(), p_aquarius.imag());

		const std::pair<MComplex,MComplex> rects[12] =
		{
			{ p_taurus, cpisces },
			{ p_gemini, cgemini },
			{ p_gemini, cgemini },

			{ p_cancer, cvirgo },
			{ p_virgo, cvirgo },
			{ p_virgo, cvirgo },

			{ cvirgo, p_scorpio },
			{ csagittarius, p_sagittarius },
			{ csagittarius, p_sagittarius },

			{ cpisces, p_capricorn },
			{ p_pisces, cpisces },
			{ p_pisces, cpisces }
		};
		assert( fields.size() == 12 );
		for( uint i = 0; i < 12; i++ ) fields[i].rect = MRect( rects[i].first, rects[i].second );

		pFieldSplit = MComplex( .5 * fields[1].rect.width, .5 * fields[1].rect.height );
	}

	/*****************************************************
	**
	**   EastIndianChart   ---   paintGrid
	**
	******************************************************/
	virtual void paintGrid( Painter *painter, const MRect& /*refreshRect*/ ) override
	{
		CgVedicChartGrid *cg = getCg();

		std::list<std::pair<MComplex,MComplex>> l;
		l = {
			{ p_taurus, p_libra },
			{ p_cancer, p_aquarius },
			{ p_leo, p_capricorn },
			{ p_aries, p_scorpio },
		};
		for( const auto &p : l )
		{
			painter->drawLine( p.first, p.second );
		}

		painter->drawLine( p_gemini, cgemini );
		painter->drawLine( p_virgo, cvirgo );
		painter->drawLine( p_sagittarius, csagittarius );
		painter->drawLine( p_pisces, cpisces );
		paintDisc( painter, *cg );
	}

	MComplex p_aries, p_taurus, p_gemini,
		p_cancer, p_leo, p_virgo,
		p_libra, p_scorpio, p_sagittarius,
		p_capricorn, p_aquarius, p_pisces,
		cgemini, cvirgo, csagittarius, cpisces;
};

/*****************************************************
**
**   Grid element factory methods
**
******************************************************/
ChartElementBase *ChartElementFactory::createEastIndianChartGrid( CgElement *cge ) const
{
	CgVedicChartGrid *grid = dynamic_cast<CgVedicChartGrid*>( cge );
	assert( grid );
	return new EastIndianChart( chart, grid );
}

