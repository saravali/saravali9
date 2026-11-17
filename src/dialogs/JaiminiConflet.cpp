/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/JaiminiConflet.cpp
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

#include "JaiminiConflet.h"
#include "logger.h"

#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

extern Astroconfig *astroconfig;

// begin wxGlade: ::extracode
// end wxGlade

/*****************************************************
**
**   JaiminiConflet   ---   Constructor
**
******************************************************/
JaiminiConflet::JaiminiConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, astroconfig->jaimini )
{
	// begin wxGlade: JaiminiConflet::JaiminiConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer* sizer_jaimini = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Jaimini Astrology")), wxHORIZONTAL);
	sizer_main->Add(sizer_jaimini, 0, wxALL, 3);
	wxGridSizer* grid_jaimini = new wxGridSizer(2, 2, 0, 0);
	sizer_jaimini->Add(grid_jaimini, 1, wxEXPAND, 0);
	label_arudha = new wxStaticText(sizer_jaimini->GetStaticBox(), wxID_ANY, _("Arudhas"));
	grid_jaimini->Add(label_arudha, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_arudha_choices[] = {
		_("Parasara"),
		_("Raman"),
	};
	choice_arudha = new wxChoice(sizer_jaimini->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_arudha_choices);
	choice_arudha->SetToolTip(_("Parasara: special rules for planets in angles, Raman: no special rules"));
	choice_arudha->SetSelection(0);
	grid_jaimini->Add(choice_arudha, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	label_karaka = new wxStaticText(sizer_jaimini->GetStaticBox(), wxID_ANY, _("Chara Karakas"));
	grid_jaimini->Add(label_karaka, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_charakaraka_choices[] = {
		_("Parasara"),
		_("Raman"),
	};
	choice_charakaraka = new wxChoice(sizer_jaimini->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_charakaraka_choices);
	choice_charakaraka->SetToolTip(_("Parasara: 8 karakas, Raman: 7 karakas"));
	choice_charakaraka->SetSelection(0);
	grid_jaimini->Add(choice_charakaraka, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	choice_arudha->SetValidator( wxGenericValidator( &cfg.arudhaMode ));
	choice_charakaraka->SetValidator( wxGenericValidator( &cfg.charaKarakaMode ));
}


