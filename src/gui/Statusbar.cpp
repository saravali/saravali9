/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/Statusbar.cpp
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

#include "Statusbar.h"

#include "astrobase.h"
#include "astroconfig.h"
#include "Calculator.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Formatter.h"
#include "Lang.h"
#include "mathbase.h"
#include "MenuProvider.h"
#include "Viewpoint.h"

#include <wx/dcclient.h>
#include <wx/timer.h>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

enum { STATUSBAR_PTIMER = wxID_HIGHEST + 2500, STATUSBAR_MSGTIMER };

//#define DEBUG_STATUSBAR

/*************************************************//**
*
*  wxLog specialization filtering messages
*
******************************************************/
class StatbarLogger : public wxLog
{
public:
	StatbarLogger( MainWindowStatusbar *statbar ) : wxLog(), statbar( statbar ) {}

protected:
	virtual void DoLogRecord( wxLogLevel level, const wxString &msg, const wxLogRecordInfo& /*info*/ ) override
	{
		if ( level <= LogAdapter::get()->convertToWxLogLevel( gdiconfig->view.uiNotificationLogLevel ))
		{
			statbar->echo( msg );
		}
	}

	MainWindowStatusbar *statbar = nullptr;
};

/*****************************************************
**
**   MainWindowStatusbar   ---   Constructor
**
******************************************************/
MainWindowStatusbar::MainWindowStatusbar( wxWindow *parent )
	: wxStatusBar( parent ),
	showAsc( astroconfig->general.statusbarShowAscPosition ),
	showMoon( astroconfig->general.statusbarShowMoonPosition )
{
	int fieldCount = 1;
	if ( showAsc ) fieldCount++;
	if ( showMoon ) fieldCount++;

	wxClientDC dc(this);
	wxSize planetFieldSize = dc.GetTextExtent( "x Mo 00:00:00 Ar xxxx" );
	debuglog << "MainWindowStatusbar::MainWindowStatusbar planetFieldSize " << planetFieldSize;
	//const int px = 120;
	const int px = planetFieldSize.x;

	int widths[Field_Max];
	widths[0] = -1; 
	widths[Field_AscPos] = px;
	widths[Field_MoonPos] = px;

	SetFieldsCount( fieldCount );
	SetStatusWidths(Field_Max, widths);

	planettimer = new wxTimer( this, STATUSBAR_PTIMER );
	msgtimer = new wxTimer( this, STATUSBAR_MSGTIMER );

	(void*)new wxLogChain( new StatbarLogger( this ));

	if ( showAsc || showMoon ) StartPlanetTimer( true );

	Connect( STATUSBAR_MSGTIMER, wxEVT_TIMER, wxTimerEventHandler( MainWindowStatusbar::OnMessageTimer ));
	Connect( STATUSBAR_PTIMER, wxEVT_TIMER, wxTimerEventHandler( MainWindowStatusbar::OnPlanetTimer ));
	Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( MainWindowStatusbar::OnRightDown ));
	Connect( wxEVT_SHOW, wxShowEventHandler( MainWindowStatusbar::OnShow ));
}

/*****************************************************
**
**   MainWindowStatusbar   ---   Destructor
**
******************************************************/
MainWindowStatusbar::~MainWindowStatusbar()
{
	if ( msgtimer->IsRunning()) msgtimer->Stop();
	if ( planettimer->IsRunning()) planettimer->Stop();

	delete wxLog::SetActiveTarget( nullptr );
}

/*****************************************************
**
**   MainWindowStatusbar   ---   echo
**
******************************************************/
void MainWindowStatusbar::echo( const wxString &message )
{
	if ( msgtimer->IsRunning()) msgtimer->Stop();

	SetStatusText( message, 0 );

	if ( ! message.IsEmpty())
	{
		msgtimer->Start( 1000 * astroconfig->general.statusMessageTimeout, true );
	}
}

/*****************************************************
**
**   MainWindowStatusbar   ---   OnMessageTimer
**
******************************************************/
void MainWindowStatusbar::OnMessageTimer( wxTimerEvent& )
{
	if ( msgtimer->IsRunning()) msgtimer->Stop();
	SetStatusText( wxEmptyString, 0 );
}

/*****************************************************
**
**   MainWindowStatusbar   ---   OnShow
**
******************************************************/
void MainWindowStatusbar::MainWindowStatusbar::OnShow( wxShowEvent &event )
{
#ifdef DEBUG_STATUSBAR
	debuglog << " MainWindowStatusbar::OnShow show " << event.IsShown();
#endif
	if ( ! event.IsShown())
	{
		StartPlanetTimer( false );
	}
}

/*****************************************************
**
**   MainWindowStatusbar   ---   StartPlanetTimer
**
******************************************************/
void MainWindowStatusbar::StartPlanetTimer( const bool &b )
{
	if ( b )
	{
		if ( ! planettimer->IsRunning() ) planettimer->Start( 1000 );
	}
	else
	{
		if ( planettimer->IsRunning() ) planettimer->Stop();
		SetStatusText( wxEmptyString, 1 );
		SetStatusText( wxEmptyString, 2 );
	}
}

/*****************************************************
**
**   MainWindowStatusbar   ---   OnPlanetTimer
**
******************************************************/
void MainWindowStatusbar::OnPlanetTimer( wxTimerEvent& )
{
	if ( ! showAsc && ! showMoon ) return;

	const Viewpoint viewpoint = astroconfig->general.defaultViewpoint;
#ifdef DEBUG_STATUSBAR
	debuglog << "MainWindowStatusbar::OnPlanetTimer viewpoint " << viewpoint << " isShown " << IsShown();
#endif
	Calculator *calculator = CalculatorFactory().getCalculator();
	Formatter *formatter = Formatter::get();
	Lang lang;

	DateLocation dl;
	dl.location = astroconfig->standardLocation;
	PlanetConfig planetCfg = astroconfig->getViewpointConfig( viewpoint ).planets;
	planetCfg.textFormat = TextFormat::Short;
	PlanetPropertyConfig planetProps = astroconfig->getViewpointConfig( viewpoint ).planetProps;
	planetProps.textFormat = TextFormat::Short;

	const double ayanamsa = calculator->calcAyanamsa( dl.date.jd, astroconfig->getViewpointConfig( viewpoint ).calculation.ayanamsa );

	int current = 1;
	if ( showAsc )
	{
		const std::array<double, 5> anglePos = calculator->calcAnglesSpeed( dl.date.jd, dl.location.latitude, dl.location.longitude );
		const double lasc = red_deg( anglePos[1] - ayanamsa );

		const wxString s1 = formatter->formatZodiacalLongitude( planetProps, lasc ).toWxString();
#ifdef DEBUG_STATUSBAR
		debuglog << "MainWindowStatusbar::OnPlanetTimer ascpos " << lasc << " 11 " << s1;
#endif
		SetStatusText( std::format( "{} {}", lang.getPlanetName( PlanetId::Ascendant, viewpoint, TextFormat::Short ), s1 ), current++ );
	}
	if ( showMoon )
	{
		const PlanetPosition moonpos = calculator->calcPositionSpeed( dl, PlanetId::Moon, viewpoint );
		const double lmoon = red_deg( moonpos.longitude - ayanamsa );
		wxString s2 = formatter->formatZodiacalLongitude( planetProps, lmoon ).toWxString();
#ifdef DEBUG_STATUSBAR
		debuglog << "MainWindowStatusbar::OnPlanetTimer moonpos " << lmoon << " 22 " << s2;
#endif
		SetStatusText( std::format( "{} {}", lang.getPlanetName( PlanetId::Moon, viewpoint, TextFormat::Short ), s2 ), current++ );
	}
}

/*****************************************************
**
**   MainWindowStatusbar   ---   OnRightDown
**
******************************************************/
void MainWindowStatusbar::OnRightDown( wxMouseEvent &event )
{
	wxMenu *menu = ContextMenuProvider().getAppWindowContextMenu();
	PopupMenu( menu, event.GetX(), event.GetY() );
	delete menu;
}

