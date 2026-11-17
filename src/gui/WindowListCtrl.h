/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/WindowListCtrl.h
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
#ifndef _WINDOWLISTCTRL_H_
#define _WINDOWLISTCTRL_H_

#include "guibase.h"

#include <wx/window.h>

class ChildWindow;
class wxImageList;
class wxListView;
class wxListEvent;
class wxSizeEvent;

/*************************************************//**
*
*  list control for main window supporting icons and several view types
*
******************************************************/
class WindowListCtrl : public wxWindow
{
	friend class MainWindowListEventHandler;

public:

	WindowListCtrl( wxWindow* parent );

	~WindowListCtrl();

	/**
	*  Internal list control will be recreated on style change (big/small icons or report view)
	*/
	void recreate();

	/**
	*  Handler for global config change. currently only color change.
	*/
	void onConfigChanged();

private:

	// TODO required?
	void selectItem( const int& );
	void OnRightDown( wxMouseEvent &event );

	int activeitem;
	wxListView *listctrl;

	void appendListItem( ChildWindow* );

	void updateItemLabel( ChildWindow* );
	void updateItemLabel( ChildWindow*, const int &pos );

	int getSelectedItem() const;

	//void setSelectedItem( const int& );

	wxImageList *createViewImageList( const IconSize& );

	void OnSize( wxSizeEvent& );
	void OnListItemActivated( wxListEvent& );
	void OnListItemSelected( wxListEvent& );
	void OnKeyDown( wxKeyEvent& );

	void OnWindowAdded( wxCommandEvent& );

	void OnWindowRemoved( wxCommandEvent& );

	void OnWindowActivated( wxCommandEvent& );

	void OnHoroscopeChanged( wxCommandEvent& );

	void OnWindowTitleChanged( wxCommandEvent& );

};


#endif // _WINDOWLISTCTRL_H_
