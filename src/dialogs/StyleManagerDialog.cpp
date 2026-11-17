/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/StyleManagerDialog.cpp
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

#include "StyleManagerDialog.h"

#include "FileConfig.h"
#include "guibase.h"
#include "logger.h"

#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/textdlg.h>

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_LIST = wxID_HIGHEST + 300, CD_NEW, CD_COPY, CD_RENAME, CD_DELETE, CD_UP, CD_DOWN };

/*****************************************************
**
**   StyleManagerDialog   ---   Constructor
**
******************************************************/
StyleManagerDialog::StyleManagerDialog(wxWindow* parent, std::vector<TableStyle> &configs, int &sel )
	: wxDialog( parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, DEFAULT_DIALOG_STYLE ),
	v( configs ),
	sel( sel )
{
    // begin wxGlade: StyleManagerDialog::StyleManagerDialog
    wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_top = new wxBoxSizer(wxHORIZONTAL);
    sizer_main->Add(sizer_top, 1, wxEXPAND, 0);
    wxStaticBoxSizer* sizer_list = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Styles")), wxVERTICAL);
    sizer_top->Add(sizer_list, 1, wxEXPAND, 3);
    const wxString list_choices[] = {
        _("dummy"),
    };
    list = new wxListBox(this, CD_LIST, wxDefaultPosition, wxDefaultSize, 1, list_choices, wxLB_SINGLE);
    list->SetMinSize(wxSize(300, 300));
    sizer_list->Add(list, 1, wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_right = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Actions")), wxVERTICAL);
    sizer_top->Add(sizer_right, 0, wxALIGN_BOTTOM|wxALL, 3);
    button_new = new wxButton(this, CD_NEW, _("New"));
    sizer_right->Add(button_new, 0, wxALL|wxEXPAND, 3);
    button_copy = new wxButton(this, CD_COPY, _("Copy"));
    sizer_right->Add(button_copy, 0, wxALL|wxEXPAND, 3);
    button_rename = new wxButton(this, CD_RENAME, _("Rename"));
    sizer_right->Add(button_rename, 0, wxALL|wxEXPAND, 3);
    button_delete = new wxButton(this, CD_DELETE, _("Delete"));
    sizer_right->Add(button_delete, 0, wxALL|wxEXPAND, 3);
    button_up = new wxButton(this, CD_UP, _("Up"));
    sizer_right->Add(button_up, 0, wxALL|wxEXPAND, 3);
    button_down = new wxButton(this, CD_DOWN, _("Down"));
    sizer_right->Add(button_down, 0, wxALL|wxEXPAND, 3);
    wxStaticLine* static_line = new wxStaticLine(this, wxID_ANY);
    sizer_main->Add(static_line, 0, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_bottom = new wxBoxSizer(wxHORIZONTAL);
    sizer_main->Add(sizer_bottom, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
    button_ok = new wxButton(this, wxID_OK, _("Close"));
    button_ok->SetDefault();
    sizer_bottom->Add(button_ok, 0, wxALL, 3);
    
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    Layout();
    // end wxGlade

	populateChoice();
	list->SetSelection( sel );
	list->Refresh();

	updateUi();

	Connect( CD_NEW, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( StyleManagerDialog::OnNew ));
	Connect( CD_COPY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( StyleManagerDialog::OnCopy ));
	Connect( CD_RENAME, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( StyleManagerDialog::OnRename ));
	Connect( CD_DELETE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( StyleManagerDialog::OnDelete ));
	Connect( CD_UP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( StyleManagerDialog::OnUp ));
	Connect( CD_DOWN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( StyleManagerDialog::OnDown ));
	Connect( CD_LIST, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( StyleManagerDialog::OnListbox ));
	Connect( CD_LIST, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( StyleManagerDialog::OnListboxDoubleClick ));
}

/*****************************************************
**
**   StyleManagerDialog   ---   populateChoice
**
******************************************************/
void StyleManagerDialog::populateChoice()
{
	list->Clear();

	for( const auto &a : v )
	{
		list->Append( a.name );
	}
	list->Refresh();
}

/*****************************************************
**
**   StyleManagerDialog   ---   updateUi
**
******************************************************/
void StyleManagerDialog::updateUi()
{
	sel = getSelection();
	button_copy->Enable( sel != -1 );
	button_rename->Enable( sel != -1 );
	button_delete->Enable( sel != -1 );
	button_up->Enable( sel > 0 );
	button_down->Enable( sel != (int)(list->GetCount() - 1 ));
}

/*****************************************************
**
**   StyleManagerDialog   ---   getSelection
**
******************************************************/
int StyleManagerDialog::getSelection() const
{
	return list->GetSelection();
}

/*****************************************************
**
**   StyleManagerDialog   ---   OnListbox
**
******************************************************/
void StyleManagerDialog::OnListbox( wxCommandEvent& )
{
	updateUi();
}

/*****************************************************
**
**   StyleManagerDialog   ---   OnListboxDoubleClick
**
******************************************************/
void StyleManagerDialog::OnListboxDoubleClick( wxCommandEvent& )
{
	renameEntry( getSelection() );
}

/*****************************************************
**
**   StyleManagerDialog   ---   OnNew
**
******************************************************/
void StyleManagerDialog::OnNew( wxCommandEvent& )
{
	wxTextEntryDialog dialog( this, _( "Enter Name" ), _( "New Entry" ), _( "New Entry" ));
	if ( dialog.ShowModal() == wxID_OK )
	{
		TableStyle style;
		style.name = dialog.GetValue();
		v.push_back( style );

		populateChoice();
		list->SetSelection( list->GetCount() - 1 );
		list->Refresh();
	}
}

/*****************************************************
**
**   StyleManagerDialog   ---   OnCopy
**
******************************************************/
void StyleManagerDialog::OnCopy( wxCommandEvent& )
{
	TableStyle style;
	style.name = v[getSelection()].name +  " (copy)";
	v.push_back( style );

	populateChoice();
	list->SetSelection( list->GetCount() - 1 );
	list->Refresh();
	updateUi();
}

/*****************************************************
**
**   StyleManagerDialog   ---   OnUp
**
******************************************************/
void StyleManagerDialog::OnUp( wxCommandEvent& )
{
	sel = getSelection();
	const int target = sel - 1;
	assert( target >= 0 );

	TableStyle cfg = v[sel];
	v[sel] = v[target];
	v[target] = cfg;

	populateChoice();
	list->SetSelection( target );
	list->Refresh();
	updateUi();
}

/*****************************************************
**
**   StyleManagerDialog   ---   OnDown
**
******************************************************/
void StyleManagerDialog::OnDown( wxCommandEvent& )
{
	sel = getSelection();
	const int target = sel + 1;
	assert( target < (int)list->GetCount() );

	TableStyle cfg = v[sel];
	v[sel] = v[target];
	v[target] = cfg;

	populateChoice();
	list->SetSelection( target );
	list->Refresh();
	updateUi();
}

/*****************************************************
**
**   StyleManagerDialog   ---   renameEntry
**
******************************************************/
void StyleManagerDialog::renameEntry( const int &sel )
{
	wxTextEntryDialog dialog( this, _( "Enter New Name" ), _( "Rename Entry" ), v[sel].name );
	if ( dialog.ShowModal() == wxID_OK )
	{
		v[sel].name = dialog.GetValue();
		populateChoice();
		list->SetSelection( sel );
		list->Refresh();
	}
	list->Refresh();
	updateUi();
}

/*****************************************************
**
**   StyleManagerDialog   ---   OnRename
**
******************************************************/
void StyleManagerDialog::OnRename( wxCommandEvent& )
{
	renameEntry( getSelection() );
	updateUi();
}

/*****************************************************
**
**   StyleManagerDialog   ---   OnDelete
**
******************************************************/
void StyleManagerDialog::OnDelete( wxCommandEvent& )
{
	sel = getSelection();
	assert( sel >= 0 && sel < (int)v.size());

	const int answer = doMessageBox( this,
		wxString::Format( _( "Do you want to delete the entry %s?" ), v[sel].name ),
		wxYES_NO + wxNO_DEFAULT + wxICON_QUESTION );
	if ( answer == wxID_YES )
	{
		v.erase( std::next( v.begin(), sel ));

		if ( v.size() == 0 )
		{
			v.push_back( TableStyle() );
		}
		if ( sel == (int)v.size()) sel--;

		populateChoice();
		list->SetSelection( sel );
		list->Refresh();
	}
	updateUi();
}


