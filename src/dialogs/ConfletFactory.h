/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ConfletFactory.h
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

#ifndef CONFLETFACTORY_H_
#define CONFLETFACTORY_H_

#include "gdibase.h"

class Conflet;
class wxPanel;
class wxWindow;

enum class ConfletId
{
	None = -1,
	Root = 0,
	General,
	StandardLocation,
	Toolbar,
	Atlas,
	Animation,
	Printout,
	Page,
	Pdf,

	// calculation
	Ephemeris,
	CustomAyanamsa,
	Directions,
	Varga,
	Dasa,
	Ashtakavarga,
	Jaimini,
	Kalavela,
	UranianConfig,

	// View
	MultipleView,
	TabbedViewRadix,
	TabbedViewDirection,
	TabbedViewMultiDirection,
	TabbedViewSolar,
	TabbedViewPartner,

	// widget
	BasicColors,
	ChartColors,
	Bardiagram,
	Fonts,
	TableStyle,

	// vedic
	VedicCalculation,
	VedicPlanets,
	VedicPlanetProperties,
	VedicBasedata,
	VargaChart,
	Sbc,

	// western
	WesternCalculation,
	WesternPlanets,
	WesternPlanetProperties,
	WesternBasedata,
	WesternChart,
	WesternAspects,

	// Uranian
	UranianCalculation,
	UranianPlanets,
	UranianPlanetProperties,
	UranianBasedata,
	UranianChart,
	UranianAspects,

	// Misc
	Miscellaneous,
	AstronomicalCalculation,
	AstronomicalPlanets,
	AstronomicalPlanetProperties,
	AstronomicalBasedata,
	UniversalChart,

	First = General,
	Last = UniversalChart
};

/*************************************************//**
*
* 
*
******************************************************/
struct ConfletIdTreenode
{
	ConfletIdTreenode( const ConfletId cid = ConfletId::None, const wxString title = "" )
		: id( cid ),
			title( title )
		{}

	ConfletIdTreenode( const ConfletId &cid, const std::list<ConfletIdTreenode> &children )
		: id( cid ),
		title( "" ),
		children( children )
	{
	}

	ConfletIdTreenode( const ConfletId &cid, const wxString &title, const std::list<ConfletIdTreenode> &children )
		: id( cid ),
		title( title ),
		children( children )
	{
	}

	const ConfletId id;

	const wxString title;

	std::list<ConfletIdTreenode> children;
};

/*************************************************//**
*
* 
*
******************************************************/
class ConfletFactory : public Singleton<ConfletFactory>
{
	friend class Singleton<ConfletFactory>;
public:

	Conflet *createConflet( wxWindow *parent, const ConfletIdTreenode& );

	std::list<ConfletIdTreenode> calculateTreenodes( const ConfletSet& );
	
	wxString getConfletTitle( const ConfletId& ) const;

private:

	ConfletFactory();

	void init();

	std::map<ConfletId, wxString> titles;
};

#endif // CONFLETFACTORY_H_
