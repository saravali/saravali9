/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/PrintoutTextHelper.cpp
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

#include "PrintoutTextHelper.h"

#include "astroconfig.h"
#include "ContextProperties.h"
#include "Dasa.h"
#include "DasaTool.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "Horoscope.h"
#include "logger.h"
#include "mathbase.h"
#include "mstring.h"
#include "Sheet.h"
#include "SpWriter.h"
#include "Table.h"
#include "TableWriter.h"
#include "WesternAspect.h"

extern Astroconfig *astroconfig;

#define DEBUG_PRINTOUTTEXTHELPER

/*****************************************************
**
**   PrintoutTextHelper   ---   Constructor
**
******************************************************/
PrintoutTextHelper::PrintoutTextHelper( ContextProps_ptr props )
	: props( props )
{
	horoscope = props->getRadixHoroscope();
	assert( horoscope );
	assert( horoscope->getRole() == HoroscopeRole::Radix );
}

/*****************************************************
**
**   PrintoutTextHelper   ---   generateDocument
**
******************************************************/
MDocument_ptr  PrintoutTextHelper::generateDocument( const SheetSpec &sheetSpec )
{
#ifdef DEBUG_PRINTOUTTEXTHELPER
	debuglog << "Start PrintoutTextHelper::generateDocument name " << sheetSpec.name << " viewpoint " << sheetSpec.viewpoint
		<< " autoPageBreak " << std::boolalpha << sheetSpec.autoPageBreak;
#endif

	doc = MDocument_ptr( new MDocument );
	doc->autoPageBreak = sheetSpec.autoPageBreak;

	for( SheetItem *sheetitem : sheetSpec.items )
	{
		#ifdef DEBUG_PRINTOUTTEXTHELPER
		debuglog << "PrintoutTextHelper::generateDocument type " << sheetitem->type;
		#endif
		switch( sheetitem->type )
		{
			case SheetItemType::Text:
			{
				SheetItemText *titem = dynamic_cast<SheetItemText*>( sheetitem );
				assert( titem );
				const Align align = titem->subType == TextItemSubType::Header1 ? Align::Center : Align::West;
				doc->addItem( new DocItemText( titem->subType, replaceHoroscopeBasedVariables( titem->contents, horoscope ), align ));
			}
			break;
			case SheetItemType::VargaChartGrid:
			{
				SheetItemVargaChartGrid *grid = dynamic_cast<SheetItemVargaChartGrid*>( sheetitem );
				assert( grid );
				if ( grid->vargas.size() && grid->cols > 0 )
				{
					DocItemVargaChartGrid *docgrid = new DocItemVargaChartGrid( grid->viewpoint, grid->vargas );
					docgrid->cols = grid->cols;
					doc->addItem( docgrid );
				}
				else
				{
					doc->addLine( std::format( "Error: Wrong Varga chart grid with {} vargas and {} columns", grid->vargas.size(), grid->cols ));
				}
			}
			break;
			case SheetItemType::Chart:
			case SheetItemType::VargaChart:
			case SheetItemType::SarvatobhadraChart:
			case SheetItemType::UranianChart:
			case SheetItemType::WesternChart:
			{
					SheetItemChart *chartitem = dynamic_cast<SheetItemChart*>( sheetitem );
					assert( chartitem );

				DocItemChart *docitem = new DocItemChart;
				docitem->category = chartitem->category;
				docitem->viewpoint = chartitem->viewpoint;
				docitem->varga = chartitem->varga;
				docitem->division = chartitem->division;
				docitem->width = chartitem->width;

				doc->addItem( docitem );
			}
			break;
			case SheetItemType::Table:
			{
				SheetItemTableDefinition *tableitem = dynamic_cast<SheetItemTableDefinition*>( sheetitem );
				assert( tableitem );
				doc->addItem( writeGenericTable( tableitem ));
			}
			break;
			case SheetItemType::DasaSummary:
			{
				const SheetItemDasa *dasaitem = dynamic_cast<SheetItemDasa*>( sheetitem );
				assert( dasaitem );

				// trim sign names in case of Kalachakra, table gets too wide otherwise
				const TextFormat maxTextFormat = TextFormat::Medium;
				//const TextFormat maxTextFormat = DasaExpert::isPlanetDasa( dasaitem->dasaId ) ? TextFormat::Long : TextFormat::Medium;
				DasaTool::get()->writeReport( doc, dasaitem->reportType, dasaitem->dasaId, props, true, maxTextFormat );
			}
			break;

			case SheetItemType::Aspectarium:
			{
				WesternAspectExpert expert( props );
				const std::list<WesternAspectEvent> events = expert.calculate();

				PlanetConfig planetCfg( astroconfig->getViewpointConfig( props->getPreferredViewpoint() ).planets );
				trimTextFormat( planetCfg.textFormat, sheetitem->maxTextFormat );
				#ifdef DEBUG_PRINTOUTTEXTHELPER
				debuglog << "PrintoutTextHelper::generateDocument Aspectarium, maxTextFormat " << sheetitem->maxTextFormat;
				#endif
				doc->addItem( expert.createAspectariumTable( events, planetCfg, false ));
			}
			break;

			case SheetItemType::SPTable:
			{
				const SheetItemSPTable *sitem = dynamic_cast<SheetItemSPTable*>( sheetitem );
				assert( sitem );

				// TODO division ist immer 1 Termid immer Midpoint
				#ifdef DEBUG_PRINTOUTTEXTHELPER
				debuglog << "PrintoutTextHelper::generateDocument TODO division " << sitem->division << " TermId " << sitem->termId;
				#endif

				UranianSection section( astroconfig->getUranianSection( UranianContext::RadixMatches ));
				section.matchDivision = sitem->division;
				section.filter.clear();
				for ( const PlanetId p : props->getPlanets( sheetSpec.viewpoint ))
				{
					if ( ! PlanetHelper::isHouse( p )) section.filter.selectedPlanets.insert( p );
				}

				//section.termId = sitem->termId;
				SensitivePointWriter spwriter( props->getRadixHoroscope(), props, &section );
				spwriter.writeMatrix( doc, sitem->termId );
			}
			break;

			case SheetItemType::PageBreak:
			{
				const SheetItemPageBreak *pitem = dynamic_cast<SheetItemPageBreak*>( sheetitem );
				assert( pitem );
				doc->addItem( new DocItemPageBreak( pitem->orientation ));
			}
			break;

			default:
				warnlog << "PrintoutTextHelper::generateDocument: unsupported sheet item type " << sheetitem->type;
			break;
		}
	}
	return doc;
}

/*****************************************************
**
**   PrintoutTextHelper   ---   writeTitle
**
******************************************************/
void  PrintoutTextHelper::writeTitle()
{
	assert( horoscope );
	doc->addHeader1( horoscope->dl.name, Align::Center );
}

/*****************************************************
**
**   PrintoutTextHelper   ---   writeGenericTable
**
******************************************************/
Table *PrintoutTextHelper::writeGenericTable( SheetItemTableDefinition *item )
{
	assert( item );
#ifdef DEBUG_PRINTOUTTEXTHELPER
	debuglog << "Start PrintoutTextHelper::writeGenericTable expand " << std::boolalpha << item->expand
		<< " maxTextFormat " << item->maxTextFormat;
#endif

	PlanetConfig planetCfg( astroconfig->getViewpointConfig( props->getPreferredViewpoint() ).planets);
	trimTextFormat( planetCfg.textFormat, item->maxTextFormat );

	PlanetPropertyConfig propertyCfg( astroconfig->getViewpointConfig( props->getPreferredViewpoint() ).planetProps);
	trimTextFormat( propertyCfg.textFormat, item->maxTextFormat );
	trimTextFormat( propertyCfg.nakshatraTextFormat, item->maxTextFormat );
	propertyCfg.precision = std::min( propertyCfg.precision, item->maxDegPrecision );

	uint cols = 0;
	uint rows = 0;
	for( const auto &g : item->groups )
	{
		cols += g.getColumnCount();
		rows = std::max( rows, g.getRowCount());
	}

	TableWriter tw( props, horoscope );
	Table *table = new Table( cols, rows, item->header );
	table->format.expand = item->expand;
	table->format.showVerticalBorders = item->showVerticalBorders;
	table->format.showHorizontalBorders = item->showHorizontalBorders;
	table->format.showEmptyColumns = item->showEmptyColumns;

	uint currentCol = 0;
	for( const auto &g : item->groups )
	{
		uint planetCols = 0;

		#ifdef DEBUG_PRINTOUTTEXTHELPER
		debuglog << "PrintoutTextHelper::writeGenericTable new group basedata size " << g.basedata.size()
			<< " planets size " << g.planets.size() << " colgroup size " << g.columnGroups.size()
			<< " auto header " << std::boolalpha << g.autoHeader << " common header '" << g.commonHeader << "'";
		#endif

		// empty column
		if ( g.basedata.size() == 0 && g.columnGroups.size() == 0 )
		{
			currentCol++;
			continue;
		}

		// basedata
		uint row = 0;
		if ( g.basedata.size())
		{
			bool showAutoHeader = g.autoHeader;
			if ( g.commonHeader.size() > 0 )
			{
				table->setHeaderEntry( currentCol, row++, g.commonHeader, 2 );
				showAutoHeader = false;
			}
			tw.writeBaseDataAttributes( table, g.basedata, planetCfg, currentCol, row, showAutoHeader );
			row += g.basedata.size();
			if ( showAutoHeader ) row++;
		}

		// planets
		if ( g.planets.size() != 0 && g.columnGroups.size() != 0 )
		{
			int spanWidth = 0;

			// flat list of enum ColumnGroup must be mapped to pair including enabled switch
			for( auto &cg : propertyCfg.columnGroups )
			{
				const bool active  = ( std::find( g.columnGroups.begin(), g.columnGroups.end(), cg.first ) != g.columnGroups.end() );
				cg.second = active;
				if ( active ) spanWidth++;
			}

			bool showAutoHeader = g.autoHeader && g.basedata.size() == 0 ? true : false;
			if ( g.commonHeader.size() > 0 )
			{
				table->setHeaderEntry( currentCol, row++, g.commonHeader, spanWidth );
				showAutoHeader = false;
			}

			tw.writePlanetListTable( PlanetHelper::expandPlanets( g.planets ), planetCfg, propertyCfg, table, currentCol, row, showAutoHeader );
			planetCols += g.getColumnCount();
		}

		currentCol += std::max( 2u, planetCols );
	} // end for groups

	table->calculateSpans();
	return table;
}

/*****************************************************
**
**   PrintoutTextHelper   ---   replaceHoroscopeBasedVariables
**
******************************************************/
wxString PrintoutTextHelper::replaceHoroscopeBasedVariables( wxString s, Horoscope_ptr horoscope )
{
	if ( horoscope )
	{
		s.Replace( "$name" , horoscope->dl.name );
		s.Replace( "$date", DateFormatter::get()->formatDateOffset( horoscope->getJD(),
			horoscope->dl.location.tz + horoscope->dl.location.dst, astroconfig->general.dateFormat ));

		s.Replace( "$fulldate", DateFormatter::get()->formatDatetimeOffset( horoscope->getJD(),
			horoscope->dl.location.tz + horoscope->dl.location.dst, astroconfig->general.dateFormat ));
		s.Replace( "$location", horoscope->dl.location.locName );
	}
	return s;
}

