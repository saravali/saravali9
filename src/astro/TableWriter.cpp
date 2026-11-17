/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/TableWriter.cpp
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

#include "TableWriter.h"

#include "Ashtakavarga.h"
#include "astrobase.h"
#include "astroconfig.h"
#include "ContextProperties.h"
#include "Dasa.h"
#include "DasaTool.h"
#include "DateFormatter.h"
#include "Dignity.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "Hora.h"
#include "Horoscope.h"
#include "Jaimini.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "Nakshatra.h"
#include "PlanetBalance.h"
#include "MDocument.h"
#include "Table.h"
#include "Varga.h"
#include "WesternAspect.h"

#include <format>

extern Astroconfig *astroconfig;

/*****************************************************
**
**   TableWriter   ---   Constructor
**
******************************************************/
TableWriter::TableWriter( ContextProps_ptr props, Horoscope_ptr horoscope )
 : props( props ),
 h( horoscope )
{
	kpdepth = 3;
	viewpoint = props->getEffectiveViewpoint( Viewpoint::None );
}

/*****************************************************
**
**   TableWriter   ---   createPlanetListTable
**
******************************************************/
Table *TableWriter::createPlanetListTable()
{
	return createPlanetListTable( props->getPlanets( viewpoint ), 
		astroconfig->getViewpointConfig( viewpoint ).planets,
		astroconfig->getViewpointConfig( viewpoint ).planetProps );
}

/*****************************************************
**
**   TableWriter   ---   getGroupColumnCount
**
******************************************************/
uint TableWriter::getGroupColumnCount( const Planets &obs, const ColumnGroup &group )
{
	switch( group )
	{
		// kp lord property has three columns
		case ColumnGroup::KpLords:
			return 3;
		break;
		case ColumnGroup::WesternAspectarium:
			return obs.size() - 1;
		break;
		default:
		break;
	}
	return 1;
}

/*****************************************************
**
**   TableWriter   ---   getPlanetListColumnCount
**
******************************************************/
uint TableWriter::getPlanetListColumnCount( const Planets &obs, const PlanetPropertyConfig &propertyCfg )
{
	uint colcount = 0;
	for( const auto &a : propertyCfg.columnGroups )
	{
		if ( a.second )
		{
			colcount += getGroupColumnCount( obs, a.first );
		}
	}
	return colcount;
}

/*****************************************************
**
**   TableWriter   ---   writePlanetListTable
**
******************************************************/
void TableWriter::writePlanetListTable( Planets obs, const PlanetConfig &planetCfg,
	PlanetPropertyConfig propertyCfg, Table *table,
	const uint colOffset, const int rowOffset, const bool doPrintTableHeader )
{
	assert( table );
	assert( h );

	// check if planet list is empty. If so, replace by default list
	if ( obs.size() == 0 )
	{
		warnlog << "TableWriter::writePlanetListTable has empty planet list, replacing by default";
		obs = ViewpointHelper::get()->getDefaultPlanets( viewpoint );
	}

	// check if columns group list is empty. If so, replace by default list
	int columnGroupCount = 0;
	for( const auto &c : propertyCfg.columnGroups )
	{
		if ( c.second ) columnGroupCount++;
	}
	if ( columnGroupCount == 0 )
	{
		warnlog << "TableWriter::writePlanetListTable has empty column list, replacing by default";
		propertyCfg.columnGroups	 = PlanetPropertyConfig( viewpoint ).columnGroups;
	}

	uint row = rowOffset;
	uint col = colOffset;

	Formatter *formatter = Formatter::get();
	Lang lang;
	JaiminiExpert *jexpert = nullptr;
	AshtakavargaExpert *aexpert = nullptr;

	// Aspectarium table is inserted out of the loop
	uint aspectariumStartCol = 0;

	const bool vedic = ( viewpoint == Viewpoint::Vedic );

	for( const auto &a : propertyCfg.columnGroups )
	{
		if ( ! a.second ) continue;
		switch( a.first )
		{
			case ColumnGroup::CharaKaraka:
				jexpert = new JaiminiExpert( props, Varga::Rasi );
				jexpert->calcCharaKarakas();
			break;

			case ColumnGroup::RekhaPoints:
				aexpert = new AshtakavargaExpert( props, Varga::Rasi );
				aexpert->update();
			break;

			default:
			break;
		}
	}

	// set headers
	if ( doPrintTableHeader )
	{
		//table->setHeaderRow( 0 );
		col = colOffset;
		for( const auto &g : propertyCfg.columnGroups )
		{
			if ( ! g.second ) continue;
			if ( g.first == ColumnGroup::KpLords )
			{
				table->setHeaderEntry( col++, row, tr( "KPL" ));
				table->setHeaderEntry( col++, row, tr( "SL" ));
				table->setHeaderEntry( col++, row, tr( "SSL" ));
			}
			else if ( g.first == ColumnGroup::WesternAspectarium ) {}
			else
			{
				table->setHeaderEntry( col++, row,
					g.first != ColumnGroup::ObjectName ? ViewpointHelper::get()->getColumnGroupName( g.first, TextFormat::Short )
						: wxString( "" ));
			}
		}
		row++;
	}

	for( const PlanetId &ob : obs )
	{
		col = colOffset;

		for( const auto& colgroup : propertyCfg.columnGroups )
		{
			if ( ! colgroup.second ) continue;
			switch( colgroup.first )
			{
				case ColumnGroup::ObjectName:
				{
					table->setEntry( col++, row, MToken( ob, planetCfg ));
					auto a = table->getTextItem( col-1, row );
				}
				break;
				case ColumnGroup::Longitude:
					table->setEntry( col++, row, formatter->formatZodiacalLongitude(
						propertyCfg,
            h->getLongitude( ob, viewpoint ),
            h->isRetrograde( ob, viewpoint ) ? MovingDirection::Retrograde : MovingDirection::None
					));
					if ( PlanetHelper::isEphemPlanet( ob ) && h->hasSuspiciousLongitude( ob, viewpoint )) table->errorcount++;
				break;
				case ColumnGroup::Longitude90:
					table->setEntry( col++, row, formatter->formatDegrees( d_red( h->getLongitude( ob, viewpoint ), 4 ),
						propertyCfg.precision ));

					if ( PlanetHelper::isEphemPlanet( ob )
						&& h->hasSuspiciousLongitude( ob, viewpoint )) table->errorcount++;
				break;
				case ColumnGroup::Longitude45:
					table->setEntry( col++, row, formatter->formatDegrees( d_red( h->getLongitude( ob, viewpoint ), 8 ),
						propertyCfg.precision ));
					if ( PlanetHelper::isEphemPlanet( ob ) && h->hasSuspiciousLongitude( ob, viewpoint )) table->errorcount++;
				break;
				case ColumnGroup::Longitude225:
					table->setEntry( col++, row, formatter->formatDegrees( d_red( h->getLongitude( ob, viewpoint ), 16 ),	
						propertyCfg.precision ));
					if ( PlanetHelper::isEphemPlanet( ob ) && h->hasSuspiciousLongitude( ob, viewpoint )) table->errorcount++;
				break;

				case ColumnGroup::Antiscia:
					table->setEntry( col++, row, formatter->formatZodiacalLongitude(
            propertyCfg,
            getAntiscium( h->getLongitude( ob, viewpoint )),
            h->isRetrograde( ob, viewpoint ) ? MovingDirection::Retrograde : MovingDirection::None ));
					if ( PlanetHelper::isEphemPlanet( ob ) && h->hasSuspiciousLongitude( ob, viewpoint )) table->errorcount++;
				break;

				case ColumnGroup::Speed:
					table->setEntry( col++, row, formatter->formatSignedDegreeString( h->getLSpeed( ob, viewpoint ),
						propertyCfg.precision ));
				break;
				case ColumnGroup::Latitude:
					table->setEntry( col++, row, formatter->formatLatitude( h->getLatitude( ob, viewpoint ), propertyCfg.precision ));
				break;
				case ColumnGroup::Declination:
					table->setEntry( col++, row, formatter->formatLatitude( h->getDeclination( ob, viewpoint ), propertyCfg.precision ));
				break;
				case ColumnGroup::HousePosition:
					table->setEntry( col++, row, std::format( "{}", (int)h->getHousePos( ob, viewpoint ) + 1 ));
				break;
				case ColumnGroup::Element:
					table->setEntry( col++, row, lang.getElementName( getElement( h->getLongitude( ob, viewpoint ))));
				break;
				case ColumnGroup::SignQuality:
					table->setEntry( col++, row, lang.getSignQualityName( getSignQuality( h->getLongitude( ob, viewpoint ))));
				break;
				case ColumnGroup::Signification:
					table->setEntry( col++, row, lang.getObjectSignification( ob, TextFormat::Long, vedic ));
				break;
				case ColumnGroup::SignLord:
					table->setEntry( col++, row, MToken( getLord( getRasi( h->getLongitude( ob, viewpoint )), vedic ), planetCfg ));
				break;
				case ColumnGroup::Dignity:
					if ( ob >= PlanetId::Sun && ob <= PlanetId::Saturn )
					{
						table->setEntry( col++, row, lang.getDignityName( h->getVargaData( ob, Varga::Rasi, viewpoint ).getDignity(),
              propertyCfg.textFormat, propertyCfg.vedicSignNames ));
					}
				break;

				case ColumnGroup::WesternAspectarium:
					aspectariumStartCol = col;
					col += getGroupColumnCount( obs, ColumnGroup::WesternAspectarium );
				break;

				case ColumnGroup::Navamsa:
					table->setEntry( col++, row, MToken( h->getVargaData( ob, Varga::Navamsa, viewpoint ).getRasi(), propertyCfg ));
				break;
				case ColumnGroup::Nakshatra:
					table->setEntry( col++, row, lang.getNakshatra27Name( ::getNakshatra27( h->getLongitude( ob, viewpoint )),
            propertyCfg.nakshatraTextFormat ));
				break;
				case ColumnGroup::NakshatraPada:
					table->setEntry( col, row, std::format( "{}",
						(int)(getNakshatraLongitude27( h->getLongitude( ob, viewpoint )) / 3.3333333333 ) + 1 ));
					col++;
				break;
				case ColumnGroup::KpLords:
				{
					DasaExpert_ptr expert( DasaExpertFactory().getDasaExpert( DasaId::Vimsottari ));
					const KpData kp = h->getKPLords( ob, viewpoint );

					table->setEntry( col++, row, expert->getDasaLordName( kp.lord, planetCfg, propertyCfg ));
					if ( kpdepth > 1 ) table->setEntry( col++, row, expert->getDasaLordName( kp.sublord, planetCfg, propertyCfg ));
					if ( kpdepth > 1 ) table->setEntry( col++, row, expert->getDasaLordName( kp.subsublord, planetCfg, propertyCfg ));
				}
				break;
				case ColumnGroup::ShastiamsaLord:
				{
					const static bool k_shastiamsa_benefic[60] = {
						false, false, true, true, true, true, false, false, false, false,
						false, false, true, true, false, false, true, true, true, true,
						true, true, true, true, true, true, true, true, true, false,
						false, false, false, false, false, false, true, true, true, false,
						false, false, false, false, true, true, true, false, true, true,
						false, false, true, true, false, true, true, true, false, true,
					};

					int shast = (int)( a_red( h->getLongitude( ob, viewpoint ), 30 ) * 2 );
					if ( isEvenRasi( h->getLongitude( ob, viewpoint ) )) shast = 59 - shast;
					assert( shast >= 0 && shast < 60 );

					table->setEntry( col++, row, std::format( "{} ({})", lang.getShastiamsaName( shast ), ( k_shastiamsa_benefic[(int)shast] ? 'B' : 'M' )));
				}
				break;
				case ColumnGroup::DasaVarga:
					if ( ob <= PlanetId::Saturn )
					{
						const VedicPlanetData &pdata = h->getVedicPlanetData( ob, viewpoint );
						const int dignity = pdata.vimsopakaBalaGoodVargas[2];

							if ( dignity <= 1 )
								table->setEntry( col++, row, std::format( "{}", dignity ));
							else
								table->setEntry( col++, row, std::format( "{} ({})", lang.getVimsopakaDignityName( 2, dignity ), dignity ));
					}
				break;
				case ColumnGroup::CharaKaraka:
				{
					assert( jexpert );
					bool docontinue = false;
					PlanetId p = ob;
					if ( astroconfig->jaimini.charaKarakaMode ) // 0: Parasara, 1: Raman
					{
						if ( p > PlanetId::Saturn ) docontinue = true;
					}
					else // Parasara
					{
						if ( p == PlanetId::Uranus ) docontinue = true;
						if ( p == PlanetId::AscNode ) p = (PlanetId)7;
						if ( (int)p > 7 ) docontinue = true;
					}
					if ( ! docontinue ) table->setEntry( col++, row, lang.getKarakaName( jexpert->getCharaKarakaProperty( p )));
				}
				break;
				case ColumnGroup::RekhaPoints:
					assert( aexpert );
					if (( ob >= PlanetId::Sun && ob <= PlanetId::Saturn ) || ob == PlanetId::Ascendant )
					{
						table->setEntry( col++, row,
							std::format( "{}", aexpert->getItem( AvType::Rekha, ob, getRasi( h->getLongitude( ob, viewpoint )))));
					}
				break;
				default:
					errorlog << "invalid column id " << (int)colgroup.first;
					table->setEntry( col++, row, "unknown column type" );
				break;
			}
		}

		row++;
	}

	// Western aspectarium
	for( const auto& colgroup : propertyCfg.columnGroups )
	{
		if ( ! colgroup.second ) continue;

		if ( colgroup.first == ColumnGroup::WesternAspectarium )
		{
			WesternAspectExpert expert( props );
			expert.writeAspectariumTable( table, aspectariumStartCol, doPrintTableHeader ? 1 : 0, expert.calculate(), planetCfg );
		}
	}

	if ( jexpert ) delete jexpert;
	if ( aexpert ) delete aexpert;
}

/*****************************************************
**
**   TableWriter   ---   createPlanetListTable
**
******************************************************/
Table *TableWriter::createPlanetListTable( const Planets &obs, const PlanetConfig &planetCfg, const PlanetPropertyConfig &propertyCfg )
{
	const uint colcount = getPlanetListColumnCount( obs, propertyCfg );
	Table *table = new Table( colcount, obs.size() + 1 );
	writePlanetListTable( obs, planetCfg, propertyCfg, table, 0, 0 );

	return table;
}

/*****************************************************
**
**   TableWriter   ---   addBalanceItem
**
******************************************************/
void TableWriter::addBalanceItem( PlanetBalanceExpert &expert, Table *table, const uint &col, const uint &row,
	const std::variant<Element,SignQuality,ChartSide> &item )
{
	const std::pair<wxString, MString> p = expert.getItemValue( item );
	table->setEntry( col, row, MToken( p.first ));
	table->setEntry( col + 1, row, p.second );
}

/*****************************************************
**
**   TableWriter   ---   writeBaseDataAttributes
**
******************************************************/
void TableWriter::writeBaseDataAttributes( Table *table, std::list<BasedataAttribute> atts, const PlanetConfig &planetCfg,
	const uint colOffset, const int rowOffset, const bool doPrintTableHeader )
{
	assert( table );
	assert( table->getNbCols() >= colOffset + 2 );
	assert( table->getNbRows() >= atts.size() + rowOffset );

	// check if attribute list is empty. If so, replace by default list
	if ( atts.size() == 0 )
	{
		warnlog << "TableWriter::writeBaseDataAttributes has empty basedata list, replacing by default";
		atts = ViewpointHelper::get()->getDefaultBasedataConfigs( viewpoint );
	}

	uint row = rowOffset;

	table->format.col_alignment[colOffset] = Align::West;
	if ( doPrintTableHeader )
	{
		table->setHeaderEntry( colOffset, row++, tr( "Basedata" ), 2 );
		table->setAllHeaderRow( 0 );
	}

	const auto l = ViewpointHelper::get()->expandBasedataConfigList( atts );

	// Preparation: create PlanetBalanceExpert and update if necessary
	PlanetBalanceExpert expert( props, planetCfg );
	for( const BasedataAttribute &a : l )
	{
		if ( a >= BasedataAttribute::ElementFire && a <= BasedataAttribute::SignQualityDual )
		{
			expert.update();
			break;
		}
	}

	// Preparation: create HoraExpert and update if necessary
	HoraExpert horaExpert;
	for( const BasedataAttribute &a : l )
	{
		if ( a >= BasedataAttribute::HoraLord && a <= BasedataAttribute::VarshaLord )
		{
			horaExpert.location = h->dl.location;
			horaExpert.update( h->getJD() );
			break;
		}
	}

	for( const BasedataAttribute &att : l )
	{
		debuglog <<	"writeBaseDataAttributes att " << (int)att << " currentRow " << row;
		switch( att )
		{
			case BasedataAttribute::ElementFire:
				addBalanceItem( expert, table, colOffset, row, Element::Fire );
			break;
			case BasedataAttribute::ElementEarth:
				addBalanceItem( expert, table, colOffset, row, Element::Earth );
			break;
			case BasedataAttribute::ElementAir:
				addBalanceItem( expert, table, colOffset, row, Element::Air );
			break;
			case BasedataAttribute::ElementWater:
				addBalanceItem( expert, table, colOffset, row, Element::Water );
			break;
			case BasedataAttribute::ObjectsNorth:
				addBalanceItem( expert, table, colOffset, row, ChartSide::North );
			break;
			case BasedataAttribute::ObjectsWest:
				addBalanceItem( expert, table, colOffset, row, ChartSide::West );
			break;
			case BasedataAttribute::ObjectsSouth:
				addBalanceItem( expert, table, colOffset, row, ChartSide::South );
			break;
			case BasedataAttribute::ObjectsEast:
				addBalanceItem( expert, table, colOffset, row, ChartSide::East );
			break;
			case BasedataAttribute::SignQualityMovable:
				addBalanceItem( expert, table, colOffset, row, SignQuality::Movable );
			break;
			case BasedataAttribute::SignQualityFixed:
				addBalanceItem( expert, table, colOffset, row, SignQuality::Fixed );
			break;
			case BasedataAttribute::SignQualityDual:
				addBalanceItem( expert, table, colOffset, row, SignQuality::Dual );
			break;

			case BasedataAttribute::HoraLord:
				table->setEntry( colOffset, row, tr( "Hora Lord" ) );
				table->setEntry( colOffset + 1, row, MToken( horaExpert.getHoraLord(), viewpoint ));
			break;
			case BasedataAttribute::DinaLord:
				table->setEntry( colOffset, row, tr( "Dina Lord" ) );
				table->setEntry( colOffset + 1, row, MToken( horaExpert.getDinaLord(), viewpoint ));
			break;
			case BasedataAttribute::MasaLord:
				table->setEntry( colOffset, row, tr( "Masa Lord" ) );
				table->setEntry( colOffset + 1, row, MToken( horaExpert.getMasaLord(), viewpoint ));
			break;
			case BasedataAttribute::VarshaLord:
				table->setEntry( colOffset, row, tr( "Varsha Lord" ) );
				table->setEntry( colOffset + 1, row, MToken( horaExpert.getVarshaLord(), viewpoint ));
			break;

			default:
				writeCustomDataEntry( table, att, colOffset, row );
			break;
		}
		row++;
	}
}

/*****************************************************
**
**   TableWriter   ---   createBaseDataTable
**
******************************************************/
Table *TableWriter::createBaseDataTable( const std::list<BasedataAttribute> &atts )
{
	const auto l = ViewpointHelper::get()->expandBasedataConfigList( atts );
	Table *table = new Table( 2, l.size() );
	writeBaseDataAttributes( table, l, astroconfig->getViewpointConfig( viewpoint ).planets, 0, 0 );
	return table;
}

/*****************************************************
**
**   TableWriter   ---   writeCustomDataEntry
**
******************************************************/
void TableWriter::writeCustomDataEntry( Table *table, const BasedataAttribute &type, const uint &col, const uint &row )
{
	assert( table );
	Lang lang;
	debuglog << "writeCustomDataEntry type " << (int)type << " col " << col << " row " << row;

	switch( type )
	{
		case BasedataAttribute::Name:
			table->setEntry( col, row, tr( "Name" ) );
			table->setEntry( col + 1, row, h->dl.name );
		break;
		case BasedataAttribute::LocalDate:
			table->setEntry( col, row, tr( "Date" ) );
			table->setEntry( col + 1, row,
				DateFormatter::get()->formatDateOffset( h->getJD(), h->dl.location.tz + h->dl.location.dst, astroconfig->general.dateFormat ));
		break;
		case BasedataAttribute::LocalISODate:
			table->setEntry( col, row, tr( "Local ISO Date" ) );
			table->setEntry( col + 1, row,
				DateFormatter::get()->formatDateOffset( h->getJD(), h->dl.location.tz + h->dl.location.dst, DateFormat::ISO ));
		break;
		case BasedataAttribute::FullLocalDate:
			table->setEntry( col, row, tr( "Date" ) );
			table->setEntry( col + 1, row,
				DateFormatter::get()->formatDatetimeOffset( h->getJD(), h->dl.location.tz + h->dl.location.dst, astroconfig->general.dateFormat ));
		break;
		case BasedataAttribute::LocalTime:
			table->setEntry( col, row, tr( "Local Time" ) );
			table->setEntry( col + 1, row, Formatter::get()->formatTime(
				a_red( getTimeFromJD( h->getJD()) + h->dl.location.tz + h->dl.location.dst, 24 )) );
		break;

		case BasedataAttribute::Weekday:
			table->setEntry( col, row, tr( "Weekday" ) );
			table->setEntry( col + 1, row, lang.getWeekdayName( h->dl.getWeekDay() ));
		break;

		// Date and time stuff in UT
		case BasedataAttribute::UTDate:
			table->setEntry( col, row, tr( "UT Date" ) );
			table->setEntry( col + 1, row,
				DateFormatter::get()->formatUTDate( h->getJD() ));
		break;
		case BasedataAttribute::UTFullDate:
			table->setEntry( col, row, tr( "UT Date" ) );
			table->setEntry( col + 1, row,
				DateFormatter::get()->formatUTDatetime( h->getJD() ));
		break;
		case BasedataAttribute::UniversalTime:
			table->setEntry( col, row, tr( "Universal Time" ) );
			table->setEntry( col + 1, row, Formatter::get()->formatTime( getTimeFromJD( h->getJD() )));
		break;

		// Location based attributes
		case BasedataAttribute::LocationName:
			table->setEntry( col, row, tr( "Location" ) );
			table->setEntry( col + 1, row, h->dl.location.locName );
		break;
		case BasedataAttribute::LocationLongitude:
			table->setEntry( col, row, tr( "Longitude" ) );
			table->setEntry( col + 1, row, Formatter::get()->formatLongitude( h->dl.location.longitude ));
		break;

		case BasedataAttribute::LocationLatitude:
			table->setEntry( col, row, tr( "Latitude" ) );
			table->setEntry( col + 1, row, Formatter::get()->formatLatitude( h->dl.location.latitude ));
		break;
		case BasedataAttribute::Timezone:
			table->setEntry( col, row, tr( "Time Zone" ) );
			table->setEntry( col + 1, row, std::format( "{} {}", ( h->dl.location.tz >= 0 ? '+' : '-' ), Formatter::get()->formatTime( h->dl.location.tz )));
		break;
		case BasedataAttribute::DaylightSaving: 
		{
			table->setEntry( col, row, tr( "Daylight Saving" ) );
			table->setEntry( col + 1, row, std::format( "{:.1f}", h->dl.location.dst ));
		}
		break;
		case BasedataAttribute::SiderealTime:
			table->setEntry( col, row, tr( "Sidereal Time" ) );
			table->setEntry( col + 1, row, Formatter::get()->formatTime( h->getSiderealTime()));
		break;
		case BasedataAttribute::Eps:
			table->setEntry( col, row, tr( "Ecliptic Obliquity" ) );
			table->setEntry( col + 1, row, Formatter::get()->formatDegrees( h->getEps()));
		break;
		case BasedataAttribute::Armc:
			table->setEntry( col, row, tr( "ARMC" ) );
			table->setEntry( col + 1, row, Formatter::get()->formatDegrees( h->getArmc()));
		break;
		case BasedataAttribute::JulianDate:
			table->setEntry( col, row, tr( "Julian Date" ) );
			table->setEntry( col + 1, row, std::format( "{:14.5f}", h->getJD()));
		break;
		case BasedataAttribute::SunriseTime:
			table->setEntry( col, row, tr( "Sunrise" ));
			if ( h->getSunrise() != 0 )
			{
				table->setEntry( col + 1, row, Formatter::get()->formatTime(
					getTimeFromJD( h->getSunrise() + ( h->dl.location.tz + h->dl.location.dst ) / 24.0 )));
			}
			else table->setEntry( col + 1, row, tr( "n.a." ));
		break;
		case BasedataAttribute::SunsetTime:
			table->setEntry( col, row, tr( "Sunset" ) );
			if ( h->getSunset() != 0 )
			{
				table->setEntry( col + 1, row, Formatter::get()->formatTime(
					getTimeFromJD( h->getSunset() + ( h->dl.location.tz + h->dl.location.dst ) / 24.0 )));
			}
			else table->setEntry( col + 1, row, tr( "n.a." ) );
		break;

		case BasedataAttribute::SunSign:
			table->setEntry( col, row, tr( "Sun Sign" ) );
			table->setEntry( col + 1, row, MToken( getRasi( h->getLongitude( PlanetId::Sun, viewpoint )), viewpoint ));
		break;
		case BasedataAttribute::MoonSign:
			table->setEntry( col, row, tr( "Moon Sign" ) );
			table->setEntry( col + 1, row, MToken( getRasi( h->getLongitude( PlanetId::Moon, viewpoint )), viewpoint ));
		break;
		case BasedataAttribute::AscendantSign:
			table->setEntry( col, row, tr( "Ascendant Sign" ) );
			table->setEntry( col + 1, row, MToken( getRasi( h->getLongitude( PlanetId::Ascendant, viewpoint )),
				viewpoint ));
		break;

		case BasedataAttribute::AyanamsaName:
		{
			const CalculationConfig &config = astroconfig->getViewpointConfig( viewpoint ).calculation;
			table->setEntry( col, row, tr( "Ayanamsa" ));
			table->setEntry( col + 1, row, lang.getAyanamsaName( config.ayanamsa ));
		}
		break;
		case BasedataAttribute::AyanamsaValue:
			table->setEntry( col, row, tr( "Ayanamsa Value" ) );
			table->setEntry( col + 1, row, Formatter::get()->formatDegrees( h->getAyanamsa( viewpoint ), DegPrecision::Second ));
		break;
		case BasedataAttribute::AyanamsaNameAndValue:
		{
			const CalculationConfig &config = astroconfig->getViewpointConfig( viewpoint ).calculation;
			table->setEntry( col, row, tr( "Ayanamsa" ));
			table->setEntry( col + 1, row, 
				std::format( "{} \({})",
					lang.getAyanamsaName( config.ayanamsa ), 
					Formatter::get()->formatDegrees( h->getAyanamsa( viewpoint ), DegPrecision::Second )));
		}
		break;

		case BasedataAttribute::Varna:
			table->setEntry( col, row, tr( "Varna" ) );
			table->setEntry( col + 1, row, lang.getVarnaName( getVarna( h->getLongitude( PlanetId::Moon, viewpoint ))));
		break;
		case BasedataAttribute::Gana:
			table->setEntry( col, row, tr( "Gana" ) );
			table->setEntry( col + 1, row, lang.getGanaName( NakshatraExpert(props ).getGana( PlanetId::Moon )));
		break;
		case BasedataAttribute::Yoni:
		{
			const Yoni yoni = NakshatraExpert( props ).getYoni( PlanetId::Moon );
			table->setEntry( col, row, tr( "Yoni" ) );
			table->setEntry( col + 1, row, yoni.name );
		}
		break;
		case BasedataAttribute::Nadi:
		{
			table->setEntry( col, row, tr( "Nadi" ) );
			table->setEntry( col + 1, row, lang.getNadiName( NakshatraExpert( props ).getNadi( PlanetId::Moon )));
		}
		break;
		case BasedataAttribute::Rajju:
		{
			const Rajju rajju = NakshatraExpert( props ).getRajju(PlanetId::Moon );
			table->setEntry( col, row, tr( "Rajju" ) );
			table->setEntry( col + 1, row, lang.getRajjuName( rajju.aroha, rajju.type ));
		}
		break;
		case BasedataAttribute::MoonNakshatra:
		{
			table->setEntry( col, row, tr( "Birth Star" ) );
			table->setEntry( col + 1, row, lang.getNakshatra27Name( getNakshatra27( h->getLongitude( PlanetId::Moon, viewpoint )),
				astroconfig->getViewpointConfig( viewpoint ).planetProps.nakshatraTextFormat ));
		}
		break;
		case BasedataAttribute::MoonNakshatraPortion:
		{
			const std::pair<double, Nakshatra27> p = DasaTool::get()->getNakshatraPortion( h, PlanetId::Moon, viewpoint );
			table->setEntry( col, row, tr( "Nakshatra Portion" ) );
			table->setEntry( col + 1, row, std::format( "{:.1f}%", 100.0 * p.first ));
		}
		break;
		case BasedataAttribute::MoonNakshatraPada:
		{
			table->setEntry( col, row, tr( "Birth Pada" ) );
			table->setEntry( col + 1, row, std::format( "{}",
          (int)(getNakshatraLongitude27( h->getLongitude( PlanetId::Moon, viewpoint )) / 3.3333333333 ) + 1 ));
		}
		break;
		case BasedataAttribute::MoonNakshatraPadaPortion:
			table->setEntry( col, row, tr( "Pada Portion" ) );
			table->setEntry( col + 1, row, std::format( "{}%%", ( 100 * DasaTool::get()->getPadaPortion( h, PlanetId::Moon, viewpoint ))));
		break;
		case BasedataAttribute::MoonVimsottariLord:
		{
			DasaExpert_ptr expert( DasaExpertFactory().getDasaExpert( DasaId::Vimsottari ));
			const KpData kp = h->getKPLords( PlanetId::Moon, viewpoint );
			table->setEntry( col, row, tr( "Influence" ) );
			table->setEntry( col + 1, row, expert->getDasaLordName( kp.lord ));
		}
		break;
		case BasedataAttribute::MoonVimsottariInfluenceTime:
		{
			table->setEntry( col, row, tr( "Influence Time" ) );
			table->setEntry( col + 1, row, DasaTool::get()->getDasaInfluenceTimeAsString( props, DasaId::Vimsottari ));
		}
		break;

		case BasedataAttribute::SunNakshatra:
			table->setEntry( col, row, tr( "Sun Star" ) );
			table->setEntry( col + 1, row, lang.getNakshatra27Name( getNakshatra27(
				h->getLongitude( PlanetId::Sun, viewpoint )),
				astroconfig->getViewpointConfig( viewpoint ).planetProps.nakshatraTextFormat ));
		break;
		case BasedataAttribute::AscendantNakshatra:
			table->setEntry( col, row, tr( "Ascendant Star" ) );
			table->setEntry( col + 1, row, lang.getNakshatra27Name( getNakshatra27(
				h->getLongitude( PlanetId::Ascendant, viewpoint )),
				astroconfig->getViewpointConfig( viewpoint ).planetProps.nakshatraTextFormat ));
		break;
		case BasedataAttribute::HouseSystem:
			table->setEntry( col, row, tr( "House System" ) );
			table->setEntry( col + 1, row, resolveCamelNotation(
				astroconfig->getViewpointConfig( viewpoint ).calculation.houseSystem ));
		break;


		case BasedataAttribute::LunarTithi:
		{
			const double angle = red_deg( h->getLongitude( PlanetId::Moon, viewpoint )
				- h->getLongitude( PlanetId::Sun, viewpoint ));
			table->setEntry( col, row, tr( "Lunar Tithi" ));
			table->setEntry( col + 1, row, lang.getTithiName( (int)(angle / 12 )) );
		}
		break;

		default:
			table->setEntry( col, row, tr( "Error" ) );
			table->setEntry( col + 1, row, std::format( "wrong custom data entry id {}", enum_to_string( type )));
		break;
	}
}


