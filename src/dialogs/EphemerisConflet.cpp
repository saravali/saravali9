/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/EphemerisConflet.cpp
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

#include "EphemerisConflet.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "logger.h"
#include "TextValidator.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/dirdlg.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>

extern Astroconfig *astroconfig;

// begin wxGlade: ::extracode
// end wxGlade

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

enum { CD_SEPATH = wxID_HIGHEST + 1 };

/*****************************************************
**
**   EphemerisConflet   ---   Constructor
**
******************************************************/
EphemerisConflet::EphemerisConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, astroconfig->ephem )
{
	// begin wxGlade: EphemerisConflet::EphemerisConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* sizer_left = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_left, 0, wxALL, 3);
	wxStaticBoxSizer* sizer_calc = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Calculation")), wxVERTICAL);
	sizer_left->Add(sizer_calc, 0, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* grid_sizer_calc = new wxFlexGridSizer(3, 2, 3, 3);
	sizer_calc->Add(grid_sizer_calc, 1, wxALL|wxEXPAND, 3);
	label_timemodel = new wxStaticText(sizer_calc->GetStaticBox(), wxID_ANY, _("Time Model"));
	grid_sizer_calc->Add(label_timemodel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_timemodel_choices[] = {
		_("Universal"),
		_("Ephemeris"),
	};
	choice_timemodel = new wxChoice(sizer_calc->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_timemodel_choices);
	choice_timemodel->SetSelection(0);
	grid_sizer_calc->Add(choice_timemodel, 0, wxALL|wxEXPAND, 3);
	label_coord = new wxStaticText(sizer_calc->GetStaticBox(), wxID_ANY, _("Coordinates"));
	grid_sizer_calc->Add(label_coord, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_coord_choices[] = {
		_("Geocentric"),
		_("Topocentric"),
		_("Equatorial"),
		_("Barycentric"),
		_("Heliocentric"),
	};
	choice_coord = new wxChoice(sizer_calc->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 5, choice_coord_choices);
	choice_coord->SetSelection(0);
	grid_sizer_calc->Add(choice_coord, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
	label_pos = new wxStaticText(sizer_calc->GetStaticBox(), wxID_ANY, _("Positions"));
	grid_sizer_calc->Add(label_pos, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_pos_choices[] = {
		_("Apparent"),
		_("True"),
	};
	choice_pos = new wxChoice(sizer_calc->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_pos_choices);
	choice_pos->SetSelection(0);
	grid_sizer_calc->Add(choice_pos, 0, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* sizer_right = new wxFlexGridSizer(2, 1, 0, 0);
	sizer_main->Add(sizer_right, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_sunrise = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Sunrise and Sunset")), wxVERTICAL);
	sizer_right->Add(sizer_sunrise, 1, wxALL, 3);
	wxFlexGridSizer* sizer_sunrise_disk = new wxFlexGridSizer(1, 2, 3, 3);
	sizer_sunrise->Add(sizer_sunrise_disk, 1, wxALL, 3);
	label_sunrise_disk = new wxStaticText(sizer_sunrise->GetStaticBox(), wxID_ANY, _("Reference Point"));
	sizer_sunrise_disk->Add(label_sunrise_disk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_sunrise_disk_choices[] = {
		_("Center of Disk"),
		_("Edge of Disk"),
	};
	choice_sunrise_disk = new wxChoice(sizer_sunrise->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_sunrise_disk_choices);
	choice_sunrise_disk->SetSelection(0);
	sizer_sunrise_disk->Add(choice_sunrise_disk, 0, wxEXPAND, 0);
	check_sunrise_refrac = new wxCheckBox(sizer_sunrise->GetStaticBox(), wxID_ANY, _("Use Refraction"));
	check_sunrise_refrac->SetToolTip(_("Checked: apparent position will be used"));
	sizer_sunrise->Add(check_sunrise_refrac, 0, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_ephem_path = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Path for Ephemeris Data Files")), wxVERTICAL);
	sizer_right->Add(sizer_ephem_path, 1, wxALL|wxEXPAND, 3);
	button_sepath = new wxButton(sizer_ephem_path->GetStaticBox(), CD_SEPATH, _("Choose ..."));
	sizer_ephem_path->Add(button_sepath, 0, wxALL, 5);
	text_sepath = new wxTextCtrl(sizer_ephem_path->GetStaticBox(), wxID_ANY, wxEmptyString);
	text_sepath->SetToolTip(_("Only required for high precision calculation"));
	sizer_ephem_path->Add(text_sepath, 0, wxALL|wxEXPAND, 5);
	
	sizer_sunrise_disk->AddGrowableCol(0);
	sizer_right->AddGrowableCol(0);
	grid_sizer_calc->AddGrowableCol(1);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	choice_sunrise_disk->SetValidator( wxGenericValidator( &cfg.sunrise_def ));

	check_sunrise_refrac->SetValidator( wxGenericValidator( &cfg.sunrise_refrac ));
	text_sepath->SetValidator( DirectoryValidator( &cfg.sePath ));

	choice_timemodel->SetValidator( wxGenericValidator( &cfg.useEphemerisTime ));
	choice_coord->SetValidator( wxGenericValidator( (int*)&cfg.ephemCoordinateSystem ));
	choice_pos->SetValidator( wxGenericValidator( &cfg.useTruePositions ));

	Connect( wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( EphemerisConflet::OnCommand ));
	Connect( wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( EphemerisConflet::OnCommand ));
	Connect( CD_SEPATH, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EphemerisConflet::OnSelectSEPath ));
}

/*****************************************************
**
**   EphemerisConflet   ---   OnCommand
**
******************************************************/
void EphemerisConflet::OnCommand( wxCommandEvent& )
{
	debuglog << "EphemerisConflet::OnCommand";
	guiHasChanged();
	doNotShowAgainMessage( this, DoNotShowAgainTopic::EphemConfigChange,
		tr( "Changes in ephemeris configuration may result in wrong calculation results." ));
}

/*****************************************************
**
**   EphemerisConflet   ---   OnSelectSEPath
**
******************************************************/
void EphemerisConflet::OnSelectSEPath( wxCommandEvent& )
{
	wxDirDialog dialog( this, _( "Choose a directory" ), text_sepath->GetValue());
	if ( dialog.ShowModal() == wxID_OK )
	{
		text_sepath->SetValue( dialog.GetPath());
		guiHasChanged();
		//doNotShowAgainMessage( this, DoNotShowAgainTopic::EphemConfigChange );
	}
}

/*****************************************************
**
**   EphemerisConflet   ---   updateUi
**
******************************************************/
void EphemerisConflet::updateUi()
{
	debuglog << "EphemerisConflet::updateUi";
}


