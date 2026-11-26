/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/Sidebar.cpp
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

#include "Sidebar.h"

#include "astroconfig.h"
#include "CommonControls.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "guibase.h"
#include "Lang.h"
#include "logger.h"
#include "SpinCtrl.h"
#include "UranianControls.h"
#include "UranianTextLoader.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

//#define DEBUG_SIDEBAR

// Side bar gui element IDs for event handling
enum
{
	// Choices
	SBE_SORTORDER = SBE_BUTTON_UPDATE + 1,
	SBE_HOUSETYPE,
	SBE_CHART_DIVISION,
	SBE_MATCH_DIVISION,
	SBE_PROGRESSIONMODE,

	// check boxes
	SBE_TOGGLE_LRPERMUTATIONS,
	SBE_TOGGLE_DUEDATE,
	SBE_TOGGLE_DUEDATE_TIME,
	SBE_TOGGLE_SPEED,
	SBE_TOGGLE_RESIDUUM,
	SBE_TOGGLE_ROLE,
	SBE_TOGGLE_TEXT,

	SBE_TOGGLE_ANTISCIA,
	SBE_TOGGLE_OFFSET,

	SBE_TOGGLE_PPLONGITUDE,
	SBE_TOGGLE_PPSCORE,

	SBE_CHARTFORMULAE,
	SBE_MATCHFORMULAE,
};

enum class SidebarItem
{
	// choices
	ChartDivision = wxID_HIGHEST + 1000,
	MatchDivision,
	Sortorder,
	HouseType,
	ProgressionMode,
	PartnerChartOrigin,

	// spins
	Orbis,

	// check boxes
	Antiscia,
	LrPermutations,
	ActivateOffset,

	ChartFormulas,
	MatchFormulas,
	Columns,
	PpOptions,
	FilterPanel,
	RestrictorPanel,
	MatchCountControl,

	OffsetToggle,
	OffsetTextCtrl,

	ButtonUpdate,

	Separator
};

/*****************************************************
**
**   SidebarPanelFactory   ---   createSidebarPanel
**
******************************************************/
SidebarPanel *SidebarPanelFactory::createSidebarPanel( wxWindow *parent, ContextProps_ptr cprops,
	const UranianContext &ctx, UranianSection *section )
{
	this->props = cprops;
	this->ucontext = ctx;
	this->usection = section;
	assert( usection );

	SidebarPanel *panel = new SidebarPanel( parent, SBI_SIDEPANEL, props, ucontext, usection );
	wxBoxSizer* sizer_main = new wxBoxSizer( wxVERTICAL );
	panel->SetSizer( sizer_main );

	debuglog << "SidebarPanelFactory::createSidebarPanel uranian context " << ucontext;

	switch( ucontext )
	{
		case UranianContext::EventFinder:
			addItemGroup( panel, tr( "Options" ),
			{
				SidebarItem::MatchDivision,
				SidebarItem::Sortorder,
				SidebarItem::ProgressionMode,
				SidebarItem::MatchFormulas,
				SidebarItem::Antiscia,
				SidebarItem::MatchCountControl,
				SidebarItem::LrPermutations,
				SidebarItem::Columns
			} );
			addSeparator( panel );
			addItemGroup( panel, tr( "Calculation Filters" ),
			{
				SidebarItem::RestrictorPanel,
			});
			addItemGroup( panel, tr( "Update" ),
			{
				SidebarItem::ButtonUpdate
			});
		break;
		case UranianContext::RadixMatches:
			addItemGroup( panel, tr( "Matches" ),
			{
				SidebarItem::MatchDivision,
				SidebarItem::Sortorder,
				SidebarItem::Orbis,
				SidebarItem::MatchFormulas,
				SidebarItem::Antiscia,
				SidebarItem::MatchCountControl,
				SidebarItem::Columns,
			} );
			addSeparator( panel );
			addItemGroup( panel, tr( "Filter" ),
			{
				SidebarItem::FilterPanel
			});
		break;
		case UranianContext::RadixPp:
			addItemGroup( panel, tr( "Options" ),
			{
				SidebarItem::MatchDivision,
				SidebarItem::Sortorder,
				SidebarItem::Orbis,
				SidebarItem::MatchFormulas,
				SidebarItem::Antiscia,
				SidebarItem::MatchCountControl,
				SidebarItem::PpOptions,
			} );
			addSeparator( panel );
			addItemGroup( panel, tr( "Filter" ),
			{
				SidebarItem::FilterPanel
			});
		break;
		case UranianContext::RadixChart:
			addItemGroup( panel, tr( "Chart" ),
			{
				SidebarItem::ChartDivision,
				SidebarItem::HouseType,
				SidebarItem::ChartFormulas,
				SidebarItem::Antiscia,
			} );
			addSeparator( panel );
			addItemGroup( panel, tr( "Events" ),
			{
				SidebarItem::MatchDivision,
				SidebarItem::Orbis,
				SidebarItem::MatchFormulas,
				SidebarItem::MatchCountControl,
			});
			addSeparator( panel );
			addItemGroup( panel, tr( "Display filter" ),
			{
				SidebarItem::FilterPanel
			});
			addSeparator( panel );
			addItemGroup( panel, tr( "Offset" ),
			{
				SidebarItem::ActivateOffset,
				SidebarItem::OffsetTextCtrl,
			});
		break;

		case UranianContext::DirectionChart:
		case UranianContext::SolarChart:
		case UranianContext::PartnerChart:
			addItemGroup( panel, tr( "Chart" ),
			{
				SidebarItem::ChartDivision,
				SidebarItem::HouseType,
				SidebarItem::PartnerChartOrigin,
				SidebarItem::ChartFormulas,
				SidebarItem::Antiscia,
			} );
			addSeparator( panel );
			addItemGroup( panel, tr( "Events" ),
			{
				SidebarItem::MatchDivision,
				SidebarItem::Orbis,
				SidebarItem::MatchFormulas,
				SidebarItem::MatchCountControl,
			});
			addSeparator( panel );
			addItemGroup( panel, tr( "Display filter" ),
			{
				SidebarItem::FilterPanel
			});
		break;
		case UranianContext::DirectionMatches:
			addItemGroup( panel, tr( "Options" ),
			{
				SidebarItem::MatchDivision,
				SidebarItem::Sortorder,
				SidebarItem::ProgressionMode,
				SidebarItem::Orbis,
				SidebarItem::MatchFormulas,
				SidebarItem::Antiscia,
				SidebarItem::MatchCountControl,
				SidebarItem::LrPermutations,
				SidebarItem::Columns
			} );
			addSeparator( panel );
			addItemGroup( panel, tr( "Display filter" ),
			{
				SidebarItem::FilterPanel
			});
		break;
		case UranianContext::SolarMatches:
			addItemGroup( panel, tr( "Options" ),
			{
				SidebarItem::MatchDivision,
				SidebarItem::Sortorder,
				SidebarItem::Orbis,
				SidebarItem::MatchFormulas,
				SidebarItem::Antiscia,
				SidebarItem::MatchCountControl,
				SidebarItem::LrPermutations,
				SidebarItem::Columns
			} );
			addSeparator( panel );
			addItemGroup( panel, tr( "Display filter" ),
			{
				SidebarItem::FilterPanel
			});
		break;
		case UranianContext::DirectionPp:
		case UranianContext::SolarPp:
			addItemGroup( panel, tr( "Options" ),
			{
				SidebarItem::MatchDivision,
				SidebarItem::Sortorder,
				SidebarItem::Orbis,
				SidebarItem::MatchFormulas,
				SidebarItem::Antiscia,
				SidebarItem::MatchCountControl,
				SidebarItem::PpOptions,
			} );
			addSeparator( panel );
			addItemGroup( panel, tr( "Display filter" ),
			{
				SidebarItem::FilterPanel
			});
		break;

		case UranianContext::MultiDirectionChart:
			addItemGroup( panel, tr( "Chart" ),
			{
				SidebarItem::ChartDivision,
				SidebarItem::HouseType,
				SidebarItem::ChartFormulas,
				SidebarItem::Antiscia,
			} );
			addSeparator( panel );
			addItemGroup( panel, tr( "Events" ),
			{
				SidebarItem::MatchDivision,
				SidebarItem::Orbis,
				SidebarItem::MatchFormulas,
				SidebarItem::MatchCountControl,
			});
			addSeparator( panel );
			addItemGroup( panel, tr( "Filters" ),
			{
				SidebarItem::RestrictorPanel,
			});
		break;

		case UranianContext::MultiDirectionMatches:
			addItemGroup( panel, tr( "Options" ),
			{
				SidebarItem::MatchDivision,
				SidebarItem::Sortorder,
				SidebarItem::Orbis,
				SidebarItem::MatchFormulas,
				SidebarItem::Antiscia,
				SidebarItem::LrPermutations,
				SidebarItem::Columns,
			} );
			addSeparator( panel );
			addItemGroup( panel, tr( "Filters" ),
			{
				SidebarItem::RestrictorPanel,
			});
		break;

		case UranianContext::PartnerMatches:
			addItemGroup( panel, tr( "Options" ),
			{
				SidebarItem::MatchDivision,
				SidebarItem::Sortorder,
				SidebarItem::Orbis,
				SidebarItem::MatchFormulas,
				SidebarItem::MatchCountControl,
				SidebarItem::Antiscia,
				SidebarItem::LrPermutations,
				SidebarItem::Columns
			} );
			addSeparator( panel );
			addItemGroup( panel, tr( "Display filter" ),
			{
				SidebarItem::FilterPanel
			});
		break;
		case UranianContext::PartnerPp:
			addItemGroup( panel, tr( "Options" ),
			{
				SidebarItem::MatchDivision,
				SidebarItem::Sortorder,
				SidebarItem::Orbis,
				SidebarItem::MatchFormulas,
				SidebarItem::Antiscia,
				SidebarItem::MatchCountControl,
				SidebarItem::PpOptions
			} );
			addSeparator( panel );
			addItemGroup( panel, tr( "Display filter" ),
			{
				SidebarItem::FilterPanel
			});
		break;
		default:
			fatallog << "invalid context " << ucontext;
			assert( false );
		break;
	}
	sizer_main->Fit( panel );
	panel->TransferDataToWindow();
	panel->updateUi();
	return panel;
}

/*****************************************************
**
**   SidebarPanelFactory   ---   addItemGroup
**
******************************************************/
void SidebarPanelFactory::addItemGroup( SidebarPanel *panel, const wxString &title, const std::list<SidebarItem> &items )
{
	std::list<SidebarItem> pair_items;
	std::list<SidebarItem> single_items;

	wxStaticBoxSizer* sizer = new wxStaticBoxSizer( new wxStaticBox( panel, wxID_ANY, title ), wxVERTICAL );
	for( const SidebarItem &item: items )
	{
		switch( item )
		{
			// choices
			case SidebarItem::MatchDivision:
			case SidebarItem::ChartDivision:
			case SidebarItem::Sortorder:
			case SidebarItem::HouseType:
			case SidebarItem::ProgressionMode:
			case SidebarItem::PartnerChartOrigin:

			// spins
			case SidebarItem::Orbis:
				pair_items.push_back( item );
			break;

			// check boxes
			case SidebarItem::Antiscia:
			case SidebarItem::LrPermutations:
			case SidebarItem::ChartFormulas:
			case SidebarItem::MatchFormulas:
			case SidebarItem::FilterPanel:
			case SidebarItem::Columns:
			case SidebarItem::PpOptions:
			case SidebarItem::RestrictorPanel:
			case SidebarItem::MatchCountControl:
			case SidebarItem::ActivateOffset:
			case SidebarItem::OffsetTextCtrl:
			case SidebarItem::ButtonUpdate:
				single_items.push_back( item );
			break;
			default:
				assert( false );
			break;
		}
	}
	sizer->Add( createItemGridSizer( panel, pair_items ), 1, wxEXPAND, 0 );
	addSingleItems( panel, sizer, single_items );

	wxSizer *panel_sizer = panel->GetSizer();
	assert( panel_sizer );
	panel_sizer->Add( sizer, 0, wxEXPAND, 0 );
}

/*****************************************************
**
**   SidebarPanelFactory   ---   addSingleItems
**
******************************************************/
void SidebarPanelFactory::addSingleItems( SidebarPanel *panel, wxSizer *sizer, const std::list<SidebarItem> &items )
{
	assert( usection );

	for( const SidebarItem &item: items )
	{
		switch( item )
		{
			// check boxes
			case SidebarItem::Antiscia:
				addCheckboxItem( sizer, panel, SBE_TOGGLE_ANTISCIA, tr( "Include Antiscia" ), &usection->includeAntiscia );
			break;
			case SidebarItem::LrPermutations:
				addCheckboxItem( sizer, panel, SBE_TOGGLE_LRPERMUTATIONS, tr( "LR Permutations" ), &usection->eventsIncludeLrPermutations );
			break;

			case SidebarItem::ChartFormulas:
			{
				panel->formularyPanel = new FormularyPanel( panel, SBE_CHARTFORMULAE, ucontext, usection->chartFormulae );
				sizer->Add( panel->formularyPanel, 0, wxALL|wxEXPAND, 3 );
			}
			break;
			case SidebarItem::MatchFormulas:
			{
				panel->matchFormularyPanel = new FormularyPanel( panel, SBE_MATCHFORMULAE, ucontext, usection->matchFormulae );
				sizer->Add( panel->matchFormularyPanel, 0, wxALL|wxEXPAND, 3 );
			}
			break;
			case SidebarItem::ActivateOffset:
			{
				wxCheckBox *checkbox = new wxCheckBox( panel, SBE_TOGGLE_OFFSET, tr( "Activate Offset" ));
				checkbox->SetValue( false );
				sizer->Add( checkbox, 0, wxALL|wxEXPAND, 3 );
				panel->Connect( SBE_TOGGLE_OFFSET, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SidebarPanel::OnCheckOffsetToggle ));
			}
			break;
			case SidebarItem::OffsetTextCtrl:
			{
				panel->spinOffset = new DegreeSpin( panel, -1, &usection->offset, 0, 360 );
				panel->spinOffset->Enable( false );
				sizer->Add( panel->spinOffset, 0, wxALL|wxEXPAND, 3 );
			}
			break;

			case SidebarItem::Columns:
			{
				wxStaticBoxSizer* sizer_columns = new wxStaticBoxSizer( new wxStaticBox( panel, wxID_ANY, tr("Columns")), wxVERTICAL);

				const bool up = ( ucontext == UranianContext::PartnerMatches );
				const bool uf = ( ucontext == UranianContext::EventFinder );

				if ( ! up && ! uf )
				{
					addCheckboxItem( sizer_columns, panel, SBE_TOGGLE_DUEDATE, tr( "Due Date" ), &usection->matchesShowDuedate );
				}
				if ( ! up )
				{
					panel->checkDueDateTime = addCheckboxItem( sizer_columns, panel, SBE_TOGGLE_DUEDATE_TIME, tr( "Show Time in Due Date" ),
						&usection->matchesShowDuedateTime, false );
				}
				if ( ! up && ! uf )
				{
					addCheckboxItem( sizer_columns, panel, SBE_TOGGLE_SPEED, tr( "Speed" ), &usection->matchesShowSpeed, false );
				}

				addCheckboxItem( sizer_columns, panel, SBE_TOGGLE_RESIDUUM, tr( "Residuum" ), &usection->matchesShowResiduum, false );
				addCheckboxItem( sizer_columns, panel, SBE_TOGGLE_ROLE, tr( "Roles" ), &usection->matchesShowRole, false );

				if ( UranianTextLoader::get()->hasItemsForMode( true ))
				{
					addCheckboxItem( sizer_columns, panel, SBE_TOGGLE_TEXT, tr( "Text" ), &usection->matchesShowText, false );
				}

				sizer->Add(sizer_columns, 0, wxALL|wxEXPAND, 3);
			}
			break;

			case SidebarItem::PpOptions:
			{
				wxStaticBoxSizer* sizer_pp = new wxStaticBoxSizer( new wxStaticBox( panel, wxID_ANY, tr("Pp Options")), wxVERTICAL);
				addCheckboxItem( sizer_pp, panel, SBE_TOGGLE_PPLONGITUDE, tr( "Show Longitude" ), &usection->showPpLongitude );
				addCheckboxItem( sizer_pp, panel, SBE_TOGGLE_PPSCORE, tr( "Show Score" ), &usection->showPpScore );
				sizer->Add(sizer_pp, 0, wxALL|wxEXPAND, 3);
			}
			break;

			case SidebarItem::RestrictorPanel:
			{
				panel->restrictorPanel = new RestrictorPanel( panel, props, ucontext );
				sizer->Add( panel->restrictorPanel, 0, wxALL | wxEXPAND, 0);
				panel->Connect( wxID_ANY, COMMAND_FILTER_CHANGED, wxCommandEventHandler( SidebarPanel::OnCommand ));
			}
			break;
			case SidebarItem::FilterPanel:
				sizer->Add( new FilterPanel( panel, wxID_ANY, props, ucontext, usection->filter ), 1, wxEXPAND );
				panel->Connect( wxID_ANY, COMMAND_FILTER_CHANGED, wxCommandEventHandler( SidebarPanel::OnCommand ));
			break;

			case SidebarItem::MatchCountControl:
				panel->matchCountField = new TextOutputField( panel, wxID_ANY, "dummy" );
				sizer->Add( panel->matchCountField, 0, wxALL | wxEXPAND | wxALIGN_BOTTOM, 3 );
			break;

			case SidebarItem::ButtonUpdate:
				panel->buttonUpdate = new wxButton( panel, SBE_BUTTON_UPDATE, tr( "Start Update" ));
				sizer->Add( panel->buttonUpdate, 0, wxALL | wxCENTER, 3 );
			break;

			default:
				assert( false );
			break;
		}
	}
}

/*****************************************************
**
**   SidebarPanelFactory   ---   addSeparator
**
******************************************************/
void SidebarPanelFactory::addSeparator( SidebarPanel *panel )
{
	wxStaticLine *statline = new wxStaticLine( panel, wxHORIZONTAL );
	wxSizer *panel_sizer = panel->GetSizer();
	panel_sizer->Add( statline, 0, wxALL | wxEXPAND | wxALIGN_BOTTOM, 3 );
}

/*****************************************************
**
**   SidebarPanelFactory   ---   createItemGridSizer
**
******************************************************/
wxSizer *SidebarPanelFactory::createItemGridSizer( SidebarPanel *panel, const std::list<SidebarItem> &items )
{
	wxFlexGridSizer* grid = new wxFlexGridSizer( items.size(), 2, 0, 0 );
	grid->AddGrowableCol(1);

	for( const SidebarItem &item : items )
	{
		debuglog << "SidebarPanelFactory::createItemGridSizer itemn " << (int)item;
		switch( item )
		{
			case SidebarItem::ChartDivision:
				grid->Add( new wxStaticText(panel, -1, tr( "Dial")), 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );
				panel->choiceChartDivision = new DivisionChoice( panel, SBE_CHART_DIVISION );
				panel->choiceChartDivision->setDivision( usection->chartDivision );
				grid->Add( panel->choiceChartDivision, wxALL|wxEXPAND, 3 );
				panel->Connect( SBE_CHART_DIVISION, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SidebarPanel::OnChoice ));
			break;

			case SidebarItem::MatchDivision:
				grid->Add( new wxStaticText(panel, -1, tr( "Division" )), 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );
				panel->choiceMatchDivision = new DivisionChoice( panel, SBE_MATCH_DIVISION );
				panel->choiceMatchDivision->setDivision( usection->matchDivision );
				grid->Add( panel->choiceMatchDivision, wxALL|wxEXPAND, 3 );
				panel->Connect( SBE_MATCH_DIVISION, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SidebarPanel::OnChoice ));
			break;

			case SidebarItem::Sortorder:
				grid->Add( new wxStaticText(panel, wxID_ANY, tr("Order")) , 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );
				grid->Add( new SortOrderChoice( panel, SBE_SORTORDER, ucontext, &usection->sortOrder ),
					1, wxALL|wxEXPAND, 3);
				panel->Connect( SBE_SORTORDER, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SidebarPanel::OnSortCommand ));
			break;

			case SidebarItem::ProgressionMode:
				panel->labelProgressionMode = new wxStaticText( panel, wxID_ANY, tr("Progression Mode"));
				grid->Add( panel->labelProgressionMode, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );
				panel->choiceProgressionMode = new wxChoice( panel, SBE_PROGRESSIONMODE );
				panel->choiceProgressionMode->Append( tr( "Radix-Progression" ));
				panel->choiceProgressionMode->Append( tr( "Progression only" ));
				panel->choiceProgressionMode->Append( tr( "Both" ));
				grid->Add( panel->choiceProgressionMode, 1, wxALL|wxEXPAND, 3);
				panel->choiceProgressionMode->SetValidator( wxGenericValidator( &usection->progressionMode ));
				panel->Connect( SBE_PROGRESSIONMODE, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SidebarPanel::OnCommand ));
			break;

			case SidebarItem::HouseType:
				panel->labelHouseType = new wxStaticText(panel, wxID_ANY, tr("Houses"));
				grid->Add( panel->labelHouseType, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );
				panel->choiceHouse = new UranianHouseChoice( panel, SBE_HOUSETYPE );
				panel->choiceHouse->setSelectedPlanet( usection->houseSystemPlanetId );
				grid->Add( panel->choiceHouse, 0, wxALL|wxEXPAND, 3 );
				panel->Connect( SBE_HOUSETYPE, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SidebarPanel::OnChoice ));
			break;

			case SidebarItem::PartnerChartOrigin:
				grid->Add( new PartnerShiftPanel( panel, -1, usection ), 0, wxALL|wxEXPAND, 3 );
			break;

			case SidebarItem::Orbis:
			{
				grid->Add( new wxStaticText(panel, wxID_ANY, tr("Orbis")) , 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );
				wxBoxSizer* sizer_orbis = new wxBoxSizer(wxHORIZONTAL);
				panel->spinOrbis = new wxSpinCtrl( panel, (int)item, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 300 );
				panel->spinOrbis->SetValue( 60 * usection->orbis );
				sizer_orbis->Add( panel->spinOrbis, 1, wxALL|wxEXPAND, 3 );
				sizer_orbis->Add( new wxStaticText( panel, wxID_ANY, tr("min")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
				grid->Add( sizer_orbis, 1, wxALL|wxEXPAND, 3 );
				panel->Connect( (int)SidebarItem::Orbis, wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SidebarPanel::OnSpin ));
			}
			break;
			default:
				assert( false );
			break;
		}
	}
	return grid;
}

/*****************************************************
**
**   SidebarPanelFactory   ---   addCheckboxItem
**
******************************************************/
wxCheckBox *SidebarPanelFactory::addCheckboxItem( wxSizer *sizer, wxWindow *parent, const int &id, const wxString &label,
	bool *cfgitem, const bool updateNeeded )
{
	wxCheckBox *checkbox = new wxCheckBox( parent, id, label );

	checkbox->SetValidator( wxGenericValidator( cfgitem ));
	sizer->Add( checkbox, 0, wxALL, 3 );

	if ( updateNeeded )
	{
		parent->Connect( id, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SidebarPanel::OnCommand ));
	}
	else
	{
		parent->Connect( id, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SidebarPanel::OnRefreshCommand ));
	}
	return checkbox;
}

/*****************************************************
**
**   SidebarPanel   ---   Constructor
**
******************************************************/
SidebarPanel::SidebarPanel( wxWindow* parent, int id, ContextProps_ptr props, const UranianContext& ctx, UranianSection *ucfg )
	: wxPanel( parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL),
		props( props ),
		ucontext( ctx ),
		usection( ucfg )
{
	Connect( SBE_CHARTFORMULAE, COMMAND_FORMULA_CHANGED, wxCommandEventHandler( SidebarPanel::OnCommand ));
	Connect( SBE_MATCHFORMULAE, COMMAND_FORMULA_CHANGED, wxCommandEventHandler( SidebarPanel::OnMatchFormulas ));

	Connect( (int)SidebarItem::OffsetTextCtrl, wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SidebarPanel::OnOffsetSpin ));

	Connect( wxEVT_IDLE, wxIdleEventHandler( SidebarPanel::OnIdle ));
}

/*****************************************************
**
**   SidebarPanel   ---   OnCommand
**
******************************************************/
void SidebarPanel::OnCommand( wxCommandEvent &event )
{
	debuglog << "SidebarPanel::OnCommand " << event.GetId();
	updateUi();
	emitChangeEvent();
}

/*****************************************************
**
**   SidebarPanel   ---   OnRefreshCommand
**
******************************************************/
void SidebarPanel::OnRefreshCommand( wxCommandEvent &event )
{
	debuglog << "SidebarPanel::OnRefreshCommand " << event.GetId();
	updateUi();

	wxCommandEvent e( COMMAND_SIDEPANEL_REFRESH, GetId());
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   SidebarPanel   ---   OnSortCommand
**
******************************************************/
void SidebarPanel::OnSortCommand( wxCommandEvent &event )
{
	debuglog << "SidebarPanel::OnSortCommand " << event.GetId();
	updateUi();

	wxCommandEvent e( COMMAND_SIDEPANEL_SORT, GetId());
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   SidebarPanel   ---   setDegreeSpinValue
**
******************************************************/
void SidebarPanel::setDegreeSpinValue( const double& )
{
	TransferDataToWindow();
}

/*****************************************************
**
**   SidebarPanel   ---   OnCheckOffsetToggle
**
******************************************************/
void SidebarPanel::OnCheckOffsetToggle( wxCommandEvent &event )
{
	const bool isChecked = event.IsChecked();
	debuglog << "SidebarPanel::OnCheckOffsetToggle checked " << isChecked;
	if ( spinOffset )
	{
		spinOffset->Enable( isChecked );
	}

	usection->offsetToggle = isChecked;
	wxCommandEvent e( COMMAND_SIDEPANEL_OFFSET, GetId());
	e.SetInt( (int)isChecked );
	wxPostEvent( GetParent(), e );
	OnCommand( event );
}

/*****************************************************
**
**   SidebarPanel   ---   OnChoice
**
******************************************************/
void SidebarPanel::OnChoice( wxCommandEvent &event )
{
	debuglog << "SidebarPanel::OnChoice id " << event.GetId();
	switch( event.GetId())
	{
		case SBE_CHART_DIVISION:
			assert( choiceChartDivision );
			usection->chartDivision = choiceChartDivision->getDivision();
		break;
		case SBE_MATCH_DIVISION:
			assert( choiceMatchDivision );
			usection->matchDivision = choiceMatchDivision->getDivision();
		break;
		case SBE_HOUSETYPE:
			assert( choiceHouse );
			usection->houseSystemPlanetId = choiceHouse->getSelectedPlanet();
		break;
		default:
			assert( false );
		break;
	}
	updateUi();
	emitChangeEvent();
}

/*****************************************************
**
**   SidebarPanel   ---   OnOffsetSpin
**
******************************************************/
void SidebarPanel::OnOffsetSpin( wxSpinEvent& )
{
	debuglog << "SidebarPanel::OnOffsetSpin";
}

/*****************************************************
**
**   SidebarPanel   ---   OnSpin
**
******************************************************/
void SidebarPanel::OnSpin( wxSpinEvent& )
{
	debuglog << "SPIN";
	assert( spinOrbis );
	usection->orbis = spinOrbis->GetValue() / 60.0;
	emitChangeEvent();
}

/*****************************************************
**
**   SidebarPanel   ---   OnMatchFormulas
**
******************************************************/
void SidebarPanel::OnMatchFormulas( wxCommandEvent& )
{
	debuglog << "SidebarPanel::OnMatchFormulas";
	usection->matchCount = 0;
	updateUi();
	emitChangeEvent();
}

/*****************************************************
**
**   SidebarPanel   ---   OnToggleOffset
**
******************************************************/
void SidebarPanel::OnToggleOffset( wxCommandEvent &event )
{
	debuglog << "SidePanel OnToggleOffset " << event.IsChecked();
	usection->offsetToggle = event.IsChecked();
	wxCommandEvent e( COMMAND_SIDEPANEL_OFFSET, GetId());
	e.SetInt( (int)event.IsChecked());
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   SidebarPanel   ---   OnIdle
**
******************************************************/
void SidebarPanel::OnIdle( wxIdleEvent& )
{
	extern Astroconfig *astroconfig;
	static int count = 0;
	debuglog << "SidebarPanel::OnIdle";

	if ( matchCountField && myMatchCount != usection->matchCount )
	{
		wxString s;
		if ( usection->matchCount < (int)astroconfig->uranian.maxMatchCount )
		{
			s = std::format( "{}: {} {}", tr( "Result" ), usection->matchCount, tr( "matches" ));
		}
		else
		{
			s = tr( "WARN: too many matches" );
		}
		matchCountField->setLabel( s );
		matchCountField->Refresh();
		myMatchCount = usection->matchCount;
		debuglog << " SidebarPanel::OnIdle " << ++count << " match count " << usection->matchCount;
	}
}

/*****************************************************
**
**   SidebarPanel   ---   updateUi
**
******************************************************/
void SidebarPanel::updateUi()
{
	TransferDataFromWindow();
	debuglog << "SidebarPanel::updateUi context " << ucontext;

	if ( choiceHouse ) choiceHouse->Enable( usection->chartDivision == 1 );
	if ( labelHouseType ) labelHouseType->Enable( usection->chartDivision == 1 );

	// whether to activate choice for progression mode
	bool b = false;
	if ( ucontext == UranianContext::EventFinder )
	{
		if ( restrictorPanel )
		{
			b = restrictorPanel->hasActiveProgressions();
		}
	}
	else
	{
		b = props->hasActiveRole( HoroscopeRole::Progressive );
	}
	if ( choiceProgressionMode ) choiceProgressionMode->Enable( b );
	if ( labelProgressionMode ) labelProgressionMode->Enable( b );

	if ( checkDueDateTime )
	{
		checkDueDateTime->Enable( usection->matchesShowDuedate );
	}

	debuglog << "SidebarPanel::updateUi hasActiveRole( HoroscopeRole::Progressive ): " << std::boolalpha << b;
}

/*****************************************************
**
**   SidebarPanel   ---   emitChangeEvent
**
******************************************************/
void SidebarPanel::emitChangeEvent()
{
#ifdef DEBUG_SIDEBAR
	debuglog << "SidebarPanel::emitChangeEvent";
#endif

	wxCommandEvent e( COMMAND_SIDEPANEL_CHANGED, GetId());
	wxPostEvent( GetParent(), e );
}

