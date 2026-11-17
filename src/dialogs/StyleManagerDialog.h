/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/StyleManagerDialog.h
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

#ifndef STYLEMANAGERDIALOG_H
#define STYLEMANAGERDIALOG_H

#include "ConfigListItems.h"

#include <vector>
#include <wx/dialog.h>

// begin wxGlade: ::dependencies
#include <wx/statline.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

class wxButton;
class wxListBox;
class wxStaticBox;

/*************************************************//**
*
*    CLASS    ---   StyleManagerDialog
*
******************************************************/
class StyleManagerDialog: public wxDialog
{
public:
    // begin wxGlade: StyleManagerDialog::ids
    // end wxGlade

	StyleManagerDialog(wxWindow* parent, std::vector<TableStyle>&, int &sel );

private:

	int getSelection() const;

	void populateChoice();
	void updateUi();
	void renameEntry( const int& );

	void OnListbox( wxCommandEvent& );
	void OnListboxDoubleClick( wxCommandEvent& );
	void OnNew( wxCommandEvent& );
	void OnCopy( wxCommandEvent& );
	void OnRename( wxCommandEvent& );
	void OnDelete( wxCommandEvent& );
	void OnUp( wxCommandEvent& );
	void OnDown( wxCommandEvent& );

	std::vector<TableStyle> &v;

	int &sel;

protected:
    // begin wxGlade: StyleManagerDialog::attributes
    wxListBox* list;
    wxButton* button_new;
    wxButton* button_copy;
    wxButton* button_rename;
    wxButton* button_delete;
    wxButton* button_up;
    wxButton* button_down;
    wxButton* button_ok;
    // end wxGlade
}; // wxGlade: end class


#endif // STYLEMANAGERDIALOG_H
