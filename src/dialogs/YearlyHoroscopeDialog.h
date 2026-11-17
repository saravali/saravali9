/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/YearlyHoroscopeDialog.h
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

#ifndef YEARLYHOROSCOPEDIALOG_H
#define YEARLYHOROSCOPEDIALOG_H

#include "datelocation.h"

#include <wx/dialog.h>

class CombinedDegreeSpin;

class wxButton;
class wxChoice;
class wxPanel;
class wxSpinCtrl;
class wxStaticLine;
class wxStaticText;
class wxSpinCtrl;
class wxTextCtrl;

/*************************************************//**
*
* 
*
******************************************************/
class YearlyHoroscopeDialog: public wxDialog
{
public:
    // begin wxGlade: YearlyHoroscopeDialog::ids
    // end wxGlade

	YearlyHoroscopeDialog( wxWindow* parent );

	~YearlyHoroscopeDialog();

private:

	void OnSearchLocation( wxCommandEvent& );

	void OnOK( wxCommandEvent& );

	void OnYearSpin( wxCommandEvent& );

	void updatePreviewLabel();

	Horoscope_ptr createYearlyHoroscope();

	Location location;

	const wxString configkey;

protected:
    // begin wxGlade: YearlyHoroscopeDialog::attributes
    wxSpinCtrl* spin_year;
    wxPanel* panel_preview;
    wxStaticText* label_date;
    wxTextCtrl* text_name;
    wxButton* button_search;
    wxStaticText* label_longitude;
    CombinedDegreeSpin* degree_longitude;
    wxStaticText* label_latitude;
    CombinedDegreeSpin* degree_latitude;
    wxStaticText* label_timezone;
    CombinedDegreeSpin* degree_timezone;
    wxStaticLine* static_line;
    wxButton* button_ok;
    wxButton* button_cancel;
    // end wxGlade
}; // wxGlade: end class


#endif // YEARLYHOROSCOPEDIALOG_H
