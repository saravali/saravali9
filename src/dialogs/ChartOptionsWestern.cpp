/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ChartOptionsWestern.cpp
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

#include "ChartOptionsWestern.h"

#include "ChartConfig.h"
#include "logger.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_PER_TYPE = wxID_HIGHEST + 1,
	CD_CHECK_ACTIVE,
	CD_BUTTON_PEN,
	CD_SPIN_ORBIS
};

/*****************************************************
**
**   ChartOptionsWestern   ---   Constructor
**
******************************************************/
ChartOptionsWestern::ChartOptionsWestern(  wxWindow *parent, ContextProps_ptr props, ChartConfig *cfg )
	: ChartOptionsPanel( parent, props, cfg )
{
	// begin wxGlade: ChartOptionsWestern::ChartOptionsWestern
	wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer* sizer_options = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Western Chart Options")), wxVERTICAL);
	sizer_main->Add(sizer_options, 0, wxEXPAND, 0);
	wxFlexGridSizer* grid_options = new wxFlexGridSizer(2, 2, 0, 0);
	sizer_options->Add(grid_options, 1, wxEXPAND, 0);
	label_orientation = new wxStaticText(sizer_options->GetStaticBox(), wxID_ANY, _("Chart Orientation"));
	grid_options->Add(label_orientation, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_leftside_choices[] = {
		_("Ascendant left"),
		_("1st House left"),
		_("Meridian top"),
		_("0 Aries top"),
		_("15 Aries top"),
		_("0 Cancer top"),
		_("15 Cancer top"),
		_("0 Leo top"),
		_("0 Libra top"),
		_("15 Libra top"),
		_("0 Capricorn top"),
		_("15 Capricorn top"),
	};
	choice_leftside = new wxChoice(sizer_options->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 12, choice_leftside_choices);
	choice_leftside->SetSelection(0);
	grid_options->Add(choice_leftside, 1, wxALL|wxEXPAND, 3);
	label_house_number_style = new wxStaticText(sizer_options->GetStaticBox(), wxID_ANY, _("House Number Style"));
	grid_options->Add(label_house_number_style, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_house_number_style_choices[] = {
		_("None"),
		_("Arabic"),
		_("Roman"),
	};
	choice_house_number_style = new wxChoice(sizer_options->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_house_number_style_choices);
	choice_house_number_style->SetSelection(0);
	grid_options->Add(choice_house_number_style, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_colors = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Use Colors for")), wxVERTICAL);
	sizer_main->Add(sizer_colors, 0, wxALL|wxEXPAND, 3);
	check_coloredplanets = new wxCheckBox(sizer_colors->GetStaticBox(), wxID_ANY, _("Planets"));
	sizer_colors->Add(check_coloredplanets, 0, wxALL, 3);
	check_use_sign_colors = new wxCheckBox(sizer_colors->GetStaticBox(), wxID_ANY, _("Signs"));
	sizer_colors->Add(check_use_sign_colors, 0, wxALL|wxEXPAND, 3);
	check_use_house_colors = new wxCheckBox(sizer_colors->GetStaticBox(), wxID_ANY, _("House Numbers"));
	sizer_colors->Add(check_use_house_colors, 0, wxALL|wxEXPAND, 3);
	
	grid_options->AddGrowableCol(1);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	// houses
	choice_house_number_style->SetValidator( wxGenericValidator( (int*)&cfg->houseNumberStyle ));

	// orientation
	choice_leftside->SetValidator( wxGenericValidator( (int*)&cfg->chartOrientation ));

	// other options
	check_coloredplanets->SetValidator( wxGenericValidator( &cfg->showPlanetColors ));
	check_use_sign_colors->SetValidator( wxGenericValidator( &cfg->showSignColors ));
	check_use_house_colors->SetValidator( wxGenericValidator( &cfg->showHouseColors ));
}

/*****************************************************
**
**   ChartOptionsWestern   ---   updateUi
**
******************************************************/
void ChartOptionsWestern::updateUi()
{
	debuglog << "ChartOptionsWestern::updateUi";

	const bool hasHouses = choice_house_number_style->GetSelection() != 0;
	check_use_house_colors->Enable( hasHouses );
}



