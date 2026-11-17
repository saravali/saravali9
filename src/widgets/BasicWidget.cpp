/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/BasicWidget.cpp
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

#include "BasicWidget.h"

#include "CategoryHelper.h"
#include "ChartGeometry.h"
#include "ConfigLoaderImpls.h"
#include "ContextProperties.h"
#include "Commands.h"
#include "CommonControls.h"
#include "enumserializer.h"
#include "ExportDialog.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "Lang.h"
#include "MenuProvider.h"
#include "Painter.h"
#include "ToolPanel.h"

#include <wx/clipbrd.h>
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/graphics.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>

extern Gdiconfig *gdiconfig;

enum { TOOLTIP_TIMER = wxID_HIGHEST + 1 };

#define DEBUG_BASICWIDGET

/*****************************************************
**
**   BasicWidget   ---   Constructor
**
******************************************************/
BasicWidget::BasicWidget( wxWindow *parent, ContextProps_ptr props, const WidgetCategory& category, const ToolbarWidgetId toolbarId )
	: wxScrolledWindow( parent ),
	props( props ),
	category( category ),
	configkey( enum_to_string( category ))
{
	assert( props );
	debuglog << "CTRL " << (int)toolbarId;

	exportMode = false;
	dragMode = false;
	SetScrollRate( 1, 1 );
	unscrolledMousePosition = mousePosition = 0;
	mouseInside = false;

	mainsizer = new wxBoxSizer( wxVERTICAL );
	//SetSizer( mainsizer );
	if ( toolbarId != ToolbarWidgetId::None )
	{
#ifdef DEBUG_BASICWIDGET
		debuglog << "BasicWidget add toolbar id " << (int)toolbarId;
#endif
		toolbar = ToolbarFactory::get()->createToolbar( this, toolbarId );
		mainsizer->Add( toolbar, 0, wxEXPAND );

		// toolbarLabelCtrl will receive echo events if set
    toolbarLabelCtrl = getToolbarControl<ToolbarLabelCtrl>( ToolbarCmd::ToolbarLabel );
#ifdef DEBUG_BASICWIDGET
		debuglog << "BasicWidget " << category << " add toolbar toolbarLabelCtrl " << toolbarLabelCtrl << " this " << this;
#endif
	}
		SetAcceleratorTable( CommandHandler::get()->getAcceleratorTable() );

//TODO siehe 12673
#if 0
#if defined( __WXGTK__ )
	if ( toolbarId == ToolbarWidgetId::None )
	{
		SetAcceleratorTable( CommandHandler::get()->getAcceleratorTable() );
	}
#else
  SetAcceleratorTable( CommandHandler::get()->getAcceleratorTable() );
#endif
#else
  //SetAcceleratorTable( CommandHandler::get()->getAcceleratorTable() );
#endif

	setWidgetActions( WidgetActionBundle::Default );

	// Toolbar commands
	Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( BasicWidget::OnToolbarCommand ));
	Connect( wxEVT_SPINCTRL, wxCommandEventHandler( BasicWidget::OnToolbarCommand ));

	Connect( COMMAND_DIALOGCTRL_CHANGED, wxCommandEventHandler( BasicWidget::OnToolbarCommand ));

	Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( BasicWidget::OnKeyDown ));
	Connect( wxEVT_KEY_UP, wxKeyEventHandler( BasicWidget::OnKeyUp ));
	Connect( (int)CommandId::ChildFirst+1, (int)CommandId::ChildLast, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler( BasicWidget::OnCommand ));
}

/*****************************************************
**
**   BasicWidget   ---   postCreate
**
******************************************************/
void BasicWidget::postCreate()
{
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::postCreate";
#endif
}

/*****************************************************
**
**   BasicWidget   ---   OnToolbarCommand
**
******************************************************/
void BasicWidget::OnToolbarCommand()
{
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::OnToolbarCommand";
#endif
	OnDataChanged();
}

/*****************************************************
**
**   BasicWidget   ---   sendUpdateTitleEvent
**
******************************************************/
void BasicWidget::sendUpdateTitleEvent()
{
	wxCommandEvent e( WIDGET_UPDATE_TITLE_QUERY, GetId());
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   BasicWidget   ---   initMouseEventhandlers
**
******************************************************/
void BasicWidget::initMouseEventhandlers()
{
	Connect( wxEVT_MOTION, wxMouseEventHandler( BasicWidget::OnMouseMotion ));
	Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( BasicWidget::OnMouseWheelEvent ));
	Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( BasicWidget::OnMouseLeave ));
	Connect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( BasicWidget::OnMouseEnter ));
	Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( BasicWidget::OnMouseLeftDown ));
	Connect( wxEVT_LEFT_UP, wxMouseEventHandler( BasicWidget::OnMouseLeftUp ));
	Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( BasicWidget::OnMouseDoubleClick ));
	Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( BasicWidget::OnContextMenuEvent ));
}

/*****************************************************
**
**   BasicWidget   ---   simple getters and setters
**
******************************************************/
wxString BasicWidget::getTitle( const bool /*shortname*/ )
{
	return Lang().getWidgetCategoryName( category );
}

ContextProps_ptr BasicWidget::getProps() const
{
	return props;
}

Viewpoint BasicWidget::getViewpoint() const
{
// test for completeness for IDS in getStickyVp
#if 0
	for( const WidgetCategory cat : Enum<WidgetCategory>())
	{
		ViewpointHelper::get()->getStickyVp( cat );
	}
#endif

	ViewpointHelper *vh = ViewpointHelper::get();
	const Viewpoint stickyVp = vh->getStickyVp( category );
	//const Viewpoint viewpoint = vh->getViewpointByStickyVp( props->getPreferredViewpoint(), stickyVp );
	const Viewpoint viewpoint = props->getEffectiveViewpoint( stickyVp );
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::getViewpoint category " << category << " stickyVp " << stickyVp
		<< " result vp " << viewpoint;
#endif
	return viewpoint;
}

/**************************************************************
***
**   BasicWidget   ---   setWidgetActions
***
***************************************************************/
void BasicWidget::setWidgetActions( const WidgetActionBundle &actions )
{
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::setWidgetActions " << (int)actions;
#endif
	switch( actions )
	{
		case WidgetActionBundle::None:
		break;
		case WidgetActionBundle::Default:
		case WidgetActionBundle::Bardiagram:
			mouseWheelActions = 
			{
				{ KeyboardModfier::KbNone, WidgetAction::Scroll },
				{ KeyboardModfier::KbShift, WidgetAction::ChangeSkin },
				{ KeyboardModfier::KbCtrl, WidgetAction::ChangeZoom },
			};
		break;
		case WidgetActionBundle::Chart:
			mouseWheelActions = 
			{
				{ KeyboardModfier::KbNone, WidgetAction::ChangeGeometry },
				{ KeyboardModfier::KbShift, WidgetAction::ChangeSkin },
				{ KeyboardModfier::KbCtrl, WidgetAction::ChangeZoom },
				{ KeyboardModfier::KbShift + KeyboardModfier::KbCtrl, WidgetAction::ChangeChartZoom },
			};
		break;
		case WidgetActionBundle::Text:
		break;
		default:
			errorlog << "invalid widget action bundle ID " << (int)actions;
		break;
	}
}

/*****************************************************
**
**   BasicWidget   ---   OnDataChanged
**
******************************************************/
void BasicWidget::OnDataChanged()
{
	guiHasChanged();
	Refresh();
}

/*****************************************************
**
**   BasicWidget   ---   OnKeyUp
**
******************************************************/
void BasicWidget::OnKeyUp( wxKeyEvent &event )
{
	const int keycode = event.GetKeyCode();
#ifdef DEBUG_BASICWIDGET
	debuglog << "KEY UP in BasicWidget::OnKeyUp code " << keycode;
#endif

	switch( keycode )
	{
		case WXK_SHIFT:
		case WXK_CONTROL:
		case WXK_ALT:
			dispatchResponsiveEvent();
			//mouseHasMoved( false );
		break;
		default:
		break;
	}
}

/*****************************************************
**
**   BasicWidget   ---   OnKeyDown
**
******************************************************/
void BasicWidget::OnKeyDown( wxKeyEvent &event )
{
	const int keycode = event.GetKeyCode();
#ifdef DEBUG_BASICWIDGET
	debuglog << "KEY DOWN in BasicWidget::OnKeyDown code " << keycode;
#endif

	switch( keycode )
	{
#if 0
		case WXK_UP:
		case WXK_DOWN:
		{
			const int keymod = getCurrentKeyMod();
			if ( mouseWheelActions.find( keymod ) != mouseWheelActions.end())
			{
				doAction( mouseWheelActions[keymod], event.GetKeyCode() == WXK_UP ? 1 : -1 );
				OnDataChanged();
			}
			else
			{
#ifdef DEBUG_BASICWIDGET
				debuglog << "no action for wheel + keymod " << keymod;
#endif
			}
		}
		break;

		case WXK_LEFT:
		case WXK_RIGHT:
		case WXK_PAGEDOWN:
		case WXK_PAGEUP:
			//context->getProps()->processNavigationKey( wxKeyEventToCommandId( keycode, false ));
			//OnDataChanged();
		break;
#endif

		case WXK_SHIFT:
		case WXK_CONTROL:
		case WXK_ALT:
#ifdef DEBUG_BASICWIDGET
			debuglog << "KEY DOWN in BasicWidget::OnKeyDown ALT/SHIFT/CTRL";
#endif
			dispatchResponsiveEvent();
			//mouseHasMoved( false );
		break;
		default:
		break;
	}
}

/*****************************************************
**
**   BasicWidget  ---   OnMouseMotion
**
******************************************************/
void BasicWidget::OnMouseMotion( wxMouseEvent& )
{
	wxMouseState mst = wxGetMouseState();
	const wxPoint p = ScreenToClient( wxPoint( mst.GetX(), mst.GetY()));

#ifdef DEBUG_BASICWIDGET
	static int count = 0;
	debuglog << "BasicWidget::OnMouseMotion " << count++ << " p " << p.x << "/" << p.y;
#endif
	
	if ( dragMode )
	{
		Scroll( mousePosition.real() - p.x , mousePosition.imag() - p.y );
	}

	int vx, vy;
	GetViewStart( &vx, &vy );
	unscrolledMousePosition = MComplex( p.x, p.y );
	mousePosition = MComplex( p.x + vx, p.y + vy );

	dispatchResponsiveEvent();
}

/*****************************************************
**
**   BasicWidget  ---   dispatchResponsiveEvent
**
******************************************************/
void BasicWidget::dispatchResponsiveEvent()
{
	if ( ! gdiconfig->view.doHandleResponsiveWidgetEvents ) return;

	const WidgetResponse wr = handleResponsiveChartEvent( mousePosition, getCurrentKeyMod());
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::dispatchResponsiveEvent doRefresh " << wr.doRefresh << " rect " << wr.refreshRect
		<< " message " << wr.message << " coordinatesChanged " << wr.coordinatesChanged;
#endif
	if ( wr.coordinatesChanged )
	{
		SendSizeEvent();
		Refresh();
	}
	else if ( wr.doRefresh )
	{
		if ( wr.refreshRect.isNil()) { Refresh(); }
		else
		{
			// rect must be translated to unscrolled coordinates
			int vx, vy;
			GetViewStart( &vx, &vy );
			const wxRect rect( wr.refreshRect.x - vx, wr.refreshRect.y - vy, wr.refreshRect.width, wr.refreshRect.height );
			RefreshRect( rect );
		}
	}

	if ( ! wr.message.IsEmpty())
	{
		#ifdef DEBUG_BASICWIDGET
		debuglog << "BasicWidget::dispatchResponsiveEvent send echo event message " << wr.message;
		#endif
		wxCommandEvent e( COMMAND_ECHO_MESSAGE, GetId());
		e.SetString( wr.message );
		wxPostEvent( GetParent(), e );
	}
}

/**************************************************************
***
**   BasicWidget   ---   OnMouseLeftDown
***
***************************************************************/
void BasicWidget::OnMouseLeftDown( wxMouseEvent& )
{
	SetFocus();
	dragMode = true;

	const WidgetResponse wr = handleMouseLeftClick( mousePosition, getCurrentKeyMod());
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::OnMouseLeftDown doRefresh " << wr.doRefresh << " rect " << wr.refreshRect
		<< " notify " << wr.notifyParent;
#endif
	if ( wr.coordinatesChanged )
	{
		SendSizeEvent();
		Refresh();
	}
	else if ( wr.doRefresh )
	{
		if ( wr.refreshRect.isNil()) { Refresh(); }
		else { RefreshRect( wr.refreshRect.toWxRect()); }
	}
	if ( wr.notifyParent )
	{
		wxCommandEvent e( WIDGET_NOTIFY_PARENT, GetId());
		wxPostEvent( GetParent(), e );
	}
}

/**************************************************************
***
**   BasicWidget   ---   OnMouseLeftUp
***
***************************************************************/
void BasicWidget::OnMouseLeftUp( wxMouseEvent& )
{
	dragMode = false;
}

/**************************************************************
***
**   BasicWidget   ---   OnMouseDoubleClick
***
***************************************************************/
void BasicWidget::OnMouseDoubleClick( wxMouseEvent& )
{
	const WidgetResponse wr = handleLeftDoubleClick( mousePosition, getCurrentKeyMod());
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::OnMouseDoubleClick doRefresh " << wr.doRefresh << " rect " << wr.refreshRect
		<< " notify " << wr.notifyParent;
#endif

	if ( wr.coordinatesChanged )
	{
		SendSizeEvent();
		Refresh();
	}
	else if ( wr.doRefresh )
	{
		if ( wr.refreshRect.isNil()) { Refresh(); }
		else { RefreshRect( wr.refreshRect.toWxRect()); }
	}
	if ( wr.notifyParent )
	{
		wxCommandEvent e( WIDGET_NOTIFY_PARENT, GetId());
		wxPostEvent( GetParent(), e );
	}
}

/**************************************************************
***
**   BasicWidget   ---   OnMouseLeave
***
***************************************************************/
void BasicWidget::OnMouseLeave( wxMouseEvent &event )
{
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::OnMouseLeave x " << event.GetX() << " y " << event.GetY();
#endif
	dragMode = false;
	mouseInside = false;

	unscrolledMousePosition = mousePosition = 0;
	dispatchResponsiveEvent();
	//mouseHasMoved( true );
}

/**************************************************************
***
**   BasicWidget   ---   OnMouseEnter
***
***************************************************************/
void BasicWidget::OnMouseEnter( wxMouseEvent &event )
{
	int vx, vy;
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::OnMouseEnter x " << event.GetX() << " y " << event.GetY();
#endif
	unscrolledMousePosition = MComplex( event.GetPosition().x, event.GetPosition().y );
	GetViewStart( &vx, &vy );
	mousePosition = MComplex( event.GetPosition().x + vx, event.GetPosition().y + vy );
	mouseInside = true;

	dispatchResponsiveEvent();
	//mouseHasMoved( false );
}

/*****************************************************
**
**   BasicWidget   ---   OnMouseWheelEvent
**
******************************************************/
void BasicWidget::OnMouseWheelEvent( wxMouseEvent &event )
{
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::OnMouseWheelEvent offset " << event.GetWheelRotation();
#endif

#if defined( __WXMAC__ )
	if ( fabs( event.GetWheelRotation() ) < 3 ) return;
#endif

	const int keymod = getCurrentKeyMod();
	if ( mouseWheelActions.find( keymod ) != mouseWheelActions.end())
	{
		doAction( mouseWheelActions[keymod], event.GetWheelRotation());
	}
	else
	{
#ifdef DEBUG_BASICWIDGET
		debuglog << "no action for wheel + keymod " << keymod;
#endif
		event.Skip();
	}
}

/**************************************************************
***
**   BasicWidget   ---   doAction
***
***************************************************************/
void BasicWidget::doAction( const WidgetAction &action, const int &offset )
{
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::doAction id " << (int)action;
#endif
	switch( action )
	{
		case WidgetAction::Scroll:
			doScroll( offset );
		break;
		case WidgetAction::ChangeZoom:
			props->increaseLogZoom( offset > 0 ? 1 : -1 );
			changeZoom();
		break;
		case WidgetAction::ChangeChartZoom:
			props->increaseChartLogZoom( offset > 0 ? 1 : -1 );
			OnDataChanged();
		break;
		case WidgetAction::ChangeSkin:
			changeSkin( offset );
		break;
		case WidgetAction::ChangeGeometry:
			changeGeometry( offset );
		break;
		case WidgetAction::PropagateEvent:
			propagateAction( offset );
		break;
		default:
			warnlog << "BasicWidget::doAction no action assigned to id " << (int)action;
		break;
	}
	Refresh();
}

/**************************************************************
***
**   BasicWidget   ---   doScroll
***
***************************************************************/
void BasicWidget::doScroll( const int &offset )
{
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::doScroll " << offset;
#endif
	int x, y, x0, y0, xs, ys;
	const int value = 60;

	GetViewStart( &x, &y );      // scroll position
	GetSize( &xs, &ys );         // size of visible widget only
	GetVirtualSize( &x0, &y0 );  // complete size of widget

	// uncommented version scroll in x-direction of y is not possible (makes sense for GDasa)
	if ( offset < 0 )
	{
		if ( y + ys >= y0 ) x = std::min( x0, x + value );
		else y += value;
		y = std::min( y0, y + value );
	}
	else
	{
		if ( y > 0 ) y = std::max ( 0, y - value );
		else x -= value;
		y = std::max ( 0, y - value );
	}
	Scroll( x, y );
}

/**************************************************************
***
**   BasicWidget   ---   changeZoom
***
***************************************************************/
void BasicWidget::changeZoom()
{
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::changeZoom " << props->getLogZoom() << " mouse " << mousePosition;
#endif

	//context->getProps()->increaseZoom( offset > 0 ? 1 : -1 );
	//OnDataChanged();
	wxCommandEvent e( ZOOM_CHANGED, GetId());
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   BasicWidget   ---   changeGeometry
**
******************************************************/
void BasicWidget::changeGeometry( const int &offset )
{
	const WidgetCategory cat = getWidgetCategory();
	const WidgetCategory cat2 = CategoryHelper::get()->getGeomCategory( cat );
	uint geomid = props->getGeometry( cat );
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::changeGeometry offset " << offset
		<< " geom id from props " << (int)geomid << " widget category " << cat << " corrected cat " << cat2;
#endif
	ChartGeometryLoader *loader = ChartGeometryLoader::get();

	if ( offset < 0 )
	{
		geomid = loader->findNextGeometry( cat2, geomid );
	}
	else
	{
		geomid = loader->findPreviousGeometry( cat2, geomid );
	}
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::changeGeometry new geom id " << (int)geomid;
#endif

	props->setGeometry( cat, geomid );
	OnDataChanged();

	// needed by vedic/WesternChartPanel to change selection in skin choice
	wxCommandEvent e( COMMAND_CHART_SKIN_CHANGED, GetId());
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   BasicWidget   ---   changeSkin
**
******************************************************/
void BasicWidget::changeSkin( const int &offset )
{
	const WidgetCategory cat = getWidgetCategory();
	const uint num_skins = WidgetSkinLoader::get()->getSize();
	uint skin = props->getSkin( cat );
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::changeSkin skin " << skin << " offset " << offset;
#endif
	if ( offset < 0 )
	{
		++skin;
		if ( skin >= num_skins ) skin = 0;
	}
	else
	{
		if ( skin == 0 ) skin = num_skins - 1;
		else --skin;
	}
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::changeSkin NEW skin " << skin;
#endif
	props->setSkin( cat, skin );

	// needed by vedic/WesternChartPanel to change selection in skin choice
	wxCommandEvent e( COMMAND_CHART_SKIN_CHANGED, GetId());
	wxPostEvent( GetParent(), e );
}

/**************************************************************
***
**   BasicWidget   ---   propagateAction
***
***************************************************************/
void BasicWidget::propagateAction( const int &offset )
{
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::propagateAction " << offset;
#endif
	if ( offset > 0 )
	{
		wxCommandEvent event( MOUSE_WHEEL_DOWN, GetId() );
		wxPostEvent( GetParent(), event );
	}
	else
	{
		wxCommandEvent event( MOUSE_WHEEL_UP, GetId() );
		wxPostEvent( GetParent(), event );
	}
}

/*****************************************************
**
**   BasicWidget   ---   OnEraseBackground
**
******************************************************/
void BasicWidget::OnEraseBackground( wxEraseEvent& )
{
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::OnEraseBackground";
#endif
}

/*****************************************************
**
**   BasicWidget   ---   OnPaint
**
******************************************************/
void BasicWidget::OnPaint( wxPaintEvent& )
{
	if ( ! props->isValid())
	{
		warnlog << "BasicWidget::OnPaint received paint event on invalid widget, ignore";
		return;
	}

	int vx, vy;
	wxRect refreshRect;

	wxRegionIterator upd( GetUpdateRegion());
	//if ( upd ) refreshRect = upd.GetRect();

	while ( upd )
	{
		refreshRect.Union( upd.GetRect() );
		upd++;
	}
	GetViewStart( &vx, &vy );
	const MRect r = MRect( refreshRect ).translate( vx, vy );
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::createPainter -- REFRESHRECT: " << r << " vx " << vx << " vy " << vy;
#endif

	wxPaintDC dc( this );
	wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	assert( gc );
	Painter *painter = PainterFactory().createGcPainter( gc );
	assert( painter );

	gc->Translate( -vx, -vy );
	doPaint( painter, r );
	delete painter;
	delete gc;
}

/*****************************************************
**
**   BasicWidget   ---   OnCommand
**
******************************************************/
void BasicWidget::OnCommand( wxCommandEvent &event )
{
	bool datachanged = false;
	bool finished = false;

	const CommandId command = (CommandId)event.GetId();
#ifdef DEBUG_BASICWIDGET
	debuglog << "Start BasicWidget::OnCommand  viewpoint " << getViewpoint() << " command " << command;
#endif

	if ( command >= CommandId::ObToggleFirstPlanet && command <= CommandId::ObToggleLastPlanet )
	{
#ifdef DEBUG_BASICWIDGET
		const PlanetId planetId  = (PlanetId)( (int)command - (int)CommandId::ObToggleFirstPlanet );
		debuglog << "BasicWidget::OnCommand  Toggle planet " << planetId;
#endif
		props->togglePlanetId( (PlanetId)( (int)command - (int)CommandId::ObToggleFirstPlanet ), getViewpoint() );
		datachanged = true;
	}

  switch ( command )
	{
		case CommandId::ColToggleLatitude:
		case CommandId::ColToggleSpeed:
		case CommandId::ColToggleDeclination:
		case CommandId::ColToggleT4Longitude:
		case CommandId::ColToggleT8Longitude:
		case CommandId::ColToggleT16Longitude:
		case CommandId::ColToggleAntiscia:
		case CommandId::ColToggleLord:
		case CommandId::ColToggleDiginity:
		case CommandId::ColToggleNavamsa:
		case CommandId::ColToggleKaraka:
		case CommandId::ColToggleShastiamsa:
		case CommandId::ColToggleNakshatra:
		case CommandId::ColToggleDasavarga:
		case CommandId::ColToggleHousePos:
		case CommandId::ColToggleKp:
		case CommandId::ColToggleAv:
		case CommandId::ColTogglePada:
		case CommandId::ColToggleElement:
		case CommandId::ColToggleSignification:
		case CommandId::ColToggleSignQuality:
		{
			const ColumnGroup group = ViewpointHelper::get()->getColumnGroup( command );
			props->toggleColumnGroups( group, getViewpoint() );
			datachanged = true;
			finished = true;
		}
		break;

		case CommandId::ExportImageJpeg:
		case CommandId::ExportImagePng:
		case CommandId::ExportImageSvg:
			doImageToFileExport( command );
			finished = true;
		break;

		case CommandId::ExportImageClipboard:
			doClipboardExport();
			finished = true;
		break;

    case CommandId::NextSkin:
      props->increaseSkin( getWidgetCategory(), 1 );
			OnDataChanged();
			finished = true;
    break;
    case CommandId::PreviousSkin:
      props->increaseSkin( getWidgetCategory(), -1 );
			OnDataChanged();
			finished = true;
    break;
    case CommandId::NextGeometry:
      props->increaseGeometry( getWidgetCategory(), 1 );
			OnDataChanged();
			finished = true;
    break;
    case CommandId::PreviousGeometry:
      props->increaseGeometry( getWidgetCategory(), -1 );
			OnDataChanged();
			finished = true;
    break;


		case CommandId::ZoomIn:
			props->increaseLogZoom( 1 );
			OnDataChanged();
			finished = true;
		break;
		case CommandId::ZoomOut:
			props->increaseLogZoom( -1 );
			OnDataChanged();
			finished = true;
		break;

		default:

			// chart geometry
			if ( command >= CommandId::ChartGeom && command <= CommandId::ChartGeomLast )
			{
				const WidgetCategory cat = getWidgetCategory();
				const WidgetCategory cat2 = CategoryHelper::get()->getGeomCategory( cat );

#ifdef DEBUG_BASICWIDGET
				debuglog << "BasicWidget::OnCommand GEOM " << event.GetId();
#endif
				const std::vector<std::pair<uint,wxString>> ids
					= ChartGeometryLoader::get()->getAllIdsAndNames( cat2 );
				const int index = (int)command - (int)CommandId::ChartGeom;
				assert( index < (int)ids.size());

				//props->setGeometry( getWidgetCategory(), ids[index].first );
				props->setGeometry( cat, ids[index].first );
				datachanged = true;
				finished = true;
			}
			// skin change
			else if ( command >= CommandId::ChartSkin && command <= CommandId::ChartSkinLast )
			{
#ifdef DEBUG_BASICWIDGET
				debuglog << "BasicWidget::OnCommand SKIN " << event.GetId();
#endif
				props->setSkin( getWidgetCategory(), (int)command - (int)CommandId::ChartSkin );

				// only refresh in this case and no propagation to parent
				Refresh();
				finished = true;
			}
		break;
	}

	if ( datachanged )
	{
		// propagate change event, caught by ChildWindow (calling OnDataChanged)
#ifdef DEBUG_BASICWIDGET
		debuglog << "BasicWidget::OnCommand propagate change event";
#endif
		wxCommandEvent e( WIDGET_DATA_CHANGED, GetId());
		wxPostEvent( GetParent(), e );
	}

	// continue processing if not finished
	event.Skip( ! finished );

#ifdef DEBUG_BASICWIDGET
		debuglog << "Finished BasicWidget::OnCommand";
#endif
}

/*****************************************************
**
**   BasicWidget   ---   OnContextMenuEvent
**
******************************************************/
void BasicWidget::OnContextMenuEvent( wxMouseEvent& event)
{
	int x, y;
	x = event.m_x;
	y = event.m_y;
	wxWindow *window = (wxWindow*)event.GetEventObject();
	window->ClientToScreen( &x, &y );
	this->ScreenToClient( &x, &y );

	wxMenu *menu = ContextMenuProvider().getWidgetMenu( this );
	PopupMenu( menu, x, y );
	delete menu;
}

/*****************************************************
**
**   BasicWidget   ---   doEcho
**
******************************************************/
void BasicWidget::doEcho( const wxString &message )
{
#ifdef DEBUG_BASICWIDGET
  debuglog << "BasicWidget::doEcho message \"" << message << "\" toolbarLabelCtrl set "
		<< std::boolalpha  << (bool)toolbarLabelCtrl
		<< " category " << category << " this " << this;
#endif

  if ( props->isValid() )
  {
		if ( toolbarLabelCtrl )
    {
#ifdef DEBUG_BASICWIDGET
      debuglog << "BasicWidget::doEcho to ToolbarLabel: " << message;
#endif
      toolbarLabelCtrl->SetLabel( message );
    }
    else
    {
			if ( message.IsEmpty())
			{
				UnsetToolTip();
			}
			else
			{
				SetToolTip( message );
			}
    }
  }
}

/*****************************************************
**
**   BasicWidget   ---   doPdfExport
**
******************************************************/
void BasicWidget::doPdfExport()
{
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::doPdfExport";
#endif
	wxString filename = "out.pdf";

	doMessageBox( this, "should no appear" );

	int style = wxFD_SAVE;
	if ( gdiconfig->view.exportAskOnOverwrite ) style |= wxFD_OVERWRITE_PROMPT;

	const static wxString filetypes =
	    "PDF (*.pdf)|*.pdf|All files (*)| *.*";

	wxFileDialog exportFileDialog( this, tr( "Export as Pdf" ),
		gdiconfig->prefs.defExportPath, filename, filetypes, style, wxDefaultPosition );
	exportFileDialog.SetFilterIndex( 0 );

	if ( exportFileDialog.ShowModal() == wxID_OK )
	{
		filename = exportFileDialog.GetPath();
	}
}

/*****************************************************
**
**   BasicWidget   ---   getImageExportSize
**
******************************************************/
wxSize BasicWidget::getImageExportSize()
{
	int xsize, ysize;

	GetVirtualSize( &xsize, &ysize );

	if ( gdiconfig->view.graphicExportSizeMode == 0 )
	{
		ExportDialog dialog( this, xsize, ysize );
		if ( dialog.ShowModal() != wxID_OK ) return wxSize( xsize, ysize );
		dialog.getSizes( xsize, ysize );
		gdiconfig->view.graphicExportSizeMode = dialog.getCheckShowDialog();
	}
	return wxSize( xsize, ysize );
}

/*****************************************************
**
**   BasicWidget   ---   createImageBitmap
**
******************************************************/
wxBitmap BasicWidget::createImageBitmap( const wxSize &size )
{
	wxBitmap bitmap( size );
	wxMemoryDC dc;
	dc.SelectObject( bitmap );
	wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	assert( gc );
	Painter *painter = PainterFactory().createGcPainter( gc );
	assert( painter );

	exportMode = true;
	MRect refreshRect( 0, 0, size.x, size.y );
	doPaint( painter, refreshRect );
	exportMode = false;
	delete painter;
	delete gc;

	return bitmap;
}

/*****************************************************
**
**   BasicWidget   ---   doClipboardExport
**
******************************************************/
void BasicWidget::doClipboardExport()
{
	const wxSize size = getImageExportSize();
#ifdef DEBUG_BASICWIDGET
	debuglog << "BasicWidget::doClipboardExport category " << category << " size " << size;
#endif

	if ( wxTheClipboard->Open() )
	{
		wxTheClipboard->Clear();

		const wxBitmap bitmap = createImageBitmap( size );
		if ( bitmap.IsOk())
		{
			wxTheClipboard->SetData( new wxBitmapDataObject( createImageBitmap( size ) ));
		}
		else
		{
			errorlog << "Bitmap object cannot be copied to clipboard";
		}
		wxTheClipboard->Flush();
		wxTheClipboard->Close();
		doNotShowAgainMessage( this, DoNotShowAgainTopic::ImageExportSuccessful, tr( "Image exported to clipboard" ), true );
	}
	else
	{
		errorlog << "Cannot open clipboard";
	}
}

/*****************************************************
**
**   BasicWidget   ---   doImageToFileExport
**
******************************************************/
void BasicWidget::doImageToFileExport( const CommandId &commandId )
{
	wxString filename;
#ifdef DEBUG_BASICWIDGET
		debuglog << "BasicWidget::doImageToFileExport command " << commandId;
#endif

	const static wxString filetypes =
	    "PNG (*.png)|*.png|JPG (*.jpg)|*.jpg|SVG (*.svg)|*.svg|All files (*)| *.*";

	int filterIndex = 0;
	switch ( commandId )
	{
		case CommandId::ExportImageJpeg:
			filename = "out.jpg";
			filterIndex = 1;
		break;
		case CommandId::ExportImageSvg:
			filename = "out.svg";
			filterIndex = 2;
		break;
		default:
			filename = "out.png";
		break;
	}
	int style = wxFD_SAVE;
	if ( gdiconfig->view.exportAskOnOverwrite ) style |= wxFD_OVERWRITE_PROMPT;

	wxFileDialog exportFileDialog( this, tr( "Export Picture" ),
		gdiconfig->prefs.defExportPath, filename, filetypes, style, wxDefaultPosition );
	exportFileDialog.SetFilterIndex( filterIndex );

	int wxtype = wxBITMAP_TYPE_INVALID;
	if ( exportFileDialog.ShowModal() == wxID_OK )
	{
		filename = exportFileDialog.GetPath();
		const wxString e = filename.Right(4).MakeLower();
		if ( e == ".png" )
		{
			wxtype = wxBITMAP_TYPE_PNG;
		}
		else if ( e == ".jpg" || e == "jpeg" )
		{
			wxtype = wxBITMAP_TYPE_JPEG;
		}
		else
		{
			wxString s = tr( "Cannot export image, wrong image extension" ) + " " + e;
			doMessageBox( this, s, wxOK | wxICON_ERROR );
			return;
		}
		gdiconfig->prefs.defExportPath = exportFileDialog.GetDirectory();

		const wxSize size = getImageExportSize();
		const wxBitmap bitmap = createImageBitmap( size );

		if ( bitmap.SaveFile( filename, (wxBitmapType)wxtype ))
		{
			doNotShowAgainMessage( this, DoNotShowAgainTopic::ImageExportSuccessful,
				std::format( "{} {}", tr("Image exported to"), filename ), true );
		}
	}
}

