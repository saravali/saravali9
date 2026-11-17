/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AtlasAliasDialog.cpp
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

#include "AtlasAliasDialog.h"

// begin wxGlade: ::extracode
// end wxGlade

#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textdlg.h>
#include <wx/tokenzr.h>
#include <sstream>

#include "AtlasDao.h"
#include "constants.h"
#include "guibase.h"

enum { AL_LIST = wxID_HIGHEST + 1, AL_ADD, AL_EDIT, AL_DELETE };

/*****************************************************
**
**   AtlasAliasDialog   ---   Constructor
**
******************************************************/
AtlasAliasDialog::AtlasAliasDialog(wxWindow* parent, AtlasEntry &entry )
 : wxDialog(parent, -1, _( "Edit Aliases" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE),
 entry( entry )
{
    // begin wxGlade: AtlasAliasDialog::AtlasAliasDialog
    SetTitle(_("Edit Aliases"));
    wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
    label_name = new wxStaticText(this, wxID_ANY, _("Name"));
    sizer_main->Add(label_name, 0, wxALL, 3);
    wxBoxSizer* sizer_attributes = new wxBoxSizer(wxHORIZONTAL);
    sizer_main->Add(sizer_attributes, 1, wxEXPAND, 0);
    wxStaticBoxSizer* sizer_list = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Aliases")), wxHORIZONTAL);
    sizer_attributes->Add(sizer_list, 1, wxEXPAND, 0);
    wxFlexGridSizer* grid_list = new wxFlexGridSizer(1, 1, 0, 0);
    sizer_list->Add(grid_list, 1, wxEXPAND, 0);
    const wxString list_choices[] = {
        _("dummy"),
    };
    list = new wxListBox(this, AL_LIST, wxDefaultPosition, wxDefaultSize, 1, list_choices, wxLB_SINGLE|wxLB_SORT);
    list->SetMinSize(wxSize(200, 300));
    grid_list->Add(list, 0, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_actions = new wxBoxSizer(wxVERTICAL);
    sizer_attributes->Add(sizer_actions, 0, 0, 3);
    button_add = new wxButton(this, AL_ADD, _("Add Entry"));
    sizer_actions->Add(button_add, 0, wxALL|wxEXPAND, 3);
    button_edit = new wxButton(this, AL_EDIT, _("Edit Entry"));
    sizer_actions->Add(button_edit, 0, wxALL|wxEXPAND, 3);
    button_delete = new wxButton(this, AL_DELETE, _("Delete Entry"));
    sizer_actions->Add(button_delete, 0, wxALL|wxEXPAND, 3);
    static_line_4 = new wxStaticLine(this, wxID_ANY);
    sizer_actions->Add(static_line_4, 0, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_bottom = new wxBoxSizer(wxHORIZONTAL);
    sizer_main->Add(sizer_bottom, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
    button_ok = new wxButton(this, wxID_OK, _("OK"));
    sizer_bottom->Add(button_ok, 0, wxALL|wxEXPAND, 3);
    button_cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
    sizer_bottom->Add(button_cancel, 0, wxALL|wxEXPAND, 3);
    
    grid_list->AddGrowableRow(0);
    grid_list->AddGrowableCol(0);
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    Layout();
    // end wxGlade

	wxString label;
	label << _( "Name" ) << ": " << entry.name;
	label_name->SetLabel( label );

	wxStringTokenizer tk( entry.aliases, "," );
	wxString token;

	list->Clear();
	while ( tk.HasMoreTokens() )
	{
		token = tk.GetNextToken().Trim( true ).Trim( false );
		list->Append( token );
	}
	if ( list->GetCount() > 0 ) list->Select( 0 );

	Connect( AL_ADD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasAliasDialog::OnAdd ));
	Connect( AL_EDIT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasAliasDialog::OnEdit ));
	Connect( AL_DELETE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasAliasDialog::OnDelete ));
	Connect( wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasAliasDialog::OnOK ));
}

/*****************************************************
**
**   AtlasAliasDialog   ---   OnAdd
**
******************************************************/
void AtlasAliasDialog::OnAdd( wxCommandEvent& )
{
	wxTextEntryDialog dialog( this, _( "Add Entry" ), _( "Alias" ));
	if ( dialog.ShowModal() == wxID_OK )
	{
		list->Append( dialog.GetValue());

		// select new entry
		const int index = list->FindString( dialog.GetValue());
		if ( index != wxNOT_FOUND )
		{
			list->SetSelection( index );
		}
	}
}

/*****************************************************
**
**   AtlasAliasDialog   ---   OnEdit
**
******************************************************/
void AtlasAliasDialog::OnEdit( wxCommandEvent& )
{
	const int sel = list->GetSelection();
	if ( sel != wxNOT_FOUND )
	{
		wxTextEntryDialog dialog( this, _( "Edit Entry" ), _( "Alias" ), list->GetString( sel ) );
		if ( dialog.ShowModal() == wxID_OK )
		{
			list->SetString( sel, dialog.GetValue());
		}
	}
	else
	{
		doMessageBox( this, "No entry selected" );
	}
}

/*****************************************************
**
**   AtlasAliasDialog   ---   OnDelete
**
******************************************************/
void AtlasAliasDialog::OnDelete( wxCommandEvent& )
{
	int sel = list->GetSelection();
	if ( sel != wxNOT_FOUND )
	{
		//wxString message = wxString::Format( tr( "Do you want to delete the item %s?" ), list->GetString( sel ));
		list->Delete( sel );
	}
	else
	{
		doMessageBox( this, "No entry selected" );
	}
}

/*****************************************************
**
**   AtlasAliasDialog   ---   OnOK
**
******************************************************/
void AtlasAliasDialog::OnOK( wxCommandEvent& )
{
	const int size = list->GetCount();
	wxString s;

	for( int i = 0; i < size; i++ )
	{
		if ( i > 0 )
		{
			s += ", ";
		}
		s += list->GetString( i );
	}
	entry.aliases = s;
	EndModal( wxID_OK );
}

