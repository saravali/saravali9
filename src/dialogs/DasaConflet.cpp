/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/DasaConflet.cpp
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

#include "DasaConflet.h"

#include "CommonControls.h"
#include "enumserializer.h"
#include "logger.h"
#include "mathbase.h"

#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

extern Astroconfig *astroconfig;

// begin wxGlade: ::extracode
// end wxGlade

/*****************************************************
**
**   DasaConflet   ---   Constructor
**
******************************************************/
DasaConflet::DasaConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, astroconfig->dasa )
{
	// begin wxGlade: DasaConflet::DasaConflet
	wxFlexGridSizer* sizer_main = new wxFlexGridSizer(3, 1, 0, 0);
	wxStaticBoxSizer* sizer_general = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("General")), wxHORIZONTAL);
	sizer_main->Add(sizer_general, 1, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* grid_general = new wxFlexGridSizer(3, 2, 3, 3);
	sizer_general->Add(grid_general, 1, wxALL, 3);
	label_nakdasa = new wxStaticText(sizer_general->GetStaticBox(), wxID_ANY, _("Nakshatra Dasa Calculation"));
	grid_general->Add(label_nakdasa, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString *choice_nakdasa_choices = NULL;
	choice_nakdasa = new wxChoice(sizer_general->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, choice_nakdasa_choices);
	grid_general->Add(choice_nakdasa, 0, wxALIGN_RIGHT|wxALL, 3);
	label_dasa_enddate = new wxStaticText(sizer_general->GetStaticBox(), wxID_ANY, _("Display Date for Dasas"));
	grid_general->Add(label_dasa_enddate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString *choice_dasa_show_date_mode_choices = NULL;
	choice_dasa_show_date_mode = new wxChoice(sizer_general->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, choice_dasa_show_date_mode_choices);
	grid_general->Add(choice_dasa_show_date_mode, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	label_last_mahadasa = new wxStaticText(sizer_general->GetStaticBox(), wxID_ANY, _("Start of last Mahadasa"));
	grid_general->Add(label_last_mahadasa, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxBoxSizer* sizer_maha = new wxBoxSizer(wxHORIZONTAL);
	grid_general->Add(sizer_maha, 1, wxEXPAND, 0);
	spin_last_mahadasa = new wxSpinCtrl(sizer_general->GetStaticBox(), wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 50, 500);
	sizer_maha->Add(spin_last_mahadasa, 0, wxALL, 3);
	label_last_mahadasa2 = new wxStaticText(sizer_general->GetStaticBox(), wxID_ANY, _("years"));
	sizer_maha->Add(label_last_mahadasa2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	yearLengthPanel = new YearLengthPanel(this, wxID_ANY, cfg.yearLength);
	yearLengthPanel->SetMinSize(wxSize(200, 100));
	sizer_main->Add(yearLengthPanel, 1, wxEXPAND, 0);
	wxStaticBoxSizer* sizer_kalachakra = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Kalachakra Dasa")), wxVERTICAL);
	sizer_main->Add(sizer_kalachakra, 1, wxEXPAND, 0);
	wxBoxSizer* sizer_kalachakra1 = new wxBoxSizer(wxHORIZONTAL);
	sizer_kalachakra->Add(sizer_kalachakra1, 1, wxALL|wxEXPAND, 2);
	label_kalachakra = new wxStaticText(sizer_kalachakra->GetStaticBox(), wxID_ANY, _("Calculation Mode"));
	sizer_kalachakra1->Add(label_kalachakra, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString *choice_kala_choices = NULL;
	choice_kala = new wxChoice(sizer_kalachakra->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, choice_kala_choices);
	sizer_kalachakra1->Add(choice_kala, 1, wxALL|wxEXPAND, 3);
	
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	for( const NakshatraPortionMode &m : Enum<NakshatraPortionMode>())
	{
		choice_nakdasa->Append( resolveCamelNotation( m ));
	}
	choice_nakdasa->SetValidator( wxGenericValidator( (int*)&cfg.nakshatraPortionMode ));

	for( const DasaShowDateMode &m : Enum<DasaShowDateMode>())
	{
		choice_dasa_show_date_mode->Append( resolveCamelNotation( m ));
	}
	choice_dasa_show_date_mode->SetValidator( wxGenericValidator( (int*)&cfg.dasaDateDisplayMode ));

	spin_last_mahadasa->SetValidator( wxGenericValidator( &cfg.startOfLastMahadasa ));

	for( const KalachakraMode &m : Enum<KalachakraMode>())
	{
		choice_kala->Append( resolveCamelNotation( m ));
	}
	choice_kala->SetValidator( wxGenericValidator( (int*)&cfg.kalachakraMode ));
}

