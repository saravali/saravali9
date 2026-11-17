/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/MenuProvider.h
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

#ifndef _MENUPROVIDER_H_
#define _MENUPROVIDER_H_

#include "bootstrap.h"
#include "constants.h"

#include <wx/menu.h>
#include <wx/string.h>
#include <deque>
#include <list>

class ChildWindow;
class BasicWidget;
class WidgetContext;
enum class CommandId;

/*************************************************//**
*
*  specific menu class, transporting View details
*
******************************************************/
class MyMenu : public wxMenu
{
public:

	MyMenu( const wxString title = wxEmptyString );

	wxMenuItem *addItem( const CommandId& );
	void addCheckItem( const CommandId&, const bool checked = false );
	void addCheckItem( const CommandId&, const wxString title, const bool checked = false );

	void addItemList( const std::list<CommandId>& );

	void addWindowListMenu();
	void addVargaMenu();

	void enable( const CommandId&, const bool &doEnable );
	void check( const CommandId&, const bool &doEnable );

	void append( const CommandId&, const wxString& );

private:
	class CommandHandler *handler;
};


/*************************************************//**
*
*  main menu bar of the appliation
*
******************************************************/
class AppMenuBar : public wxMenuBar
{
public:

	AppMenuBar( const int &style );
	~AppMenuBar();

	void updateMenus( ChildWindow* );
	void OnOpenMenu( wxMenu* );

	void addToRecentFiles( const wxString& );
	wxString getRecentFilesEntry( const int& );
	void saveRecentFiles();

private:

	void OnUpdateMenus( wxCommandEvent& );

	void onMViewMenuOpen( wxMenuEvent& );

	MyMenu *filemenu, *horoscopemenu, *westernmenu, *vedicmenu,
		*extramenu, *windowmenu, *helpmenu, *recentmenu, *newmenu;
	wxArrayString recentfiles;
	wxMenuItem *newDefaultItem;

	std::deque<wxString> recent_files;
};

/*************************************************//**
*
*  creates the context menu according to view options
*
******************************************************/
class ContextMenuProvider : public wxObject
{
public:
	MyMenu *getWidgetMenu( BasicWidget* );

	MyMenu *getChildWindowListCtrlMenu( ChildWindow* );
	MyMenu *getAppWindowContextMenu();

	MyMenu *getToolbarNewMenu();

private:

	MyMenu *getSkinMenu( ContextProps_ptr, const WidgetCategory&  );
	MyMenu *getGeomMenu( ContextProps_ptr, const WidgetCategory&  );

	MyMenu *getPlanetListMenu( BasicWidget* );
	MyMenu *getColumnGroupMenu( ContextProps_ptr );

	void appendExportMenu( MyMenu*, const WidgetCategory& );

	void appendSizeMenu( MyMenu*, const WidgetCategory& );

	MyMenu *getNewViewMenu( ContextProps_ptr );

};


#endif


