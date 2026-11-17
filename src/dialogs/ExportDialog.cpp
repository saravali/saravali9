/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ExportDialog.cpp
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

#include "ExportDialog.h"

#include "guibase.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/intl.h>
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/stattext.h>

/*****************************************************
**
**   ExportDialog   ---   Constructor
**
******************************************************/
ExportDialog::ExportDialog(wxWindow* parent, const int &x, const int &y )
		:  wxDialog(parent, -1, "--", wxDefaultPosition, wxDefaultSize, DEFAULT_DIALOG_STYLE)
{
	sizex = x;
	sizey = y;
    // begin wxGlade: ExportDialog::ExportDialog
    SetTitle(_("Export Picture"));
    wxFlexGridSizer* sizer_main = new wxFlexGridSizer(3, 1, 0, 0);
    wxStaticBoxSizer* sizer_size = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Size")), wxVERTICAL);
    sizer_main->Add(sizer_size, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
    wxFlexGridSizer* grid_size = new wxFlexGridSizer(2, 2, 3, 3);
    sizer_size->Add(grid_size, 1, wxEXPAND, 0);
    label_x = new wxStaticText(this, wxID_ANY, _("Horizontal Size"));
    grid_size->Add(label_x, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    spin_x = new wxSpinCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, 1, 100000);
    grid_size->Add(spin_x, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_y = new wxStaticText(this, wxID_ANY, _("Vertical Size"));
    grid_size->Add(label_y, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    spin_y = new wxSpinCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, 1, 100000);
    grid_size->Add(spin_y, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    sizer_size->Add(20, 20, 0, wxALL|wxEXPAND, 3);
    check_show_dialog = new wxCheckBox(this, wxID_ANY, _("Don't show this dialog again"));
    sizer_size->Add(check_show_dialog, 0, wxALL|wxEXPAND, 3);
    static_line_1 = new wxStaticLine(this, wxID_ANY);
    sizer_main->Add(static_line_1, 0, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_buttons = new wxBoxSizer(wxHORIZONTAL);
    sizer_main->Add(sizer_buttons, 1, 0, 3);
    button_ok = new wxButton(this, wxID_OK, _("OK"));
    sizer_buttons->Add(button_ok, 0, wxALL, 3);
    button_cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
    sizer_buttons->Add(button_cancel, 0, wxALL, 3);
    
    sizer_main->AddGrowableCol(0);
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    Layout();
    // end wxGlade

	spin_x->SetValue( sizex );
	spin_y->SetValue( sizey );

	check_show_dialog->SetValue( false );
	button_ok->SetFocus();
}

/*****************************************************
**
**   ExportDialog   ---   getSizes
**
******************************************************/
void ExportDialog::getSizes( int &x, int &y )
{
	x = spin_x->GetValue();
	y = spin_y->GetValue();
}

/*****************************************************
**
**   ExportDialog   ---   getCheckShowDialog
**
******************************************************/
bool ExportDialog::getCheckShowDialog()
{
	return check_show_dialog->GetValue();
}

