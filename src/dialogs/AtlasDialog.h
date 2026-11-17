/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AtlasDialog.h
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

#ifndef ATLASDIALOG_H
#define ATLASDIALOG_H

// begin wxGlade: ::dependencies
#include <wx/statline.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

class AtlasDialog;
class AtlasGridTable;
class AtlasLogic;
class wxButton;
class wxChoice;
class wxGridEvent;
class wxStaticBox;
class wxStaticText;
class wxTextCtrl;
class wxTimer;
class wxGrid;
class MultiLineStaticText;

#include <wx/dialog.h>

#include "AtlasDao.h"
#include "bootstrap.h"

enum { ATLASDIALOG_CALLER_APPLICATIONWINDOW, ATLASDIALOG_CALLER_DATADIALOG, ATLASDIALOG_CALLER_DEFAULTLOCATIONPANEL };

/*************************************************//**
*
*  
*
******************************************************/
class AtlasDialogWrapper
{
public:
	AtlasDialogWrapper( wxWindow *parent, const int &callerId, wxString searchString = wxEmptyString );
	~AtlasDialogWrapper();

	int run();

	AtlasEntry *getSelectedEntry();

private:
	const int callerId;
	wxString searchString;
	wxWindow *parent;
	AtlasDialog *dialog;

};

/*************************************************//**
*
*  GUI access to atlas database
*
******************************************************/
class AtlasDialog: public wxDialog
{
public:
    // begin wxGlade: AtlasDialog::ids
    // end wxGlade

	AtlasDialog(wxWindow* parent, const int &callerId, wxString searchString = wxEmptyString );
	~AtlasDialog();

	AtlasEntry &getSelectedEntry() { return selectedEntry; }

private:

	void OnFilterChanged( wxCommandEvent& );
	void OnCellChange( wxGridEvent& );
	void OnCellDClick( wxGridEvent& );
	void OnSelectLabel( wxGridEvent& );

	void OnIdle( wxIdleEvent& );
	void OnAddEntry( wxCommandEvent& );
	void OnEditEntry( wxCommandEvent& );
	void OnDeleteEntry( wxCommandEvent& );
	void OnKeyDown( wxKeyEvent& );
	void OnTimer( class wxTimerEvent& );

	void editEntry();
	void updateFilterMatchLabel( const int& );
	void updateEntryDetails();
	wxString getSelectedCountryISO();
	void updateCountryChoice();
	void checkNews();

	const int callerId;
	const wxString configkey;

	int selectedRow;
	AtlasEntry selectedEntry;

	AtlasLogic *logic;
	AtlasGridTable *gridTable;
	wxTimer *timer;

protected:
    // begin wxGlade: AtlasDialog::attributes
    wxStaticText* label_filtername;
    wxTextCtrl* filter_edit;
    wxStaticText* label_filtercountry;
    wxChoice* country_choice;
    wxStaticText* label_mode;
    wxChoice* choice_mode;
    wxGrid* locgrid;
    wxStaticText* match_label;
    wxStaticLine* static_line_match;
    wxStaticText* label_name;
    wxStaticText* text_name;
    wxStaticText* label_longitude;
    wxStaticText* text_longitude;
    wxStaticText* label_asciiname;
    wxStaticText* text_asciiname;
    wxStaticText* label_latitude;
    wxStaticText* text_latitude;
    wxStaticText* label_country;
    wxStaticText* text_country;
    wxStaticText* label_timezone;
    wxStaticText* text_timezone;
    wxStaticText* label_admincode;
    wxStaticText* text_admincode;
    wxStaticText* label_tzoffset;
    wxStaticText* text_tzoffset;
    wxStaticText* label_population;
    wxStaticText* text_population;
    wxStaticText* label_id;
    wxStaticText* text_id;
    wxStaticText* label_alias;
    wxTextCtrl* text_aliases;
    wxButton* button_add;
    wxButton* button_edit;
    wxButton* button_delete;
    wxStaticLine* static_line_button;
    wxButton* button_ok;
    wxButton* button_cancel;
    // end wxGlade
}; // wxGlade: end class


#endif // ATLASDIALOG_H
