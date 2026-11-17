/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AtlasImportDialog.h
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

#ifndef ATLASIMPORTDIALOG_H
#define ATLASIMPORTDIALOG_H

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

// prevent compiler error based upon usage of wxGauge95
#if defined(__WXMSW__)
#include <wx/gauge.h>
#endif

#include <wx/dialog.h>

class wxButton;
class wxGauge;
class wxPanel;
class wxStaticText;
class wxTimer;
class wxTimerEvent;

class AtlasImporter;

/*************************************************//**
*
*
*
******************************************************/
class AtlasImportDialog : public wxDialog
{

public:
    // begin wxGlade: AtlasImportDialog::ids
    // end wxGlade

	AtlasImportDialog(wxWindow* parent, wxString title = wxEmptyString );
	~AtlasImportDialog();

	bool run();
	bool setData( wxString sqlfile, wxString dbfile );
	bool setDefaultFiles();
	wxString getErrorMessage();

private:

	void OnTimer( wxTimerEvent& );
	void OnCancel( wxCommandEvent& );
	void OnRun( wxCommandEvent& );

	bool checkFiles();

	AtlasImporter *importer;
	wxTimer *timer;
	wxString sqlfile, dbfile;
	bool close_called;

protected:
    // begin wxGlade: AtlasImportDialog::attributes
    wxPanel* panel_gauge;
    wxGauge* gauge;
    wxStaticText* text_status;
    wxButton* button_cancel;
    // end wxGlade
}; // wxGlade: end class


#endif // ATLASIMPORTDIALOG_H

