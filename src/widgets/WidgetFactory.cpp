/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/WidgetFactory.cpp
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

#include "WidgetFactory.h"

#include "enumserializer.h"
#include "gdiconfig.h"
#include "GdiWidgetWrapper.h"
#include "logger.h"
#include "ToolPanel.h"
#include "GdiWidget.h"

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   WidgetFactory   ---   createWidget
**
******************************************************/
BasicWidget *WidgetFactory::createWidget( wxWindow* parent, const WidgetCategory &category, const Varga varga )
{
	debuglog << "WidgetFactory::createWidget category " << category << " varga " << varga;
	BasicWidget *widget = nullptr;
	switch( category )
	{
		using enum WidgetCategory;

		case UniversalChart:
		case WesternChart:
		case VargaChart:
		case SarvatobhadraChart:
			widget = createSingleChart( parent, category, varga );
		break;

		case UniversalChartWidget:
			widget = createSingleChart( parent, WidgetCategory::UniversalChart, Varga::Rasi );
		break;
		case WesternChartWidget:
			widget = createSingleChart( parent, WidgetCategory::WesternChart, Varga::Rasi );
		break;
		case VargaChartWidget:
			widget = createSingleChart( parent, WidgetCategory::VargaChart, varga );
		break;
		case SbcWidget:
			widget = createSingleChart( parent, WidgetCategory::SarvatobhadraChart, Varga::Rasi );
		break;

		case TextBaseData:
		case TextArabicParts:
		case TextAshtakavarga:
		case TextPlanetBalance:
		case TextVedicHouses:
		case TextUranianHouses:
		case TextDasaShort:
		case TextDasaCompact:
		case TextDasaComprehensive:
		case TextJaimini:
		case TextKp:
		case TextNakshatra:
		case TextPlanetListCombined:
		case TextPlanetListUniversal:
		case TextPlanetListVedic:
		case TextPlanetListWestern:
		case TextPlanetListUranian:
		case TextPlanetListAstronomical:
		case TextShadbala:
		case TextVarga:
		case TextVedicAspectarium:
		case TextWesternAspectarium:
		case TextWesternAspectList:
			widget = createGenericTextWidget( parent, category );
		break;
		case TextAshtakoota:
			widget = createAshtakootaWidget( parent );
		break;
		case ComprehensiveTextWidget:
			widget = createComprehensiveTextWidget( parent );
		break;

		case RasiNavamsaGrid:
			widget = createRasiNavamsaGrid( parent );
		break;
		case Varga1Grid:
		case Varga2Grid:
		case Varga3Grid:
			widget = createVargaGrid( parent, category );
		break;
		case VimsopakabalaDiagram:
			widget = createVimsopakabalaWidget( parent );
		break;
		case DasaTree:
		case DasaTreeComposite:
			widget = createDasaTreeWidget( parent, category );
		break;
		case DasaChartWidget:
			widget = createDasaChartWidget( parent );
		break;
		case UranianChartWidget:
			widget = createUranianChartWidget( parent );
		break;
		case MatchesWidget:
			widget = createMatchesWidget( parent );
		break;
		case SpWidget:
			widget = createSpWidget( parent );
		break;
		case EventFinderWidget:
			widget = createEventFinderWidget( parent );
		break;
		case ShadbalaWidget:
			widget = createShadbalaWidget( parent );
		break;
		case AvWidget:
			widget = createAshtakavargaWidget( parent );
		break;
		case YogaWidget:
			widget = createYogaWidget( parent );
		break;
		case PpWidget:
			widget = createPpWidget( parent );
		break;
		case PartnerWidget:
			widget = createPartnerWidget( parent );
		break;

		case UniversalRadixWidget:
			widget = createNotebookWidget( parent, UniversalRadixWidget,
				gdiconfig->multipleView.extractItems( DataContext::Radix ),
				ToolbarWidgetId::None );
		break;
		case VargaGridWidget:
			widget = createNotebookWidget( parent, VargaGridWidget,
				{ WidgetCategory::Varga1Grid,
					WidgetCategory::Varga2Grid,
					WidgetCategory::Varga3Grid,
					WidgetCategory::VimsopakabalaDiagram,
					WidgetCategory::TextVarga
				},
				ToolbarWidgetId::None );
		break;
		case DirectionWidget:
			widget = createDirectionWidget( parent );
		break;
		case MultiDirectionWidget:
			widget = createMultiDirectionWidget( parent );
		break;
		case SolarWidget:
			widget = createSolarWidget( parent );
		break;

		case PlanetListChartPair:
		case CompositePair:
		case SynastryPair:
			widget = createCombiWidget( parent, category );
		break;
		case VedicTriple:
			widget = createMixedWidgetGrid( parent, category, 
			{
				WidgetCategory::TextPlanetListVedic,
				WidgetCategory::RasiNavamsaGrid,
				WidgetCategory::DasaTree
			});
		break;

		case YogaEditorWidget:
			widget = createYogaEditorWidget( parent );
		break;
		case EclipseWidget:
			widget = createEclipseWidget( parent );
		break;
		case HoraWidget:
			widget = createHoraWidget( parent );
		break;
		case TextFileAndDirectoryReport:
			widget = createFileAndDirectoryReportWidget( parent );
		break;
		case TextLicense:
			widget = createLicenseWidget( parent );
		break;
		case EphemWidget:
			widget = createEphemWidget( parent );
		break;
		case EmptyWidget:
			widget = createEmptyWidget( parent, "error" );
		break;

		default:
		{
			wxString s = "WidgetFactory::createWidget cannot create widget for category " + enum_to_string( category );
			errorlog << s;
			widget = createEmptyWidget( parent, s, LogLevel::Error );
		}
		break;
	}

	assert( widget );
	return widget;
}

/*************************************************//**
*
* 
*
******************************************************/
class EmptyWidget : public GdiWidgetWrapper
{
public:

	EmptyWidget( wxWindow *parent, ContextProps_ptr props, const wxString message, const LogLevel level )
		: GdiWidgetWrapper( parent, props, WidgetCategory::EmptyWidget )
	{
		setWidget( new ErrorDiagram( props, message, level ));
	}
};

/*************************************************//**
*
* 
*
******************************************************/
BasicWidget *WidgetFactory::createEmptyWidget( wxWindow *parent, const wxString message, const LogLevel level )
{
	return new EmptyWidget( parent, props, message, level );
}

