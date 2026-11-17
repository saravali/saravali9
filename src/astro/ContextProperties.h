/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ContextProperties.h
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

#ifndef _CONTEXTPROPERTIES_H_
#define _CONTEXTPROPERTIES_H_

#include "bootstrap.h"
#include "constants.h"
#include "Planet.h"
#include "Viewpoint.h"

#include <array>
#include <list>
#include <set>
#include <vector>

/*************************************************//**
*
* Contains major properties for an astrological context.
*
* Context semantic is used
* - globally in pure text mode and gdi mode
* - per view in full graphical mode, so each view can its own context properties (e.g. data type or column groups)
*
******************************************************/
class ContextProperties
{
public:

	ContextProperties();

	// preferred viewpoint
	void setPreferredViewpoint( const Viewpoint &v ) { preferredViewpoint = v; }

	Viewpoint getPreferredViewpoint() const { return preferredViewpoint; }

	Viewpoint getEffectiveViewpoint( const Viewpoint &stickyVp ) const;

	Planets getPlanets( const Viewpoint&, const std::set<FilterGroup> excludes = {} );

	void togglePlanetId( const PlanetId&, const Viewpoint& );

	bool hasPlanetId( const PlanetId&, const Viewpoint& ) const;

	const std::vector<std::pair<PlanetId,bool>> getPlanetPairs( const Viewpoint& ) const;

	void setPlanets( const Planets&, const Viewpoint& );

	// column style
	const std::vector<std::pair<ColumnGroup,bool>> &getColumnGroups( const Viewpoint &v ) const { return columnGroups[(int)v]; }

	void setColumnGroups( const std::vector<std::pair<ColumnGroup,bool>> &groups, const Viewpoint &v )
	{
		assert_valid_enum<Viewpoint>( v );
		columnGroups[(int)v] = groups;
	}
 
	void toggleColumnGroups( const ColumnGroup&, const Viewpoint& );

	uint getSkin( const WidgetCategory& );
	void setSkin( const WidgetCategory&, const uint &skin );
	uint increaseSkin( const WidgetCategory&, const int& );
	std::map<WidgetCategory, uint> &getSkins() { return skins; }

	uint getGeometry( const WidgetCategory& );
	void setGeometry( const WidgetCategory&, const uint& );
	uint increaseGeometry( const WidgetCategory&, const int& );

	bool isAnimated() const { return animated; }
	void setAnimated( const bool &b ) { animated = b; }
	void toggleAnimation() { animated =! animated; }

	int getLogZoom() const { return logZoom; }
	double getZoom() const;

	void setLogZoom( const int &z );
	void increaseLogZoom( const int z = 1.0 ) { setLogZoom( logZoom + z ); }

	int getChartLogZoom() const { return chartLogZoom; }
	double getChartZoom() const;

	void setChartLogZoom( const int &z );
	void increaseChartLogZoom( const int z = 1.0 ) { setChartLogZoom( chartLogZoom + z ); }

	Horoscope_ptr getHoroscope( const HoroscopeRole& ) const;
	Horoscope_ptr getRadixHoroscope() const { return getHoroscope( HoroscopeRole::Radix ); }

	void setHoroscope( const HoroscopeRole&, Horoscope_ptr );
	void setRadixHoroscope( Horoscope_ptr h ) { setHoroscope( HoroscopeRole::Radix, h ); }

	bool hasRadixHoroscope() const { return getHoroscope( HoroscopeRole::Radix ) != nullptr; }

	std::vector<HoroscopeRole> getActiveRoles() const;

	bool hasActiveRole( const HoroscopeRole& ) const;

	std::vector<Horoscope_ptr> getActiveHoroscopes() const;

	DataContext getDataContext() const { return datacontext; }
	void setDataContext( const DataContext &t ) { datacontext = t; }

	bool isLocaltime = false;

	// used on child window close to avoid bad calls 
	void invalidate() { valid = false; }

	// returns true if the main window is about to close
	bool isValid() const { return valid; }

private:

	Viewpoint preferredViewpoint;

	DataContext datacontext;

	bool animated;

	std::array<Horoscope_ptr, (int)HoroscopeRole::Last + 1> horoscopes;

	int logZoom = 0;

	int chartLogZoom = 0;

	bool valid;

	std::vector<std::pair<PlanetId,bool>> planets[VIEWPOINT_ARRAY_SIZE];

	std::vector<std::pair<ColumnGroup,bool>> columnGroups[VIEWPOINT_ARRAY_SIZE];

	std::map<WidgetCategory, uint> skins;

  std::map<WidgetCategory,uint> geoms;
};


#endif // _CONTEXTPROPERTIES_H_

