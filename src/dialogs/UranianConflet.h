/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/UranianConflet.h
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

#ifndef URANIANCONFLET_H_
#define URANIANCONFLET_H_

#include "Conflet.h"
#include "astroconfig.h"

class AntisciaPreviewPanel;
class ContextProperties;
class MatchFormatPreviewPanel;
class TextWidget;

class wxCheckListBox;
class wxChoice;
class wxSpinCtrl;

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
class UranianConflet : public ConfletIncarnation<UranianConfig>
{
public:
	// begin wxGlade: UranianConflet::ids
	// end wxGlade

	UranianConflet( wxWindow* );

	virtual void updateUi() override;

private:

	ContextProps_ptr props;

	void OnCommand( wxCommandEvent& );

protected:
	// begin wxGlade: UranianConflet::attributes
	wxChoice* choice_antiscia;
	AntisciaPreviewPanel* antisciaPreview;
	wxChoice* choice_match_format;
	MatchFormatPreviewPanel* matchFormatPreview;
	wxSpinCtrl* spin_maxmatches;
	wxCheckListBox* check_primes;
	wxSpinCtrl* spin_maximum;
	TextWidget* preview;
	// end wxGlade
}; // wxGlade: end class


#endif // URANIANCONFLET_H_
