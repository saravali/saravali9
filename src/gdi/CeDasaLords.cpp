/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/CeDasaLords.cpp
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

#include "ChartConfig.h"
#include "ConfigListItems.h"
#include "gdiconfig.h"
#include "GenericDasa.h"
#include "HybridChart.h"
#include "mathbase.h"

extern Gdiconfig *gdiconfig;

/*************************************************//**
*
* 
*
******************************************************/
class CeDasaLords : public CeLabelledAnnulus
{
public:

	CeDasaLords( HybridChart *chart, CgLabelledAnnulus *annulus )
		: CeLabelledAnnulus( chart, annulus, 27 )
	{
		// no symbols available
		assert( annulus );
		annulus->useSymbols = false;
	}

protected:

	virtual std::pair<wxString, wxColour> getLabel( const int &f )
	{
		CgLabelledAnnulus *annulus = getCg();
		assert( annulus );

		DasaExpert_ptr eptr( DasaExpertFactory().getDasaExpert( DasaId::Vimsottari ));
		GenericNakshatraDasaExpert* expert = dynamic_cast<GenericNakshatraDasaExpert*>( eptr.get());

		const int dasaindex = expert->getDasaIndex4Nakshatra( (Nakshatra27)f );
		const int years = expert->getDasaDuration( dasaindex );
		const Sign sign = (Sign)red12( f );

		return std::make_pair(
			std::format( "{} {}", expert->getDasaLordName( expert->getDasaLordByIndex( dasaindex )).toWxString(), years ),
			chart->getChartConfig( WidgetCategory::WesternChart )->showSignColors ?
				gdiconfig->chartColors.getSignFgColor( sign )
				: wxColour() );
	}

	const ChartFieldPattern *getFieldPattern() const
	{
		assert( chart );
		const WidgetSkin &skin = chart->getSkin();
		return &skin.nakshatraPattern;
	}

	virtual double getSegmentStartLongitude( const int &f )
	{
		return red_deg( chart->getAries() + f * NAKSHATRA_LEN );
	}
};

/*****************************************************
**
**   Grid element factory methods
**
******************************************************/
ChartElementBase *ChartElementFactory::createDasaLords( CgElement *cge ) const
{
	CgLabelledAnnulus *annulus = dynamic_cast<CgLabelledAnnulus*>( cge );
	assert( annulus );
	return new CeDasaLords( chart, annulus );
}

