/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PlanetFilterDialog.cpp
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

#include "PlanetFilterDialog.h"

#include "ContextProperties.h"
#include "guibase.h"
#include "Lang.h"
#include "logger.h"

// begin wxGlade: ::extracode
// end wxGlade

#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/statline.h>

enum { OD_ALL = wxID_HIGHEST + 300, OD_SLOW_PLANETS, OD_FAST_PLANETS, OD_NONE };

/*****************************************************
**
**   PlanetFilterDialog   ---   Constructor
**
******************************************************/
PlanetFilterDialog::PlanetFilterDialog( wxWindow* parent, ContextProps_ptr props, const UranianContext &ctx,
	const HoroscopeRole &role, const MatchFilter &f )
		: wxDialog( parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, DEFAULT_DIALOG_STYLE ),
		props( props ),
		filter( f ),
		configkey( "PlanetFilterDialog" )
{
    // begin wxGlade: PlanetFilterDialog::PlanetFilterDialog
    wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* sizer_top = new wxFlexGridSizer(1, 2, 0, 0);
    sizer_main->Add(sizer_top, 1, wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_list = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Matching Planets")), wxHORIZONTAL);
    sizer_top->Add(sizer_list, 1, wxALL|wxEXPAND, 3);
    list = new wxCheckListBox(this, wxID_ANY);
    sizer_list->Add(list, 1, wxALL|wxEXPAND, 3);
    wxFlexGridSizer* sizer_right = new wxFlexGridSizer(3, 1, 0, 0);
    sizer_top->Add(sizer_right, 1, 0, 3);
    wxStaticBoxSizer* sizer_quick_select = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Quick Select")), wxVERTICAL);
    sizer_right->Add(sizer_quick_select, 0, 0, 3);
    button_none = new wxButton(this, OD_NONE, _("None"));
    sizer_quick_select->Add(button_none, 0, wxALL|wxEXPAND, 3);
    button_fast_planets = new wxButton(this, OD_FAST_PLANETS, _("Fast Planets"));
    sizer_quick_select->Add(button_fast_planets, 0, wxALL|wxEXPAND, 3);
    button_slow_planets = new wxButton(this, OD_SLOW_PLANETS, _("Slow Planets"));
    sizer_quick_select->Add(button_slow_planets, 0, wxALL|wxEXPAND, 3);
    button_all = new wxButton(this, OD_ALL, _("All"));
    sizer_quick_select->Add(button_all, 0, wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_side = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Matching Side")), wxVERTICAL);
    sizer_right->Add(sizer_side, 1, wxALL|wxEXPAND, 3);
    const wxString choice_side_choices[] = {
        _("Left"),
        _("Right"),
        _("Both"),
    };
    choice_side = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_side_choices);
    choice_side->SetSelection(0);
    sizer_side->Add(choice_side, 0, wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_rule = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Rule")), wxVERTICAL);
    sizer_right->Add(sizer_rule, 1, wxALL|wxEXPAND, 3);
    const wxString choice_rule_choices[] = {
        _("One must match"),
        _("All must be contained"),
        _("Only selected"),
    };
    choice_rule = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_rule_choices);
    choice_rule->SetSelection(0);
    sizer_rule->Add(choice_rule, 0, wxALL|wxEXPAND, 3);
    static_line_2 = new wxStaticLine(this, wxID_ANY);
    sizer_main->Add(static_line_2, 0, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_bottom = new wxBoxSizer(wxHORIZONTAL);
    sizer_main->Add(sizer_bottom, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
    button_ok = new wxButton(this, wxID_OK, _("OK"));
    button_ok->SetDefault();
    sizer_bottom->Add(button_ok, 0, wxALL, 3);
    button_cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
    sizer_bottom->Add(button_cancel, 0, wxALL, 3);
    
    sizer_top->AddGrowableRow(0);
    sizer_top->AddGrowableCol(0);
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    Layout();
    // end wxGlade

	switch( ctx )
	{
		using enum UranianContext;

		// one sided
		case RadixPp:
		case RadixChart:
		case DirectionPp:
		case DirectionChart:
		case MultiDirectionChart:
		case SolarPp:
		case SolarChart:
		case PartnerPp:
		case PartnerChart:
			SetTitle( tr("Configure term filter"));
			sizer_side->Show( false );
			choice_side->Show( false );
		break;

		// double sided
		case RadixMatches:
		case DirectionMatches:
		case MultiDirectionMatches:
		case SolarMatches:
		case PartnerMatches:
		case EventFinder:
			SetTitle( tr("Configure term filter"));
		break;

		// planet only: spwidget
		case SensitivePoints:
			SetTitle( tr("Configure active planets"));
			sizer_side->Show( false );
			choice_side->Show( false );
			sizer_rule->Show( false );
			choice_rule->Show( false );
		break;

		default:
			assert( false );
		break;
	}

	Lang lang;
	std::set<FilterGroup> excludes;
	switch( role )
	{
		case HoroscopeRole::Radix:
		case HoroscopeRole::Transit:
		case HoroscopeRole::Progressive:
		case HoroscopeRole::Lunations:
			excludes = { FilterGroup::CardinalPoints };
		break;
		default:
		break;
	}
	for( const auto &p : props->getPlanets( Viewpoint::Uranian, excludes ))
	{
		model.push_back( { p, filter.isPassThrough() || filter.selectedPlanets.contains( p ) } );
		list->Append( lang.getPlanetName( p, Viewpoint::Uranian, TextFormat::Long ));
	}
	updateSelections();

	choice_side->SetSelection( filter.side );
	choice_rule->SetSelection( filter.rule );

	Connect( wxEVT_CHECKLISTBOX, wxCommandEventHandler( PlanetFilterDialog::OnItemChecked ));

	Connect( OD_ALL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlanetFilterDialog::OnAll ));
	Connect( OD_FAST_PLANETS, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler( PlanetFilterDialog::OnFastPlanets ));
	Connect( OD_SLOW_PLANETS, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler( PlanetFilterDialog::OnSlowPlanets ));
	Connect( OD_NONE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlanetFilterDialog::OnNone ));
	Connect( wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlanetFilterDialog::OnOK ));

	setSavedWindowSize( this, configkey );
}

/*****************************************************
**
**   PlanetFilterDialog   ---   Destructor
**
******************************************************/
PlanetFilterDialog::~PlanetFilterDialog()
{
	saveWindowSize( this, configkey );
}

/*****************************************************
**
**   PlanetFilterDialog   ---   updateSelections
**
******************************************************/
void PlanetFilterDialog::updateSelections()
{
	assert( model.size() == list->GetCount());
	for( uint i = 0; i < model.size(); i++ )
	{
		list->Check( i, model[i].second );
	}
}

/*****************************************************
**
**   PlanetFilterDialog   ---   OnOK
**
******************************************************/
void PlanetFilterDialog::OnOK( wxCommandEvent& )
{
	filter.side = choice_side->GetSelection();
	filter.rule = choice_rule->GetSelection();

	bool allSelected = true;
	for( auto &a : model )
	{
		if ( ! a.second )
		{
			allSelected = false;
			break;
		}
	}

	filter.selectedPlanets.clear();
	if ( ! allSelected )
	{
		for( auto &a : model )
		{
			if ( a.second )
			{
				filter.selectedPlanets.insert( a.first );
			}
		}
	}
	debuglog << "PlanetFilterDialog::OnOK: Filter has " << filter.selectedPlanets.size() << " selected entries.";
	EndModal( wxID_OK );
}

/*****************************************************
**
**   PlanetFilterDialog   ---   OnItemChecked
**
******************************************************/
void PlanetFilterDialog::OnItemChecked( wxCommandEvent &event )
{
	const uint sel = event.GetInt();
	assert( sel < model.size());

	model[sel].second = list->IsChecked( sel );
}

/*****************************************************
**
**   PlanetFilterDialog   ---   OnAll
**
******************************************************/
void PlanetFilterDialog::OnAll( wxCommandEvent& )
{
	for( auto &a : model ) { a.second = true; }
	updateSelections();
}

/*****************************************************
**
**   PlanetFilterDialog   ---   OnFastPlanets
**
******************************************************/
void PlanetFilterDialog::OnFastPlanets( wxCommandEvent& )
{
	for( auto &a : model )
	{
		a.second = ( a.first <= PlanetId::Mars || PlanetHelper::isAngle( a.first ));
	}
	updateSelections();
}

/*****************************************************
**
**   PlanetFilterDialog   ---   OnSlowPlanets
**
******************************************************/
void PlanetFilterDialog::OnSlowPlanets( wxCommandEvent& )
{
	for( auto &a : model )
	{
		a.second = ( a.first > PlanetId::Mars && ! PlanetHelper::isAngle( a.first ));
	}
	updateSelections();
}

/*****************************************************
**
**   PlanetFilterDialog   ---   OnNone
**
******************************************************/
void PlanetFilterDialog::OnNone( wxCommandEvent& )
{
	for( auto &a : model ) { a.second = false; }
	updateSelections();
}

