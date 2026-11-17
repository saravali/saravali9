/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Planet.h
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
#ifndef _PLANET_H_
#define _PLANET_H_

#include "bootstrap.h"
#include "constants.h"

#include <list>
#include <set>
#include <variant>
#include <vector>

enum class Viewpoint;

/**************************************************************
***
**       FACTORS = planets, luminaries, angles, fictitious points
***
***************************************************************/
#define MAX_ARABIC_PARTS 177
/**
* Enumeration for planets, i.e. objects in astrological sense.
*
* The enumeration contains planets (in astronomical sense), luminaries, angles, fictitious points, asteroids etc.
*/
enum class PlanetId
{
	// json:enum
	Error = -2,
	None = -1,

	Sun = 0,
	Moon,
	Mercury,
	Venus,
	Mars,
	Jupiter,
	Saturn, // 0 .. 6

	Uranus,
	Neptune,
	Pluto, // 7 ... 9

	// Nodes: meta ids, must resoved to mean/true node for calculation
	AscNode,
	DescNode,		// 10 + 11

	TrueAscNode,
	TrueDescNode,
	MeanAscNode,
	MeanDescNode,	// 12 ... 15

	// AS-MC-DC-IC
	Ascendant,
	Meridian,
	Descendant,
	ImumCoeli, // 16 ... 19

	// TNP = Trans Neptunian Planets
	Cupido,
	Hades,
	Zeus,
	Kronos,
	Apollon,
	Admetos,
	Vulkanus,
	Poseidon, // 20 ... 27

	// Planetoids and Lilith
	Chiron,
	Pholus,
	Ceres,
	Pallas,
	Juno,
	Vesta,
	Lilith, // 28 ... 34

	Aries,
	Cancer,
	Libra,
	Capricorn, // 35 ...38

	Dhuma,
	Vyatipata,
	Parivesha,
	Chapa,
	Upaketu, // 39 ... 43

	Kala,
	Mrityu,
	Ardhaprahara,
	Yamaghantaka,
	Gulika,
	Mandi,  // 44 ... 49

	BhavaLagna,
	HoraLagna,
	GhatikaLagna, // 52
	D9Lagna,      // 53
	Earth,   // 54

	House1,
	House2,
	House3,
	House4,
	House5,
	House6,
	House7,
	House8,
	House9,
	House10,
	House11,
	House12, // 55..66

	// groups of planets
	Inner,
	InnerVedic,
	Outer,
	InnerTNP,
	OuterTNP,
	AllTNP,
	Planetoids,  // 70 ... 73

	Upagrahas,
	Kalavelas,
	SpecialLagnas, // 74 .. 76

	AllHouses,
	FourHouses,
	SixHouses, // 77 .. 79

	ArabicParts, // 80

	First = Sun,
	Last = ArabicParts + MAX_ARABIC_PARTS, // 80 + 177 = 257

	FirstContracted = Inner,
	LastContracted = Last,

	// aliases for already defined planets
	Rahu = AscNode,
	Ketu = DescNode,

	LastEphem = Lilith,

	LastInner = Saturn,

	FirstNode = AscNode,
	LastNode = MeanDescNode,

	FirstAngle = Ascendant,
	LastAngle = ImumCoeli,

	FirstTNP = Cupido,
	LastInnerTNP = Kronos,
	FirstOuterTNP = Apollon,
	LastTNP = Poseidon,

	FirstPlanetoid = Ceres,
	LastPlanetoid = Vesta,

	FirstCardinalPoint = Aries,
	LastCardinalPoint = Capricorn,

	FirstUpagraha = Dhuma,
	LastUpagraha = Upaketu,

	FirstKalavela = Kala,
	LastKalavela = Mandi,

	FirstSpecialLagna = BhavaLagna,
	LastSpecialLagna = GhatikaLagna,

	FirstHouse = House1,
	LastHouse = House12,

	LastExtended = Earth,
	FirstArabic = ArabicParts + 1,
	LastArabic = FirstArabic + MAX_ARABIC_PARTS - 1
};

typedef std::vector<PlanetId> Planets;

constexpr int MAX_EPHEM_FACTORS  = (int)PlanetId::LastEphem + 1;
constexpr int LAST_FACTOR  = (int)PlanetId::Last;
constexpr int MAX_EXTENDED_FACTORS  = 15; // = 3 lagnas, 11 upagrahas/kalavelas + d9lagna
constexpr int LAST_EXTENDED_FACTOR = (int)PlanetId::LastExtended;

typedef std::variant<PlanetId, Sign > DasaLord;
mostream& operator <<( mostream&, const DasaLord& );

/*************************************************//**
*
*    CLASS   ---   PlanetComparator
*
******************************************************/
class PlanetComparator
{
public:
  PlanetComparator( const Planets &obs ) : obs( obs ) {}

  bool operator()( const PlanetId &p1, const PlanetId &p2 ) const;

private:

  const Planets &obs;
};

enum class FilterGroup
{
	// json:enum
	None = -1,

	// including earth
	Luminaries,

	// mercury .. saturn
	InnerPlanets,

	// uranus .. pluto
	OuterPlanets,

	// including special lagnas and D9Lagna
	Angles,

	Nodes,
	TNP,

	// chiron etc.
	ExtraPlanets,

	// aries, cancer, libra, capricorn
	CardinalPoints,

	Upagrahas,
	Kalavelas,
	ArabicParts,
	Houses,

	First = Luminaries,
	Last = Houses
};

/*************************************************//**
*
*   CLASS   ---   PlanetHelper
*
******************************************************/
struct PlanetHelper
{
	static bool planetMatchesFilter( const PlanetId&, const std::set<FilterGroup>& );

	// factor type section
	static inline bool isAngle( const PlanetId &f ) { return f >= PlanetId::FirstAngle && f <= PlanetId::LastAngle; }

	static inline bool isNode( const PlanetId &f ) { return f >= PlanetId::FirstNode && f <= PlanetId::LastNode; }

	static inline bool isNodeAlias( const PlanetId &f ) { return f >= PlanetId::TrueAscNode && f <= PlanetId::MeanDescNode; }

	static inline bool isAscNode( const PlanetId &f )
		{ return f == PlanetId::AscNode || f == PlanetId::TrueAscNode || f == PlanetId::MeanAscNode; }

	static inline bool isDescNode( const PlanetId &f ) { return PlanetHelper::isNode( f ) && ! PlanetHelper::isAscNode( f ); }

	/*
	* calculates the correct node for the viewpoint according to configuration, e.g. AscNode -> TrueAscNode or Ketu -> MeanDescNode.
	*
	* Objects other than nodes are passwded through
	*
	*/
	static PlanetId resolveNode( const PlanetId&, const Viewpoint& );

	static inline bool isHouse( const PlanetId &f ) { return f >= PlanetId::FirstHouse && f <= PlanetId::LastHouse; }

	static inline bool isEphemPlanet( const PlanetId &f ) { return f >= PlanetId::First && f <= PlanetId::LastEphem; }

	static inline bool isInnerPlanet( const PlanetId &f ) { return f >= PlanetId::First && f <= PlanetId::Saturn; }

	static inline bool isTNP( const PlanetId &f ) { return f >= PlanetId::FirstTNP && f <= PlanetId::LastTNP; }

	static inline bool isInnerTNP( const PlanetId &f ) { return f >= PlanetId::FirstTNP && f <= PlanetId::LastInnerTNP; }

	static inline bool isOuterTNP( const PlanetId &f ) { return f >= PlanetId::FirstOuterTNP && f <= PlanetId::LastTNP; }

	static inline bool isCardinalPoint( const PlanetId &f ) { return f >= PlanetId::FirstCardinalPoint && f <= PlanetId::LastCardinalPoint; }

	static inline bool isPlanetoid( const PlanetId &f ) { return f >= PlanetId::FirstPlanetoid && f <= PlanetId::LastPlanetoid; }

	static inline bool isUpagraha( const PlanetId &f ) { return f >= PlanetId::FirstUpagraha && f <= PlanetId::LastUpagraha; }

	static inline bool isKalavela( const PlanetId &f ) { return f >= PlanetId::FirstKalavela && f <= PlanetId::LastKalavela; }

	static inline bool isSpecialLagna( const PlanetId &f ) { return f >= PlanetId::FirstSpecialLagna && f <= PlanetId::LastSpecialLagna; }

	static inline bool isArabicPart( const PlanetId &f ) { return f >= PlanetId::FirstArabic && f <= PlanetId::LastArabic; }

	static inline bool isAvPlanet( const PlanetId &f ) { return ( f >= PlanetId::Sun && f <= PlanetId::Saturn ) || f == PlanetId::Ascendant; }

	static inline bool isPlanetGroup( const PlanetId &f ) { return f >= PlanetId::FirstContracted && f <= PlanetId::LastContracted; }

	static bool isExpanded( const Planets& );

	static Planets expandPlanets( const Viewpoint& );

	// resolves aliases like PlanetId::OuterPlanets
	static Planets expandPlanets( const Planets&, const std::list<int> arabicParts = {} );

	inline static Planets createJaiminiPlanets()
	{
		return 
		{
			PlanetId::Sun,
			PlanetId::Moon,
			PlanetId::Mars,
			PlanetId::Mercury,
			PlanetId::Jupiter,
			PlanetId::Venus,
			PlanetId::Saturn,
			PlanetId::Rahu,
			PlanetId::Ketu
		};
	}

	inline static Planets createVedicOnlyPlanets()
	{
		return 
		{
			PlanetId::Sun,
			PlanetId::Moon,
			PlanetId::Mars,
			PlanetId::Mercury,
			PlanetId::Jupiter,
			PlanetId::Venus,
			PlanetId::Saturn
		};
	}

	inline static Planets createAshtakaVargaPlanets()
	{
		return 
		{
			PlanetId::Sun,
			PlanetId::Moon,
			PlanetId::Mars,
			PlanetId::Mercury,
			PlanetId::Jupiter,
			PlanetId::Venus,
			PlanetId::Saturn,
			PlanetId::Ascendant
		};
	}
};

#endif // _PLANET_H_

