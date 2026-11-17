/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/BasicColorConflet.h
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

#ifndef BASICCOLORCONFLET_H
#define BASICCOLORCONFLET_H

#include "Conflet.h"
#include "gdiconfig.h"

class PenPreviewWidget;
class wxButton;
class wxColourPickerCtrl;
class wxStaticText;
class wxTextCtrl;

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class BasicColorConflet : public ConfletIncarnation<ColorConfig>
{
public:
	// begin wxGlade: BasicColorConflet::ids
	// end wxGlade

	BasicColorConflet( wxWindow* parent );

	virtual void updateUi() override;

private:

	void OnDefaultPen( wxCommandEvent& );

	MBrush previewBgBrush;

protected:
	// begin wxGlade: BasicColorConflet::attributes
	wxButton* button_default_pen;
	PenPreviewWidget* pen_preview;
	wxStaticText* label_text_fg;
	wxColourPickerCtrl* button_text_fg;
	wxStaticText* label_benefic_fg;
	wxColourPickerCtrl* button_benefic_fg;
	wxStaticText* label_malefic_fg;
	wxColourPickerCtrl* button_malefic_fg;
	wxStaticText* label_transit_fg;
	wxColourPickerCtrl* button_transit_fg;
	wxStaticText* label_bg;
	wxColourPickerCtrl* button_bg;
	wxStaticText* label_marked_text_bg;
	wxColourPickerCtrl* button_marked_text_bg;
	wxStaticText* label_chart_markup_bg;
	wxColourPickerCtrl* button_chart_markup_bg;
	wxStaticText* label_error_bg;
	wxColourPickerCtrl* button_error_bg;
	// end wxGlade
}; // wxGlade: end class


#endif // BASICCOLORCONFLET_H
