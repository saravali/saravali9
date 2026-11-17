/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/BirthDataDialog.h
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

#ifndef BIRTHDATADIALOG_H
#define BIRTHDATADIALOG_H

#include "datelocation.h"

#include <wx/dialog.h>

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

class CombinedDegreeSpin;
class HelpButton;
class DateSpin;
class DegreeSpin;

class wxButton;
class wxCheckBox;
class wxEvent;
class wxGridSizer;
class wxPanel;
class wxStaticLine;
class wxStaticText;
class wxTextCtrl;

/*************************************************//**
*
*  dialog for birth data
*
******************************************************/
class BirthDataDialog: public wxDialog
{
public:
    // begin wxGlade: BirthDataDialog::ids
    // end wxGlade

	BirthDataDialog( wxWindow* parent, Horoscope_ptr );

	~BirthDataDialog();

private:

	virtual bool TransferDataToWindow() override;
	virtual bool TransferDataFromWindow() override;

	void updateUi();
	void setDirty( const bool = true );
	void doApply();
	void updateLocalTime();

	// command handlers
	void OnCommand( wxCommandEvent& );
	void OnTimeWrap( wxCommandEvent& );
	void OnLongitude( wxCommandEvent& );
	void OnSetCurrentDate( wxCommandEvent& );
	void OnStandardLocation( wxCommandEvent& );
	void OnSearchLocation( wxCommandEvent& );
	void OnApply( wxCommandEvent& );
	void OnOK( wxCommandEvent& );
	void OnCancel( wxCommandEvent& );
	void OnLocalTime( wxCommandEvent& );
	void OnCalcDst( wxCommandEvent& );

	Horoscope_ptr horoscope;

	// data object
	DateLocation dl;

	// spin data
	double spin_date_value = 0;
	double spin_time_value = 0;
	double isLocalTime = false;
	
	const wxString configkey;

protected:
    // begin wxGlade: BirthDataDialog::attributes
    wxStaticText* label_name;
    wxTextCtrl* text_name;
    wxStaticText* label_date;
    DateSpin* spin_date;
    wxButton* button_now;
    wxStaticText* label_time;
    DegreeSpin* spin_time;
    wxPanel* panel_date_display;
    wxGridSizer* sizer_date_formatted;
    wxStaticText* label_localdate;
    wxStaticText* label_weekday;
    wxStaticText* label_utdate;
    wxStaticText* label_calendar;
    wxStaticText* label_locname;
    wxTextCtrl* text_locname;
    wxButton* button_defloc;
    wxStaticText* label_long;
    CombinedDegreeSpin* degree_longitude;
    wxButton* button_searchloc;
    wxStaticText* label_lat;
    CombinedDegreeSpin* degree_latitude;
    wxStaticText* label_tz;
    CombinedDegreeSpin* degree_timezone;
    wxCheckBox* check_localtime;
    HelpButton* button_help_localtime;
    wxStaticText* label_dst;
    DegreeSpin* spin_dst;
    wxButton* button_calcdst;
    HelpButton* button_help_dst;
    wxTextCtrl* text_remark;
    wxButton* button_ok;
    wxButton* button_apply;
    wxButton* button_cancel;
    // end wxGlade
}; // wxGlade: end class


#endif // BIRTHDATADIALOG_H
