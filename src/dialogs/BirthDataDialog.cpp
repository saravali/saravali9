/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/BirthDataDialog.cpp
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

#include "BirthDataDialog.h"

#include "astroconfig.h"
#include "AtlasDialog.h"
#include "Calculator.h"
#include "ChildWindow.h"
#include "CommonControls.h"
#include "constants.h"
#include "DateFormatter.h"
#include "guibase.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "SpinCtrl.h"
#include "TextValidator.h"
#include "WindowDirectory.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <math.h>

extern Astroconfig *astroconfig;

enum { DD_NOW = wxID_HIGHEST + 300, DD_DEFLOC, DD_LOCNAME, DD_SEARCHLOC, DD_LOCALTIME, DD_CALCDST,
	DD_NAME, DD_DATE, DD_TIME, DD_LONGITUDE, DD_LATITUDE, DD_TIMEZONE, DD_DST, DD_REMARK };

// begin wxGlade: ::extracode
// end wxGlade

constexpr int BIRTHDATA_DIALOG_STYLE = wxRESIZE_BORDER|wxSYSTEM_MENU|wxMAXIMIZE_BOX|wxCLOSE_BOX|wxCAPTION;

#define DEBUG_BIRTHDATADIALOG

/*****************************************************
**
**   BirthDataDialog   ---   Constructor
**
******************************************************/
BirthDataDialog::BirthDataDialog( wxWindow* parent, Horoscope_ptr h )
	: wxDialog( parent, -1, "", wxDefaultPosition, wxDefaultSize, BIRTHDATA_DIALOG_STYLE ),
	horoscope( h ),
	configkey( "BirthDataDialog" )
{
	dl = horoscope->dl;

	SetMinSize( wxSize( 400, 500 ));

    // begin wxGlade: BirthDataDialog::BirthDataDialog
    SetTitle(_("Birth Data"));
    wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_name = new wxBoxSizer(wxHORIZONTAL);
    sizer_main->Add(sizer_name, 0, wxEXPAND, 0);
    label_name = new wxStaticText(this, wxID_ANY, _("Name"));
    sizer_name->Add(label_name, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    text_name = new wxTextCtrl(this, DD_NAME, wxEmptyString);
    sizer_name->Add(text_name, 1, wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_date = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Date")), wxVERTICAL);
    sizer_main->Add(sizer_date, 0, wxEXPAND, 0);
    wxFlexGridSizer* grid_date = new wxFlexGridSizer(2, 3, 0, 0);
    sizer_date->Add(grid_date, 0, wxEXPAND, 0);
    label_date = new wxStaticText(this, wxID_ANY, _("Date"));
    grid_date->Add(label_date, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    spin_date = new DateSpin(this, DD_DATE, &spin_date_value);
    grid_date->Add(spin_date, 1, wxALL|wxEXPAND, 3);
    button_now = new wxButton(this, DD_NOW, _("Set Current Date"));
    button_now->SetToolTip(_("Set current date"));
    grid_date->Add(button_now, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
    label_time = new wxStaticText(this, wxID_ANY, _("Local Time"));
    grid_date->Add(label_time, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    spin_time = new DegreeSpin(this, DD_TIME, &spin_time_value, 0.0, 24.0, wxDefaultPosition, wxDefaultSize);
    grid_date->Add(spin_time, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    grid_date->Add(20, 20, 0, 0, 0);
    
    panel_date_display = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    sizer_date->Add(panel_date_display, 0, wxALL|wxEXPAND, 3);
    sizer_date_formatted = new wxGridSizer(2, 2, 0, 0);
    label_localdate = new wxStaticText(panel_date_display, wxID_ANY, _("localdate"));
    sizer_date_formatted->Add(label_localdate, 0, wxALL, 3);
    label_weekday = new wxStaticText(panel_date_display, wxID_ANY, _("Weekday"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    sizer_date_formatted->Add(label_weekday, 0, wxALL|wxEXPAND, 3);
    label_utdate = new wxStaticText(panel_date_display, wxID_ANY, _("utdate"));
    sizer_date_formatted->Add(label_utdate, 1, wxALL|wxEXPAND, 3);
    label_calendar = new wxStaticText(panel_date_display, wxID_ANY, _("calendar"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    sizer_date_formatted->Add(label_calendar, 1, wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_location = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Location")), wxVERTICAL);
    sizer_main->Add(sizer_location, 0, wxEXPAND, 0);
    wxFlexGridSizer* grid_location = new wxFlexGridSizer(5, 3, 0, 0);
    sizer_location->Add(grid_location, 1, wxEXPAND, 0);
    label_locname = new wxStaticText(this, wxID_ANY, _("Location"));
    grid_location->Add(label_locname, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    text_locname = new wxTextCtrl(this, DD_LOCNAME, wxEmptyString);
    grid_location->Add(text_locname, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    button_defloc = new wxButton(this, DD_DEFLOC, _("Use Standard Location"));
    button_defloc->SetToolTip(_("Set standard location"));
    grid_location->Add(button_defloc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_long = new wxStaticText(this, wxID_ANY, _("Longitude"));
    grid_location->Add(label_long, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    degree_longitude = new CombinedDegreeSpin(this, DD_LONGITUDE, &dl.location.longitude, DegreeType::Longitude);
    grid_location->Add(degree_longitude, 1, wxALL|wxEXPAND, 3);
    button_searchloc = new wxButton(this, DD_SEARCHLOC, _("Search Location ..."));
    button_searchloc->SetToolTip(_("Search location in database"));
    grid_location->Add(button_searchloc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_lat = new wxStaticText(this, wxID_ANY, _("Latitude"));
    grid_location->Add(label_lat, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    degree_latitude = new CombinedDegreeSpin(this, DD_LATITUDE, &dl.location.latitude, DegreeType::Latitude);
    grid_location->Add(degree_latitude, 1, wxALL|wxEXPAND, 3);
    grid_location->Add(20, 20, 0, 0, 0);
    label_tz = new wxStaticText(this, wxID_ANY, _("Time Zone"));
    grid_location->Add(label_tz, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    degree_timezone = new CombinedDegreeSpin(this, DD_TIMEZONE, &dl.location.tz, DegreeType::Timezone);
    grid_location->Add(degree_timezone, 1, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_localtime = new wxBoxSizer(wxHORIZONTAL);
    grid_location->Add(sizer_localtime, 1, wxEXPAND, 0);
    check_localtime = new wxCheckBox(this, DD_LOCALTIME, _("Set Local Time"));
    sizer_localtime->Add(check_localtime, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    button_help_localtime = new HelpButton(this, "Local Time");
    sizer_localtime->Add(button_help_localtime, 0, wxALL|wxEXPAND, 3);
    label_dst = new wxStaticText(this, wxID_ANY, _("DST"));
    grid_location->Add(label_dst, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    spin_dst = new DegreeSpin(this, DD_DST, &dl.location.dst, 0.0, 13.0, wxDefaultPosition, wxDefaultSize);
    spin_dst->SetMinSize(wxSize(8, 8));
    grid_location->Add(spin_dst, 1, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_dst = new wxBoxSizer(wxHORIZONTAL);
    grid_location->Add(sizer_dst, 1, wxALIGN_CENTER_VERTICAL, 0);
    button_calcdst = new wxButton(this, DD_CALCDST, _("Calculate DST"));
    button_calcdst->SetToolTip(_("Calculate DST for local time zone"));
    sizer_dst->Add(button_calcdst, 1, wxALL|wxEXPAND, 3);
    button_help_dst = new HelpButton(this, "Daylight Saving");
    sizer_dst->Add(button_help_dst, 0, wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_remark = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Remark")), wxHORIZONTAL);
    sizer_main->Add(sizer_remark, 1, wxALL|wxEXPAND, 3);
    wxFlexGridSizer* grid_sizer_remark = new wxFlexGridSizer(1, 1, 0, 0);
    sizer_remark->Add(grid_sizer_remark, 1, wxEXPAND, 0);
    text_remark = new wxTextCtrl(this, DD_REMARK, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    text_remark->SetMinSize(wxSize(300,80));
    grid_sizer_remark->Add(text_remark, 0, wxALL|wxEXPAND, 3);
    wxFlexGridSizer* sizer_buttons = new wxFlexGridSizer(1, 9, 0, 0);
    sizer_main->Add(sizer_buttons, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
    button_ok = new wxButton(this, wxID_OK, _("OK"));
    button_ok->SetToolTip(_("Apply data and close dialog"));
    button_ok->SetDefault();
    sizer_buttons->Add(button_ok, 0, wxALL, 5);
    button_apply = new wxButton(this, wxID_APPLY, _("Apply"));
    button_apply->SetToolTip(_("Apply changes and recalculate chart with new data"));
    sizer_buttons->Add(button_apply, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    button_cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
    button_cancel->SetToolTip(_("Cancel changes and close dialog"));
    sizer_buttons->Add(button_cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    sizer_buttons->AddGrowableCol(0);
    sizer_buttons->AddGrowableCol(1);
    sizer_buttons->AddGrowableCol(2);
    sizer_buttons->AddGrowableCol(3);
    grid_sizer_remark->AddGrowableRow(0);
    grid_sizer_remark->AddGrowableCol(0);
    grid_location->AddGrowableCol(1);
    panel_date_display->SetSizer(sizer_date_formatted);
    grid_date->AddGrowableCol(1);
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    Layout();
    // end wxGlade

	text_name->SetValidator( StringValidator( &dl.name ));
	text_locname->SetValidator( StringValidator( &dl.location.locName ));
	
	setSavedWindowSize( this, configkey );
	setSavedWindowPosition( this, configkey );

	button_help_localtime->contents = "Local time appears sometimes in old birth data records.\n"
		"In local time the Sun is in the Meridian at noon.\n"
		"Local time depends on the longitude of the birth place.";

	button_help_dst->contents = "DST calculation is based on the operating system's capabilities.\n"
		"It may be wrong for natives in other locations than the configured local time zone.";

	TransferDataToWindow();
	button_apply->Enable( false );
	updateUi();

#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog size of tz field " << degree_timezone->GetSize();
	debuglog << "BirthDataDialog size of dst field " << spin_dst->GetSize();
#endif

	Connect( DD_NOW, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BirthDataDialog::OnSetCurrentDate ));
	Connect( DD_DEFLOC, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BirthDataDialog::OnStandardLocation ));
	Connect( DD_SEARCHLOC, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BirthDataDialog::OnSearchLocation ));

	Connect( wxID_APPLY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BirthDataDialog::OnApply ));
	Connect( wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BirthDataDialog::OnOK ));
	Connect( wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BirthDataDialog::OnCancel ));

	Connect( DD_LOCALTIME, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BirthDataDialog::OnLocalTime ));
	Connect( DD_CALCDST, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BirthDataDialog::OnCalcDst ));

	Connect( DD_NAME, wxEVT_TEXT, wxTextEventHandler( BirthDataDialog::OnCommand ));
	Connect( DD_LOCNAME, wxEVT_TEXT, wxTextEventHandler( BirthDataDialog::OnCommand ));
	Connect( DD_REMARK, wxEVT_TEXT, wxTextEventHandler( BirthDataDialog::OnCommand ));

	Connect( DD_TIME, COMMAND_SPIN_WRAP, wxCommandEventHandler( BirthDataDialog::OnTimeWrap ));
	Connect( COMMAND_DIALOGCTRL_CHANGED, wxCommandEventHandler( BirthDataDialog::OnCommand ));
	Connect( COMMAND_SPIN_WRAP, wxCommandEventHandler( BirthDataDialog::OnTimeWrap ));

	Connect( DD_LONGITUDE, COMMAND_DIALOGCTRL_CHANGED, wxCommandEventHandler( BirthDataDialog::OnLongitude ));
	Connect( DD_LATITUDE, COMMAND_DIALOGCTRL_CHANGED, wxCommandEventHandler( BirthDataDialog::OnCommand ));
	Connect( DD_TIMEZONE, COMMAND_DIALOGCTRL_CHANGED, wxCommandEventHandler( BirthDataDialog::OnCommand ));
	Connect( DD_DST, COMMAND_DIALOGCTRL_CHANGED, wxCommandEventHandler( BirthDataDialog::OnCommand ));
}

/*****************************************************
**
**   BirthDataDialog   ---   Destructor
**
******************************************************/
BirthDataDialog::~BirthDataDialog()
{
	saveWindowSize( this, configkey );
	saveWindowPosition( this, configkey );
}

/*****************************************************
**
**   BirthDataDialog   ---   TransferDataFromWindow
**
******************************************************/
bool BirthDataDialog::TransferDataFromWindow()
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::TransferDataFromWindow oldjd " << dl.date.jd;;
#endif
	const bool b =  wxWindow::TransferDataFromWindow();

	dl.date.jd = spin_date_value + ( spin_time_value - dl.location.tz - dl.location.dst ) / 24.0;

	dl.remark = text_remark->GetValue();
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::TransferDataFromWindow NEW jd " << dl.date.jd;;
#endif
	return b;
}

/*****************************************************
**
**   BirthDataDialog   ---   TransferDataToWindow
**
******************************************************/
bool BirthDataDialog::TransferDataToWindow()
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::TransferDataToWindow";
#endif
	const double tmpjd = dl.date.jd + ( dl.location.tz + dl.location.dst ) / 24.0;

	spin_date_value =  floor( tmpjd + .5 ) - .5;
	spin_time_value = a_red(( tmpjd - floor( tmpjd ) + .5 ) * 24, 24 );

	text_remark->ChangeValue( dl.remark );
	return wxWindow::TransferDataToWindow();
}

/*****************************************************
**
**   BirthDataDialog   ---   doApply
**
******************************************************/
void BirthDataDialog::doApply()
{
	TransferDataFromWindow();
	horoscope->dl.name = dl.name;
	horoscope->setDate( dl.date.jd );
	horoscope->dl.location = dl.location;
	horoscope->dl.remark = dl.remark;

	horoscope->update();
	for( auto *child : WindowDirectory::get()->getWindowsForHoroscope( horoscope ))
  {
    assert( child );
    child->OnDataChanged();
  }
	horoscope->dl.dirty = true;
	WindowDirectory::get()->horoscopeDataChanged( horoscope );
}

/*****************************************************
**
**   BirthDataDialog   ---   setDirty
**
******************************************************/
void BirthDataDialog::setDirty( const bool b )
{
	dl.dirty = b;
	button_apply->Enable( b );
}

/*****************************************************
**
**   BirthDataDialog   ---   updateUi
**
******************************************************/
void BirthDataDialog::updateUi()
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::updateUi jd " << dl.date.jd << " tz " << dl.location.tz
		<< " dst " << dl.location.dst << " valid " << Validate();
#endif

	//TransferDataFromWindow();
	DateFormatter *formatter = DateFormatter::get();
	Calculator *calculator = CalculatorFactory().getCalculator();

	{
		wxString s;
		s << tr( "Local Date" ) << ": " << formatter->formatDatetimeOffset( dl.date.jd, dl.location.tz + dl.location.dst, DateFormat::ISO );
		label_localdate->SetLabel( s );
	}

	label_calendar->SetLabel( calculator->isGregorianCalendarDate( dl.date.jd )
		? _( "Gregorian Calendar" ) : _( "Julian Calendar" ));

	{
		wxString s;
		s << tr( "UT Date" ) << ": " << formatter->formatUTDatetime( dl.date.jd, DateFormat::ISO );
		label_utdate->SetLabel( s );
	}
	{
		wxString s;
		s << tr( "Weekday" ) << ": " << Lang().getWeekdayName( dl.getWeekDay() );
		label_weekday->SetLabel( s );
	}
	sizer_date_formatted->Layout();

  degree_timezone->Enable( ! isLocalTime );
	spin_dst->Enable( ! isLocalTime );
	button_calcdst->Enable( ! isLocalTime );
}

/*****************************************************
**
**   BirthDataDialog   ---   OnCommand
**
******************************************************/
void BirthDataDialog::OnCommand( wxCommandEvent& )
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::OnCommand";
#endif
	TransferDataFromWindow();
	setDirty( true );
	updateUi();
}

/*****************************************************
**
**   BirthDataDialog   ---   OnTimeWrap
**
******************************************************/
void BirthDataDialog::OnTimeWrap( wxCommandEvent &event )
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::OnTimeWrap int " << event.GetInt();
#endif
	TransferDataFromWindow();
	if ( event.GetInt() > 0 )
	{
		dl.date.jd++;
	}
	else
	{
		dl.date.jd--;
	}
	TransferDataToWindow();
	updateUi();
}

/*****************************************************
**
**   BirthDataDialog   ---   OnLongitude
**
******************************************************/
void BirthDataDialog::OnLongitude( wxCommandEvent& )
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::OnLongitude";
#endif

	if ( isLocalTime )
	{
		updateLocalTime();
	}

	TransferDataFromWindow();
	setDirty( true );
	updateUi();
}

/*****************************************************
**
**   BirthDataDialog   ---   OnSetCurrentDate
**
******************************************************/
void BirthDataDialog::OnSetCurrentDate( wxCommandEvent& )
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::OnSetCurrentDate";
#endif
	dl.date.jd = MDate::getCurrentJD();
	dl.location.dst = DateFormatter::get()->getSystemDstOffset( dl.date.jd );
	
	const double tmpjd = dl.date.jd + ( dl.location.tz + dl.location.dst ) / 24.0;

	spin_date_value = floor( tmpjd + .5 ) - .5;
	spin_time_value = a_red(( tmpjd - floor( tmpjd ) + .5 ) * 24, 24 );

	TransferDataToWindow();
	updateUi();
	setDirty( true );
}

/*****************************************************
**
**   BirthDataDialog   ---   OnStandardLocation
**
******************************************************/
void BirthDataDialog::OnStandardLocation( wxCommandEvent& )
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::OnStandardLocation";
#endif
	
	dl.location.locName = astroconfig->standardLocation.locName;
	dl.location.longitude = astroconfig->standardLocation.longitude;
	dl.location.latitude = astroconfig->standardLocation.latitude;
	dl.location.tz = astroconfig->standardLocation.tz;

	TransferDataToWindow();
	updateUi();
	setDirty( true );
}

/*****************************************************
**
**   BirthDataDialog   ---   OnSearchLocation
**
******************************************************/
void BirthDataDialog::OnSearchLocation( wxCommandEvent& )
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::OnSearchLocation";
#endif
	TransferDataFromWindow();
	AtlasDialogWrapper dialog( this, ATLASDIALOG_CALLER_DEFAULTLOCATIONPANEL, text_locname->GetValue() );
	if ( dialog.run() == wxID_OK )
	{
		AtlasEntry *e = dialog.getSelectedEntry();
		dl.location.locName = e->name;
		dl.location.longitude = e->longitude;
		dl.location.latitude = e->latitude;
		dl.location.tz = e->tzoffset;
#ifdef DEBUG_BIRTHDATADIALOG
		debuglog << "BirthDataDialog::OnSearchLocation tz " << dl.location.tz;
#endif

		TransferDataToWindow();
		updateUi();
		setDirty( true );
	}
}

/*****************************************************
**
**   BirthDataDialog   ---   OnApply
**
******************************************************/
void BirthDataDialog::OnApply( wxCommandEvent& )
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::OnApply";
#endif
	doApply();
	button_apply->Enable( false );

	setDirty( false );
}

/*****************************************************
**
**   BirthDataDialog   ---   OnOK
**
******************************************************/
void BirthDataDialog::OnOK( wxCommandEvent& )
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::OnOK";
#endif
	doApply();
	EndModal( wxID_OK );
}

/*****************************************************
**
**   BirthDataDialog   ---   OnCancel
**
******************************************************/
void BirthDataDialog::OnCancel( wxCommandEvent& )
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::OnCancel";
#endif
	EndModal( wxID_CANCEL );
}

/*****************************************************
**
**   BirthDataDialog   ---   OnLocalTime
**
******************************************************/
void BirthDataDialog::OnLocalTime( wxCommandEvent &event )
{
	isLocalTime = event.IsChecked();
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::OnLocalTime checked " << std::boolalpha << isLocalTime;
#endif

	if ( isLocalTime )
	{
		updateLocalTime();
	}
	updateUi();
	setDirty( true );
}

/*****************************************************
**
**   BirthDataDialog   ---   updateLocalTime
**
******************************************************/
void BirthDataDialog::updateLocalTime()
{
	dl.location.tz = dl.location.longitude / 15.0;
	dl.location.dst = 0;

#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::updateLocalTime tz " << dl.location.tz;
#endif
	degree_timezone->setValue( dl.location.tz );
	TransferDataFromWindow();
}

/*****************************************************
**
**   BirthDataDialog   ---   OnCalcDst
**
******************************************************/
void BirthDataDialog::OnCalcDst( wxCommandEvent& )
{
#ifdef DEBUG_BIRTHDATADIALOG
	debuglog << "BirthDataDialog::OnCalcDst";
#endif
	doNotShowAgainMessage( this, DoNotShowAgainTopic::DaylightSavingTime,
		tr( "DST calculation is based on a builtin operating system feature and may not be correct for the location you entered.") );

	dl.location.dst = DateFormatter::get()->getSystemDstOffset( dl.date.jd );

	TransferDataToWindow();
	updateUi();
	setDirty( true );
}



