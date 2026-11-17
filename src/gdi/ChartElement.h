/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/ChartElement.h
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

#ifndef CHART_ELEMENT_H_
#define CHART_ELEMENT_H_

#include "gdibase.h"
#include "geometry.h"
#include "constants.h"
#include "WidgetResponse.h"

class HybridChart;
class MTerm;
class Painter;
struct ChartGeometry;

enum class FirstChartAngleMode { Extern, Integrated };

/*************************************************//**
*
*   Base class for all chart elements.
*
******************************************************/
class ChartElementBase
{
	friend class HybridChart;

public:

	ChartElementBase( HybridChart* );

	virtual ~ChartElementBase() {}

	/**
	* Paint delegate method for a chart element.
	*/
	virtual void paint( Painter*, const MRect &refreshRect ) = 0;

	virtual double getRadius() const = 0;

	virtual double getOuterRadius( const int& ) = 0;

	virtual CgElementType getElementType() const = 0;

	/**
	*  Returns the pen either of the chart geometry (if defined) or the chart's one.
	*/
	wxPen getPen() const;

	/**
	*  Returns the brush either of the chart geometry (if defined) or the chart's one.
	*
	* The inheritFromSkin determines if the brush should be transparent if the geom has no brush.
	*/
	MBrush getBrush( const bool inheritFromSkin = false ) const;

	/**
	*  Returns the text color either of the chart geometry (if defined) or the chart's one.
	*/
	wxColour getTextColor() const;

	/**
	*  Set the pen, brush and text color according to the geom and skin.
	*/
	void setInheritedStyle( Painter*, const bool inheritBrushFromSkin = false );

	/**
	*  Can be overridden in order to react on mouse left click.
	*/
	virtual WidgetResponse handleMouseLeftEvent( const MComplex& ) { return false; }

	/**
	*  Can be overridden in order to react on mouse move events.
	*/
	virtual WidgetResponse onResponsiveChartEvent( const MComplex&, const int /*keymod*/ = 0 ) { return false; }

	int getDrawStackPosition() const { return drawStackPosition; }

	HybridChart *getChart() const { return chart; }

protected:

	/**
	*	Calculate all coordinate/geom based parameters of a chart element.
	*
	* The method should be used in order to keep the paint method short.
	* It is called
	* - on initial setup of the element
	* - after chart size change
	* - on geom change 
	* 
	* It is not called on skin change.
	*/
	virtual void onCoordinatesChanged() {}

	/*
	* hint for order of drawing addressing the problem of wrong order an override
	*/
	int drawStackPosition;

	virtual wxPen &getCgPen() const = 0;

	virtual MBrush &getCgBrush() const = 0;

	virtual wxColour &getCgTextColor() const = 0;

	MComplex discPointToXy( const double &angle, const double &radius, const ChartShape& );

	MComplex discPointToXy( const double &angle, CgDisc* );

	void paintDisc( Painter*, CgDisc );

	void paintDisc( Painter*, CgDisc* );

	void paintMarkerLine( Painter*, const double &longitude, CgDisc );

	void drawAnnulusSegment( Painter*, CgAnnulus*, const double &alfa0, const double &alfa1 );

	void paintAnnulusConnector( Painter*, const double &angle, const double &r1, const double &r2, const ChartShape& );

	void paintAnnulusConnector( Painter*, const double &angle, const CgDisc &disc, const double &r );

	HybridChart *chart;

private:

};

typedef std::shared_ptr<ChartElementBase> ChartElementBase_ptr;

/*************************************************//**
*
*  
*
******************************************************/
class ChartElementFactory
{
public:

	ChartElementFactory( HybridChart *h ) : chart( h ) {}

	std::list<ChartElementBase_ptr> createElements( const ChartGeometry& ) const;

private:

	ChartElementBase_ptr createChartElement( CgElement* ) const;

	ChartElementBase *createDisc( CgElement* ) const;
	ChartElementBase *createAnnulus( CgElement* ) const;
	ChartElementBase *createAspects( CgElement* ) const;
	ChartElementBase *createImage( CgElement* ) const;
	ChartElementBase *createError( CgElement* ) const;

	ChartElementBase *createHouses( CgElement* ) const;
	ChartElementBase *createSigns( CgElement* ) const;
	ChartElementBase *createNakshatras( CgElement* ) const;
	ChartElementBase *createDasaLords( CgElement* ) const;

	ChartElementBase *createWesternPlanets( CgElement* ) const;
	ChartElementBase *createUranianPlanets( CgElement* ) const;
	ChartElementBase *createAngles( CgElement* ) const;

	ChartElementBase *createUranianDial( CgElement* ) const;
	ChartElementBase *createUranianAspects( CgElement* ) const;

	ChartElementBase *createEastIndianChartGrid( CgElement* ) const;
	ChartElementBase *createSouthIndianChartGrid( CgElement* ) const;
	ChartElementBase *createDiamondChartGrid( CgElement* ) const;

	ChartElementBase *createSbcGrid( CgElement* ) const;

	HybridChart *chart;
};


#endif

