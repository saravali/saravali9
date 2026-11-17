/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/GdiWidgetGrid.h
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

#ifndef _GDIWIDGETGRID_H_
#define _GDIWIDGETGRID_H_

#include <list>

#include "gdibase.h"
#include "GdiWidget.h"

struct WidgetSkin;
class Painter;

enum class WidgetGridFormat
{
	None = -1,
	OptimizedFloat,
	FloatFixedColWidth,
	FloatVariableColWidth, // not implemented
	FixedCols, // unused
	FixedColsScaled
};

/*************************************************//**
*
*  This class is a container for a list of GdiWidget instances.
*
* The following layout strategies are supported:
	- OptimizedFloat: widgets with given x/y ratio are arranged to use maximum space in the grid
	- FloatFixedColWidth: widgets with fixed size are arranged in rows, new line when space is exhausted, width is equal.
	- FloatVariableColWidth same as before but each widget has its own width.
	- FixedCols: fixed number of colums, then next row.
	- FixedColsScaled: fixed number of columns, widgets are scaled.
*
*
* Usages
* - OptimizedFloat: AshtakavargaWidget, ShadbalaWidget, VimsopakabalaWidget
* - FloatFixedColWidth: PpWidget
* - FloatVariableColWidth: unused
	- FixedCols: unused
* - FixedColsScaled: VargaGridWidget
*
******************************************************/
class GdiWidgetGrid : public GdiWidget
{
public:

	GdiWidgetGrid( ContextProps_ptr props, const WidgetCategory &category, const WidgetGridFormat &format, const int colcount = 1 )
		: GdiWidget( props, category ), gridFormat( format ), columnCount( colcount ) {}

	virtual ~GdiWidgetGrid();

	void clearWidgets();

	void setWidgetRatio( const double &a ) { widgetX2yratio = a; }

	void setXMin( const double &a ) { widgetXMin = a; }

	void setXMax( const double &a ) { widgetXMax = a; }

	/**
	* Calculate the size for single widget supposed that the total width is given
	*/ 
	MComplex calculatePreferredWidgetSize( Painter*, const double &xmax );

	/**
	* Layouts and moves all widgets to the right place. Preferred widget size is calculated internally
	*
	* Return: total size of the widget set - which may be different from overal grid size,
	* e.g. if the grid is too small and minimum sizes must be used.
	*
	*/ 
	virtual MComplex doLayout( Painter* ) override;

	/**
	* Real size of the widget grid.
	*/
	MComplex getWidgetAreaSize();

	virtual void doPaint( Painter*, const MRect &refreshRect ) override;

	virtual void OnDataChanged() override;

	virtual void onCoordinatesChanged() override;

	void addWidget( GdiWidget* );

	std::list<GdiWidget*> widgets;

  MRect getRect() const { return rect; }

	const WidgetSkin &getSkin() const;

	virtual WidgetResponse onResponsiveChartEvent( const MComplex&, const int &keymod ) override;

	virtual WidgetResponse handleMouseLeftEvent( const MComplex& ) override;

protected:

	/**
	* call float layout optimization and postion all widgets
	*/
	MComplex layoutWidgetsOptimizedFloat( Painter* );

	MComplex layoutWidgetsFloatCluster( Painter* );

	/**
	*
	*/ 
	MComplex calculateOptimizedFloatLayout( const double &containerRatio, const uint &totalsize );

	MComplex layoutWidgetsFixedCol( Painter* );

	MComplex layoutWidgetsFixedColsScaled( Painter* );

	GdiWidget *findWidgetForPosition( const MComplex& );

	GdiWidget *lastActiveResponseWidget = nullptr;

	const WidgetGridFormat gridFormat;

	const uint columnCount;

	double widgetX2yratio = 1.0;

	double widgetXMin = 200;

	double widgetXMax = 2000;
};

#endif // _GDIWIDGETGRID_H_

