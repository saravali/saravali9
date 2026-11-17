/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/DragChart.h
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

#ifndef DRAGCHART_H
#define DRAGCHART_H

#include <wx/bitmap.h>
#include <wx/scrolwin.h>

#include "gdibase.h"
#include "constants.h"
#include "Planet.h"

#include <list>
#include <map>

class wxDragImage;
class wxImage;
class wxTimer;
class wxTimerEvent;

class DragShape;
class Painter;

/*************************************************//**
*
*  Drag an drop chart
*
******************************************************/
class DragChart : public wxScrolledWindow
{
public:

	DragChart( wxWindow* );

	~DragChart();

	void clearChart();

	void addObject( const PlanetId& );

	std::map<PlanetId, Sign> getPlanetData() const;

	void echo( wxString );

private:

	void OnPaint( wxPaintEvent& );
	void OnTimer( wxTimerEvent& );

	void OnMouseLeftDown( wxMouseEvent& );
	void OnMouseLeftUp( wxMouseEvent& );
	void OnMouseDrag( wxMouseEvent& );

	void DrawShapes( wxDC& );
	void EraseShape( DragShape*, wxDC& );
	void ClearShapes();
	void drawChart( Painter* );
	wxRect getRasiCoord( const Sign& );

	DragShape* FindShape( const wxPoint& ) const;

	int dragMode, lwidth, rwidth, height, xtol, ytol, xstep, ystep;

	std::list<DragShape*> displayList;
	DragShape *draggedShape;
	wxPoint dragStartPos;
	wxDragImage * dragImage;
	wxString centerString;
	wxTimer *msgtimer;
};

/*************************************************//**
*
*  Drag and drop object
*
******************************************************/
class DragShape: public wxObject
{
public:
	DragShape(const PlanetId& );
	~DragShape();

	bool HitTest( const wxPoint& pt ) const;
	bool Draw( wxDC& );

	wxPoint GetPosition() const { return pos; }
	void SetPosition( const wxPoint& p ) { pos = p; }

	wxPoint getMidpoint() const;

	wxRect GetRect() const { return wxRect( pos.x, pos.y, bitmap.GetWidth(), bitmap.GetHeight() ); }

	wxBitmap& GetBitmap() const { return (wxBitmap&)bitmap; }

	bool IsShown() const { return show; }
	void SetShow(bool s ) { show = s; }

	void setRasipos( const Sign &r ) { rasipos = r; }
	Sign getRasipos() const { return rasipos; }

	PlanetId getPlanetId() const { return objectid; }

	void setHighlight( const bool &b ) { highlight = b; }

protected:
	wxPoint pos;
	wxBitmap bitmap;
	bool show;
	bool highlight;
	Sign rasipos;
	PlanetId objectid;

	bool createImage();

};


#endif // DRAGCHART_H
