/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/PlanetRing.h
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

#ifndef _PLANETRING_H_
#define _PLANETRING_H_

#include "ChartElementImpl.h"
#include "MDocument.h"
#include "Term.h"

#include <list>
#include <map>

class PlanetRing;

/*************************************************//**
*
*  A graphical wrapper for a single factor in graphical chart, can be planet, angle or term.
*
******************************************************/
struct PlanetRingItem : public DocItemText
{
public:
	PlanetRingItem(
		PlanetRing *ring,
		const HoroscopeRole&,
		const MTerm&,
		const double &longitude );

	void writeText();

	void format( Painter* );

	void paint( Painter* );

	PlanetRing *ring;

	const HoroscopeRole role;

	MComplex point = 0;

	MTerm term;

	const double longitude;

	double correctedLongitude;

	bool isSymbol = true;

	double baseFontSize = 100.0;
};

/*************************************************//**
*

*
******************************************************/
class PlanetRing : public ChartElement<CgPlanetRing>
{
public:

	PlanetRing( HybridChart*, CgPlanetRing* );

	~PlanetRing();

	virtual void onCoordinatesChanged() override;

	virtual void paint( Painter*, const MRect &refreshRect ) override;

	virtual std::list<MTerm> createTerms( const HoroscopeRole& ) = 0;

protected:

	void clearItems();

	virtual void adjustLongitudes( const uint& );

	virtual WidgetResponse onResponsiveChartEvent( const MComplex &p, const int keymod ) override;

	wxString writeTooltip() const;

	/**
	* auxiliary flag causing formatting on first paint. Flag is reset to true if any size or other relevant change takes place.
	*/
	bool formatRequired = true;

	double getLabelRadiusForRoleIndex( const uint &id );

	double getOuterRadiusForRoleIndex( const uint &id );

	/**
	* outer radius of the last role's planets
	*/
	virtual double getOuterRadius() override;

	/**
	* is set true if radix chart is inside the inner circle (mostly in western chart).
	*/
	bool reverseFirstRole = false;

	std::map<HoroscopeRole, std::list<PlanetRingItem*>> items;

	std::vector<HoroscopeRole> chartRoles;

};

#endif // _PLANETRING_H_

