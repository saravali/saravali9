/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/BrushDialog.h
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

#ifndef BRUSHDIALOG_H
#define BRUSHDIALOG_H

#include <wx/dialog.h>
#include "gdibase.h"
#include "constants.h"


// begin wxGlade: ::dependencies
#include <wx/statline.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

class BackgroundImageChoice;
class BrushDialogPreviewWidget;
class wxButton;
class wxChoice;
class wxColourPickerCtrl;
class wxColourPickerEvent;
class wxPanel;
class wxSlider;
class wxStaticBox;
class wxStaticLine;
class wxStaticText;

/*************************************************//**
*
* 
*
******************************************************/
class BrushDialog: public wxDialog
{
public:
    // begin wxGlade: BrushDialog::ids
    // end wxGlade

	BrushDialog( wxWindow* parent, const MBrush& );

	MBrush getBrush() const { return brush; }

private:

	void OnColorPicker( wxColourPickerEvent& );
	void OnSliderAlpha( wxCommandEvent& );
	void OnStyleChoice( wxCommandEvent& );
	void OnChoiceBgImage( wxCommandEvent& );
	void OnSliderHue( wxCommandEvent&  );

	void updateUi();
	void OnOK( wxCommandEvent& );

	MBrush brush;

protected:
    // begin wxGlade: BrushDialog::attributes
    wxStaticText* label_bgcolor;
    wxColourPickerCtrl* button_bgcolor;
    wxStaticText* label_alpha;
    wxSlider* slider_alpha;
    wxStaticText* label_bgstyle;
    wxChoice* choice_bgstyle;
    wxStaticText* label_bgimage;
    BackgroundImageChoice* choice_bgimage;
    wxStaticText* label_rotatehue;
    wxSlider* slider_rotatehue;
    wxPanel* panel_preview;
    BrushDialogPreviewWidget* preview;
    wxStaticLine* static_line;
    wxButton* button_ok;
    wxButton* button_cancel;
    // end wxGlade
}; // wxGlade: end class


#endif // BRUSHDIALOG_H
