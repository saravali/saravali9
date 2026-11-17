/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Sheet.cpp
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

#include "Sheet.h"

#include "astroconfig.h"
#include "Viewpoint.h"

#ifdef CPP26
#include "JsonWorker.h"
#else
#include "enumserializer.h"
#include "JsonConverter.h"
#include "json/json.h"
#endif

extern Astroconfig *astroconfig;

#define DEBUG_SHEET

/*****************************************************
**
**   SheetItem   ---   Constructor
**
******************************************************/
SheetItem::SheetItem( const SheetItemType &t, const Viewpoint viewpoint )
	: type( t ),
	viewpoint( viewpoint )
{
	if ( this->viewpoint == Viewpoint::None )
	{
		this->viewpoint = astroconfig->general.defaultViewpoint;
	}
}

/*****************************************************
**
**   ColumnGroupDefinition   ---   getColumnCount
**
******************************************************/
uint ColumnGroupDefinition::getColumnCount() const
{
	uint cols = 0;

	if ( columnGroups.size())
	{
		for( auto &c : columnGroups )
		{
			switch( c )
			{
				case ColumnGroup::KpLords:
					cols +=3;
				break;

				case ColumnGroup::WesternAspectarium:
					cols += PlanetHelper::expandPlanets( planets ).size() - 1;
				break;

				default:
					cols++;
				break;
			}
		}
	}
	if ( basedata.size())
	{
		cols = std::max( 2u, cols );
	}

	return std::max( 1u, cols );
}

/*****************************************************
**
**   ColumnGroupDefinition   ---   getRowCount
**
******************************************************/
uint ColumnGroupDefinition::getRowCount() const
{
	uint rows = 0;

	if ( planets.size())
	{
		rows += PlanetHelper::expandPlanets( planets ).size();
	}
	rows += basedata.size();
	if ( autoHeader || ! commonHeader.IsEmpty() ) rows++;

	return std::max( 1u, rows );
}

/*****************************************************
**
**   SheetSpec   ---   Constructor
**
******************************************************/
SheetSpec::SheetSpec( const SheetSpec &a )
{
	name = a.name;
	description = a.description;
	viewpoint = a.viewpoint;
	autoPageBreak = a.autoPageBreak;

	for( SheetItem *item : a.items )
	{
		this->items.push_back( item->clone());
	}
}

/*****************************************************
**
**   SheetSpec   ---   Destructor
**
******************************************************/
SheetSpec::~SheetSpec()
{
	for( SheetItem* item : items ) { delete item; }
}

/*****************************************************
**
**   SheetSpec   ---   getSummary
**
******************************************************/
SheetSpecSummary SheetSpec::getSummary() const
{
	SheetSpecSummary summary;

	for ( const auto item : items )
	{
		summary.itemCount++;

		#ifdef DEBUG_SHEET
		debuglog << "SheetSpec::getSummary item " << item->type;
		#endif
		switch( item->type )
		{
			case SheetItemType::Table:
			{
				SheetItemTableDefinition *tableitem = dynamic_cast<SheetItemTableDefinition*>( item );
				assert( tableitem );
				for( const auto &g : tableitem->groups )
				{
					for( auto p : PlanetHelper::expandPlanets( { g.planets } ))
					{
						//addPlanet( p );
						if ( std::find( summary.planets.begin(), summary.planets.end(), p ) == summary.planets.end() )
						{
							#ifdef DEBUG_SHEET
							debuglog << "SheetSpec::getSummary INSERT " << p;
							#endif
							summary.planets.push_back( p );
						}
						else
						{
							#ifdef DEBUG_SHEET
							debuglog << "SheetSpec::getSummary has duplicate planet " << p;
							#endif
						}
					}
					if ( g.planets.size() != 0 ) summary.columnCount += g.columnGroups.size();;
					summary.basedataCount += g.basedata.size();;
				}
			}
			break;
			case SheetItemType::Chart:
			case SheetItemType::WesternChart:
			case SheetItemType::UranianChart:
			case SheetItemType::VargaChart:
			case SheetItemType::SarvatobhadraChart:
				#ifdef DEBUG_SHEET
				debuglog << "SheetSpec::getSummary found chart";
				#endif

				summary.chartCount++;
			break;
			case SheetItemType::VargaChartGrid:	
			{
				SheetItemVargaChartGrid *grid = dynamic_cast<SheetItemVargaChartGrid*>( item );
				assert( grid );
				summary.chartCount += grid->vargas.size();
			}
			break;
			default:
			break;
		}
	}
	summary.planets = PlanetHelper::expandPlanets( summary.planets );

	#ifdef DEBUG_SHEET
	for( auto p : summary.planets )
	{
		debuglog << " SheetSpec::getSummary planet: " << p;
	}
	debuglog << " SheetSpec::getSummary sheet has totally " << summary.planets.size() << " planets.";
	#endif
	return summary;
}

/*****************************************************
**
**   SheetSpec   ---   toJsonValue
**
******************************************************/
Json::Value SheetSpec::toJsonValue( JsonHandler& ) const
{
	// not implemented
	return Json::Value();
}

#if CPP26
/*****************************************************
**
**   SheetSpecLoader   ---   read_single_node
**
******************************************************/
void SheetSpecLoader::read_single_node( SheetSpec &cfg, const Json::Value &v, JsonParser &parser )
{
#ifdef DEBUG_SHEET
	wxString name = "unknown";
	wxString extends = "none";
	if ( v[ "name" ]) name = v[ "name" ].asString();
	if ( v[ "extends" ]) extends = v[ "extends" ].asString();

	debuglog << "Start SheetSpecLoader::read_single_node name " << name << " extends " << extends;
#endif

	if ( v[ "extends" ])
	{
		wxString baseconfig = v[ "extends" ].asString();
		bool found = false;

		for( const auto &conf : configs )
		{
			if ( conf.name == baseconfig )
			{
#ifdef DEBUG_SHEET
	debuglog << "SheetSpecLoader::read_single_node found extends: " << baseconfig;
#endif
				found = true;
				cfg.viewpoint = conf.viewpoint;
				cfg.autoPageBreak = conf.autoPageBreak;

				// copy available items of original config
				for( auto &item : conf.items )
				{
					cfg.items.push_back( item->clone());
				}
				break;
			}
		}

		if ( ! found )
		{
			const wxString s = "sheet config cannot extend config \"" + baseconfig + "\" (not found)";
			errorlog << s;
			cfg.items.push_back( new SheetItemText( s, TextItemSubType::Paragraph ));
		}
	}
	JsonWorker worker( parser );
	worker.jsonToObject( cfg, v );

#ifdef DEBUG_SHEET
	debuglog << "SheetSpecLoader::read_single_node name " << cfg.name << " autoPageBreak " << cfg.autoPageBreak;
#endif

	const Json::Value& vitems = v[ "items" ];
	if ( vitems && vitems.isArray())
	{
		SheetItem *item = nullptr;
		for ( auto child : vitems )
		{
			SheetItemType type = SheetItemType::None;
			if ( child[ "type" ] )
			{
				worker.jsonToObject( type, child[ "type" ] ); 
#ifdef DEBUG_SHEET
				debuglog << "SheetSpecLoader::read_single_node type " << type;
#endif
				switch( type )
				{
					case SheetItemType::Text:
						item = new SheetItemText;
						worker.jsonToObject( *static_cast<SheetItemText*>( item ), child );
					break;

					case SheetItemType::Table:
					{
						SheetItemTableDefinition *tableitem = new SheetItemTableDefinition;
						worker.jsonToObject( *tableitem, child );
						for( auto &g : tableitem->groups )
						{
							g.planets = PlanetHelper::expandPlanets( g.planets );
						}
						item = tableitem;
					}
					break;

					case SheetItemType::DasaSummary:
						item = new SheetItemDasa;
						worker.jsonToObject( *static_cast<SheetItemDasa*>( item ), child );
					break;

					case SheetItemType::VargaChartGrid:
						item = new SheetItemVargaChartGrid;
						worker.jsonToObject( *static_cast<SheetItemVargaChartGrid*>( item ), child );
					break;
					case SheetItemType::VargaChart:
						item = new SheetItemChart( WidgetCategory::VargaChart, Viewpoint::Vedic );
						worker.jsonToObject( *static_cast<SheetItemChart*>( item ), child );
					break;
					case SheetItemType::SarvatobhadraChart:
						item = new SheetItemChart( WidgetCategory::SarvatobhadraChart, Viewpoint::Vedic );
						worker.jsonToObject( *static_cast<SheetItemChart*>( item ), child );
					break;
					case SheetItemType::WesternChart:
						item = new SheetItemChart( WidgetCategory::WesternChart, Viewpoint::Western );
						worker.jsonToObject( *static_cast<SheetItemChart*>( item ), child );
					break;
					case SheetItemType::UranianChart:
						item = new SheetItemChart( WidgetCategory::UranianChart, Viewpoint::Uranian );
						worker.jsonToObject( *static_cast<SheetItemChart*>( item ), child );
					break;
					case SheetItemType::Aspectarium:
						item = new SheetItem( SheetItemType::Aspectarium );
						worker.jsonToObject( *item, child );
					break;

					case SheetItemType::SPTable:
						item = new SheetItemSPTable;
						worker.jsonToObject( *static_cast<SheetItemSPTable*>( item ), child );
					break;

					case SheetItemType::PageBreak:
						item = new SheetItemPageBreak;
						worker.jsonToObject( *static_cast<SheetItemPageBreak*>( item ), child );
					break;

					case SheetItemType::None:
					default:
					{
						mstringstream ms;
						ms << "Error SheetSpecLoader::read_single_node: Invalid attribute \"" << child[ "type" ].asString()
							<< "\". Allowed values are: " << reportEnumParams<SheetItemType>();
						errorlog << ms.str();
						item = new SheetItemText( ms.str(), TextItemSubType::Paragraph );
					}
					break;
				}
				assert( item );
				cfg.items.push_back( item );
			}
			else
			{
				const wxString s = "Attribut \"type\" is missing";
				errorlog << s;
				cfg.items.push_back( new SheetItemText( s, TextItemSubType::Paragraph ));
			}
		}
	}
#ifdef DEBUG_SHEET
	debuglog << "Finished SheetSpecLoader::read_single_node, name " << cfg.name << " sheet has " << cfg.items.size() << " items "
		<< " autoPageBreak " << cfg.autoPageBreak;
#endif
}

#else
/*****************************************************
**
**   SheetSpecLoader   ---   loadSingleConfig ALT 
**
******************************************************/
void SheetSpecLoader::loadSingleConfig( const Json::Value &v, JsonHandler &jh )
{
#ifdef DEBUG_SHEET
	debuglog << "Start SheetSpecLoader::loadSingleConfig";
#endif
	SheetSpec cfg;
	if ( v[ "extends" ])
	{
		wxString baseconfig = v[ "extends" ].asString();;
		bool found = false;

		for( const auto &conf : configs )
		{
			if ( conf.name == baseconfig )
			{
				found = true;
				cfg.viewpoint = conf.viewpoint;

				// copy available items of original config
				for( auto &item : conf.items )
				{
					cfg.items.push_back( item->clone());
				}
				break;
			}
		}
		if ( ! found )
		{
			const wxString s = "sheet config cannot extend config \"" + baseconfig + "\" (not found)";
			errorlog << s;
			cfg.items.push_back( new SheetItemText( s, TextItemSubType::Paragraph ));
		}
	}

	if ( v[ "name" ] ) extractFromJson( v[ "name" ], cfg.name, jh ); 
	if ( v[ "viewpoint" ] ) extractFromJson( v[ "viewpoint" ], cfg.viewpoint, jh ); 
	if ( v[ "description" ] ) extractFromJson( v[ "description" ], cfg.description, jh ); 
	if ( v[ "autoPageBreak" ] ) extractFromJson( v[ "autoPageBreak" ], cfg.autoPageBreak, jh ); 

	if ( v[ "items" ] ) addItemList( cfg.items, v, jh );
	if ( jh.hasErrors())
	{
		mstringstream ms;
		ms << "Error message from JsonHandler: " << std::endl;
		for( const JsonErrorItem &err : jh.getErrors ())
		{
			ms << "File " << jh.getFilename() << " type " << err.errorType
				<< " message; "<< err.message << std::endl;
		}
		cfg.items.push_back( new SheetItemText( ms.str(), TextItemSubType::Paragraph ));

		jh.reportErrors();
	}
	//configs.push_back( cfg );
	configs.push_back( std::move( cfg ));
}

/*****************************************************
**
**   SheetSpecLoader   ---   addItemList ALT
**
******************************************************/
void SheetSpecLoader::addItemList( std::list<SheetItem*>& items, const Json::Value &v, JsonHandler &jh )
{
#ifdef DEBUG_SHEET
	debuglog << "Start SheetSpecLoader::getItemList";
#endif

	if ( v[ "items" ] )
	{
		for ( auto child : v[ "items" ] )
		{
			items.push_back( createItem( child, jh ));
		}
	}
}

/*****************************************************
**
**   SheetSpecLoader   ---   createItem ALT
**
******************************************************/
SheetItem *SheetSpecLoader::createItem( const Json::Value &v, JsonHandler &jh )
{
	SheetItemType type = SheetItemType::None;
	if ( v[ "type" ] )
	{
		extractFromJson( v[ "type" ], type, jh ); 
		debuglog << "SheetSpecLoader::createItem type " << type;

		switch( type )
		{
			case SheetItemType::Text:
			{
				SheetItemText *cfg = new SheetItemText;
				if ( v[ "subType" ] ) extractFromJson( v[ "subType" ], cfg->subType, jh ); 
				if ( v[ "contents" ] ) extractFromJson( v[ "contents" ], cfg->contents, jh ); 
				return cfg;
			}
			break;
			case SheetItemType::DasaSummary:
			{
				SheetItemDasa *cfg = new SheetItemDasa;
				if ( v[ "reportType" ] ) extractFromJson( v[ "reportType" ], cfg->reportType, jh ); 
				if ( v[ "dasaId" ] ) extractFromJson( v[ "dasaId" ], cfg->dasaId, jh ); 
				if ( v[ "maxTextFormat" ] ) extractFromJson( v[ "maxTextFormat" ], cfg->maxTextFormat, jh ); 
				if ( v[ "viewpoint" ] )
					extractFromJson<Viewpoint>( v[ "viewpoint" ], cfg->viewpoint, jh ); 
				return cfg;
			}
			break;

			case SheetItemType::WesternChart:
			{
				SheetItemChart *cfg = new SheetItemChart( WidgetCategory::WesternChart );
				if ( v[ "viewpoint" ] ) extractFromJson( v[ "viewpoint" ], cfg->viewpoint, jh ); 
				if ( v[ "width" ] ) extractFromJson( v[ "width" ], cfg->width, jh ); 
				return cfg;
			}
			break;
			case SheetItemType::UranianChart:
			{
				SheetItemChart *cfg = new SheetItemChart( WidgetCategory::UranianChart );
				if ( v[ "viewpoint" ] ) extractFromJson( v[ "viewpoint" ], cfg->viewpoint, jh ); 
				if ( v[ "width" ] ) extractFromJson( v[ "width" ], cfg->width, jh ); 
				if ( v[ "division" ] ) extractFromJson( v[ "division" ], cfg->division, jh ); 
				return cfg;
			}
			break;
			case SheetItemType::VargaChart:
			{
				SheetItemChart *cfg = new SheetItemChart( WidgetCategory::VargaChart );
				if ( v[ "viewpoint" ] ) extractFromJson( v[ "viewpoint" ], cfg->viewpoint, jh ); 
				if ( v[ "width" ] ) extractFromJson( v[ "width" ], cfg->width, jh ); 
				if ( v[ "varga" ] ) extractFromJson<Varga>( v[ "varga" ], cfg->varga, jh ); 
				//if ( v[ "width" ] ) extractFromJson( v[ "width" ], cfg->width, jh ); 
				return cfg;
			}
			break;
			case SheetItemType::SarvatobhadraChart:
			{
				SheetItemChart *cfg = new SheetItemChart( WidgetCategory::SarvatobhadraChart );
				if ( v[ "viewpoint" ] ) extractFromJson( v[ "viewpoint" ], cfg->viewpoint, jh ); 
				if ( v[ "width" ] ) extractFromJson( v[ "width" ], cfg->width, jh ); 
				return cfg;
			}
			break;
			case SheetItemType::VargaChartGrid:
			{

				std::list<Varga> vargas;
				if ( v[ "vargas" ] ) extractFromJson( v[ "vargas" ], vargas, jh ); 
				if ( vargas.size() == 0 )
				{
					return new SheetItemText( "Varga list must not be empty", TextItemSubType::Paragraph );
				}
				SheetItemVargaChartGrid *cfg = new SheetItemVargaChartGrid( Viewpoint::Vedic );
				cfg->vargas = vargas;
				if ( v[ "viewpoint" ] ) extractFromJson( v[ "viewpoint" ], cfg->viewpoint, jh ); 
				if ( v[ "cols" ] ) extractFromJson( v[ "cols" ], cfg->cols, jh ); 
				if ( cfg->cols < 1 || cfg->cols > 6 )
				{
					warnlog << "wrong value for SheetItemVargaChartGrid::cols " << cfg->cols << " replacing by default.";
					cfg->cols = 2;
				}
				return cfg;
			}
			break;

			case SheetItemType::Table:
			{
				SheetItemTableDefinition *cfg = new SheetItemTableDefinition;
				if ( v[ "groups" ] ) extractFromJson( v[ "groups" ], cfg->groups, jh ); 
				if ( v[ "expand" ] ) extractFromJson( v[ "expand" ], cfg->expand, jh ); 
				if ( v[ "showVerticalBorders" ] ) extractFromJson( v[ "showVerticalBorders" ], cfg->showVerticalBorders, jh ); 
				if ( v[ "showHorizontalBorders" ] ) extractFromJson( v[ "showHorizontalBorders" ], cfg->showHorizontalBorders, jh ); 
				if ( v[ "showEmptyColumns" ] ) extractFromJson( v[ "showEmptyColumns" ], cfg->showEmptyColumns, jh ); 
				if ( v[ "maxTextFormat" ] ) extractFromJson( v[ "maxTextFormat" ], cfg->maxTextFormat, jh ); 

				for( auto &g : cfg->groups )
				{
					g.planets = PlanetHelper::expandPlanets( g.planets );
				}
				return cfg;
			}
			break;

			case SheetItemType::Aspectarium:
			{
				SheetItem *cfg = new SheetItem( SheetItemType::Aspectarium );
				if ( v[ "maxTextFormat" ] ) extractFromJson( v[ "maxTextFormat" ], cfg->maxTextFormat, jh ); 
				return cfg;
			}
			break;

			case SheetItemType::SPTable:
			{
				SheetItemSPTable *cfg = new SheetItemSPTable;
				if ( v[ "division" ] ) extractFromJson( v[ "division" ], cfg->division, jh ); 
				if ( v[ "termId" ] ) extractFromJson( v[ "termId" ], cfg->termId, jh ); 
				return cfg;
			}
			break;

			case SheetItemType::PageBreak:
			{
				SheetItemPageBreak *cfg = new SheetItemPageBreak;
				if ( v[ "orientation" ] ) extractFromJson( v[ "orientation" ], cfg->orientation, jh ); 
				return cfg;
			}
			break;
			case SheetItemType::None:
			default:
			{
				mstringstream ms;
				ms << "Invalid attribute \"" << type << "\" (" << v[ "type" ].asString() << ") . Allowed values are: " << reportEnumParams<SheetItemType>();
				errorlog << ms.str();
				jh.addReadValueError( v, ms.str());
				//return new SheetItemText( ms.str(), TextItemSubType::Paragraph );
				return new SheetItemText( "Error", TextItemSubType::Paragraph );
			}
			break;
		}
	}
	else
	{
		const wxString s = "Attribut \"type\" is missing";
		errorlog << s;
		jh.addReadValueError( v, s );
		return new SheetItemText( s, TextItemSubType::Paragraph );
	}
}

#endif

/*****************************************************
**
**   SheetSpecLoader   ---   loadDefaultConfigs
**
******************************************************/
void SheetSpecLoader::loadDefaultConfigs()
{
	debuglog << "Start loadDefaultConfigs";

	// Vedic Default
	{
		SheetSpec spec;
		spec.name = "Vedic Default";
		spec.description = "Default Vedic printout with Rasi, Navamsa and Vimsottari Dasa in compact style";
		spec.viewpoint = Viewpoint::Vedic;

		spec.items.push_back( new SheetItemText( "$name" ));

		// table with basedata on the left, planets on the right side
		SheetItemTableDefinition *table = new SheetItemTableDefinition;

		ColumnGroupDefinition def1;
		def1.commonHeader = "Basedata";
		def1.basedata = {
			BasedataAttribute::Name,
			BasedataAttribute::LocalDate,
			BasedataAttribute::LocalTime,
			BasedataAttribute::LocationName,
			BasedataAttribute::LocationLongitude,
			BasedataAttribute::LocationLatitude,
			BasedataAttribute::AyanamsaNameAndValue,
			BasedataAttribute::Varna,
			BasedataAttribute::Yoni,
			BasedataAttribute::Nadi
		};
		table->groups.push_back( def1 );

		ColumnGroupDefinition def2;
		def2.planets = 
		{
			PlanetId::InnerVedic,
			PlanetId::Rahu,
			PlanetId::Ketu,
			PlanetId::Ascendant,
		};

		// empty column
		table->groups.push_back( ColumnGroupDefinition() );

		def2.columnGroups =
		{
			ColumnGroup::ObjectName,
			ColumnGroup::Longitude,
			ColumnGroup::Nakshatra,
			ColumnGroup::CharaKaraka
		};

		table->groups.push_back( def2 );
		spec.items.push_back( table );


		SheetItemVargaChartGrid *gridItem = new SheetItemVargaChartGrid;
		gridItem->vargas.push_back( Varga::Rasi );
		gridItem->vargas.push_back( Varga::Navamsa );
		spec.items.push_back( gridItem );

		SheetItemDasa *dasaItem = new SheetItemDasa( DasaReportType::Compact, DasaId::Vimsottari );
		spec.items.push_back( dasaItem );

		configs.push_back( std::move( spec ));
	}

	// Western Default
	{
		SheetSpec spec;
		spec.name = "Western Default";
		spec.description = "Default Western printout with list of planets and western chart";
		spec.viewpoint = Viewpoint::Western;

		spec.items.push_back( new SheetItemText( "$name" ));

		// upper table with base data
		SheetItemTableDefinition *table1 = new SheetItemTableDefinition;

		ColumnGroupDefinition def11;
		def11.basedata = {
			BasedataAttribute::Name,
			BasedataAttribute::LocalDate,
			BasedataAttribute::LocalTime,
			BasedataAttribute::UniversalTime,
			BasedataAttribute::Timezone,
			BasedataAttribute::DaylightSaving
		};
		table1->groups.push_back( def11 );

		// empty column
		table1->groups.push_back( ColumnGroupDefinition() );

		ColumnGroupDefinition def12;
		def12.basedata = {
			BasedataAttribute::LocationName,
			BasedataAttribute::LocationLongitude,
			BasedataAttribute::LocationLatitude,
			BasedataAttribute::SiderealTime,
			BasedataAttribute::HouseSystem
		};
		table1->groups.push_back( def12 );
		spec.items.push_back( table1 );

		SheetItemChart *chartItem = new SheetItemChart( WidgetCategory::WesternChart );
		spec.items.push_back( chartItem );

		// lower table with planetary positions
		SheetItemTableDefinition *table2 = new SheetItemTableDefinition;

		// standard planets up to Jupiter
		ColumnGroupDefinition def21;
		def21.autoHeader = false;
		def21.planets = 
		{
			PlanetId::Inner,
		};
		def21.columnGroups =
		{
			ColumnGroup::ObjectName,
			ColumnGroup::Longitude,
			ColumnGroup::HousePosition
		};
		table2->groups.push_back( def21 );

		// empty column
		table2->groups.push_back( ColumnGroupDefinition() );

		// standard planets: Saturn to Node
		ColumnGroupDefinition def22;
		def22.autoHeader = false;
		def22.planets = 
		{
			PlanetId::Outer,
			PlanetId::AscNode
		};
		def22.columnGroups =
		{
			ColumnGroup::ObjectName,
			ColumnGroup::Longitude,
			ColumnGroup::HousePosition
		};
		table2->groups.push_back( def22 );

		// empty column
		table2->groups.push_back( ColumnGroupDefinition() );

		// angles and house cusps
		ColumnGroupDefinition def23;
		def23.autoHeader = false;
		def23.planets =
		{
			PlanetId::Meridian,
			PlanetId::Ascendant,
			PlanetId::House2,
			PlanetId::House3,
			PlanetId::House11,
			PlanetId::House12,
		};
		def23.columnGroups =
		{
			ColumnGroup::ObjectName,
			ColumnGroup::Longitude,
		};
		table2->groups.push_back( def23 );

		spec.items.push_back( table2 );
		configs.push_back( std::move( spec ));
	}

	// Uranian Default
	{
		SheetSpec spec;
		spec.name = "Uranian Default";
		spec.description = "Uranian printout with 8 Transuranian planets and 360° chart";
		spec.viewpoint = Viewpoint::Uranian;

		spec.items.push_back( new SheetItemText( "$name" ));

		// upper table with base data
		SheetItemTableDefinition *table1 = new SheetItemTableDefinition;
		table1->expand = true;

		ColumnGroupDefinition def11;
		def11.basedata = {
			BasedataAttribute::Name,
			BasedataAttribute::LocalDate,
			BasedataAttribute::LocalTime,
			BasedataAttribute::UniversalTime,
		BasedataAttribute::Timezone,
		BasedataAttribute::DaylightSaving
		};
		table1->groups.push_back( def11 );

		// empty column
		table1->groups.push_back( ColumnGroupDefinition() );

		ColumnGroupDefinition def12;
		def12.basedata = {
			BasedataAttribute::LocationName,
			BasedataAttribute::LocationLongitude,
			BasedataAttribute::LocationLatitude,
			BasedataAttribute::SiderealTime,
			BasedataAttribute::HouseSystem
		};
		table1->groups.push_back( def12 );
		spec.items.push_back( table1 );

		SheetItemChart *chartItem = new SheetItemChart( WidgetCategory::UranianChart );
		spec.items.push_back( chartItem );

		// lower table with planetary positions
		SheetItemTableDefinition *table2 = new SheetItemTableDefinition;
		table2->expand = true;

		// standard planets
		ColumnGroupDefinition def21;
		def21.autoHeader = false;
		def21.planets = 
		{
			PlanetId::Inner,
			PlanetId::Outer,
		};
		def21.columnGroups =
		{
			ColumnGroup::ObjectName,
			ColumnGroup::Longitude,
			ColumnGroup::HousePosition
		};
		table2->groups.push_back( def21 );

		// empty column
		table2->groups.push_back( ColumnGroupDefinition() );

		// Transuranian planets
		ColumnGroupDefinition def22;
		def22.autoHeader = false;
		def22.planets = 
		{
			PlanetId::AscNode,
			PlanetId::InnerTNP,
			PlanetId::OuterTNP,
		};
		def22.columnGroups =
		{
			ColumnGroup::ObjectName,
			ColumnGroup::Longitude,
			ColumnGroup::HousePosition
		};
		table2->groups.push_back( def22 );

		// empty column
		table2->groups.push_back( ColumnGroupDefinition() );

		// angles and house cusps
		ColumnGroupDefinition def23;
		def23.autoHeader = false;
		def23.planets =
		{
			PlanetId::Meridian,
			PlanetId::Ascendant,
			PlanetId::House1,
			PlanetId::House2,
			PlanetId::House3,
			PlanetId::House11,
			PlanetId::House12,
		};
		def23.columnGroups =
		{
			ColumnGroup::ObjectName,
			ColumnGroup::Longitude,
		};
		table2->groups.push_back( def23 );

		spec.items.push_back( table2 );
		configs.push_back( std::move( spec ));
	}
}

