/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ConfigListLoader.cpp
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

#include "ConfigListLoader.h"

#include "FileConfig.h"
#include "logger.h"

#ifndef CPP26
#include "enumserializer.h"
#include "JsonHandler.h"
#include "json/json.h"
#endif

#include <wx/dir.h>
#include <wx/filename.h>

#define DEBUG_CONFIGLISTLOADER

/*****************************************************
**
**   BasicConfigLoader   ---   loadConfigs
**
******************************************************/
#if 1
bool BasicConfigLoader::loadConfigs()
{
	loadDefaultConfigs();

	FileConfig *fc = FileConfig::get();
	ConfigResourceDefinition def = fc->getConfigResourceDefinition( resource );
	const wxString localDirName = fc->getConfigDir() + def.tokenName << wxFileName::GetPathSeparator();
	const wxString globalDirName = fc->getResourceDir() + def.tokenName << wxFileName::GetPathSeparator();

#ifdef DEBUG_CONFIGLISTLOADER
	debuglog << "BasicConfigLoader::loadConfigs resource " << resource << " localDirName " << localDirName << " globalDirName " << globalDirName;
#endif


	// each base file name will be used only once. Either in localdir or (if not found there) in global dir.
	// std::set is used for sort order according to base file name instead of full path
	std::set<wxString> baseFilenames;

	for( const auto &f :  fc->getMatchingFiles( localDirName, { "json" }, false ))
	{
		baseFilenames.insert( f );
	}
	for( const auto &f :  fc->getMatchingFiles( globalDirName, { "json" }, false ))
	{
		if ( ! baseFilenames.contains( f )) baseFilenames.insert( f );
	}

#ifdef DEBUG_CONFIGLISTLOADER
	debuglog << "BasicConfigLoader::loadConfigs base filenames";
	for( const auto &a : baseFilenames )
	{
		debuglog << "BasicConfigLoader::loadConfigs   --> " << a;
	}
#endif

	for( const auto &f : baseFilenames )
	{
		if ( wxFileName::FileExists( localDirName + f ))
		{
#ifdef DEBUG_CONFIGLISTLOADER
			debuglog << "BasicConfigLoader::loadConfigs found local file " << localDirName + f;
#endif
			loadConfigFile( localDirName + f, true );
		}
		else if ( wxFileName::FileExists( globalDirName + f ))
		{
#ifdef DEBUG_CONFIGLISTLOADER
			debuglog << "BasicConfigLoader::loadConfigs found global file " << globalDirName + f;
#endif
			loadConfigFile( globalDirName + f, true );
		}
		else
		{
			errorlog << "BasicConfigLoader::loadConfigs: could not file file for resource " << f << " neither in " << localDirName << " nor " << globalDirName;
		}
	}
	return true;
}

#else
bool BasicConfigLoader::loadConfigs()
{
	loadDefaultConfigs();
	const bool b = traverseConfigDir( FileConfig::get()->getResourceDir( resource ), true );
	postLoad();
	return b;
}
#endif
/*
1. local und  global dir ermitteln
2. beide auflisten und basenames ermitteln
3. basenames sortieren
4. Files aus global/local in die volle Liste schreiben (local + global nur wenn noch nicht vorhanden)
5. load file pro item

*/

/*****************************************************
**
**   BasicConfigLoader   ---   loadConfigFile
**
******************************************************/
#ifdef CPP26
bool BasicConfigLoader::loadConfigFile( wxString filename, const bool mustExist )
{
	if ( mustExist && ! wxFileName::FileExists( filename ))
	{
		errorlog << "BasicConfigLoader::loadConfigFile: file " << filename << " not found or not readable";
		return false;
	}

	debuglog << "BasicConfigLoader::loadConfigFile: Start load config " << filename;
	load_config_list( filename );

	return true;
}

#else    // ALT
bool BasicConfigLoader::loadConfigFile( wxString filename, const bool mustExist )
{
	JsonHandler jh;

	if ( mustExist && ! wxFileName::FileExists( filename ))
	{
		errorlog << "BasicConfigLoader::loadConfigFile: file " << filename << " not found or not readable";
		return false;
	}

	debuglog << "BasicConfigLoader::loadConfigFile: Start load config " << filename;
	const Json::Value root = jh.parseJsonFile( filename );
	debuglog << "BasicConfigLoader::loadConfigFile: config parsed: " << filename;

	for( const auto &a : root[ "configs" ] )
	{
		try
		{
			loadSingleConfig( a, jh );
		}
		catch( std::exception &e )
		{
			errorlog << "Error while reading json file " << filename << " reason: " << e.what();
			errorlog << "json element: " << a.asString();
		}
	}

	if ( jh.hasErrors() )
	{
		jh.calculateErrorLinesAndColums();
		jh.reportErrors();
	}
	debuglog << "BasicConfigLoader::loadConfigFile Finished  load config " << filename;
	return true;
}
#endif

/*****************************************************
**
**   BasicConfigLoader   ---   traverseConfigDir
**
******************************************************/
#if 0
bool BasicConfigLoader::traverseConfigDir( const wxString &dirname, const bool mustExist )
{
	wxString filename;
	debuglog << "Start loading JSON configs for resource " << resource << " from directory " << dirname
		<< " mustExist " << std::boolalpha << mustExist;

	if ( ! wxFileName::DirExists( dirname ))
	{
		if ( mustExist )
		{
			warnlog << "directory " << dirname << " does not exist";
		}
		else
		{
			debuglog << "directory " << dirname << " does not exist";
		}
		return false;
	}
	const std::list<wxString> filespec = { "json" };
	int count = 0;
	for( const auto &f : FileConfig::get()->getMatchingFiles( dirname, filespec, true ))
	{
		debuglog << "Try to load JSON file " << f;
		loadConfigFile( f, true );
		++count;
	}

	debuglog << "JSON config loader for resource " << resource << " from directory " << dirname
		<< " read " << count << " files";
	if ( mustExist && ! count  )
	{
		errorlog << "JSON config loader for resource " << resource << " from directory " << dirname
			<< " found no files";
	}
	return true;
}
#endif

