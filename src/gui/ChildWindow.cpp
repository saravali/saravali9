/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/ChildWindow.cpp
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

#include "astroconfig.h"
#include "BasicWidget.h"
#include "BirthDataDialog.h"
#include "ContextProperties.h"
#include "Commands.h"
#include "enumserializer.h"
#include "FileConfig.h"
#include "gdiconfig.h"
#include "ImageProvider.h"
#include "PdfLauncher.h"
#include "WindowDirectory.h"

#include <wx/event.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/timer.h>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

enum { DOCUMENT_TIMER = wxID_HIGHEST + 1 };

#define DEBUG_CHILDWINDOW

/*****************************************************
**
**   ChildWindow   ---   Constructor
**
******************************************************/
ChildWindow::ChildWindow( wxWindow *parent, const ViewId &viewId, const BitmapId &bmpid, Horoscope_ptr horoscope )
	: wxFrame( parent, -1, "ChildWindow", wxDefaultPosition, //wxDefaultSize ),
			gdiconfig->prefs.getWidgetSize( viewId )),
		viewId( viewId ),
		bmpId( bmpid )
{
#ifdef DEBUG_CHILDWINDOW
	debuglog << "ChildWindow initial size " <<  gdiconfig->prefs.getWidgetSize( viewId ) << " horoscope " << horoscope;
#endif

	SetMinSize( wxSize( 400, 300 ));

	SetIcon( ImageProvider::get()->getIcon( bmpId ));

	props = ContextProps_ptr( new ContextProperties );
	props->setRadixHoroscope( horoscope );

	// set initial viewpoint.
	const Viewpoint stickyVp = ViewpointHelper::get()->getStickyViewVp( viewId );
	const Viewpoint viewpoint = stickyVp == Viewpoint::None ? astroconfig->general.defaultViewpoint : stickyVp;
	props->setPreferredViewpoint( viewpoint );
#ifdef DEBUG_CHILDWINDOW
	debuglog << "ChildWindow has sticky vp " << stickyVp << " preferred viewpoint " << viewpoint;
#endif

	switch( viewId )
	{
		case ViewId::Direction:
			props->setDataContext( DataContext::Direction );
		break;
		case ViewId::MultiDirection:
			props->setDataContext( DataContext::MultiDirection );
		break;
		case ViewId::Solar:
			props->setDataContext( DataContext::Solar );
		break;
		case ViewId::Ashtakavarga:
			props->setDataContext( DataContext::Ashtakavarga );
		break;
		case ViewId::Partner:
			props->setDataContext( DataContext::Partner );
		break;
		default:
			props->setDataContext( DataContext::Radix );
		break;
	}

	anitimer = new wxTimer( this, DOCUMENT_TIMER );
	StartAniTimer( false );

	Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( ChildWindow::OnSetFocus ));
	Connect( wxEVT_ACTIVATE, wxActivateEventHandler( ChildWindow::OnActivate ));

	// animation timer
	Connect( DOCUMENT_TIMER, wxEVT_TIMER, wxTimerEventHandler( ChildWindow::OnAniTimer ));

	Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ChildWindow::OnClose ));
	Connect( wxID_ANY, WIDGET_DATA_CHANGED, wxCommandEventHandler( ChildWindow::OnWidgetDataChanged ));
	Connect( wxID_ANY, WIDGET_UPDATE_TITLE_QUERY, wxCommandEventHandler( ChildWindow::OnWidgetUpdateTitleQuery ));
	Connect( wxID_ANY, COMMAND_ECHO_MESSAGE, wxCommandEventHandler( ChildWindow::OnCatchEchoEvent ));
	Connect( wxID_ANY, ZOOM_CHANGED, wxCommandEventHandler( ChildWindow::OnZoomChangedEvent ));

	Connect( (int)CommandId::ZoomIn, (int)CommandId::ZoomOut, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler( ChildWindow::OnChangeZoomCommand ));
}

/*****************************************************
**
**   ChildWindow   ---   Destructor
**
******************************************************/
ChildWindow::~ChildWindow()
{
	if ( anitimer->IsRunning() ) anitimer->Stop();

	if ( ismainwindow )
	{
		props->setRadixHoroscope( nullptr );
	}
#ifdef DEBUG_CHILDWINDOW
		debuglog << "ChildWindow::Destructor size " << GetSize() << " client Size " << GetClientSize();
#endif
	gdiconfig->prefs.setWidgetSize( getViewId(), GetSize());
	//gdiconfig->prefs.setWidgetSize( getViewId(), GetClientSize());
}

/*****************************************************
**
**   ChildWindow   ---   postCreate
**
******************************************************/
void ChildWindow::postCreate()
{
	if ( widget )
	{
#ifdef DEBUG_CHILDWINDOW
		debuglog << "ChildWindow::postCreate";
#endif
		widget->postCreate();
		widget->SetFocus();
	}
	else errorlog << "ChildWindow::postCreate: no widget";
}

/*****************************************************
**
**   ChildWindow   ---   OnActivate
**
******************************************************/
void ChildWindow::OnActivate( wxActivateEvent &event )
{
	updateTitle();
	if ( event.GetActive()) WindowDirectory::get()->setActiveWindow( this );
}

/*****************************************************
**
**   ChildWindow   ---   OnSetFocus
**
******************************************************/
void ChildWindow::OnSetFocus( wxFocusEvent& )
{
	if ( widget ) widget->SetFocus();
}

/*****************************************************
**
**   ChildWindow   ---   OnWidgetDataChanged
**
******************************************************/
void ChildWindow::OnWidgetDataChanged( wxCommandEvent& )
{
#ifdef DEBUG_CHILDWINDOW
	debuglog << "ChildWindow::OnWidgetDataChanged";
#endif
	OnDataChanged();
}

/*****************************************************
**
**   ChildWindow   ---   OnWidgetUpdateTitleQuery
**
******************************************************/
void ChildWindow::OnWidgetUpdateTitleQuery( wxCommandEvent& )
{
#ifdef DEBUG_CHILDWINDOW
	debuglog << "ChildWindow::OnWidgetUpdateTitleQuery";
#endif
  WindowDirectory::get()->windowTitleChanged( this );
	updateTitle();
}

/*****************************************************
**
**   ChildWindow   ---   OnChangeZoomCommand
**
******************************************************/
void ChildWindow::OnChangeZoomCommand( wxCommandEvent &event )
{
#ifdef DEBUG_CHILDWINDOW
	debuglog << "ChildWindow::OnChangeZoomCommand event id " << event.GetId() << " CommandId::ZoomIn " << (int)CommandId::ZoomIn;
#endif
	switch( (CommandId)event.GetId())
	{
		case CommandId::ZoomIn:
			props->increaseLogZoom( 1 );
		break;
		case CommandId::ZoomOriginal:
			props->setLogZoom( 0 );
		break;
		case CommandId::ZoomOut:
			props->increaseLogZoom( -1 );
		break;
		default:
			assert( false );
		break;
	}
	zoomChanged();
}

/*****************************************************
**
**   ChildWindow   ---   zoomChanged
**
******************************************************/
void ChildWindow::zoomChanged()
{
	OnDataChanged();
}

/*****************************************************
**
**   ChildWindow   ---   OnDataChanged
**
******************************************************/
void ChildWindow::OnDataChanged()
{
#ifdef DEBUG_CHILDWINDOW
	debuglog << "ChildWindow::OnDataChanged";
#endif
	updateTitle();
	assert( widget );
	widget->OnDataChanged();
}

/*****************************************************
**
**   ChildWindow   ---   updateTitle
**
******************************************************/
void ChildWindow::updateTitle()
{
	if ( widget )
	{
		title= widget->getTitle();
	}
	else
	{
		title = wxString( "no widget" );
	}

	if ( props->getRadixHoroscope())
	{
		title += " <" + props->getRadixHoroscope()->dl.name + ">";
	}
#ifdef DEBUG_CHILDWINDOW
	debuglog << "ChildWindow::updateTitle " << title << " widget title " << widget->getTitle();
#endif

	SetTitle( title );
}

/*****************************************************
**
**   ChildWindow   ---   OnCatchEchoEvent
**
******************************************************/
void ChildWindow::OnCatchEchoEvent( wxCommandEvent &event )
{
	echo( event.GetString());
}

/*****************************************************
**
**   ChildWindow   ---   echo
**
******************************************************/
void ChildWindow::echo( const wxString &message )
{
#ifdef DEBUG_CHILDWINDOW
	debuglog << "ChildWindow::echo " << message;
#endif
	if ( widget )
	{
		widget->doEcho( message );
	}
}

/*****************************************************
**
**   ChildWindow   ---   OnClose
**
******************************************************/
void ChildWindow::OnClose( wxCloseEvent &event )
{
	WindowDirectory *wd = WindowDirectory::get();
#ifdef DEBUG_CHILDWINDOW
	debuglog << "ChildWindow::OnClose children count " << wd->getWindowCount()
		<< " is main window " << std::boolalpha << ismainwindow;
#endif

	if ( queryClose())
	{
		props->invalidate();
		Horoscope_ptr h = props->getRadixHoroscope();
		if ( h && ismainwindow )
		{
			wd->closeSiblings( this );
			wd->forgetHoroscope( h );
		}
		wd->releaseWindow( this );
		Destroy();
	}
	else
	{
#ifdef DEBUG_CHILDWINDOW
		debuglog << "ChildWindow::OnClose VETO, not closing.";
#endif
		event.Veto();
	}
}

/*****************************************************
**
**   ChildWindow   ---   queryClose
**
******************************************************/
bool ChildWindow::queryClose()
{
	Horoscope_ptr h = props->getRadixHoroscope();
	if ( ! h  || ! ismainwindow ) return true;

	if ( ! h->dl.dirty ) return true;

	const wxString s = tr( "Do you want to save the chart" ) + ": " + h->dl.name + "?";
	const int i = doMessageBox( this, s, wxYES_NO | wxCANCEL | wxCENTRE );

	if ( i == wxID_CANCEL ) return false;
	else if ( i == wxID_YES )
	{
		return save();
	}
	else
	{
		h->dl.dirty = false;
		return true;
	}
}

#define MINIMUM_ANIMATION_FREQUENCY_MILLISEC 300

/*****************************************************
**
**   ChildWindow   ---   StartAniTimer
**
******************************************************/
void ChildWindow::StartAniTimer( bool b )
{
	if ( b ) // Intervall 0 will be 100 msec, eat up myself otherwise. What about slow machines
	{
		if ( ! anitimer->IsRunning() || 	myAniInterval != astroconfig->animation.interval )
		{
			anitimer->Start( std::max( astroconfig->animation.interval * 1000, MINIMUM_ANIMATION_FREQUENCY_MILLISEC ) );
			myAniInterval = astroconfig->animation.interval;
		}
	}
	else
	{
		if ( anitimer->IsRunning() ) anitimer->Stop();
	}
}

/*****************************************************
**
**   ChildWindow   ---   OnAniTimer
**
******************************************************/
void ChildWindow::OnAniTimer( wxTimerEvent& )
{
	Horoscope_ptr h = props->getRadixHoroscope();
	assert( h );
	double sw;
	if ( astroconfig->animation.mode )
	{
		sw = astroconfig->animation.stepLength;
		switch ( astroconfig->animation.stepDimension )
		{
		case 0:  // seconds
			sw /= 86400;
			break;
		case 1:  // minutes
			sw /= 1400;
			break;
		case 2:  // hours
			sw /= 24;
			break;
		case 3:  // days
			break;
		case 4:  // months
			sw *= 30.6;
			break;
		case 5:  // years
			sw *= 365.25;
			break;
		default:  // seconds
			sw /= 86400;
			break;
		}
		h->setDate( h->getJD() + sw );
	}
	else
	{
		h->dl.date.setCurrentDate();
	}
	h->update();

	for( auto *window : WindowDirectory::get()->getWindowsForHoroscope( h ))
	{
		assert( window );
		window->OnDataChanged();
	}

	// see if interval param has changed, so restart timer
	if ( astroconfig->animation.interval != myAniInterval )	StartAniTimer( true );
}

/*****************************************************
**
**   ChildWindow   ---   dispatchApplicationWindowCommand
**
******************************************************/
void ChildWindow::dispatchApplicationWindowCommand( const CommandId &command )
{
	if ( ! props->isValid() ) return; // nothing to do for closing views
#ifdef DEBUG_CHILDWINDOW
	debuglog << "ChildWindow::dispatchApplicationWindowCommand " << command;
#endif

	switch( command )
	{
		case CommandId::ViewpointVedic:
		case CommandId::ViewpointWestern:
		case CommandId::ViewpointUranian:
		case CommandId::ViewpointAstronomical:
			dispatchViewpointCommand( command );
		return;
		case CommandId::Close:
			Close();
		break;
		case CommandId::CloseDoc:
			for( auto *window : WindowDirectory::get()->getWindowsForHoroscope( props->getRadixHoroscope() ))
			{
				if ( window != this ) { window->Close(); }
				Close();
			}
		break;
		case CommandId::Save:
			if ( props->getRadixHoroscope() ) save();
		break;
		case CommandId::SaveAs:
			if ( props->getRadixHoroscope() ) saveAs();
		break;
		case CommandId::QuickPrint:
			if ( props->getRadixHoroscope() ) doQuickPrint();
		break;
		case CommandId::Print:
			if ( props->getRadixHoroscope() ) doPrint();
		break;
		case CommandId::EditData:
			//Raise();
			if ( props->getRadixHoroscope() ) editData();
		break;

		case CommandId::ZoomIn:
			props->increaseLogZoom( 1 );
			zoomChanged();
		break;
		case CommandId::ZoomOut:
			props->increaseLogZoom( -1 );
			zoomChanged();
		break;
		case CommandId::ZoomOriginal:
#ifdef DEBUG_CHILDWINDOW
			debuglog << "ZoomOriginal";
#endif
			props->setLogZoom( 0 );
			zoomChanged();
		break;

		case CommandId::ZoomChartIn:
#ifdef DEBUG_CHILDWINDOW
			debuglog << "ChartZoomIn";
#endif
			props->increaseChartLogZoom( 1 );
			zoomChanged();
		break;
		case CommandId::ZoomChartOut:
			props->increaseChartLogZoom( -1 );
			zoomChanged();
		break;
		case CommandId::ZoomChartOriginal:
#ifdef DEBUG_CHILDWINDOW
			debuglog << "FontZoomOriginal";
#endif
			props->setChartLogZoom( 0 );
			zoomChanged();
		break;

		case CommandId::Animate:
			if ( props->getRadixHoroscope() )
			{
				props->toggleAnimation();
				StartAniTimer( props->isAnimated() );
			}
		break;
		default:
			warnlog << "ChildWindow::dispatchApplicationWindowCommand received unsupported command " << command;
		break;
	}
}

/*****************************************************
**
**   ChildWindow   ---   dispatchViewpointCommand
**
******************************************************/
void ChildWindow::dispatchViewpointCommand( const CommandId &command )
{
#ifdef DEBUG_CHILDWINDOW
	debuglog << "ChildWindow::dispatchViewpointCommand commandId " << command;
#endif
	Viewpoint newviewpoint = Viewpoint::None;	
	switch( command )
	{
		case CommandId::ViewpointVedic:
			newviewpoint = Viewpoint::Vedic;	
		break;
		case CommandId::ViewpointWestern:
			newviewpoint = Viewpoint::Western;	
		break;
		case CommandId::ViewpointUranian:
			newviewpoint = Viewpoint::Uranian;	
		break;
		case CommandId::ViewpointAstronomical:
			newviewpoint = Viewpoint::Astronomical;	
		break;
		default:
			assert( false );
		break;
	}
#ifdef DEBUG_CHILDWINDOW
	debuglog << "ChildWindow::dispatchViewpointCommand: old viewpoint " << props->getEffectiveViewpoint( Viewpoint::None )
		<< " new " << (int)newviewpoint;
#endif
	assert_valid_enum<Viewpoint>( newviewpoint );

	switch( astroconfig->general.viewpointChangePolicy )
	{
		case ViewpointChangePolicy::Fixed:
			errorlog << "cannot change viewpoint because of fixed change policy";
		break;
		case ViewpointChangePolicy::Medium:
		case ViewpointChangePolicy::Flexible:
		{
			props->setPreferredViewpoint( newviewpoint );
			OnDataChanged();
		}
		break;
		default:
			fatallog << "wrong viewpoint change policy " << (int)astroconfig->general.viewpointChangePolicy;
			assert( false );
		break;
	}
}

/*****************************************************
**
**   ChildWindow   ---   save
**
******************************************************/
bool ChildWindow::save()
{
	Horoscope_ptr h = props->getRadixHoroscope();
	assert( h );

	if ( h->dl.filename.IsEmpty())
	{
		return saveAs();
	}
	else if ( h->dl.filename.Right( 4 ).Lower() == ".atx" )
	{
		h->dl.filename = h->dl.filename.Left( h->dl.filename.Len() - 4 ) + ".mtx";
		return saveAs();
	}
	else
	{
		FileConfig::get()->backupFile( h->dl.filename );
		try
		{
			h->save();
			h->dl.dirty = false;
			WindowDirectory::get()->horoscopeSaved( h );
		}
		catch( std::exception &e )
		{
			const wxString s = tr( "Cannot save file" ) + " " + h->dl.filename + ", " + e.what();
			doMessageBox( this, s, wxOK | wxICON_ERROR | wxCENTRE );
			return false;
		}
	}
	return true;
}

/*****************************************************
**
**   ChildWindow   ---   saveAs
**
******************************************************/
bool ChildWindow::saveAs()
{
	Horoscope_ptr h = props->getRadixHoroscope();
	assert( h );

	bool ret = false;

	wxString filename = h->dl.filename;
	if ( filename.empty())
	{
		if ( h->dl.name.empty()) filename = "unknown";
		else filename = h->dl.name;
		filename += ".mtx";
	}
	wxFileName wxfn( filename );

	wxString filetypes = "Maitreya text (*.mtx)|*.mtx| All files (*.*)|*.*";

	wxFileDialog saveFileDialog( this, tr("Save file as" ),
		gdiconfig->prefs.defSavePath, wxfn.GetFullName(), filetypes, wxFD_SAVE, wxDefaultPosition );

	if ( saveFileDialog.ShowModal() == wxID_OK )
	{
		filename = saveFileDialog.GetPath();
		try
		{
			h->saveAs( filename );
			h->dl.dirty = false;
			WindowDirectory::get()->horoscopeSaved( h );
			ret = true;
			gdiconfig->prefs.defSavePath = saveFileDialog.GetDirectory();
		}
		catch( std::exception &e )
		{
			const wxString s = tr( "Cannot save file" ) + " " + filename + ", " + e.what();
			doMessageBox( this, s, wxOK | wxICON_ERROR | wxCENTRE );
		}
	}
	return ret;
}

/*****************************************************
**
**   ChildWindow   ---   editData
**
******************************************************/
void ChildWindow::editData()
{
	Horoscope_ptr h = props->getRadixHoroscope();
	assert( h );
	BirthDataDialog( this, h ).ShowModal();
	//BirthDataDialog( GetParent(), h ).ShowModal();
}

/*****************************************************
**
**   ChildWindow   ---   doQuickPrint
**
******************************************************/
void ChildWindow::doQuickPrint()
{
	Horoscope_ptr h = props->getRadixHoroscope();
	assert( h );
	PdfLauncher().printHoroscope( h );
}

/*****************************************************
**
**   ChildWindow   ---   doPrint
**
******************************************************/
void ChildWindow::doPrint()
{
	Horoscope_ptr h = props->getRadixHoroscope();
	assert( h );
	PdfLauncher().printHoroscope( h, true );
}

/*****************************************************
**
**   ChildWindow   ---   getHoroscope
**
******************************************************/
Horoscope_ptr ChildWindow::getHoroscope() const
{
	assert( props );
	return props->getRadixHoroscope();
}
 
