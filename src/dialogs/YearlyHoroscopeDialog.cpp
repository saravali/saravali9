/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/YearlyHoroscopeDialog.cpp
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

#include "YearlyHoroscopeDialog.h"

#include "astroconfig.h"
#include "AtlasDialog.h"
#include "DateFormatter.h"
#include "guibase.h"
#include "logger.h"
#include "mathbase.h"
#include "SolarHoroscope.h"
#include "SpinCtrl.h"

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/valtext.h>

extern Astroconfig *astroconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_SEARCHLOCATION = wxID_HIGHEST + 1, CD_NAME };
#define TIMER_PERIOD 100

/*****************************************************
**
**   YearlyHoroscopeDialog   ---   Constructor
**
******************************************************/
YearlyHoroscopeDialog::YearlyHoroscopeDialog(wxWindow* parent )
 : wxDialog(parent, -1, tr( "Yearly Horoscope" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ),
	configkey( "YearlyHoroscopeDialog" )
{
    // begin wxGlade: YearlyHoroscopeDialog::YearlyHoroscopeDialog
    SetTitle(_("Yearly Horoscope"));
    wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_vert = new wxBoxSizer(wxVERTICAL);
    sizer_main->Add(sizer_vert, 1, wxEXPAND, 0);
    wxBoxSizer* sizer_year = new wxBoxSizer(wxHORIZONTAL);
    sizer_vert->Add(sizer_year, 0, wxEXPAND, 0);
    wxStaticText* label_year = new wxStaticText(this, wxID_ANY, _("Year"));
    sizer_year->Add(label_year, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    spin_year = new wxSpinCtrl(this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -1500, 3000);
    sizer_year->Add(spin_year, 0, wxALL, 3);
    
    panel_preview = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    sizer_vert->Add(panel_preview, 0, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_preview = new wxBoxSizer(wxVERTICAL);
    label_date = new wxStaticText(panel_preview, wxID_ANY, _("YYYY-MM-DD HH:MM:SS"));
    sizer_preview->Add(label_date, 0, wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_location = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Location")), wxVERTICAL);
    sizer_vert->Add(sizer_location, 1, wxEXPAND, 0);
    wxBoxSizer* sizer_locname = new wxBoxSizer(wxHORIZONTAL);
    sizer_location->Add(sizer_locname, 0, wxALL|wxEXPAND, 3);
    text_name = new wxTextCtrl(this, CD_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    sizer_locname->Add(text_name, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    button_search = new wxButton(this, CD_SEARCHLOCATION, _("Search ..."));
    sizer_locname->Add(button_search, 0, wxALL, 3);
    wxFlexGridSizer* grid_coordinates = new wxFlexGridSizer(3, 2, 3, 3);
    sizer_location->Add(grid_coordinates, 1, wxALL|wxEXPAND, 3);
    label_longitude = new wxStaticText(this, wxID_ANY, _("Longitude"));
    grid_coordinates->Add(label_longitude, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    degree_longitude = new CombinedDegreeSpin(this, wxID_ANY, &location.longitude, DegreeType::Longitude);
    grid_coordinates->Add(degree_longitude, 1, wxALL|wxEXPAND, 3);
    label_latitude = new wxStaticText(this, wxID_ANY, _("Latitude"));
    grid_coordinates->Add(label_latitude, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    degree_latitude = new CombinedDegreeSpin(this, wxID_ANY, &location.latitude, DegreeType::Latitude);
    grid_coordinates->Add(degree_latitude, 1, wxALL|wxEXPAND, 3);
    label_timezone = new wxStaticText(this, wxID_ANY, _("Time Zone"));
    grid_coordinates->Add(label_timezone, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    degree_timezone = new CombinedDegreeSpin(this, wxID_ANY, &location.tz, DegreeType::Timezone);
    grid_coordinates->Add(degree_timezone, 1, wxALL|wxEXPAND, 3);
    static_line = new wxStaticLine(this, wxID_ANY);
    sizer_main->Add(static_line, 0, wxEXPAND, 0);
    wxBoxSizer* sizer_bottom = new wxBoxSizer(wxHORIZONTAL);
    sizer_main->Add(sizer_bottom, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
    button_ok = new wxButton(this, wxID_OK, _("OK"));
    button_ok->SetDefault();
    sizer_bottom->Add(button_ok, 0, wxALL, 3);
    button_cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
    sizer_bottom->Add(button_cancel, 0, wxALL, 3);
    
    grid_coordinates->AddGrowableCol(1);
    panel_preview->SetSizer(sizer_preview);
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    Layout();
    // end wxGlade

	location = astroconfig->standardLocation;
	spin_year->SetValue( getCurrentYear());

	text_name->SetValidator( wxTextValidator( wxFILTER_NONE, &location.locName ));

	setSavedWindowSize( this, configkey );
	updatePreviewLabel();

	Connect( CD_SEARCHLOCATION, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler( YearlyHoroscopeDialog::OnSearchLocation ));
	Connect( wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YearlyHoroscopeDialog::OnOK ));
	Connect( wxID_ANY, wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler( YearlyHoroscopeDialog::OnYearSpin ));
}

/*****************************************************
**
**   YearlyHoroscopeDialog   ---   Destructor
**
******************************************************/
YearlyHoroscopeDialog::~YearlyHoroscopeDialog()
{
	saveWindowSize( this, configkey );
}

/*****************************************************
**
**   YearlyHoroscopeDialog   ---   OnYearSpin
**
******************************************************/
void YearlyHoroscopeDialog::OnYearSpin( wxCommandEvent& )
{
	updatePreviewLabel();
}

/*****************************************************
**
**   YearlyHoroscopeDialog   ---   OnSearchLocation
**
******************************************************/
void YearlyHoroscopeDialog::OnSearchLocation( wxCommandEvent& )
{
	TransferDataFromWindow();
	AtlasDialogWrapper dialog( this, ATLASDIALOG_CALLER_DEFAULTLOCATIONPANEL, text_name->GetValue() );
	if ( dialog.run() == wxID_OK )
	{
		AtlasEntry *e = dialog.getSelectedEntry();
		location.locName = e->name;
		location.longitude = e->longitude;
		location.latitude = e->latitude;
		location.tz = e->tzoffset;
		debuglog << "OnSearchLocation long " << location.longitude << " lat " << location.latitude << " tz " << location.tz;

		TransferDataToWindow();
	}
}

/*****************************************************
**
**   YearlyHoroscopeDialog   ---   createYearlyHoroscope
**
******************************************************/
Horoscope_ptr YearlyHoroscopeDialog::createYearlyHoroscope()
{
	const int year = spin_year->GetValue();
	return SolarHoroscopeExpert::createYearlyHoroscope( year, location, astroconfig->general.defaultViewpoint );
}

/*****************************************************
**
**   YearlyHoroscopeDialog   ---   updatePreviewLabel
**
******************************************************/
void YearlyHoroscopeDialog::updatePreviewLabel()
{
	Horoscope_ptr  horoscope = createYearlyHoroscope();
	label_date->SetLabel( DateFormatter::get()->formatUTDatetime( horoscope->dl.date.jd ));
}

/*****************************************************
**
**   YearlyHoroscopeDialog   ---   OnOK
**
******************************************************/
void YearlyHoroscopeDialog::OnOK( wxCommandEvent& )
{
	debuglog << "YearlyHoroscopeDialog::OnOK";

	wxCommandEvent e( APPWINDOW_CREATEVIEW, GetId());
	e.SetEventObject( new HoroscopeWrapper( createYearlyHoroscope() ));
	wxPostEvent( GetParent(), e );

	EndModal( true );
}


