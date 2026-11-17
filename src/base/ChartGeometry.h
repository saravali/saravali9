/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ChartGeometry.h
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

#ifndef _CHARTGEOMETRY_H_
#define _CHARTGEOMETRY_H_

#include "ConfigListLoader.h"
#include "geometry.h"

#include <map>
#include <set>

/*************************************************//**
*
* 
*
******************************************************/
struct ChartGeometry : public Jsonable
{
	ChartGeometry( wxString n = "", ChartShape = ChartShape::Circle );

	ChartGeometry( const ChartGeometry& );

	//ChartGeometry( ChartGeometry&& );

	virtual ~ChartGeometry();

	void removeElements();

	wxString name;

	// Basic geometry type of the chart, e.g. disc, rectangle etc.
	// Subelements have default type "inherited", i.e. they will have same the geometry as their parents
	ChartShape geometryType;

	bool containsElementType( const CgElementType& ) const;

	/**
	* Calls findWidgetCategory and updates the matchingCategories set.
	*/
	void updateMatchingCategories();

	bool matchesWidgetCategory( const WidgetCategory &cat ) const { return matchingCategories.contains( cat ); }

	/**
	* Interates through the elements and finds out if any of them matches the given category.
	* Only called once on creation, information is stored in attribute matchingCategories.
	* Return value is true if found.
	*/
	bool findWidgetCategory( const WidgetCategory& ) const;

#ifdef CPP26
	[[=skip_json]]
	std::set<WidgetCategory> matchingCategories;

	[[=skip_json]]
	std::map<CgElementType, CgElement*> elements;
#else
	std::set<WidgetCategory> matchingCategories;

	std::map<CgElementType, CgElement*> elements;
#endif

	// impls in ChartGeometry.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*  contains configs for generic charts
*
******************************************************/
class ChartGeometryLoader : public ConfigLoader<ChartGeometry>, public Singleton<ChartGeometryLoader>
{
	friend class Singleton<ChartGeometryLoader>;

public:

	CgElement *createDefaultElement( const CgElementType& );

	virtual ChartGeometry &getErrorConfig() override;

	std::pair<wxString,wxString> getNameAndDescription( const uint &g );

	std::vector<std::pair<uint,wxString>> getAllIdsAndNames( const WidgetCategory& );

	uint findNextGeometry( const WidgetCategory&, const uint &geom );

	uint findFirstGeometry( const WidgetCategory&, const uint &geom );

	uint findPreviousGeometry( const WidgetCategory&, const uint &geom );

private:

	ChartGeometryLoader();

	virtual void loadDefaultConfigs() override;

#ifdef CPP26

	// overriden from ConfigListLoader)
	virtual void read_single_node( ChartGeometry&, const Json::Value&, JsonParser& ) override;
#else
	virtual void loadSingleConfig( const Json::Value&, JsonHandler& ) override;
#endif

	inline static ChartGeometry errorElement = ChartGeometry( "Error", ChartShape::Circle );

	const static std::map<CgElementType, wxString> elementTags;

};

#endif

