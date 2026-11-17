/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/CeSigns.cpp
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

#include "CeLabelledAnnulus.h"

#include "astroconfig.h"
#include "ChartConfig.h"
#include "ConfigListItems.h"
#include "gdiconfig.h"
#include "HybridChart.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "Painter.h"
#include "SymbolProvider.h"

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

/*************************************************//**
*
* 
*
******************************************************/
class CeSigns : public CeLabelledAnnulus
{
public:

	CeSigns( HybridChart *chart, CgLabelledAnnulus *annulus )
		: CeLabelledAnnulus( chart, annulus, 12 )
	{
	}

protected:

	virtual std::pair<wxString, wxColour> getLabel( const int &f )
	{
		CgLabelledAnnulus *annulus = getCg();
		assert( annulus );
		debuglog << "CeSigns::paint useSymbols " << annulus->useSymbols;
		const Viewpoint viewpoint = chart->getViewpoint();

		const wxColour color = chart->getChartConfig( WidgetCategory::WesternChart )->showSignColors ?
			gdiconfig->chartColors.getSignFgColor( (Sign)f )
			: wxColour();

		if ( annulus->useSymbols )
		{
			const PlanetPropertyConfig &cfg = astroconfig->getViewpointConfig( chart->getViewpoint() ).planetProps;

			return std::make_pair(
				SymbolProvider().getSignCode( (Sign)f, cfg.capricornSymbol ), color );
		}
		else
		{
			return std::make_pair( Lang().getSignName( (Sign)f, viewpoint, annulus->textLength ), color );
		}
	}

	const ChartFieldPattern *getFieldPattern() const
	{
		assert( chart );
		const WidgetSkin &skin = chart->getSkin();
		return &skin.signPattern;
	}

	virtual double getSegmentStartLongitude( const int &f )
	{
		return red_deg( chart->getAries() + f * 30 );
	}
};

/*****************************************************
**
**   Grid element factory methods
**
******************************************************/
ChartElementBase *ChartElementFactory::createSigns( CgElement *cge ) const
{
	CgLabelledAnnulus *annulus = dynamic_cast<CgLabelledAnnulus*>( cge );
	assert( annulus );
	return new CeSigns( chart, annulus );
}

