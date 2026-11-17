/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ConfletFactory.cpp
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

#include "ConfletFactory.h"

#include "AnimationConflet.h"
#include "AshtakavargaConflet.h"
#include "AtlasConflet.h"
#include "AyanamsaConflet.h"
#include "BardiagramConflet.h"
#include "BasedataConflet.h"
#include "BasicColorConflet.h"
#include "CalculationConflet.h"
#include "ChartColorConflet.h"
#include "ChartConflet.h"
#include "DasaConflet.h"
#include "DirectionConflet.h"
#include "enumserializer.h"
#include "EphemerisConflet.h"
#include "FontConflet.h"
#include "gdiconfig.h"
#include "GeneralConfigConflet.h"
#include "JaiminiConflet.h"
#include "KalavelaConflet.h"
#include "MiscellaneousConflet.h"
#include "MultipleViewConflet.h"
#include "PlanetConflet.h"
#include "PlanetPropertyConflet.h"
#include "PdfConflet.h"
#include "PrintoutConflet.h"
#include "PageConflet.h"
#include "StandardLocationConflet.h"
#include "TabbedViewConflet.h"
#include "TableStyleConflet.h"
#include "ToolbarConflet.h"
#include "VargaConflet.h"
#include "UranianAspectConflet.h"
#include "UranianConflet.h"
#include "WesternAspectConflet.h"

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   ConfletFactory   ---   Constructor
**
******************************************************/
ConfletFactory::ConfletFactory()
{
	init();
}

/*****************************************************
**
**   ConfletFactory   ---   init
**
******************************************************/
void ConfletFactory::init()
{
	titles =
	{
		{ ConfletId::General, tr( "General" ) },
		{ ConfletId::StandardLocation,tr( "Standard Location" ), },
		{ ConfletId::Toolbar, tr( "Toolbar" ), },
		{ ConfletId::Atlas, tr( "Atlas" ), },
		{ ConfletId::Animation, tr( "Animation" ), },

		// Printout
		{ ConfletId::Printout, tr( "Printout" ), },
		{ ConfletId::Page, tr( "Page Setup" ), },
		{ ConfletId::Pdf, tr( "Pdf Options" ), },

		// Section Calculation
		{ ConfletId::Ephemeris, tr( "Calculation" ), },
		{ ConfletId::CustomAyanamsa, tr( "Custom Ayanamsa" ), },
		{ ConfletId::Directions, tr( "Directions" ), },
		{ ConfletId::Varga, tr( "Varga" ), },
		{ ConfletId::Dasa, tr( "Dasa" ), },
		{ ConfletId::Ashtakavarga, tr( "Ashtakavarga" ), },
		{ ConfletId::Jaimini, tr( "Jaimini" ), },
		{ ConfletId::Kalavela, tr( "Kalavelas" ), },
		{ ConfletId::UranianConfig, tr( "Uranian" ), },

		// Section View
		{ ConfletId::MultipleView, tr( "Multiple View" ), },
		{ ConfletId::TabbedViewRadix, tr( "Universal Radix" ), },
		{ ConfletId::TabbedViewDirection, tr( "Directions" ), },
		{ ConfletId::TabbedViewMultiDirection, tr( "Multi Direction" ), },
		{ ConfletId::TabbedViewSolar, tr( "Solar" ), },
		{ ConfletId::TabbedViewPartner, tr( "Partner" ), },

		// Section widget
		{ ConfletId::BasicColors, tr( "Colors" ), },
		{ ConfletId::ChartColors, tr( "Chart Colors" ), },
		{ ConfletId::Bardiagram, tr( "Bar Diagrams" ), },
		{ ConfletId::Fonts, tr( "Fonts" ), },
		{ ConfletId::TableStyle, tr( "Table Style" ), },

		// Section Vedic
		{ ConfletId::VedicPlanets, tr( "Vedic Planets" ), },
		{ ConfletId::VedicCalculation, tr( "Calculation" ), },
		{ ConfletId::VedicPlanetProperties, tr( "Planet Properties" ), },
		{ ConfletId::VedicBasedata, tr( "Base Data" ), },
		{ ConfletId::VargaChart, tr( "Varga Charts" ), },
		{ ConfletId::Sbc, tr( "Sarvatobhadra Chart" ), },

		// Section Western
		{ ConfletId::WesternPlanets, tr( "Western Planets" ), },
		{ ConfletId::WesternCalculation, tr( "Calculation" ), },
		{ ConfletId::WesternPlanetProperties, tr( "Planet Properties" ), },
		{ ConfletId::WesternBasedata, tr( "Base Data" ), },
		{ ConfletId::WesternChart, tr( "Chart" ), },
		{ ConfletId::WesternAspects, tr( "Aspects" ), },

		// Section Uranian
		{ ConfletId::UranianPlanets, tr( "Uranian Planets" ), },
		{ ConfletId::UranianCalculation, tr( "Calculation" ), },
		{ ConfletId::UranianPlanetProperties, tr( "Planet Properties" ), },
		{ ConfletId::UranianBasedata, tr( "Base Data" ), },
		{ ConfletId::UranianChart, tr( "Chart" ), },
		{ ConfletId::UranianAspects, tr( "Aspects" ), },

		// Section Miscellaneous
		{ ConfletId::Miscellaneous, tr( "Miscellaneous" ), },
		{ ConfletId::AstronomicalPlanets, tr( "Astronomical Planets" ), },
		{ ConfletId::AstronomicalCalculation, tr( "Astronomical Calculation" ), },
		{ ConfletId::AstronomicalPlanetProperties, tr( "Astronomical Planet Properties" ), },
		{ ConfletId::AstronomicalBasedata, tr( "Astronomical Base Data" ), },
		{ ConfletId::UniversalChart, tr( "Universal Chart" ), },
	};
}

/*****************************************************
**
**   ConfletFactory   ---   getConfletTitle
**
******************************************************/
wxString ConfletFactory::getConfletTitle( const ConfletId &id ) const
{
	if ( titles.find( id ) == titles.end())
	{
		return "n.a";
	}
	return titles.at( id );
}

/*****************************************************
**
**   ConfletFactory   ---   createConflet
**
******************************************************/
Conflet *ConfletFactory::createConflet( wxWindow *parent, const ConfletIdTreenode &node )
{
	debuglog << "ConfletFactory::createConflet " << (int)node.id;
	switch( node.id )
	{
		using enum ConfletId;

		case General:
			return new GeneralConfigConflet( parent );
		break;
		case StandardLocation:
			return new StandardLocationConflet( parent );
		break;
		case Toolbar:
			return new ToolbarConflet( parent );
		break;
		case Atlas:
			return new AtlasConflet( parent );
		break;
		case Animation:
			return new AnimationConflet( parent );
		break;
		case Printout:
			return new PrintoutConflet( parent );
		break;
		case Page:
			return new PageConflet( parent );
		break;
		case Pdf:
			return new PdfConflet( parent );
		break;

		case Ephemeris:
			return new EphemerisConflet( parent );
		break;
		case CustomAyanamsa:
			return new AyanamsaConflet( parent );
		break;
		case Directions:
			return new DirectionConflet( parent );
		break;
		case Varga:
			return new VargaConflet( parent );
		break;
		case Dasa:
			return new DasaConflet( parent );
		break;
		case Ashtakavarga:
			return  new AshtakavargaConflet( parent );
		break;
		case Jaimini:
			return  new JaiminiConflet( parent );
		break;
		case Kalavela:
			return new KalavelaConflet( parent );
		break;
		case UranianConfig:
			return new UranianConflet( parent );
		break;

		case MultipleView:
			return new MultipleViewConflet( parent );
		break;
		case TabbedViewRadix:
			return new TabbedViewConflet( parent, DataContext::Radix, gdiconfig->multipleView.radixItems );
		break;
		case TabbedViewMultiDirection:
			return new TabbedViewConflet( parent, DataContext::MultiDirection, gdiconfig->multipleView.multiDirectionItems );
		break;
		case TabbedViewDirection:
			return new TabbedViewConflet( parent, DataContext::Direction, gdiconfig->multipleView.directionItems );
		break;
		case TabbedViewSolar:
			return new TabbedViewConflet( parent, DataContext::Solar, gdiconfig->multipleView.solarItems );
		break;
		case TabbedViewPartner:
			return new TabbedViewConflet( parent, DataContext::Partner, gdiconfig->multipleView.partnerItems );
		break;

		case BasicColors:
			return new BasicColorConflet( parent );
		break;
		case ChartColors:
			return new ChartColorConflet( parent );
		break;
		case Bardiagram:
			return new BardiagramConflet( parent );
		break;
		case Fonts:
			return new FontConflet( parent );
		break;
		case TableStyle:
			return new TableStyleConflet( parent );
		break;

		case WesternChart:
		case UranianChart:
		case VargaChart:
		case Sbc:
		case UniversalChart:
			return new ChartConflet( parent, node.id );
		break;

		case WesternAspects:
			return new WesternAspectConflet( parent );
		break;

		case VedicCalculation:
			return new CalculationConflet( parent, Viewpoint::Vedic );
		break;
		case VedicPlanets:
			return new PlanetConflet( parent, Viewpoint::Vedic );
		break;
		case VedicPlanetProperties:
			return new PlanetPropertyConflet( parent, Viewpoint::Vedic );
		break;
		case VedicBasedata:
			return new BasedataConflet( parent, Viewpoint::Vedic );
		break;

		case WesternCalculation:
			return new CalculationConflet( parent, Viewpoint::Western );
		break;
		case WesternPlanets:
			return new PlanetConflet( parent, Viewpoint::Western );
		break;
		case WesternPlanetProperties:
			return new PlanetPropertyConflet( parent, Viewpoint::Western );
		break;
		case WesternBasedata:
			return new BasedataConflet( parent, Viewpoint::Western );
		break;


		case UranianCalculation:
			return new CalculationConflet( parent, Viewpoint::Uranian );
		break;
		case UranianPlanets:
			return new PlanetConflet( parent, Viewpoint::Uranian );
		break;
		case UranianPlanetProperties:
			return new PlanetPropertyConflet( parent, Viewpoint::Uranian );
		break;
		case UranianBasedata:
			return new BasedataConflet( parent, Viewpoint::Uranian );
		break;
		case UranianAspects:
			return new UranianAspectConflet( parent );
		break;

		case Miscellaneous:
			return new MiscellaneousConflet( parent );
		break;
		case AstronomicalCalculation:
			return new CalculationConflet( parent, Viewpoint::Astronomical );
		break;
		case AstronomicalPlanets:
			return new PlanetConflet( parent, Viewpoint::Astronomical );
		break;
		case AstronomicalPlanetProperties:
			return new PlanetPropertyConflet( parent, Viewpoint::Astronomical );
		break;
		case AstronomicalBasedata:
			return new BasedataConflet( parent, Viewpoint::Astronomical );
		break;
		default:
			assert( false );
		break;
	}
	return nullptr;
}

/*****************************************************
**
**   ConfletFactory   ---   calculateTreenodes
**
******************************************************/
std::list<ConfletIdTreenode> ConfletFactory::calculateTreenodes( const ConfletSet &cs )
{
	debuglog <<  "ConfletFactory::calculateTreenodes confletSet " << cs;
	switch( cs )
	{
		case ConfletSet::Printout:
			return
			{
				ConfletId::Printout,
				ConfletId::Page,
				ConfletId::Pdf,
			};
		break;
		case ConfletSet::Calculation:
			return
			{
				ConfletId::Directions,
				ConfletId::Varga,
				ConfletId::Dasa,
				ConfletId::Kalavela,
				ConfletId::Ashtakavarga,
				ConfletId::Jaimini,
				ConfletId::Ephemeris,
				ConfletId::CustomAyanamsa,
				ConfletId::StandardLocation
			};
		break;
		case ConfletSet::Directions:
			return
			{
				ConfletId::Directions,
			};
		break;
		case ConfletSet::General:
		case ConfletSet::Gui:
		default:
			return
			{
				ConfletIdTreenode( ConfletId::General,
				{
					ConfletId::StandardLocation,
					ConfletId::Toolbar,
					ConfletId::Atlas,
					ConfletId::Animation,
				} ),
				ConfletIdTreenode( ConfletId::Ephemeris,
				{
					ConfletId::CustomAyanamsa,
					ConfletId::Directions,
					ConfletId::Varga,
					ConfletId::Dasa,
					ConfletId::Ashtakavarga,
					ConfletId::Jaimini,
					ConfletId::Kalavela,
					ConfletId::UranianConfig,
				} ),
				ConfletIdTreenode( ConfletId::Printout, tr( "Printout" ),
				{
					ConfletId::Page,
					ConfletId::Pdf,
				} ),

				ConfletIdTreenode( ConfletId::MultipleView,
				{
					ConfletId::TabbedViewRadix,
					ConfletId::TabbedViewDirection,
					ConfletId::TabbedViewMultiDirection,
					ConfletId::TabbedViewSolar,
					ConfletId::TabbedViewPartner,
				} ),

				ConfletIdTreenode( ConfletId::BasicColors, tr( "User Interface" ),
				{
					ConfletId::ChartColors,
					ConfletId::Bardiagram,
					ConfletId::Fonts,
					ConfletId::TableStyle,
				} ),
				ConfletIdTreenode( ConfletId::VedicPlanets, tr( "Vedic Tradition" ),
				{
					ConfletId::VedicCalculation,
					ConfletId::VedicPlanetProperties,
					ConfletId::VedicBasedata,
					ConfletId::VargaChart,
					ConfletId::Sbc,
				} ),
				ConfletIdTreenode( ConfletId::WesternPlanets, tr( "Western Tradition" ),
				{
					ConfletId::WesternCalculation,
					ConfletId::WesternPlanetProperties,
					ConfletId::WesternBasedata,
					ConfletId::WesternChart,
					ConfletId::WesternAspects,
				} ),
				ConfletIdTreenode( ConfletId::UranianPlanets, tr( "Uranian Tradition" ),
				{
					ConfletId::UranianCalculation,
					ConfletId::UranianPlanetProperties,
					ConfletId::UranianBasedata,
					ConfletId::UranianChart,
					ConfletId::UranianAspects,
				} ),
				ConfletIdTreenode( ConfletId::Miscellaneous,
				{
					ConfletId::AstronomicalCalculation,
					ConfletId::AstronomicalPlanets,
					ConfletId::AstronomicalPlanetProperties,
					ConfletId::AstronomicalBasedata,
					ConfletId::UniversalChart,
				} ),
		};
		break;
	}
}

