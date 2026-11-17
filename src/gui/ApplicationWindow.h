/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/ApplicationWindow.h
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

#ifndef _APPLICATIONWINDOW_H_
#define _APPLICATIONWINDOW_H_

#include "bootstrap.h"

#include <wx/frame.h>

// workaround for macOS
#if defined( __WXMAC__ ) || defined( __WXMSW__ )
#include <wx/infobar.h>
#endif

class AppMenuBar;
class AppWindowDropTarget;
class ChildWindow;
class MainWindowStatusbar;
class WindowListCtrl;
enum class CommandId;

class wxEvent;
class wxListEvent;
class wxLogWindow;

/*************************************************//**
*
*  Main window of application
*
******************************************************/
class ApplicationWindow : public wxFrame
{
public:

	ApplicationWindow( const wxString &title, const wxPoint&, const wxSize& );

	~ApplicationWindow();
	void NewFile( const bool &useMultipleView, const int viewId = 0 );
	bool OpenFile( wxString );
	void CreateNewWindow( Horoscope_ptr );

protected:

	void addWindow( ChildWindow* );
	void OnOpenRecent( wxCommandEvent& );
	void OnFileNew( wxCommandEvent& );
	void OnFileOpen( wxCommandEvent& );
	void OnFileQuit( wxCommandEvent& );
	void OnNewWindow( wxCommandEvent& );
	void createNewWindow( const CommandId& );

	void doCreateNew( Horoscope_ptr horoscope, const bool &useMultipleView, const int mViewId = 0  );

	void OnChildCommand( wxCommandEvent& );
	void OnCreateWindowFromExternal( wxCommandEvent& );

	void OnSaveConfig( wxCommandEvent& );
	void OnConfigurationDialog( wxCommandEvent& );

	void OnLocationDialog( wxCommandEvent& );
	void OnImportLocationDialog( wxCommandEvent& );
	void OnAppAbout( wxCommandEvent& );
	void OnAppHelp( wxCommandEvent& );
	void OnYearlyHoroscope( wxCommandEvent& );
	void OnClose( wxCloseEvent& );

	// status bar related
	void showStatusBar();
	virtual wxStatusBar* OnCreateStatusBar( int number, long style, wxWindowID id, const wxString& name ) override;
	void OnToggleStatusBar( wxCommandEvent& );

	// log window related
	void showLogWindow( const bool &doShow );
	void OnToggleLogWindow( wxCommandEvent& );
	//void OnLogMessage( wxCommandEvent& );

	// toolbar related
	void OnToggleToolBars( wxCommandEvent& );
	void OnToolbarConfigChanged( wxCommandEvent& );
	void recreateToolbar();

	void OnToggleIcons( wxCommandEvent& );
	void OnConfigChanged( wxCommandEvent& );

	void OnDropFilesEvent( wxDropFilesEvent& );
	void prepareSaveConfig();
	void OnIdle( wxIdleEvent& );

	// recent files/extra menu and dropdown lazy loading
	void OnAppMenuOpen( wxMenuEvent& );
	void onDropdown( wxMenuEvent& );

	void OnWindowActivated( wxCommandEvent& );
	void OnHoroscopeSaved( wxCommandEvent& );

	void OnKeyDown( wxKeyEvent& );
	void OnChar( wxKeyEvent& );

	MainWindowStatusbar *statusbar = nullptr;
	WindowListCtrl *listctrl = nullptr;
	AppMenuBar *menubar = nullptr;
	AppWindowDropTarget *droptarget = nullptr;

	wxLogWindow *logWindow = nullptr;

	bool docommands, updateTools;
};

#endif

