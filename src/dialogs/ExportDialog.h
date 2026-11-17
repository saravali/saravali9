/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ExportDialog.h
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

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

// begin wxGlade: dependencies
// end wxGlade

#include <wx/dialog.h>

class wxButton;
class wxCheckBox;
class wxSpinCtrl;
class wxStaticBox;
class wxStaticLine;
class wxStaticText;

/*************************************************//**
*
*    panel for graphics export (size configuration)
*
******************************************************/
class ExportDialog: public wxDialog
{
public:
    // begin wxGlade: ExportDialog::ids
    // end wxGlade

	ExportDialog(wxWindow* parent, const int &sizex, const int &sizey );
	void getSizes( int &x, int &y );
	bool getCheckShowDialog();

private:
	int sizex, sizey;

protected:
    // begin wxGlade: ExportDialog::attributes
    wxStaticText* label_x;
    wxSpinCtrl* spin_x;
    wxStaticText* label_y;
    wxSpinCtrl* spin_y;
    wxCheckBox* check_show_dialog;
    wxStaticLine* static_line_1;
    wxButton* button_ok;
    wxButton* button_cancel;
    // end wxGlade
};


#endif // EXPORTDIALOG_H
