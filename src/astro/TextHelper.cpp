/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/TextHelper.cpp
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

#include "TextHelper.h"

#include "ArabicParts.h"
#include "Ashtakavarga.h"
#include "astrobase.h"
#include "astroconfig.h"
#include "Calculator.h"
#include "ContextProperties.h"
#include "Dasa.h"
#include "DasaTool.h"
#include "enumserializer.h"
#include "PlanetBalance.h"
#include "Formatter.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "Jaimini.h"
#include "Lang.h"
#include "logger.h"
#include "constants.h"
#include "mathbase.h"
#include "Nakshatra.h"
#include "Shadbala.h"
#include "MDocument.h"
#include "Table.h"
#include "TableWriter.h"
#include "UranianHelper.h"
#include "Varga.h"
#include "VedicAspect.h"
#include "WesternAspect.h"

#include <math.h>
#include <format>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define DEBUG_TEXTHELPER

/*****************************************************
**
**   TextHelper   ---   Constructor
**
******************************************************/
TextHelper::TextHelper( ContextProps_ptr props, MDocument_ptr doc )
	: Expert( props, Viewpoint::None ),
	doc( doc )
{
	//show_header = gdiconfig->view.showTextViewHeaders;
	show_header = true;
}

/*****************************************************
**
**   TextHelper   ---   writeTextAnalysis
**
******************************************************/
int TextHelper::writeTextAnalysis( const WidgetCategory &cat, const Varga varga, const DasaId dasa )
{
	int ret = 0;
	Horoscope_ptr horoscope = props->getHoroscope( HoroscopeRole::Radix );
	assert( horoscope );

	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::None );
	debuglog << "TextHelper::writeTextAnalysis category " << cat
		//<< " ViewpointPreference " << pref
		<< " viewpoint " << viewpoint;

	switch ( cat )
	{
		case WidgetCategory::TextBaseData:
			writeBasicHoroscopeData();
		break;

		case WidgetCategory::TextPlanetListAstronomical:
			ret = writePlanets( horoscope, Viewpoint::Astronomical );
		break;
		case WidgetCategory::TextPlanetListUniversal:
			ret = writePlanets( horoscope, viewpoint );
		break;
		case WidgetCategory::TextPlanetListUranian:
			ret = writePlanets( horoscope, Viewpoint::Uranian );
		break;
		case WidgetCategory::TextPlanetListVedic:
			ret = writePlanets( horoscope, Viewpoint::Vedic );
		break;
		case WidgetCategory::TextPlanetListWestern:
			ret = writePlanets( horoscope, Viewpoint::Western );
		break;

		case WidgetCategory::TextPlanetBalance:
		{
			PlanetBalanceExpert expert( props, astroconfig->getViewpointConfig( viewpoint ).planets );
			expert.update();
			expert.write( doc, show_header );
		}
		break;

		case WidgetCategory::TextNakshatra:
			NakshatraExpert( props ).write( doc, show_header );
		break;

		case WidgetCategory::TextAshtakavarga:
		{
			AshtakavargaExpert aexpert( props, varga );
			aexpert.update();
			aexpert.write( doc, show_header );
		}
		break;

		case WidgetCategory::TextKp:
			ret = writeKp( dasa );
		break;

		case WidgetCategory::TextVedicHouses:
			writeVedicHouses();
		break;

		case WidgetCategory::TextUranianHouses:
			writeUranianHouses();
		break;

		case WidgetCategory::TextArabicParts:
			ArabicPartsExpert( horoscope.get() ).write( doc, viewpoint, show_header );
		break;

		case WidgetCategory::TextVedicAspectarium:
		{
			VedicAspectExpert aspexpert( props );
			aspexpert.writeVedicAspectarium( doc, horoscope, show_header );
		}
		break;

		case WidgetCategory::TextWesternAspectarium:
		{
			WesternAspectExpert expert( props );
			const auto l = expert.calculate();
			//expert.writeAspectarium( doc, l, astroconfig->getViewpointConfig( viewpoint ).planets );
			doc->addItem( expert.createAspectariumTable( l, astroconfig->getViewpointConfig( viewpoint ).planets, true ));
		}
		break;

		case WidgetCategory::TextWesternAspectList:
		{
			WesternAspectExpert expert( props );
			const auto l = expert.calculate();
			expert.writeAspectList( doc, l );
		}
		break;

		case WidgetCategory::TextShadbala:
		{
			ShadbalaExpert se( props );
			se.updateAllBalas();
			se.write( doc );
		}
		break;

		case WidgetCategory::TextVarga:
		{
			VargaExpert vexpert( horoscope.get(), viewpoint );
			vexpert.write( props, doc, show_header );
		}
		break;

		case WidgetCategory::TextDasaShort:
		{
			assert_valid_enum<DasaId>( dasa );
			DasaTool *tool = DasaTool::get();
			tool->writeShortReport( doc, (DasaId)dasa, props, show_header );
		}
		break;

		case WidgetCategory::TextDasaCompact:
		{
			assert_valid_enum<DasaId>( dasa );
			DasaTool *tool = DasaTool::get();
			tool->writeCompactReport( doc, (DasaId)dasa, props, show_header );
		}
		break;

		case WidgetCategory::TextDasaComprehensive:
		{
			assert_valid_enum<DasaId>( dasa );
			DasaTool *tool = DasaTool::get();
			tool->writeComprehensiveReport( doc, (DasaId)dasa, props, show_header );
		}
		break;

		case WidgetCategory::TextJaimini:
		{
			JaiminiExpert jexpert( props, varga );
			jexpert.update();
			jexpert.write( doc, show_header );
		}
		break;


		default:
		{
			const wxString err = std::format( "Error: Invalid widget category for text generation: ", enum_to_string( cat ));
			doc->addHeader2( err );
			errorlog << "TextHelper::writeTextAnalysis: " << err;
		}
		break;
	}
	if ( ret ) warnlog << ret << " objects couldn't be calculated";

	debuglog << "TEXT HELPER doc size " << doc->items.size();
	return ret;
}

/*****************************************************
**
**   TextHelper   ---   writePlanets
**
******************************************************/
int TextHelper::writePlanets( Horoscope_ptr horoscope )
{
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::None );
	return writePlanets( horoscope, viewpoint );
}

/*****************************************************
**
**   TextHelper   ---   writePlanets
**
******************************************************/
int TextHelper::writePlanets( Horoscope_ptr horoscope, const Viewpoint &viewpoint )
{
#ifdef DEBUG_TEXTHELPER
	debuglog << "TextHelper::writePlanets ROLE " << horoscope->getRole();
#endif

	wxString s = Lang().getHoroscopeRoleName( horoscope->getRole());
	if ( horoscope->getRole() == HoroscopeRole::Radix )
	{
		s += ": " + horoscope->dl.name;
	}
	doc->addHeader1( s );

	std::set<FilterGroup> filter;
	if ( horoscope->getLongitudeOffset() == 0 )
	{
		filter = { FilterGroup::CardinalPoints, };
	}
	const auto obs = props->getPlanets( viewpoint, filter );

#ifdef DEBUG_TEXTHELPER
	for( const auto &p : obs )
	{
		debuglog << "TextHelper::writePlanets p " << p;
	}
#endif

  PlanetPropertyConfig ocfg( astroconfig->getViewpointConfig( viewpoint ).planetProps );
	ocfg.columnGroups = props->getColumnGroups( viewpoint );

	Table *table = TableWriter( props, horoscope ).createPlanetListTable( obs,
		astroconfig->getViewpointConfig( viewpoint ).planets, ocfg );
	doc->addItem( table );
	return table->errorcount;
}

/*****************************************************
**
**   TextHelper   ---   writeBasicHoroscopeData
**
******************************************************/
void TextHelper::writeBasicHoroscopeData()
{
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );
	TableWriter extractor( props, horoscope );
	const BasedataConfig &bConfig = astroconfig->getViewpointConfig( getViewpoint() ).basedata;

	doc->addHeader2( tr( "Basic Data" ));

	std::list<BasedataAttribute> bl;
	for( const auto &a : bConfig.basedataItems )
  {
    if ( a.second ) bl.push_back( a.first );
  }
	doc->addItem( extractor.createBaseDataTable( bl ));
}

/*****************************************************
**
**   TextHelper   ---   writeUranianHouses
**
******************************************************/
void TextHelper::writeUranianHouses()
{
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Vedic );

	UranianHelper::get()->writeHouseReport( props, doc, viewpoint );
}

/*****************************************************
**
**   TextHelper   ---   writeVedicHouses
**
******************************************************/
void TextHelper::writeVedicHouses()
{
	Lang lang;
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );

	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Vedic );
	Formatter *formatter = Formatter::get();
	const CalculationConfig cconfig = astroconfig->getViewpointConfig( viewpoint ).calculation;

	Table *t1 = new Table( tr( "Houses (Bhavas) in Vedic astrology" ));
	t1->addHeaderRow(
	{
		tr( "No." ),
		tr( "Name" ),
		tr( "Cusp" ),
		tr( "Sandhi" ),
	});

	for( const House i : Enum<House>() )
	{
		TableRow row;
		row.addInteger( (int)i + 1 );
		row.addValue( lang.getBhavaName( i ));
		row.addValue( formatter->formatZodiacalLongitude( viewpoint, horoscope->getHouseCusp( i, viewpoint )));
		row.addValue( formatter->formatZodiacalLongitude( viewpoint, horoscope->getHouseSandhi( i, viewpoint )));
		t1->addRow( row );
	}
	doc->addItem( t1 );

	Table *t2 = new Table( tr( "Planets in Houses" ));
	t2->addHeaderRow(
	{
		tr( "Planet" ),
		tr( "Longitude" ),
		tr( "No." ),
		tr( "Vedic Name" ),
	});

	for ( const PlanetId p : props->getPlanets( viewpoint, { FilterGroup::CardinalPoints, }))
	{
		TableRow row;
		row.addValue( MToken( p, viewpoint ));
		row.addValue( formatter->formatZodiacalLongitude( viewpoint, horoscope->getLongitude( p, viewpoint ), 
			horoscope->isRetrograde( p, viewpoint ) ? MovingDirection::Retrograde : MovingDirection::None
		));

		const House hpos = horoscope->getHousePos( p, viewpoint );
		row.addInteger( (int)hpos + 1 );
		row.addValue( lang.getBhavaName( hpos ));
		t2->addRow( row );
	}
	doc->addItem( t2 );
	doc->addLine( std::format( "{}: {}", tr( "Viewpoint" ), enum_to_string( viewpoint )));
	doc->addLine( std::format( "{}: {}", tr( "House system" ), enum_to_string( cconfig.houseSystem )));
	doc->addLine( std::format( "{}: {}", tr( "House start at" ), enum_to_string( cconfig.houseStart )));

}

/*****************************************************
**
**   TextHelper   ---   writeKp
**
******************************************************/
int TextHelper::writeKp( const DasaId &dasaindex )
{
	Lang lang;
	int ret = 0;
	double len, hlen;

	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );

	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Vedic );
	DasaExpert_ptr expert (DasaExpertFactory().getDasaExpert( dasaindex ));

	DasaTool *tool = DasaTool::get();
	KpData kp;
	Formatter *formatter = Formatter::get();

	if ( ! expert->hasKpFeatures() ) // Not supported by all (e.g. Jaimini)
	{
		doc->addLine( tr( "Not supported" ) );
		return 0;
	}
	if ( show_header )
	{
		doc->addHeader2( tr( "Krishnamurti Paddhati" ));
		doc->addParagraph( tr( "Dasa") + ": " + tool->getDasaName( dasaindex ));
	}

	auto obs = props->getPlanets( viewpoint );
	const bool hasSubsublord = ( dasaindex != DasaId::Kalachakra );

	Table *table = new Table;
	table->addHeaderRow(
	{
		tr( "Planet" ),
		tr( "Longitude" ),
		tr( "Sign Lord" ),
		tr( "Bhava" ),
		tr( "KP Lord" ),
		tr( "Sublord" ),
		tr( "Subsublord" )
	});

	for ( const PlanetId &o : obs )
	{
		if ( o != PlanetId::Aries
			&& o != PlanetId::Capricorn
			&& horoscope->hasSuspiciousLongitude( o, viewpoint )) ret++;

		len = horoscope->getLongitude( o, viewpoint );
		TableRow row;
		
		row.addValue( MToken( o, viewpoint ));
		row.addValue( formatter->formatZodiacalLongitude( viewpoint, len, 
			horoscope->isRetrograde( o, viewpoint ) ? MovingDirection::Retrograde : MovingDirection::None ));
		row.addValue( MToken( getLord( getRasi( len )), viewpoint ));
		row.addValue( std::format( "{:02d}",  ((int)(horoscope->getHousePos( o, viewpoint )) + 1)));

		kp = expert->getKPLords( horoscope->getLongitude( o, viewpoint ));

		row.addValue( expert->getDasaLordName( kp.lord ));
		row.addValue( expert->getDasaLordName( kp.sublord ));
		if ( hasSubsublord )
		{
			row.addValue( expert->getDasaLordName( kp.subsublord ));
		}
		else
		{
			row.addValue();
		}
		table->addRow( row );
	}
	for( const House p : Enum<House>() )
	{
		hlen = horoscope->getHouseCusp( p, viewpoint );
		TableRow row;
		row.addValue( lang.getBhavaName( p ));
		row.addValue( formatter->formatZodiacalLongitude( viewpoint, hlen ));
		row.addValue( MToken( getLord(getRasi( hlen )), viewpoint ));

		row.addValue();

		kp = expert->getKPLords( hlen );
		row.addValue( expert->getDasaLordName( kp.lord ));
		row.addValue( expert->getDasaLordName( kp.sublord ));
		if ( hasSubsublord )
		{
			row.addValue( expert->getDasaLordName( kp.subsublord ));
		}
		else
		{
			row.addValue();
		}
		table->addRow( row );
	}
	doc->addItem( table );
	return ret;
}

/*****************************************************
**
**   TextHelper   ---   writeInvalidPartnerDoc
**
******************************************************/
void TextHelper::writeInvalidPartnerDoc()
{
	doc->addLine( tr( "No horoscope set." ));
}

/*****************************************************
**
**   TextHelper   ---   writeMultiDirectionReport
**
******************************************************/
int TextHelper::writeMultiDirectionReport()
{
	constexpr bool show_header = true;
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::None );
	Formatter *formatter = Formatter::get();

	Horoscope_ptr hradix = props->getRadixHoroscope();
	assert( hradix );
	assert( hradix->getRole() == HoroscopeRole::Radix );

	// get direction roles
	std::list<HoroscopeRole> roles;
	for( const auto &role : props->getActiveRoles())
	{
		if ( role == HoroscopeRole::Radix ) continue;

		if ( role >= HoroscopeRole::FirstDirection && role <= HoroscopeRole::LastDirection )
		{
			roles.push_back( role );
		}
	}
	debuglog << "TextHelper::writeMultitransitReport has " << roles.size() << " active direction roles";

	Table *table = new Table( roles.size() + 1, 1 );
	table->setHeader( 0, tr( "Planet" ));
	uint col = 1;

	// write direction roles into header
	for( const auto &role : roles )
	{
		assert( col < roles.size() + 1 );
		table->setHeader( col++, enum_to_string( role ));
	}
	if ( show_header ) table->setTitle( tr( "Multitransit" ));

	// write contents
	for ( const PlanetId p : props->getPlanets( viewpoint )) 
	{
		TableRow row;
		row.addValue( MToken( p, viewpoint  ));

		col = 1;
		for( const auto &role : roles )
		{
			assert( col < roles.size() + 1 );

			Horoscope_ptr h = props->getHoroscope( role );
			assert( h );

			row.addValue( formatter->formatZodiacalLongitude(
				viewpoint,
				h->getLongitude( p, viewpoint ),
				h->isRetrograde( p, viewpoint ) ? MovingDirection::Retrograde : MovingDirection::None ));
		}
		table->addRow( row );
	}

	doc->addItem( table );
	return table->errorcount;
}

/*****************************************************
**
**   TextHelper   ---   writeSynastryReport
**
******************************************************/
int TextHelper::writeSynastryReport()
{
	Horoscope_ptr h1 = props->getHoroscope( HoroscopeRole::Partner1 );
	assert( h1 );
	Horoscope_ptr h2 = props->getHoroscope( HoroscopeRole::Partner2 );
	assert( h2 );
	if ( ! h1 || ! h2 )
	{
		return 0;
	}
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::None );
	Formatter *formatter = Formatter::get();

	Table *table = new Table( show_header ? tr( "Partner Positions" ) : wxString() );
	table->addHeaderRow(
	{
		tr( "Planet" ),
		h1->dl.name,
		h2->dl.name
	}); 

	for ( const PlanetId p : props->getPlanets( viewpoint, { FilterGroup::CardinalPoints, }))
	{
		TableRow row;
		row.addValue( MToken( p, viewpoint  ));
		row.addValue( formatter->formatZodiacalLongitude(
			viewpoint,
			h1->getLongitude( p, viewpoint ),
			h1->isRetrograde( p, viewpoint ) ? MovingDirection::Retrograde : MovingDirection::None ));
		if ( PlanetHelper::isEphemPlanet( p ) && h1->hasSuspiciousLongitude( p, viewpoint ))
		{
			warnlog << "TextHelper::writeSynastryReport hasSuspiciousLongitude on " << (int)p;
			table->errorcount++;
		}

		row.addValue( formatter->formatZodiacalLongitude(
			viewpoint,
			h2->getLongitude( p, viewpoint ),
			h2->isRetrograde( p, viewpoint ) ? MovingDirection::Retrograde : MovingDirection::None ));
		if ( PlanetHelper::isEphemPlanet( p )
			&& h2->hasSuspiciousLongitude( p, viewpoint )) table->errorcount++;

		table->addRow( row );
	}
	doc->addItem( table );
	return table->errorcount;
}

