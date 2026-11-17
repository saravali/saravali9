/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/BasicWidget.h
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


#ifndef _BASICWIDGET_H_
#define _BASICWIDGET_H_

#include "guibase.h"
#include "ToolPanel.h"
#include "WidgetResponse.h"

#include <wx/scrolwin.h>

class Painter;
class ToolbarLabelCtrl;
struct WidgetNode;
class wxBoxSizer;
enum class CommandId;

enum class WidgetAction
{
	None = -1,
	Scroll,
	ChangeZoom,
	ChangeChartZoom,
	ChangeSkin,
	ChangeGeometry,
	PropagateEvent,
};

enum class WidgetActionBundle
{
	None = -1,
	Default,
	Chart,
	Bardiagram,
	Text
};

/*************************************************//**
*
*  Basic abstract class for graphical display based upon wxScrolledWindow
*
******************************************************/
class BasicWidget : public wxScrolledWindow
{
	friend class TextWidgetEventHandler;

	friend class ChildWindow;

public:
	BasicWidget( wxWindow *parent, ContextProps_ptr, const WidgetCategory&, const ToolbarWidgetId = ToolbarWidgetId::None );

	virtual void OnDataChanged();

	virtual void postCreate();

	ContextProps_ptr getProps() const;

	virtual WidgetCategory getWidgetCategory() const { return category; }

	void setWidgetCategory( const WidgetCategory &cat ) { category = cat; }

	virtual wxString getTitle( const bool shortname = false );

	virtual Viewpoint getViewpoint() const;

	void setWidgetActions( const WidgetActionBundle& );

	void setChartActions() { setWidgetActions( WidgetActionBundle::Chart ); }

	void setWheelActions( const std::map<int, WidgetAction> &m ) { mouseWheelActions = m; }

	void OnEraseBackground( wxEraseEvent& );

	void OnPaint( wxPaintEvent& );

	MComplex getMousePosition() const { return mousePosition; }

	MComplex getUnscrolledMousePosition() const { return unscrolledMousePosition; }

	virtual bool queryClose() { return true; }

protected:

	/**
	* Not all subclasses need mouse event handling (textwidget with wxHtmlWindow). So it can be called manually
	*/
	void initMouseEventhandlers();

	virtual void doPaint( Painter*, const MRect& /*refreshRect*/ ) {}

	/**
	* enable/disable controls etc.
	*/
	virtual void guiHasChanged() {}

	virtual void OnToolbarCommand();

	void sendUpdateTitleEvent();

	void OnToolbarCommand( wxCommandEvent& ) { OnToolbarCommand(); }

	template<typename T> T* getToolbarControl( const ToolbarCmd &id )
  {
    if ( toolbar )
    {
      return dynamic_cast<T*>( toolbar->FindControl( (int)id ));
    }
    return nullptr;
  }

	void OnKeyDown( wxKeyEvent& );
	void OnKeyUp( wxKeyEvent& );

	void doAction( const WidgetAction&, const int &offset );
	void doScroll( const int &offset );
	void changeZoom();
	void changeGeometry( const int &offset );
	void changeSkin( const int &offset );
	void propagateAction( const int &offset );

	virtual WidgetResponse handleMouseLeftClick( const MComplex&, const int& /*keymod*/ ) { return false; }

	virtual WidgetResponse handleLeftDoubleClick( const MComplex&, const int& /*keymod*/ ) { return false; }

	virtual WidgetResponse handleResponsiveChartEvent( const MComplex&, const int& /*keymod*/ ) { return false; }

	wxSize getImageExportSize();

	wxBitmap createImageBitmap( const wxSize& );

	void doImageToFileExport( const CommandId& );

	void doClipboardExport();

	void doPdfExport();

	virtual void OnCommand( wxCommandEvent& );

	bool dragMode, exportMode;

	std::map<int, WidgetAction> mouseWheelActions;

	wxToolBar *toolbar = nullptr;

	wxBoxSizer *mainsizer = nullptr;

	ContextProps_ptr props;

	WidgetCategory category;

	const wxString configkey;

private:

	// Mouse
	void OnMouseWheelEvent( wxMouseEvent& );
	void OnMouseLeave( wxMouseEvent& );
	void OnMouseEnter( wxMouseEvent& );
	void OnMouseLeftDown( wxMouseEvent& );
	void OnMouseLeftUp( wxMouseEvent& );
	void OnMouseDoubleClick( wxMouseEvent& );
	void OnMouseMotion( wxMouseEvent& );

	void dispatchResponsiveEvent();

	void OnContextMenuEvent( wxMouseEvent& );

	MComplex mousePosition;
	MComplex unscrolledMousePosition;
	bool mouseInside;

	// Tooltip and toolbar echo stuff
	void doEcho( const wxString &message );

	ToolbarLabelCtrl *toolbarLabelCtrl = nullptr;

};

#endif

