/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/ToolPanel.cpp
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

#include "ToolPanel.h"

#include "Commands.h"
#include "CommonControls.h"
#include "Dasa.h"
#include "DasaTool.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "ImageProvider.h"
#include "Lang.h"
#include "SpinCtrl.h"
#include "UranianControls.h"
#include "Varga.h"

#include <wx/combobox.h>
#include <wx/menu.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   ToolbarFactory   ---    Constructor
**
******************************************************/
ToolbarFactory::ToolbarFactory()
{
	init();
}

/*****************************************************
**
**   ToolbarFactory   ---    createToolbar
**
******************************************************/
wxToolBar *ToolbarFactory::createToolbar( wxWindow *parent, const ToolbarWidgetId &tbId )
{
	const static std::map<ToolbarWidgetId, std::list<ToolbarCmd>> tbconfig =
	{
		{ ToolbarWidgetId::AshtakavargaWidget,
			{
				ToolbarCmd::ChoiceVarga
			}
		},
		{ ToolbarWidgetId::ComprehensiveTextWidget,
			{
				ToolbarCmd::ChoiceTextMode,
				ToolbarCmd::ChoiceVarga,
				ToolbarCmd::ChoiceDasa
			}
		},
		{ ToolbarWidgetId::DasaChartWidget,
			{
				ToolbarCmd::ButtonGotoYear,
				ToolbarCmd::ButtonNow,
				ToolbarCmd::ButtonSelectDasa,
				ToolbarCmd::None,
				ToolbarCmd::ButtonZoomOriginal,
				ToolbarCmd::ButtonZoomIn,
				ToolbarCmd::ButtonZoomOut,
				ToolbarCmd::None,
				ToolbarCmd::ChoiceAntardasa,
				ToolbarCmd::ToolbarLabel
			}
		},
		{ ToolbarWidgetId::EclipseWidget,
			{
				ToolbarCmd::SpinYear,
				ToolbarCmd::SpinYearCount,
				ToolbarCmd::ChoiceEclipseType,
				ToolbarCmd::ChoiceTimezone
			}
		},
		{ ToolbarWidgetId::EphemWidget,
			{
				ToolbarCmd::SpinYear,
				ToolbarCmd::SpinMonth,
				ToolbarCmd::ButtonNow,
				ToolbarCmd::ChoiceTimezone,
				ToolbarCmd::None,
				ToolbarCmd::ChoiceEphemCoordinateType,
				ToolbarCmd::ChoiceDivision,
				ToolbarCmd::ChoiceDasa
			}
		},
		{ ToolbarWidgetId::HoraWidget,
			{
				ToolbarCmd::InputDate,
				ToolbarCmd::ButtonNow,
				ToolbarCmd::ChoiceTimezone,
			}
		},
		{ ToolbarWidgetId::MultiDirectionWidget,
			{
				ToolbarCmd::DateSelectionCombiCtrl,
				ToolbarCmd::None,
				ToolbarCmd::ButtonConfigure,
			}
		},
		{ ToolbarWidgetId::PartnerWidget,
			{
				ToolbarCmd::ChoicePartner1,
				ToolbarCmd::ChoicePartner2
			}
		},
		{ ToolbarWidgetId::EventFinderWidget,
			{
				ToolbarCmd::InputDate,
				ToolbarCmd::ButtonNow,
				ToolbarCmd::None,
				ToolbarCmd::ChoicePeriodDuration
			}
		},
		{ ToolbarWidgetId::SolarWidget,
			{
				ToolbarCmd::SpinYear,
				ToolbarCmd::ButtonNow,
				ToolbarCmd::ButtonContextHoroscope
			}
		},
		{ ToolbarWidgetId::SpWidget,
			{
				ToolbarCmd::ChoiceSpTerm,
				ToolbarCmd::ChoiceDivision,
				ToolbarCmd::ChoiceSpDisplaymode,
				ToolbarCmd::ChoiceSpObject,
				ToolbarCmd::ButtonFilter,
			}
		},
		{ ToolbarWidgetId::DirectionWidget,
			{
				ToolbarCmd::ChoiceDirectionMode,
				ToolbarCmd::DateSelectionCombiCtrl,
				ToolbarCmd::ButtonContextHoroscope
			}
		},
		{ ToolbarWidgetId::YogaEditorWidget,
			{
				ToolbarCmd::ButtonFileNew,
				ToolbarCmd::ButtonFileOpen,
				ToolbarCmd::ButtonFileSave,
				ToolbarCmd::ButtonFileSaveAs,
			}
		},
		{ ToolbarWidgetId::YogaWidget,
			{
				ToolbarCmd::ChoiceVarga,
				ToolbarCmd::ChoiceYogaGroupFilter,
				ToolbarCmd::ChoiceYogaSourceFilter,
				ToolbarCmd::ChoiceYogaFirstHouse
			}
		},
	};
	wxToolBar *toolbar = new wxToolBar( parent, TOOLBAR_ID );
	//toolbar->SetToolBitmapSize( wxSize( 24, 24 ));

	//toolbar->SetAcceleratorTable( wxNullAcceleratorTable );
	toolbar->SetAcceleratorTable( CommandHandler::get()->getAcceleratorTable( AcceleratorTableId::Minimal ));


	const auto &cfg = tbconfig.find( tbId );
	assert( cfg != tbconfig.end());

	for( const auto &item : cfg->second )
	{
		debuglog << "ToolbarFactory::createToolbar item " << (int)item;
		switch( item )
		{
			using enum ToolbarCmd;

			case None:
				toolbar->AddSeparator();
			break;
			case ButtonNow:
			case ButtonContextHoroscope:
			case ButtonConfigure:
			case ButtonFileClose:
			case ButtonFileNew:
			case ButtonFileOpen:
			case ButtonFileSave:
			case ButtonFileSaveAs:
			case ButtonGotoYear:
			case ButtonLaunchViewer:
			case ButtonSelectDasa:
			case ButtonFilter:
			case ButtonZoomIn:
			case ButtonZoomOriginal:
			case ButtonZoomOut:
				addButton( toolbar, item );
			break;
			case ChoiceAntardasa:
			{
				const static wxString choices[] = {
					tr( "Mahadasa" ),
					tr( "Antardasa" ),
					tr( "Pratyantardasa" ),
					tr( "Sookshmantardasa" ),
					tr( "Pranadasa" )
				};
				wxChoice *choice = new wxChoice( toolbar, (int)item, wxDefaultPosition, wxDefaultSize, 5, choices, 0 );
				choice->SetSelection( 1 );
				choice->SetToolTip( tr( "Dasa Level (Shift+MouseWheel)" ));
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceDasa:
			{
				wxChoice *choice = new wxChoice( toolbar, (int)item );
				DasaTool *tool = DasaTool::get();
				for ( const DasaId id : Enum<DasaId>() ) { choice->Append( tool->getDasaName( id )); }
				choice->SetSelection( 0 );
				choice->SetToolTip( tr( "Dasa" ));
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceEclipseType:
			{
				const static wxString eclipse_types[] = { tr("All"), tr("Solar"), tr("Lunar") };
				wxChoice *choice = new wxChoice( toolbar, (int)item, wxDefaultPosition, wxDefaultSize, 3, eclipse_types, 0 );
				choice->SetToolTip( tr( "Eclipse Type" ));
				choice->SetSelection( 0 );
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceEphemCoordinateType:
			{
				const static wxString choices[] = { tr("Longitude"), tr( "Declination" ), tr("Declination/ Absolute") };
				wxChoice *choice = new wxChoice( toolbar, (int)item, wxDefaultPosition, wxDefaultSize, 3, choices, 0 );
				choice->SetToolTip( tr( "Coordinate Type" ));
				choice->SetSelection( 0 );
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceDivision:
			{
				DivisionChoice *choice = new DivisionChoice( toolbar, (int)item );
				choice->SetToolTip( tr( "Division" ));
				toolbar->AddControl( choice );
			}
			break;
			case ChoicePartner1:
			{
				wxChoice *choice = new wxChoice( toolbar, (int)item );
				choice->SetToolTip( tr( "Partner 1" ));
				toolbar->AddControl( choice );
			}
			break;
			case ChoicePartner2:
			{
				wxChoice *choice = new wxChoice( toolbar, (int)item );
				choice->SetToolTip( tr( "Partner 2" ));
				toolbar->AddControl( choice );
			}
			break;

			case ChoicePeriodDuration:
			{
				PeriodDurationChoice *choice = new PeriodDurationChoice( toolbar, (int)item );
				choice->SetToolTip( tr( "Duration" ));
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceSpDisplaymode:
			{
				//SpDisplayModeChoice *choice = new SpDisplayModeChoice( toolbar, (int)item );
				wxChoice *choice = new wxChoice( toolbar, (int)item );
				choice->SetToolTip( tr( "Tuple Mode" ));
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceSpObject:
			{
				SpObjectSelectionChoice *choice = new SpObjectSelectionChoice( toolbar, (int)item );
				choice->SetToolTip( tr( "Select Object" ));
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceSpTerm:
			{
				SpTermChoice *choice = new SpTermChoice( toolbar, (int)item );
				choice->SetToolTip( tr( "Formula" ));
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceTextMode:
			{
				TextViewModeChoice *choice = new TextViewModeChoice( toolbar, (int)item );
				choice->SetToolTip( tr( "Text Mode" ));
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceTimezone:
			{
				TimezoneChoice *choice = new TimezoneChoice( toolbar, (int)item );
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceDirectionMode:
			{
				wxChoice *choice = new DirectionModeChoice( toolbar, (int)item );
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceVarga:
			{
				Lang lang;
				wxChoice *choice = new wxChoice( toolbar, (int)item );
				for ( Varga i : Enum<Varga>() )
				{
					wxString s = lang.getVargaName(i);
					if ( i != Varga::Bhava ) { s += std::format( " (D-{})", VargaExpert::getVargaDivision( i )); }
					choice->Append( s );
				}
				choice->SetSelection( 0 );
				choice->SetToolTip( tr( "Varga" ));
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceYogaGroupFilter:
			{
				wxChoice *choice = new wxChoice( toolbar, (int)item );
				choice->SetToolTip( tr( "Filter for Yoga Groups" ));
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceYogaSourceFilter:
			{
				wxChoice *choice = new wxChoice( toolbar, (int)item );
				choice->SetToolTip( tr( "Filter for Yoga Sources" ));
				toolbar->AddControl( choice );
			}
			break;
			case ChoiceYogaFirstHouse:
			{
				const static wxString choices[] = { tr("Ascendant"), tr("Moon"), tr("Arudha Lagna"),
					tr( "Aries" ), tr( "Taurus" ), tr( "Gemini" ), tr( "Cancer" ), tr( "Leo" ), tr( "Virgo" ), tr( "Libra" ),
					tr( "Scorpio" ), tr( "Sagittarius" ), tr( "Capricorn" ), tr( "Aquarius" ), tr( "Pisces" ) };
				wxChoice *choice = new wxChoice( toolbar, (int)item, wxDefaultPosition, wxDefaultSize, 15, choices );
				choice->SetToolTip( tr( "Ascendant" ));
				toolbar->AddControl( choice );
			}
			break;
			case InputDate:
			{
				DateSpin *dateCtrl = new DateSpin( toolbar, (int)item, nullptr );
				dateCtrl->SetToolTip( tr( "Date" ));
				toolbar->AddControl( dateCtrl );
			}
			break;
			case DateSelectionCombiCtrl:
			{
				DateInputCombiCtrl *dateCtrl = new DateInputCombiCtrl( toolbar, (int)item );
				dateCtrl->SetToolTip( tr( "Date" ));
				toolbar->AddControl( dateCtrl );
			}
			break;
			case SpinMonth:
			{
				wxSpinCtrl *spin = new wxSpinCtrl( toolbar, (int)item, "", wxDefaultPosition, wxDefaultSize,
					wxSP_ARROW_KEYS | wxSP_WRAP | wxTE_PROCESS_ENTER, 1, 12 );
				spin->SetToolTip( tr( "Month" ));
				toolbar->AddControl( spin );
			}
			break;
			case SpinYear:
			{
				wxSpinCtrl *spin = new wxSpinCtrl( toolbar, (int)item, "", wxDefaultPosition, wxDefaultSize,
					wxSP_ARROW_KEYS | wxTAB_TRAVERSAL, -3000, 5000 );
				spin->SetToolTip( tr( "Year" ));
				toolbar->AddControl( spin );
			}
			break;
			case SpinYearCount:
			{
				wxSpinCtrl *spin = new wxSpinCtrl( toolbar, (int)item, "", wxDefaultPosition, wxDefaultSize,
					wxSP_ARROW_KEYS | wxTAB_TRAVERSAL, 1, 10 );
				spin->SetToolTip( tr( "Number of Years" ));
				toolbar->AddControl( spin );
			}
			break;
			case ToolbarLabel:
			{
				constexpr int bmpsize = 24;
				const int textHeight = bmpsize + toolbar->GetMargins().x;
				ToolbarLabelCtrl *label = new ToolbarLabelCtrl( toolbar, (int)item, wxDefaultPosition, wxSize( 600, textHeight ));
				toolbar->AddControl( label );
			}
			break;
			default:
			break;
		}
	}
	toolbar->Realize();
	return toolbar;
}

/*****************************************************
**
**   ToolbarFactory   ---    addButton
**
******************************************************/
void ToolbarFactory::addButton( wxToolBar *toolbar, const ToolbarCmd &tbcmd )
{
	CommandId commandId = CommandId::None;
	switch( tbcmd )
	{
		using enum ToolbarCmd;

		case ButtonNow:
			commandId = CommandId::Now;
		break;
		case ButtonContextHoroscope:
			commandId = CommandId::NewContextHoroscope;
		break;
		case ButtonFileClose:
			commandId = CommandId::Close;
		break;
		case ButtonConfigure:
			commandId = CommandId::AppConfigure;
		break;
		case ButtonFileNew:
			commandId = CommandId::AppNew;
		break;
		case ButtonFileOpen:
			commandId = CommandId::AppOpen;
		break;
		case ButtonFileSave:
			commandId = CommandId::Save;
		break;
		case ButtonFileSaveAs:
			commandId = CommandId::SaveAs;
		break;
		case ButtonGotoYear:
			commandId = CommandId::GotoYear;
		break;
		case ButtonLaunchViewer:
			commandId = CommandId::LaunchViewer;
		break;
		case ButtonSelectDasa:
			commandId = CommandId::SelectDasa;
		break;
		case ButtonFilter:
			commandId = CommandId::Filter;
		break;
		case ButtonZoomIn:
			commandId = CommandId::ZoomIn;
		break;
		case ButtonZoomOriginal:
			commandId = CommandId::ZoomOriginal;
		break;
		case ButtonZoomOut:
			commandId = CommandId::ZoomOut;
		break;
		default:
			assert( false );
		break;
	}
	MCommand *command = CommandHandler::get()->getCommand( commandId );
	if ( ! command )
	{
		errorlog << "ToolbarFactory tool with id " << (int)tbcmd << " and cmd " << commandId << " not found";
	}
	else
	{
		struct MyBmpButton : public wxControl
		{
			MyBmpButton( wxWindow *parent, const int &cmdId, MCommand *command ) : wxControl( parent, cmdId )
			{
				wxBitmapButton *button = new wxBitmapButton( this, cmdId, ImageProvider::get()->getBitmapBundle( command->bmpId ));
				button->SetToolTip( command->text );

				wxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
				sizer->Add( button, 0, wxEXPAND|wxALL, 3 );

				SetSizer( sizer );
				Layout();
			}
		};
		toolbar->AddControl( new MyBmpButton( toolbar, (int)commandId, command ), command->text );
	}
}

/*************************************************//**
*
* 
*
******************************************************/
static const struct ToolbarCommandId
{
	CommandId cmdId;
	bool isDefault;
}
theIdList[] =
{
	{ CommandId::AppNew, true },
	{ CommandId::AppOpen, true },
	{ CommandId::Save, true },
	{ CommandId::SaveAs, false },
	{ CommandId::Print, true },
	{ CommandId::QuickPrint, false },
	{ CommandId::EditData, true },
	{ CommandId::None, true },

  { CommandId::NewD1Chart, true },
  { CommandId::NewD9Chart, true },
  { CommandId::NewD2Chart, false },
  { CommandId::NewD3Chart, false },
  { CommandId::NewD4Chart, false },
  { CommandId::NewD6Chart, false },
  { CommandId::NewD7Chart, false },
  { CommandId::NewD8Chart, false },
  { CommandId::NewD10Chart, false },
  { CommandId::NewD12Chart, false },
  { CommandId::NewD16Chart, false },
  { CommandId::NewD20Chart, false },
  { CommandId::NewD24Chart, false },
  { CommandId::NewD27Chart, false },
  { CommandId::NewD30Chart, false },
  { CommandId::NewD40Chart, false },
  { CommandId::NewD45Chart, false },
  { CommandId::NewD60Chart, false },
  { CommandId::NewD108Chart, false },
  { CommandId::NewD144Chart, false },
  { CommandId::NewBhavaChart, false },

	{ CommandId::NewSbc, true },
	{ CommandId::NewWesternChart, true },
	{ CommandId::NewUranianChart, true },

	{ CommandId::NewVarga, true },
	{ CommandId::NewText, true },
	{ CommandId::NewDasaTreeComposite, true },
	{ CommandId::NewDasaChart, true },
	{ CommandId::NewAshtakavarga, false },
	{ CommandId::NewDirection, true },
	{ CommandId::NewYoga, true },
	{ CommandId::NewSolar, false },
	{ CommandId::NewUranian, false },
	{ CommandId::NewSensitivePoints, false },
	{ CommandId::NewEventFinder, false },
	{ CommandId::None, true },

	{ CommandId::AppHora, false },
	{ CommandId::AppPartner, true },
	{ CommandId::AppEphemeris, true },
	{ CommandId::AppEclipse, false },
	{ CommandId::AppAtlasdialog, true },
	{ CommandId::AppConfigure, true },
	{ CommandId::AppHelp, false },
	{ CommandId::AppAbout, false }
};


/*****************************************************
**
**   ToolbarFactory   ---    init
**
******************************************************/
void ToolbarFactory::init()
{
	if ( gdiconfig->toolbar.mainToolbarItems.size() == 0)
	{
		for ( size_t i = 0; i < WXSIZEOF( theIdList ); i++ )
		{
			if ( theIdList[i].isDefault )
			{
				gdiconfig->toolbar.mainToolbarItems.push_back( theIdList[i].cmdId );
			}
		}
	}
}

/*****************************************************
**
**   ToolbarFactory   ---    getDefaultCommandIdList
**
******************************************************/
std::vector<CommandId> ToolbarFactory::getDefaultCommandIdList()
{
	std::vector<CommandId> l;
	for ( size_t i = 0; i < WXSIZEOF( theIdList ); i++ )
	{
		if ( theIdList[i].isDefault )
		{
			l.push_back( (CommandId)theIdList[i].cmdId );
		}
	}
	return l;
}

/*****************************************************
**
**   ToolbarFactory   ---    getFullCommandIdList
**
******************************************************/
std::vector<CommandId> ToolbarFactory::getFullCommandIdList()
{
	std::vector<CommandId> l;
	for ( size_t i = 0; i < WXSIZEOF( theIdList ); i++ )
	{
		if ( theIdList[i].cmdId != CommandId::None )
		{
			l.push_back( theIdList[i].cmdId );
		}
	}
	return l;
}

/*****************************************************
**
**   ToolbarFactory   ---    populateMainToolbar
**
******************************************************/
void ToolbarFactory::populateMainToolbar( wxToolBar *tb )
{
	assert( tb );
	tb->ClearTools();
	//tb->SetToolBitmapSize( wxSize( 24, 24 ));

	for( uint i = 0; i < gdiconfig->toolbar.mainToolbarItems.size(); i++ )
	{
		if ( gdiconfig->toolbar.mainToolbarItems[i] == CommandId::AppNew )
		{
			addTool( tb, gdiconfig->toolbar.mainToolbarItems[i], wxITEM_DROPDOWN );
			// tb->SetDropdownMenu( CommandId::AppNew, ContextMenuProvider().getToolbarNewMenu());
			tb->SetDropdownMenu( (int)CommandId::AppNew, new wxMenu );
		}
		else if ( gdiconfig->toolbar.mainToolbarItems[i] != CommandId::None )
		{
			addTool( tb, gdiconfig->toolbar.mainToolbarItems[i] );
		}
		else tb->AddSeparator();
	}
	tb->Realize();
}

/*****************************************************
**
**   ToolbarFactory   ---    getToolbarStyle
**
******************************************************/
int ToolbarFactory::getToolbarStyle()
{
	int style = wxTB_FLAT | wxTB_HORIZONTAL | wxTB_DOCKABLE;
	switch( gdiconfig->toolbar.toolbarStyle )
	{
		case 0: // text
			style |= wxTB_TEXT;
			style |= wxTB_NOICONS;
		break;
		case 1: // icon
		break;
		case 2: // text and icon
		default:
			style |= wxTB_TEXT;
		break;
	}
	return style;
}

/*****************************************************
**
**   ToolbarFactory   ---    addTool
**
******************************************************/
void ToolbarFactory::addTool( wxToolBar *tb, const CommandId &id,  const wxItemKind kind )
{
	CommandHandler *handler = CommandHandler::get();
	MCommand *command = handler->getCommand( id );

	if ( ! command )
	{
		errorlog << "tool with id " << id << " not found";
	}
	else
	{
		tb->AddTool( (int)id, command->getShortText(),
			ImageProvider::get()->getBitmapBundle( command->bmpId ),
			command->text, kind );
	}
}

