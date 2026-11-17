/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/constants.h
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
#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "bootstrap.h"

#include <limits.h>

/**************************************************************
***
**  VARIOUS CONSTANTS
***
***************************************************************/
constexpr double YEAR_LEN = 365.25;
constexpr double SQRT2 = 1.414213562;

// Constants for transformations deg <=> rad
constexpr double DEG2RAD  = 0.0174532925199433;
constexpr double RAD2DEG = 57.2957795130823;

// point to mm
constexpr double PT2MM = .352777777778;

constexpr double DEFAULT_T0 = 2435553.5;
constexpr double DEFAULT_AYAN_T0 = 23.250182778 - 0.004658035;
constexpr double DEFAULT_AYA_PERIOD = 25771.4021;
constexpr bool DEFAULT_AYAN_TO_UT  = false;
constexpr int DEFAULT_PRECESSION_MODEL = 1; // SEMOD_PREC_IAU_1976

// min and max year according to Moshier ephemeris, see Swiss Ephemeris doc
constexpr double MIN_EPHEM_YEAR = -3000.0;
constexpr double MAX_EPHEM_YEAR = 3000.0;
// corresponding JDs
constexpr double MIN_EPHEM_JD = 625673;
constexpr double MAX_EPHEM_JD = 2817152;

constexpr uint UINT_FOR_NOT_FOUND = UINT_MAX - 1;
constexpr uint UINT_FOR_OUTSIDE = UINT_MAX - 2;
constexpr uint UINT_FOR_NOT_SET = UINT_MAX - 3;
constexpr uint UINT_FOR_LAZY_WAITING = UINT_MAX - 4;

enum class DataContext
{
	// json:enum
  None = -1,
  Radix = 0,
  Direction,
	Solar,
  Partner,
	MultiDirection,
  Ashtakavarga,
  First = Radix,
  Last = Ashtakavarga
};

enum class HoroscopeRole
{
	// json:enum
	None = -1,
	Radix = 0,
	Partner1,
	Partner2,
	Composite,
	Solar,

	Transit,
	Progressive, // = secondary directions, 1 day = 1 year

	// start shifted modes
	SolarArc,    // Solar Arc by progressive horoscope, all planets are shifted by this value
	ReverseSolarArc,
	DoubleSolarArc,
	HalfSolarArc,
	LunarArc,
	ConstantArc,

	Lunations,    // synodic lunations
	Profections,  // solar arc of synodic lunations

	// Used for second horoscope in uranian chart (offset shift)
	HandShifted,

	FirstDirection = Transit,
	LastDirection = Profections,

	First = Radix,
	Last = HandShifted,
};

enum class WidgetCategory
{
	// json:enum
	None = -1,

	// charts and chart grids
	UniversalChart = 0,
	WesternChart,
	UranianChart,
	SarvatobhadraChart,
  VargaChart,
	RasiNavamsaGrid,
	Varga1Grid,
	Varga2Grid,
	Varga3Grid,
	
	UniversalChartWidget,
	WesternChartWidget,
  VargaChartWidget,
  SbcWidget,
	UranianChartWidget,

	// Ashtakavarga
	AvWidget,
	BardiagramGeneric,
	AvPindaDiagram,
	AvRasiDiagram,

	YogaWidget,

	// dasa (tree/graphical)
	DasaChartWidget,
	DasaChart,
	DasaTreeWidget,
	DasaTree,
	DasaTreeCompositeWidget,
	DasaTreeComposite,

	// Shadbala
	ShadbalaWidget,
	VimsopakabalaDiagram,
	BalaOverviewDiagram,
	SthanabalaDiagram,
	SaptabalaDiagram,
	KalabalaDiagram,
	BalaSummaryDiagram,

	//PlanetaryPictures
	PpWidget,
	PpTree,

	// EventFinder
	EventFinderWidget,
	EventFinderGraphical,

	// Ephemeris
	EphemWidget,
	EphemGraphical,

	EclipseWidget,
	HoraWidget,

	UniversalRadixWidget,
	VargaGridWidget,
	DirectionWidget,
	MultiDirectionWidget,
	SolarWidget,

	PartnerWidget,
	SynastryPair,
	CompositePair,

	ComprehensiveTextWidget,

	// Text
	TextGeneric, // only for general cases, e.g. in ConfigDialog
	TextAshtakoota,
	TextSynastry,
	TextEclipse,
	TextHora,
	TextFileAndDirectoryReport,
	TextLicense,
	TextEphemCoordinates,
	TextEphemDetails,
	TextEphemLunar,
	TextEphemIngress,
	TextEphemKp,
	TextSensitivePoints,
	TextUranianMatches,
	TextYoga,
	TextDasaTree,

	// Uranian Matches
	MatchesWidget,

	//SensitivePoints
	SpWidget,
	
	// fist item for ComprehensiveTextView
	TextBaseData,
	TextArabicParts,
	TextAshtakavarga,
	TextPlanetBalance,
	TextDasaShort,
	TextDasaCompact,
	TextDasaComprehensive,
	TextJaimini,
	TextKp,
	TextNakshatra,
	TextPlanetListVedic,
	TextPlanetListWestern,
	TextPlanetListUranian,
	TextPlanetListAstronomical,
	TextShadbala,
	TextUranianHouses,
	TextVarga,
	TextVedicAspectarium,
	TextVedicHouses,
	TextWesternAspectarium,
	TextWesternAspectList,

	TextPlanetListUniversal,
	TextPlanetListCombined,

	// aggregrations of two or three widgets
	PlanetListChartPair,
	VedicTriple,

	YogaEditorWidget,
	EmptyWidget,

	FirstChart = UniversalChart,
	LastChart = Varga3Grid,

	FirstText = TextGeneric,
	LastText = TextPlanetListCombined,

	FirstComprehensiveTextWidgetItem = TextBaseData,
	LastComprehensiveTextWidgetItem = TextWesternAspectList,

	FirstPlanetList = TextPlanetListVedic,
	LastPlanetList = TextPlanetListAstronomical,

	FirstBardiagram = BardiagramGeneric,
	LastBardiagram = BalaSummaryDiagram,

	First = UniversalChart,
	Last = EmptyWidget
};

#define IS_TEXT_WIDGET( c ) ( c >= WidgetCategory::FirstText && c <= WidgetCategory::LastText )
#define IS_CHART_WIDGET( c ) ( c >= WidgetCategory::FirstChart && c <= WidgetCategory::LastChart )
#define IS_OBJECTLIST_WIDGET( c ) (( c >= WidgetCategory::FirstPlanetList && c <= WidgetCategory::LastPlanetList ) || c == WidgetCategory::TextPlanetListCombined || c == WidgetCategory::TextPlanetListUniversal )
#define IS_BARDIAGRAM_WIDGET( c ) ( c >= WidgetCategory::FirstBardiagram && c <= WidgetCategory::LastBardiagram )

enum class Viewpoint
{
	// json:enum
	None = -1,
	Vedic = 0,
	Western,
	Uranian,
	Astronomical,
	First = Vedic,
	Last = Astronomical
};

//constexpr int VIEWPOINT_ARRAY_SIZE = (int)Viewpoint::Last + 1;
constexpr int VIEWPOINT_ARRAY_SIZE = 4;

// behaviour of the application on change of viewport, e.g. vedic/western switch
enum class ViewpointChangePolicy
{
	// json:enum
	None = -1,
	Fixed = 0,
	Medium,
	Flexible,
	First = Fixed,
	Last = Flexible
};

// key for UranianSection, stored in astroconfig
enum class UranianContext
{
	// json:enum
	None = -1,
	RadixMatches = 0,
	RadixPp,
	RadixChart,
	DirectionMatches,
	DirectionPp,
	DirectionChart,
	MultiDirectionMatches,
	MultiDirectionChart,
	SolarMatches,
	SolarPp,
	SolarChart,
	PartnerMatches,
	PartnerPp,
	PartnerChart,
	SensitivePoints,
	EventFinder,
	First = RadixMatches,
	Last = EventFinder
};

#define IS_CHART_CONTEXT( ctx ) ( ctx == UranianContext::RadixChart \
	|| ctx == UranianContext::DirectionChart \
	|| ctx == UranianContext::MultiDirectionChart \
	|| ctx == UranianContext::SolarChart \
	|| ctx == UranianContext::PartnerChart )

enum class UranianMatchFormat
{
	// json:enum
	None = -1,
	Default = 0,
	Signs,
	Numbers,
	First = Default,
	Last = Numbers
};

enum class TermId
{
	// json:enum
  None = -1,
  Single = 0,
  Midpoint,
  ReflectionPoint,
  Sum,
  Difference,
  Triple,
  TripleSum,
  AntiSingle,
  AntiMidpoint,
  AntiReflectionPoint,
  AntiSum,
  AntiDifference,
  AntiTriple,
  AntiTripleSum,

  First = Single,
  Last = AntiTripleSum,

  LastSingleTerm = TripleSum,
  FirstAntiTerm = AntiSingle
};

enum class WesternAspectId
{
	// json:enum
	None = -1,
	Conjunction = 0,
	Opposition,
	Trine,
	Square,
	Sextile,
	Quincunx,
	Semisquare,
	Sesquisquare,
	Semisextile,
	Quintile,
	Septile,
	Novile,
	Parallele,
	Contraparallele,

	First = Conjunction,
	Last = Contraparallele,
};

// aspect id plus numerator, e.g. Septile,2 for Biseptile
typedef std::pair<WesternAspectId,int> WesternAspectType;

// Sort orders for events and aspects
enum class EventSortOrder
{
	// json:enum
	None = -1,
	RightPlanet = 0, // planet on right side of equation p1 = p2
	LeftPlanet,      // left side of p1 = p2
	Orbis,           // negative values first i.e. matchings in the past for e.g. solar arc
	OrbisAbsolute,   // smallest value (i.e. best matching) first
	OrbisReverse,    // see orbis
	Type,            // conjunction, opposition etc. also used in uranian astrology (gradkreis)
	TypeReverse,    
	Speed,           // speed of the term, may be left, right or a difference of both
	SpeedReverse,
	Date,            // matching date
	DateReverse,   

	// start pp
	Longitude,
	Term,
	Score,           // end pp
	First = RightPlanet,
	Last = Score
};

enum class VedicAspectId
{
	// json:enum
	None = -1,
	GrahaDrishti = 0,
	RasiDrishti,
	Vedha,

	First = GrahaDrishti,
	Last = Vedha
};

enum class DegPrecision
{
	// json:enum
	None = -1,

	// show always DD:MM
	Minute,

	// show always DD:MM:SS
	Second,

	// show always DD:MM:SS.xxxxx
	More,

	// cut minute if zero, e.g. 1:00:00 -> 1
	FlexibleCutMinute,

	// cut second only if zero, e.g. 1:00:00 -> 1:00
	FlexibleCutSecond,

	First = Minute,
	Last = FlexibleCutMinute
};

enum class PositionStyle
{
	// json:enum
	None = -1,
	Default,
	Compact,
	Colons,
	Vedic,
	First = Default,
	Last = Vedic
};

enum class DateFormat
{
	// json:enum
	None = -1,
	ISO,
	Local,
	First = ISO,
	Last = Local
};

enum class DegreeType
{
	// json:enum
	None = -1,
	Longitude,
	Latitude,
	Timezone,
	Signed,
	First = Longitude,
	Last = Signed
};

// Ephemeris
enum class EphemCoordinateMode
{
	// json:enum
	None = -1,
	Geocentric = 0,
	Topocentric,
	Equatorial,
	Barycentric,
	Heliocentric,
	First = Geocentric,
	Last = Heliocentric,
};

enum class DaylightSavingMode
{
	// json:enum
	None = -1,
	Automatic,
	FixedZero,
	FixedOneHour,
	First = Automatic,
	Last = FixedOneHour
};

// Solar events
enum class SolarEventType
{
	None = -1,
	Sunrise = 0,
	Sunset,
	Noon,
	Midnight,
	First = Sunrise,
	Last = Midnight
};

/**
* Enumeration for Houses. Caution: first house has numeric value 0.
*
* All houses have a short form h1, h2 etc.
*/
enum class House
{
	None = -1,

	House1 = 0, /**< First house  */
	House2, /**< Second house etc.  */
	House3,
	House4,
	House5,
	House6,
	House7,
	House8,
	House9,
	House10,
	House11,
	House12,

	h1 = House1, /**< Alias for House1 etc. */
	h2 = House2,
	h3 = House3,
	h4 = House4,
	h5 = House5,
	h6 = House6,
	h7 = House7,
	h8 = House8,
	h9 = House9,
	h10 = House10,
	h11 = House11,
	h12 = House12,

	First = House1,
	Last = House12
};

// houses start either at cusp or at sandhi (middle)
enum class HouseStartMode
{
	// json:enum
	None = -1,
	Cusp,
	Middle
};

// moving direction of a planet
enum class MovingDirection
{
	Indefinite = 0,
	None,
	Direct,
	Retrograde,
	Stationary
};

/**
* Enumeration for zodiacal signs
*/
enum class Sign
{
	Error = -2,
	None = -1,
	Aries = 0,
	Taurus,
	Gemini,
	Cancer,
	Leo,
	Virgo,
	Libra,
	Scorpio,
	Sagittarius,
	Capricorn,
	Aquarius,
	Pisces,
	First = Aries,
	Last = Pisces
};

// Text styles for planets, signs etc
enum class TextFormat
{
	// json:enum
	None = -1,
	Short = 0,
	Medium,
	Long,
	Symbol,
	First = Short,
	Last = Symbol
};

enum class RetroDisplayStyle
{
	// json:enum
	None = -1,
	Nothing = 0,
	Symbol,
	SimpleR,
	Brackets,
	First = Nothing,
	Last = Brackets
};

// Text styles for house numbers
enum class HouseNumberFormat
{
	// json:enum
	None = -1,
	Nothing,
	Arabic,
	Roman,
	First = Nothing,
	Last = Roman
};

enum class FieldStatus
{
	// json:enum
  None = -1,
  Markup = 0,
  Zoom = 1,
  Aspecting,
  Aspected,
  PosFull,
  PosInner,
  PosOuter,
  AllAspects,

	First = Markup,
	Last = AllAspects
};

// Coordinate modes, used by Ephemeris
enum class CoordinateMode { Longitude = 0, Declination, AbsDeclination, Latitude };

enum class Align
{
	// json:enum
	None = 0,
	VCenter =  0x01,
	Bottom  =  0x02,
	Top     =  0x04,
	HCenter =  0x08,
	Left    =	 0x10,
	Right   =	 0x20,
	Center  =  VCenter + HCenter,

	South = Bottom + HCenter,
	SouthWest = Bottom + Right,
	West = Left + VCenter,
	NorthWest = Top + Right,
	North = Top + HCenter,
	NorthEast = Top + Left,
	East = Right + VCenter,
	SouthEast = Bottom + Left,

	First = VCenter,
	Last = Center
};

enum class PageOrientation
{
	// json:enum
	None = -1,
	Portrait,
	Landscape
};

inline bool operator& ( const Align &a1, const Align &a2 )
{
	return (int)a1 & (int)a2;
}

enum class IconSize
{
	None = -1,
	Small,
	Medium,
	Large,

	First = Small,
	Last = Large
};


// writer export options
// used by textclient, exporter
enum class WidgetExportType
{
	None = -1,
	Text = 0,
	Csv,
	Html,
	Image,
	PlainHtml,
	First = Text,
	Last = PlainHtml
};

// 15 direction modes + radix
constexpr uint MAX_HOROSCOPES = 16;

// Graphical chart frame types
enum class ChartShape
{
	// json:enum
	None = -1,
	Inherited = 0, // same as parent
	Circle,
	Ellipse,
	Square,
	Rectangle,
	Triangle,
	Rect2,    // test
	First = Inherited,
	Last = Rect2
};

enum class WesternChartOrientation
{
	// json:enum
	None = -1,
	Ascendant = 0,
	House1,
	Meridian,
	Aries0,
  Aries15,
  Cancer0,
  Cancer15,
  Leo0,
  Libra0,
  Libra15,
  Capricorn0,
  Capricorn15,
	First = Ascendant,
  Last = Capricorn15
};

/*************************************************//**
*
* 
*
******************************************************/
struct PlanetPosition
{
	double longitude = 0.0;

	double lspeed = 0.0;

	double latitude = 0.0;

	double bspeed = 0.0;

	double declination = 0.0;

	double dspeed = 0.0;
};

/**************************************************************
***
**  GENERAL ASTROLOGY
***
***************************************************************/
enum class Element
{
	None = -1,
	Fire = 0,
	Earth,
	Air,
	Water,
	First = Fire,
	Last = Water
};

enum class SignQuality
{
	None = -1,
	Movable = 0,
	Fixed,
	Dual,
	First = Movable,
	Last = Dual
};

enum class ChartSide
{
	None = -1,
	North = 0,
	South,
	East,
	West,
	First = North,
	Last = West
};

/**
*  Enumeration for supported Vargas
*/
enum class Varga
{
	// json:enum
	None = -1,
	Rasi = 0, /**< D-1 etc.  */
	Navamsa,
	Hora,
	Drekkana,
	Chaturtamsa,
	Sastamsa,
	Saptamamsa,
	Ashtamsa,
	Dasamsa,
	Dvadasamsa,
	Shodasamsa,
	Vimsamsa,
	Siddhamsa,
	Bhamsa,
	Trimsamsa,
	Chatvarimsamsa,
	Akshavedamsa,
	Shastiamsa,
	Ashtottaramsa, /**< D-108  */
	DvadasDvadasamsa, /**< D-144  */
	Bhava,  /**< According to house cusps or sandhi  */
	First = Rasi,
	Last = Bhava
};

const int vargaDivisions[(int)Varga::Last + 1] =
{
  1, 9, 2, 3, 4,
  6, 7, 8, 10, 12,
  16, 20, 24, 27, 30,
  40, 45, 60, 108, 144,
  0
};

/**
*  Tabs in Varga widget
*/
enum class VargaGroup
{
  None = -1,
  Shadvarga = 0,
  Varga2,
  Varga3,
  First = Shadvarga,
  Last = Varga3
};

enum class InfluenceType
{
  None = -1,
  Benefic,
  Malefic,
  Neutral
};

// planetary friendship
enum class PlanetaryFriendship
{
	None = -1,
	BestFriend,
	Friend,
	Neutral,
	Enemy,
	SwornEnemy,
	First = BestFriend,
	Last = SwornEnemy 
};

enum class Dignity
{
	None = -1,
	Exalted = 0,
	Debilated,
	Moolatrikona,
	OwnSign,
	BestFriend,
	Friend,
	Neutral,
	Enemy,
	SwornEnemy
};

// Karakas of Jaimini Astrology
enum class CharaKaraka
{
	None = -1,
	Atma = 0,
	Amatya,
	Bhratru,
	Matru,
	Pitru,
	Putra,
	Gnati,
	Dara,
	First = Atma,
	Last = Dara
};

enum class Varna
{
	None = -1,
	Brahmin = 0,
	Kshattriya,
	Vaishya,
	Shudra,
	First = Brahmin,
	Last = Shudra
};

enum class Gana
{
	None = -1,
	Deva = 0,
	Manuj,
	Rakshas,
	First = Deva,
	Last = Rakshas
};

enum class Nadi
{
	None = -1,
	Aadi = 0,
	Madhya,
	Antya,
	First = Aadi,
	Last = Antya
};

enum class Vashya
{
	None = -1,
	Quadruped = 0,
	Human,
	Jalachara,
	Leo,
	Scorpio,
	First = Quadruped,
	Last = Scorpio
};

enum class AvType
{   
  None = -1, 
  Rekha = 0,
  Trikona,
  Ekadhi,
  RasiSama,
  NakshatraSama,
  First = Rekha,
  Last = NakshatraSama
};  

enum class VedicAspectStrength
{
	// json:enum
	None = -1,
	P25,
	P50,
	P75,
	Full,

	First = P25,
	Last = Full
};

enum class DasaLevel
{
	// json:enum
	None = -4,
	TreeRoot = -3,
	Folder,
	Root,
	Mahadasa = 0,
	Antardasa,
	Pratyantardasa,
	Sookshmantardasa,
	Pranadasa,
};

// Dasa calculation and display
enum class NakshatraPortionMode
{
	// json:enum
  None = -1,
	Longitude = 0,
	EntryTime,

	First = Longitude,
	Last = EntryTime
};

enum class KalachakraMode
{
	// json:enum
  None = -1,
	Progressive,
	Cyclic,
	ProgressiveSameNakshatra,
	PortionZero,

	First = Progressive,
	Last = PortionZero,
};

enum class DasaShowDateMode
{
	// json:enum
  None = -1,
	StartDate,
	EndDate,
	First = StartDate,
	Last = EndDate,
};

// Vedic chart type
// used by VedicChartDisplayConfig
enum class VedicChartType
{
	None = -1,
	South = 0,
	North,
	East
};

// Vedic chart north indian options
// used by VedicChartDisplayConfig
enum class NorthIndianChartType
{
	// json:enum
	None = -1,
	Ascendant = 0,
	Number,
	Short,
	Symbol
};

// Vedic chart center
// used by VedicChartDisplayConfig
enum class VedicChartCenterType
{
	// json:enum
	None = -1,
	Nothing = 0,
	VargaName,
	Signification,
	Division,
	DivisionSignification
};

/**
* Enumeration of 28 Nakshatras incl. Abhijit
*/
enum class Nakshatra28
{
	None = -1,
	Aswini = 0, Bharani, Krittika, Rohini, Mrigasira, Ardra, Punarvasu, Pushyami, Aslesha,
	Magha, Poorvaphalguni, Uttaraphalguni, Hasta, Chitra, Swati, Visakha, Anuradha, Jyeshta,
	Moola, Poorvashadha, Uttarashadha, Abhijit, Sravana, Dhanista, Satabhisha, Poorvabhadra, Uttarabhadra, Revati,
	First = Aswini,
	Last = Revati
};

/**
* Enumeration of 27 Nakshatras (without Abhijit)
*/
enum class Nakshatra27
{
	None = -1,
	Aswini = 0, Bharani, Krittika, Rohini, Mrigasira, Ardra, Punarvasu, Pushyami, Aslesha,
	Magha, Poorvaphalguni, Uttaraphalguni, Hasta, Chitra, Swati, Visakha, Anuradha, Jyeshta,
	Moola, Poorvashadha, Uttarashadha, Sravana, Dhanista, Satabhisha, Poorvabhadra, Uttarabhadra, Revati,
	First = Aswini,
	Last = Revati
};

constexpr int ABHIJIT_INDEX = 21;

constexpr double NAKSHATRA_LEN = 13.33333333333;
constexpr double NAKSHATRA_LEN_R = 13.33333333333; // 360 / 27
constexpr double NAKSHATRA_LEN_N = 12.85714286;    // 360/28
constexpr double N27_N28_RATIO = .964285714;       // 27/28
constexpr double N28_N27_RATIO = 1.037037037;      // 28/27
constexpr double PADA_LEN = 3.333333333333;

constexpr double USHADHA_BEGIN_R = 266.6666666667;
constexpr double ABHIJIT_BEGIN_R = 276.6666666667;
constexpr double SRAVANA_BEGIN_R = 280.8888888889;
constexpr double DHANISHTA_BEGIN_R = 293.6666666667;

constexpr double USHADHA_LEN_R = 10.0;
constexpr double ABHIJIT_LEN_R = 4.2222222222; // = 4:13:20;
constexpr double SRAVANA_LEN_R = 12.444444444; // = 12:26:40

/**
* Enumeration for supported Dasas
*/
enum class DasaId
{
	// json:enum
	None = -1,
	Vimsottari = 0,
	Yogini,
	Kalachakra,
	Ashtottari,
	Shodshottari,
	Dvadashottari,
	Panchottari,
	Shatabdika,
	ChaturashitiSama,
	DvisaptatiSama,
	ShatTrimsatSama,
	LagnaVimsottari,
	First = Vimsottari,
	Last = LagnaVimsottari
};

enum class KalachakraGroup
{
	// json:enum
  None = -1,
  Savya,
  Apsavya,
  First = Savya,
  Last = Apsavya
};

enum class KalachakraSubgroup
{
	// json:enum
  None = -1,
  Aswini,
  Bharani,
  Rohini,
  Mrigasira,
  First = Aswini,
  Last = Mrigasira
};

enum class KalachakraGati
{
	// json:enum
  None = -1,
  Default,
  Mandooka,
  Markati,
  Simhavalokan,
  First = Default,
  Last = Simhavalokan
};

enum class DasaReportType
{
	// json:enum
	None = -1,
	Short,
	Compact,
	Comprehensive,
	First = Short,
	Last = Comprehensive
};

// Kala Velas: the lords are either connected to a weekday or the 8th portion
enum class KalavelaLord
{
	// json:enum
	None = -1,
	Sun = 0,
	Moon,
	Mars,
	Mercury,
	Jupiter,
	Venus,
	Saturn,
	Eight
};

// Kalavela longitude is either at the beginning, middle or end of the portion
enum class KalavelaPortion
{
	// json:enum
	None = -1,
	Beginning = 0,
	Middle,
	End
};

/**************************************************************
***
**  View IDs
***
***************************************************************/
enum class ViewId
{
	// json:enum
  None = -1,
  Text = 0,
	PlanetList,
	UniversalChart,
	WesternChart,
	UranianChart,
	VedicChart,
	SarvatobhadraChart,
	GraphicGrid,
	Direction,
	Solar,
	Yoga,
	Varga,
	MultiDirection,
	Ashtakavarga,
	DasaTree,
	DasaTreeComposite,
	DasaChart,
	UniversalRadix,
	Uranian,
	SensitivePoints,
	PlanetaryPictures,
	EventFinder,
	Vimsopakabala,
	Hora,
	FileAndDirectoryReport,
	License,
	Eclipse,
	Ephemeris,
	Partner,
	Multiple,
	Shadbala,
	YogaEditor,
	ApplicationWindow,
	LogWindow,
	Empty,

  First = Text,
  Last = Empty
};


enum class ConfigResource
{
	None = -1,
	AtlasDB,   // Default SQLite database for atlas (can be changed)
	AtlasSQL,  // Default SQL data for atlas (can be changed)
	Dasa,      // text for Dasa tree window
	Geom,      // chart geometry
	Pic,       // pictures and icons
	Sheet,     // sheet config for PDF printout
	Skin,      // chart skins/styles
	Table,     // table styles
	Uranian,   // Uranian text files
	Yoga,      // texts for Yoga view
	I18n,      // only used on Windows

  First = AtlasDB,
  Last = I18n
};

enum class ConfigResourceStoreType
{
	None = -1,
	Global,             // resources are stored under resourcedir and cannot be changed by the user (e.g. /usr/share/saravali9/dasa)
	LocalMultipleFiles, // Resource files are copied to local directory and can be changed (e.g. $HOME/.saravali9/yoga)
	LocalSingleFile     // Single resource file <resource>.json in local directory (e.g. $HOME/.saravali9/table/table.json)
};


struct KeyboardModfier
{
	inline const static int KbNone  = 0x00;
	inline const static int KbShift = 0x01;
	inline const static int KbCtrl  = 0x02;
	inline const static int KbAlt   = 0x04;
};

/*************************************************//**
*
* various strings constants
*
******************************************************/
#define LINE_BREAK "\n";
#define SYMBOL_CODE_ERROR 'Z'
#define CSV_DELIMITER ';'
#define APPLICATION_NAME "saravali9"

#ifdef __WXOSX__
#define MAC_PDF_START_COMMAND "open -a Preview"
#endif

// Atlas
constexpr int  ATLAS_MAX_GRID_ELEMENTS = 50;

#define SARAVALI_WEBSITE_URL "https://saravali.github.io/"
#define APP_NAME "Saravali 9"
#define VENDOR_NAME "Saravali Project"
#define GUI_APP_NAME "Saravali"
#define SYMBOL_FONT_NAME "Saravali"

// standard path for swiss ephem data files on Debian systems
#define DEBIAN_SWE_STANDARD_DATA_PATH "/usr/share/libswe/ephe"
#define SWE_STANDARD_DATA_PATH "swe"

#endif // _CONSTANTS_H_

