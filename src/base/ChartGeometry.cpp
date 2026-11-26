/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ChartGeometry.cpp
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

#include "ChartGeometry.h"

#include "ChartConfig.h"
#include "logger.h"

#ifdef CPP26
#include "JsonWorker.h"
#else
#include "enumserializer.h"
#include "JsonConverter.h"
#include "json/json.h"
#endif

#include <format>

#define DEBUG_CHARTGEOMETRY

/*****************************************************
**
**   ChartConfig   ---   Constructor
**
******************************************************/
ChartConfig::ChartConfig( const WidgetCategory category )
{
	switch( category )
	{
		case WidgetCategory::WesternChart:
			textFormat = TextFormat::Symbol;
			zoomFactor = 120;
			highlightOnMouseOver = true;
		break;

		case WidgetCategory::UranianChart:
			textFormat = TextFormat::Symbol;
			zoomFactor = 120;
		break;

		case WidgetCategory::VargaChart:
			retroStyle = RetroDisplayStyle::SimpleR;
		break;

		case WidgetCategory::SarvatobhadraChart:
			retroStyle = RetroDisplayStyle::SimpleR;
		break;

		default:
		break;
	}
}

/*****************************************************
**
**   ChartGeometry   ---   Constructor
**
******************************************************/
ChartGeometry::ChartGeometry( wxString n, ChartShape geometry )
{
	name = n;
	geometryType = geometry;
}

/*****************************************************
**
**   ChartGeometry   ---   COPY Constructor
**
******************************************************/
ChartGeometry::ChartGeometry( const ChartGeometry &tocopy )
{
	this->name = tocopy.name;
	this->geometryType = tocopy.geometryType;
	this->matchingCategories = tocopy.matchingCategories;

	for( const auto &e: tocopy.elements )
	{
		this->elements[e.first] = e.second->clone();
	}

	//static int i = 0;
	//debuglog << "COPY ChartGeometry " << ++i << " name " << name << " MAP size " << elements.size() ;
}

/*****************************************************
**
**   ChartGeometry   ---   MOVE Constructor
**
******************************************************/
#if 0
ChartGeometry::ChartGeometry( ChartGeometry &&orig )
{
	this->name = orig.name;
	this->geometryType = orig.geometryType;
	this->matchingCategories = orig.matchingCategories;

	this->elements = orig.elements;
	orig.elements.clear();
}
#endif

/*****************************************************
**
**   ChartGeometry   ---   Destructor
**
******************************************************/
ChartGeometry::~ChartGeometry()
{
	removeElements();
}

/*****************************************************
**
**   ChartGeometry   ---   removeElements()
**
******************************************************/
void ChartGeometry::removeElements()
{
	for( auto &e : elements ) delete e.second;
	elements.clear();
}

/*****************************************************
**
**   ChartGeometry   ---   containsElementType
**
******************************************************/
bool ChartGeometry::containsElementType( const CgElementType &type ) const
{
	for( auto &elem : elements )
	{
		if ( elem.second->getElementType() == type ) return true;
	}
	return false;
}

/*****************************************************
**
**   ChartGeometry   ---   updateMatchingCategories
**
******************************************************/
void ChartGeometry::updateMatchingCategories()
{
	matchingCategories.clear();

	for( const auto &cat : { WidgetCategory::UniversalChart,
      WidgetCategory::WesternChart,
      WidgetCategory::UranianChart,
      WidgetCategory::VargaChart,
      WidgetCategory::SarvatobhadraChart } )
	{
		if ( findWidgetCategory( cat )) matchingCategories.insert( cat );
	}

#if 1
	for( const auto cat : matchingCategories )
	{
		debuglog << "ChartGeometry::updateMatchingCategories " << name << " has category " << cat;
	}
#endif
}

/*****************************************************
**
**   ChartGeometry   ---   findWidgetCategory
**
******************************************************/
bool ChartGeometry::findWidgetCategory( const WidgetCategory &category ) const
{
#ifdef DEBUG_CHARTGEOMETRY
	debuglog << "ChartGeometry::findWidgetCategory name " << name << " category " << category << " elements size " << elements.size();
#endif
	switch( category )
	{
		case WidgetCategory::UniversalChart:
			return true;
		break;
		case WidgetCategory::None:
			return false;
		break;
		default:
		break;
	}

	for( auto &elem : elements )
	{
		assert( elem.second );
		const CgElementType type = elem.second->getElementType();
		switch( category )
		{
			case WidgetCategory::VargaChart:
				if ( type == CgElementType::EastGrid
				|| type == CgElementType::SouthGrid
				|| type == CgElementType::DiamondGrid ) return true;
			break;
			case WidgetCategory::SarvatobhadraChart:
				if ( type == CgElementType::SbcGrid) return true;
			break;
			case WidgetCategory::WesternChart:
				if ( type == CgElementType::Signs
					// TODO warum crash wenn man das einkommentiert
					//|| type ==  CgElementType::Houses
					|| type ==  CgElementType::Nakshatras
					|| type ==  CgElementType::DasaLords
					|| type ==  CgElementType::Aspects
					|| type ==  CgElementType::Angles
					|| type ==  CgElementType::WesternPlanetRing
				) return true;
			break;
			case WidgetCategory::UranianChart:
					if ( type ==  CgElementType::UranianDisk
					|| type ==  CgElementType::UranianAspects
				) return true;
			break;
			default:
			break;
		}
	}
	return false;
}

/*****************************************************
**
**   ChartGeometryLoader   ---   Constructor
**
******************************************************/
ChartGeometryLoader::ChartGeometryLoader()
 : ConfigLoader<ChartGeometry>( ConfigResource::Geom )
{
}

/*****************************************************
**
**   ChartGeometryLoader   ---   loadDefaultConfigs
**
******************************************************/
void ChartGeometryLoader::loadDefaultConfigs()
{
	auto addDefault = [ this ]( auto &m, const CgElementType &t ){
		m[t] = createDefaultElement( t );
	};

	{
		ChartGeometry geom( "Default Western Chart", ChartShape::Circle );
		addDefault( geom.elements, CgElementType::Signs );
		addDefault( geom.elements, CgElementType::Houses );
		addDefault( geom.elements, CgElementType::Angles );
		addDefault( geom.elements, CgElementType::WesternPlanetRing );
		addDefault( geom.elements, CgElementType::Aspects );
		geom.updateMatchingCategories();
		configs.push_back( std::move( geom ));
	}
	{
		ChartGeometry geom( "Default Uranian Chart", ChartShape::Circle );
		CgElement *disk = new CgUranianDisk;
		disk->brush = MBrush( "#dedede" );
		geom.elements[ CgElementType::UranianDisk ] = disk;
		addDefault( geom.elements, CgElementType::UranianPlanetRing );
		addDefault( geom.elements, CgElementType::UranianAspects );
		geom.updateMatchingCategories();
		configs.push_back( std::move( geom ));
	}
	{
		ChartGeometry geom( "Default South Indian Grid", ChartShape::Rectangle );
		addDefault( geom.elements, CgElementType::SouthGrid );
		geom.matchingCategories = { WidgetCategory::UniversalChart, WidgetCategory::VargaChart };
		configs.push_back( std::move( geom ));
	}
	{
		ChartGeometry geom( "Default East Indian Grid", ChartShape::Rectangle );
		addDefault( geom.elements, CgElementType::EastGrid );
		geom.updateMatchingCategories();
		configs.push_back( std::move( geom ));
	}
	{
		ChartGeometry geom( "Default North Indian/Diamond Grid", ChartShape::Rectangle );
		addDefault( geom.elements, CgElementType::DiamondGrid );
		geom.updateMatchingCategories();
		configs.push_back( std::move( geom ));
	}
	{
		ChartGeometry geom( "Default Sarvatobhadra Chart", ChartShape::Rectangle );
		addDefault( geom.elements, CgElementType::SbcGrid );
		geom.updateMatchingCategories();
		configs.push_back( std::move( geom ));
	}
#ifdef DEBUG_CHARTGEOMETRY
	debuglog << "ChartGeometryLoader::loadDefaults " << configs.size() << " elements";
#endif
}

/*****************************************************
**
**   ChartGeometryLoader   ---   createDefaultElement
**
******************************************************/
CgElement *ChartGeometryLoader::createDefaultElement( const CgElementType &type )
{
#ifdef DEBUG_CHARTGEOMETRY
	debuglog << "ChartGeometryLoader::createDefaultElement type " << (int)type;
#endif
	switch( type )
	{
		case CgElementType::Disk:
			return new CgDisk;
		break;

		case CgElementType::Signs:
			return  new CgLabelledAnnulus( CgElementType::Signs, 92, 79, 86 );
		break;

		case CgElementType::Houses:
		{
			CgLabelledAnnulus *t = new CgLabelledAnnulus( CgElementType::Houses, 79, 50, 55 );
			t->useSymbols = false;
			return t;
		}
		break;

		case CgElementType::Aspects:
			return new CgDisk( CgElementType::Aspects, 50 );
		break;

		case CgElementType::UranianPlanetRing:
			return new CgPlanetRing( CgElementType::UranianPlanetRing, 84, /* unused*/ 100, 95 );
		break;
		case CgElementType::WesternPlanetRing:
			return new CgPlanetRing( CgElementType::WesternPlanetRing, 92, 79, 70 );
		break;

		case CgElementType::UranianAspects:
			return new CgDisk( CgElementType::UranianAspects, 84 );
		break;

		case CgElementType::UranianDisk:
			return new CgUranianDisk;
		break;

		case CgElementType::Angles:
		{
			CgLabelledAnnulus *t = new CgPlanetRing( CgElementType::Angles, 100, 50, 95 );
			t->labelSize  = 150;
			t->showOuterFrame = false;
			t->showInnerFrame = false;
			return t;
		}
		break;

		case CgElementType::Image:
			return new CgInlayImage;
		break;

		case CgElementType::DasaLords:
			return new CgLabelledAnnulus( CgElementType::DasaLords, 93, 83 );
		break;

		case CgElementType::Nakshatras:
			return new CgLabelledAnnulus( CgElementType::Nakshatras, 80, 70 );
		break;

		case CgElementType::EastGrid:
			return new CgVedicChartGrid( CgElementType::EastGrid );
		break;

		case CgElementType::SouthGrid:
			return new CgVedicChartGrid( CgElementType::SouthGrid );
		break;

		case CgElementType::DiamondGrid:
			return new CgVedicChartGrid( CgElementType::DiamondGrid );
		break;

		case CgElementType::SbcGrid:
		{
			CgVedicChartGrid *grid = new CgVedicChartGrid( CgElementType::SbcGrid, 95.0 );
			grid->showFieldNames = true;
			return new CgVedicChartGrid( CgElementType::SbcGrid, 95.0 );
		}
		break;

		default:
			assert( false );
		break;
	}
	assert( false );
	return nullptr;
}

/*****************************************************
**
**   ChartGeometryLoader   ---   getNameAndDescription
**
******************************************************/
std::pair<wxString,wxString> ChartGeometryLoader::getNameAndDescription( const uint &g )
{
	//debuglog << "ChartGeometryLoader::getNameAndDescription " << g;
	if ( g == UINT_FOR_NOT_FOUND )
	{
		return std::pair<wxString,wxString>( "Error", "not found" );
	}
	else
	{
		const ChartGeometry &geom = getConfig( g );
		return std::pair<wxString,wxString>( geom.name, std::format( "{}", g ));
	}
}

#ifdef CPP26
/*****************************************************
**
**   ChartGeometryLoader   ---   read_single_node (virtual, overriden from ConfigListLoader)
**
******************************************************/
void ChartGeometryLoader::read_single_node( ChartGeometry &geom, const Json::Value &v, JsonParser &parser )
{
#ifdef DEBUG_CHARTGEOMETRY
	debuglog << "Start ChartGeometryLoader::read_single_node";
#endif
	JsonWorker worker( parser );

	if ( v[ "extends" ])
	{
		wxString baseconfig = v[ "extends" ].asString();;
		bool found = false;

#ifdef DEBUG_CHARTGEOMETRY
		debuglog << "Start ChartGeometryLoader::load_single_config_item found extends " << baseconfig;
#endif
		for( const auto &conf : configs )
		{
			if ( conf.name == baseconfig )
			{
#ifdef DEBUG_CHARTGEOMETRY
				debuglog << "Start ChartGeometryLoader::load_single_config_item found extends " << baseconfig << " CONF " << conf.name;
#endif
				found = true;
				geom.name = conf.name;
				geom.geometryType = conf.geometryType;

				// available elements of original geom must be copied
				for( auto &item : conf.elements )
				{
					if ( geom.elements.find( item.first ) != geom.elements.end()) delete geom.elements[ item.first ];
					geom.elements[item.first] = item.second->clone();
				}
				break;
			}
		}
		if ( ! found )
		{
			const wxString s = "chart config cannot extend config \"" + baseconfig + "\" (not found)";
			errorlog << s;
			geom.elements[ CgElementType::Error ] = new CgError( s );
		}
	}

	// generic load
	worker.jsonToObject( geom, v );

	if ( ! is_valid_enum( geom.geometryType ))
	{
		const wxString s = "invalid geometry \"" + v[ "geom" ].asString() + "\"";
		errorlog << s;

		geom.geometryType = ChartShape::Circle;
		geom.removeElements();
		geom.elements[ CgElementType::Error ] = new CgError( s );
	}

	// load elements
	for( auto velem : v )
	{
		for( const auto &type : Enum<CgElementType>() )
		{
			const wxString tname = enum_to_string( type );
			if ( v[ tname ] )
			{
				const Json::Value &key = v[ tname ];
#ifdef DEBUG_CHARTGEOMETRY
				debuglog << "JsonWorker::read_single_node name " << geom.name << " has element " << tname;
#endif

				CgElement *elem = nullptr;
				if ( geom.elements.find( type ) != geom.elements.end()) 
				{
					elem = geom.elements[ type ];
				}
				else
				{
					elem = ChartGeometryLoader::get()->createDefaultElement( type );
				}
				assert( elem );

				switch( type )
				{
					case CgElementType::Disk:
					case CgElementType::Aspects:
					case CgElementType::UranianAspects:
						worker.jsonToObject( *static_cast<CgDisk*>( elem ), key );
					break;

					case CgElementType::Annulus:
						worker.jsonToObject( *static_cast<CgAnnulus*>( elem ), key );
					break;

					case CgElementType::Signs:
					case CgElementType::Houses:
					case CgElementType::DasaLords:
					case CgElementType::Nakshatras:
						worker.jsonToObject( *static_cast<CgLabelledAnnulus*>( elem ), key );
					break;

					case CgElementType::UranianPlanetRing:
					case CgElementType::WesternPlanetRing:
					case CgElementType::Angles:
						worker.jsonToObject( *static_cast<CgPlanetRing*>( elem ), key );
					break;

					case CgElementType::UranianDisk:
						worker.jsonToObject( *static_cast<CgUranianDisk*>( elem ), key );
					break;

					case CgElementType::EastGrid:
					case CgElementType::SouthGrid:
					case CgElementType::DiamondGrid:
					case CgElementType::SbcGrid:
						worker.jsonToObject( *static_cast<CgVedicChartGrid*>( elem ), key );
					break;

					case CgElementType::Image:
						worker.jsonToObject( *static_cast<CgInlayImage*>( elem ), key );
					break;

					default:
						errorlog << "Invalid CgElementType " << tname;
						//assert( false );
					break;
				}
				if ( elem ) geom.elements[ type ] = elem;
			}
		}
	}

	configs.push_back( std::move( geom ));
	geom.updateMatchingCategories();
#ifdef DEBUG_CHARTGEOMETRY
	debuglog << "Finished ChartGeometryLoader::read_single_node";
#endif
}

// DUMMY
void ChartGeometry::fromJsonValue( const Json::Value&, JsonHandler& ) {}

#else

/*****************************************************
**
**  json transform template:
*  - element lookup or create if not found
*  - cast to target type
*  - load json
*  - insert/replace in element map
**
******************************************************/
template<typename CLASS> void transformJson( const Json::Value &v, JsonHandler &jh,
	ChartGeometry *geom, const CgElementType &e )
{
	const wxString s( enum_to_string( e ));
	if ( v[ s ] )
	{
		CgElement *elem = nullptr;
		if ( geom->elements.find( e ) != geom->elements.end()) 
		{
			elem = geom->elements[ e ];
		}
		else
		{
			elem = ChartGeometryLoader::get()->createDefaultElement( e );
		}
		assert( elem );
		CLASS *t = static_cast<CLASS*>( elem );
		assert( t );

		t->fromJsonValue( v[ s ], jh );
		geom->elements[ e ] = t;
	}
}

/*****************************************************
**
**   ChartGeometry   ---   fromJsonValue
**
******************************************************/
void ChartGeometry::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "name" ] )
		extractFromJson( v[ "name" ], name, jh );
	else name = tr( "Unknown" );

	if ( v[ "geom" ] )
		extractFromJson( v[ "geom" ], geometryType, jh );

	if ( ! is_valid_enum( geometryType ))
	{
		const wxString s = "invalid geometry \"" + v[ "geom" ].asString() + "\"";
		errorlog << s;

		geometryType = ChartShape::Circle;
		removeElements();
		elements[ CgElementType::Error ] = new CgError( s );
		return;
	}

#if 1
	transformJson<CgDisk>( v, jh, this, CgElementType::Disk );
	transformJson<CgLabelledAnnulus>( v, jh, this, CgElementType::Signs );

	transformJson<CgLabelledAnnulus>( v, jh, this, CgElementType::Houses );
	transformJson<CgDisk>( v, jh, this, CgElementType::Aspects );

	transformJson<CgPlanetRing>( v, jh, this, CgElementType::UranianPlanetRing );
	transformJson<CgPlanetRing>( v, jh, this, CgElementType::WesternPlanetRing );
	transformJson<CgPlanetRing>( v, jh, this, CgElementType::Angles );

	transformJson<CgUranianDisk>( v, jh, this, CgElementType::UranianDisk );
	transformJson<CgDisk>( v, jh, this, CgElementType::UranianAspects );

	transformJson<CgLabelledAnnulus>( v, jh, this, CgElementType::DasaLords );
	transformJson<CgLabelledAnnulus>( v, jh, this, CgElementType::Nakshatras );

	transformJson<CgVedicChartGrid>( v, jh, this, CgElementType::EastGrid );
	transformJson<CgVedicChartGrid>( v, jh, this, CgElementType::SouthGrid );
	transformJson<CgVedicChartGrid>( v, jh, this, CgElementType::DiamondGrid );
	transformJson<CgVedicChartGrid>( v, jh, this, CgElementType::SbcGrid );
	transformJson<CgInlayImage>( v, jh, this, CgElementType::Image );
#endif
}
/*****************************************************
**
**   ChartGeometryLoader   ---   loadSingleConfig
**
******************************************************/
void ChartGeometryLoader::loadSingleConfig( const Json::Value &v, JsonHandler &jh )
{
	ChartGeometry geom;
	if ( v[ "extends" ])
	{
		wxString baseconfig = v[ "extends" ].asString();;
		bool found = false;

		for( const auto &conf : configs )
		{
			if ( conf.name == baseconfig )
			{
				found = true;
				geom.name = conf.name;
				geom.geometryType = conf.geometryType;

				// available elements of original geom must be copied
				for( auto &item : conf.elements )
				{
					if ( geom.elements.find( item.first ) != geom.elements.end()) delete geom.elements[ item.first ];
					geom.elements[item.first] = item.second->clone();
				}
				break;
			}
		} if ( ! found )
		{
			const wxString s = "chart config cannot extend config \"" + baseconfig + "\" (not found)";
			errorlog << s;
			geom.elements[ CgElementType::Error ] = new CgError( s );
		}
	}
	geom.fromJsonValue( v, jh );
	geom.updateMatchingCategories();
	

#ifdef DEBUG_CHARTGEOMETRY
	debuglog << "Finished elem: " << geom.name << " elem size " << geom.elements.size()
		<< " matches " << geom.matchingCategories.size() << " categories ";
#endif
	configs.push_back( std::move( geom ));
}


#endif

/*****************************************************
**
**   ChartGeometry   ---   toJsonValue ALT
**
******************************************************/
Json::Value ChartGeometry::toJsonValue( JsonHandler& ) const
{
	return Json::Value();
}

/*****************************************************
**
**   ChartGeometryLoader   ---   getErrorConfig
**
******************************************************/
ChartGeometry &ChartGeometryLoader::getErrorConfig()
{
	return errorElement;
}

/*****************************************************
**
**   ChartGeometryLoader   ---   getAllIdsAndNames
**
******************************************************/
std::vector<std::pair<uint,wxString>> ChartGeometryLoader::getAllIdsAndNames( const WidgetCategory &cat )
{
#ifdef DEBUG_CHARTGEOMETRY
	debuglog << "ChartGeometryLoader::getAllIdsAndNames " << cat << " size " << getSize();
#endif
	std::vector<std::pair<uint,wxString>> list;

	uint id = 0;
	for( const ChartGeometry &geom : getConfigs())
	{
#ifdef DEBUG_CHARTGEOMETRY
		debuglog << "ChartGeometryLoader::getAllIdsAndNames " << geom.name << " matching cats " << geom.matchingCategories.size();
#endif
		if ( geom.matchesWidgetCategory( cat )) list.push_back( std::make_pair( id, geom.name ));
		++id;
	}
#ifdef DEBUG_CHARTGEOMETRY
	debuglog << "ChartGeometryLoader::getAllIdsAndNames size " << list.size();
#endif
	return list;
}

/*****************************************************
**
**   ChartGeometryLoader   ---   findNextGeometry
**
******************************************************/
uint ChartGeometryLoader::findNextGeometry( const WidgetCategory &category, const uint &geom )
{
	const uint totalsize = getSize();
#ifdef DEBUG_CHARTGEOMETRY
	debuglog << "ChartGeometryLoader::findNextGeometry index " << geom << " category " << category
		<< " totalsize " << totalsize;
#endif

	if ( totalsize == 0 ) return UINT_FOR_NOT_FOUND;

	uint ret = geom;
	for( uint i = 0; i < totalsize; i++ )
	{
		ret++;
		if ( ret >= totalsize ) ret = 0;
#if 0
		const ChartGeometry geom = getConfig( ret );
		debuglog << "ChartGeometryLoader::findNextGeometry geom " << geom.name << " categories " << geom.matchingCategories.size();
		for( const auto c : geom.matchingCategories )
		{
			debuglog << "ChartGeometryLoader::findNextGeometry geom " << geom.name << " category " << c;
		}
#endif
		if ( getConfig( ret ).matchesWidgetCategory( category )) return ret;
	}
	errorlog << "ChartGeometryLoader::findNextGeometry cannot find next geom for id " << geom
		<< " and category " << category;
	return UINT_FOR_NOT_FOUND;
}

/*****************************************************
**
**   ChartGeometryLoader   ---   findFirstGeometry
**
******************************************************/
uint ChartGeometryLoader::findFirstGeometry( const WidgetCategory &category, const uint &geom )
{
#ifdef DEBUG_CHARTGEOMETRY
	debuglog << "ChartGeometryLoader::findFirstGeometry index " << geom << " category " << category;
#endif

	if ( geom < getSize() && getConfig( geom ).matchesWidgetCategory( category )) return geom;
	return findNextGeometry( category, geom );
}

/*****************************************************
**
**   ChartGeometryLoader   ---   findPreviousGeometry
**
******************************************************/
uint ChartGeometryLoader::findPreviousGeometry( const WidgetCategory &category, const uint &geom )
{
	const uint totalsize = getSize();
#ifdef DEBUG_CHARTGEOMETRY
	debuglog << "ChartGeometryLoader::findPreviousGeometry index " << geom << " category " << category
		<< " totalsize " << totalsize;
#endif

	if ( totalsize == 0 ) return UINT_FOR_NOT_FOUND;

	uint ret = geom;
	for( uint i = 0; i < totalsize; i++ )
	{
		if ( ret == 0 ) ret = totalsize - 1;
		else ret--;
		if ( getConfig( ret ).matchesWidgetCategory( category )) return ret;
	}
	errorlog << "ChartGeometryLoader::findPreviousGeometry cannot find next geom for id " << geom
		<< " and category " << (int)category;
	return UINT_FOR_NOT_FOUND;
}
