/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AshtakavargaConflet.cpp
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

#include "AshtakavargaConflet.h"
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
**   AshtakavargaConflet   ---   Constructor
**
******************************************************/
AshtakavargaConflet::AshtakavargaConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, astroconfig->ashtakavarga )
{
	// begin wxGlade: AshtakavargaConflet::AshtakavargaConflet
	wxStaticBoxSizer* sizer_main = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Ashtakavarga")), wxVERTICAL);
	wxGridSizer* grid_av = new wxGridSizer(4, 2, 0, 0);
	sizer_main->Add(grid_av, 0, wxALL, 3);
	label_av_calcrules = new wxStaticText(sizer_main->GetStaticBox(), wxID_ANY, _("Rekha Calculation Rules"));
	grid_av->Add(label_av_calcrules, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_av_calcrules_choices[] = {
		_("Parasara"),
		_("Vahara Mihira"),
	};
	choice_av_calcrules = new wxChoice(sizer_main->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_av_calcrules_choices);
	choice_av_calcrules->SetSelection(0);
	grid_av->Add(choice_av_calcrules, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	label_av_benefics = new wxStaticText(sizer_main->GetStaticBox(), wxID_ANY, _("Display Benefic points as"));
	grid_av->Add(label_av_benefics, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_av_benefics_choices[] = {
		_("Number"),
		_("Line"),
		_("Dot"),
	};
	choice_av_benefics = new wxChoice(sizer_main->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_av_benefics_choices);
	choice_av_benefics->SetSelection(0);
	grid_av->Add(choice_av_benefics, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	label_av_sarva = new wxStaticText(sizer_main->GetStaticBox(), wxID_ANY, _("Sarvashtakavarga includes"));
	grid_av->Add(label_av_sarva, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_av_sarva_choices[] = {
		_("Planets only"),
		_("With Lagna"),
	};
	choice_av_sarva = new wxChoice(sizer_main->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_av_sarva_choices);
	choice_av_sarva->SetSelection(0);
	grid_av->Add(choice_av_sarva, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	label_av_sodhya = new wxStaticText(sizer_main->GetStaticBox(), wxID_ANY, _("Sodhya Pinda Calculcation Rule"));
	grid_av->Add(label_av_sodhya, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_av_sodhya_choices[] = {
		_("Parasara"),
		_("Mantreswar"),
	};
	choice_av_sodhya = new wxChoice(sizer_main->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_av_sodhya_choices);
	choice_av_sodhya->SetSelection(0);
	grid_av->Add(choice_av_sodhya, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	choice_av_benefics->SetValidator( wxGenericValidator( &cfg.ashtakavargaNumberMode ));
	choice_av_calcrules->SetValidator( wxGenericValidator( &cfg.ashtakavargaMode ));
	choice_av_benefics->SetValidator( wxGenericValidator( &cfg.ashtakavargaNumberMode ));
	choice_av_sarva->SetValidator( wxGenericValidator( &cfg.ashtakavargaSarva));
	choice_av_sodhya->SetValidator( wxGenericValidator( &cfg.ashtakavargaSodhyaPindaMode ));
}


