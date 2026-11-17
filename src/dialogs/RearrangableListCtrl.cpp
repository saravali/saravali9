/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/RearrangableListCtrl.cpp
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

#include "RearrangableListCtrl.h"

#include "Commands.h"
#include "guibase.h"
#include "ImageProvider.h"
#include "logger.h"
#include "mathbase.h"

#include <wx/bmpbuttn.h>
#include <wx/checklst.h>
#include <wx/sizer.h>

enum { CD_LIST = wxID_HIGHEST + 1, CD_UP, CD_DOWN };

/*****************************************************
**
**   RearrangableListCtrlBase   ---   Constructor
**
******************************************************/
RearrangableListCtrlBase::RearrangableListCtrlBase( wxWindow *parent, const int &id )
	: wxPanel( parent, id, wxDefaultPosition, wxDefaultSize )
{
	wxSizer *sizer =  new wxBoxSizer( wxHORIZONTAL );
	checklist = new wxCheckListBox( this, CD_LIST );
	sizer->Add( checklist, 1, wxALL | wxEXPAND , 3 );
	wxSizer *buttonsizer =  new wxBoxSizer( wxVERTICAL );

	ImageProvider *ip = ImageProvider::get();
	button_up = new wxBitmapButton( this, CD_UP, ip->getBitmap( BitmapId::ArrowUp, IconSize::Small ));
	button_up->SetToolTip(_("Move current item up"));
	buttonsizer->Add( button_up, 0, wxALL | wxEXPAND , 3 );
	button_down = new wxBitmapButton( this, CD_DOWN, ip->getBitmap( BitmapId::ArrowDown, IconSize::Small ));
	button_down->SetToolTip(_("Move current item down"));
	buttonsizer->Add( button_down, 0, wxALL | wxEXPAND , 3 );

	sizer->Add( buttonsizer, 0, wxALL | wxCENTER , 3 );
	SetSizer( sizer );
	updateUi();

	Connect( CD_LIST, wxEVT_CHECKLISTBOX, wxCommandEventHandler( RearrangableListCtrlBase::OnItemChecked ));
	Connect( CD_LIST, wxEVT_LISTBOX, wxCommandEventHandler( RearrangableListCtrlBase::OnListbox ));
	Connect( CD_UP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RearrangableListCtrlBase::OnUp ));
	Connect( CD_DOWN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RearrangableListCtrlBase::OnDown ));
}

/*****************************************************
**
**   RearrangableListCtrlBase   ---   appendItem
**
******************************************************/
void RearrangableListCtrlBase::appendItem( const wxString &s, const bool &checked )
{
	assert( checklist );
	checklist->Append( resolveCamelNotation( s ));
	checklist->Check( checklist->GetCount() - 1, checked );
	updateUi();
}

/*****************************************************
**
**   RearrangableListCtrlBase   ---   OnListbox
**
******************************************************/
void RearrangableListCtrlBase::OnListbox( wxCommandEvent &event )
{
	const int sel = checklist->GetSelection();
	debuglog << "RearrangableListCtrlBase::OnListbox int " << event.GetInt() << " selection " << sel;
	updateUi();
}

/*****************************************************
**
**   RearrangableListCtrlBase   ---   OnItemChecked
**
******************************************************/
void RearrangableListCtrlBase::OnItemChecked( wxCommandEvent &event )
{
	const uint i = event.GetInt();
	const bool isChecked = checklist->IsChecked( i );
	debuglog << "RearrangableListCtrlBase::OnItemChecked int " << i << " checked " << isChecked;

	checkItem( i, isChecked );

	updateUi();
	emitChangeEvent();
}

/*****************************************************
**
**   RearrangableListCtrlBase   ---   emitChangeEvent
**
******************************************************/
void RearrangableListCtrlBase::emitChangeEvent()
{
	wxCommandEvent e( COMMAND_REARRANGECTRL_CHANGED, GetId() );
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   RearrangableListCtrlBase   ---   OnUp
**
******************************************************/
void RearrangableListCtrlBase::OnUp( wxCommandEvent &event )
{
	const uint sel = checklist->GetSelection();
	debuglog << "RearrangableListCtrl::OnUp int " << event.GetInt() << " selection " << sel;
	if ( sel < 1 )
	{
		warnlog << "Cannot move item " << sel << " up";
		return;
	}

	// store status of item at position sel-1
	const wxString tmplabel = checklist->GetString( sel - 1 );
	const bool tmpchecked = checklist->IsChecked( sel - 1 );

	// set status of sel-1 -> sel
	checklist->SetString( sel - 1, checklist->GetString( sel ));
	checklist->Check( sel - 1, checklist->IsChecked( sel ));

	// set status of sel to stored status
	checklist->SetString( sel, tmplabel );
	checklist->Check( sel, tmpchecked );

	checklist->SetSelection( sel - 1 );
	swapItems( sel, sel - 1 );

	updateUi();
	emitChangeEvent();
}

/*****************************************************
**
**   RearrangableListCtrlBase   ---   OnDown
**
******************************************************/
void RearrangableListCtrlBase::OnDown( wxCommandEvent &event )
{
	const int sel = checklist->GetSelection();
	debuglog << "RearrangableListCtrl::OnDown int " << event.GetInt() << " selection " << sel;
	if ( sel >= (int)checklist->GetCount() - 1 )
	{
		warnlog << "Cannot move item " << sel << " up";
		return;
	}

	// store status of item at position sel-1
	const wxString tmplabel = checklist->GetString( sel + 1 );
	const bool tmpchecked = checklist->IsChecked( sel + 1 );

	// set status of sel-1 -> sel
	checklist->SetString( sel + 1, checklist->GetString( sel ));
	checklist->Check( sel + 1, checklist->IsChecked( sel ));

	// set status of sel to stored status
	checklist->SetString( sel, tmplabel );
	checklist->Check( sel, tmpchecked );

	checklist->SetSelection( sel + 1 );
	swapItems( sel, sel + 1 );

	updateUi();
	emitChangeEvent();
}

/*****************************************************
**
**   RearrangableListCtrlBase   ---   clearList
**
******************************************************/
void RearrangableListCtrlBase::clearList()
{
	assert( checklist );
	checklist->Clear();
}

/*****************************************************
**
**   RearrangableListCtrlBase   ---   updateUi
**
******************************************************/
void RearrangableListCtrlBase::updateUi()
{
	const int sel = checklist->GetSelection();
	if ( sel == -1 )
	{
		button_up->Enable( false );
		button_down->Enable( false );
	}
	else if ( sel == 0 )
	{
		button_up->Enable( false );
		button_down->Enable( true );
	}
	else if ( sel == (int)checklist->GetCount() - 1 )
	{
		button_up->Enable( true );
		button_down->Enable( false );
	}
	else
	{
		button_up->Enable( true );
		button_down->Enable( true );
	}
}

