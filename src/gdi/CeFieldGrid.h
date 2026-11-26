/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/CeFieldGrid.h
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

#ifndef CEFIELDGRID_H_
#define CEFIELDGRID_H_

#include "ChartElementImpl.h"
#include "ChartField.h"
#include "FieldWriter.h"
#include "MDocument.h"
#include "VedicAspect.h"

#include <set>

class FieldWriter;
class HybridChart;
class Painter;

/*************************************************//**
*
*  This is the (abstract) grid for Vedic charts and SBC.
*
******************************************************/
class CeFieldGrid : public ChartElement<CgVedicChartGrid>
{
	friend class VargaFieldWriter;
	friend class SbcFieldWriter;
	friend class AshtakavargaFieldWriter;

public:

	CeFieldGrid( HybridChart*, CgVedicChartGrid*, const int field_count );

	~CeFieldGrid();

	virtual void paint( Painter*, const MRect& refreshRect ) override;

	virtual WidgetResponse onResponsiveChartEvent( const MComplex &p, const int keymod = 0 ) override;

	virtual void onCoordinatesChanged() override;

protected:

	/**
	*  Initial paint activities.
	*
	* Varga chart: format center string (coordinates will be reuqired later for North Indian chart).
	*
	* sbc: paint backgound of marked fields
	*/
	virtual void startPaint( Painter* );

	virtual void setupFieldRectangles() = 0;

	virtual void paintGrid( Painter*, const MRect& ) = 0;

	virtual bool doPaintFieldBackground( const uint& );

	virtual void updateAspects( Horoscope_ptr aspecting_h, Horoscope_ptr aspected_h ) = 0;

	/**
	*  Writes the contents of the text fields.
	*
	* Overriden by SBC because of Nakshatra titles and qualtities.
	*/
	virtual void writePlanetFields()
	{
		assert( proxy );
		proxy->writePlanetFields();
	}

	void calculateFieldStatus( const MComplex&, const int &keymod );

	void addKeybordModifierOptions( const int &id );

	void setAspectPainterStyle( Painter*, const std::pair<bool, VedicAspectStrength>& );
	
	bool areAspectsFieldsSeparated() const;

	std::pair<FieldPart, FieldPart> getActiveFieldParts() const;

	std::pair<MComplex, MComplex> calculateFieldCenters( const int &field1, const int &field2, const bool &isSeparated ) const;

	void paintAspectsForTarget( Painter*, const int &field );

	void paintAspectsForSource( Painter*, const int &geom_field );

	// geom_field = field in chart
	void paintAspectBundle( Painter*, const int &geom_field1, const int &geom_field2,
		const std::list<std::pair<bool, VedicAspectStrength>>& );

	int geomToAstroField( const int &geom_field );

	int astroToGeomField( const int &astro_field );

	/**
	* Determines whether a specific VedicAscpect is usable in the current context (VargaChart, Sbc).
	*
	* Default implementation works for VargaChart, Sbc must override.
	*/
	virtual bool isAspectValid( const  VedicAspectId& ) const;

	wxString debugFieldStatus( const std::set<FieldStatus>& );

	std::set<FieldStatus> fieldStatus;

	uint activeField = UINT_FOR_NOT_FOUND;

	int positionOffset = 0;

	// standard point that will likely be used by most charts.
	MComplex p_north, p_south, p_east, p_west;
	MComplex p_northeast, p_northwest, p_southeast, p_southwest;

	// holds the x/y coordianates for double chart field split
	// from outer angle of the chart to the corner of the transit rect
	MComplex pFieldSplit;

	// rectangle describing the border between two charts
	MRect transitRect;

	/**
	*  Essence of the chart.
	*/
	std::vector<ChartField> fields;

	std::list<VedicAspect> aspects;

	/**
	*  Polymorphic instance writing the text contents of the chart fields.
	*/
	FieldWriter *proxy = nullptr;

	/**
	*  Either 12 (Varga charts) or 28 (Sbc).
	*/
	const uint field_count = 0;

	/*
	*  Text int the center of the chart showing the Varga description.
	*/
	DocItemText centerItem;
};

/*************************************************//**
*
*
*
******************************************************/
class VedicChartGrid : public CeFieldGrid
{
public:
	VedicChartGrid( HybridChart*, CgVedicChartGrid* );

protected:
	virtual void updateAspects( Horoscope_ptr aspecting_h, Horoscope_ptr aspected_h ) override;

};


#endif // CEFIELDGRID_H_

