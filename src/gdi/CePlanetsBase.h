/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/CePlanetsBase.h
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

#ifndef _CEPLANETSBASE_H_
#define _CEPLANETSBASE_H_

#include "ChartElementImpl.h"
#include "MDocument.h"
#include "Term.h"

#include <list>
#include <map>

/*************************************************//**
*
*  A graphical wrapper for a single factor in graphical chart, can be planet, angle or term.
*
******************************************************/
class GraphicalChartItem : public DocItemText
{
public:
	GraphicalChartItem(
		ChartElement<CgPlanetScale> *elem,
		const HoroscopeRole&,
		const MComplex &point,
		const MTerm&, const double &longitude );

	void writeText();

	void format( Painter* );

	void paint( Painter* );

	const MTerm &getTerm() { return term; }

	// true longitude in the chart
	double getLongitude() const { return longitude; }

protected:

	ChartElement<CgPlanetScale> *elem;

	const HoroscopeRole role;

	const MComplex point;

	MTerm term;

	const double longitude;

	bool isSymbol = true;

	double baseFontSize = 100.0;

};

// DegreemarkerType
enum class Dmt { t1, t5, t10 };

/*************************************************//**
*
*  Base class for chart elements with graphical factors (planets, angles and general terms).
*
*  It contains essentially a list of GraphicalChartItem representing the terms.
*
*  Required attributes
*  - radius
*  - geometryType
*  - markerLength
*  - rLabels
*  - labelSize
******************************************************/
class CePlanetsBase : public ChartElement<CgPlanetScale>
{
public:

	CePlanetsBase( HybridChart*, CgPlanetScale* );

	~CePlanetsBase();

	virtual void paint( Painter*, const MRect &refreshRect ) override;

protected:

	void clearItems();

	void addItem( ChartElement<CgPlanetScale>*, const HoroscopeRole&, const MComplex&, const MTerm&, const double &longitude );

	double getModuloLongitude( const double&, const int division = 1 );

	bool drawAnglesRequired( const HoroscopeRole& ) const;

	virtual WidgetResponse onResponsiveChartEvent( const MComplex &p, const int keymod ) override;

	MComplex getCoordinatesForItem( const HoroscopeRole&, const double& );

	void paintDegreeScales( Painter*, const MRect& /*refreshRect*/, const int division = 1 );

	void paintSingleScale( Painter*, const double &radius, const int &division, const ChartShape &shape,
		CgDegreeMarker &markerStyle, const double &stepWidth, const std::list<Dmt> &rule, const double &aries );

	std::map<HoroscopeRole, std::list<GraphicalChartItem*>> citems;

	// roles copied from HybridChart, may be add handshifted
	std::vector<HoroscopeRole> chartRoles;

	bool doPaintMarker = true;

private:

	int getRoleIndex( const HoroscopeRole& ) const;

	wxString writeTooltip() const;

};

#endif // _CEPLANETSBASE_H_

