/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/StandardLocationConflet.cpp
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

#include "StandardLocationConflet.h"
#include "AtlasDialog.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "SpinCtrl.h"
#include "TextValidator.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/valgen.h>

extern Astroconfig *astroconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_SEARCHLOCATION = wxID_HIGHEST + 1, CD_NAME, CD_TIMER };

// timer period in millisec
#define TIMER_PERIOD 1000

/*****************************************************
**
**   StandardLocationConflet   ---   Constructor
**
******************************************************/
StandardLocationConflet::StandardLocationConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, astroconfig->standardLocation )
{
	// begin wxGlade: StandardLocationConflet::StandardLocationConflet
	wxFlexGridSizer* sizer_main = new wxFlexGridSizer(1, 1, 0, 0);
	wxStaticBoxSizer* sizer_standard_location = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Standard Location")), wxVERTICAL);
	sizer_main->Add(sizer_standard_location, 1, wxEXPAND, 0);
	wxBoxSizer* sizer_locname = new wxBoxSizer(wxHORIZONTAL);
	sizer_standard_location->Add(sizer_locname, 0, wxALL|wxEXPAND, 3);
	text_name = new wxTextCtrl(sizer_standard_location->GetStaticBox(), CD_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	sizer_locname->Add(text_name, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_search = new wxButton(sizer_standard_location->GetStaticBox(), CD_SEARCHLOCATION, _("Search ..."));
	sizer_locname->Add(button_search, 0, wxALL, 3);
	wxStaticBoxSizer* sizer_coordinates = new wxStaticBoxSizer(new wxStaticBox(sizer_standard_location->GetStaticBox(), wxID_ANY, _("Coordinates")), wxHORIZONTAL);
	sizer_standard_location->Add(sizer_coordinates, 0, wxEXPAND, 0);
	wxFlexGridSizer* grid_coordinates = new wxFlexGridSizer(3, 2, 3, 3);
	sizer_coordinates->Add(grid_coordinates, 1, wxALL, 3);
	label_longitude = new wxStaticText(sizer_coordinates->GetStaticBox(), wxID_ANY, _("Longitude"));
	grid_coordinates->Add(label_longitude, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	degree_longitude = new CombinedDegreeSpin(sizer_coordinates->GetStaticBox(), wxID_ANY, &cfg.longitude, DegreeType::Longitude);
	grid_coordinates->Add(degree_longitude, 1, wxALL|wxEXPAND, 3);
	label_latitude = new wxStaticText(sizer_coordinates->GetStaticBox(), wxID_ANY, _("Latitude"));
	grid_coordinates->Add(label_latitude, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	degree_latitude = new CombinedDegreeSpin(sizer_coordinates->GetStaticBox(), wxID_ANY, &cfg.latitude, DegreeType::Latitude);
	grid_coordinates->Add(degree_latitude, 1, wxALL|wxEXPAND, 3);
	label_timezone = new wxStaticText(sizer_coordinates->GetStaticBox(), wxID_ANY, _("Time Zone"));
	grid_coordinates->Add(label_timezone, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	degree_timezone = new CombinedDegreeSpin(sizer_coordinates->GetStaticBox(), wxID_ANY, &cfg.tz, DegreeType::Timezone);
	grid_coordinates->Add(degree_timezone, 1, wxALL|wxEXPAND, 3);
	wxBoxSizer* sizer_bottom = new wxBoxSizer(wxHORIZONTAL);
	sizer_standard_location->Add(sizer_bottom, 1, wxALL|wxEXPAND, 3);
	wxBoxSizer* sizer_tzleft = new wxBoxSizer(wxVERTICAL);
	sizer_bottom->Add(sizer_tzleft, 1, wxEXPAND, 0);
	wxStaticBoxSizer* sizer_tz = new wxStaticBoxSizer(new wxStaticBox(sizer_standard_location->GetStaticBox(), wxID_ANY, _("Daylight Saving Time (DST)")), wxVERTICAL);
	sizer_tzleft->Add(sizer_tz, 1, wxEXPAND, 0);
	const wxString radio_tzmode_choices[] = {
		_("Automatic Values from System Settings"),
		_("Off"),
		_("On: Use One Hour"),
	};
	radio_tzmode = new wxRadioBox(sizer_tz->GetStaticBox(), wxID_ANY, _("Rule"), wxDefaultPosition, wxDefaultSize, 3, radio_tzmode_choices, 1, wxRA_SPECIFY_COLS);
	radio_tzmode->SetSelection(0);
	sizer_tz->Add(radio_tzmode, 0, wxALL, 4);
	
	panel_currenttime = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
	sizer_bottom->Add(panel_currenttime, 1, wxALL|wxEXPAND, 3);
	wxBoxSizer* sizer_currenttime2 = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer* sizer_tzsettings = new wxStaticBoxSizer(new wxStaticBox(panel_currenttime, wxID_ANY, _("Current Time and Offsets")), wxVERTICAL);
	sizer_currenttime2->Add(sizer_tzsettings, 1, wxALL|wxEXPAND, 3);
	wxGridSizer* grid_systemsettings = new wxGridSizer(4, 2, 3, 6);
	sizer_tzsettings->Add(grid_systemsettings, 1, wxALL|wxEXPAND, 3);
	label_localtime = new wxStaticText(sizer_tzsettings->GetStaticBox(), wxID_ANY, _("Local Time"));
	grid_systemsettings->Add(label_localtime, 0, wxALIGN_CENTER_VERTICAL, 0);
	text_localtime = new wxStaticText(sizer_tzsettings->GetStaticBox(), wxID_ANY, _("00:00:00"));
	grid_systemsettings->Add(text_localtime, 0, wxALIGN_CENTER_VERTICAL, 0);
	label_uttime = new wxStaticText(sizer_tzsettings->GetStaticBox(), wxID_ANY, _("Universal Time"));
	grid_systemsettings->Add(label_uttime, 0, wxALIGN_CENTER_VERTICAL, 0);
	text_uttime = new wxStaticText(sizer_tzsettings->GetStaticBox(), wxID_ANY, _("00:00:00"));
	grid_systemsettings->Add(text_uttime, 0, wxALIGN_CENTER_VERTICAL, 0);
	label_tzoffset = new wxStaticText(sizer_tzsettings->GetStaticBox(), wxID_ANY, _("Time Zone Offset"));
	grid_systemsettings->Add(label_tzoffset, 0, wxALIGN_CENTER_VERTICAL, 0);
	text_tzoffset = new wxStaticText(sizer_tzsettings->GetStaticBox(), wxID_ANY, _("+00:00"));
	grid_systemsettings->Add(text_tzoffset, 0, wxALIGN_CENTER_VERTICAL, 0);
	label_dstoffset = new wxStaticText(sizer_tzsettings->GetStaticBox(), wxID_ANY, _("DST Offset"));
	grid_systemsettings->Add(label_dstoffset, 0, wxALIGN_CENTER_VERTICAL, 0);
	text_dstoffset = new wxStaticText(sizer_tzsettings->GetStaticBox(), wxID_ANY, _("+00:00"));
	grid_systemsettings->Add(text_dstoffset, 0, wxALIGN_CENTER_VERTICAL, 0);
	wxStaticBoxSizer* sizer_angles = new wxStaticBoxSizer(new wxStaticBox(panel_currenttime, wxID_ANY, _("Current Angles")), wxVERTICAL);
	sizer_currenttime2->Add(sizer_angles, 1, wxALL|wxEXPAND, 3);
	wxGridSizer* grid_angles = new wxGridSizer(3, 2, 3, 6);
	sizer_angles->Add(grid_angles, 1, wxALL|wxEXPAND, 3);
	label_viewpoint = new wxStaticText(sizer_angles->GetStaticBox(), wxID_ANY, _("Viewpoint"));
	grid_angles->Add(label_viewpoint, 0, wxALIGN_CENTER_VERTICAL, 0);
	text_viewpoint = new wxStaticText(sizer_angles->GetStaticBox(), wxID_ANY, _("..."));
	grid_angles->Add(text_viewpoint, 0, wxALIGN_CENTER_VERTICAL, 0);
	label_ascendant = new wxStaticText(sizer_angles->GetStaticBox(), wxID_ANY, _("Ascendant"));
	grid_angles->Add(label_ascendant, 0, wxALIGN_CENTER_VERTICAL, 0);
	text_ascendant = new wxStaticText(sizer_angles->GetStaticBox(), wxID_ANY, _("00:00:00 Aries"));
	grid_angles->Add(text_ascendant, 0, wxALIGN_CENTER_VERTICAL, 0);
	label_meridian = new wxStaticText(sizer_angles->GetStaticBox(), wxID_ANY, _("Meridian"));
	grid_angles->Add(label_meridian, 0, wxALIGN_CENTER_VERTICAL, 0);
	text_meridian = new wxStaticText(sizer_angles->GetStaticBox(), wxID_ANY, _("00:00:00 Aries"));
	grid_angles->Add(text_meridian, 0, wxALIGN_CENTER_VERTICAL, 0);
	
	panel_currenttime->SetSizer(sizer_currenttime2);
	grid_coordinates->AddGrowableCol(1);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	text_viewpoint->SetLabel( enum_to_string( astroconfig->general.defaultViewpoint ));

	text_name->SetValidator( StringValidator( &cfg.locName ));
	radio_tzmode->SetValidator( wxGenericValidator( (int*)&cfg.dstMode ));

	timer = new wxTimer( this, CD_TIMER );
	// start is in onActivate()

	Connect( CD_NAME, wxEVT_COMMAND_TEXT_ENTER, wxTextEventHandler( StandardLocationConflet::OnTextEnter ));
	Connect( CD_SEARCHLOCATION, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler( StandardLocationConflet::OnSearchLocation ));

	Connect( CD_TIMER, wxEVT_TIMER, wxTimerEventHandler( StandardLocationConflet::OnTimer ));
}

/*****************************************************
**
**   StandardLocationConflet   ---   Destructor
**
******************************************************/
StandardLocationConflet::~StandardLocationConflet()
{
	debuglog << "StandardLocationConflet::Destructor";
	if ( timer->IsRunning() ) timer->Stop();
}

/*****************************************************
**
**   StandardLocationConflet   ---   TransferDataFromWindow
**
******************************************************/
bool StandardLocationConflet::TransferDataFromWindow()
{
	debuglog << "StandardLocationConflet::TransferDataToWindow";
	cfg.dstMode = (DaylightSavingMode)radio_tzmode->GetSelection();
	return wxPanel::TransferDataFromWindow();
}

/*****************************************************
**
**   StandardLocationConflet   ---   TransferDataToWindow
**
******************************************************/
bool StandardLocationConflet::TransferDataToWindow()
{
	debuglog << "StandardLocationConflet::TransferDataToWindow";
	radio_tzmode->SetSelection( (int)cfg.dstMode );
	return wxPanel::TransferDataToWindow();
}

/*****************************************************
**
**   StandardLocationConflet   ---   updateUi
**
******************************************************/
void StandardLocationConflet::updateUi()
{
	const bool valid = Validate();
	debuglog << "StandardLocationConflet::updateUi valid " << std::boolalpha << valid << " dst mode " << cfg.dstMode;

	DateFormatter *dh = DateFormatter::get();
	Formatter *fmt = Formatter::get();

	const double jd = MDate::getCurrentJD();
	const double tzoffset = cfg.tz;
	const double dstoffset = dh->getDstOffset( jd, cfg.dstMode );
	
	text_localtime->SetLabel( fmt->formatTime(
		getTimeFromJD( jd + ( tzoffset + dstoffset ) / 24.0 )));
	text_uttime->SetLabel( dh->formatTime( jd, false ));
	text_tzoffset->SetLabel( fmt->formatDegrees( tzoffset, DegPrecision::Minute ));
	text_dstoffset->SetLabel( fmt->formatDegrees( dstoffset, DegPrecision::Minute ));

	Horoscope_ptr horoscope( new Horoscope );

  horoscope->dl.location = cfg;
  horoscope->setDate( MDate::getCurrentJD());
  horoscope->dl.location.dst = dh->getDstOffset( horoscope->dl.date.jd, astroconfig->standardLocation.dstMode );
  horoscope->update();
	debuglog << "StandardLocationConflet::updateUi dstMode " << cfg.dstMode;

  const Viewpoint viewpoint = astroconfig->general.defaultViewpoint;

  Formatter *formatter = Formatter::get();

  text_ascendant->SetLabel( formatter->formatZodiacalLongitude( viewpoint, horoscope->getLongitude( PlanetId::Ascendant, viewpoint )).toWxString());
  text_meridian->SetLabel( formatter->formatZodiacalLongitude( viewpoint, horoscope->getLongitude( PlanetId::Meridian, viewpoint )).toWxString());
}

/*****************************************************
**
**   StandardLocationConflet   ---   OnTextEnter
**
******************************************************/
void StandardLocationConflet::OnTextEnter( wxCommandEvent &event )
{
	debuglog << "StandardLocationConflet::OnTextEnter";
	event.Skip();
	searchLocation();
}

/*****************************************************
**
**   StandardLocationConflet   ---   OnSearchLocation
**
******************************************************/
void StandardLocationConflet::OnSearchLocation( wxCommandEvent& )
{
	searchLocation();
}

/*****************************************************
**
**   StandardLocationConflet   ---   searchLocation
**
******************************************************/
void StandardLocationConflet::searchLocation()
{
	TransferDataFromWindow();
	AtlasDialogWrapper dialog( this, ATLASDIALOG_CALLER_DEFAULTLOCATIONPANEL, text_name->GetValue() );
	if ( dialog.run() == wxID_OK )
	{
		AtlasEntry *e = dialog.getSelectedEntry();
		cfg.locName = e->name;
		cfg.longitude = e->longitude;
		cfg.latitude = e->latitude;
		cfg.tz = e->tzoffset;
		debuglog << "OnSearchLocation long " << cfg.longitude << " lat " << cfg.latitude << " tz " << cfg.tz;

		TransferDataToWindow();
		setDirty();
		guiHasChanged();
	}
}

/*****************************************************
**
**   StandardLocationConflet   ---   onActivate
**
******************************************************/
void StandardLocationConflet::onActivate()
{
	debuglog << "StandardLocationConflet::onActivate";
	timer->Start( TIMER_PERIOD );
	updateUi();
}

/*****************************************************
**
**   StandardLocationConflet   ---   onPassivate
**
******************************************************/
void StandardLocationConflet::onPassivate()
{
	debuglog << "StandardLocationConflet::onPassivate";
	if ( timer->IsRunning() ) timer->Stop();
}

/*****************************************************
**
**   StandardLocationConflet   ---   OnTimer
**
******************************************************/
void StandardLocationConflet::OnTimer( wxTimerEvent& )
{
	updateUi();
}

