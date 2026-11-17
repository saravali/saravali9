/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PlanetConflet.h
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

#ifndef _PLANETDISPLAYCONFLET_H
#define _PLANETDISPLAYCONFLET_H

#include "Conflet.h"
#include "astroconfig.h"
#include "Viewpoint.h"

class TextWidget;

class wxButton;
class wxCheckBox;
class wxChoice;
class wxPanel;
class wxStaticText;

template<class E> class RearrangableListCtrl;
typedef RearrangableListCtrl<PlanetId> RearrangableListCtrlPlanetId;

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class PlanetConflet : public ConfletIncarnation<PlanetConfig>
{
public:
	// begin wxGlade: PlanetConflet::ids
	// end wxGlade

	PlanetConflet( wxWindow* parent, const Viewpoint& );

	virtual void updateUi() override;

private:

	ContextProps_ptr props;

	const Viewpoint viewpoint;

	//virtual void config2model() override;

	//virtual void model2config() override;

	void OnArabicParts( wxCommandEvent& );

	void OnCheckBox( wxCommandEvent& );

	virtual void restoreDefaults() override;

protected:
	// begin wxGlade: PlanetConflet::attributes
	RearrangableListCtrlPlanetId* listctrl;
	wxStaticText* label_planet_textformat;
	wxChoice* choice_factor_textformat;
	wxStaticText* label_node_names;
	wxChoice* choice_node_names;
	wxStaticText* label_uranus;
	wxChoice* choice_uranus;
	wxStaticText* preview_uranus;
	wxStaticText* label_pluto;
	wxChoice* choice_pluto;
	wxStaticText* preview_pluto;
	wxCheckBox* check_vedic_names;
	wxButton* button_arabic;
	wxStaticText* label_arabic_selection;
	wxPanel* panel_preview;
	TextWidget* preview;
	// end wxGlade
}; // wxGlade: end class


#endif // _PLANETDISPLAYCONFLET_H
