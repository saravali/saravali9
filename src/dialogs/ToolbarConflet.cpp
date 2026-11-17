/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ToolbarConflet.cpp
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

#include "ToolbarConflet.h"

#include "Commands.h"
#include "CommonControls.h"
#include "guibase.h"
#include "ImageProvider.h"
#include "logger.h"
#include "ToolPanel.h"

#include <wx/bmpbuttn.h>
#include <wx/choice.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>

#include <format>

extern Gdiconfig *gdiconfig;

enum { CD_LEFT = wxID_HIGHEST + 1, CD_RIGHT, CD_UP, CD_DOWN, CD_LIST1, CD_LIST2 };

// begin wxGlade: ::extracode
// end wxGlade

/*****************************************************
**
**   ToolbarConflet   ---   Constructor
**
******************************************************/
ToolbarConflet::ToolbarConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->toolbar )
{
	// begin wxGlade: ToolbarConflet::ToolbarConflet
	wxFlexGridSizer* sizer_main = new wxFlexGridSizer(2, 1, 3, 3);
	wxFlexGridSizer* sizer_listbox = new wxFlexGridSizer(1, 3, 3, 3);
	sizer_main->Add(sizer_listbox, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_available = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Available Items")), wxVERTICAL);
	sizer_listbox->Add(sizer_available, 1, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* sizer_available2 = new wxFlexGridSizer(1, 1, 0, 0);
	sizer_available->Add(sizer_available2, 1, wxALL|wxEXPAND, 3);
	list1 = new wxListCtrl(sizer_available->GetStaticBox(), CD_LIST1, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL);
	list1->SetMinSize(wxSize(200,250));
	sizer_available2->Add(list1, 1, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* sizer_buttons = new wxFlexGridSizer(4, 1, 3, 3);
	sizer_listbox->Add(sizer_buttons, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND|wxSHAPED, 3);
	button_right = new wxBitmapButton(this, CD_RIGHT, wxNullBitmap);
	button_right->SetMinSize(wxSize(32,32));
	button_right->SetToolTip(_("Move selected item to toolbar"));
	sizer_buttons->Add(button_right, 0, wxALL|wxEXPAND, 3);
	button_left = new wxBitmapButton(this, CD_LEFT, wxNullBitmap);
	button_left->SetMinSize(wxSize(32,32));
	button_left->SetToolTip(_("Remove selected item from toolbar"));
	sizer_buttons->Add(button_left, 0, wxALL|wxEXPAND, 3);
	button_up = new wxBitmapButton(this, CD_UP, wxNullBitmap);
	button_up->SetMinSize(wxSize(32,32));
	button_up->SetToolTip(_("Move selected item up"));
	sizer_buttons->Add(button_up, 0, wxALL|wxEXPAND, 3);
	button_down = new wxBitmapButton(this, CD_DOWN, wxNullBitmap);
	button_down->SetMinSize(wxSize(32,32));
	button_down->SetToolTip(_("Move selected item down"));
	sizer_buttons->Add(button_down, 0, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_selected = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Items in Toolbar")), wxVERTICAL);
	sizer_listbox->Add(sizer_selected, 1, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* sizer_selected2 = new wxFlexGridSizer(1, 1, 0, 0);
	sizer_selected->Add(sizer_selected2, 1, wxALL|wxEXPAND, 3);
	list2 = new wxListCtrl(sizer_selected->GetStaticBox(), CD_LIST2, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL);
	list2->SetMinSize(wxSize(200,250));
	sizer_selected2->Add(list2, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_options = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Toolbar Options")), wxHORIZONTAL);
	sizer_main->Add(sizer_options, 1, wxEXPAND, 0);
	label_toolstyle = new wxStaticText(sizer_options->GetStaticBox(), wxID_ANY, _("Show Tools as"));
	sizer_options->Add(label_toolstyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_toolstyle_choices[] = {
		_("Text"),
		_("Icon"),
		_("Text and Icon"),
	};
	choice_toolstyle = new wxChoice(sizer_options->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_toolstyle_choices);
	choice_toolstyle->SetSelection(0);
	sizer_options->Add(choice_toolstyle, 0, wxALL, 3);
	
	sizer_selected2->AddGrowableRow(0);
	sizer_selected2->AddGrowableCol(0);
	sizer_available2->AddGrowableRow(0);
	sizer_available2->AddGrowableCol(0);
	sizer_listbox->AddGrowableRow(0);
	sizer_listbox->AddGrowableCol(0);
	sizer_listbox->AddGrowableCol(2);
	sizer_main->AddGrowableRow(0);
	sizer_main->AddGrowableCol(0);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	const int w = 150;
	ImageProvider *ip = ImageProvider::get();

	wxListItem itemCol;
	itemCol.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE;
	itemCol.m_text = _( "Type" );
	list1->InsertColumn(0, itemCol);
	list1->SetColumnWidth( 0, w );

	list2->InsertColumn(0, itemCol);
	list2->SetColumnWidth( 0, w );

	button_right->SetBitmapLabel( ip->getBitmapBundle( BitmapId::ArrowRight ));
	button_left->SetBitmapLabel( ip->getBitmapBundle( BitmapId::ArrowLeft ));
	button_up->SetBitmapLabel( ip->getBitmapBundle( BitmapId::ArrowUp ));
	button_down->SetBitmapLabel( ip->getBitmapBundle( BitmapId::ArrowDown ));

	choice_toolstyle->SetValidator( wxGenericValidator( &cfg.toolbarStyle ));

	initToolItems();
	initRightControl();
	updateUi();

	Connect( CD_LEFT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolbarConflet::OnLeft ));
	Connect( CD_RIGHT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolbarConflet::OnRight ));
	Connect( CD_UP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolbarConflet::OnUp ));
	Connect( CD_DOWN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolbarConflet::OnDown ));

	Connect( CD_LIST1, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ToolbarConflet::OnLeftSelected ));
	Connect( CD_LIST2, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ToolbarConflet::OnRightSelected ));
}

/*****************************************************
**
**   ToolbarConflet   ---   Destructor
**
******************************************************/
ToolbarConflet::~ToolbarConflet()
{
	delete imageList;
}

/*****************************************************
**
**   ToolbarConflet   ---   initToolItems
**
******************************************************/
void ToolbarConflet::initToolItems()
{
	uint i;
	ToolbarFactory *factory = ToolbarFactory::get();
	CommandHandler *handler = CommandHandler::get();
	ImageProvider *ip = ImageProvider::get();
	MCommand *command;

	std::vector<CommandId> fullCommandIds = factory->getFullCommandIdList();

	//const int size = ip->getDefaultBitmapSize();
	const int size = 24;
	imageList = new wxImageList( size, size, true );
	imageList->Add( ip->getBitmap( BitmapId::Separator, IconSize::Medium ));

	list1->SetImageList( imageList, wxIMAGE_LIST_SMALL );
	list2->SetImageList( imageList, wxIMAGE_LIST_SMALL );
	imageListIds.clear();

	// setup left list
	list1->DeleteAllItems();

	// insert separator
	wxListItem item;
	item.SetId( 0 );
	item.SetImage( 0 );
	item.SetText( _( "Separator" ));
	list1->InsertItem( item );

	for ( i = 0; i < fullCommandIds.size(); i++ )
	{
		command = handler->getCommand( fullCommandIds[i] );
		imageList->Add( ip->getBitmap( command->bmpId, IconSize::Medium ));
		imageListIds[ command->bmpId ] = i + 1;

		wxListItem item;
		item.SetId( i + 1 );
		item.SetData( (int)fullCommandIds[i] );
		item.SetImage( i + 1 );
		item.SetText( command->text );
		list1->InsertItem( item );
	}

	list1->SetColumnWidth( 0, wxLIST_AUTOSIZE );
	list1->Arrange();
	left_sel = 0;
	list1->SetItemState( 0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

/*****************************************************
**
**   ToolbarConflet   ---   updateItemConfig
**
******************************************************/
void ToolbarConflet::updateItemConfig()
{
	long commandId;
	cfg.mainToolbarItems.clear();
	for( long l = 0; l < list2->GetItemCount(); l++ )
	{
		commandId = list2->GetItemData( l );
		cfg.mainToolbarItems.push_back( (CommandId)commandId );
	}
}

/*****************************************************
**
**   ToolbarConflet   ---   initRightControl
**
******************************************************/
void ToolbarConflet::initRightControl()
{
	ToolbarFactory *factory = ToolbarFactory::get();

	if ( cfg.mainToolbarItems.size() == 0 )
	{
		cfg.mainToolbarItems = factory->getDefaultCommandIdList();
	}

	list2->DeleteAllItems();
	for( uint i = 0; i < cfg.mainToolbarItems.size(); i++ )
	{
		insertRightItemById( cfg.mainToolbarItems[i], i, false );
	}

	assert( list2->GetItemCount() > 0 );
	list2->SetItemState( 0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	right_sel = 0;
	list2->SetColumnWidth( 0, wxLIST_AUTOSIZE );
	list2->Arrange();
}

/*****************************************************
**
**   ToolbarConflet   ---   OnLeft
**
******************************************************/
void ToolbarConflet::OnLeft( wxCommandEvent& )
{
	if ( right_sel > -1 )
	{
		list2->DeleteItem( right_sel );
		const int new_sel = std::max( 0, right_sel - 1 );
		list2->SetItemState( new_sel, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
		list2->EnsureVisible( new_sel );
		list2->Arrange();
		updateItemConfig();
		guiHasChanged();
	}
}

/*****************************************************
**
**   ToolbarConflet   ---   OnRight
**
******************************************************/
void ToolbarConflet::OnRight( wxCommandEvent& )
{
	if ( left_sel < 0 || right_sel < 0 )
	{
		errorlog << "ToolbarConflet::OnRight ERROR: wrong selection left n" << left_sel << " right " << right_sel;
		return;
	}
	const CommandId commandId = (CommandId)list1->GetItemData( left_sel );

	for( long l = 0; l < list2->GetItemCount(); l++ )
	{
		if ( commandId != CommandId::None && (CommandId)list2->GetItemData( l ) == commandId )
		{
			wxString message = _( "Item already in toolbar at position" ) + " " + std::format( "{}", l + 1 );
			doMessageBox( this, message, wxICON_ERROR | wxOK );
			return;
		}
	}

	insertRightItemById( commandId, right_sel + 1 );
	updateItemConfig();
	guiHasChanged();
}

/*****************************************************
**
**   ToolbarConflet   ---   insertRightItemById
**
******************************************************/
void ToolbarConflet::insertRightItemById( const CommandId &commandId, const int &pos, const bool doarrange )
{
	CommandHandler *handler = CommandHandler::get();
	MCommand *command = handler->getCommand( commandId );

	wxListItem item;
	item.SetId( pos );
		item.SetData( (int)commandId );

	if ( command )
	{
		item.SetImage( imageListIds[ command->bmpId ] );
		item.SetText( command->text );
	}
	else
	{
		item.SetImage( 0 );
		item.SetText( _( "Separator" ));
	}
	list2->InsertItem( item );

	if ( doarrange )
	{
		list2->SetItemState( pos, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
		list2->EnsureVisible( pos );
		list2->Arrange();
	}
}

/*****************************************************
**
**   ToolbarConflet   ---   OnUp
**
******************************************************/
void ToolbarConflet::OnUp( wxCommandEvent& )
{
	if ( right_sel > 0 )
	{
		const CommandId commandId = (CommandId)list2->GetItemData( right_sel );
		list2->DeleteItem( right_sel );
		insertRightItemById( commandId, right_sel - 1 );
		updateItemConfig();
		guiHasChanged();
	}
}

/*****************************************************
**
**   ToolbarConflet   ---   OnDown
**
******************************************************/
void ToolbarConflet::OnDown( wxCommandEvent& )
{
	if ( right_sel < list2->GetItemCount() - 1 )
	{
		const CommandId commandId = (CommandId)list2->GetItemData( right_sel );
		list2->DeleteItem( right_sel );
		insertRightItemById( commandId, right_sel + 1 );
		updateItemConfig();
		guiHasChanged();
	}
}

/*****************************************************
**
**   ToolbarConflet   ---   OnRightSelected
**
******************************************************/
void ToolbarConflet::OnRightSelected( wxListEvent& event )
{
	right_sel = event.GetIndex();
	long l = list2->GetItemCount();
	button_up->Enable( right_sel > 0 );
	button_down->Enable( right_sel < l - 1 );
}

/*****************************************************
**
**   ToolbarConflet   ---   OnLeftSelected
**
******************************************************/
void ToolbarConflet::OnLeftSelected( wxListEvent& event )
{
	left_sel = event.GetIndex();
}

/*****************************************************
**
**   ToolbarConflet   ---   model2config
**
******************************************************/
void ToolbarConflet::model2config()
{
	debuglog << "ToolbarConflet::model2config";
	globalConfig = cfg;
	propagateEvent( CONFIG_TOOLBAR_CHANGED );
}

/*****************************************************
**
**   ToolbarConflet   ---   restoreDefaults
**
******************************************************/
void ToolbarConflet::restoreDefaults()
{
	cfg = ToolbarConfig();
	cfg.mainToolbarItems = ToolbarFactory::get()->getDefaultCommandIdList();
	initRightControl();
}

