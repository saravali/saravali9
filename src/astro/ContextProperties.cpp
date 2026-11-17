/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ContextProperties.cpp
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

#include "ContextProperties.h"

#include "astroconfig.h"
#include "CategoryHelper.h"
#include "ChartGeometry.h"
#include "ConfigLoaderImpls.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "logger.h"
#include "constants.h"
#include "mathbase.h"
#include "Viewpoint.h"

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

//#define DEBUG_CONTEXTPROPERTIES

/*****************************************************
**
**   ContextProperties   ---   Constructor
**
******************************************************/
ContextProperties::ContextProperties()
{
	animated = false;
	valid = true;
	datacontext = DataContext::Radix;

	preferredViewpoint = astroconfig->general.defaultViewpoint;

	skins = gdiconfig->skins;
	geoms = gdiconfig->geoms;

	for( const Viewpoint v : Enum<Viewpoint>())
	{
#ifdef DEBUG_CONTEXTPROPERTIES
		debuglog << "ContextProperties::init viewpoint " << v;
#endif
		//planets[(int)v] = PlanetHelper::expandPlanets( astroconfig->getViewpointConfig( v ).planets.items );
		planets[(int)v] = astroconfig->getViewpointConfig( v ).planets.items;
		columnGroups[(int)v] = astroconfig->getViewpointConfig( v ).planetProps.columnGroups;
#ifdef DEBUG_CONTEXTPROPERTIES
		for( const auto &a : planets[(int)v] )
		{
			debuglog << "ContextProperties::init planet " << a.first << "/" << a.second;
		}
#endif
	}
}

/*****************************************************
**
**   ContextProperties   ---   getEffectiveViewpoint
**
******************************************************/
Viewpoint ContextProperties::getEffectiveViewpoint( const Viewpoint &stickyVp ) const
{
#ifdef DEBUG_CONTEXTPROPERTIES
	debuglog << "ContextProperties::getEffectiveViewpoint preferredViewpoint " << preferredViewpoint << " stickyVp " << stickyVp;
#endif
	return ViewpointHelper::get()->getViewpointByStickyVp( preferredViewpoint, stickyVp );
}

/*****************************************************
**
**   ContextProperties   ---   getHoroscope
**
******************************************************/
Horoscope_ptr ContextProperties::getHoroscope( const HoroscopeRole &role ) const
{
	if ( is_valid_enum( role ))
	{
		return horoscopes[(uint)role];
	}
	else return nullptr;
}

/*****************************************************
**
**   ContextProperties   ---   setHoroscope
**
******************************************************/
void ContextProperties::setHoroscope( const HoroscopeRole &role, Horoscope_ptr h )
{
	assert_valid_enum<HoroscopeRole>( role );
	horoscopes[(uint)role] = h;
}

/*****************************************************
**
**   ContextProperties   ---   getActiveRoles
**
******************************************************/
std::vector<HoroscopeRole> ContextProperties::getActiveRoles() const
{
	std::vector<HoroscopeRole> l;
	for( const Horoscope_ptr &h : horoscopes )
	{
		if ( h ) l.push_back( h->getRole() );
	}
	return l;
}

/*****************************************************
**
**   ContextProperties   ---   hasActiveRole
**
******************************************************/
bool ContextProperties::hasActiveRole( const HoroscopeRole &role ) const
{
	for( const Horoscope_ptr &h : horoscopes )
	{
		if ( h && h->getRole() == role ) return true;
	}
	return false;
}

/*****************************************************
**
**   ContextProperties   ---   getActiveHoroscopes
**
******************************************************/
std::vector<Horoscope_ptr> ContextProperties::getActiveHoroscopes() const
{
	std::vector<Horoscope_ptr> l;
	for( const Horoscope_ptr &h : horoscopes )
	{
		if ( h ) l.push_back( h );
	}
	return l;
}

/*****************************************************
**
**   ContextProperties   ---   getPlanets
**
******************************************************/
Planets ContextProperties::getPlanets( const Viewpoint &viewpoint, const std::set<FilterGroup> filter )
{
	assert_valid_enum( viewpoint );
	Planets l;
	for( const auto &a : planets[ (int)viewpoint ] )
	{
#ifdef DEBUG_CONTEXTPROPERTIES
		debuglog << "ContextProperties::getPlanets planets " << a.first << " active " << std::boolalpha << a.second;
#endif
		if ( a.second )
		{
			if ( PlanetHelper::planetMatchesFilter( a.first, filter )) continue;
			l.push_back( a.first );
		}
	}
	//assert( PlanetHelper::isExpanded( l ));
	return l;
}

/*****************************************************
**
**   ContextProperties   ---   setPlanets
**
******************************************************/
void ContextProperties::setPlanets( const Planets &plist, const Viewpoint &viewpoint )
{
	assert_valid_enum( viewpoint );
	assert( PlanetHelper::isExpanded( plist ));

	for( auto &p1 : planets[ (int)viewpoint ] )
	{
		p1.second = ( std::find( plist.begin(), plist.end(), p1.first ) != plist.end() ); 
	}
}

/*****************************************************
**
**   ContextProperties   ---   togglePlanetId
**
******************************************************/
void ContextProperties::togglePlanetId( const PlanetId &id, const Viewpoint &viewpoint )
{
	assert_valid_enum<PlanetId>( id );
	assert_valid_enum<Viewpoint>( viewpoint );
#ifdef DEBUG_CONTEXTPROPERTIES
	debuglog << "ContextProperties::togglePlanetId( " << id  << " Viewpoint " << viewpoint;
#endif

	for( auto &a : planets[ (int)viewpoint ] )
	{
		if ( a.first == id )
		{
			a.second = !a.second;
			break;
		}
	}
}

/*****************************************************
**
**   ContextProperties   ---   hasPlanetId
**
******************************************************/
bool ContextProperties::hasPlanetId( const PlanetId &id, const Viewpoint& viewpoint ) const
{
	assert_valid_enum<PlanetId>( id );
	assert_valid_enum<Viewpoint>( viewpoint );
	for( const auto &a : planets[(int)viewpoint ] )
	{
		if ( a.second && a.first == id ) return true;
	}
	return false;
}

/*****************************************************
**
**   ContextProperties   ---   getPlanetPairs
**
******************************************************/
const std::vector<std::pair<PlanetId,bool>> ContextProperties::getPlanetPairs( const Viewpoint &viewpoint ) const
{
	assert_valid_enum<Viewpoint>( viewpoint );
	return planets[ (int)viewpoint ];
}

/*****************************************************
**
**   ContextProperties   ---   toggleColumnGroups
**
******************************************************/
void ContextProperties::toggleColumnGroups( const ColumnGroup &group, const Viewpoint &viewpoint )
{
	assert_valid_enum<ColumnGroup>( group );
	assert_valid_enum<Viewpoint>( viewpoint );
	for( auto &a : columnGroups[ (int)viewpoint ] )
	{
		if ( a.first == group )
		{
			a.second = !a.second;
			break;
		}
	}
}

/*****************************************************
**
**   ContextProperties   ---   getSkin
**
******************************************************/
uint ContextProperties::getSkin( const WidgetCategory &cat )
{
	assert_valid_enum<WidgetCategory>( cat );
	const WidgetCategory cat2 = CategoryHelper::get()->getSkinCategory( cat );
#ifdef DEBUG_CONTEXTPROPERTIES
	debuglog << "ContextProperties::getSkin category " << cat << " superior cat " << cat2;
#endif

	const auto &skin = skins.find( cat2 );
	if ( skin != skins.end() ) return skin->second;
	else return 0;
}

/*****************************************************
**
**   ContextProperties   ---   setSkin
**
******************************************************/
void ContextProperties::setSkin( const WidgetCategory &cat, const uint &skin )
{
	assert_valid_enum<WidgetCategory>( cat );
	const WidgetCategory cat2 = CategoryHelper::get()->getSkinCategory( cat );
#ifdef DEBUG_CONTEXTPROPERTIES
	debuglog << "ContextProperties::setSkin category " << cat << " superior cat " << cat2;
#endif

	skins[cat2] = skin;
}

/*****************************************************
**
**   ContextProperties   ---   increaseSkin
**
******************************************************/
uint ContextProperties::increaseSkin( const WidgetCategory &cat, const int &i )
{
	const uint skincount = WidgetSkinLoader::get()->getSize();
	assert( skincount );

	const WidgetCategory cat2 = CategoryHelper::get()->getSkinCategory( cat );

	uint newskin = 0;
	const auto &skin = skins.find( cat2 );
	if ( skin != skins.end() )
	{
		newskin = ( skin->second + i ) % skincount;
		skins[cat2] = newskin;
	}
	return newskin;
}

/*****************************************************
**
**   ContextProperties   ---   getGeometry
**
******************************************************/
uint ContextProperties::getGeometry( const WidgetCategory &cat )
{
	assert_valid_enum<WidgetCategory>( cat );
	const WidgetCategory cat2 = CategoryHelper::get()->getGeomCategory( cat );

#ifdef DEBUG_CONTEXTPROPERTIES
	debuglog << "ContextProperties::getGeometry category " << cat << " superior cat " << cat2;
#endif
	const auto &geom = geoms.find( cat2 );
	if ( geom != geoms.end()) { return geom->second; }
	return UINT_FOR_LAZY_WAITING;
}

/*****************************************************
**
**   ContextProperties   ---   setGeometry
**
******************************************************/
void ContextProperties::setGeometry( const WidgetCategory &cat, const uint &g )
{
	assert_valid_enum<WidgetCategory>( cat );
	const WidgetCategory cat2 = CategoryHelper::get()->getGeomCategory( cat );
#ifdef DEBUG_CONTEXTPROPERTIES
	debuglog << "ContextProperties::setGeometry category " << cat << " superior cat " << cat2;
#endif
	geoms[cat2] = g;
}

/*****************************************************
**
**   ContextProperties   ---   increaseGeometry
**
******************************************************/
uint ContextProperties::increaseGeometry( const WidgetCategory &cat, const int &i )
{
	if ( i == 0 ) return 0;

	ChartGeometryLoader *loader = ChartGeometryLoader::get();
	const uint geomcount = loader->getSize();
	assert( geomcount );

	const WidgetCategory cat2 = CategoryHelper::get()->getGeomCategory( cat );

	uint newgeom = 0;
	const auto &geom = geoms.find( cat2 );
	if ( geom != geoms.end() )
	{
		newgeom = geom->second;
		if ( newgeom == UINT_FOR_LAZY_WAITING || newgeom == UINT_FOR_NOT_FOUND )
    {
			newgeom = loader->findFirstGeometry( cat, 0 );
		}

#ifdef DEBUG_CONTEXTPROPERTIES
		debuglog << "ContextProperties::increaseGeometry category " << cat << " superior cat " << cat2 << " increase " << i
			<< " geomcount " << geomcount << " geom " << newgeom;
#endif

		if ( i > 0 )
		{
			for( int j = 0; j < i; j++ )
			{
				newgeom = loader->findNextGeometry( cat2, newgeom );
			}
		}
		else
		{
			for( int j = 0; j < -i; j++ )
			{
				newgeom = loader->findPreviousGeometry( cat2, newgeom );
			}
		}
		geoms[cat2] = newgeom;
	}
#ifdef DEBUG_CONTEXTPROPERTIES
	debuglog << "ContextProperties::increaseGeometry category " << cat2 << " newgeom " << newgeom;
#endif
	return newgeom;
}

/*****************************************************
**
**   ContextProperties   ---   setLogZoom
**
******************************************************/
void ContextProperties::setLogZoom( const int &z )
{
  //constexpr int MIN_LOGZOOM = -8;
  constexpr int MIN_LOGZOOM = -20;
  constexpr int MAX_LOGZOOM = 20;

	logZoom = z;
  logZoom = std::min( MAX_LOGZOOM, logZoom );
  logZoom = std::max( MIN_LOGZOOM, logZoom );
}

/*****************************************************
**
**   ContextProperties   ---   setChartLogZoom
**
******************************************************/
void ContextProperties::setChartLogZoom( const int &z )
{
  //constexpr int MIN_LOGZOOM = -8;
  constexpr int MIN_LOGZOOM = -20;
  constexpr int MAX_LOGZOOM = 20;

	chartLogZoom = z;
  chartLogZoom = std::min( MAX_LOGZOOM, chartLogZoom );
  chartLogZoom = std::max( MIN_LOGZOOM, chartLogZoom );
}

/*****************************************************
**
**   ContextProperties   ---   getZoom
**
******************************************************/
double ContextProperties::getZoom() const
{
	return pow( 1.2, logZoom );
}

/*****************************************************
**
**   ContextProperties   ---   getChartZoom
**
******************************************************/
double ContextProperties::getChartZoom() const
{
	return pow( 1.2, chartLogZoom );
}

