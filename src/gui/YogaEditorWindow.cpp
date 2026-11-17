/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/YogaEditorWindow.cpp
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

#include "ChildWindow.h"
#include "Commands.h"
#include "WindowFactory.h"
#include "WidgetFactory.h"
#include "YogaEditorWidget.h"

// Mac OS X does not support menus in subwindows 
#ifndef __WXOSX__
#define USE_YOGAEDITOR_MENU
#include "MenuProvider.h"
#endif

enum { YOGAMENU_NEW = wxID_HIGHEST + 12000, YOGAMENU_OPEN, YOGAMENU_SAVE, YOGAMENU_SAVEAS };

/*************************************************//**
*
*  Editor tool for Yoga configurations
*
******************************************************/
class YogaEditorWindow: public ChildWindow
{
public:

	/*****************************************************
	**
	**   YogaEditorWindow   ---   Constructor
	**
	******************************************************/
	YogaEditorWindow( wxWindow *parent, const ViewId &viewId, const BitmapId &bmpId )
		: ChildWindow( parent, viewId, bmpId, nullptr )
	{
		WidgetFactory factory( getProps() );
		widget = factory.createWidget( this, WidgetCategory::YogaEditorWidget );

#ifdef USE_YOGAEDITOR_MENU
		wxMenuBar *menubar = new wxMenuBar( wxMB_DOCKABLE );
		wxMenu *menu = new wxMenu;
		menu->Append( YOGAMENU_NEW, tr( "New" ));
		menu->Append( YOGAMENU_OPEN, tr( "Open" ));
		menu->Append( YOGAMENU_SAVE, tr( "Save" ));
		menu->Append( YOGAMENU_SAVEAS, tr( "Save As" ));
		menu->Append( (int)CommandId::Close, tr( "Close" ));

		menubar->Append( menu, tr( "File" ));
		SetMenuBar( menubar );

		SetAcceleratorTable( wxNullAcceleratorTable );
		//SetAcceleratorTable( CommandHandler::get()->getAcceleratorTable( AcceleratorTableId::Minimal ));

		Connect( YOGAMENU_NEW, YOGAMENU_NEW+10, wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler( YogaEditorWindow::OnMenuCommand ));
#endif
	}


	/*****************************************************
	**
	**   YogaEditorWindow   ---   queryClose
	**
	******************************************************/
	virtual bool queryClose() override
	{
		debuglog << "YogaEditorWindow::queryClose";
		assert( widget );
		return widget->queryClose();
	}

private:

	/*****************************************************
	**
	**   YogaEditorWindow   ---   OnMenuCommand
	**
	******************************************************/
#ifdef USE_YOGAEDITOR_MENU
	void OnMenuCommand( wxCommandEvent &event )
	{
		YogaEditorWidget *ywidget = dynamic_cast<YogaEditorWidget*>( widget );
		assert( ywidget );
		switch( event.GetId() )
		{
			case YOGAMENU_NEW:
				ywidget->OnNewFile( event );
			break;
			case YOGAMENU_OPEN:
				ywidget->OnOpenFile( event );
			break;
			case YOGAMENU_SAVE:
				ywidget->OnSaveFile( event );
			break;
			case YOGAMENU_SAVEAS:
				ywidget->OnSaveFileAs( event );
			break;

			default:
				assert( false );
			break;
		}
	}
#endif

};

/*****************************************************
**
**   WindowFactory   ---   createYogaEditorWindow
**
******************************************************/
ChildWindow *WindowFactory::createYogaEditorWindow( wxWindow *parent, const ViewId &viewId, const BitmapId &bmpId )
{
	return new YogaEditorWindow( parent, viewId, bmpId );
}

