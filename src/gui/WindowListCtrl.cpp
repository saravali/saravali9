/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/WindowListCtrl.cpp
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

#include "WindowListCtrl.h"

#include "ChildWindow.h"
#include "ColorConfig.h"
#include "Commands.h"
#include "DateFormatter.h"
#include "datelocation.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "ImageProvider.h"
#include "Lang.h"
#include "logger.h"
#include "MenuProvider.h"
#include "WindowDirectory.h"

#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/string.h>

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   CLASS MainWindowListEventHandler
**
******************************************************/
class MainWindowListEventHandler : public wxEvtHandler
{
public:
	MainWindowListEventHandler( WindowListCtrl *listctrl ) : listctrl( listctrl ) {}

private:
	virtual void OnContextMenuEvent( wxMouseEvent& event)
	{
		listctrl->OnRightDown( event );
	}

	WindowListCtrl *listctrl;

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE( MainWindowListEventHandler, wxEvtHandler)
	EVT_RIGHT_DOWN( MainWindowListEventHandler::OnContextMenuEvent )
END_EVENT_TABLE()

/*****************************************************
**
**   WindowListCtrl   ---   Constructor
**
******************************************************/
WindowListCtrl::WindowListCtrl( wxWindow *parent ) : wxWindow( parent, -1 )
{
	listctrl = nullptr;
	activeitem = -1;
	WindowDirectory::get()->subscribe( this );

	Connect( wxEVT_SIZE, wxSizeEventHandler( WindowListCtrl::OnSize ));
	Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( WindowListCtrl::OnListItemActivated ));
	//Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( WindowListCtrl::OnListItemSelected ));

	Connect( wxID_ANY, WD_WINDOW_ADDED, wxCommandEventHandler( WindowListCtrl::OnWindowAdded ));
	Connect( wxID_ANY, WD_WINDOW_REMOVED, wxCommandEventHandler( WindowListCtrl::OnWindowRemoved ));
	Connect( wxID_ANY, WD_WINDOW_ACTIVATED, wxCommandEventHandler( WindowListCtrl::OnWindowActivated ));

	Connect( wxID_ANY, WD_WINDOW_TITLE_CHANGED, wxCommandEventHandler( WindowListCtrl::OnWindowTitleChanged ));
	Connect( wxID_ANY, WD_HOROSCOPE_DATA_CHANGED, wxCommandEventHandler( WindowListCtrl::OnHoroscopeChanged ));

	recreate();
}

/*****************************************************
**
**   WindowListCtrl   ---   Destructor
**
******************************************************/
WindowListCtrl::~WindowListCtrl()
{
	WindowDirectory::get()->unsubscribe( this );
	listctrl->PopEventHandler( true );
}

/*****************************************************
**
**   WindowListCtrl   ---   recreate
**
******************************************************/
void WindowListCtrl::recreate()
{
	wxListItem itemCol;

	// delete old listctrl after creating the new one
	// this is a bug of 7.0.1 on GTK (GTK event loop crash on style change)
	wxListView *old_listctrl = listctrl;

	if ( false && listctrl )
	{
		listctrl->PopEventHandler( true );
		delete listctrl;
	}
	int width = GetParent()->GetSize().GetWidth() / 4 - 2;
	switch ( gdiconfig->prefs.listStyle )
	{
		case 1: // small icons
			listctrl = new wxListView( this, -1, wxDefaultPosition, wxDefaultSize,
				wxLC_SMALL_ICON|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
			listctrl->AssignImageList( createViewImageList( IconSize::Small ), wxIMAGE_LIST_SMALL );
		break;
		case 2: // report view
			listctrl = new wxListView( this, -1, wxDefaultPosition, wxDefaultSize,
				wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
			listctrl->AssignImageList( createViewImageList( IconSize::Small ), wxIMAGE_LIST_SMALL );

			itemCol.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE;
			itemCol.m_text = _( "Type" );
			listctrl->InsertColumn(0, itemCol);
			listctrl->SetColumnWidth( 0, width );

			itemCol.m_text = _( "Name" );
			listctrl->InsertColumn(1, itemCol);
			listctrl->SetColumnWidth( 1, width );

			itemCol.m_text = _( "Date" );
			listctrl->InsertColumn(2, itemCol);
			listctrl->SetColumnWidth( 2, width );

			itemCol.m_text = _( "Location" );
			listctrl->InsertColumn(3, itemCol);
			listctrl->SetColumnWidth( 3, width );
		break;
		default: // large icons
			listctrl = new wxListView( this, -1, wxDefaultPosition, wxDefaultSize,
				wxLC_ICON|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
			listctrl->AssignImageList( createViewImageList( IconSize::Large ), wxIMAGE_LIST_NORMAL );
		break;
	}
	assert( listctrl );

	for( auto *window : WindowDirectory::get()->getWindows())
  {
    assert( window );
		appendListItem( window );
  }

	listctrl->SetSize( GetSize() );
	listctrl->Arrange();
	if ( activeitem != -1 ) selectItem( activeitem );
	listctrl->PushEventHandler( new MainWindowListEventHandler( this ));
	listctrl->Show( true );

	listctrl->SetBackgroundColour( gdiconfig->colors.bgColor );
	listctrl->SetForegroundColour( gdiconfig->colors.getDefaultFgColour() );

	if ( old_listctrl )
	{
		old_listctrl->PopEventHandler( true );
		delete old_listctrl;
	}
	wxFlexGridSizer* sizer = new wxFlexGridSizer(1, 1, 0, 0);
	sizer->AddGrowableRow(0);
	sizer->AddGrowableCol(0);
	//wxSizer *sizer = new wxBoxSizer( wxVERTICAL );
	//wxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
	sizer->Add( listctrl, wxEXPAND, 3 );
	SetSizer( sizer );
	Layout();
}

/*****************************************************
**
**   WindowListCtrl   ---   updateItemLabel
**
******************************************************/
void WindowListCtrl::updateItemLabel( ChildWindow *window )
{
	assert( window );
	WindowDirectory *directory = WindowDirectory::get();
	const int pos = directory->getIndexForWindow( window );
	if ( pos != -1 ) updateItemLabel( window, pos );
	else errorlog << "WindowListCtrl::updateItemLabel index for window " << window->getTitle() << " cannot be found";
}

/*****************************************************
**
**   WindowListCtrl   ---   updateItemLabel
**
******************************************************/
void WindowListCtrl::updateItemLabel( ChildWindow *child, const int &pos )
{
	assert( child );
	DateFormatter *formatter = DateFormatter::get();
	Horoscope_ptr horoscope = child->getHoroscope();
	const wxString label = child->getTitle();

	debuglog << "WindowListCtrl::updateItemLabel label " << label << " pos " << pos;


	if ( gdiconfig->prefs.listStyle == 2 ) // report view with multiple columns
	{
		listctrl->SetItem( pos, 0, label );
		if ( horoscope )
		{
			listctrl->SetItem( pos, 1, horoscope->dl.name );
			const Location &loc = horoscope->dl.location;
			listctrl->SetItem( pos, 2,
				formatter->formatUTDatetime( horoscope->getJD() + ( loc.tz + loc.dst )/24 ) );
			listctrl->SetItem( pos, 3, loc.locName );
		}
	}
	else // list control with small or large icons
	{
		wxString name;
		if ( horoscope ) name = horoscope->dl.name;
		wxString s;

		if( child->isMainWindow())
		{
			assert( horoscope );
			if ( name.size() > 12 )
			{
				s = name.substr( 0, 11 ) + "...";
			}
			else s = name;
		}
		else
		{
#if 0
			if ( horoscope )
			{
				if ( name.size() > 10 )
				{
					s = name.substr( 0, 10 ) + "..." + label;
				}
				else
				{
					s = name + " <" + label + ">";
				}
			}
			else // Simplechild without doc
			{
				s = label;
			}
#else
			s = label;
#endif
		}
		listctrl->SetItemText( pos, s );
	}
}

/*****************************************************
**
**   WindowListCtrl   ---   appendListItem
**
******************************************************/
void WindowListCtrl::appendListItem( ChildWindow *item )
{
	const BitmapId bid = item->getBitmapId();
	int i = 0;
	if ( bid >= BitmapId::ViewImagelistFirst && bid <= BitmapId::ViewImagelistLast )
	{
		i = (int)bid - (int)BitmapId::ViewImagelistFirst;
	}
	else
	{
		i = (int)BitmapId::ViewImagelistLast - (int)BitmapId::ViewImagelistFirst;
	}

	int pos = listctrl->InsertItem( listctrl->GetItemCount(), "--", i );
	updateItemLabel( item, pos );
	selectItem( pos );
}

/*****************************************************
**
**   WindowListCtrl   ---   getSelectedItem
**
******************************************************/
int WindowListCtrl::getSelectedItem() const
{
	for( int pos = 0; pos < listctrl->GetItemCount(); ++pos )
	{
		if ( listctrl->GetItemState( (long)pos, wxLIST_STATE_SELECTED ) == wxLIST_STATE_SELECTED )
		{
			return pos;
		}
	}
	return -1;
}

/*****************************************************
**
**   WindowListCtrl   ---   setSelectedItem
**
******************************************************/
/*
void WindowListCtrl::setSelectedItem( const int &pos )
{
	//listctrl->SetItemState( pos, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	listctrl->Select( pos, true );
}
*/

/*****************************************************
**
**   WindowListCtrl   ---   createViewImageList
**
******************************************************/
wxImageList *WindowListCtrl::createViewImageList( const IconSize &size )
{
	int wxsize = 16;
	switch( size )
	{
		case IconSize::Medium:
			wxsize = 24;
		break;
		case IconSize::Large:
			wxsize = 32;
		break;
		default:
		break;
	}
	wxImageList *imageList = new wxImageList( wxsize, wxsize );
	ImageProvider *ip = ImageProvider::get();

	for ( const BitmapId id : Enum<BitmapId>( BitmapId::ViewImagelistFirst, BitmapId::ViewImagelistLast ))
	{
		imageList->Add( ip->getBitmap( id, size ));
	}
	return imageList;
}

/*****************************************************
**
**   WindowListCtrl   ---   onConfigChanged
**
******************************************************/
void WindowListCtrl::onConfigChanged()
{
	// colors may have changed
	listctrl->SetBackgroundColour( gdiconfig->colors.bgColor );
	listctrl->SetForegroundColour( gdiconfig->colors.getDefaultFgColour() );
	//recreate();
}

/*****************************************************
**
**   WindowListCtrl   ---   OnSize
**
******************************************************/
void WindowListCtrl::OnSize( wxSizeEvent &event )
{
	debuglog << "WindowListCtrl::OnSize";
	listctrl->SetSize( event.GetSize() );
	listctrl->Arrange();
}

/*****************************************************
**
**   WindowListCtrl   ---   selectItem
**
******************************************************/
void WindowListCtrl::selectItem( const int &i )
{
	if ( activeitem == i ) return;
	if ( activeitem != -1 ) listctrl->Select( activeitem, false );
	activeitem = i;
	listctrl->Select( i, true );
}

/*****************************************************
**
**   WindowListCtrl   ---   OnListItemActivated
**
******************************************************/
void WindowListCtrl::OnListItemActivated( wxListEvent& event )
{
	debuglog << "WindowListCtrl::OnListItemActivated index " << event.GetIndex();
	WindowDirectory *directory = WindowDirectory::get();

	const int count = directory->getWindowCount();
	if ( event.GetIndex() >= count )
	{
		errorlog << "WindowListCtrl::OnListItemActivated: wrong index " << event.GetIndex()
			<< " item count is " << count;
		return;
	}
	ChildWindow *window = directory->getWindow( event.GetIndex());
	assert( window );
	window->Raise();
}

/*****************************************************
**
**   WindowListCtrl   ---   OnListItemSelected
**
******************************************************/
void WindowListCtrl::OnListItemSelected( wxListEvent& event )
{
	debuglog << "WindowListCtrl::OnListItemSelected index " << event.GetIndex();
	WindowDirectory *directory = WindowDirectory::get();
	const int count = directory->getWindowCount();
	if ( event.GetIndex() >= count )
	{
		errorlog << "WindowListCtrl::OnListItemSelected: wrong index " << event.GetIndex()
			<< " item count is " << count;
		return;
	}
	return;
	ChildWindow *window = directory->getWindow( event.GetIndex());
	assert( window );
	directory->setActiveWindow( window );
}

/*****************************************************
**
**   WindowListCtrl   ---   OnRightDown
**
******************************************************/
void WindowListCtrl::OnRightDown( wxMouseEvent &event )
{
	int flag = wxLIST_HITTEST_ONITEM;
	wxMenu *menu = nullptr;
	WindowDirectory *directory = WindowDirectory::get();

	const int i = (int)listctrl->HitTest( event.GetPosition(), flag );
	debuglog << "WindowListCtrl::OnRightDown event point " << event.GetPosition().x << ":" << event.GetPosition().y
		<< " item index " << i;

	if ( i == -1 )
	{
		menu = ContextMenuProvider().getAppWindowContextMenu();
	}
	else
	{
		selectItem( i );
		const int count = directory->getWindowCount();
		if ( i >= count )
		{
			errorlog << "WindowListCtrl::OnListItemActivated: wrong index " << i
				<< " item count is " << count;
			return;
		}
		ChildWindow *window = directory->getWindow( i );
		assert( window );
		debuglog << "WindowListCtrl::OnRightDown window is " << window->getTitle();
		menu = ContextMenuProvider().getChildWindowListCtrlMenu( window );
	}
	PopupMenu( menu, event.GetX(), event.GetY() );
	delete menu;
}

/*****************************************************
**
**   WindowListCtrl   ---   OnWindowAdded
**
******************************************************/
void WindowListCtrl::OnWindowAdded( wxCommandEvent &event )
{
	const int index = (int)event.GetExtraLong();
	debuglog << "WindowListCtrl::OnWindowAdded index " << index;
	ChildWindow *window = WindowDirectory::get()->getWindow( index );
	if ( window ) appendListItem( window );
}

/*****************************************************
**
**   WindowListCtrl   ---   OnWindowRemoved
**
******************************************************/
void WindowListCtrl::OnWindowRemoved( wxCommandEvent &event )
{
	const long index = event.GetExtraLong();
	debuglog << "WindowListCtrl::OnWindowRemoved " << index;

	if ( index >= 0 && index < listctrl->GetItemCount())
	{
		listctrl->DeleteItem( index );
		const int newsel = index > 0 ? index - 1 : 0;
		if ( listctrl->GetItemCount() > 0 )
		{
			selectItem( newsel );
			listctrl->Arrange();
			WindowDirectory::get()->setActiveWindow( newsel );
		}
	}
	else
	{
		errorlog << "WindowListCtrl::OnWindowRemoved: cannot delete window " << index << ", size " << listctrl->GetItemCount();
	}
}

/*****************************************************
**
**   WindowListCtrl   ---   OnWindowActivated
**
******************************************************/
void WindowListCtrl::OnWindowActivated( wxCommandEvent &event )
{
	const long index = event.GetExtraLong();
	debuglog << "WindowListCtrl::OnWindowActivated " << index;
	if ( index >= 0 && index < listctrl->GetItemCount())
	{
		selectItem( index );
	}
}

/*****************************************************
**
**   WindowListCtrl   ---   OnHoroscopeChanged
**
******************************************************/
void WindowListCtrl::OnHoroscopeChanged( wxCommandEvent &event )
{
	WindowDirectory *directory = WindowDirectory::get();
	const long index = event.GetExtraLong();
	debuglog << "WindowListCtrl::OnHoroscopeChanged index" << index;

	Horoscope_ptr h = directory->getHoroscope( index );
	if ( h )
	{
		for( ChildWindow *window : directory->getWindowsForHoroscope( h ))
		{
			const int pos = directory->getIndexForWindow(  window );
			updateItemLabel( window, pos );
		}
	}
	else
	{
		errorlog << "WindowListCtrl::OnHoroscopeChanged found no horoscope at position " << index;
	}
}

/*****************************************************
**
**   WindowListCtrl   ---   OnWindowTitleChanged
**
******************************************************/
void WindowListCtrl::OnWindowTitleChanged( wxCommandEvent &event )
{
	const long pos = event.GetExtraLong();
	debuglog << "WindowListCtrl::OnWindowTitleChanged index " << pos;

	if ( pos >= 0 && pos < listctrl->GetItemCount())
	{
		ChildWindow *window = WindowDirectory::get()->getWindow( pos );
		if ( window )
		{
			updateItemLabel( window, pos );
			return;
		}
	}
	errorlog << "WindowListCtrl::OnWindowTitleChanged cannot change title at index " << pos << ", size is " << listctrl->GetItemCount();
}

