/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Lang.cpp
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

#include "Lang.h"

#include "ArabicPartBase.h"
#include "astroconfig.h"
#include "Calculator.h"
#include "enumserializer.h"
#include "logger.h"
#include "mathbase.h"

#include <format>

extern Astroconfig *astroconfig;

//#define DEBUG_LANG

/**************************************************************
***
**   Lang   ---   getWidgetCategoryName
***
***************************************************************/
wxString Lang::getWidgetCategoryName( const WidgetCategory &cat, const TextFormat format ) const
{
	switch( cat )
	{
		using enum WidgetCategory;

		case UniversalChart:
			return tr( "Universal chart" );
		break;
		case WesternChart:
			return tr( "Western chart" );
		break;
		case UranianChart:
		case UranianChartWidget:
			return tr( "Uranian chart" );
		break;
		case SarvatobhadraChart:
			switch( format )
			{
				case TextFormat::Long:
					return tr( "Sarvatobhadra chart" );
				break;
				default:
					return tr( "Sbc" );
				break;
			}
		break;
		case VargaChart:
			return tr( "Varga chart" );
		break;
		case RasiNavamsaGrid:
			return tr( "Rasi Navamsa grid" );
		break;
		case Varga1Grid:
			return tr( "Shadvarga" );
		break;
		case Varga2Grid:
			return tr( "Vargas (2)" );
		break;
		case Varga3Grid:
			return tr( "Vargas (3)" );
		break;
		case BardiagramGeneric:
			return tr( "Bardiagram" );
		break;
		case AvPindaDiagram:
			return tr( "Pinda" );
		break;
		case AvRasiDiagram:
			return tr( "Rasi" );
		break;
		case VimsopakabalaDiagram:
			return tr( "Vimsopakabala" );
		break;
		case BalaOverviewDiagram:
			return tr( "Bala overview" );
		break;
		case SthanabalaDiagram:
			return tr( "Sthanabala" );
		break;
		case SaptabalaDiagram:
			return tr( "Saptabala" );
		break;
		case KalabalaDiagram:
			return tr( "Kalabala" );
		break;
		case BalaSummaryDiagram:
			return tr( "Bala Summary" );
		break;
		case PpTree:
		case PpWidget:
			switch( format )
			{
				case TextFormat::Long:
					return tr( "Planetary pictures" );
				break;
				default:
					return tr( "PP" );
				break;
			}
		break;
		case EventFinderGraphical:
			return tr( "Events" );
		break;
		case EphemGraphical:
			return tr( "Graphical Epehemeris" );
		break;
		case DasaChart:
		case DasaChartWidget:
			return tr( "Dasa chart" );
		break;
		case DasaTree:
		case DasaTreeWidget:
			return tr( "Dasa tree" );
		break;
		case DasaTreeComposite:
			return tr( "Dasa tree composite" );
		break;
		case AvWidget:
			return tr( "Ashtakavarga" );
		break;
		case TextGeneric:
			return tr( "Text" );
		break;
		case HoraWidget:
		case TextHora:
			return tr( "Hora" );
		break;
		case TextEclipse:
		case EclipseWidget:
			return tr( "Eclipses" );
		break;
		case TextEphemCoordinates:
			return tr( "Coordinates" );
		break;
		case TextEphemDetails:
			return tr( "Details" );
		break;
		case TextEphemLunar:
			return tr( "Lunar" );
		break;
		case TextEphemIngress:
			return tr( "Ingress" );
		break;
		case TextEphemKp:
			return tr( "Kp" );
		break;
		case EphemWidget:
			return tr( "Ephemeris" );
		break;
		case DirectionWidget:
			return tr( "Directions" );
		break;
		case SolarWidget:
			return tr( "Solar" );
		break;
		case PartnerWidget:
			return tr( "Partner" );
		break;
		case UniversalRadixWidget:
			return tr( "Radix" );
		break;
		case EventFinderWidget:
			return tr( "Event Finder" );
		break;
		case TextUranianMatches:
		case MatchesWidget:
			return tr( "Events" );
		break;
		case SpWidget:
		case TextSensitivePoints:
			switch( format )
			{
				case TextFormat::Long:
					return tr( "Sensitive points" );
				break;
				default:
					return tr( "SP" );
				break;
			}
		break;
		case ComprehensiveTextWidget:
			switch( format )
			{
				case TextFormat::Long:
					return tr( "Text widget (comprehensive)" );
				break;
				default:
					return tr( "Text" );
				break;
			}
			return tr( "Dasa tree" );
		case TextDasaTree:
			return tr( "Dasa tree" );
		break;
		case TextAshtakoota:
			return tr( "Ashtakoota" );
		break;
		case YogaWidget:
			return tr( "Yogas" );
		break;
		case TextYoga:
			return tr( "Yoga" );
		break;
		case TextSynastry:
			return tr( "Synastry" );
		break;
		case TextBaseData:
			return tr( "Base data" );
		break;
		case TextArabicParts:
			return tr( "Arabic parts" );
		break;
		case TextAshtakavarga:
			return tr( "Ashtakavarga" );
		break;
		case TextPlanetBalance:
			return tr( "Planet balance" );
		break;
		case TextVedicHouses:
			return tr( "Vedic Houses" );
		break;
		case TextUranianHouses:
			return tr( "Uranian Houses" );
		break;
		case TextDasaShort:
			return tr( "Dasa (short)" );
		break;
		case TextDasaCompact:
			return tr( "Dasa (compact)" );
		break;
		case TextDasaComprehensive:
			return tr( "Dasa (comprehensive)" );
		break;
		case TextJaimini:
			return tr( "Jaimini" );
		break;
		case TextKp:
			return tr( "Kp" );
		break;
		case TextNakshatra:
			return tr( "Nakshatras" );
		break;
		case TextPlanetListVedic:
			return tr( "Vedic" );
		break;
		case TextPlanetListWestern:
			return tr( "Western" );
		break;
		case TextPlanetListUranian:
			return tr( "Uranian" );
		break;
		case TextPlanetListAstronomical:
			return tr( "Astronomical" );
		break;
		case TextPlanetListCombined:
			return tr( "Planets" );
		break;

		case ShadbalaWidget:
		case TextShadbala:
			return tr( "Shadbala" );
		break;
		case TextVarga:
			return tr( "Varga summary" );
		break;
		case TextVedicAspectarium:
			return tr( "Vedic aspectarium" );
		break;
		case TextWesternAspectarium:
			return tr( "Western aspectarium" );
		break;
		case TextWesternAspectList:
			return tr( "Western aspects" );
		break;
		case PlanetListChartPair:
			return tr( "Planets" );
		break;
		case SynastryPair:
			return tr( "Synastry" );
		break;
		case CompositePair:
			return tr( "Composite" );
		break;
		case VedicTriple:
			switch( format )
			{
				case TextFormat::Long:
					return tr( "Vedic widgets: planet list, 2 charts and Dasa" );
				break;
				default:
					return tr( "Vedic triple" );
				break;
			}
		break;
		case EmptyWidget:
			return tr( "Empty" );
		break;
		default:
			return enum_to_string( cat );
		break;
	}
}

/**************************************************************
***
**   Lang   ---   getNakshatraName
***
***************************************************************/
wxString Lang::getNakshatraName( const int &nak, const bool &use28, const TextFormat format ) const
{
	const static wxString k_nakshatra_name_large[28]  = {
		tr( "Aswini" ), tr( "Bharani" ), tr( "Krittika" ), tr( "Rohini" ),
		tr( "Mrigasira" ), tr( "Ardra" ), tr( "Punarvasu" ), tr( "Pushyami" ),
		tr( "Aslesha" ), tr( "Magha" ), tr( "Poorvaphalguni" ),
		tr( "Uttaraphalguni" ), tr( "Hasta" ), tr( "Chitra" ),
		tr( "Swati" ), tr( "Visakha" ), tr( "Anuradha" ), tr( "Jyeshta" ),
		tr( "Moola" ), tr( "Poorvashadha" ), tr( "Uttarashadha" ), tr( "Abhijit" ),
		tr( "Sravana" ), tr( "Dhanista" ), tr( "Satabhisha" ), tr( "Poorvabhadra" ),
		tr( "Uttarabhadra" ), tr( "Revati" )
	};
	const static wxString k_nakshatra_name_medium[28]  = {
		tr( "Aswini" ), tr( "Bharani" ), tr( "Krittika" ), tr( "Rohini" ),
		tr( "Mrigasira" ), tr( "Ardra" ), tr( "Punarvasu" ), tr( "Pushyami" ),
		tr( "Aslesha" ), tr( "Magha" ), tr( "P.Phalguni" ), tr( "U.Phalguni" ),
		tr( "Hasta" ), tr( "Chitra" ), tr( "Swati" ), tr( "Visakha" ),
		tr( "Anuradha" ), tr( "Jyeshta" ), tr( "Moola" ), tr( "P.Shadha" ),
		tr( "U.Shadha" ), tr( "Abhijit" ), tr( "Sravana" ), tr( "Dhanista" ),
		tr( "Satabisha" ), tr( "P.Bhadra" ), tr( "U.Bhadra" ), tr( "Revati" )
	};
	const static wxString k_nakshatra_name_short[28]  = {
		tr( "Asw" ), tr( "Bhr" ), tr( "Kri" ), tr( "Roh" ),
		tr( "Mri" ), tr( "Ard" ), tr( "Pun" ), tr( "Pus" ), tr( "Asl" ), tr( "Mak" ), tr( "PPl" ),
		tr( "UPl" ), tr( "Hst" ), tr( "Cit" ), tr( "Sva" ), tr( "Vis" ), tr( "Anu" ), tr( "Jye" ),
		tr( "Mul" ), tr( "PSa" ), tr( "USa" ), tr( "Abh" ), tr( "Sra" ), tr( "Dha" ), tr( "Sat" ),
		tr( "PBa" ), tr( "UBa" ), tr( "Rev" )
	};
	// shift index after  U. shadha for system of 27 nakshatras
	const int index = ( ! use28 && ( nak > 20 )) ? nak+1 : nak;

	//switch(  dconfig.signTextLength )
	switch(  format )
	{
		case TextFormat::Short:
			return k_nakshatra_name_short[index];
		break;
		case TextFormat::Medium:
			return k_nakshatra_name_medium[index];
		break;
		default:
			return k_nakshatra_name_large[index];
		break;
	}
}

/**************************************************************
***
**   Lang   ---   getNakshatra27Name
***
***************************************************************/
wxString Lang::getNakshatra27Name( const Nakshatra27 &n, const TextFormat format ) const
{
	assert_valid_enum<Nakshatra27>( n );
	return getNakshatraName( (int)n, false, format );
}

/**************************************************************
***
**   Lang   ---   getNakshatra28Name
***
***************************************************************/
wxString Lang::getNakshatra28Name( const Nakshatra28 &n, const TextFormat format ) const
{
	assert_valid_enum<Nakshatra28>( n );
	return getNakshatraName( (int)n, true, format );
}

/**************************************************************
***
**    Lang   ---   getPlanetName
***
***************************************************************/
wxString Lang::getPlanetName( const PlanetId &ob, const Viewpoint &viewpoint, const TextFormat format ) const
{
	PlanetConfig cfg = astroconfig->getViewpointConfig( viewpoint ).planets;
	if ( cfg.textFormat != TextFormat::None )
	{
		cfg.textFormat = format;
	}
	return getPlanetName( ob, cfg );
}

/**************************************************************
***
**    Lang   ---   getPlanetName
***
***************************************************************/
wxString Lang::getPlanetName( const PlanetId &ob, const PlanetConfig &cfg ) const
{
#ifdef DEBUG_LANG
	debuglog << "Lang::getPlanetName ob " << (int)ob << " text format " << cfg.textFormat;
#endif	
	wxString ret;
	switch( ob )
	{
		using enum  PlanetId;

		case None:
			return c_none;
		break;
		case Sun:
		case Moon:
		case Mercury:
		case Venus:
		case Mars:
		case Jupiter:
		case Saturn:
		{
			const static wxString k_inner[7] =
				{ tr("Sun"), tr("Moon"), tr("Mercury"), tr( "Venus" ), tr( "Mars" ), tr("Jupiter" ), tr("Saturn") };
			const static wxString k_inner_vedic[7] =
				{ tr("Surya"), tr("Chandra"), tr("Budha"), tr("Shukra"), tr("Kuja"), tr("Guru"), tr("Shani") };

			const int id = (int)ob - (int)PlanetId::Sun;
			assert( id >= 0 && id < 7 );
			ret = cfg.useVedicNames ?  k_inner_vedic[ id ] : k_inner[ id ];
		}
		break;
		case Uranus:
		case Neptune:
		case Pluto:
		{
			const static wxString k_outer[3] = { tr("Uranus"), tr("Neptune"), tr("Pluto") };
			const int id = (int)ob - (int)PlanetId::Uranus;
			assert( id >= 0 && id < 3 );
			ret = k_outer[ id ];
		}
		break;
		case Outer:
			return tr( "Outer" );
		break;
		case AscNode:
		case TrueAscNode:
		case MeanAscNode:
			switch( cfg.nodeNames )
			{
				case 1: // dragon head
				{
					// custom abbreviations
					switch( cfg.textFormat )
					{
						case TextFormat::Medium:
							return "DrH";
						break;
						case TextFormat::Short:
							return "DH";
						break;
						case TextFormat::Long:
							return tr( "Dragon Head" );
						break;
						default:
						break;
					}
					//ret = tr( "Dragon Head" );
				}
				break;
				case 2: // vedic
					ret = tr( "Rahu" );
				break;
				case 0: // node
				default:
					ret = tr( "Node" );
				break;
			}
		break;
		case DescNode:
		case TrueDescNode:
		case MeanDescNode:
			switch( cfg.nodeNames )
			{
				case 1: // dragon tail
				{
					// custom abbreviations
					switch( cfg.textFormat )
					{
						case TextFormat::Medium:
							return "DrT";
						break;
						case TextFormat::Short:
							return "DT";
						break;
						case TextFormat::Long:
							return tr( "Dragon Tail" );
						break;
						default:
						break;
					}
				}
				break;
				case 2: // vedic
					ret = tr( "Ketu" );
				break;
				case 0: // node
				default:
					switch( cfg.textFormat )
					{
						case TextFormat::Medium:
							return "DNo";
						break;
						case TextFormat::Short:
							return "DN";
						break;
						case TextFormat::Long:
							return tr( "Desc Node" );
						break;
						default:
						break;
					}
				break;
			}
		break;
		case Ascendant:
			ret = cfg.useVedicNames ? tr( "Lagna" ) : tr( "Ascendant" );
		break;
		case Meridian:
			switch( cfg.textFormat )
			{
				case TextFormat::Medium:
				case TextFormat::Short:
					return "MC";
				break;
				case TextFormat::Long:
				default:
					ret = tr( "Meridian" );
				break;
			}
		break;
		case Descendant:
			ret = tr( "Descendant" );
		break;
		case ImumCoeli:
			switch( cfg.textFormat )
			{
				case TextFormat::Medium:
				case TextFormat::Short:
					return "IC";
				break;
				case TextFormat::Long:
				default:
					ret = tr( "Imum Coeli" );
				break;
			}
		break;

		case Cupido:
		case Hades:
		case Zeus:
		case Kronos:
		{
			const static wxString u_inner[4] = { tr( "Cupido" ), tr( "Hades" ), tr( "Zeus" ), tr( "Kronos" ) };
			const int id = (int)ob - (int)PlanetId::Cupido;
			assert( id >= 0 && id < 4 );
			ret = u_inner[ id ];
		}
		break;
		case Apollon:
		case Admetos:
		case Vulkanus:
		case Poseidon:
		{
			const static wxString u_outer[4] = { tr("Apollon" ), tr( "Admetos" ), tr( "Vulkanus" ), tr( "Poseidon" ) };
			const int id = (int)ob - (int)PlanetId::Apollon;
			assert( id >= 0 && id < 4 );
			ret = u_outer[ id ];
		}
		break;
		case Chiron:
			ret = tr( "Chiron" );
		break;
		case Pholus:
			ret = tr( "Pholus" );
		break;
		case Ceres:
		case Pallas:
		case Juno:
		case Vesta:
		{
			const static wxString k_planetoids[4] = { tr( "Ceres" ), tr( "Pallas" ), tr( "Juno" ), tr( "Vesta" ) };
			const int id = (int)ob - (int)PlanetId::Ceres;
			assert( id >= 0 && id < 4 );
			ret = k_planetoids[ id ];
		}
		break;
		case Lilith:
			ret = tr( "Lilith" );
		break;
		case Aries:
			ret = tr( "Aries" );
		break;
		case Cancer:
			ret = tr( "Cancer" );
		break;
		case Libra:
			ret = tr( "Libra" );
		break;
		case Capricorn:
			ret = tr( "Capricorn" );
		break;

		case Dhuma:
		case Vyatipata:
		case Parivesha:
		case Chapa:
		case Upaketu:
		{
			const static wxString k_upa[5] =
				{ tr( "Dhuma" ), tr( "Vyatipata" ), tr( "Parivesha" ), tr( "Chapa" ), tr( "Upaketu" ) };
			const int id = (int)ob - (int)PlanetId::Dhuma;
			assert( id >= 0 && id < 5 );
			ret = k_upa[ id ];
		}
		break;
		case Kala:
		case Mrityu:
		case Ardhaprahara:
		case Yamaghantaka:
		case Gulika:
		case Mandi:
		{
			const static wxString k_kalavelas[6] =
				{ tr( "Kala" ), tr( "Mrityu" ), tr( "Ardhaprahara" ), tr( "Yamaghantaka" ), tr( "Gulika" ), tr( "Mandi" ) };
			const int id = (int)ob - (int)PlanetId::Kala;
			assert( id >= 0 && id < 6 );
			ret = k_kalavelas[ id ];
		}
		break;
		case BhavaLagna:
		case HoraLagna:
		case GhatikaLagna:
		{
			const static wxString k_lagna[3] = { tr( "Bhava Lagna" ), tr( "Hora Lagna" ), tr( "Ghatika Lagna" ) };
			const int id = (int)ob - (int)PlanetId::BhavaLagna;
			assert( id >= 0 && id < 3 );
			ret = k_lagna[ id ];
		}
		break;
		case D9Lagna:
			ret = tr( "D9Lagna" );
		break;
		case Earth:
			ret = tr( "Earth" );
		break;

		case House1:
		case House2:
		case House3:
		case House4:
		case House5:
		case House6:
		case House7:
		case House8:
		case House9:
		case House10:
		case House11:
		case House12:
			if ( cfg.useVedicNames )
				{ ret = getBhavaName( (House)((int)ob - (int)PlanetId::House1 )); }
			else
			{ ret = std::format( "{}", (int)ob - (int)PlanetId::House1 + 1 ); }
		break;

		case InnerTNP:
			ret = tr( "Inner TNP" );
		break;
		case OuterTNP:
			ret = tr( "Outer TNP" );
		break;
		case Planetoids:
			ret = tr( "Planetoids" );
		break;

		case Upagrahas:
			ret = tr( "Upagrahas" );
		break;
		case Kalavelas:
			ret = tr( "Kalavelas" );
		break;
		case SpecialLagnas:
			ret = tr( "Special Lagnas" );
		break;
		case SixHouses:
			ret = tr( "Six Houses" );
		break;
		case FourHouses:
			ret = tr( "Four Houses" );
		break;
		case AllHouses:
			ret = tr( "All Houses" );
		break;
		case ArabicParts:
			ret = tr( "Arabic Parts" );
		break;

		default:
			if ( PlanetHelper::isArabicPart( ob ))
			{
				return ArabicPartsConfig::getPlanetName( ob, cfg.textFormat );
			}
			else
			{
				errorlog << "invalid object id " << (int)ob;
				return std::format( "ERROR: invalid object id {}", (int)ob );
			}
		break;
	}
#ifdef DEBUG_LANG
	debuglog << "Lang::getPlanetName ob " << (int)ob << " ret " << ret;
#endif	
	switch( cfg.textFormat )
	{
		case TextFormat::Medium:
			return ret.substr( 0, 3 );
		break;
		case TextFormat::Short:
			return ret.substr( 0, 2 );
		break;
		case TextFormat::Long:
		default:
			return ret;
		break;
	}
}

/**************************************************************
***
**    Lang   ---   getMovingDirectionName
***
***************************************************************/
wxString Lang::getMovingDirectionName( const MovingDirection &d ) const
{
	switch( d )
	{
		case MovingDirection::None:
			return( " " );
		break;
		case MovingDirection::Direct:
			return "D";
		break;
		case MovingDirection::Retrograde:
			return "R";
		break;
		case MovingDirection::Stationary:
			return "S";
		break;
		default:
			assert( false );
    break;
	}
	return "<error>";
}

/**************************************************************
***
**    Lang   ---   getVargaName
***
***************************************************************/
wxString Lang::getVargaName( const Varga &i ) const
{
	assert_valid_enum<Varga>( i );
	const wxString vargaNames[enum_count<Varga>()] =
	{
		tr( "Rasi" ),
		tr( "Navamsa" ),
		tr( "Hora" ),
		tr( "Drekkana" ),
		tr( "Chaturtamsa" ),
		tr( "Sastamsa" ),
		tr( "Saptamamsa" ),
		tr( "Ashtamsa" ),
		tr( "Dasamsa" ),
		tr( "Dvadasamsa" ),
		tr( "Shodasamsa" ),
		tr( "Vimsamsa" ),
		tr( "Siddhamsa" ),
		tr( "Bhamsa" ),
		tr( "Trimsamsa" ),
		tr( "Chatvarimsamsa" ),
		tr( "Akshavedamsa" ),
		tr( "Shastiamsa" ),
		tr( "Ashtottaramsa" ),
		tr( "Dvadas-Dvadasamsa" ),
		tr( "Bhava" )
	};
	return vargaNames[(int)i];
}

/**************************************************************
***
**    Lang   ---   getDefaultVargaSignification
***
***************************************************************/
wxString Lang::getVargaSignification( const Varga &i ) const
{
	assert_valid_enum<Varga>( i );
	const wxString defaultSignifications[enum_count<Varga>()] =
	{
		tr( "Physique" ),
		tr( "Marriage" ),
		tr( "Wealth" ),
		tr( "Coborn" ),
		tr( "Fortunes" ),
		tr( "Health" ),
		tr( "Children" ),
		tr( "Troubles" ),
		tr( "Profession" ),
		tr( "Parents" ),
		tr( "Conveyances" ),
		tr( "Spirituality" ),
		tr( "Learning" ),
		tr( "Strength" ),
		tr( "Evils" ),
		tr( "Maternal Legacy" ),
		tr( "Paternal Legacy" ),
		tr( "Past Karma" ),
		"", // D-108
		"", // D-144
		"" // Bhava
	};
	return defaultSignifications[(int)i];
}

/**************************************************************
***
**    Lang   ---   getGanaName
***
***************************************************************/
wxString Lang::getGanaName( const Gana &g ) const
{
	assert_valid_enum<Gana>( g );
	const static wxString k_gana_name[3] = { tr( "Deva" ), tr( "Manuj" ), tr( "Rakshas" ) };
	return k_gana_name[(int)g];
}

/**************************************************************
***
**    Lang   ---   getTaraName
***
***************************************************************/
wxString Lang::getTaraName( const int &index ) const
{
	assert( index >= -1 && index < 9 );

	if ( index == -1 ) return "none";

	const wxString k_tara_name[9] = {
		tr( "Janma" ), tr( "Sampat" ), tr( "Vipat" ), tr( "Kshema" ),
		tr( "Pratyak" ), tr( "Sadhaka" ), tr( "Vadha" ), tr( "Mitra" ), tr( "Parma Mitra" ) };

	return k_tara_name[index];
}

/**************************************************************
***
**    Lang   ---   getNadiName
***
***************************************************************/
wxString Lang::getNadiName( const Nadi &n ) const
{
	assert_valid_enum<Nadi>( n );
	const wxString k_nadi_name[3] =
	{
		tr( "Adi (Vata)" ), tr( "Madhya (Pitta)" ), tr( "Antya (Kapha)" )
	};
	return k_nadi_name[(int)n];
}

/**************************************************************
***
**    Lang   ---   getVarnaName
***
***************************************************************/
wxString Lang::getVarnaName( const Varna &v ) const
{
	assert_valid_enum<Varna>( v );
	const wxString k_varna_name[4] =
	{
		tr( "Brahmin" ),
		tr( "Kshattriya" ),
		tr( "Vaishya" ),
		tr( "Shudra" )
	};
	return k_varna_name[(int)v];
}

/**************************************************************
***
**    Lang   ---   getVashyaName
***
***************************************************************/
wxString Lang::getVashyaName( const Vashya &v ) const
{
	assert_valid_enum<Vashya>( v );
	switch( v )
	{
		case Vashya::Quadruped:
			return tr( "Quadruped" );
		break;
		case Vashya::Human:
			return tr( "Human" );
		break;
		case Vashya::Jalachara:
			return tr( "Jalachara" );
		break;
		case Vashya::Leo:
			return tr( "Leo" );
		break;
		case Vashya::Scorpio:
			return tr( "Scorpio" );
		break;
		default:
			assert( false );
		break;
	}
	return "<error>";
}

/**************************************************************
***
**   Lang   ---   getRajjuName
***
***************************************************************/
wxString Lang::getRajjuName( const int &aroha, const int &type ) const
{
	assert( aroha >= 0 && aroha <= 2 );
	assert( type >= 0 && type <= 4 );
	const wxString k_aroha_name[3] = { tr( "Aroha" ), "", tr( "Avaroha" ) };
	const wxString k_rajjutype_name[5] =
	{
		tr( "Pada (foot)" ), tr("Kati (waist)" ),
		tr( "Nabhi (navel)" ), tr( "Kantha (neck)" ), tr( "Siro (head)" )
	};

	if ( aroha == 1 ) return k_rajjutype_name[4];
	return std::format( "{}/{}", k_aroha_name[aroha], k_rajjutype_name[type] );
}

/**************************************************************
***
**    Lang   ---   getFriendshipName
***
***************************************************************/
wxString Lang::getFriendshipName( const PlanetaryFriendship &f, const TextFormat &format,
	const bool &useVedicNames ) const
{
	assert_valid_enum<PlanetaryFriendship>( f );
	const static wxString k_friendship_sk[5] =
	{
		tr( "Adhimitra" ), tr( "Mitra" ),
		tr( "Sama" ), tr( "Satru" ), tr( "Adhisatru" )
	}; 
	const static wxString k_friendship[5] =
	{
		tr( "Best friend" ), tr( "Friend" ),
		tr( "Neutral" ), tr( "Enemy" ), tr( "Sworn enemy" )
	}; 
	const static wxString k_friendship_short[5] =
	{
		tr( "BstFr" ), tr( "Frnd" ),
		tr( "Neutr" ), tr( "Enem" ), tr( "SwrEn" )
	};
	if ( format == TextFormat::Short )	return k_friendship_short[(int)f];
	else return useVedicNames ? k_friendship_sk[(int)f] : k_friendship[(int)f];
}

/**************************************************************
***
**    Lang   ---   getDignityName
***
***************************************************************/
wxString Lang::getDignityName( const Dignity &d, const TextFormat &format, const bool &useVedicNames ) const
{
	if ( d == Dignity::None ) return wxString( "[None]" );

	assert( d >= Dignity::Exalted && d <= Dignity::SwornEnemy );

	const static wxString k_dignity_name_sk[4] =
	{
		tr( "Uchcha" ), tr( "Nicha" ), tr( "Moolatrikona" ), tr( "Svakshetra" )
	};
	const static wxString k_dignity_name[4] =
	{
		tr( "Exalted" ), tr( "Debilated" ), tr( "Moolatrikona" ), tr( "Own" )
	};
	const static wxString k_dignity_short[4] = { tr( "Exal" ), tr( "Deb" ), tr( "Mool" ), tr( "Own" ) };
	int index = 0;

	switch( d )
	{
		case Dignity::Exalted:
			index = 0;
		break;
		case Dignity::Debilated:
			index = 1;
		break;
		case Dignity::Moolatrikona:
			index = 2;
		break;
		case Dignity::OwnSign:
			index = 3;
		break;
		case Dignity::BestFriend:
			return getFriendshipName( PlanetaryFriendship::BestFriend, format, useVedicNames );
		break;
		case Dignity::Friend:
			return getFriendshipName( PlanetaryFriendship::Friend, format, useVedicNames );
		break;
   	case Dignity::Neutral:
			return getFriendshipName( PlanetaryFriendship::Neutral, format, useVedicNames );
		break;
		case Dignity::Enemy:
			return getFriendshipName( PlanetaryFriendship::Enemy, format, useVedicNames );
		break;
		case Dignity::SwornEnemy:
			return getFriendshipName( PlanetaryFriendship::SwornEnemy, format, useVedicNames );
		default:
		break;
	}
	assert( index >= 0 && index < 4 );

	if ( format == TextFormat::Short )	return k_dignity_short[index];
	else return useVedicNames ?
		k_dignity_name_sk[(int)index] : k_dignity_name[(int)index];
}

/**************************************************************
***
**   Lang   ---   getVimsopakaDignityName
***
***************************************************************/
wxString Lang::getVimsopakaDignityName( const int &index, const int &dignity ) const
{
	assert( index >= 0 && index < 4 && dignity >= 0 );
	//debuglog << "Lang::getVimsopakaDignityName index " << index << " dignity " << dignity;

	const static wxString shadvarga_dignity[5] =
	{
		tr( "Kimsuka" ), tr( "Vyanjana" ), tr( "Chamara" ), tr( "Chatra" ), tr( "Kundala" )
	};
	const static wxString saptavarga_dignity[6] =
	{
		tr( "Kimsuka" ), tr( "Vyanjana" ), tr( "Chamara" ),
		tr( "Chatra" ), tr( "Kundala" ), tr( "Mukuta" )
	};
	const static wxString dasavarga_dignity[9] =
	{
		tr( "Parijata" ), tr( "Uttama" ),	tr( "Gopura" ), tr( "Simhasana" ), tr( "Paravata" ),
		tr( "Devaloka" ), tr( "Brahmaloka" ), tr( "Sakravahana" ), tr( "Sridhama"  )
	};
	const static wxString shodasavarga_dignity[15] =
	{
		tr( "Bhedaka" ), tr( "Kusumia"), tr( "Nagapushpa" ), tr( "Kanduka" ),
		tr( "Kerala" ), tr( "Kalpa Vriksha" ), tr( "Chandana Vana" ), tr( "PoornaChandra" ),
		tr( "Uchchaisrava" ), tr( "Dhanvantari" ), tr( "Suryakanta" ), tr( "Vidruma" ),
		tr( "Chakra Simhasana" ), tr( "Goloki" ), tr( "Sri Vallabha" )
	};

	if ( dignity < 2 ) return "";

	switch ( index )
	{
	case 0:
		assert( dignity <= 6 );
		return shadvarga_dignity[dignity-2];
		break;
	case 1:
		assert( dignity <= 7 );
		return saptavarga_dignity[dignity-2];
		break;
	case 2:
		assert( dignity <= 10 );
		return dasavarga_dignity[dignity-2];
		break;
	case 3:
		assert( dignity <= 16 );
		return shodasavarga_dignity[dignity-2];
		break;
	default:
		assert( false );
		break;
	}
	return "";
}

/**************************************************************
***
**     Lang   ---   getTithiName
***
***************************************************************/
wxString Lang::getTithiName( const int &index ) const
{
	assert( index >= 0 && index < 30 );
	const static wxString k_tithi_name[30] = {
		tr( "Sukla - Pratipad" ),
		tr( "Sukla - Dviteeya" ),
		tr( "Sukla - Triteeya" ),
		tr( "Sukla - Chaturthi" ),
		tr( "Sukla - Panchami" ),
		tr( "Sukla - Shasti" ),
		tr( "Sukla - Saptami" ),
		tr( "Sukla - Asthami" ),
		tr( "Sukla - Navami" ),
		tr( "Sukla - Dasami" ),
		tr( "Sukla - Ekadasi" ),
		tr( "Sukla - Dvadasi" ),
		tr( "Sukla - Trayodasi" ),
		tr( "Sukla - Chaturdasi" ),
		tr( "Sukla - Purnima" ),
		tr( "Krishna - Pratipad" ),
		tr( "Krishna - Dviteeya" ),
		tr( "Krishna - Triteeya" ),
		tr( "Krishna - Chaturthi" ),
		tr( "Krishna - Panchami" ),
		tr( "Krishna - Shasti" ),
		tr( "Krishna - Saptami" ),
		tr( "Krishna - Asthami" ),
		tr( "Krishna - Navami" ),
		tr( "Krishna - Dasami" ),
		tr( "Krishna - Ekadasi" ),
		tr( "Krishna - Dvadasi" ),
		tr( "Krishna - Trayodasi" ),
		tr( "Krishna - Chaturdasi" ),
		tr( "Krishna - Amavasya" ),
	};
	return k_tithi_name[index];
}

/**************************************************************
***
**     Lang   ---   getWeekdayName
***
***************************************************************/
wxString Lang::getWeekdayName( const int &day ) const
{
	assert(( day >= 0 ) || ( day < 7 ));
	const static wxString wd[7] =
	{
		tr("Sunday"), tr("Monday"), tr("Tuesday"),
		tr("Wednesday"), tr("Thursday"), tr("Friday"), tr("Saturday")
	};
	return wd[day];
}

/**************************************************************
***
**     Lang   ---   getSignName
***
***************************************************************/
wxString Lang::getSignName( const Sign &sign, const Viewpoint &viewpoint, const TextFormat format ) const
{
  PlanetPropertyConfig cfg = astroconfig->getViewpointConfig( viewpoint ).planetProps;
	if ( format != TextFormat::None )
	{
		cfg.textFormat = format;
	}
	return getSignName( sign, cfg );
}

/**************************************************************
***
**     Lang   ---   getSignName
***
***************************************************************/
wxString Lang::getSignName( const Sign &sign, const PlanetPropertyConfig &cfg ) const
{
	if ( sign == Sign::None ) return c_none;

	if ( ! is_valid_enum( sign ))
	{
		errorlog << "invalid sign " << (int)sign;
		return c_error;
	}

	switch( cfg.textFormat )
	{
		case TextFormat::Short:
		{
			if ( cfg.vedicSignNames )
			{
				const static wxString k_sign_short_vedic[12] =
				{
					"Ms", "Vs", "Mi", "Kt", "Si", "Kn",
					"Tu", "Vc", "Dh", "Mk", "Ku", "Mn"
				};
				return k_sign_short_vedic[(int)sign];
			}
			else
			{
				const static wxString k_sign_short[12] =
				{
					tr( "Ar" ), tr( "Ta" ), tr( "Ge" ), tr( "Cn" ), tr( "Le" ), tr( "Vi" ),
					tr( "Li" ), tr( "Sc" ), tr( "Sa" ), tr( "Cp" ), tr( "Aq" ), tr( "Pi" )
				};
				return k_sign_short[(int)sign];
			}
		}
		break;
		case TextFormat::Medium:
		case TextFormat::Long:
		{
			if ( cfg.vedicSignNames )
			{
				if ( cfg.textFormat == TextFormat::Long )
				{
					const static wxString k_sign_full_vedic[12] =
					{
						"Mesha", "Vrishabha", "Mithuna",
						"Kataka", "Simha", "Kanya",
						"Tula", "Vrischika", "Dhanus",
						"Makara", "Kumbha", "Meena"
					};
					return k_sign_full_vedic[(int)sign];
				}
				else // format == TextFormat::Medium
				{
					const static wxString k_sign_medium_vedic[12] =
					{
						"Mes", "Vrs", "Mit", "Kat", "Sim", "Kan",
						"Tul", "Vrc", "Dha", "Mak", "Kum", "Mee"
					};
					return k_sign_medium_vedic[(int)sign];
				}
			}
			else // ! vedicSignNames
			{
				const static wxString k_sign_full[12]=
				{
					"Aries", "Taurus", "Gemini",
					"Cancer", "Leo", "Virgo",
					"Libra", "Scorpio", "Sagittarius",
					"Capricorn", "Aquarius", "Pisces"
				};
				const static wxString k_sign_full_translated[12]=
				{
					tr( "Aries" ), tr( "Taurus" ), tr( "Gemini" ),
					tr( "Cancer" ), tr( "Leo" ), tr( "Virgo" ),
					tr( "Libra" ), tr( "Scorpio" ), tr( "Sagittarius" ),
					tr( "Capricorn" ), tr( "Aquarius" ), tr( "Pisces" )
				};
				if ( cfg.textFormat == TextFormat::Long )
				{
					//return k_sign_full[(int)sign];
					return cfg.translateSignNames ? k_sign_full_translated[(int)sign] : k_sign_full[(int)sign];
				}
				else
				{
					//return k_sign_full[(int)sign].substr( 0, 3 );
					return cfg.translateSignNames ? k_sign_full_translated[(int)sign].substr( 0, 3 )
						: k_sign_full[(int)sign].substr( 0, 3 );
				}
			}
		}
		default:
			errorlog << "invalid text format " << (int)cfg.textFormat;
			return c_error;
		break;
	}
}

/**************************************************************
***
**    Lang   ---   getKarakaName
***
***************************************************************/
wxString Lang::getKarakaName( const CharaKaraka &ck, const TextFormat ) const
{
	const static wxString chara_karaka_name_long[8] =
	{
		tr( "Atma" ), tr( "Amatya" ), tr( "Bhratru" ),
		tr( "Matru" ), tr( "Pitru" ), tr( "Putra" ),
		tr( "Gnati" ), tr( "Dhanya"  )
	};
	assert_valid_enum_or_none<CharaKaraka>( ck );
	if ( ck == CharaKaraka::None ) return "";
	else return chara_karaka_name_long[(int)ck];
}

/**************************************************************
***
**    Lang   ---   getShastiamsaName
***
***************************************************************/
wxString Lang::getShastiamsaName( const int &i ) const
{
	const static wxString k_shastiamsa_name[60] = {
		tr( "Ghora" ), tr( "Rakshasa" ), tr( "Deva" ), tr( "Kubera" ), tr( "Yaksha" ),
		tr( "Kinnara" ), tr( "Bhrashta" ), tr( "Kulaghna" ), tr( "Garala" ), tr( "Vahni" ),
		tr( "Maya" ), tr( "Purishaka" ), tr( "Apampathi" ), tr( "Marut" ), tr( "Kaala" ),
		tr( "Sarpa" ), tr( "Amrita" ), tr( "Indu" ), tr( "Mridu" ), tr( "Komala" ),
		tr( "Heramba" ), tr( "Brahma" ), tr( "Vishnu" ), tr( "Maheswara" ), tr( "Deva" ),
		tr( "Ardra" ), tr( "Kalinasa" ), tr( "Kshiteesa" ), tr( "Kamalakara" ), tr( "Gulika" ),
		tr( "Mrithyu" ), tr( "Kaala" ), tr( "Davagani" ), tr( "Ghora" ), tr( "Yama" ),
		tr( "Kantaka" ), tr( "Sudha" ), tr( "Amrita" ), tr( "Poornachandra" ), tr( "Vishadagdha" ),
		tr( "Kulanasa" ), tr( "Vamsakshaya" ), tr( "Utpata" ), tr( "Kaala" ), tr( "Saumya" ),
		tr( "Komala" ), tr( "Seetala" ), tr( "Karaladamshtra" ), tr( "Chandramukhi" ), tr( "Praveena" ),
		tr( "Kaala Pavaka" ), tr( "Dandayudha" ), tr( "Nirmala" ), tr( "Saumya" ), tr( "Kroora" ),
		tr( "Atiseetala" ), tr( "Amrita" ), tr( "Payodhi" ), tr( "Bhramana" ), tr( "Chandra Rekha" )
	};
	assert( i >= 0 && i < 60 );
	return k_shastiamsa_name[i];
}

/**************************************************************
***
**    Lang   ---   getBhavaName
***
***************************************************************/
wxString Lang::getBhavaName( const House &i ) const
{
	const static wxString k_bhava_name[12] = { tr( "Lagna" ), tr( "Dhana" ),
		tr( "Sahaja" ), tr( "Bandhu" ), tr( "Putra" ), tr( "Satru" ), tr( "Yuvati" ),
		tr( "Randhra" ), tr( "Dharma" ), tr( "Karma" ), tr( "Labha" ), tr( "Vyaya" ) }; 
	assert_valid_enum<House>( i );
	return k_bhava_name[(int)i];
}

/**************************************************************
***
**    Lang   ---   getMonthName
***
***************************************************************/
wxString Lang::getMonthName( const int &i, const TextFormat format ) const
{
	assert( i >= 0 && i < 12 );
	const static wxString month_name[12] = {
		tr( "January" ), tr( "February" ), tr( "March" ),
		tr( "April" ), tr( "May" ), tr( "June" ),
		tr( "July" ), tr( "August" ), tr( "September" ),
		tr( "October" ), tr( "November" ), tr( "December" ) };
	if ( format == TextFormat::Short )
	{
		return month_name[i].substr( 0, 3 );
	}
	return month_name[i];
}

/*****************************************************
**
**   Lang   ---   getElementName
**
******************************************************/
wxString Lang::getElementName( const Element &elem ) const
{
	assert_valid_enum<Element>( elem );
	switch( elem )
	{
		case Element::Fire:
			return tr( "Fire" );
		break;
		case Element::Earth:
			return tr( "Earth" );
		break;
		case Element::Air:
			return tr( "Air" );
		break;
		case Element::Water:
			return tr( "Water" );
		break;
		default:
			assert( false );
		break;
	}
	return "<error>";
}

/*****************************************************
**
**   Lang   ---   getSignQualityName
**
******************************************************/
wxString Lang::getSignQualityName( const SignQuality &qual ) const
{
	assert_valid_enum<SignQuality>( qual );
	switch( qual )
	{
		case SignQuality::Movable:
			return tr( "Movable" );
		break;
		case SignQuality::Fixed:
			return tr( "Fixed" );
		break;
		case SignQuality::Dual:
			return tr( "Dual" );
		break;
		default:
			assert( false );
		break;
	}
	return "<error>";
}

/*****************************************************
**
**   Lang   ---   getChartSideName
**
******************************************************/
wxString Lang::getChartSideName( const ChartSide &side ) const
{
	assert_valid_enum<ChartSide>( side );
	switch( side )
	{
		case ChartSide::North:
			return tr( "Above horizon" );
		break;
		case ChartSide::West:
			return tr( "Western part" );
		break;
		case ChartSide::South:
			return tr( "Below horizon" );
		break;
		case ChartSide::East:
			return tr( "Eastern part" );
		break;
		default:
			assert( false );
		break;
	}
	return "<error>";
}

/*****************************************************
**
**   Lang   ---   getRoleContextSubscriptum
**
******************************************************/
wxString Lang::getRoleContextSubscriptum( const HoroscopeRole &role ) const
{
	switch( role )
  {
    case HoroscopeRole::Radix:
			return "r";
    break;
    case HoroscopeRole::Solar:
			return "S";
    break;
    case HoroscopeRole::Partner1:
			return "1";
    break;
    case HoroscopeRole::Partner2:
			return "2";
    break;
    case HoroscopeRole::Composite:
			return "c";
    break;
    case HoroscopeRole::Transit:
			return "t";
    break;
    case HoroscopeRole::Progressive:
			return "p";
    break;
    case HoroscopeRole::SolarArc:
			return "s";
    break;
    case HoroscopeRole::ReverseSolarArc:
			return "-s";
    break;
    case HoroscopeRole::DoubleSolarArc:
			return "2s";
    break;
    case HoroscopeRole::HalfSolarArc:
			return "1/2s";
    break;
    case HoroscopeRole::LunarArc:
			return "ls";
    break;
    case HoroscopeRole::ConstantArc:
			return "cs";
    break;
    case HoroscopeRole::Lunations:
			return "L";
    break;
    case HoroscopeRole::Profections:
			return "P";
    break;
    default:
			warnlog << "Lang::getRoleContextSubscriptum: wrong role " << (int)role;
			return "X";
    break;
  }
}

/*****************************************************
**
**   Lang   ---   getObjectSignification
**
******************************************************/
wxString Lang::getObjectSignification( const PlanetId &p, const TextFormat&, const bool vedic )
{
	const static wxString k_vedic_signification[7] =
	{
		tr( "Atma" ),
		tr( "Mind" ),
		tr( "Speech" ),
		tr( "Semen" ),
		tr( "Strength" ),
		tr( "Guru" ),
		tr( "Grief" )
	};
	if ( vedic && p >= PlanetId::Sun && p <= PlanetId::Saturn ) return k_vedic_signification[int(p)];
	else return "";
};

/*****************************************************
**
**   Lang   ---   getHoroscopeRoleName
**
******************************************************/
wxString Lang::getHoroscopeRoleName( const HoroscopeRole &role )
{
	return resolveCamelNotation(  role );
#if 0
	const static wxString k_role_name[enum_count<HoroscopeRole>()] =
	{
		tr( "Radix" ),
		tr( "Partner1" ),
		tr( "Partner2" ),
		tr( "Composite" ),
		tr( "Solar" ),
		tr( "Transit" ),
		tr( "Progressive" ),
		tr( "Solar Arc" ),
		tr( "Reverse Solar Arc" ),
		tr( "Double Solar Arc" ),
		tr( "Lunations" ),
		tr( "Profections" ),
		tr( "Shifted Gravitation Point" ),
		tr( "Shifted Meridian" ),
		tr( "Lunar Arc" ),
		tr( "Constant Arc" )
	};
	return k_role_name[int(role)];
#endif
}

/*****************************************************
**
**   Lang   ---   getAyanamsaName
**
******************************************************/
wxString Lang::getAyanamsaName( const int &ayanamsa )
{
	switch ( ayanamsa )
	{
		case 0:
			return tr( "Tropical" );
		break;
		case 1:
			return tr( "Custom" );
		break;

		default:
			return CalculatorFactory().getCalculator()->getBuiltinAyanamsaName( ayanamsa - 2 );
		break;
	}
	return tr( "ERROR" );
}

