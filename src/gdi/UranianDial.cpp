/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/UranianDial.cpp
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

#include "astroconfig.h"
#include "ChartConfig.h"
#include "ChartElementImpl.h"
#include "ConfigListItems.h"
#include "DocHelper.h"
#include "Formatter.h"
#include "gdiconfig.h"
#include "HybridChart.h"
#include "Lang.h"
#include "mathbase.h"
#include "Painter.h"
#include "SymbolProvider.h"

#include <format>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define DEBUG_URANIANDIAL

/*************************************************//**
*
* 
*
******************************************************/
class UranianDial : public virtual ChartElement<CgUranianDial>
{
public:

	UranianDial( HybridChart *chart, CgUranianDial *dial )
		: ChartElement( chart, dial ) {}

	/*****************************************************
	**
	**   UranianDial   ---   paint
	**
	******************************************************/
	virtual void paint( Painter *painter, const MRect& /*refreshRect*/ ) override
	{
		UranianSection *usection = chart->getUranianSection();
		const int division = usection ? usection->chartDivision : 1;
		assert( division );
		CgUranianDial *dial = getCg();
		assert( dial );

#ifdef DEBUG_URANIANDIAL
		debuglog << "UranianDial::paint division " << division << " radius " << dial->radius;
#endif

		if ( ! dial->show ) return;
		setInheritedStyle( painter );
		//chart->setGraphicFont( painter, dial->labelSize );

#ifdef DEBUG_URANIANDIAL
		debuglog << "UranianDial::paint type " << (int)dial->getElementType()
			<< " division " << division;
#endif

		paintTNDisc( painter, dial, division );
		ShowChartCenterLabel( painter, dial, division );
	}

protected:

	/*****************************************************
	**
	**   UranianDial   ---   paintTNDisc
	**
	******************************************************/
	void paintTNDisc( Painter *painter, CgUranianDial *dial, const int &division )
	{
		assert( division >= 1 );
#ifdef DEBUG_URANIANDIAL
		debuglog << "UranianDial::paintTNDisc division " << division << " residuum " << ( 12 % division );
#endif

		paintFieldPatterns( painter, division );

		// D-2, D-3, D-4
		if ( division <= 4 ) paintSignSymbols( painter, dial, division );

		setInheritedStyle( painter );
		paintDisc( painter, *dial );
	}

	/*****************************************************
	**
	**   UranianDial   ---   paintSignSymbols
	**
	******************************************************/
	void paintSignSymbols( Painter *painter, CgUranianDial *dial, const int &division )
	{
#ifdef DEBUG_URANIANDIAL
		debuglog << "UranianDial::paintSignSymbols division " << division;
#endif
		assert( division > 0 && division <= 12 );
		assert( ! ( 12 % division ));
		if ( division > 1 && ! dial->showHigherDivisionSigns ) return;

		SymbolProvider sp;
		Lang lang;
		ChartConfig *chartCfg = chart->getChartConfig();

		// TODO hängt ab von Länge des Strings, Radius und Schriftgröße
		constexpr double shiftAngle = 15;

		const PlanetPropertyConfig &pCfg = astroconfig->getViewpointConfig( chart->getViewpoint() ).planetProps;

		const double labelSize = dial->labelSize;
		if ( dial->useSymbols )
			chart->setSymbolFont( painter, labelSize );
		else
			chart->setGraphicFont( painter, labelSize );

		const int field_count = 12 / division;  // eg division = 4 -> field_count = 3
		const int angle_step = 360 / field_count; // eg division= 4 -> angle_step = 90

		const WidgetSkin &skin = chart->getSkin();
		const ChartFieldPattern pattern = skin.signPattern;

		double angle = 0;
		for( int i = 0; i < field_count; i++ )
		{
			angle = ( i + .5 ) * angle_step; //center of field
			angle -= ((double)( division - 1 ) / 2.0 ) * shiftAngle;

			for( int j = 0; j < division; j++ )
			{
				const Sign sign = (Sign)( j * field_count + i );
				assert_valid_enum<Sign>( sign );

				if ( chartCfg->showSignColors )
				{
					painter->setTextColor( gdiconfig->chartColors.getSignFgColor( sign ));
				}

				const MComplex p = discPointToXy( angle, dial->rLabels, dial->geometryType );
				if ( dial->useSymbols )
				{
					painter->drawRotatedText( sp.getSignCode( sign, pCfg.capricornSymbol ), p.real(), p.imag(), angle - 90 );
				}
				else
				{
					painter->drawRotatedText( lang.getSignName( sign, chart->getViewpoint() ), p.real(), p.imag(),
						dial->rotateLabels ? angle - 90 : 0 );
				}
				angle += shiftAngle;
			}
		}
	}

	/*****************************************************
	**
	**   UranianDial   ---   paintFieldPatterns
	**
	******************************************************/
	void paintFieldPatterns( Painter *painter, const int &division )
	{
		CgUranianDial *dial = getCg();
		assert( chart );
		assert( division );

		double patternOffset = 1;
		if ( division <= 4 )
		{
			patternOffset = 30.0;
		}
		else if ( division <= 16 )
		{
			patternOffset = 5.0;
		}
		const double stepWidth = patternOffset * division;
#ifdef DEBUG_URANIANDIAL
		debuglog << "UranianDial::paintFieldPatterns division " << division << " patternOffset " << patternOffset << " stepWidth " << stepWidth;
#endif

		setInheritedStyle( painter );
		chart->setGraphicFont( painter, dial->degreeLabelSize );
		const WidgetSkin &skin = chart->getSkin();
		const ChartFieldPattern *pattern = &skin.signPattern;

		CgAnnulus annulus( CgElementType::Annulus, dial->radius,
			division > 1 ? dial->higherDivisionInnerRadius : dial->innerRadius );
		int i = 0;
		double d = 0;
		while( d < 360 )
		{
			if ( pattern->isValid())
			{
				painter->setBrush( pattern->getBrush( i ));
			}
			ChartElementBase::drawAnnulusSegment( painter, &annulus, d, std::min( d + stepWidth, 360.0 ));
			//debuglog << "UranianDial::paintFieldPatterns drawAnnulusSegment from " << d << " to " << std::min( d + stepWidth, 360.0 );

#if 0
			{
				MComplex p = discPointToXy( d, dial->radius, dial->geometryType );
				painter->drawLine( chart->getCenter(), p );
			}
#endif

			if ( division > 1 && i )
			{
				const MComplex p = discPointToXy( red_deg( d ), dial->rDegreeLabels, dial->geometryType );
				painter->drawRotatedText( std::format( "{}", (int)( i * patternOffset )), p.real(), p.imag(), d - 90 );
			}

			++i;
			d += stepWidth;
		}

		setInheritedStyle( painter );
	}

	/*****************************************************
	**
	**   UranianDial   ---   ShowChartCenterLabel
	**
	******************************************************/
	void ShowChartCenterLabel( Painter *painter, CgUranianDial *dial, const int &division )
	{
		assert( division > 0 );
		MString s;
		switch( chart->getChartConfig()->chartCenterType )
		{
			case 1: // Division D-N
				s << "D-" << std::format( "{}", division );
			break;

			case 2: // Total Degrees
				s << getChartCenterDegreeString( division );
			break;

			case 3: // D-N + Total Degrees
				s << "D-" << std::format( "{}", division ) << MToken( TokenId::LineBreak ) << getChartCenterDegreeString( division );
			break;

			case 0: // nothing
			default:
				return;
			break;
		}
		if ( ! s.isEmpty())
		{
#ifdef DEBUG_URANIANDIAL
			debuglog << "UranianDial::ShowChartCenterLabel label size " << dial->degreeLabelSize;
#endif
			chart->setGraphicFont( painter, dial->degreeLabelSize );
			DocItemText centerItem = DocItemText( s, chart->getRect(), Align::Center );
			DocHelper helper( painter );
			helper.formatAndFitTextItem( centerItem );
			helper.drawTextItem( centerItem );
		}
	}

	/*****************************************************
	**
	**   UranianDial   ---   getChartCenterDegreeString
	**
	******************************************************/
	MString getChartCenterDegreeString( const int division )
	{
		assert( division );
		MString s;
		const double deg = 360.0 / (double)division;
		assert( deg > 0 );

		if ( deg == (int)deg )
		{
			s << std::format( "{}", (int)deg ) << "°";
		}
		else
		{
			s << Formatter::get()->formatDegrees( deg, DegPrecision::FlexibleCutMinute );
		}
		return s;
	}

};

/*****************************************************
**
**   Grid element factory
**
******************************************************/
ChartElementBase *ChartElementFactory::createUranianDial( CgElement *cge ) const
{
	CgUranianDial *dial = dynamic_cast<CgUranianDial*>( cge );
	assert( dial );
	return new UranianDial( chart, dial );
}

