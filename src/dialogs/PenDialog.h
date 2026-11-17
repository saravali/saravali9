/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PenDialog.h
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

#ifndef PENDIALOG_H
#define PENDIALOG_H

#include <wx/dialog.h>
#include <wx/panel.h>

#include "gdibase.h"

// begin wxGlade: ::dependencies
#include <wx/spinctrl.h>
#include <wx/statline.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

class BackgroundImageChoice;
class wxButton;
class wxChoice;
class wxColourPickerCtrl;
class wxColourPickerEvent;
class wxCommandEvent;
class wxPanel;
class wxPen;
class wxSlider;
class wxSpinCtrl;
class wxSpinEvent;
class wxStaticBox;
class wxStaticLine;
class wxStaticText;
struct MBrush;

/*************************************************//**
*
* 
*
******************************************************/
class PenPreviewWidget: public wxPanel
{
public:

	PenPreviewWidget( wxWindow*, wxWindowID, wxPen*, const MBrush& );

private:

	void OnPaint( wxPaintEvent& );

	wxPen *pen = nullptr;

	MBrush brush;
};

/*************************************************//**
*
* 
*
******************************************************/
class PenDialog: public wxDialog
{
public:
    // begin wxGlade: PenDialog::ids
    // end wxGlade

	PenDialog( wxWindow* parent, const wxPen&, const MBrush& );

	wxPen getPen() const { return pen; }

private:

	wxPen pen;
	MBrush bgbrush;

	void updateUi();
	wxPenStyle getSelectedPenStyle() const;

	void OnColorPicker( wxColourPickerEvent& ) { updateUi(); }
	void OnSpin( wxSpinEvent& ) { updateUi(); }
	void OnChoice( wxCommandEvent& ) { updateUi(); }

protected:
    // begin wxGlade: PenDialog::attributes
    wxStaticText* label_color;
    wxColourPickerCtrl* button_color;
    wxStaticText* label_alpha;
    wxSlider* slider_alpha;
    wxStaticText* label_width;
    wxSpinCtrl* spin_width;
    wxStaticText* label_bgstyle;
    wxChoice* choice_style;
    wxPanel* panel_preview;
    PenPreviewWidget* preview;
    wxStaticLine* static_line;
    wxButton* button_ok;
    wxButton* button_cancel;
    // end wxGlade
}; // wxGlade: end class


#endif // PENDIALOG_H
