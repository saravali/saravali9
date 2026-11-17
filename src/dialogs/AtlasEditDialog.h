/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AtlasEditDialog.h
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

#ifndef ATLASEDITDIALOG_H
#define ATLASEDITDIALOG_H

// begin wxGlade: ::dependencies
#include <wx/spinctrl.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

#include <wx/dialog.h>
#include <list>

#include <AtlasLogic.h>

class wxButton;
class wxChoice;
class wxPanel;
class wxSpniCtrl;
class wxStaticBox;
class wxStaticText;
class wxTextCtrl;

class CombinedDegreeSpin;
class MultiLineStaticText;

/*************************************************//**
*
* 
*
******************************************************/
class AtlasEditDialog: public wxDialog
{
public:
    // begin wxGlade: AtlasEditDialog::ids
    // end wxGlade

	AtlasEditDialog(wxWindow* parent, AtlasLogic *logic, const AtlasEntry entry );

	~AtlasEditDialog();

	AtlasEntry getEntry() const { return model; }

private:

	AtlasLogic *logic;
	AtlasEntry model;

	std::list<TimezoneEntry> tzlist;
	std::list<AtlasCountry> countrylist;
	const wxString configkey;

	void OnOK( wxCommandEvent& );
	void OnChoiceCountry( wxCommandEvent& );
	void OnChoiceTzName( wxCommandEvent& );
	void OnEntryAliases( wxCommandEvent& );
	void OnTextChanged( wxCommandEvent& );

	void updateTzHours();
	void updateAdminChoice();
	void updateTzChoice();

protected:
    // begin wxGlade: AtlasEditDialog::attributes
    wxStaticText* label_name;
    wxTextCtrl* text_name;
    wxStaticText* label_asciiname;
    wxTextCtrl* text_asciiname;
    wxStaticText* label_country;
    wxChoice* choice_country;
    wxStaticText* label_admincode;
    wxChoice* choice_admin;
    wxStaticText* label_population;
    wxSpinCtrl* spin_population;
    wxStaticText* label_longitude;
    CombinedDegreeSpin* degree_longitude;
    wxStaticText* label_latitude;
    CombinedDegreeSpin* degree_latitude;
    wxStaticText* label_timezone;
    wxChoice* choice_timezone;
    wxStaticText* label_tzh;
    wxStaticText* label_tzhours;
    wxStaticText* label_aliases;
    wxButton* button_aliases;
    wxTextCtrl* text_alias;
    wxButton* button_ok;
    wxButton* button_cancel_copy;
    // end wxGlade
}; // wxGlade: end class


#endif // ATLASEDITDIALOG_H
