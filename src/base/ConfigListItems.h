/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ConfigListItems.h
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

#ifndef _CONFIGLISTITEMS_H_
#define _CONFIGLISTITEMS_H_

#include "ColorConfig.h"
#include "constants.h"
#include "gdibase.h"
#include "Planet.h"

#include <set>
#include <variant>

class Rule;
typedef std::shared_ptr<Rule> Rule_ptr;

/*************************************************//**
*
*  config and descrition for a single Dasa configuration
*
******************************************************/
struct DasaConfig : public Jsonable
{
	// json:start
	wxString description;
	wxString effect;
	wxString source;
	wxString sourcefile;
	DasaLord lord = PlanetId::None;
	DasaLord sublord = PlanetId::None;
	DasaLord subsublord = PlanetId::None;
	bool isvalid = true;
	DasaId dasaId = DasaId::None;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct TableStyle : public Jsonable
{
	TableStyle();

	virtual ~TableStyle() {};

	// json:start
	wxString name;

	bool useHeaderColors = true;
	wxColour headerTextColor;
	wxColour headerBgColor;

	bool useCellColors = true;
	wxColour cellTextColor;

	int cellBgMode = 0;
	wxColour allCellBgColor;
	wxColour oddCellBgColor;
	wxColour evenCellBgColor;

	bool showGrid = true;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*  This class is use for adapting the style of widgets.
*
*  It is base on override semantics, i.e. the attributes
*  do not take effect until they are set explicitly. The default
*  constructed WidgetSkin will produce the default skin, brush, etc.
*
******************************************************/
struct WidgetSkin : public Jsonable
{
	// impl in CategoryHelper.cpp
	WidgetSkin();

	// json:start
	wxString name;

	wxPen pen;

	MBrush brush;

	wxColour textColor;

	MBrush markedFieldBrush;

	ChartFieldPattern signPattern;

	ChartFieldPattern housePattern;

	ChartFieldPattern nakshatraPattern;

	// impl in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*  config and descrition for a single Yoga configuration
*
******************************************************/
struct YogaConfig : public Jsonable
{
	YogaConfig();

	~YogaConfig();

	// json:start
	wxString description;
	wxString group;
	wxString effect;
	wxString source;
	wxString rule;
	// json:end

	Rule_ptr rtree;
	
	bool highervargas = true;

  static inline int create_count = 0;

  static inline int delete_count = 0;

	// impl in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};


#endif // _CONFIGLISTITEMS_H_



