/***********************************************************************
This is the source code of Saravali, a program for Vedic and western astrology.

File       src/base/astroconfig.h
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

#ifndef _ASTROCONFIG_H_
#define _ASTROCONFIG_H_

#include "datelocation.h"
#include "constants.h"
#include "Viewpoint.h"

#include <array>
#include <list>
#include <set>
#include <vector>
#include <wx/string.h>

/**************************************************************
***
**     GeneralConfig
***
***************************************************************/
struct GeneralConfig : public Jsonable
{
	// json:start
	Viewpoint defaultViewpoint = Viewpoint::Vedic;
	ViewpointChangePolicy viewpointChangePolicy = ViewpointChangePolicy::Medium;

	// ISO 369-1: 2 letters, en, de, fr etc.
	wxString languageCode;

	// ISO 369-1:
	std::list<wxString> additionalLanguageCodes;

	DateFormat dateFormat = DateFormat::ISO;

	// statusbar
	bool statusbarShowLogMessages = true;
	bool statusbarShowAscPosition = false;
	bool statusbarShowMoonPosition = false;
	int statusMessageTimeout = 10;

	bool openNewDocOnStart = true;
	bool askOnQuit = false;
	// json:end

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/**************************************************************
***
**     MatchFilter
***
***************************************************************/
struct MatchFilter : public Jsonable
{
	/**
	* 
	*/
	bool isPassThrough() const { return selectedPlanets.size() == 0; }

	/**
	* Clears the whole list of entries and resests the choice options
	*/
	void clear();

	// json:start
	int rule = 0;
	int side = 0;

	std::set<PlanetId> selectedPlanets;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/**************************************************************
***
**  RoleRestrictor adds an on/off switch and a filter to a HoroscopeRole.
**  Used in Uranian astrology
***
***************************************************************/
struct RoleRestrictor : public Jsonable
{
	RoleRestrictor() {}

	RoleRestrictor( const HoroscopeRole &role, const bool enabled = false ) : role( role), enabled( enabled ){}

	// json:start
	HoroscopeRole role = HoroscopeRole::None;

	bool enabled = false;

	MatchFilter filter;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* This a context data class for Uranian astrology.
*
******************************************************/
struct UranianSection : public Jsonable
{
	UranianSection()
	{
		chartFormulae = { 0 };
		matchFormulae = { 0 };
	}

	// transient params
#ifdef CPP26
	[[=skip_json]]
	int matchCount = 0;

	[[=skip_json]]
	PlanetId houseSystemPlanetId = PlanetId::Meridian;

	[[=skip_json]]
	double offset = 0;

	[[=skip_json]]
	bool offsetToggle = false;

	[[=skip_json]]
	bool eventsIncludeLrPermutations = false;

	// differentiation section
	[[=skip_json]]
	bool enablePlanetShift = false;

	[[=skip_json]]
	std::map<HoroscopeRole,PlanetId> shiftPlanetIds;

	[[=skip_json]]
	MatchFilter filter;

#else

	int matchCount = 0;

	PlanetId houseSystemPlanetId = PlanetId::Meridian;

	double offset = 0;

	bool offsetToggle = false;

	bool eventsIncludeLrPermutations = false;

	// differentiation section
	bool enablePlanetShift = false;

	std::map<HoroscopeRole,PlanetId> shiftPlanetIds;

	MatchFilter filter;

#endif

	// json:start
	bool includeAntiscia = false;

	bool showPpLongitude = false;
	bool showPpScore = false;

	std::list<int> chartFormulae;
	int chartDivision = 1;
	int progressionMode = 0;

	EventSortOrder sortOrder = EventSortOrder::RightPlanet;
	std::list<RoleRestrictor> roleRestrictors;

	int matchDivision = 1;
	std::list<int> matchFormulae;
	double orbis = 1.5;

	bool matchesShowDuedate = false;
	bool matchesShowDuedateTime = false;
	bool matchesShowOrbis = true;
	bool matchesShowSpeed = false;
	bool matchesShowResiduum = false;
	bool matchesShowRole = false;
	bool matchesShowText = false;
	// json:end

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct UranianConfig : public Jsonable
{
	UranianConfig()
	{
		divisionPrimes = { 2 };
	}

	// json:start
	int antisciaDisplayStyle = 0;

	UranianMatchFormat matchFormat = UranianMatchFormat::Default;

	std::set<int> divisionPrimes;

	int maximumDivision = 16;

	uint maxMatchCount = 1000;

	// cut seconds and long planet names to keep tables well arranged
	bool doTrimTableItems = true;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct YearLengthConfig : public Jsonable
{
	double getValue() const;

	// json:start
	int type = 0;

	double customValue = YEAR_LEN;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct DirectionConfig : public Jsonable
{
	DirectionConfig();

	// json:start
	YearLengthConfig yearLength;

	std::list<HoroscopeRole> multiDirectionRoles;

	std::list<RoleRestrictor> eventFinderItems;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct KalavelaElement : public Jsonable
{
	KalavelaElement(
		const KalavelaLord lord = KalavelaLord::None,
		const KalavelaPortion portion = KalavelaPortion::Beginning )
		: lord( lord ),
		portion( portion )
	{}

	// json:start
	KalavelaLord lord;
	KalavelaPortion portion;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct KalavelaConfig : public Jsonable
{
	KalavelaConfig();

	// json:start
	std::map<PlanetId,KalavelaElement> elements;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct VargaConfig : public Jsonable
{
	// json:start
	int moonBenefic = 1;
	int mercuryBenefic = 1;

	// 0 = Sign, 1 = Varga
	int tempFriendMode = 0;

	int vargaHoraMode = 0;
	int vargaDrekkanaMode = 0;
	int vargaChaturtamsaMode = 0;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct DasaCalculationConfig : public Jsonable
{
	// json:start
	// start of last Mahadasa in years after birth
	int startOfLastMahadasa = 100;

	NakshatraPortionMode nakshatraPortionMode = NakshatraPortionMode::Longitude;

	KalachakraMode kalachakraMode = KalachakraMode::Progressive;

	DasaShowDateMode dasaDateDisplayMode = DasaShowDateMode::StartDate;

	YearLengthConfig yearLength;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct AshtakavargaConfig : public Jsonable
{
	// json:start
	int ashtakavargaMode = 0;
	int ashtakavargaSarva = 0;
	int ashtakavargaNumberMode = 0;

	// 0 = Parasara 1 = Mantreswar
	int ashtakavargaSodhyaPindaMode = 0;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct JaiminiConfig : public Jsonable
{
	// json:start
	int arudhaMode = 0;
	int charaKarakaMode = 0;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct AnimationConfig : public Jsonable
{
	// json:start
	int interval = 5;
	int mode = 0;
	int stepLength = 5;
	int stepDimension = 1;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct EphemConfig : public Jsonable
{
	EphemConfig();

	// json:start
	int useEphemerisTime;
	EphemCoordinateMode ephemCoordinateSystem;
	int useTruePositions;
	bool sunrise_refrac;
	int sunrise_def;

	wxString sePath;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};


/*************************************************//**
*
* 
*
******************************************************/
struct CustomAyanamsaConfig : public Jsonable
{
	// json:start
	double t0 = DEFAULT_T0;
	double ayan_t0 = DEFAULT_AYAN_T0;
	double aya_constant_period = DEFAULT_AYA_PERIOD;
	bool hasConstantPeriod = false;

	bool t0_ut = DEFAULT_AYAN_TO_UT;

	int precessionModel = DEFAULT_PRECESSION_MODEL;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct Astroconfig : public Jsonable
{
	Astroconfig();
	virtual ~Astroconfig() {}

	ViewpointConfig &getViewpointConfig( const Viewpoint& );

	UranianSection &getUranianSection( const UranianContext& );

	// json:start
	GeneralConfig general;

	DirectionConfig directions;

	VargaConfig varga;
	DasaCalculationConfig dasa;
	AshtakavargaConfig ashtakavarga;
	JaiminiConfig jaimini;

	KalavelaConfig kalavela;

	AnimationConfig animation;

	EphemConfig ephem;
	CustomAyanamsaConfig customAyanamsa;

	Location standardLocation;

	std::map<Viewpoint,ViewpointConfig> viewpointConfigs;

	std::array<PlanetPropertyConfig,4> objectPropertyConfigs;

	UranianConfig uranian;
	std::map<UranianContext,UranianSection> uranianSections;

	void load();
	void save();

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

#endif

