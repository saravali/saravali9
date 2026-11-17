/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ChartColorConflet.h
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

#ifndef CHARTCOLORCONFLET_H
#define CHARTCOLORCONFLET_H

#include "Conflet.h"
#include "gdiconfig.h"

class wxButton;
class wxChoice;
class wxColourPickerCtrl;
class wxColourPickerEvent;
class wxStaticText;
class wxTextCtrl;

class PlanetPreviewPanel;
class SignPreviewPanel;
class HousePreviewPanel;

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class ChartColorConflet : public ConfletIncarnation<ChartColorConfig>
{
public:
	// begin wxGlade: ChartColorConflet::ids
	// end wxGlade

	ChartColorConflet( wxWindow* parent );

	virtual void updateUi() override;

private:

	void OnChoice( wxCommandEvent &event );

	void OnObjectColorChanged( wxColourPickerEvent& );
	void OnObjectChoice( wxCommandEvent& );
	void OnButtonSignBrush( wxCommandEvent& );
	void OnButtonHouseBrush( wxCommandEvent& );

	int planetChoiceToIndex( const int &selection );


protected:
	// begin wxGlade: ChartColorConflet::attributes
	wxChoice* choice_planets;
	wxStaticText* label_fg_planets;
	wxColourPickerCtrl* color_fgplanets;
	wxStaticText* label_preview_planets;
	PlanetPreviewPanel* preview_planetcolor;
	wxChoice* choice_signs;
	wxStaticText* label_fg_signs;
	wxColourPickerCtrl* color_fgsigns;
	wxStaticText* label_bg_sign;
	wxButton* button_signbrush;
	wxStaticText* label_preview_sign;
	SignPreviewPanel* preview_sign;
	wxChoice* choice_houses;
	wxStaticText* label_fg_houses;
	wxColourPickerCtrl* color_fghouses;
	wxStaticText* label_bg_houses;
	wxButton* button_housebrush;
	wxStaticText* label_preview_houses;
	HousePreviewPanel* preview_house;
	// end wxGlade
}; // wxGlade: end class


#endif // CHARTCOLORCONFLET_H
