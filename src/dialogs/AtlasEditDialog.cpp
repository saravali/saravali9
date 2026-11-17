/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AtlasEditDialog.cpp
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

#include "AtlasEditDialog.h"

#include "AtlasLogic.h"
#include "AtlasAliasDialog.h"
#include "CommonControls.h"
#include "guibase.h"
#include "Lang.h"
#include "logger.h"
#include "SpinCtrl.h"

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>

#include <format>

// begin wxGlade: ::extracode
// end wxGlade

enum { AE_COUNTRY = wxID_HIGHEST + 1, AE_ADMIN, AE_ALIASES, AE_TZNAME };

/*****************************************************
**
**   AtlasEditDialog   ---   Constructor
**
******************************************************/
AtlasEditDialog::AtlasEditDialog(wxWindow* parent, AtlasLogic *logic, const AtlasEntry entry )
 : wxDialog(parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ),
	logic( logic ),
	configkey( "AtlasEditDialog" )
{
 model = entry;

    // begin wxGlade: AtlasEditDialog::AtlasEditDialog
    SetTitle(_("Edit Atlas Entry"));
    wxStaticBoxSizer* sizer_main = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxEmptyString), wxVERTICAL);
    wxStaticBoxSizer* sizer_left = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Attributes")), wxVERTICAL);
    sizer_main->Add(sizer_left, 1, wxEXPAND, 0);
    wxFlexGridSizer* grid_attributes = new wxFlexGridSizer(10, 2, 0, 0);
    sizer_left->Add(grid_attributes, 0, wxEXPAND, 0);
    label_name = new wxStaticText(this, wxID_ANY, _("Name"));
    grid_attributes->Add(label_name, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    text_name = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
    grid_attributes->Add(text_name, 0, wxALL|wxEXPAND, 3);
    label_asciiname = new wxStaticText(this, wxID_ANY, _("ASCII Name"));
    grid_attributes->Add(label_asciiname, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    text_asciiname = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
    grid_attributes->Add(text_asciiname, 0, wxALL|wxEXPAND, 3);
    label_country = new wxStaticText(this, wxID_ANY, _("Country"));
    grid_attributes->Add(label_country, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    const wxString choice_country_choices[] = {
        _("dummy"),
    };
    choice_country = new wxChoice(this, AE_COUNTRY, wxDefaultPosition, wxDefaultSize, 1, choice_country_choices);
    grid_attributes->Add(choice_country, 0, wxALL, 3);
    label_admincode = new wxStaticText(this, wxID_ANY, _("Admin Code"));
    grid_attributes->Add(label_admincode, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    const wxString choice_admin_choices[] = {
        _("dummy"),
    };
    choice_admin = new wxChoice(this, AE_ADMIN, wxDefaultPosition, wxDefaultSize, 1, choice_admin_choices);
    grid_attributes->Add(choice_admin, 0, wxALL, 3);
    label_population = new wxStaticText(this, wxID_ANY, _("Population"));
    grid_attributes->Add(label_population, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    spin_population = new wxSpinCtrl(this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000000000);
    grid_attributes->Add(spin_population, 0, wxALL|wxEXPAND, 3);
    label_longitude = new wxStaticText(this, wxID_ANY, _("Longitude"));
    grid_attributes->Add(label_longitude, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    degree_longitude = new CombinedDegreeSpin(this, wxID_ANY, &model.longitude, DegreeType::Longitude);
    grid_attributes->Add(degree_longitude, 1, wxALL|wxEXPAND, 3);
    label_latitude = new wxStaticText(this, wxID_ANY, _("Latitude"));
    grid_attributes->Add(label_latitude, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    degree_latitude = new CombinedDegreeSpin(this, wxID_ANY, &model.latitude, DegreeType::Latitude);
    grid_attributes->Add(degree_latitude, 1, wxALL|wxEXPAND, 3);
    label_timezone = new wxStaticText(this, wxID_ANY, _("Time Zone"));
    grid_attributes->Add(label_timezone, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    const wxString choice_timezone_choices[] = {
        _("dummy"),
    };
    choice_timezone = new wxChoice(this, AE_TZNAME, wxDefaultPosition, wxDefaultSize, 1, choice_timezone_choices);
    grid_attributes->Add(choice_timezone, 1, wxALL, 3);
    label_tzh = new wxStaticText(this, wxID_ANY, _("TZ Hours"));
    grid_attributes->Add(label_tzh, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_tzhours = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_attributes->Add(label_tzhours, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_aliases = new wxStaticText(this, wxID_ANY, _("Aliases"));
    grid_attributes->Add(label_aliases, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    button_aliases = new wxButton(this, AE_ALIASES, _("Edit Aliases ..."));
    grid_attributes->Add(button_aliases, 0, wxALIGN_RIGHT|wxALL, 3);
    text_alias = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
    text_alias->SetMinSize(wxSize(-1, 100));
    sizer_left->Add(text_alias, 1, wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_bottom = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxEmptyString), wxHORIZONTAL);
    sizer_main->Add(sizer_bottom, 0, wxALIGN_CENTER_HORIZONTAL, 3);
    button_ok = new wxButton(this, wxID_OK, _("OK"));
    sizer_bottom->Add(button_ok, 0, wxALL, 3);
    button_cancel_copy = new wxButton(this, wxID_CANCEL, _("Cancel"));
    sizer_bottom->Add(button_cancel_copy, 0, wxALL, 3);
    
    grid_attributes->AddGrowableCol(1);
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    Layout();
    // end wxGlade

	countrylist = logic->getAllCountries();
	choice_country->Clear();
	for ( const AtlasCountry &country : countrylist )
	{
		choice_country->Append( country.name );
	}
	tzlist = logic->getAllTimezones();

	text_name->ChangeValue( model.name );
	text_asciiname->ChangeValue( model.asciiname );
	text_alias->ChangeValue( model.aliases );

	spin_population->SetValidator( wxGenericValidator( &model.population ));

	choice_country->SetStringSelection( model.country );
	updateAdminChoice();
	updateTzChoice();

	setSavedWindowSize( this, configkey );

	Connect( AE_COUNTRY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AtlasEditDialog::OnChoiceCountry ));
	Connect( AE_TZNAME, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AtlasEditDialog::OnChoiceTzName ));
	Connect( AE_ALIASES, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasEditDialog::OnEntryAliases ));
	Connect( wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasEditDialog::OnOK ));
	Connect( wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AtlasEditDialog::OnTextChanged ));

	text_name->SetFocus();
}

/*****************************************************
**
**   AtlasEditDialog   ---   Destructor
**
******************************************************/
AtlasEditDialog::~AtlasEditDialog()
{
	saveWindowSize( this, configkey );
}

/*****************************************************
**
**   AtlasEditDialog   ---   updateAdminChoice
**
******************************************************/
void AtlasEditDialog::updateAdminChoice()
{
	choice_admin->Clear();
	choice_admin->Append( wxEmptyString );

	for ( const auto &name : logic->getAllAdminNamesForCountry( choice_country->GetStringSelection()))
	{
		choice_admin->Append( name );
	}
	if ( choice_admin->FindString( model.admin ) != wxNOT_FOUND ) choice_admin->SetStringSelection( model.admin );
	else choice_admin->SetSelection( 1 );
}

/*****************************************************
**
**   AtlasEditDialog   ---   OnChoiceCountry
**
******************************************************/
void AtlasEditDialog::OnChoiceCountry( wxCommandEvent& )
{
	updateAdminChoice();
	updateTzChoice();
}

/*****************************************************
**
**   AtlasEditDialog   ---   OnChoiceTzName
**
******************************************************/
void AtlasEditDialog::OnChoiceTzName( wxCommandEvent& )
{
	updateTzHours();
}

/*****************************************************
**
**   AtlasEditDialog   ---   updateTzHours
**
******************************************************/
void AtlasEditDialog::updateTzHours()
{
	wxString tzfullname = choice_timezone->GetStringSelection();

	for ( const auto &a : tzlist )
	{
		if ( a.name == tzfullname )
		{
			label_tzhours->SetLabel( std::format( "{:02.1f}", a.tzoffset));
			return;
		}
	}
	errorlog << "no time zone found for entry " << tzfullname;
}

/*****************************************************
**
**   AtlasEditDialog   ---   updateTzChoice
**
******************************************************/
void AtlasEditDialog::updateTzChoice()
{
	wxString country_code;
	wxString country = choice_country->GetStringSelection();

	// get country
	for ( const auto &a : countrylist )
	{
		if ( a.name == country )
		{
			country_code = a.iso;
			break;
		}
	}

	choice_timezone->Clear();
	for ( const auto &a : tzlist )
	{
		if ( a.country_code != country_code ) continue;

		wxString tzname = a.name;
		choice_timezone->Append( tzname );
	}
	if ( choice_timezone->FindString( model.timezone ) != wxNOT_FOUND )
		choice_timezone->SetStringSelection( model.timezone );
	else choice_timezone->SetSelection( 0 );

	updateTzHours();
}

/*****************************************************
**
**   AtlasEditDialog   ---   OnEntryAliases
**
******************************************************/
void AtlasEditDialog::OnEntryAliases( wxCommandEvent& )
{
	AtlasAliasDialog dialog( this, model );
	if ( dialog.ShowModal() == wxID_OK )
	{
		text_alias->SetValue( model.aliases );
	}
}

/*****************************************************
**
**   AtlasEditDialog   ---   OnOk
**
******************************************************/
void AtlasEditDialog::OnOK( wxCommandEvent& )
{
	TransferDataFromWindow();

	model.name = text_name->GetValue();
	model.asciiname = text_asciiname->GetValue();
	model.aliases = text_alias->GetValue();

	model.country = choice_country->GetStringSelection();
	model.country_code = logic->getCountryCodeForName( choice_country->GetStringSelection());

	model.admin = choice_admin->GetStringSelection();
	model.admin1_code = logic->getAdminCodeForCountryAndName(
		model.country_code,
		choice_admin->GetStringSelection());

	EndModal( wxID_OK );
}

/*****************************************************
**
**   AtlasEditDialog   ---   OnTextChanged
**
******************************************************/
void AtlasEditDialog::OnTextChanged( wxCommandEvent &event )
{
	Validate();
	event.Skip();
}

