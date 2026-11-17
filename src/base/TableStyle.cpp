/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/TableStyle.cpp
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

#include "ConfigListItems.h"
#include "ConfigLoaderImpls.h"
#include "FileConfig.h"
#include "logger.h"

#ifdef CPP26
#include "JsonWorker.h"
#else

#include "JsonHandler.h"
#include <fstream>
#include <json/json.h>
#endif

/*****************************************************
**
**   TableStyle   ---   Constructor
**
******************************************************/
TableStyle::TableStyle()
{
	name = tr( "Default" );
	headerBgColor = "#92BCD5";

	allCellBgColor = "#EFEFD4";
	oddCellBgColor = "#E9E1BB";
	evenCellBgColor = "#F1EBD3";
}

#ifdef CPP26
/*****************************************************
**
**   TableStyleLoader   ---   saveConfigs
**
******************************************************/
void TableStyleLoader::saveConfigs()
{
	wxString filename = FileConfig::get()->getResourceDir( resource ) + "table.json";
	debuglog << "saveConfig filename " << filename;
	
	JsonWorker( filename ).write_config_list_node( configs );
}

#else
/*****************************************************
**
**   TableStyleLoader   ---   saveConfigs
**
******************************************************/
void TableStyleLoader::saveConfigs()
{
	Json::Value root;
	JsonHandler js;
	wxString filename = FileConfig::get()->getResourceDir( resource ) + "table.json";
	debuglog << "saveConfig filename " << filename;
	
	for( const auto &config : configs )
	{
		Json::Value v = config.toJsonValue( js );
		root[ "configs" ].append( v );
	}

	Json::StreamWriterBuilder wbuilder;
	const std::unique_ptr<Json::StreamWriter> writer(wbuilder.newStreamWriter());

	std::ofstream o( filename.ToStdString() );
	if ( ! o.good())
	{
		//errorlog << "ERROR opening stream for file " << filename;
		js.addError( JsonErrorItem( JsonErrorType::FileWriteError, "output stream is not good" ));

	}
	else
	{
		writer->write( root, &o );
	}
	if ( js.hasErrors() ) js.reportErrors();
	else debuglog << "File " << filename << " successfully saved, Json has " << root.size() << " entries";
}

#endif
