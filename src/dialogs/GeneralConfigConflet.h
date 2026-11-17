/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/GeneralConfigConflet.h
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

#ifndef GENERALCONFIGCONFLET_H
#define GENERALCONFIGCONFLET_H

#include "Conflet.h"
#include "astroconfig.h"

class wxCheckBox;
class wxChoice;
class wxStaticText;

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
class GeneralConfigConflet : public ConfletIncarnation<GeneralConfig>
{
public:
	// begin wxGlade: GeneralConfigConflet::ids
	// end wxGlade

	GeneralConfigConflet( wxWindow* parent );

	virtual void updateUi() override;

	virtual void restoreDefaults() override;

private:

	void setLanguageChoiceSelection();

	void OnLanguageChoice( wxCommandEvent& );

	// Consist of 2 letter code and displayable text
	std::vector<std::pair<wxString,wxString>> languageChoices;


protected:
	// begin wxGlade: GeneralConfigConflet::attributes
	wxChoice* choice_default_viewpoint;
	wxChoice* choice_viewpoint_policy;
	wxChoice* choice_lang;
	wxChoice* choice_dateformat;
	wxStaticText* label_datepreview;
	wxCheckBox* checkbox_logmessages;
	wxCheckBox* checkbox_ascpos;
	wxCheckBox* checkbox_moonpos;
	wxSpinCtrl* spin_statusmessage_timeout;
	wxCheckBox* check_opennewdoc;
	wxCheckBox* check_askonquit;
	// end wxGlade
}; // wxGlade: end class


#endif // GENERALCONFIGCONFLET_H
