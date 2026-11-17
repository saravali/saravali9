/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/guibase.cpp
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

#include "guibase.h"

#include "enumserializer.h"
#include "gdiconfig.h"
#include "logger.h"

#include <wx/msgdlg.h>
#include <wx/richmsgdlg.h>

extern Gdiconfig *gdiconfig;

wxDEFINE_EVENT( MOUSE_WHEEL_UP, wxCommandEvent );
wxDEFINE_EVENT( MOUSE_WHEEL_DOWN, wxCommandEvent );

wxDEFINE_EVENT( COMMAND_SPIN_WRAP, wxCommandEvent );
wxDEFINE_EVENT( COMMAND_REARRANGECTRL_CHANGED, wxCommandEvent );

wxDEFINE_EVENT( COMMAND_DIALOGCTRL_CHANGED, wxCommandEvent );

wxDEFINE_EVENT( COMMAND_SIDEPANEL_CHANGED, wxCommandEvent );
wxDEFINE_EVENT( COMMAND_SIDEPANEL_REFRESH, wxCommandEvent );
wxDEFINE_EVENT( COMMAND_SIDEPANEL_SORT, wxCommandEvent );
wxDEFINE_EVENT( COMMAND_SIDEPANEL_OFFSET, wxCommandEvent );
wxDEFINE_EVENT( COMMAND_FORMULA_CHANGED, wxCommandEvent );
wxDEFINE_EVENT( COMMAND_FILTER_CHANGED, wxCommandEvent );

wxDEFINE_EVENT( COMMAND_CHART_SKIN_CHANGED, wxCommandEvent );
wxDEFINE_EVENT( COMMAND_ECHO_MESSAGE, wxCommandEvent );
wxDEFINE_EVENT( WIDGET_NOTIFY_PARENT, wxCommandEvent );
wxDEFINE_EVENT( WIDGET_DATA_CHANGED, wxCommandEvent );
wxDEFINE_EVENT( WIDGET_UPDATE_TITLE_QUERY, wxCommandEvent );
wxDEFINE_EVENT( APPWINDOW_CREATEVIEW, wxCommandEvent );
wxDEFINE_EVENT( ZOOM_CHANGED, wxCommandEvent );
wxDEFINE_EVENT( CONFIG_CHANGED, wxCommandEvent );
wxDEFINE_EVENT( GEOMETRY_CHANGED, wxCommandEvent );
wxDEFINE_EVENT( CONFIG_TOOLBAR_CHANGED, wxCommandEvent );
wxDEFINE_EVENT( CONFLET_CHANGED, wxCommandEvent );

wxDEFINE_EVENT( WD_WINDOW_ADDED, wxCommandEvent );
wxDEFINE_EVENT( WD_WINDOW_REMOVED, wxCommandEvent );
wxDEFINE_EVENT( WD_WINDOW_ACTIVATED, wxCommandEvent );
wxDEFINE_EVENT( WD_WINDOW_TITLE_CHANGED, wxCommandEvent );
wxDEFINE_EVENT( WD_HOROSCOPE_DATA_CHANGED, wxCommandEvent );
wxDEFINE_EVENT( WD_HOROSCOPE_SAVED, wxCommandEvent );
wxDEFINE_EVENT( WD_HOROSCOPE_LIST_CHANGED, wxCommandEvent );

/*****************************************************
**
**   setSavedWindowSize
**
******************************************************/
void setSavedWindowSize( wxWindow *widget, const wxString &configkey, const wxSize defaultSize )
{
	wxSize size( defaultSize );
	if ( const auto &a = gdiconfig->prefs.widgetSize.find( configkey ); a != gdiconfig->prefs.widgetSize.end())
	{
		size = a->second;
	}
	debuglog << "setSavedWindowSize defaultSize " << defaultSize << " new size " << size;
	if ( size.GetWidth() > 0 && size.GetHeight() > 0 ) widget->SetSize( size );
}

/*****************************************************
**
**   saveWindowSize
**
******************************************************/
void saveWindowSize( wxWindow *widget, const wxString &configkey )
{
	const wxSize size = widget->GetSize();
	debuglog << "saveWindowSize " << configkey << " size " << size.x << "/" << size.y;
	gdiconfig->prefs.widgetSize[ configkey ] = size;
}

/*****************************************************
**
**   setSavedWindowPosition
**
******************************************************/
void setSavedWindowPosition( wxWindow *widget, const wxString &configkey )
{
	if ( const auto &a = gdiconfig->prefs.widgetPosition.find( configkey ); a != gdiconfig->prefs.widgetPosition.end())
	{
		widget->SetPosition( wxPoint( a->second.x, a->second.y ));
	}
}

/*****************************************************
**
**   saveWindowPosition
**
******************************************************/
void saveWindowPosition( wxWindow *widget, const wxString &configkey )
{
	const wxPoint point = widget->GetPosition();
	gdiconfig->prefs.widgetPosition[ configkey ] = wxSize( point.x, point.y );
}

/*****************************************************
**
**   get current status of shift, ctrl and alt keys
**
******************************************************/
int getCurrentKeyMod()
{
	int keymod = KeyboardModfier::KbNone;
	if ( wxGetKeyState( WXK_SHIFT )) keymod += KeyboardModfier::KbShift;
	if ( wxGetKeyState( WXK_CONTROL )) keymod += KeyboardModfier::KbCtrl;
	if ( wxGetKeyState( WXK_ALT )) keymod += KeyboardModfier::KbAlt;
	return keymod;
}

/**************************************************************
***
**    doMessageBox
***
***************************************************************/
int doMessageBox( wxWindow *parent, const wxString message, const long style )
{
	return wxMessageDialog( parent, message, GUI_APP_NAME, style | wxCENTRE ).ShowModal();
}

/*************************************************//**
*
*    showMissingEphemDataMessage
*
******************************************************/
void showMissingEphemDataMessage( wxWindow *parent )
{
	doNotShowAgainMessage( parent, DoNotShowAgainTopic::MissingEphemData,
		tr( "Some items could not be calculated properly. Please check if you configured "
		"Swiss Ephemeris data files in Configuration/Ephemeris correctly." ));
}

/*************************************************//**
*
*    doNotShowAgainMessage
*
******************************************************/
void doNotShowAgainMessage( wxWindow *parent, const DoNotShowAgainTopic &topic, const wxString &message, const bool repeatPerSession )
{
	assert_valid_enum<DoNotShowAgainTopic>( topic );

	// topics are switched off permanently if the user checks the message dialog
	const bool cfgfound = gdiconfig->prefs.doNotShowAgainTopics.find( topic )
		!= gdiconfig->prefs.doNotShowAgainTopics.end();
	if ( cfgfound )
	{
		debuglog << "doNotShowAgainMessage topic " << topic << " found in gdiconfig: do not show message";
		return;
	}

	// show each message only once per session, i.e. store the topic in static set if shown.
	static std::set<DoNotShowAgainTopic> sessionProtocol;
	const bool shownInSession = sessionProtocol.find( topic ) != sessionProtocol.end();
	if ( shownInSession && ! repeatPerSession )
	{
		debuglog << "doNotShowAgainMessage topic " << topic << " already shown";
		return;
	}

	wxRichMessageDialog dlg( parent, message, GUI_APP_NAME, wxICON_EXCLAMATION | wxOK );
	dlg.ShowCheckBox( tr( "Do not show this message again." ));
	dlg.ShowModal();
	if ( dlg.IsCheckBoxChecked() )
	{
		debuglog << "doNotShowAgainMessage CHECKED, switch it off";
		gdiconfig->prefs.doNotShowAgainTopics.insert( topic );
	}
	sessionProtocol.insert( topic );
}

