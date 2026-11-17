/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Viewpoint.cpp
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

#include "Viewpoint.h"

#include "astroconfig.h"
#include "Commands.h"
#include "enumserializer.h"
#include "Lang.h"
#include "logger.h"
#include "constants.h"

extern Astroconfig *astroconfig;

/***************************************************
**
**   PlanetConfig   ---   Constructor
**
******************************************************/
PlanetConfig::PlanetConfig( const Viewpoint viewpoint )
	: viewpoint( viewpoint )
{
	  switch( viewpoint )
  {
    case Viewpoint::Western:
      textFormat = TextFormat::Symbol;
    break;

    case Viewpoint::Vedic:
      nodeNames = 2; // Rahu+Ketu
    break;

    case Viewpoint::Uranian:
      textFormat = TextFormat::Medium;
    break;

    case Viewpoint::Astronomical:
    default:
    break;
  }

	ViewpointHelper *helper = ViewpointHelper::get();
	Planets defaultItems = helper->getDefaultPlanets( viewpoint );
	for( const auto &p : helper->getAvailablePlanets( viewpoint ))
	{
		const bool found = std::find( defaultItems.begin(), defaultItems.end(), p ) != defaultItems.end();;
		debuglog << "PlanetConfig item " << p << " found in defaults " << std::boolalpha << found;
		items.push_back( std::make_pair( p, found ));
	}
}

/*****************************************************
**
**   PlanetPropertyConfig   ---   Constructor
**
******************************************************/
PlanetPropertyConfig::PlanetPropertyConfig( const Viewpoint viewpoint )
{
	const auto defaultColumnGroups = ViewpointHelper::get()->getDefaultColumnGroups( viewpoint );
	for( const auto &group : ViewpointHelper::get()->getAvailableColumnGroups( viewpoint ))
	{
		const bool found = std::find( defaultColumnGroups.begin(), defaultColumnGroups.end(), group ) != defaultColumnGroups.end();
		columnGroups.push_back( std::make_pair( group, found ));
	}

	switch( viewpoint )
	{
		case Viewpoint::Western:
			textFormat = TextFormat::Symbol;
		break;

		case Viewpoint::Uranian:
			textFormat = TextFormat::Medium;
		break;

		case Viewpoint::Astronomical:
			precision = DegPrecision::More;
		break;
		
		case Viewpoint::Vedic:
		default:
		break;
	}
}

/***************************************************
**
**   CalculationConfig   ---   Constructor
**
******************************************************/
CalculationConfig::CalculationConfig( const Viewpoint viewpoint )
	: viewpoint( viewpoint )
{
	switch( viewpoint )
	{
		case Viewpoint::Western:
			houseSystem = HouseSystem::Placidus;
			lunarNodeMode = LunarNodeMode::True;
		break;

		case Viewpoint::Vedic:
			ayanamsa = 3; // Lahiri
			houseSystem = HouseSystem::Placidus;
			lunarNodeMode = LunarNodeMode::Mean;
		break;

		case Viewpoint::Uranian:
			houseSystem = HouseSystem::MeridianAxialRotation;
			lunarNodeMode = LunarNodeMode::Mean;
		break;

		case Viewpoint::Astronomical:
			houseSystem = HouseSystem::Placidus;
			lunarNodeMode = LunarNodeMode::True;
		break;
		default:
			//assert( false );
		break;
	}
}

/***************************************************
**
**   BasedataConfig   ---   Constructor
**
******************************************************/
BasedataConfig::BasedataConfig( const Viewpoint viewpoint)
	: viewpoint( viewpoint )
{
	ViewpointHelper *helper = ViewpointHelper::get();
	const std::list<BasedataAttribute> defaultList = helper->getDefaultBasedataConfigs( viewpoint );

	for( const auto &a : helper->getAvailableBasedataConfigs( viewpoint ))
	{
		const bool found = std::find( defaultList.begin(), defaultList.end(), a ) != defaultList.end();
		basedataItems.push_back( std::make_pair( a, found ));
	}
}

/***************************************************
**
**   ViewpointConfig   ---   Constructor
**
******************************************************/
ViewpointConfig::ViewpointConfig( const Viewpoint viewpoint )
	: viewpoint( viewpoint ),
	planets( viewpoint),
	planetProps( viewpoint),
	calculation( viewpoint ),
	basedata( viewpoint )

{
	debuglog << "Constructor ViewpointConfig viewpoint " << viewpoint;
}

/**************************************************************
***
**   ViewpointHelper   ---   getViewpointName
***
***************************************************************/
wxString ViewpointHelper::getViewpointName( const Viewpoint &v ) const
{
	switch( v )
	{
		case Viewpoint::None:
			return "None";
		break;
		case Viewpoint::Vedic:
			return "Vedic";
		break;
		case Viewpoint::Western:
			return "Western";
		break;
		case Viewpoint::Uranian:
			return "Uranian";
		break;
		case Viewpoint::Astronomical:
			return "Astronomical";
		break;
		default:
			assert( false );
		break;
	}
	return "<error>";
}

/*****************************************************
**
**   ViewpointHelper   ---   Constructor
**
******************************************************/
ViewpointHelper::ViewpointHelper()
	: commandToColumnGroup(
	{
		{ CommandId::ColToggleLatitude, ColumnGroup::Latitude },
		{ CommandId::ColToggleDeclination, ColumnGroup::Declination },
		{ CommandId::ColToggleSpeed, ColumnGroup::Speed },
		{ CommandId::ColToggleT4Longitude, ColumnGroup::Longitude90 },
		{ CommandId::ColToggleT8Longitude, ColumnGroup::Longitude45 },
		{ CommandId::ColToggleT16Longitude, ColumnGroup::Longitude225 },
		{ CommandId::ColToggleAntiscia, ColumnGroup::Antiscia },
		{ CommandId::ColToggleDiginity, ColumnGroup::Dignity },
		{ CommandId::ColToggleNavamsa, ColumnGroup::Navamsa },
		{ CommandId::ColToggleLord, ColumnGroup::SignLord },
		{ CommandId::ColToggleNakshatra, ColumnGroup::Nakshatra },
		{ CommandId::ColTogglePada, ColumnGroup::NakshatraPada },
		{ CommandId::ColToggleKp, ColumnGroup::KpLords },
		{ CommandId::ColToggleShastiamsa, ColumnGroup::ShastiamsaLord },
		{ CommandId::ColToggleDasavarga, ColumnGroup::DasaVarga },
		{ CommandId::ColToggleKaraka, ColumnGroup::CharaKaraka },
		{ CommandId::ColToggleAv, ColumnGroup::RekhaPoints },
		{ CommandId::ColToggleHousePos, ColumnGroup::HousePosition },
		{ CommandId::ColToggleElement, ColumnGroup::Element },
		{ CommandId::ColToggleSignification, ColumnGroup::Signification },
		{ CommandId::ColToggleSignQuality, ColumnGroup::SignQuality },
		{ CommandId::ColToggleWesternAspectarium, ColumnGroup::WesternAspectarium }
 	}
	)
{
}

/*****************************************************
**
**   ViewpointHelper   ---   getStickyVp (static)
**
******************************************************/
Viewpoint ViewpointHelper::getStickyVp( const WidgetCategory &cat )
{
	debuglog << "ViewpointHelper::getStickyVp cat " << cat;
	switch( cat )
	{
		using enum WidgetCategory;

		case TextPlanetListVedic:
		case VargaChart:
		case SarvatobhadraChart:
		case Varga1Grid:
		case Varga2Grid:
		case Varga3Grid:

		case BardiagramGeneric:
		case VimsopakabalaDiagram:
		case BalaOverviewDiagram:
		case SthanabalaDiagram:
		case SaptabalaDiagram:
		case KalabalaDiagram:
		case BalaSummaryDiagram:
		case AvPindaDiagram:
		case AvRasiDiagram:

		case DasaChart:
		case DasaTree:
		case DasaTreeComposite:

		case TextDasaTree:
		case TextAshtakoota:
		case TextAshtakavarga:
		case TextDasaShort:
		case TextDasaCompact:
		case TextDasaComprehensive:
		case TextJaimini:
		case TextKp:
		case TextNakshatra:
		case TextShadbala:
		case TextVarga:
		case TextVedicAspectarium:
		case TextHora:
		case TextYoga:
		case VedicTriple:
			return Viewpoint::Vedic;
		break;

		case WesternChart:
		case TextPlanetListWestern:
		case TextWesternAspectarium:
		case TextWesternAspectList:
			return Viewpoint::Western;
		break;

		case TextPlanetListUranian:
		case UranianChart:
		case PpTree:
		case TextUranianMatches:
		case TextSensitivePoints:
		case EventFinderGraphical:
			return Viewpoint::Uranian;
		break;

		case TextPlanetListAstronomical:
			return Viewpoint::Astronomical;
		break;

		default:
			return Viewpoint::None;
		break;
	}
}

/*****************************************************
**
**   ViewpointHelper   ---   getViewpointByStickyVp (static)
**
******************************************************/
Viewpoint ViewpointHelper::getViewpointByStickyVp( const Viewpoint &preferredVp, const Viewpoint &stickyVp )
{
	//debuglog << "ViewpointHelper::getViewpointByStickyVp preferredViewpoint " << preferredVp
		//<< " stickyVp " << stickyVp << " change policy " << astroconfig->general.viewpointChangePolicy;
	switch( astroconfig->general.viewpointChangePolicy )
	{
		case ViewpointChangePolicy::Fixed:
    case ViewpointChangePolicy::Medium:
			switch( stickyVp )
			{
				case Viewpoint::Vedic:
				case Viewpoint::Western:
				case Viewpoint::Uranian:
				case Viewpoint::Astronomical:
					return stickyVp;
				break;

				case Viewpoint::None:
					// this is the only difference between Fixed and Medium
					return astroconfig->general.viewpointChangePolicy == ViewpointChangePolicy::Fixed
						?  astroconfig->general.defaultViewpoint
						: preferredVp;
				break;

				default:
					fatallog << "unsupported sticky viewpoint " << stickyVp;
					assert( false );
				break;
			}
    break;
    case ViewpointChangePolicy::Flexible:
			return preferredVp;
    break;
    default:
      fatallog << "wrong viewpoint change policy " << (int)astroconfig->general.viewpointChangePolicy;
      assert( false );
    break;
  }
	return preferredVp;
}

/*****************************************************
**
**   ViewpointHelper   ---   getViewpointByWidgetCategory (static)
**
******************************************************/
Viewpoint ViewpointHelper::getViewpointByWidgetCategory( const Viewpoint &preferredVp, const WidgetCategory &cat )
{
	return getViewpointByStickyVp( preferredVp, getStickyVp( cat ));
}

/*****************************************************
**
**   ViewpointHelper   ---   getStickyViewVp (static)
**
******************************************************/
Viewpoint ViewpointHelper::getStickyViewVp( const ViewId &viewId )
{
	switch( viewId )
	{
		case ViewId::Yoga:
		case ViewId::Varga:
		case ViewId::Ashtakavarga:
		case ViewId::DasaTree:
		case ViewId::DasaTreeComposite:
		case ViewId::DasaChart:
		case ViewId::VedicChart:
		case ViewId::SarvatobhadraChart:
		case ViewId::Hora:
		case ViewId::Shadbala:
		case ViewId::YogaEditor:
			return Viewpoint::Vedic;
		break;

		case ViewId::Uranian:
		case ViewId::SensitivePoints:
		case ViewId::EventFinder:
		case ViewId::UranianChart:
			return Viewpoint::Uranian;
		break;

		case ViewId::WesternChart:
			return Viewpoint::Western;
		break;

		default:
			return Viewpoint::None;
		break;
	}
}

/*****************************************************
**
**   ViewpointHelper   ---   getDefaultPlanets
**
******************************************************/
Planets ViewpointHelper::getDefaultPlanets( const Viewpoint &viewpoint ) const
{
	Planets l;

	switch( viewpoint )
	{
		case Viewpoint::Western:
			l =
			{
				PlanetId::Inner,
				PlanetId::Outer,
				PlanetId::AscNode,
				PlanetId::Ascendant,
				PlanetId::Meridian
			};
		break;

		case Viewpoint::Vedic:
			l =
			{
				PlanetId::Sun,
				PlanetId::Moon,
				PlanetId::Mars,
				PlanetId::Mercury,
				PlanetId::Jupiter,
				PlanetId::Venus,
				PlanetId::Saturn,
				PlanetId::AscNode,
				PlanetId::DescNode,
				PlanetId::Ascendant
			};
		break;

		case Viewpoint::Uranian:
			l =
			{
				PlanetId::Meridian,
				PlanetId::Capricorn,
				PlanetId::Sun,
				PlanetId::Ascendant,
				PlanetId::Moon,
				PlanetId::AscNode,
				PlanetId::Mercury,
				PlanetId::Venus,
				PlanetId::Mars,
				PlanetId::Jupiter,
				PlanetId::Saturn,
				PlanetId::Outer,
				PlanetId::InnerTNP,
				PlanetId::OuterTNP,
			};
		break;

		case Viewpoint::Astronomical:
			l =
			{
				PlanetId::Inner,
				PlanetId::Saturn,
				PlanetId::Outer,
				PlanetId::AscNode,
				PlanetId::Ascendant,
				PlanetId::Meridian
			};
		break;
		default:
			//assert( false );
		break;
	}
	//return l;
	return PlanetHelper::expandPlanets( l );;
}

/*****************************************************
**
**   ViewpointHelper   ---   getAvailablePlanets
**
******************************************************/
Planets ViewpointHelper::getAvailablePlanets( const Viewpoint &viewpoint ) const
{
	debuglog << "ViewpointHelper::getAvailablePlanets viewpoint " << viewpoint;
	Planets l;
	switch( viewpoint )
	{
		case Viewpoint::Vedic:
			l = 
			{
				PlanetId::Sun,
				PlanetId::Moon,
				PlanetId::Mars,
				PlanetId::Mercury,
				PlanetId::Jupiter,
				PlanetId::Venus,
				PlanetId::Saturn,

				PlanetId::Outer,

				PlanetId::AscNode,
				PlanetId::DescNode,
				PlanetId::Ascendant,
				PlanetId::Meridian,
				PlanetId::Descendant,
				PlanetId::ImumCoeli,

				PlanetId::AllTNP,

				PlanetId::Chiron,
				PlanetId::Planetoids,
				PlanetId::Pholus,
				PlanetId::Lilith,

				PlanetId::Upagrahas,
				PlanetId::Kalavelas,
				PlanetId::SpecialLagnas,

				PlanetId::D9Lagna,
				PlanetId::AllHouses,
				PlanetId::FourHouses,
				PlanetId::SixHouses
			};
		break;

		case Viewpoint::Uranian:
			l = 
			{
				PlanetId::Meridian,
				PlanetId::Capricorn,
				PlanetId::Sun,
				PlanetId::Ascendant,
				PlanetId::Moon,
				PlanetId::AscNode,
				PlanetId::Mercury,
				PlanetId::Venus,
				PlanetId::Mars,
				PlanetId::Jupiter,
				PlanetId::Saturn,
				PlanetId::Outer,

				PlanetId::DescNode,
				PlanetId::Descendant,
				PlanetId::ImumCoeli,

				PlanetId::AllTNP,

				PlanetId::Chiron,
				PlanetId::Planetoids,
				PlanetId::Pholus,
				PlanetId::Lilith,

				PlanetId::Aries,
				PlanetId::AllHouses,
				PlanetId::FourHouses,
				PlanetId::SixHouses
			};
		break;

		case Viewpoint::Western:
		case Viewpoint::Astronomical:
		default:
			l = 
			{
				PlanetId::Inner,
				PlanetId::Outer,

				PlanetId::AscNode,
				PlanetId::DescNode,
				PlanetId::Ascendant,
				PlanetId::Meridian,
				PlanetId::Descendant,
				PlanetId::ImumCoeli,

				PlanetId::AllTNP,

				PlanetId::Chiron,
				PlanetId::Planetoids,
				PlanetId::Pholus,
				PlanetId::Lilith,

				PlanetId::Aries,
				PlanetId::Capricorn,
				PlanetId::AllHouses,
				PlanetId::FourHouses,
				PlanetId::SixHouses
			};
		break;
	}
	return PlanetHelper::expandPlanets( l );;
}

/*****************************************************
**
**   ViewpointHelper   ---   getColumnGroupName
**
******************************************************/
wxString ViewpointHelper::getColumnGroupName( const ColumnGroup &g, const TextFormat &format ) const
{
	assert_valid_enum<ColumnGroup>( g );
	const static std::pair<wxString, wxString> k_group_name[(int)ColumnGroup::Last + 1] =
	{
		{ tr( "Object Name" ), tr( "Name" ) },
		{ tr( "Longitude" ), "" },
		{ tr( "D-4 Longitude" ), tr( "D-4" ) },
		{ tr( "D-8 Longitude" ), tr( "D-8" ) },
		{ tr( "D-16 Longitude" ), tr( "D-16" ) },
		{ tr( "Antiscium" ), "" } ,
		{ tr( "Latitude" ), "" },
		{ tr( "Declination" ), "" },
		{ tr( "Speed" ), "Speed" },
		{ tr( "Sign Lord" ), tr( "Lord" )} ,
		{ tr( "Element" ), "" },
		{ tr( "Sign Quality" ), tr( "Quality" ) },
		{ tr( "Basic Signification" ), tr( "Signification" ) },
		{ tr( "House Position" ), tr( "House" ) },
		{ tr( "Navamsa" ), tr( "D-9" ) },
		{ tr( "Nakshatra" ), "" },
		{ tr( "Nakshatra Pada" ), tr( "Pada" ) },
		{ tr( "KP Lords" ), "" },
		{ tr( "Chara Karaka" ), tr( "Karaka" ) },
		{ tr( "Dignity" ), "" },
		{ tr( "Shastiamsa Lord" ), tr( "D-60 Lord" ) },
		{ tr( "Dasa Varga" ), tr( "D-10" ) },
		{ tr( "AV Rekha Points" ), tr( "AV" ) },
		{ tr( "Western Aspectarium" ), "" },
		{ tr( "Custom Key-Value" ), "" },
		{ tr( "Error" ), "" },
		{ tr( "Empty" ), ""}
	};
	if ( format == TextFormat::Long || k_group_name[(int)g].second.empty() )
	{
		return k_group_name[(int)g].first;
	}
	else return k_group_name[(int)g].second;
}

/*****************************************************
**
**   ViewpointHelper   ---   getColumnGroup
**
******************************************************/
ColumnGroup ViewpointHelper::getColumnGroup( const CommandId &command ) const
{
	for( auto a : commandToColumnGroup )
	{
		if ( a.first == command ) return a.second;
	}
	return ColumnGroup::None;
}

/*****************************************************
**
**   ViewpointHelper   ---   columnGroupToCommandId
**
******************************************************/
CommandId ViewpointHelper::columnGroupToCommandId( const ColumnGroup &group ) const
{
	for( auto a : commandToColumnGroup )
	{
		if ( a.second == group ) return a.first;
	}
	return CommandId::None;
}

/*****************************************************
**
**   ViewpointHelper   ---   getDefaultColumnGroups
**
******************************************************/
std::list<ColumnGroup> ViewpointHelper::getDefaultColumnGroups( const Viewpoint &viewpoint ) const
{
	switch( viewpoint )
	{
		case Viewpoint::Western:
			return
			{
				ColumnGroup::ObjectName,
				ColumnGroup::Longitude,
				ColumnGroup::Element,
				ColumnGroup::SignQuality,
				ColumnGroup::HousePosition
			};
		break;

		case Viewpoint::Vedic:
			return
			{
				ColumnGroup::ObjectName,
				ColumnGroup::Longitude,
				ColumnGroup::SignLord,
				ColumnGroup::Navamsa,
				ColumnGroup::Nakshatra
			};
		break;

		case Viewpoint::Uranian:
			return
			{
				ColumnGroup::ObjectName,
				ColumnGroup::Longitude,
				ColumnGroup::Longitude90,
				ColumnGroup::Antiscia
			};
		break;

		case Viewpoint::Astronomical:
			return
			{
				ColumnGroup::ObjectName,
				ColumnGroup::Longitude,
				ColumnGroup::Declination,
				ColumnGroup::Speed
			};
		break;
		default:
		break;
	}
	return {};
}

/*****************************************************
**
**   ViewpointHelper   ---   getAvailableColumnGroups
**
******************************************************/
std::list<ColumnGroup> ViewpointHelper::getAvailableColumnGroups( const Viewpoint &viewpoint ) const
{
	switch( viewpoint )
	{
		case Viewpoint::Western:
			return
			{
				ColumnGroup::ObjectName,
				ColumnGroup::Longitude,
				ColumnGroup::Longitude90,
				ColumnGroup::Longitude45,
				ColumnGroup::Longitude225,
				ColumnGroup::Antiscia,
				ColumnGroup::Latitude,
				ColumnGroup::Declination,
				ColumnGroup::Speed,
				ColumnGroup::SignLord,
				ColumnGroup::Element,
				ColumnGroup::SignQuality,
				ColumnGroup::HousePosition,
				ColumnGroup::WesternAspectarium
			};
		break;

		case Viewpoint::Uranian:
			return
			{
				ColumnGroup::ObjectName,
				ColumnGroup::Longitude,
				ColumnGroup::Longitude90,
				ColumnGroup::Longitude45,
				ColumnGroup::Longitude225,
				ColumnGroup::Antiscia,
				ColumnGroup::Latitude,
				ColumnGroup::Declination,
				ColumnGroup::Speed,
				ColumnGroup::SignLord,
				ColumnGroup::Element,
				ColumnGroup::SignQuality,
				ColumnGroup::HousePosition
			};
		break;

		case Viewpoint::Astronomical:
			return
			{
				ColumnGroup::ObjectName,
				ColumnGroup::Longitude,
				ColumnGroup::Antiscia,
				ColumnGroup::Latitude,
				ColumnGroup::Declination,
				ColumnGroup::Speed,
				ColumnGroup::HousePosition
			};
		break;

		case Viewpoint::Vedic:
			return
			{
				ColumnGroup::ObjectName,
				ColumnGroup::Longitude,
				ColumnGroup::Speed,
				ColumnGroup::SignLord,
				ColumnGroup::Element,
				ColumnGroup::SignQuality,
				ColumnGroup::Signification,
				ColumnGroup::HousePosition,
				ColumnGroup::Navamsa,
				ColumnGroup::Nakshatra,
				ColumnGroup::NakshatraPada,
				ColumnGroup::KpLords,
				ColumnGroup::CharaKaraka,
				ColumnGroup::Dignity,
				ColumnGroup::ShastiamsaLord,
				ColumnGroup::DasaVarga,
				ColumnGroup::RekhaPoints
			};
		break;

		default:
			//assert( false );
		break;
	}
	return {};
}

/**************************************************************
***
**   ViewpointHelper   ---   getAvailableBasedataConfigs
***
***************************************************************/
std::list<BasedataAttribute> ViewpointHelper::getAvailableBasedataConfigs( const Viewpoint &viewpoint )
{
	std::list<BasedataAttribute> l;
	for( const auto &a : Enum<BasedataAttribute>( BasedataAttribute::FirstGeneral, BasedataAttribute::LastGeneral ))
	{
		l.push_back( a );
	}
	if ( viewpoint == Viewpoint::Vedic )
	{
		for( const auto &a : Enum<BasedataAttribute>( BasedataAttribute::FirstVedic, BasedataAttribute::LastVedic ))
		{
			l.push_back( a );
		}
	}
	for( const auto &a : Enum<BasedataAttribute>( BasedataAttribute::FirstWestern, BasedataAttribute::LastWestern ))
	{
		l.push_back( a );
	}
	if ( viewpoint != Viewpoint::Vedic )
	{
		for( const auto &a : Enum<BasedataAttribute>( BasedataAttribute::FirstVedic, BasedataAttribute::LastVedic ))
		{
			l.push_back( a );
		}
	}
	return l;
}

/**************************************************************
***
**   ViewpointHelper   ---   getDefaultBasedataConfigs
***
***************************************************************/
std::list<BasedataAttribute> ViewpointHelper::getDefaultBasedataConfigs( const Viewpoint &viewpoint )
{
	std::list<BasedataAttribute> l = 
	{
		BasedataAttribute::LocalDate,
		BasedataAttribute::LocalTime,
		BasedataAttribute::UniversalTime,
		BasedataAttribute::Timezone,
		BasedataAttribute::DaylightSaving, 
		BasedataAttribute::LocationName,
		BasedataAttribute::LocationLongitude,
		BasedataAttribute::LocationLatitude
	};
	switch( viewpoint )
	{
		case Viewpoint::Vedic:
			l.push_back( BasedataAttribute::AyanamsaName );
			l.push_back( BasedataAttribute::AyanamsaValue );
			l.push_back( BasedataAttribute::Varna );
			l.push_back( BasedataAttribute::Yoni );
			l.push_back( BasedataAttribute::MoonNakshatra );
			l.push_back( BasedataAttribute::MoonVimsottariInfluenceTime );
		break;
		case Viewpoint::Western:
			l.push_back( BasedataAttribute::PlanetElements );
		break;
		case Viewpoint::Astronomical:
			l.push_back( BasedataAttribute::JulianDate );
			l.push_back( BasedataAttribute::SiderealTime );
			l.push_back( BasedataAttribute::Eps );
		break;
		case Viewpoint::Uranian:
		default:
		break;
	}

	return l;
}

/**************************************************************
***
**   ViewpointHelper   ---   expandBasedataConfigList
***
***************************************************************/
std::list<BasedataAttribute> ViewpointHelper::expandBasedataConfigList( const std::list<BasedataAttribute> &l )
{
	std::list<BasedataAttribute> r;
	for( const auto &a : l )
	{
		switch( a )
		{
			case BasedataAttribute::PlanetElements:
				r.push_back( BasedataAttribute::ElementFire );
				r.push_back( BasedataAttribute::ElementEarth );
				r.push_back( BasedataAttribute::ElementAir );
				r.push_back( BasedataAttribute::ElementWater );
			break;
			case BasedataAttribute::PlanetQuadrants:
				r.push_back( BasedataAttribute::ObjectsNorth );
				r.push_back( BasedataAttribute::ObjectsWest );
				r.push_back( BasedataAttribute::ObjectsSouth );
				r.push_back( BasedataAttribute::ObjectsEast );
			break;
			case BasedataAttribute::PlanetSignQualities:
				r.push_back( BasedataAttribute::SignQualityMovable );
				r.push_back( BasedataAttribute::SignQualityFixed );
				r.push_back( BasedataAttribute::SignQualityDual );
			break;

			default:
				r.push_back( a );
			break;
		}
	}
	return r;
}

