/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/DragChart.cpp
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

//#define wxUSE_GENERIC_DRAGIMAGE 1


// wx includes needed first, wxgtk 3.2
#include <wx/dc.h>
#include <wx/dragimag.h>

#include "DragChart.h"

#include "ColorConfig.h"
#include "guibase.h"
#include "Lang.h"
#include "logger.h"
#include "Painter.h"
#include "SymbolProvider.h"
#include "DocHelper.h"

#include <wx/app.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/graphics.h>
#include <wx/image.h>
#include <wx/timer.h>

// size of drag images
#define WDRAGIMAGE 24
#define HDRAGIMAGE 32

// Dragging modes
#define TEST_DRAG_NONE     0
#define TEST_DRAG_START    1
#define TEST_DRAG_DRAGGING 2

#define MESSAGE_TIMEOUT 1000

enum { MESSAGE_TIMER = wxID_HIGHEST + 2500 };


/*****************************************************
**
**   DragChart   ---   Constructor
**
******************************************************/
DragChart::DragChart( wxWindow* parent )
		: wxScrolledWindow( parent, -1, wxDefaultPosition, wxDefaultSize,
			wxHSCROLL | wxVSCROLL | wxFULL_REPAINT_ON_RESIZE )
{
	SetBackgroundColour( *wxWHITE );
	msgtimer = new wxTimer( this, MESSAGE_TIMER );

	addObject( PlanetId::Ascendant );
	for( const PlanetId i : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		addObject( i );
	}
	addObject( PlanetId::AscNode );
	addObject( PlanetId::DescNode );

	SetCursor( wxCursor( wxCURSOR_ARROW ));
	dragMode = TEST_DRAG_NONE;
	draggedShape = 0;
	dragImage = 0;

	Connect( wxEVT_PAINT, wxPaintEventHandler( DragChart::OnPaint ));
	Connect( MESSAGE_TIMER, wxEVT_TIMER, wxTimerEventHandler( DragChart::OnTimer ));
	Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( DragChart::OnMouseLeftDown ));
	Connect( wxEVT_LEFT_UP, wxMouseEventHandler( DragChart::OnMouseLeftUp ));
	Connect( wxEVT_MOTION, wxMouseEventHandler( DragChart::OnMouseDrag ));
}

/*****************************************************
**
**   DragChart   ---   Destructor
**
******************************************************/
DragChart::~DragChart()
{
	ClearShapes();
	if ( dragImage ) delete dragImage;
}

/*****************************************************
**
**   DragChart   ---   getPlanetData
**
******************************************************/
std::map<PlanetId, Sign> DragChart::getPlanetData() const
{
	std::map<PlanetId, Sign> data;
	for( DragShape *shape : displayList )
	{
		if ( shape->getRasipos() != Sign::None ) data[ shape->getPlanetId() ] = shape->getRasipos();
	}
	return data;
}

/*****************************************************
**
**   DragChart   ---   addObject
**
******************************************************/
void DragChart::addObject( const PlanetId &id )
{
	displayList.push_back( new DragShape( id ));
}

/*****************************************************
**
**   DragChart   ---   clearChart
**
******************************************************/
void DragChart::clearChart()
{
	for( DragShape *shape : displayList )
	{
		shape->setRasipos( Sign::None );
	}
}

/*****************************************************
**
**   DragChart   ---   OnPaint
**
******************************************************/
void DragChart::OnPaint( wxPaintEvent& )
{
	int planetsPerSign[12], signStepLength[12];
	wxList::compatibility_iterator node;

	wxCoord w, h;
	GetSize( &w, &h );

	lwidth = 2 * w / 3;
	rwidth = w - lwidth;
	height = h;
	xtol = 3;
	ytol = 3;
	xstep = ( lwidth - ( 2 * xtol )) / 4;
	ystep = ( height - ( 2 * ytol )) / 4;

	wxPaintDC dc( this );
	PrepareDC( dc );

	wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	assert( gc );
	Painter *painter = PainterFactory().createGcPainter( gc );
	assert( painter );
	DocHelper util( painter );

	//painter->setTransparentBrush();
	painter->setDefaultBrush();
	painter->setDefaultPen();
	painter->drawRectangle( 0, 0, w, h );

	drawChart( painter );

	// reset variables
	for( Sign i : Enum<Sign>() )
	{
		planetsPerSign[(int)i] = 0;
		signStepLength[(int)i] = WDRAGIMAGE;
	}

	// calculate number of planets per sign
	for( Sign i : Enum<Sign>() )
	{
		for( DragShape *shape : displayList )
		{
			if ( shape->getRasipos() == i )
			{
				planetsPerSign[(int)i]++;
			}
		}
	}

	// calculate step length per sign
	for( Sign i : Enum<Sign>() )
	{
		wxRect rasirect = getRasiCoord( i );
		if ( planetsPerSign[(int)i] * WDRAGIMAGE > rasirect.width )
		{
			signStepLength[(int)i] = rasirect.width / planetsPerSign[(int)i];
		}
	}

	for( Sign sign : Enum<Sign>( Sign::None, Sign::Last ))
	{
		const wxRect rasirect = getRasiCoord( sign );
		int xleft = rasirect.x;
		int ytop = rasirect.y;

		for( DragShape *shape : displayList )
		{
			if ( shape->getRasipos() == sign )
			{
				shape->SetPosition( wxPoint( xleft, ytop ));

				if ( sign != Sign::None ) xleft += signStepLength[(int)sign];
				else
				{
					xleft += WDRAGIMAGE;
					if ( xleft + WDRAGIMAGE > rasirect.x + rasirect.width )
					{
						xleft = rasirect.x;
						ytop += HDRAGIMAGE;
					}
				}
			}
		}
	}
	DrawShapes( dc );

	const MRect rr( xtol, ytol, lwidth, height );
	debuglog << "DragChart centerString " << centerString << " rect " << rr;
	painter->setGraphicFont();
	painter->setDefaultBrush();
	painter->setDefaultPen();
	if ( ! centerString.empty())
	{
		DocItemText item( centerString, rr, Align::Center );
		util.formatAndFitTextItem( item );
		//debuglog << "DragChart centerString rect " << item.rect << " w " << item.width << " h " << item.height
			//<< " tf " << item.tf;
		util.drawTextItem( item );
	}

	delete painter;
	delete gc;
}

/*****************************************************
**
**   DragChart   ---   drawChart
**
******************************************************/
void DragChart::drawChart( Painter *painter )
{
	assert( painter );

	painter->drawRectangle( xtol, ytol, lwidth - 2 * xtol, height - 2 * ytol );
	painter->drawLine( xtol, ytol + ystep, lwidth - 2 * xtol, ytol + ystep );
	painter->drawLine( xtol, ytol + 2 * ystep, xstep, ytol + 2 * ystep );
	painter->drawLine( xtol + 3 * xstep, ytol + 2 * ystep, xtol + 4 * xstep, ytol + 2 *ystep );
	painter->drawLine( xtol, ytol + 3 * ystep, lwidth - 2 * xtol, ytol + 3 * ystep );

	painter->drawLine( xtol + xstep, ytol, xtol + xstep, height - 2 * ytol );
	painter->drawLine( xtol + 2 * xstep, ytol, xtol + 2 * xstep, ytol + ystep );
	painter->drawLine( xtol + 2 * xstep, ytol + 3 * ystep, xtol + 2 * xstep, ytol + 4 * ystep );
	painter->drawLine( xtol + 3 * xstep, ytol, xtol + 3 * xstep, height - 2 * ytol );

	wxRect r = getRasiCoord( Sign::None );
	painter->drawRectangle( r.x, r.y, r.width, r.height );
}

/*****************************************************
**
**   DragChart   ---   getRasiCoord
**
******************************************************/
wxRect DragChart::getRasiCoord( const Sign &r )
{
	assert_valid_enum_or_none<Sign>( r );
	if ( r == Sign::None ) return wxRect( lwidth + xtol, ytol, rwidth - 2 * xtol, height - 2 * ytol );
	const int coords[12][2] = {
		{ 1, 0 },
		{ 2, 0 },
		{ 3, 0 },
		{ 3, 1 },
		{ 3, 2 },
		{ 3, 3 },
		{ 2, 3 }, // Libra
		{ 1, 3 },
		{ 0, 3 },
		{ 0, 2 },
		{ 0, 1 },
		{ 0, 0 }
	};
	return wxRect( xtol + coords[(int)r][0] * xstep, ytol + coords[(int)r][1] * ystep, xstep, ystep );
}

/*****************************************************
**
**   DragChart   ---   echo
**
******************************************************/
void DragChart::echo( wxString message )
{
	if ( msgtimer->IsRunning()) msgtimer->Stop();
	centerString = message;

	if ( ! message.IsEmpty()) msgtimer->Start( MESSAGE_TIMEOUT, true );
	Refresh();
}

/*****************************************************
**
**   DragChart   ---   OnTimer
**
******************************************************/
void DragChart::OnTimer( wxTimerEvent& )
{
	if ( msgtimer->IsRunning()) msgtimer->Stop();
	centerString.clear();
	Refresh();
}

/*****************************************************
**
**   DragChart   ---   OnMouseLeftDown
**
******************************************************/
void DragChart::OnMouseLeftDown( wxMouseEvent &event )
{
	DragShape* shape = FindShape( event.GetPosition() );
	if ( shape )
	{
		// We tentatively start dragging, but wait for
		// mouse movement before dragging properly.

		dragMode = TEST_DRAG_START;
		dragStartPos = event.GetPosition();
		draggedShape = shape;
		draggedShape->setHighlight( false );
	}
}

/*****************************************************
**
**   DragChart   ---   OnMouseLeftUp
**
******************************************************/
void DragChart::OnMouseLeftUp( wxMouseEvent &event )
{
	if ( dragMode != TEST_DRAG_NONE )
	{
		// Finish dragging
		dragMode = TEST_DRAG_NONE;
		if ( ! draggedShape || ! dragImage) return;

		draggedShape->setRasipos( Sign::None );
		draggedShape->setHighlight( false );
		wxPoint droppoint = draggedShape->getMidpoint()+ event.GetPosition() - dragStartPos;

		for( Sign r : Enum<Sign>() )
		{
			if ( getRasiCoord( r ).Contains( droppoint ))
			{
				draggedShape->setRasipos( r );
				//draggedShape->setHighlight( true );
				break;
			}
		}

		dragImage->Hide();
		dragImage->EndDrag();
		delete dragImage;
		dragImage = 0;

		draggedShape->SetShow( true );
		draggedShape = 0;
		Refresh( true );
	}
}

/*****************************************************
**
**   DragChart   ---   OnMouseDrag
**
******************************************************/
void DragChart::OnMouseDrag( wxMouseEvent &event )
{
	if ( dragMode != TEST_DRAG_NONE )
	{
		if ( dragMode == TEST_DRAG_START )
		{
			// We will start dragging if we've moved beyond a couple of pixels
			const int tolerance = 2;
			const int dx = abs(event.GetPosition().x - dragStartPos.x);
			const int dy = abs(event.GetPosition().y - dragStartPos.y);
			if ( dx <= tolerance && dy <= tolerance ) return;

			// Start the drag.
			dragMode = TEST_DRAG_DRAGGING;

			if ( dragImage ) delete dragImage;

			// Erase the dragged shape from the canvas
			draggedShape->SetShow( false );

			// redraw immediately
			Refresh( true );
			Update();

			dragImage = new wxDragImage( draggedShape->GetBitmap(), wxCursor( wxCURSOR_HAND ));

			// The offset between the top-left of the shape image and the current shape position
			wxPoint beginDragHotSpot = dragStartPos - draggedShape->GetPosition();

			if ( ! dragImage->BeginDrag( beginDragHotSpot, this, false ))
			{
				delete dragImage;
				dragImage = 0;
				dragMode = TEST_DRAG_NONE;

			}
			else
			{
				dragImage->Move( event.GetPosition() );
				dragImage->Show();
			}
		}
		else if ( dragMode == TEST_DRAG_DRAGGING )
		{
			wxPoint shapepoint = draggedShape->GetPosition()+ event.GetPosition() - dragStartPos;
			for( Sign r : Enum<Sign>() )
			{
				if ( getRasiCoord( r ).Contains( shapepoint ))
				{
					draggedShape->setHighlight( true );
					break;
				}
			}
			dragImage->Move(event.GetPosition());
		}
	}
}

/*****************************************************
**
**   DragChart   ---   DrawShapes
**
******************************************************/
void DragChart::DrawShapes(wxDC& dc)
{
	for( DragShape *shape : displayList )
	{
		if ( shape->IsShown() && draggedShape != shape )
		{
			shape->Draw( dc );
		}
	}
}

/*****************************************************
**
**   DragChart   ---   EraseShape
**
******************************************************/
void DragChart::EraseShape(DragShape* shape, wxDC& dc)
{
	wxSize sz = GetClientSize();
	wxRect rect( 0, 0, sz.x, sz.y );

	wxRect rect2(shape->GetRect());
	dc.SetClippingRegion( rect2.x, rect2.y, rect2.width, rect2.height );
	dc.DestroyClippingRegion();
}

/*****************************************************
**
**   DragChart   ---   ClearShapes
**
******************************************************/
void DragChart::ClearShapes()
{
	for( DragShape *shape : displayList )
	{
		delete shape;
	}
	displayList.clear();
}

/*****************************************************
**
**   DragChart   ---   FindShape
**
******************************************************/
DragShape* DragChart::FindShape(const wxPoint& pt) const
{
	for( DragShape *shape : displayList )
	{
		if ( shape->HitTest( pt )) return shape;
	}
	return nullptr;
}

/*****************************************************
**
**   DragShape   ---   Constructor
**
******************************************************/
DragShape::DragShape(const PlanetId &index )
{
	objectid = index;
	createImage();
	pos.x = 0;
	pos.y = 0;
	show = true;
	highlight = false;
	rasipos = Sign::None;
}

/*****************************************************
**
**   DragShape   ---   Destructor
**
******************************************************/
DragShape::~DragShape()
{
}

/*****************************************************
**
**   DragShape   ---   createImage
**
******************************************************/
bool DragShape::createImage()
{
	SymbolProvider sp;
	bitmap.Create( WDRAGIMAGE, HDRAGIMAGE );
	wxMemoryDC dc;
	dc.SelectObject( bitmap );

	wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	assert( gc );
	Painter *painter = PainterFactory().createGcPainter( gc );
	assert( painter );
	DocHelper util( painter );

	wxColour magic(141, 141, 141 );
	painter->setBrush( magic );
	painter->setTextColor( *wxBLACK );

	painter->setTransparentPen();

	dc.DrawRectangle(0,0, WDRAGIMAGE, HDRAGIMAGE );

	painter->setSymbolFont();
	util.drawTextFormatted(
		sp.getPlanetCode( objectid, /*uranus, pluto: unused*/ 0, 0 ),
		MRect( 0, 0, WDRAGIMAGE, HDRAGIMAGE ),
		Align::Center );

	dc.SelectObject( wxNullBitmap );

	wxMask *mask = new wxMask( bitmap, wxColour( magic ));
	bitmap.SetMask( mask );

	delete painter;
	delete gc;
	return bitmap.IsOk();
}

/*****************************************************
**
**   DragShape   ---   HitTest
**
******************************************************/
bool DragShape::HitTest(const wxPoint& pt) const
{
	wxRect rect( GetRect() );
	return rect.Contains( pt.x, pt.y );
}

/*****************************************************
**
**   DragShape   ---   Draw
**
******************************************************/
bool DragShape::Draw( wxDC& dc )
{
	if ( bitmap.Ok() )
	{
		wxMemoryDC memDC;
		memDC.SelectObject( bitmap );
		dc.Blit( pos.x, pos.y, bitmap.GetWidth(), bitmap.GetHeight(), &memDC, 0, 0, wxCOPY, true );

		// TODO geht nicht
		if ( highlight )
		{
			dc.SetPen( *wxWHITE_PEN );
			dc.SetBrush( *wxTRANSPARENT_BRUSH );
			dc.DrawRectangle( pos.x,  pos.y,  bitmap.GetWidth(),  bitmap.GetHeight() );
		}
		return true;
	}
	else return false;
}

/*****************************************************
**
**   DragShape   ---   getMidpoint
**
******************************************************/
wxPoint DragShape::getMidpoint() const
{
	return wxPoint( pos.x + WDRAGIMAGE / 2, pos.y + HDRAGIMAGE / 2 );
}
