/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/UranianDisk.cpp
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
#include <wx/bitmap.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/graphics.h>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define DEBUG_URANIANDISK
//#define USE_WXIMAGE

/*************************************************//**
*
* 
*
******************************************************/
class UranianDisk : public virtual ChartElement<CgUranianDisk>
{
public:

	/*****************************************************
	**
	**   UranianDisk   ---   Constructor
	**
	******************************************************/
	UranianDisk( HybridChart *chart, CgUranianDisk *disk )
		: ChartElement( chart, disk )
	{
		assert( disk );
	}
	
	/*****************************************************
	**
	**   UranianDisk   ---   Destructor
	**
	******************************************************/
	~UranianDisk()
	{
		if ( cghouses )delete cghouses;
		if ( cgsigns )delete cgsigns;
	}

	/*****************************************************
	**
	**   UranianDisk   ---   onCoordinatesChanged
	**
	******************************************************/
	virtual void onCoordinatesChanged() override
	{
		UranianSection *usection = chart->getUranianSection();
		const int division = usection ? usection->chartDivision : 1;
		CgUranianDisk *disk = getGeom();
		assert( disk );

		if ( division == 1 )
		{
			ChartElementFactory factory( chart );
			if ( ! cghouses )
			{
				cghouses = new CgLabelledAnnulus( CgElementType::Houses, disk->houseOuterRadius,
					disk->houseInnerRadius, disk->houseRLabels );
				cghouses->useSymbols = false;
				cghouses->rotateLabels = true;
				houses = factory.createChartElement( cghouses );
			}
			houses->onCoordinatesChanged();

			if ( ! cgsigns )
			{
				cgsigns = new CgLabelledAnnulus( CgElementType::Signs, disk->signOuterRadius, disk->signInnerRadius, disk->signRLabels );
				cgsigns->labelSize = 200;
				cgsigns->rotateLabels = true;
				cgsigns->degreeMarkerCfg.show  = false;
				signs = factory.createChartElement( cgsigns );
			}
		}
#ifdef USE_WXIMAGE
		drawImage();
#endif
	}

	/*****************************************************
	**
	**   UranianDisk   ---   paint
	**
	******************************************************/
	virtual void paint( Painter *painter, const MRect& refreshRect ) override
	{
		CgUranianDisk *disk = getGeom();
		assert( disk );
		if ( ! disk->show ) return;

#ifdef USE_WXIMAGE
		painter->drawBitmap( bitmap, 0, 0 );
#else
		paintItems( painter, refreshRect );
#endif
	}

protected:

	/*****************************************************
	**
	**   UranianDisk   ---   drawImage
	**
	******************************************************/
	void drawImage()
	{
		CgUranianDisk *disk = getGeom();
		assert( disk );
		assert( chart );

		const wxSize size( chart->getRect().width, chart->getRect().height );

#ifdef DEBUG_URANIANDISK
		debuglog << "UranianDisk::drawImage size " << size;
#endif

		wxImage image( size );
		image.SetAlpha();
		bitmap = wxBitmap( image );

		wxMemoryDC dc;
		dc.SelectObject( bitmap );
		wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
		assert( gc );

		Painter *painter = PainterFactory().createGcPainter( gc );
		assert( painter );
		paintItems( painter, MRect( 0, 0, size.x, size.y ));
	}

	/*****************************************************
	**
	**   UranianDisk   ---   paintItems
	**
	******************************************************/
	void paintItems( Painter *painter, const MRect& refreshRect )
	{
		CgUranianDisk *disk = getGeom();
		assert( disk );

		assert( chart );
		UranianSection *usection = chart->getUranianSection();
		const int division = usection ? usection->chartDivision : 1;
		assert( division );
#ifdef DEBUG_URANIANDISK
		debuglog << "UranianDisk::paintItems division " << division << " radius " << disk->radius;
#endif

		setInheritedStyle( painter );
		paintTNDisc( painter, disk, division, refreshRect );
		ShowChartCenterLabel( painter, disk, division );
	}

	/*****************************************************
	**
	**   UranianDisk   ---   paintTNDisc
	**
	******************************************************/
	void paintTNDisc( Painter *painter, CgUranianDisk *disk, const int &division, const MRect &refreshRect )
	{
		assert( division >= 1 );
		assert( disk );
#ifdef DEBUG_URANIANDISK
		debuglog << "UranianDisk::paintTNDisc radius " << disk->radius << " division " << division
			<< " paintDegreeScale " << std::boolalpha << disk->paintDegreeNumbers;
#endif

		paintDisk( painter, *disk );

		chart->setGraphicFont( painter, disk->labelSize );
		paintDegreeScale( painter, disk->radius, disk->degreeMarker, division, disk->paintDegreeNumbers );

		painter->setBrush( MBrush( painter->getPenColor()));

		std::list<std::pair<double, IconSize>> diamondCfg = {{ 0, IconSize::Large }};
		std::list<double> spokesCfg;

		switch( division )
		{
			case 1:
				diamondCfg =
				{
					{ 0, IconSize::Large },
					{ 45, IconSize::Small },
					{ 90, IconSize::Medium },
					{ 135, IconSize::Small },
					{ 180, IconSize::Medium },
					{ 235, IconSize::Small },
					{ 270, IconSize::Medium },
					{ 315, IconSize::Small },
				};
				if ( disk->showSigns )
				{
					assert( signs );
					signs->paint( painter, refreshRect );
				}
				if ( disk->showHouses )
				{
					assert( houses );
					houses->paint( painter, refreshRect );
				}
			break;
			case 2:
				diamondCfg =
				{
					{ 0, IconSize::Large },
					{ 45, IconSize::Small },
					{ 90, IconSize::Medium },
					{ 135, IconSize::Small },
				};
				if ( disk->showSigns ) paintSigns( painter, disk, division );
				spokesCfg = { 0, 90, 180, 270 };
			break;

			case 3:
				if ( disk->showSigns ) paintSigns( painter, disk, division );
				spokesCfg = { 0, 120, 240 };
			break;

			case 4:
				diamondCfg =
				{
					{ 0, IconSize::Large },
					{ 22.5, IconSize::Small },
					{ 45, IconSize::Medium },
					{ 67.5, IconSize::Small },
				};
				if ( disk->showSigns ) paintSigns( painter, disk, division );
				spokesCfg = { 0, 120, 240 };
			break;

			case 6:
				if ( disk->showSigns ) paintSigns( painter, disk, division );
				spokesCfg = { 0, 90, 180, 270 };
			break;

			case 8:
				diamondCfg =
				{
					{ 0, IconSize::Large },
					{ 11.25, IconSize::Small },
					{ 22.5, IconSize::Medium },
					{ 33.75, IconSize::Small },
				};
				spokesCfg = { 0, 180 };
			break;

			default:
				diamondCfg =
				{
					{ 0, IconSize::Large },
				};
			break;
		}
		paintDiamonds( painter, disk, division, diamondCfg );
		if ( spokesCfg.size() > 0 )
		{
			paintSpokes( painter, disk, spokesCfg );
		}
		setInheritedStyle( painter );
	}

	/*****************************************************
	**
	**   UranianDisk   ---   paintSpokes
	**
	******************************************************/
	void paintSpokes( Painter *painter, CgUranianDisk *udisk, const std::list<double> &spokesCfg )
	{
		assert( udisk );
		const ChartShape shape = udisk->geometryType;
		const double aries = chart->getAries();

		paintDisk( painter, udisk->innerRadius, shape );
		for( const auto &a : spokesCfg )
		{
			paintAnnulusConnector( painter, a + aries, udisk->radius, udisk->innerRadius, shape );
		}
	}

	/*****************************************************
	**
	**   UranianDisk   ---   paintSigns
	**
	******************************************************/
	void paintSigns( Painter *painter, CgUranianDisk  *disk, const int &division )
	{
#ifdef DEBUG_URANIANDISK
		debuglog << "UranianDisk::paintSigns division " << division;
#endif
		// attention: signs for division 1 are painted by class WesternSigns
		assert( division > 1 && division <= 12 );
		assert( ! ( 12 % division ));
		const ChartShape shape = disk->geometryType;

		SymbolProvider sp;
		Lang lang;
		ChartConfig *chartCfg = chart->getChartConfig();

		painter->setTransparentBrush();
		paintDisk( painter, disk->signInnerRadius, shape );
		paintDisk( painter, disk->signOuterRadius, shape );

		// TODO hängt ab von Länge des Strings, Radius und Schriftgröße
		constexpr double shiftAngle = 15;

		const PlanetPropertyConfig &pCfg = astroconfig->getViewpointConfig( chart->getViewpoint() ).planetProps;

		const double labelSize = 2 * disk->labelSize;
		if ( disk->useSymbols )
			chart->setSymbolFont( painter, labelSize );
		else
			chart->setGraphicFont( painter, labelSize );

		const int field_count = 12 / division;  // eg division = 4 -> field_count = 3
		const int angle_step = 360 / field_count; // eg division= 4 -> angle_step = 90

		const WidgetSkin &skin = chart->getSkin();
		const ChartFieldPattern pattern = skin.signPattern;

		double angle = 0;
		const double aries = chart->getAries();
		for( int i = 0; i < field_count; i++ )
		{
			angle = aries +  ( i + .5 ) * angle_step; //center of field
			angle -= ((double)( division - 1 ) / 2.0 ) * shiftAngle;

			for( int j = 0; j < division; j++ )
			{
				const Sign sign = (Sign)( j * field_count + i );
				assert_valid_enum<Sign>( sign );

				if ( chartCfg->showSignColors )
				{
					painter->setTextColor( gdiconfig->chartColors.getSignFgColor( sign ));
				}

				const MComplex p = diskPointToXy( angle, disk->signRLabels, disk->geometryType );
				if ( disk->useSymbols )
				{
					painter->drawRotatedText( sp.getSignCode( sign, pCfg.capricornSymbol ), p.real(), p.imag(), angle - 90 );
				}
				else
				{
					painter->drawRotatedText( lang.getSignName( sign, chart->getViewpoint() ), p.real(), p.imag(),
						disk->rotateLabels ? angle - 90 : 0 );
				}
				angle += shiftAngle;
			}
		}
	}

	/*****************************************************
	**
	**   UranianDisk   ---   paintDiamonds
	**
	******************************************************/
	void paintDiamonds( Painter *painter, CgUranianDisk *disk, const int &division,
		const std::list<std::pair<double, IconSize>> &cfg )
	{
		painter->setBrush( MBrush( painter->getPenColor()));

		const double aries = chart->getAries();
		for( const auto &a: cfg )
		{
			paintDiamond( painter, disk, aries + division * a.first );
		}
		painter->setTransparentBrush();
	}

	/*****************************************************
	**
	**   UranianDisk   ---   paintDiamond
	**
	******************************************************/
	void paintDiamond( Painter *painter, CgUranianDisk *disk, const double &l )
	{
		const ChartShape shape = disk->geometryType;
		std::vector<MComplex> points;
		points.push_back( diskPointToXy( l, disk->radius, shape ));
		points.push_back( diskPointToXy( l + 1, disk->radius - disk->degreeMarker.wDegree5, shape ));
		points.push_back( diskPointToXy( l, disk->radius - disk->degreeMarker.wDegree10, shape ));
		points.push_back( diskPointToXy( l - 1, disk->radius - disk->degreeMarker.wDegree5, shape ));
		points.push_back( diskPointToXy( l, disk->radius, shape ));
		painter->drawPolygon( points );
	}

	/*****************************************************
	**
	**   UranianDisk   ---   ShowChartCenterLabel
	**
	******************************************************/
	void ShowChartCenterLabel( Painter *painter, CgUranianDisk *disk, const int &division )
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
#ifdef DEBUG_URANIANDISK
			debuglog << "UranianDisk::ShowChartCenterLabel label size " << disk->degreeLabelSize;
#endif
			chart->setGraphicFont( painter, disk->degreeLabelSize );
			DocItemText centerItem = DocItemText( s, chart->getRect(), Align::Center );
			DocHelper helper( painter );
			helper.formatAndFitTextItem( centerItem );
			helper.drawTextItem( centerItem );
		}
	}

	/*****************************************************
	**
	**   UranianDisk   ---   getChartCenterDegreeString
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

	CgLabelledAnnulus *cghouses = nullptr;

	CgLabelledAnnulus *cgsigns = nullptr;

	ChartElementBase_ptr houses;

	ChartElementBase_ptr signs;
	
	wxBitmap bitmap;
};

/*****************************************************
**
**   Grid element factory
**
******************************************************/
ChartElementBase *ChartElementFactory::createUranianDisk( CgElement *cge ) const
{
	CgUranianDisk *udisk = dynamic_cast<CgUranianDisk*>( cge );
	assert( udisk );
	return new UranianDisk( chart, udisk );
}

