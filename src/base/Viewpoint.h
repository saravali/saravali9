/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Viewpoint.h
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

#ifndef _VIEWPOINT_H_
#define _VIEWPOINT_H_

#include "bootstrap.h"
#include "constants.h"
#include "Planet.h"

#include <list>
#include <map>
#include <vector>
#include <wx/string.h>

enum class CommandId;
enum class TextFormat;
enum class ViewId;

enum class AyanamsaType
{
	// json:enum
	None = -1,
	Tropical = 0,
	Builtin,
	Custom,
	First = Tropical,
	Last = Custom
};

enum class LunarNodeMode
{
	// json:enum
	None = -1,
	True = 0,
	Mean,
	First = True,
	Last = Mean
};

enum class HouseSystem
{
	// json:enum
	None = -1,
	Placidus = 0,
	Koch,
	Regiomontanus,
	Campanus,
	Porphyry,
	Equal,
	VehlowEqual,
	MeridianAxialRotation,
	AzimutalHorizontal,
	PolichPage,
	Alcabitus,
	Morinus,
	Krusinski,
	First = Placidus,
	Last = Krusinski
};

#define HOUSE_SYS_NUM (int)HouseSystem::Last + 1 // =13

enum class ColumnGroup
{
	// json:enum
	None = -1,
	ObjectName = 0,
	Longitude,
	Longitude90,
	Longitude45,
	Longitude225,
	Antiscia,
	Latitude,
	Declination,
	Speed,

	SignLord,
	Element,
	SignQuality,
	Signification,
	HousePosition,
	Navamsa,
	Nakshatra,
	NakshatraPada,
	KpLords,
	CharaKaraka,
	Dignity,
	ShastiamsaLord,
	DasaVarga,
	RekhaPoints,

	WesternAspectarium,
	CustomKeyValue,
	Error,
	Empty,

  First = ObjectName,
  Last = Empty
};

enum class BasedataAttribute
{
	// json:enum
  None = -1,
  Name = 0,
	LocalDate,
	LocalISODate,
	LocalTime,
	Timezone,
	DaylightSaving, 
	FullLocalDate,
	UniversalTime,
	UTDate,
	UTFullDate,

	LocationName,
	LocationLongitude,
	LocationLatitude,

	Weekday,
	JulianDate,
	SiderealTime,
	Eps,
	Armc,
	AyanamsaName,
	AyanamsaValue,
	AyanamsaNameAndValue,
	SunriseTime,
	SunsetTime,

	// general astrological properties
	SunSign,
	MoonSign,
	AscendantSign,
	SunNakshatra,
	MoonNakshatra,
	AscendantNakshatra,
	HouseSystem,

	// these groups are mostly used in western astrology
	PlanetElements,
	PlanetQuadrants,
	PlanetSignQualities,

	// explicite attributes of the groups above
	ElementFire,
	ElementEarth,
	ElementAir,
	ElementWater,
	ObjectsNorth,
	ObjectsWest,
	ObjectsSouth,
	ObjectsEast,
	SignQualityMovable,
	SignQualityFixed,
	SignQualityDual,

	// mostly used in vedic astrology
	MoonNakshatraPortion,
	MoonNakshatraPada,
	MoonNakshatraPadaPortion,
	MoonVimsottariInfluenceTime,
	MoonVimsottariLord,

	HoraLord,
	DinaLord,
	MasaLord,
	VarshaLord,
	LunarTithi,
	Varna,
	Gana,
	Yoni,
	Nadi,
	Rajju,
	Error,

  First = Name,
  Last = Error,

	FirstGeneral = First,
	LastGeneral = HouseSystem,
	
	FirstWestern = PlanetElements,
	LastWestern = PlanetSignQualities,

	FirstVedic = MoonNakshatraPortion,
	LastVedic = Rajju
};

/*************************************************//**
*
* 
*
******************************************************/
struct PlanetConfig : public Jsonable
{
	PlanetConfig( const Viewpoint = Viewpoint::None );

	bool isEmpty() { return items.size() == 0; }

	// json:start
	std::vector<std::pair<PlanetId,bool>> items;

	std::list<int> arabicParts;

	TextFormat textFormat = TextFormat::Long;

	int nodeNames = 0;

	bool useVedicNames = false;

  int uranusSymbol = 0;

  int plutoSymbol = 0;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;

private:
	Viewpoint viewpoint;
};

/*************************************************//**
*
* 
*
******************************************************/
struct PlanetPropertyConfig :	public Jsonable
{
	PlanetPropertyConfig( const Viewpoint = Viewpoint::None );

	bool isEmpty() { return columnGroups.size() == 0; }

	// json:start
	std::vector<std::pair<ColumnGroup,bool>> columnGroups;

	TextFormat textFormat = TextFormat::Long;

	bool vedicSignNames = false;

	PositionStyle positionStyle = PositionStyle::Default;

	DegPrecision precision = DegPrecision::Minute;

	int retroFormat = 0;

	TextFormat nakshatraTextFormat = TextFormat::Long;

  bool translateSignNames = false;

  int capricornSymbol = 0;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
class CalculationConfig : public Jsonable
{
public:

	CalculationConfig( const Viewpoint viewpoint = Viewpoint::None );

	Viewpoint getViewpoint() const { return viewpoint; }

	// json:start
	LunarNodeMode lunarNodeMode = LunarNodeMode::True;

	HouseSystem houseSystem = HouseSystem::Placidus;

	HouseStartMode houseStart = HouseStartMode::Cusp;

	int ayanamsa = 0;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;

private:
	Viewpoint viewpoint;
};

/*************************************************//**
*
* 
*
******************************************************/
class BasedataConfig : public Jsonable
{
public:

	BasedataConfig( const Viewpoint viewpoint = Viewpoint::None );

	Viewpoint getViewpoint() const { return viewpoint; }

	bool isEmpty() { return basedataItems.size() == 0; }

	// json:start
	bool showBasedataInPlanetListWidget = true;

	std::vector<std::pair<BasedataAttribute,bool>> basedataItems;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;

private:
	Viewpoint viewpoint;
};

/*************************************************//**
*
* 
*
******************************************************/
class ViewpointConfig : public Jsonable
{
public:

	ViewpointConfig( const Viewpoint viewpoint = Viewpoint::None );

	Viewpoint getViewpoint() const { return viewpoint; }

private:
	Viewpoint viewpoint;

public:

	// json:start
	PlanetConfig planets;

	PlanetPropertyConfig planetProps;

	CalculationConfig calculation;

	BasedataConfig basedata;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
class ViewpointHelper : public Singleton<ViewpointHelper>
{
	friend class Singleton<ViewpointHelper>;

public:

	// General Viewpoint section
	wxString getViewpointName( const Viewpoint& ) const;

	static Viewpoint getStickyVp( const WidgetCategory& );

	static Viewpoint getViewpointByStickyVp( const Viewpoint &preferredVp, const Viewpoint &stickyVp );

	static Viewpoint getViewpointByWidgetCategory( const Viewpoint &preferredVp, const WidgetCategory& );

	static Viewpoint getStickyViewVp( const ViewId& );

	// planet section
	//std::list<PlanetId> getDefaultPlanets( const Viewpoint& ) const;
	//std::list<PlanetId> getAvailablePlanets( const Viewpoint& ) const;
	Planets getDefaultPlanets( const Viewpoint& ) const;
	Planets getAvailablePlanets( const Viewpoint& ) const;

	// ColumnGroup Section
	wxString getColumnGroupName( const ColumnGroup&, const TextFormat& ) const;

	CommandId columnGroupToCommandId( const ColumnGroup& ) const;

	ColumnGroup getColumnGroup( const CommandId& ) const;

	std::list<ColumnGroup> getDefaultColumnGroups( const Viewpoint& ) const;
 
	std::list<ColumnGroup> getAvailableColumnGroups( const Viewpoint& ) const;

	// Basedata section
	std::list<BasedataAttribute> getAvailableBasedataConfigs( const Viewpoint& );

	std::list<BasedataAttribute> getDefaultBasedataConfigs( const Viewpoint& );

	std::list<BasedataAttribute> expandBasedataConfigList( const std::list<BasedataAttribute>& );

private:

	ViewpointHelper();

	const std::map<CommandId,ColumnGroup> commandToColumnGroup;

};


#endif // _VIEWPOINT_H_

