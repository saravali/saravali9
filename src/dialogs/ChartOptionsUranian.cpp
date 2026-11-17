/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ChartOptionsUranian.cpp
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

#include "ChartOptionsUranian.h"

#include "ChartConfig.h"
#include "logger.h"

#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

// begin wxGlade: ::extracode
// end wxGlade

/*****************************************************
**
**   ChartOptionsUranian   ---   Constructor
**
******************************************************/
ChartOptionsUranian::ChartOptionsUranian( wxWindow *parent, ContextProps_ptr props, ChartConfig *cfg )
	: ChartOptionsPanel( parent, props, cfg )
{
	assert( cfg );

	// begin wxGlade: ChartOptionsUranian::ChartOptionsUranian
	wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer* sizer_uranianoptions = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Uranian Options")), wxVERTICAL);
	sizer_main->Add(sizer_uranianoptions, 0, wxALL|wxEXPAND, 3);
	check_coloredplanets = new wxCheckBox(sizer_uranianoptions->GetStaticBox(), wxID_ANY, _("Use Colors for Planets"));
	sizer_uranianoptions->Add(check_coloredplanets, 0, wxALL, 3);
	check_use_sign_colors = new wxCheckBox(sizer_uranianoptions->GetStaticBox(), wxID_ANY, _("Use Colors for Signs"));
	sizer_uranianoptions->Add(check_use_sign_colors, 0, wxALL|wxEXPAND, 3);
	wxBoxSizer* sizer_1 = new wxBoxSizer(wxHORIZONTAL);
	sizer_uranianoptions->Add(sizer_1, 1, wxEXPAND, 0);
	label_chartcenter = new wxStaticText(sizer_uranianoptions->GetStaticBox(), wxID_ANY, _("Center of Chart"));
	sizer_1->Add(label_chartcenter, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_chart_center_choices[] = {
		_("Nothing"),
		_("Division D-N"),
		_("Total Degrees"),
		_("D-N + Total Degrees"),
	};
	choice_chart_center = new wxChoice(sizer_uranianoptions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, choice_chart_center_choices);
	choice_chart_center->SetSelection(0);
	sizer_1->Add(choice_chart_center, 0, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_matches = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Events")), wxVERTICAL);
	sizer_main->Add(sizer_matches, 0, wxALL|wxEXPAND, 3);
	check_showmatches = new wxCheckBox(sizer_matches->GetStaticBox(), wxID_ANY, _("Show Matches on Mouseover"));
	sizer_matches->Add(check_showmatches, 0, wxALL, 3);
	wxStaticBoxSizer* sizer_shifted = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Shifted Charts")), wxVERTICAL);
	sizer_main->Add(sizer_shifted, 0, wxALL|wxEXPAND, 3);
	check_show_eha = new wxCheckBox(sizer_shifted->GetStaticBox(), wxID_ANY, _("Show Cardinal Points"));
	sizer_shifted->Add(check_show_eha, 0, wxALL, 3);
	
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	check_coloredplanets->SetValidator( wxGenericValidator( &cfg->showPlanetColors ));
	check_use_sign_colors->SetValidator( wxGenericValidator( &cfg->showSignColors ));
	choice_chart_center->SetValidator( wxGenericValidator( &cfg->chartCenterType ));

	check_showmatches->SetValidator( wxGenericValidator( &cfg->showMatchesOnMouseOver ));

	check_show_eha->SetValidator( wxGenericValidator( &cfg->showEarthHoroscopeAnglesInShiftedCharts ));
}

