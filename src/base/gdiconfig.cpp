/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/gdiconfig.cpp
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

#include "gdiconfig.h"

#include "ColorConfig.h"
#include "Commands.h"
#include "FileConfig.h"
#include "MultipleView.h"

#ifdef CPP26
#include "JsonWorker.h"
#else
#include "enumserializer.h"
#include "JsonHandler.h"
#include "json/json.h"
#include <fstream>
#endif

/*****************************************************
**
**   ViewPreferencesConfig   ---   Constructor
**
******************************************************/
ViewPreferencesConfig::ViewPreferencesConfig()
{
	dasaChartSelectedDasas =
	{
		DasaId::Vimsottari,
		DasaId::Yogini,
		DasaId::Kalachakra
	};
}

/*****************************************************
**
**   ViewPreferencesConfig   ---   getWidgetSize
**
******************************************************/
wxSize ViewPreferencesConfig::getWidgetSize( const ViewId &viewId )
{
	const auto &iter = widgetSize.find( std::string( enum_to_string( viewId )));

	if ( iter != widgetSize.end() ) return iter->second;

	constexpr int xMediumWindow =  800;
	constexpr int yMediumWindow =  600;
	constexpr int xBigWindow =  1000;
	constexpr int yBigWindow =  800;
	//constexpr int xSmallWindow =  400;
	constexpr int xSmallWindow =  399;
	constexpr int ySmallWindow =  400;
	constexpr int rVedicChart = 500;
	constexpr int rWesternChart = 800;

	switch( viewId )
	{
		case ViewId::Multiple:
		case ViewId::Uranian:
		case ViewId::SensitivePoints:
		case ViewId::EventFinder:
		case ViewId::Yoga:
		case ViewId::Varga:
		case ViewId::Ashtakavarga:
		case ViewId::DasaChart:
		case ViewId::Direction:
		case ViewId::Ephemeris:
		case ViewId::Partner:
		case ViewId::Shadbala:
			return wxSize( xBigWindow, yBigWindow );
		break;

		case ViewId::Solar:
		case ViewId::DasaTreeComposite:
		case ViewId::YogaEditor:
		case ViewId::SarvatobhadraChart:
			return wxSize( xMediumWindow, yMediumWindow );
		break;

		case ViewId::DasaTree:
			return wxSize( xSmallWindow, yBigWindow );
		break;

		case ViewId::PlanetList:
		case ViewId::Hora:
		case ViewId::Eclipse:
			return wxSize( xSmallWindow, ySmallWindow );
		break;

		case ViewId::VedicChart:
			return wxSize( rVedicChart, rVedicChart );
		break;

		case ViewId::ApplicationWindow:
			return wxSize( 800, 300 );
		break;

		case ViewId::UniversalChart:
		case ViewId::WesternChart:
		case ViewId::UranianChart:
		default:
			return wxSize( rWesternChart, rWesternChart );
		break;
	}
}

/*****************************************************
**
**   ViewPreferencesConfig   ---   setWidgetSize
**
******************************************************/
void ViewPreferencesConfig::setWidgetSize( const ViewId &viewId, const wxSize &size )
{
	widgetSize[ enum_to_string( viewId ) ] = size;
}

/*****************************************************
**
**   AtlasConfig   ---   Constructor
**
******************************************************/
AtlasConfig::AtlasConfig()
{
	favouriteCountries = { "IN", "US", "DE", "RU", "FR", "IT", "GB", "CA" };

	filterMode = 0;
	filterCaseSensitive = false;
	cacheSize = 2000;

	FileConfig *fc = FileConfig::get();
	sqlFile = fc->getDefaultAtlasSqlFile();
	databaseFile = fc->getDefaultAtlasDbFile();
}

/*****************************************************
**
**   MultipleViewConfig   ---   Constructor
**
******************************************************/
MultipleViewConfig::MultipleViewConfig()
{
	MultipleViewHandler *helper = MultipleViewHandler::get();
	radixItems = helper->createPairs( DataContext::Radix );
	directionItems = helper->createPairs( DataContext::Direction );
	multiDirectionItems = helper->createPairs( DataContext::MultiDirection );
	solarItems = helper->createPairs( DataContext::Partner );
	partnerItems = helper->createPairs( DataContext::Partner );
}

/*****************************************************
**
**   MultipleViewConfig   ---   extractItems
**
******************************************************/
std::vector<WidgetCategory> MultipleViewConfig::extractItems( const DataContext &ctx )
{
	std::vector<std::pair<WidgetCategory,bool>> &items = radixItems;

	switch( ctx )
	{
		case DataContext::Direction:
			items = directionItems;
		break;
		case DataContext::MultiDirection:
			items = multiDirectionItems;
		break;
		case DataContext::Solar:
			items =  solarItems;
		break;
		case DataContext::Partner:
			items =  partnerItems;
		break;
		case DataContext::Radix:
		default:
		break;
	}
	std::vector<WidgetCategory>v;
	for( const auto &a : items )
	{
		if ( a.second ) v.push_back( a.first );
	}
	return v;
}

/*****************************************************
**
**   Gdiconfig   ---   getActiveNotebookTab
**
******************************************************/
int Gdiconfig::getActiveNotebookTab( const wxString &key )
{
	const auto cfgpage = prefs.activeNotebookTab.find( key );
	if ( cfgpage != prefs.activeNotebookTab.end())
	{
		return cfgpage->second;
	}
	return 0;
}

/*****************************************************
**
**   Gdiconfig   ---   setActiveNotebookTab
**
******************************************************/
void Gdiconfig::setActiveNotebookTab( const wxString &key, const int &value )
{
	prefs.activeNotebookTab[ key ] = value;
}

/*****************************************************
**
**   Gdiconfig   ---   Constructor
**
******************************************************/
Gdiconfig::Gdiconfig()
{
	chartWestern = ChartConfig( WidgetCategory::WesternChart );
	chartUranian = ChartConfig( WidgetCategory::UranianChart );
	chartVarga = ChartConfig( WidgetCategory::VargaChart );
	chartSbc = ChartConfig( WidgetCategory::SarvatobhadraChart );
	chartUniversal = ChartConfig( WidgetCategory::UniversalChart );
}

/*****************************************************
**
**   Gdiconfig   ---   load
**
******************************************************/
void Gdiconfig::load()
{
	wxString filename = FileConfig::get()->getGdiConfigFilename();
	debuglog << "Start Gdiconfig::load filename " << filename;

#ifdef CPP26

	JsonParser parser( filename );
	Json::Value root;
	if ( parser.parse( root ))
	{
		JsonWorker( parser ).jsonToObject( *this, root );
	}
	else
	{
		errorlog << "cannot parse filename, using default gdiconfig";
	}
#else

	try
	{
		JsonHandler jh;

		const Json::Value root = jh.parseJsonFile( filename );
		debuglog << "Gdiconfig::load successfully parsed file " << filename;

		fromJsonValue( root, jh );

		if ( jh.hasErrors() ) jh.reportErrors();
	}
	catch( std::exception &e )
	{
		errorlog << "Exception while loading gdiconfig '" << filename << "'. Reason was: " << e.what();
	}
#endif

	if( ! colors.isOk())
	{
		warnlog << "color config not okay, setting default config";
		colors.setDefaultConfig();
	}
	if( ! bardiagram.isOk())
	{
		warnlog << "bar diagram config not okay, setting default config";
		bardiagram.setDefaultConfig();
	}
}

/*****************************************************
**
**   Gdiconfig   ---   save
**
******************************************************/
void Gdiconfig::save()
{
	FileConfig *fc = FileConfig::get();
	wxString filename = fc->getGdiConfigFilename();
	fc->backupFile( filename );

#ifdef CPP26
	JsonParser parser( filename );
	JsonWorker( parser ).objectToJson( *this );
#else
	JsonHandler jh;
	jh.saveJson( filename, this );
	if ( jh.hasErrors() ) jh.reportErrors();
#endif
}




