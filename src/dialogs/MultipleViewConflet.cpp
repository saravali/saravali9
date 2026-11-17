/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/MultipleViewConflet.cpp
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

#include "MultipleViewConflet.h"
#include "logger.h"
#include "MultipleView.h"

#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_SWITCH_MVIEW = wxID_HIGHEST + 1, CD_CHOICE_DEFAULTVIEW };

/*****************************************************
**
**   MultipleViewConflet   ---   Constructor
**
******************************************************/
MultipleViewConflet::MultipleViewConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->multipleView )
{
	// begin wxGlade: MultipleViewConflet::MultipleViewConflet
	wxFlexGridSizer* grid_main = new wxFlexGridSizer(1, 2, 0, 0);
	wxFlexGridSizer* sizer_left = new wxFlexGridSizer(1, 1, 3, 3);
	grid_main->Add(sizer_left, 1, wxEXPAND, 0);
	wxStaticBoxSizer* sizer_mview = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Multiple View Configuration")), wxVERTICAL);
	sizer_left->Add(sizer_mview, 1, wxALL|wxEXPAND, 3);
	check_use_mviews = new wxCheckBox(sizer_mview->GetStaticBox(), CD_SWITCH_MVIEW, _("Use Multiple Views"));
	sizer_mview->Add(check_use_mviews, 0, wxALL, 3);
	wxFlexGridSizer* grid_mview = new wxFlexGridSizer(2, 1, 0, 0);
	sizer_mview->Add(grid_mview, 1, wxEXPAND, 0);
	wxFlexGridSizer* grid_select_view = new wxFlexGridSizer(1, 2, 0, 0);
	grid_mview->Add(grid_select_view, 1, wxALL|wxEXPAND, 3);
	label_defview = new wxStaticText(sizer_mview->GetStaticBox(), wxID_ANY, _("Default View"));
	grid_select_view->Add(label_defview, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_viewlist_choices[] = {
		_("dummy"),
		_("dummy"),
		_("dummy"),
		_("dummy"),
	};
	choice_viewlist = new wxChoice(sizer_mview->GetStaticBox(), CD_CHOICE_DEFAULTVIEW, wxDefaultPosition, wxDefaultSize, 4, choice_viewlist_choices);
	choice_viewlist->SetSelection(0);
	grid_select_view->Add(choice_viewlist, 0, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_description = new wxStaticBoxSizer(new wxStaticBox(sizer_mview->GetStaticBox(), wxID_ANY, _("Description")), wxVERTICAL);
	grid_mview->Add(sizer_description, 1, wxALL|wxEXPAND, 3);
	text_description = new wxTextCtrl(sizer_description->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
	text_description->SetMinSize(wxSize(-1, 150));
	sizer_description->Add(text_description, 0, wxALL|wxEXPAND, 3);
	wxBoxSizer* sizer_right = new wxBoxSizer(wxVERTICAL);
	grid_main->Add(sizer_right, 1, wxEXPAND, 0);
	wxStaticBoxSizer* sizer_nbstyle = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Notebook Style")), wxVERTICAL);
	sizer_right->Add(sizer_nbstyle, 1, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* grid_nbstyle = new wxFlexGridSizer(2, 2, 3, 3);
	sizer_nbstyle->Add(grid_nbstyle, 0, wxALL, 3);
	label_nbstyle = new wxStaticText(sizer_nbstyle->GetStaticBox(), wxID_ANY, _("Style"));
	grid_nbstyle->Add(label_nbstyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_nbstyle_choices[] = {
		_("Notebook"),
		_("Listbook"),
		_("Choicebook"),
		_("Toolbook"),
		_("Treebook"),
	};
	choice_nbstyle = new wxChoice(sizer_nbstyle->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 5, choice_nbstyle_choices);
	choice_nbstyle->SetSelection(0);
	grid_nbstyle->Add(choice_nbstyle, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
	label_orientation = new wxStaticText(sizer_nbstyle->GetStaticBox(), wxID_ANY, _("Orientation"));
	grid_nbstyle->Add(label_orientation, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_orientation_choices[] = {
		_("Top"),
		_("Bottom"),
		_("Left"),
		_("Right"),
	};
	choice_orientation = new wxChoice(sizer_nbstyle->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, choice_orientation_choices);
	choice_orientation->SetSelection(0);
	grid_nbstyle->Add(choice_orientation, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
	
	grid_select_view->AddGrowableCol(1);
	grid_mview->AddGrowableRow(2);
	grid_mview->AddGrowableCol(0);
	SetSizer(grid_main);
	grid_main->Fit(this);
	// end wxGlade

	choice_viewlist->Clear();
	for( const auto &def : MultipleViewHandler::get()->getBuiltinDefs())
	{
		choice_viewlist->Append( def.name );
	}

	choice_viewlist->SetValidator( wxGenericValidator( &cfg.defaultView ));
	check_use_mviews->SetValidator( wxGenericValidator( &cfg.useMultipleViews ));

	choice_nbstyle->SetValidator( wxGenericValidator( &cfg.notebookStyle ));
	choice_orientation->SetValidator( wxGenericValidator( &cfg.notebookOrientation ));
}

/*****************************************************
**
**   MultipleViewConflet   ---   restoreDefaults
**
******************************************************/
void MultipleViewConflet::restoreDefaults()
{
	debuglog << "MultipleViewConflet::restoreDefaults";
  cfg = MultipleViewConfig();
	TransferDataToWindow();
}

/*****************************************************
**
**   MultipleViewConflet   ---   updateUi
**
******************************************************/
void MultipleViewConflet::updateUi()
{
	debuglog << "MultipleViewConflet::OnCommand";

	const int sel = choice_viewlist->GetSelection();

	if ( sel == -1 )
	{
		text_description->ChangeValue( tr( "Error" ));
	}
	else
	{
		text_description->ChangeValue( MultipleViewHandler::get()->getBuiltinDef( sel ).description );
	}

	const bool multipleMode = check_use_mviews->GetValue();

	label_defview->Enable( multipleMode );
	choice_viewlist->Enable( multipleMode );
	text_description->Enable( multipleMode );
}


