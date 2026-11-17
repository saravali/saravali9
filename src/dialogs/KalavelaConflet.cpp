/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/KalavelaConflet.cpp
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

#include "KalavelaConflet.h"
#include "logger.h"

#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>

extern Astroconfig *astroconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_KALAVELA_CHOICE = wxID_HIGHEST + 1, CD_KALAVELA_LORD_CHOICE, CD_KALAVELA_PORTION_CHOICE };

/*****************************************************
**
**   KalavelaConflet   ---   Constructor
**
******************************************************/
KalavelaConflet::KalavelaConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, astroconfig->kalavela )
{
	// begin wxGlade: KalavelaConflet::KalavelaConflet
	wxStaticBoxSizer* sizer_kalavelas = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Kalavelas")), wxVERTICAL);
	wxFlexGridSizer* grid_kalavelas = new wxFlexGridSizer(3, 2, 3, 3);
	sizer_kalavelas->Add(grid_kalavelas, 1, wxALL, 3);
	label_kalavela = new wxStaticText(sizer_kalavelas->GetStaticBox(), wxID_ANY, _("Kala Vela"));
	grid_kalavelas->Add(label_kalavela, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_kalavela_choices[] = {
		_("Kala"),
		_("Mrityu"),
		_("Ardhaprahara"),
		_("Yama Ghantaka"),
		_("Gulika"),
		_("Mandi"),
	};
	choice_kalavela = new wxChoice(sizer_kalavelas->GetStaticBox(), CD_KALAVELA_CHOICE, wxDefaultPosition, wxDefaultSize, 6, choice_kalavela_choices);
	choice_kalavela->SetSelection(0);
	grid_kalavelas->Add(choice_kalavela, 0, wxALIGN_RIGHT|wxALL, 3);
	label_kalalord = new wxStaticText(sizer_kalavelas->GetStaticBox(), wxID_ANY, _("Lord"));
	grid_kalavelas->Add(label_kalalord, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_kalalord_choices[] = {
		_("None"),
		_("Sun"),
		_("Moon"),
		_("Mars"),
		_("Mercury"),
		_("Jupiter"),
		_("Venus"),
		_("Saturn"),
		_("Eight"),
	};
	choice_kalalord = new wxChoice(sizer_kalavelas->GetStaticBox(), CD_KALAVELA_LORD_CHOICE, wxDefaultPosition, wxDefaultSize, 9, choice_kalalord_choices);
	choice_kalalord->SetSelection(0);
	grid_kalavelas->Add(choice_kalalord, 0, wxALIGN_RIGHT|wxALL, 3);
	label_kalaportion = new wxStaticText(sizer_kalavelas->GetStaticBox(), -1, _("Part of Portion"));
	grid_kalavelas->Add(label_kalaportion, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_kalaportion_choices[] = {
		_("Beginning"),
		_("Middle"),
		_("End"),
	};
	choice_kalaportion = new wxChoice(sizer_kalavelas->GetStaticBox(), CD_KALAVELA_PORTION_CHOICE, wxDefaultPosition, wxDefaultSize, 3, choice_kalaportion_choices);
	choice_kalaportion->SetSelection(0);
	grid_kalavelas->Add(choice_kalaportion, 0, wxALIGN_RIGHT|wxALL, 3);
	
	grid_kalavelas->AddGrowableCol(1);
	SetSizer(sizer_kalavelas);
	sizer_kalavelas->Fit(this);
	// end wxGlade

	Connect( CD_KALAVELA_CHOICE, wxEVT_COMMAND_CHOICE_SELECTED,
		wxCommandEventHandler( KalavelaConflet::OnKalaVelaChoice ));
	Connect( CD_KALAVELA_LORD_CHOICE, wxEVT_COMMAND_CHOICE_SELECTED,
		wxCommandEventHandler( KalavelaConflet::OnKalaVelaLordChoice ));
	Connect( CD_KALAVELA_PORTION_CHOICE, wxEVT_COMMAND_CHOICE_SELECTED,
		wxCommandEventHandler( KalavelaConflet::OnKalaVelaPortionChoice ));
}

/*****************************************************
**
**   KalavelaConflet   ---   getSelectedElement
**
******************************************************/
KalavelaElement &KalavelaConflet::getSelectedElement()
{
	const int sel = choice_kalavela->GetSelection();
	assert( sel >= 0 && sel < 6 );
	const PlanetId ob = (PlanetId)( (int)PlanetId::Kala + sel );

	const auto &k = cfg.elements.find( ob );
	if ( k != cfg.elements.end())
	{
		return k->second;
	}
	else
	{
		cfg.elements[ ob ] = KalavelaElement();
		return cfg.elements.find( ob )->second;
	}
}

/*****************************************************
**
**   KalavelaConflet   ---   updateUi
**
******************************************************/
void KalavelaConflet::updateUi()
{
	debuglog << "KalavelaConflet::updateUi";
	const KalavelaElement &k = getSelectedElement();
	choice_kalalord->SetSelection( (int)k.lord + 1 );
	choice_kalaportion->SetSelection( (int)k.portion );
}

/*****************************************************
**
**   KalavelaConflet   ---   OnKalaVelaChoice
**
******************************************************/
void KalavelaConflet::OnKalaVelaChoice( wxCommandEvent& )
{
	updateUi();
}

/*****************************************************
**
**   KalavelaConflet   ---   OnKalaVelaLordChoice
**
******************************************************/
void KalavelaConflet::OnKalaVelaLordChoice( wxCommandEvent& )
{
	debuglog << "KalavelaConflet::OnKalaVelaLordChoice sel " << choice_kalavela->GetSelection();
	KalavelaElement &k = getSelectedElement();
	k.lord = (KalavelaLord)(choice_kalalord->GetSelection() - 1);
	guiHasChanged();
}

/*****************************************************
**
**   KalavelaConflet   ---   OnKalaVelaPortionChoice
**
******************************************************/
void KalavelaConflet::OnKalaVelaPortionChoice( wxCommandEvent& )
{
	debuglog << "KalavelaConflet::OnKalaVelaPortionChoice sel " << choice_kalavela->GetSelection();
	KalavelaElement &k = getSelectedElement();
	k.portion = (KalavelaPortion)choice_kalaportion->GetSelection();
	guiHasChanged();
}


