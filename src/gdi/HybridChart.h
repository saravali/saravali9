/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/HybridChart.h
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

#ifndef HYBRIDCHART_H_
#define HYBRIDCHART_H_

#include "ChartConfig.h"
#include "ChartElement.h"
#include "ChartField.h"
#include "GdiWidget.h"
#include "Term.h"
#include "Viewpoint.h"

class AshtakavargaExpert;

class ChartElementBase;
class Painter;

struct WidgetSkin;
struct ChartConfig;
struct ChartGeometry;

typedef std::pair<MTerm, HoroscopeRole> ActiveTerm;

/*************************************************//**
*
* Central class for graphical charts.
*
* HybridChart can be used for
* - Western and Vedic charts with objects based on longitude
* - Vedic charts based upon vargas (D-9 etc.)
* - Ashtakavarga charts
* - Sarvatobhadra charts.
*
* HybridChart essentially consists of ChartELements representing various
* items like Planets, torus segments or field grids.
* ChartElements can be mixed, so elements of Western and Vedic astrology can appear in the same chart.
*
*
******************************************************/
class HybridChart : public GdiWidget
{
public:

	HybridChart( ContextProps_ptr, const WidgetCategory&, const Varga& );

	HybridChart( ContextProps_ptr, const WidgetCategory&, const WidgetCategory parentCategory = WidgetCategory::None );

	HybridChart( ContextProps_ptr, const WidgetCategory&, UranianSection* );

	HybridChart( ContextProps_ptr, AshtakavargaExpert*, const AvType&, const PlanetId& );

	virtual ~HybridChart();

	virtual MComplex doLayout( Painter* ) override;

	virtual void doPaint( Painter*, const MRect &refreshRect ) override;

	virtual void OnDataChanged() override;

	virtual void setRect( const MRect& ) override;

	virtual WidgetResponse onResponsiveChartEvent( const MComplex&, const int &keymod ) override;

	virtual WidgetResponse handleMouseLeftEvent( const MComplex& ) override;

	std::vector<HoroscopeRole> getChartRoles() const { return chartRoles; }

	HoroscopeRole getFirstRole() const;

	Horoscope_ptr getFirstHoroscope() const;

	HoroscopeRole getSecondRole() const;

	Horoscope_ptr getSecondHoroscope() const;

	Horoscope_ptr getHoroscope( const HoroscopeRole& );

	// Ashtakavarga
	AshtakavargaExpert *getAshtakavargaExpert() const { return avexpert; }

	AvType getAvType() const { return avtype; }

	PlanetId getAvObject() const { return avobject; }

	Varga getVarga() const { return varga; }

	bool isCompositeChart() const { return parentCategory == WidgetCategory::CompositePair; }

	bool isDoubleChart() const;

	/**
	* This feature is used by ChartPreviewPanel: ChartConfig can be overriden by a temporary value.
	*/
	void setChartConfig( const WidgetCategory &cat, ChartConfig *cfg )
	{
		overiddenChartConfig = std::make_pair( cat, cfg );
	}

	ChartConfig *getChartConfig( const WidgetCategory& ) const;

	ChartConfig *getChartConfig() const { return getChartConfig( category ); }

	MComplex getCenter() const { return center; }

	MComplex getXyMax() const { return rxymax; }

	double getRMax() const { return rmax; }

	double getAries() const { return aries; }

	/**
	* Calculates the effective lontitude of a planet, i.e. the longitude where it has to be painted.
	* Zero means the right middle part of the chart (like in normal coordinate system).
	*
	* Division is integrated (if any) and the chart origins of western and uranian chart.
	*/
	double getEffectiveLongitude( const double& );

	double getAyanamsa() const;

	const ChartGeometry &getGeometry();

	virtual double getRelativeFontSize() const override;

	/**
	*  Leading active term is the term with mouse focus (WesternChart)
	*/
	const ActiveTerm& getLeadingActiveTerm() const { return leadingActiveTerm; }

	/**
	*  Is called on mouse focus. This is step one, step two is done by CeAspects if
	*  any aspected factors are found (Western Chart).
	*/
	void setLeadingActiveTerm( const ActiveTerm& );

	/**
	*  Either lading active term is found (mouse over) or dependend term by aspects (Western Chart)
	*/
	bool hasActiveTerm() const { return activeTerms.size() > 0; }

	/**
	*  Either lading active term is found (mouse over) or dependend term by aspects (Western Chart)
	*/
	const std::list<ActiveTerm> &getActiveTerms() const { return activeTerms; }

	/**
	*  WesternChart: the list of active terms is calulated by CeAspects and contains the leading active terms and aspected terms.
	*/
	void setActiveTerms( const std::list<ActiveTerm> &l ) { activeTerms = l; }

	/**
	*  WesternChart: returns if a term is either leading active term of dependend term (i.e. aspected by the leading term).
	*/
	bool isActiveTerm( const ActiveTerm& ) const;

	bool isAshtakavargaChart() const { return avexpert != nullptr; }

protected:

	void init();

	virtual void onCoordinatesChanged() override;

	void calculateChartRoles();

	uint getChartRoleCount() const { return chartRoles.size(); }

	void clearWidgets();

	void setupWidgets();

	void paintWidgets( Painter*, const MRect& refreshRect );

	double calculateAries();

	ActiveTerm leadingActiveTerm;

	std::list<ActiveTerm> activeTerms;

	// Ashtakavarga
	AshtakavargaExpert *avexpert = nullptr;

	AvType avtype = AvType::None;

	PlanetId avobject = PlanetId::None;

	Varga varga = Varga::None;

	const WidgetCategory parentCategory = WidgetCategory::None;

	std::pair<WidgetCategory, ChartConfig*> overiddenChartConfig = std::make_pair( WidgetCategory::None, nullptr );

	std::vector<HoroscopeRole> chartRoles;

	double rmax = 0.0;

	MComplex rxymax = 0.0;

	MComplex center = 0.0;

	double aries = 0.0;

	std::list<ChartElementBase_ptr> elements;

private:

	// avoid calls to ChartGeometryLoader::findFirstGeometry
	std::map<WidgetCategory,uint> geomCache;

};


#endif

