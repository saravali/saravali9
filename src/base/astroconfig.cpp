/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/astroconfig.cpp
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

#include "astroconfig.h"
#include "AspectConfig.h"
#include "FileConfig.h"
#include "logger.h"
#include "Viewpoint.h"

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
**   YearLengthConfig   ---   getValue
**
******************************************************/
double YearLengthConfig::getValue() const
{
	switch ( type )
  {
		case 0:
			return 365.25;
    break;
		case 1:
			return 365.2422;
    break;
		case 2:
			return 365;
    break;
		case 3:
			return 360;
    break;
		case 4:
			return customValue;
    break;
		default:
			warnlog << "Warning: Wrong year length index " << type << " using default 365.25";
			return 365.25;
    break;
  }
}

/*****************************************************
**
**   DirectionConfig   ---   Constructor
**
******************************************************/
DirectionConfig::DirectionConfig()
{
	multiDirectionRoles.push_back( HoroscopeRole::Transit );
	multiDirectionRoles.push_back( HoroscopeRole::Progressive );
	multiDirectionRoles.push_back( HoroscopeRole::SolarArc );

	//eventFinderItems.push_back( RoleRestrictor( { HoroscopeRole::Transit, false } ));
	//eventFinderItems.push_back( RoleRestrictor( HoroscopeRole::Progressive, false ));
	eventFinderItems.push_back( RoleRestrictor( HoroscopeRole::SolarArc, true ));
}

/*****************************************************
**
**   KalavelaConfig   ---   Constructor
**
******************************************************/
KalavelaConfig::KalavelaConfig()
{
	elements =
	{
		{ PlanetId::Kala, { KalavelaLord::Sun, KalavelaPortion::Beginning }},
		{ PlanetId::Mrityu, { KalavelaLord::Mars, KalavelaPortion::Beginning }},
		{ PlanetId::Ardhaprahara, { KalavelaLord::Mercury, KalavelaPortion::Beginning }},
		{ PlanetId::Yamaghantaka, { KalavelaLord::Jupiter, KalavelaPortion::Beginning }},
		{ PlanetId::Gulika, { KalavelaLord::Saturn, KalavelaPortion::Beginning }},
		{ PlanetId::Mandi, { KalavelaLord::None, KalavelaPortion::Beginning }},
	};
}

/*****************************************************
**
**   MatchFilter   ---   clear
**
******************************************************/
void MatchFilter::clear()
{
	selectedPlanets.clear();
	rule = side = 0;
}

/*****************************************************
**
**   EphemConfig   ---   Constructor
**
******************************************************/
EphemConfig::EphemConfig()
{
	useEphemerisTime = 0;
	ephemCoordinateSystem = EphemCoordinateMode::Geocentric;
	useTruePositions = 0;

	sePath = FileConfig::get()->getDefaultEphemDir();

	sunrise_refrac = false;
	sunrise_def = 0;
}

/*****************************************************
**
**   Astroconfig   ---   Constructor
**
******************************************************/
Astroconfig::Astroconfig()
{
	debuglog << "Constructor Astroconfig";
	//ViewpointHelper *vh = ViewpointHelper::get();
	for( const Viewpoint viewpoint : Enum<Viewpoint>())
	{
		debuglog << "Constructor Astroconfig viewpoint " << viewpoint;
		viewpointConfigs[ viewpoint ] = ViewpointConfig( viewpoint );
	}
}

/*****************************************************
**
**   Astroconfig   ---   getViewpointConfig
**
******************************************************/
ViewpointConfig &Astroconfig::getViewpointConfig( const Viewpoint &viewpoint )
{
	assert_valid_enum<Viewpoint>( viewpoint );
	const auto a = viewpointConfigs.find( viewpoint );
	assert( a != viewpointConfigs.end());
	return viewpointConfigs.at( viewpoint );
}

/*****************************************************
**
**   Astroconfig   ---   getUranianSection
**
******************************************************/
UranianSection &Astroconfig::getUranianSection( const UranianContext &ctx )
{
	debuglog << "Astroconfig::getUranianSection context " << ctx;
	assert_valid_enum<UranianContext>( ctx );
	auto u = uranianSections.find( ctx );
	if ( u == uranianSections.end()) uranianSections[ ctx ] = UranianSection();

	return uranianSections[ ctx ];
}

/*****************************************************
**
**   Astroconfig   ---   load
**
******************************************************/
void Astroconfig::load()
{
	wxString filename = FileConfig::get()->getAstroConfigFilename();

#ifdef CPP26

	JsonParser parser( filename );
	Json::Value root;
	if ( parser.parse( root ))
	{
		JsonWorker( parser ).jsonToObject( *this, root );
	}
	else
	{
		errorlog << "cannot parse filename, using default astroconfig";
	}

#else

	try
	{
		JsonHandler jh;

		const Json::Value root = jh.parseJsonFile( filename );
		fromJsonValue( root, jh );

		if ( jh.hasErrors() ) jh.reportErrors();
	}
	catch( std::exception &e )
	{
		errorlog << "Exception while loading astroconfig '" << filename << "'. Reason was: " << e.what();
	}
#endif
}

/*****************************************************
**
**   Astroconfig   ---   save
**
******************************************************/
void Astroconfig::save()
{
	FileConfig *fc = FileConfig::get();
	wxString filename = fc->getAstroConfigFilename();
	fc->backupFile( filename );

#if CPP26
	JsonParser parser( filename );
	JsonWorker( parser ).objectToJson( *this );
#else

	JsonHandler jh;
	jh.saveJson( filename, this );
	if ( jh.hasErrors() ) jh.reportErrors();
#endif
}


