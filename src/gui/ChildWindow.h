/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/ChildWindow.h
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

#ifndef _CHILDWINDOW_H_
#define _CHILDWINDOW_H_

#include "bootstrap.h"

#include <wx/frame.h>

class BasicWidget;
enum class BitmapId;
enum class CommandId;
enum class ViewId;

class wxTimer;
class wxTimerEvent;

/*************************************************//**
*
* This is the central class for all children of MainWindow.
*
******************************************************/
class ChildWindow : public wxFrame
{
public:
	ChildWindow( wxWindow*, const ViewId&, const BitmapId&, Horoscope_ptr );

	~ChildWindow();

	Horoscope_ptr getHoroscope() const;

	ViewId getViewId() const { return viewId; }

	ContextProps_ptr getProps() const { return props; }

	virtual void OnDataChanged();

	/**
	* The title for display in the title bar of the window.
	* Title includes name of the Horoscope owner (if available).
	* 
	*/
	void updateTitle();

	/**
	* The main window is a special child with properties
	*  - it is owner of the Horoscope instance
	*  - closes the other children on close.
	*/
	void setMainWindow( const bool &b ) { ismainwindow = b; }
	bool isMainWindow() { return ismainwindow; }

	BitmapId getBitmapId() const { return  bmpId; }

	virtual void postCreate();

	void echo( const wxString& );

	BasicWidget *getWidget() { return widget; }

	void setWidget( BasicWidget *w ) { widget = w; }

	// public because it is called by ApplicationWindow
	virtual bool queryClose();

	void dispatchApplicationWindowCommand( const CommandId& );

	// duplicate of wxTopLevelWindow GetTitle because of nonsense return in gtk3
	wxString getTitle() const { return title; }

protected:

	virtual void zoomChanged();

	void OnKeyDown( wxKeyEvent& );
	void OnChar( wxKeyEvent& );
	void OnWidgetUpdateTitleQuery( wxCommandEvent& );

	const ViewId viewId;

	const BitmapId bmpId;

	ContextProps_ptr props;

	BasicWidget *widget = nullptr;

private:

	/**
	*  Start or stop the animation timer
	*/
	void StartAniTimer( bool b );

	/**
	*  Handle animation timer event.
	*/
	void OnAniTimer( wxTimerEvent& );

	/**
	*  Check if the Horoscope was changed and do the necessary things (ask, save, veto)
	*/
	void OnClose( wxCloseEvent &event );

	/**
	* The WindowDirectory must be notified if the window is activated by the window manager.
	*/
	void OnActivate( wxActivateEvent& );

	void OnSetFocus( wxFocusEvent& );

	void OnWidgetDataChanged( wxCommandEvent& );

	void OnCatchEchoEvent( wxCommandEvent& );

	void OnZoomChangedEvent( wxCommandEvent& ) { zoomChanged(); }

	void OnChangeZoomCommand( wxCommandEvent& );

	void dispatchViewpointCommand( const CommandId& );

	bool save();

	bool saveAs();

	void editData();

	void doPrint();

	void doQuickPrint(); 

	bool ismainwindow = false;

	wxString title;

	/**
	*  Private copy of animation interval used to check if the global interval
	*  has changed, i.e. the timer must be restarted.
	*/
	int myAniInterval = 0;

	wxTimer *anitimer = nullptr;

};

#endif // _CHILDWINDOW_H_

