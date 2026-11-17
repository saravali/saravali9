/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/FontConflet.h
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

#ifndef FONTCONFLET_H
#define FONTCONFLET_H

#include "Conflet.h"
#include "gdiconfig.h"

class wxButton;
class wxChoice;
class wxSpinCtrl;
class wxStaticText;
class wxTextCtrl;

// begin wxGlade: ::dependencies
#include <wx/spinctrl.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class FontConflet : public ConfletIncarnation<FontConfig>
{
public:
	// begin wxGlade: FontConflet::ids
	// end wxGlade

	FontConflet( wxWindow* parent );

	virtual void updateUi() override;

private:

	//void OnSelectFont( wxCommandEvent& );

	virtual void config2model() override;
	virtual bool TransferDataToWindow() override;

	void OnChoice( wxCommandEvent& );
	//void fillInvalidFacenames();


protected:
	// begin wxGlade: FontConflet::attributes
	wxStaticText* label_standardfont;
	wxChoice* choice_standardfont_facename;
	wxTextCtrl* text_standardfont;
	wxStaticText* label_fixedfont;
	wxChoice* choice_fixedfont_facename;
	wxTextCtrl* text_fixedfont;
	wxStaticText* label_fontsize;
	wxSpinCtrl* spin_textfont_size;
	wxStaticText* label_font_pt;
	wxStaticText* label_graphicfont_facename;
	wxChoice* choice_graphicfont_facename;
	wxTextCtrl* text_graphicfont;
	wxStaticText* label_graphicfont_size;
	wxSpinCtrl* spin_graphicfont_size;
	wxStaticText* label_graphicfont_pt;
	wxStaticText* label_symbolfont_facename;
	wxChoice* choice_symbolfont_facename;
	wxTextCtrl* text_symbolfont;
	wxStaticText* label_symbolfont_size;
	wxSpinCtrl* spin_symbolfont_size;
	wxStaticText* label_symbolfont_pt;
	// end wxGlade
}; // wxGlade: end class


#endif // FONTCONFLET_H
