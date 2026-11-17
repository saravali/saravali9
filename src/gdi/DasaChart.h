/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/DasaChart.h
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

#ifndef _DASACHART_H_
#define _DASACHART_H_

#include "GdiWidget.h"
#include "GraphicalDasa.h"
#include "MDocument.h"

struct BarDiagramConfig;

/*************************************************//**
*
* Helper class for drawing graphical dasas
*
******************************************************/
class DasaBarPaintHelper
{
public:
	DasaBarPaintHelper( const BarDiagramConfig &barconfig )
		: barconfig( barconfig )
	{
	}

	void paintDasaBar( Painter*, GraphicalDasa_ptr, const bool &zoomItem );

protected:
	const BarDiagramConfig &barconfig;

};

/*************************************************//**
*
* The widget for graphical Dasa.
*
******************************************************/
class DasaChart : public GdiWidget
{
public:
	DasaChart( ContextProps_ptr );

	~DasaChart();

	virtual MComplex doLayout( Painter* ) override { return calculateViewport(); }

	MComplex calculateViewport();

	virtual void OnDataChanged() override;

	virtual WidgetResponse onResponsiveChartEvent( const MComplex&, const int &keymod ) override;

	void setDasas( const std::list<DasaId>& );

	std::list<DasaId> getDasas() const { return dasas; }

	void setAntardasaLevel( const int& );

	double jd2x( const double& );

	double x2jd( const double& );

	MRect getSheetRect() const { return sheetRect; }

private:

	/**
	* Calculate first level of dasas with start date, end date, level, lord (no rectangle calculation here)
	* and delegate to method calculateNextRawLevel for deeper dasa levels.
	*
	* Return value: number of dasas (recursive)
	*/
	int calculateRawLevels();

	/**
	* Calculate n-th level of dasas with start date, end date, level, lord (no rectangle calculation)
	* and delegate to self for deeper dasa levels.
	*
	* Return value: number of dasas (recursive)
	*/
	int calculateNextRawLevel( GraphicalDasa_ptr, DasaExpert_ptr );

	/**
	* Calculate item rectangles recursively.
	* Raw levels must be calculated before.
	*
	* Return value: number of dasas (recursive)
	*/
	int calcItemRectangles( GraphicalDasa_ptr );

	virtual void doPaint( Painter*, const MRect &refreshRect ) override;

	void paintDasaItem( Painter*,
		GraphicalDasa_ptr,
		const MRect&,
		const bool doIterate = true,
		const bool skipCurrentItem = true );

	void paintRuler( Painter*, const MRect& );

	GraphicalDasa_ptr findItemForPoint( const MComplex& );

	GraphicalDasa_ptr findItemForPointInDasa( GraphicalDasa_ptr, const MComplex& );

	MRect zoomRect( const MRect& );

	int calcYTopForDasa( GraphicalDasa_ptr );

	//  depth of antardasa calculation
	int adlevel = 1;

	// total view size
	int xviewport = 0;

	int yviewport = 0;

	// coordinates of the sheet
	MRect sheetRect;

	// x position of birth
	const double birthx = 0.0;

	// x position of last date
	double endjdx = 0.0;

	// last date of sheet
	double endjd = 0.0;
	
	// zoom value from props used by last init viewport
	double viewportZoom = 0.0;

	// hold the IDs of the correspoding Dasa experts
	std::list<DasaId> dasas;

	// active item under mouse cursor
	GraphicalDasa_ptr currentItem;

	std::list<GraphicalDasa_ptr> rootItems;

	std::map<DasaId, DasaExpert_ptr> experts;

	double jdtotalduration = 36500.0;

	inline static constexpr int hborder = 10;

	// constant height of Dasa bar
	inline static constexpr int ybar = 30;;

	inline static constexpr int DASA_TITLE_STEP_WIDTH = 1500;

	inline static constexpr int DASA_BAR_MIN_ITEM_WIDTH = 5;
};

#endif // _DASACHART_H_

