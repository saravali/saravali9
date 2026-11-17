/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/GdiWidget.h
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

#ifndef _GDIWIDGET_H_
#define _GDIWIDGET_H_

#include "gdibase.h"
#include "logger.h"
#include "constants.h"
#include "MDocument.h"
#include "WidgetResponse.h"

struct WidgetSkin;
class Painter;
struct UranianSection;

/*************************************************//**
*
*  This is the base class for all widgets from the gdi layer.
*
*	It contains several abstract methods that must be implemented by subclasses.
*
******************************************************/
class GdiWidget
{
	friend class GdiWidgetGrid;

public:

	GdiWidget( ContextProps_ptr props, const WidgetCategory category, UranianSection *ucfg = nullptr )
		: props( props ),
		category( category ),
		usection( ucfg )
	{
	}

	virtual ~GdiWidget() {}

	/**
	* Layout calculation of widgets.
	*
	* Responsibilities of this method
	*--------------------------------
	* - Internal setup of elements according to current rectangle
	* - return best size to caller. This size may be larger than the rectangle. The parent window may enlarge the viewport.
	*
	* Prerequissites
	* --------------
	* - Painter must be valid (but is not necessarily used within the widget).
	* - Rectangle of the widget must be set before with setRect().
	*
	*/
	virtual MComplex doLayout( Painter* );

	/**
	* Shortcut for setRect plus doLayout
	*/
	MComplex doLayout( Painter*, const MRect& );

	/**
	* This method paints the widget using the given painter. It must be implemented by specialiations.
	*/
	virtual void doPaint( Painter*, const MRect &refreshRect ) = 0;

	/*
	* This method is optional and can be used to update the internal data model of the widget.
	*/
	virtual void OnDataChanged() {}

	virtual void setRect( const MRect &r ) { rect = r; }

  void setRect( const double &x, const double &y, const double &w, const double &h ) { setRect( MRect( x, y, w, h )); }

  MRect getRect() const { return rect; }

	UranianSection *getUranianSection() const { return usection; }

	Viewpoint getViewpoint() const;

	/**
	* Handles mouse move and change of shift/ctrl/alt key
	* Response contains refresh flag/rect and optional a message string.
	*/ 
	virtual WidgetResponse onResponsiveChartEvent( const MComplex&, const int& /*keymod*/ ) { return false; }

	/**
	* Handles mouse left click.
	* Response contains refresh flag/rect and optional a message string.
	*/ 
	virtual WidgetResponse handleMouseLeftEvent( const MComplex& ) { return false; }

	virtual WidgetResponse handleLeftDoubleClickEvent( const MComplex& ) { return false; }

	ContextProps_ptr getProps() const { return props; }

	WidgetCategory getWidgetCategory() const { return category; }

	const WidgetSkin &getSkin() const;

	/**
	* Get the pen configured in the skin (if available) or the default one
	*/
	wxPen getSkinnedPen() const;

	/**
	* Get the brush configured in the skin (if available) or the default one
	*/
	MBrush getSkinnedBrush() const;

	/**
	* Get the marked field brush configured in the skin (if available) or the default one
	*/
	MBrush getSkinnedMarkedFieldBrush() const;

	/**
	* Get the text color configured in the skin (if available) or the default one
	*/
	wxColour getSkinnedTextColor() const;

	virtual double getRelativeFontSize() const;

	void setGraphicFont( Painter*, const int labelSizePercent = 100 );

	void setSymbolFont( Painter*, const int labelSizePercent = 100 );

protected:

	double calculateFontSize( const int &labelSizePercent );

	virtual void onCoordinatesChanged() {}

	ContextProps_ptr props;

	const WidgetCategory category;

	UranianSection *usection;

  MRect rect;
};

/*************************************************//**
*
*  Shows an empty widget and optional message, e.g. on error or missing data.
*
******************************************************/
class ErrorDiagram : public GdiWidget
{
public:
	ErrorDiagram( ContextProps_ptr, const wxString message = "", const LogLevel = LogLevel::Info );

	virtual void OnDataChanged() override {}

	virtual void doPaint( Painter*, const MRect &refreshRect ) override;

protected:
	
	DocItemText text;

	const LogLevel loglevel;
};

#endif // _GDIWIDGET_H_


