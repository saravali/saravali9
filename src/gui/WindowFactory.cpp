/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/WindowFactory.cpp
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

#include "WindowFactory.h"

#include "ChildWindow.h"
#include "BasicWidget.h"
#include "ContextProperties.h"
#include "Commands.h"
#include "enumserializer.h"
#include "logger.h"
#include "MultipleView.h"
#include "WidgetFactory.h"

#define DEBUG_WINDOWFACTORY

/**************************************************************
***
**   WindowFactory   ---   createWindow
***
***************************************************************/
ChildWindow *WindowFactory::createWindow( wxWindow *parent, Horoscope_ptr horoscope, const CommandId &commandId )
{
	std::map<CommandId, std::pair<ViewId, WidgetCategory>> commandMapping =
	{
		{ CommandId::AppEclipse, { ViewId::Eclipse, WidgetCategory::EclipseWidget }},
		{ CommandId::AppEphemeris, { ViewId::Ephemeris, WidgetCategory::EphemWidget }},
		{ CommandId::AppHora, { ViewId::Hora, WidgetCategory::HoraWidget }},
		{ CommandId::AppPartner, { ViewId::Partner, WidgetCategory::PartnerWidget }},
		{ CommandId::AppFileAndDirectoryReport, { ViewId::FileAndDirectoryReport, WidgetCategory::TextFileAndDirectoryReport }},
		{ CommandId::AppLicense, { ViewId::License, WidgetCategory::TextLicense }},

		{ CommandId::AppNewSingle, { ViewId::PlanetList, WidgetCategory::TextPlanetListCombined }},
		
		// Universal
		{ CommandId::NewText, { ViewId::Text, WidgetCategory::ComprehensiveTextWidget }},
		{ CommandId::NewUniversalRadix, { ViewId::UniversalRadix, WidgetCategory::UniversalRadixWidget }},
		{ CommandId::NewUniversalChart, { ViewId::UniversalChart, WidgetCategory::UniversalChartWidget }},
		{ CommandId::NewDirection, { ViewId::Direction, WidgetCategory::DirectionWidget }},
		{ CommandId::NewMultiDirection, { ViewId::MultiDirection, WidgetCategory::MultiDirectionWidget }},
		{ CommandId::NewSolar, { ViewId::Solar, WidgetCategory::SolarWidget }},

		// Vedic
		{ CommandId::NewYoga, { ViewId::Yoga, WidgetCategory::YogaWidget }},
		{ CommandId::NewShadbala, { ViewId::Shadbala, WidgetCategory::ShadbalaWidget }},
		{ CommandId::NewDasaTree, { ViewId::DasaTree, WidgetCategory::DasaTree }},
		{ CommandId::NewDasaTreeComposite, { ViewId::DasaTreeComposite, WidgetCategory::DasaTreeComposite }},
		{ CommandId::NewAshtakavarga, { ViewId::Ashtakavarga, WidgetCategory::AvWidget }},
		{ CommandId::NewDasaChart, { ViewId::DasaChart, WidgetCategory::DasaChartWidget }},
		{ CommandId::NewSbc, { ViewId::SarvatobhadraChart, WidgetCategory::SbcWidget }},
		{ CommandId::NewVarga, { ViewId::Varga, WidgetCategory::VargaGridWidget }},
		{ CommandId::AppYogaEditor, { ViewId::YogaEditor, WidgetCategory::YogaEditorWidget }},
		// CommandId::NewVargaChart is  handled below

		// Western
		{ CommandId::NewWesternChart, { ViewId::WesternChart, WidgetCategory::WesternChartWidget }},

		// Uranian
		{ CommandId::NewUranianChart, { ViewId::UranianChart, WidgetCategory::UranianChartWidget }},
		{ CommandId::NewSensitivePoints, { ViewId::SensitivePoints, WidgetCategory::SpWidget }},
		{ CommandId::NewPp, { ViewId::PlanetaryPictures, WidgetCategory::PpWidget }},
		{ CommandId::NewUranian, { ViewId::Uranian, WidgetCategory::MatchesWidget }},
		{ CommandId::NewEventFinder, { ViewId::EventFinder, WidgetCategory::EventFinderWidget }},
	};

	std::pair<ViewId, WidgetCategory> m( { ViewId::Empty, WidgetCategory::EmptyWidget });

	auto a = commandMapping.find( commandId );
	if ( a != commandMapping.end() )
	{
		m = a->second;
	}

	Varga varga = Varga::Rasi;
	if ( commandId >= CommandId::FirstNewVargaChart && commandId <= CommandId::LastNewVargaChart )
	{
		varga = (Varga)( (int)commandId - (int)CommandId::FirstNewVargaChart );
		m = { ViewId::VedicChart, WidgetCategory::VargaChartWidget };
	}
	assert_valid_enum<Varga>( varga );

	// find out Bitmap ID
	BitmapId bmpId = BitmapId::None;
	{
		MCommand *command = CommandHandler::get()->getCommand( commandId );
		assert( command );
		bmpId = command->bmpId;
	}

#ifdef DEBUG_WINDOWFACTORY
	debuglog << "WindowFactory::createWindow CommandId " << commandId << " ViewId " << m.first
		<< " Bitmap Id " << (int)bmpId
		<< " WidgetCategory " << m.second;
#endif

	if ( commandId == CommandId::AppYogaEditor )
	{
		return createYogaEditorWindow( parent, m.first, bmpId );
	}

	// else
	ChildWindow *view = new ChildWindow( parent, m.first, bmpId, horoscope );
	assert( view );

	view->setMainWindow( commandId == CommandId::AppNewSingle );

	WidgetFactory factory( view->getProps() );
	BasicWidget *widget = factory.createWidget( view, m.second, varga );
	assert( widget );

	view->setWidget( widget );
	return view;
}

/**************************************************************
***
**   WindowFactory   ---   createMultipleMainWindow
***
***************************************************************/
ChildWindow *WindowFactory::createMultipleMainWindow( wxWindow *parent, Horoscope_ptr horoscope, int id )
{
	ChildWindow *view = nullptr;
	MCommand *command = CommandHandler::get()->getCommand( CommandId::AppNewMultiple );
	assert( command );

	const CompositeViewDefinition &def = MultipleViewHandler::get()->getBuiltinDef( id );
	const WidgetCategory category = WidgetCategory::UniversalRadixWidget;
	view = new ChildWindow( parent, ViewId::Multiple, command->bmpId, horoscope );
	WidgetFactory factory( view->getProps() );
	view->setWidget( factory.createNotebookWidget( view, category, def.elements, ToolbarWidgetId::None ));

	view->getProps()->setPreferredViewpoint( def.viewpoint );
	assert( view );

	view->setMainWindow( true );
	return view;
}


