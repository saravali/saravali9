/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AnimationConflet.cpp
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

#include "AnimationConflet.h"
#include "logger.h"

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
**   AnimationConflet   ---   Constructor
**
******************************************************/
AnimationConflet::AnimationConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, astroconfig->animation )
{
	// begin wxGlade: AnimationConflet::AnimationConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBoxSizer* sizer_animate = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Animation")), wxVERTICAL);
	sizer_main->Add(sizer_animate, 1, wxALL, 3);
	wxFlexGridSizer* grid_ani2 = new wxFlexGridSizer(3, 2, 3, 3);
	sizer_animate->Add(grid_ani2, 1, wxALL, 3);
	label_animation_freq = new wxStaticText(sizer_animate->GetStaticBox(), wxID_ANY, _("Frequency"));
	grid_ani2->Add(label_animation_freq, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxBoxSizer* grid_freq = new wxBoxSizer(wxHORIZONTAL);
	grid_ani2->Add(grid_freq, 1, 0, 0);
	spin_animation_freq = new wxSpinCtrl(sizer_animate->GetStaticBox(), wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000);
	spin_animation_freq->SetMinSize(wxSize(50, -1));
	grid_freq->Add(spin_animation_freq, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	label_animation_freq2 = new wxStaticText(sizer_animate->GetStaticBox(), wxID_ANY, _("sec"));
	grid_freq->Add(label_animation_freq2, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 10);
	label_animation_mode = new wxStaticText(sizer_animate->GetStaticBox(), wxID_ANY, _("Mode"));
	grid_ani2->Add(label_animation_mode, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_animation_mode_choices[] = {
		_("System Time"),
		_("Custom Steps"),
	};
	choice_animation_mode = new wxChoice(sizer_animate->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_animation_mode_choices);
	choice_animation_mode->SetSelection(0);
	grid_ani2->Add(choice_animation_mode, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	label_animation_step = new wxStaticText(sizer_animate->GetStaticBox(), wxID_ANY, _("Step Length"));
	grid_ani2->Add(label_animation_step, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxBoxSizer* grid_steplen = new wxBoxSizer(wxHORIZONTAL);
	grid_ani2->Add(grid_steplen, 1, 0, 0);
	spin_animation_steplength = new wxSpinCtrl(sizer_animate->GetStaticBox(), wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, 1, 1000);
	spin_animation_steplength->SetMinSize(wxSize(50, -1));
	grid_steplen->Add(spin_animation_steplength, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_animate_dimension_choices[] = {
		_("seconds"),
		_("minutes"),
		_("hours"),
		_("days"),
		_("months"),
		_("years"),
	};
	choice_animate_dimension = new wxChoice(sizer_animate->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 6, choice_animate_dimension_choices);
	choice_animate_dimension->SetSelection(0);
	grid_steplen->Add(choice_animate_dimension, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	
	grid_ani2->AddGrowableCol(1);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	spin_animation_freq->SetValidator( wxGenericValidator( &cfg.interval ));
	choice_animation_mode->SetValidator( wxGenericValidator( &cfg.mode ));
	spin_animation_steplength->SetValidator( wxGenericValidator( &cfg.stepLength ));
	choice_animate_dimension->SetValidator( wxGenericValidator( &cfg.stepDimension ));
}

/*****************************************************
**
**   AnimationConflet   ---   updateUi
**
******************************************************/
void AnimationConflet::updateUi()
{
	debuglog << "AnimationConflet::OnCommand";
	spin_animation_steplength->Enable( choice_animation_mode->GetSelection() > 0 );
	choice_animate_dimension->Enable( choice_animation_mode->GetSelection() > 0 );
}


