/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/WesternHouses.cpp
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
#include "ContextProperties.h"
#include "gdiconfig.h"
#include "HybridChart.h"
#include "Horoscope.h"
#include "logger.h"
#include "mathbase.h"
#include "UranianHelper.h"

extern Gdiconfig *gdiconfig;

//#define DEBUG_WESTERNHOUSES

/*************************************************//**
*
* 
*
******************************************************/
class WesternHouses : public CeLabelledAnnulus
{
	friend class UranianDisk;
public:

	WesternHouses( HybridChart *chart, CgLabelledAnnulus *annulus )
		: CeLabelledAnnulus( chart, annulus, 12 )
	{
		usection = chart->getUranianSection();

		// no symbols available
		assert( annulus );
		annulus->useSymbols = false;
	}

	virtual void onCoordinatesChanged() override
	{
		if ( !  chart->getFirstHoroscope()) return;

		const Viewpoint &viewpoint = chart->getViewpoint();
		const PlanetId planetId = usection ? usection->houseSystemPlanetId : PlanetId::Meridian;

		houses = UranianHelper::get()->calculateHouses( chart->getFirstHoroscope(), viewpoint, planetId );
	}

protected:

	virtual bool isPaintable() const override
	{
		return chart->getFirstHoroscope() != nullptr;
	}

	virtual std::pair<wxString, wxColour> getLabel( const int &f ) override
	{
		ChartConfig *cfg = chart->getChartConfig( WidgetCategory::WesternChart );
		return std::make_pair(
			getHouseNumberFormatted( (House)f, cfg->houseNumberStyle ),
				cfg->showHouseColors ?
				gdiconfig->chartColors.getHouseFgColor( (House)f )
				: wxColour());
	}

	const ChartFieldPattern *getFieldPattern() const override
	{
		assert( chart );
		const WidgetSkin &skin = chart->getSkin();
		return &skin.housePattern;
	}

	/*****************************************************
	**
	**   WesternHouses   ---   getSegmentStartLongitude
	**
	******************************************************/
	virtual double getSegmentStartLongitude( const int &f ) override
	{
		assert( f >= 0 && f < 12 );

		return red_deg( chart->getAries() + houses[f] );
	}

	std::array<double, 12 > houses = { 0.0 };

	UranianSection *usection;
};

/*****************************************************
**
**   Grid element factory methods
**
******************************************************/
ChartElementBase *ChartElementFactory::createHouses( CgElement *cge ) const
{
	CgLabelledAnnulus *annulus = dynamic_cast<CgLabelledAnnulus*>( cge );
	assert( annulus );
	return new WesternHouses( chart, annulus );
}
