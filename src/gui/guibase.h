/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/guibase.h
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

#ifndef _GUIBASE_H_
#define _GUIBASE_H_

#include <wx/event.h>
#include <wx/string.h>

#include "gdibase.h"
#include "Horoscope.h"
#include "constants.h"

class wxWindow;

#define DEFAULT_DIALOG_STYLE wxRESIZE_BORDER|wxSYSTEM_MENU|wxMAXIMIZE_BOX|wxCLOSE_BOX|wxCAPTION

wxDECLARE_EVENT( MOUSE_WHEEL_UP, wxCommandEvent );
wxDECLARE_EVENT( MOUSE_WHEEL_DOWN, wxCommandEvent );

wxDECLARE_EVENT( COMMAND_SPIN_WRAP, wxCommandEvent );
wxDECLARE_EVENT( COMMAND_REARRANGECTRL_CHANGED, wxCommandEvent );

wxDECLARE_EVENT( COMMAND_DIALOGCTRL_CHANGED, wxCommandEvent );

wxDECLARE_EVENT( COMMAND_SIDEPANEL_CHANGED, wxCommandEvent );
wxDECLARE_EVENT( COMMAND_SIDEPANEL_REFRESH, wxCommandEvent );
wxDECLARE_EVENT( COMMAND_SIDEPANEL_SORT, wxCommandEvent );
wxDECLARE_EVENT( COMMAND_SIDEPANEL_OFFSET, wxCommandEvent );
wxDECLARE_EVENT( COMMAND_FORMULA_CHANGED, wxCommandEvent );
wxDECLARE_EVENT( COMMAND_FILTER_CHANGED, wxCommandEvent );

wxDECLARE_EVENT( COMMAND_CHART_SKIN_CHANGED, wxCommandEvent );
wxDECLARE_EVENT( COMMAND_ECHO_MESSAGE, wxCommandEvent );
wxDECLARE_EVENT( WIDGET_NOTIFY_PARENT, wxCommandEvent );
wxDECLARE_EVENT( WIDGET_DATA_CHANGED, wxCommandEvent );
wxDECLARE_EVENT( WIDGET_UPDATE_TITLE_QUERY, wxCommandEvent );
wxDECLARE_EVENT( APPWINDOW_CREATEVIEW, wxCommandEvent );
wxDECLARE_EVENT( ZOOM_CHANGED, wxCommandEvent );
wxDECLARE_EVENT( CONFIG_CHANGED, wxCommandEvent );
wxDECLARE_EVENT( GEOMETRY_CHANGED, wxCommandEvent );
wxDECLARE_EVENT( CONFIG_TOOLBAR_CHANGED, wxCommandEvent );
wxDECLARE_EVENT( CONFLET_CHANGED, wxCommandEvent );

wxDECLARE_EVENT( WD_WINDOW_ADDED, wxCommandEvent );
wxDECLARE_EVENT( WD_WINDOW_REMOVED, wxCommandEvent );
wxDECLARE_EVENT( WD_WINDOW_ACTIVATED, wxCommandEvent );
wxDECLARE_EVENT( WD_WINDOW_TITLE_CHANGED, wxCommandEvent );
wxDECLARE_EVENT( WD_HOROSCOPE_DATA_CHANGED, wxCommandEvent );
wxDECLARE_EVENT( WD_HOROSCOPE_SAVED, wxCommandEvent );
wxDECLARE_EVENT( WD_HOROSCOPE_LIST_CHANGED, wxCommandEvent );

int doMessageBox( wxWindow *parent, const wxString message, const long style = wxOK | wxICON_INFORMATION );

void setSavedWindowSize( wxWindow*, const wxString &configkey, const wxSize defaultSize = { 0, 0 } );
void saveWindowSize( wxWindow*, const wxString &configkey );
void setSavedWindowPosition( wxWindow *widget, const wxString &configkey );
void saveWindowPosition( wxWindow *widget, const wxString &configkey );

/**
* get the current key modifier, i.e. find out whether Alt,Ctrl and Shift keys are pressed.
*/
int getCurrentKeyMod();

/*************************************************//**
*
* 
*
******************************************************/
void doNotShowAgainMessage( wxWindow *parent, const DoNotShowAgainTopic&, const wxString&, const bool repeatPerSession = false );

void showMissingEphemDataMessage( wxWindow *parent );

/*************************************************//**
*
*  CLASS   ---   HoroscopeWrapper
*
******************************************************/
class HoroscopeWrapper  : public wxObject
{
public:
	HoroscopeWrapper( Horoscope_ptr h ) : horoscope( h ) {}

	Horoscope_ptr horoscope;
};

/**************************************************************
***
**   CLASS   ---   wxLogAppender
***
***************************************************************/
#if 0
class WxLogAppender : public LogAppender
{
public:

	virtual void writeMessage( const wxLogLevelValues&, const char *file, const int &line, const mstringstream& ) override;

  virtual void flush() override {}

};
#endif

#endif

