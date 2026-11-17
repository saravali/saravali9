/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PlanetPropertyConflet.h
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

#ifndef _PLANETPROPERTYCONFLET_H
#define _PLANETPROPERTYCONFLET_H

#include "Conflet.h"
#include "astroconfig.h"
#include "Viewpoint.h"

class TextWidget;

class wxCheckBox;
class wxChoice;
class wxStaticText;

template<class E> class RearrangableListCtrl;
typedef RearrangableListCtrl<ColumnGroup> RearrangableListCtrlColumnGroup;

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class PlanetPropertyConflet : public ConfletIncarnation<PlanetPropertyConfig>
{
public:
	// begin wxGlade: PlanetPropertyConflet::ids
	// end wxGlade

	PlanetPropertyConflet( wxWindow* parent, const Viewpoint& );

	virtual void updateUi() override;

private:

	ContextProps_ptr props;

	Horoscope_ptr horoscope;

	const Viewpoint viewpoint;

	void OnCheckBox( wxCommandEvent& );

	virtual void restoreDefaults() override;

protected:
	// begin wxGlade: PlanetPropertyConflet::attributes
	RearrangableListCtrlColumnGroup* listctrl;
	wxStaticText* label_sign_textformat;
	wxChoice* choice_textformat;
	wxStaticText* label_capricorn;
	wxChoice* choice_capricorn;
	wxStaticText* preview_capricorn;
	wxCheckBox* check_translate_sign_names;
	wxCheckBox* check_vedic_sign_names;
	wxStaticText* label_position_style;
	wxChoice* choice_position_style;
	wxStaticText* label_precision;
	wxChoice* choice_precision;
	wxStaticText* label_retrogression;
	wxChoice* choice_retrogression;
	wxStaticText* label_nakshatra_textformat;
	wxChoice* choice_nakshatra_textformat;
	wxPanel* panel_preview;
	TextWidget* preview;
	// end wxGlade
}; // wxGlade: end class


#endif // _PLANETPROPERTYCONFLET_H
