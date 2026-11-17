/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ConfigListLoader.h
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

#ifndef _CONFIGLISTLOADER_H_
#define _CONFIGLISTLOADER_H_

#include "constants.h"

#include <map>
#include <vector>
#include <wx/string.h>

#ifdef CPP26
#include "JsonWorker.h"
#endif

namespace Json { class Value; }

/*************************************************//**
*
* 
*
******************************************************/
class BasicConfigLoader 
{
public:
	virtual ~BasicConfigLoader() {}

	// used for configs that are managed regularly in user home directory
	// but need be replaced by the default on demand. ->SheetConfig
	bool loadGlobalConfigs();

	virtual void loadDefaultConfigs() {}

protected:

	BasicConfigLoader( const ConfigResource &r ) : resource( r ) {}

	bool loadConfigs();

	// placeholder for logic executed after parsing all files, use YogaConfigLoader for generating rule trees
	virtual void postLoad() {}

	bool loadConfigFile( wxString file, const bool mustExist = false );

#ifdef CPP26

	virtual void load_config_list( const wxString &filename ) = 0;
#else

	virtual void loadSingleConfig( const Json::Value&, JsonHandler& ) = 0;

#endif

	//bool traverseConfigDir( const wxString &dirname, const bool mustExist = false );

	const ConfigResource resource;
};

/*************************************************//**
*
* 
*
******************************************************/
template<class T> class ConfigLoader : public BasicConfigLoader
{
public:

	ConfigLoader( const ConfigResource &cr ) : BasicConfigLoader( cr ) {}

	std::vector<T> &getConfigs()
	{
		if ( configs.size() == 0 ) init();
		return configs;
	}

	void setConfigs( std::vector<T> t )
	{
		configs.clear();
		configs = t;
	}

	T &getConfig( const uint &i )
	{
		if ( configs.size() == 0 ) init();
		if ( i >= configs.size()) return getErrorConfig();
		return configs[i];
	}

	T *getByName( const wxString &name )
	{
		if ( configs.size() == 0 ) init();
		for( auto &i : configs )
		{
			if ( i->name == name ) return &i;
		}
		return nullptr;
	}

	uint getIndexByName( wxString name )
	{
		if ( configs.size() == 0 ) init();
		uint index = 0;
		for( auto &item : configs )
		{
			if ( item.name == name ) return index;
			++index;
		}
		return UINT_FOR_NOT_FOUND;
	}

	uint getSize()
	{
		if ( configs.size() == 0 ) init();
		return configs.size();
	}

	std::vector<wxString> getNames()
	{
		if ( configs.size() == 0 ) init();
		std::vector<wxString> a;
		for( auto &i : configs )
		{
			a.push_back( i.name );
		}
		return a;
	}

	T &getDefaultConfig()
	{
		if ( configs.size() == 0 ) initFallback();
		return configs.front();
	}
	
	virtual T &getErrorConfig()
	{
		return getDefaultConfig();
	}
	
	void clear()
	{
		configs.clear();
	}

protected:

	void init()
	{
		configs.clear();
		loadConfigs();
		if ( configs.size() == 0 ) initFallback();
		assert( configs.size() > 0 );
	}

#ifdef CPP26

	// this method is overriden by ChartGeometryLoader and SheetSpecLoader because of pre and post conditions
	virtual void read_single_node( T &t, const Json::Value &v, JsonParser &parser )
	{
		JsonWorker( parser ).jsonToObject( t, v );
	}

	// this method is overriden by YogaConfigLoader because the output is written to local vector<YogaConfig>
	virtual void load_config_list( const wxString &filename ) override
	{
		JsonParser parser( filename );
		Json::Value root;
		parser.parse( root );
		for( const auto &a : root[ "configs" ] )
		{
			T t;
			read_single_node( t, a, parser );
			configs.push_back( t );
		}
	}

#else

	virtual void loadSingleConfig( const Json::Value &v,  JsonHandler &jh ) override
	{
		T t;
		t.fromJsonValue( v, jh );
		configs.push_back( t );
	}
#endif


	void initFallback()
	{
		configs.clear();
		//configs.push_back( getDefaultConfig());
		configs.push_back( T());
	}

	std::vector<T> configs;
};

#endif



