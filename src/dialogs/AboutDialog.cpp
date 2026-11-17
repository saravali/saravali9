/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AboutDialog.cpp
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

#include "AboutDialog.h"

#include "BaseDao.h"
#include "Calculator.h"
#include "Commands.h"
#include "config.h"
#include "constants.h"
#include "guibase.h"
#include "ImageProvider.h"
#include "JsonHandler.h"
#include "logger.h"

#include <wx/button.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/image.h>
#include <wx/panel.h>
#include <wx/pdfdocument.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/timer.h>

#include <format>

enum { ABOUT_TIMER = wxID_HIGHEST + 1, AD_NOTEBOOK };

#define PIC_X 546
#define PIC_Y 205
#define LOGO_FILE "logo.jpg"
#define AUTHOR "Martin Pettau"

/*****************************************************
**
**   LogoWidget   ---   Constructor
**
******************************************************/
LogoWidget::LogoWidget( wxWindow *parent, int id )
		: wxWindow( parent, id )
{
	debuglog << "loading logo image file " << LOGO_FILE;
	image = new wxImage( ImageProvider::get()->getImage( LOGO_FILE ));

	xzoom = yzoom = 100;
	ani_mode = 0;
	ani_direction = 0;

	timer = new wxTimer( this, ABOUT_TIMER );
	timer->Start( 6000 );

	Connect( wxEVT_PAINT, wxPaintEventHandler( LogoWidget::OnPaint ));
	Connect( ABOUT_TIMER, wxEVT_TIMER, wxTimerEventHandler( LogoWidget::OnTimer ));
}

/*****************************************************
**
**   LogoWidget   ---   Destructor
**
******************************************************/
LogoWidget::~LogoWidget()
{
	timer->Stop();
	delete timer;
	delete image;
}

/*****************************************************
**
**   LogoWidget   ---   doAnimation
**
******************************************************/
void LogoWidget::doAnimation()
{
	if ( ! timer->IsRunning() ) timer->Start( 100 );
	else timer->Stop();
}

/*****************************************************
**
**   LogoWidget   ---   startTimer
**
******************************************************/
void LogoWidget::startTimer()
{
	if ( ! timer->IsRunning() ) timer->Start( 6000 );
}

/*****************************************************
**
**   LogoWidget   ---   stopTimer
**
******************************************************/
void LogoWidget::stopTimer()
{
	if ( timer->IsRunning() ) timer->Stop();
	xzoom = yzoom = 100;
	ani_mode = 0;
	ani_direction = 0;
	Refresh();
}

/*****************************************************
**
**   LogoWidget   ---   OnTimer
**
******************************************************/
void LogoWidget::OnTimer( wxTimerEvent& )
{
	if ( xzoom == 100 && yzoom == 100 ) timer->Start( 100 );
	if ( ani_direction == 0 )
	{
		if ( ani_mode == 0 ) xzoom -= 10;
		else yzoom -= 10;
		if ( xzoom <= -100 || yzoom <= -100 ) ani_direction = 1;
	}
	else
	{
		if ( ani_mode == 0 ) xzoom += 10;
		else yzoom += 10;
		if ( xzoom >= 100 && yzoom >= 100 )
		{
			ani_direction = 0;
			ani_mode = ! ani_mode;
			timer->Start( 12000 );
		}
	}
	Refresh( false );
}

/*****************************************************
**
**   LogoWidget   ---   OnPaint
**
******************************************************/
void LogoWidget::OnPaint(wxPaintEvent& )
{
	debuglog << "LogoWidget::OnPaint xzoom " << xzoom << " yzoom " << yzoom;

	double axzoom = ( xzoom > 0 ? xzoom : -xzoom );
	double ayzoom = ( yzoom > 0 ? yzoom : -yzoom );
	int x = (int)(PIC_X * axzoom / 100);
	int xborder = (int)(( PIC_X - x ) / 2);
	int y = (int)(PIC_Y * ayzoom / 100);
	int yborder = (int)(( PIC_Y - y ) / 2);
	bool mirror = ( xzoom == 100 ? false : true );

	wxPaintDC dc(this);
	wxColour bgcolor = wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND );
	dc.SetBrush( wxBrush( GetBackgroundColour(), wxBRUSHSTYLE_SOLID ));
	dc.SetPen( wxPen( bgcolor, 1, wxPENSTYLE_SOLID ));
	dc.DrawRectangle( -1, -1, xborder, PIC_Y+2 ); // left
	dc.DrawRectangle( PIC_X - xborder, -1, PIC_X/2+2, PIC_Y+2 ); // right
	dc.DrawRectangle( -1, -1, PIC_X+2, yborder ); // upper
	dc.DrawRectangle( -1, PIC_Y - yborder, PIC_X+2, PIC_Y/2+2 ); // lower

	//static double angle = 0;
	//angle += .02;
	if ( xzoom == 100 && yzoom == 100 )
	{
		dc.DrawBitmap( wxBitmap( *image ), xborder, yborder, true );
	}
	else if ( xzoom >= 0 && yzoom >= 0 )
	{
		if ( x > 0 && y > 0 )
		{
			dc.DrawBitmap( wxBitmap( image->Scale( x, y )), xborder, yborder, true );
		}
	}
	else
	{
		dc.DrawBitmap( wxBitmap( image->Mirror( mirror ).Scale( x, y )), xborder, yborder, true );
	}
}

/*****************************************************
**
**   AboutDialog   ---   Constructor
**
******************************************************/
AboutDialog::AboutDialog(wxWindow* parent )
		: wxDialog( parent, -1, "", wxDefaultPosition, wxDefaultSize, DEFAULT_DIALOG_STYLE )
{
    // begin wxGlade: AboutDialog::AboutDialog
    SetTitle(_("About"));
    wxFlexGridSizer* sizer_main = new wxFlexGridSizer(3, 1, 0, 0);
    wxFlexGridSizer* sizer_logopanel = new wxFlexGridSizer(2, 1, 0, 0);
    sizer_main->Add(sizer_logopanel, 0, 0, 0);
    panel_logo = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxTAB_TRAVERSAL);
    sizer_logopanel->Add(panel_logo, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
    wxFlexGridSizer* sizer_logo = new wxFlexGridSizer(1, 1, 0, 0);
    widget = new LogoWidget(panel_logo, wxID_ANY);
    widget->SetMinSize(wxSize(546, 205));
    sizer_logo->Add(widget, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
    wxBoxSizer* sizer_grids = new wxBoxSizer(wxVERTICAL);
    sizer_logopanel->Add(sizer_grids, 1, wxALIGN_CENTER_HORIZONTAL, 0);
    wxStaticBoxSizer* sizer_about = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("About")), wxVERTICAL);
    sizer_grids->Add(sizer_about, 1, wxEXPAND, 0);
    wxFlexGridSizer* grid_about = new wxFlexGridSizer(6, 2, 0, 0);
    sizer_about->Add(grid_about, 0, wxEXPAND, 0);
    label_title_version = new wxStaticText(this, wxID_ANY, _("Version"));
    grid_about->Add(label_title_version, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_version = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_about->Add(label_version, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_title_author = new wxStaticText(this, wxID_ANY, _("Author"));
    grid_about->Add(label_title_author, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_author = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_about->Add(label_author, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_title_url = new wxStaticText(this, wxID_ANY, _("URL"));
    grid_about->Add(label_title_url, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_url = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_about->Add(label_url, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_title_builddate = new wxStaticText(this, wxID_ANY, _("Build Date"));
    grid_about->Add(label_title_builddate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_builddate = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_about->Add(label_builddate, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_title_compiler = new wxStaticText(this, wxID_ANY, _("Compiler"));
    grid_about->Add(label_title_compiler, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_compiler = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_about->Add(label_compiler, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_title_os = new wxStaticText(this, wxID_ANY, _("Current OS"));
    grid_about->Add(label_title_os, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_os = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_about->Add(label_os, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_versions = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Integrated Software Versions")), wxVERTICAL);
    sizer_grids->Add(sizer_versions, 1, wxALL|wxEXPAND, 3);
    wxFlexGridSizer* grid_versions = new wxFlexGridSizer(6, 2, 0, 5);
    sizer_versions->Add(grid_versions, 1, wxALL, 3);
    label_title_wx = new wxStaticText(this, wxID_ANY, _("wxWidgets"));
    grid_versions->Add(label_title_wx, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_version_wx = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_versions->Add(label_version_wx, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_title_swe = new wxStaticText(this, wxID_ANY, _("Swiss Ephemeris"));
    grid_versions->Add(label_title_swe, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_version_swe = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_versions->Add(label_version_swe, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_title_sqlite3 = new wxStaticText(this, wxID_ANY, _("sqlite3"));
    grid_versions->Add(label_title_sqlite3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_version_sqlite3 = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_versions->Add(label_version_sqlite3, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_title_wxsqlite = new wxStaticText(this, wxID_ANY, _("wxsqlite3"));
    grid_versions->Add(label_title_wxsqlite, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_version_wxsqlite3 = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_versions->Add(label_version_wxsqlite3, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_title_wxpdfdoc = new wxStaticText(this, wxID_ANY, _("wxpdfdoc"));
    grid_versions->Add(label_title_wxpdfdoc, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_version_wxpdfdoc = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_versions->Add(label_version_wxpdfdoc, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_title_jsoncpp = new wxStaticText(this, wxID_ANY, _("Jsoncpp"));
    grid_versions->Add(label_title_jsoncpp, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    label_version_jsoncpp = new wxStaticText(this, wxID_ANY, _("dummy"));
    grid_versions->Add(label_version_jsoncpp, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    wxStaticLine* static_line_1 = new wxStaticLine(this, wxID_ANY);
    sizer_main->Add(static_line_1, 0, wxALL, 3);
    button_ok = new wxButton(this, wxID_OK, _("OK"));
    button_ok->SetDefault();
    sizer_main->Add(button_ok, 0, wxALIGN_CENTER|wxALL, 3);
    
    sizer_logo->AddGrowableCol(0);
    panel_logo->SetSizer(sizer_logo);
    sizer_logopanel->AddGrowableRow(1);
    sizer_logopanel->AddGrowableCol(0);
    sizer_main->AddGrowableRow(0);
    sizer_main->AddGrowableCol(0);
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    Layout();
    // end wxGlade

	label_version->SetLabel( PACKAGE_VERSION );
  label_author->SetLabel( AUTHOR );
  label_url->SetLabel( SARAVALI_WEBSITE_URL );

	// Build date
  label_builddate->SetLabel( __DATE__  );

	// Compiler
	{
		wxString s;
		// Compiler
	#ifdef __GNUC__
		s << "GNUC " << __VERSION__ ;
	#elif defined(__VISUALC__)
		s << "Visual C " << std::format( "{}", _MSC_VER );
	#else
		s << "Unknown Compiler";
	#endif
		label_compiler->SetLabel( s );
	}
  label_os->SetLabel( wxGetOsDescription());

	// wxWidgets version
	{
		wxString wxversion;
		#ifdef __WXOSX_CARBON__
			wxversion << "Carbon";
		#elif __WXOSX_COCOA__
			wxversion << "Cocoa";
		#elif defined  __MINGW32__
			wxversion << "MinGW32";
		#elif defined  __VISUALC__
			wxversion << "VisualC";
		#elif defined __WXMOTIF__ 
			s << "Motif";
		#elif defined __WXGTK__
			wxversion << "GTK";
		#elif defined __WXX11__
			wxversion << "X11";
		#else
			wxversion << "Unknown";
		#endif

		wxversion += std::format( "{}.{}.{}", wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER );
		label_version_wx->SetLabel( wxversion );
	}


	// Additional software versions
	label_version_swe->SetLabel( CalculatorFactory().getCalculator()->getVersion());
	label_version_wxpdfdoc->SetLabel( PDFDOC_VERSION_STRING );
	label_version_sqlite3->SetLabel( BaseDao().getSqliteVersion() );
	label_version_wxsqlite3->SetLabel( BaseDao().getWxSqliteVersion() );
	label_version_jsoncpp->SetLabel( JsonHandler::getJsonCppVersion());

	SetIcon( ImageProvider::get()->getIcon( BitmapId::About ));
	button_ok->SetFocus();
}

