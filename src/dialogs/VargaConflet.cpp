/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/VargaConflet.cpp
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

#include "VargaConflet.h"
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
**   VargaConflet   ---   Constructor
**
******************************************************/
VargaConflet::VargaConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, astroconfig->varga )
{
	// begin wxGlade: VargaConflet::VargaConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer* sizer_benefics = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Dignities and Benefics")), wxVERTICAL);
	sizer_main->Add(sizer_benefics, 0, wxALL|wxEXPAND, 3);
	wxGridSizer* grid_sizer_other = new wxGridSizer(3, 2, 3, 3);
	sizer_benefics->Add(grid_sizer_other, 0, 0, 0);
	label_moon = new wxStaticText(sizer_benefics->GetStaticBox(), wxID_ANY, _("Moon is Benefic"));
	grid_sizer_other->Add(label_moon, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_moon_choices[] = {
		_("Never"),
		_("Angle < 180"),
		_("120 < Angle < 240"),
		_("Always"),
	};
	choice_moon = new wxChoice(sizer_benefics->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, choice_moon_choices);
	choice_moon->SetToolTip(_("Angle is Moon - Sun"));
	choice_moon->SetSelection(0);
	grid_sizer_other->Add(choice_moon, 0, wxALIGN_RIGHT|wxALL, 3);
	label_mercury = new wxStaticText(sizer_benefics->GetStaticBox(), wxID_ANY, _("Mercury is Benefic"));
	grid_sizer_other->Add(label_mercury, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_mercury_choices[] = {
		_("Never"),
		_("No Malefic Conjunction"),
		_("Always"),
	};
	choice_mercury = new wxChoice(sizer_benefics->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_mercury_choices);
	choice_mercury->SetSelection(0);
	grid_sizer_other->Add(choice_mercury, 0, wxALIGN_RIGHT|wxALL, 3);
	label_tempfriend = new wxStaticText(sizer_benefics->GetStaticBox(), wxID_ANY, _("Temporary Friends"));
	grid_sizer_other->Add(label_tempfriend, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_tempfriend_choices[] = {
		_("Always from Rasi Chart"),
		_("From Varga Chart"),
	};
	choice_tempfriend = new wxChoice(sizer_benefics->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_tempfriend_choices);
	choice_tempfriend->SetSelection(0);
	grid_sizer_other->Add(choice_tempfriend, 0, wxALIGN_RIGHT|wxALL, 3);
	wxStaticBoxSizer* sizer_varga = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Varga Calculation Rules")), wxHORIZONTAL);
	sizer_main->Add(sizer_varga, 0, 0, 0);
	wxGridSizer* grid_varga = new wxGridSizer(3, 2, 3, 3);
	sizer_varga->Add(grid_varga, 0, wxALL, 3);
	label_hora = new wxStaticText(sizer_varga->GetStaticBox(), wxID_ANY, _("Hora"));
	grid_varga->Add(label_hora, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_hora_choices[] = {
		_("Parasara"),
		_("Continuous"),
	};
	choice_hora = new wxChoice(sizer_varga->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_hora_choices);
	choice_hora->SetSelection(0);
	grid_varga->Add(choice_hora, 0, wxALIGN_RIGHT|wxALL, 3);
	label_drekkana = new wxStaticText(sizer_varga->GetStaticBox(), wxID_ANY, _("Drekkana"));
	grid_varga->Add(label_drekkana, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_drekkana_choices[] = {
		_("Parasara"),
		_("Continuous"),
	};
	choice_drekkana = new wxChoice(sizer_varga->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_drekkana_choices);
	choice_drekkana->SetSelection(0);
	grid_varga->Add(choice_drekkana, 0, wxALIGN_RIGHT|wxALL, 3);
	label_chaturtamsa = new wxStaticText(sizer_varga->GetStaticBox(), wxID_ANY, _("Chaturtamsa"));
	grid_varga->Add(label_chaturtamsa, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_chaturtamsa_choices[] = {
		_("Parasara"),
		_("Continuous"),
	};
	choice_chaturtamsa = new wxChoice(sizer_varga->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_chaturtamsa_choices);
	choice_chaturtamsa->SetSelection(0);
	grid_varga->Add(choice_chaturtamsa, 0, wxALIGN_RIGHT|wxALL, 3);
	
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	// Diginities
	choice_moon->SetValidator( wxGenericValidator( &cfg.moonBenefic ));
	choice_mercury->SetValidator( wxGenericValidator( &cfg.mercuryBenefic ));
	choice_tempfriend->SetValidator( wxGenericValidator( &cfg.tempFriendMode ));

	// Vargas
	choice_hora->SetValidator( wxGenericValidator( &cfg.vargaHoraMode ));
	choice_drekkana->SetValidator( wxGenericValidator( &cfg.vargaDrekkanaMode ));
	choice_chaturtamsa->SetValidator( wxGenericValidator( &cfg.vargaChaturtamsaMode ));
}


