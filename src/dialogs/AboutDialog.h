/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AboutDialog.h
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


#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

// begin wxGlade: dependencies
// end wxGlade

#include <wx/dialog.h>

class wxButton;
class wxPanel;
class wxStaticText;
class wxTimer;
class wxTimerEvent;

/*************************************************//**
*
*   animated logo in AboutDialog
*
******************************************************/
class LogoWidget : public wxWindow
{
public:

	LogoWidget( wxWindow *parent, int id );
	~LogoWidget();

	void doAnimation();

	void startTimer();

	void stopTimer();

protected:

	void OnPaint(wxPaintEvent& event);
	void OnTimer( wxTimerEvent& );

	wxImage *image;
	int xzoom, yzoom, ani_mode, ani_direction;
	wxTimer *timer;
};

/*************************************************//**
*
*  About dialog (shown with F1)
*
* About dialog holding application information, license and credits.
**
******************************************************/
class AboutDialog: public wxDialog
{
public:
    // begin wxGlade: AboutDialog::ids
    // end wxGlade

	AboutDialog(wxWindow* parent );

private:

protected:

    // begin wxGlade: AboutDialog::attributes
    wxPanel* panel_logo;
    LogoWidget* widget;
    wxStaticText* label_title_version;
    wxStaticText* label_version;
    wxStaticText* label_title_author;
    wxStaticText* label_author;
    wxStaticText* label_title_url;
    wxStaticText* label_url;
    wxStaticText* label_title_builddate;
    wxStaticText* label_builddate;
    wxStaticText* label_title_compiler;
    wxStaticText* label_compiler;
    wxStaticText* label_title_os;
    wxStaticText* label_os;
    wxStaticText* label_title_wx;
    wxStaticText* label_version_wx;
    wxStaticText* label_title_swe;
    wxStaticText* label_version_swe;
    wxStaticText* label_title_sqlite3;
    wxStaticText* label_version_sqlite3;
    wxStaticText* label_title_wxsqlite;
    wxStaticText* label_version_wxsqlite3;
    wxStaticText* label_title_wxpdfdoc;
    wxStaticText* label_version_wxpdfdoc;
    wxStaticText* label_title_jsoncpp;
    wxStaticText* label_version_jsoncpp;
    wxButton* button_ok;
    // end wxGlade
};


#endif // ABOUTDIALOG_H
