/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PageConflet.h
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

#ifndef PAGECONFLET_H
#define PAGECONFLET_H

#include "Conflet.h"
#include "PrintoutConfig.h"

class HelpButton;
class PaperSizeChoice;
class wxCheckBox;
class wxChoice;
class wxSpinCtrl;
class wxStaticBoxSizer;
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
class PageConflet : public ConfletIncarnation<PageConfig>
{
public:
	// begin wxGlade: PageConflet::ids
	// end wxGlade

	PageConflet( wxWindow* parent );

	virtual void updateUi() override;

private:

protected:
	// begin wxGlade: PageConflet::attributes
	wxStaticText* label_papersize;
	wxChoice* choice_papersize;
	wxSpinCtrl* spin_margin_horizontal;
	wxSpinCtrl* spin_margin_vertical;
	wxStaticBoxSizer* sizer_fontoptions;
	wxSpinCtrl* spin_fontsize;
	wxCheckBox* check_header_showtext;
	HelpButton* button_help;
	wxTextCtrl* text_header;
	wxStaticText* label_header_orientation;
	wxChoice* choice_header_orientation;
	wxCheckBox* check_header_line;
	wxCheckBox* check_header_firstpage;
	wxCheckBox* check_footer_showtext;
	wxTextCtrl* text_footer;
	wxStaticText* label_footer_orientation;
	wxChoice* choice_footer_orientation;
	wxCheckBox* check_footer_line;
	wxCheckBox* check_footer_firstpage;
	// end wxGlade
}; // wxGlade: end class


#endif // TPAGECONFLET_H
