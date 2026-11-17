/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/YogaConfig.cpp
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


#include "constants.h"
#include "ConfigLoaderImpls.h"
#include "FileConfig.h"
#include "logger.h"
#include "YdlExpression.h"

#ifdef CPP26
#include "JsonWorker.h"

#else
#include "enumserializer.h"
#include "JsonHandler.h"
#include <json/json.h>
#include <fstream>
#endif

extern struct yy_buffer_state * yy_scan_string (const char *yy_str  );
extern int yyparse( void );

extern Rule *rule;
extern ExpressionErrorStatus parserErrorStatus;
extern int yylineno;

#define DEBUG_EXPRESSION_LIFECYCLE

/*****************************************************
**
**   YogaConfig   ---   Constructor
**
******************************************************/
YogaConfig::YogaConfig()
{
#ifdef DEBUG_EXPRESSION_LIFECYCLE
	create_count++;
	infolog << "YogaConfig Constructor: " << create_count << " instances created, " << delete_count << " instances deleted.";
#endif
}

/*****************************************************
**
**   YogaConfig   ---   Destructor
**
******************************************************/
YogaConfig::~YogaConfig()
{
#ifdef DEBUG_EXPRESSION_LIFECYCLE
	delete_count++;
	infolog << "YogaConfig Destructor: " << create_count << " instances created, " << delete_count << " instances deleted.";
#endif
}

/*****************************************************
**
**   YogaConfigLoader   ---   postLoad
**
******************************************************/
#ifdef CPP26
void YogaConfigLoader::postLoad()
{
	debuglog << "Start YogaConfigLoader::postLoad, found " << getSize() << " configs";
	for ( auto &cfg : configs )
	{
#ifdef DEBUG_EXPRESSION_LIFECYCLE
	debuglog << "YogaConfigLoader::postLoad config " << cfg.description << " rule " << cfg.rule;
#endif

		if ( cfg.rule.IsEmpty())
		{
			errorlog << "Yoga " << cfg.description << " has no rule";
			continue;
		}

		if ( cfg.rtree )
		{
			errorlog << "Yoga " << cfg.description << " has already a tree.";
		}
		else
		{
			Rule *rtree = parseRule( cfg.rule );
			if ( rtree && rtree->errorStatus == ExpressionErrorStatus::Ok )
			{
				cfg.rtree = Rule_ptr( rtree );
				cfg.highervargas = rule->supportsHigherVargas();
			}
			else if ( rtree )
			{
				errorlog << "Yoga " << cfg.description << " cannot be parsed, error status " << rtree->errorStatus;
			}
			else 
			{
				errorlog << "Yoga " << cfg.description << " cannot be parsed, tree is NULL";
			}
		}
	}
}
#else

/*****************************************************
**
**   YogaConfigLoader   ---   loadSingleConfig
**
******************************************************/
void YogaConfigLoader::loadSingleConfig( const Json::Value &v, JsonHandler &jh )
{
	debuglog << "YogaConfigLoader::loadSingleConfig";
	YogaConfig cfg;

	parserErrorStatus = ExpressionErrorStatus::Ok;
	cfg.fromJsonValue( v, jh );
	
	if ( cfg.rule.IsEmpty())
	{
		errorlog << "File " << jh.getFilename() << " has no rule entry near offset " << v.getOffsetStart();
		return;
	}

	debuglog << "YogaConfigLoader::loadSingleConfig try to parse rule " << cfg.rule;
	Rule *rtree = parseRule( cfg.rule );

	if ( rtree && rtree->errorStatus == ExpressionErrorStatus::Ok )
	{
		cfg.rtree = Rule_ptr( rtree );
		cfg.highervargas = rule->supportsHigherVargas();
		configs.push_back( cfg );
		return;
	}

	// Start of error handling
	mstringstream errorlocation;
	errorlocation << "filename " << jh.getFilename();
	if ( v[ "rule" ] )
	{
		Json::Value rv = v[ "rule" ];
		errorlocation << " offset " <<  rv.getOffsetStart() << " to " << v.getOffsetLimit();
	}

	if ( ! rtree )
	{
		errorlog << "Yoga description with rule \"" << cfg.rule << "\" has null rule tree, ignoring Yoga. Location: "
			<< errorlocation.str();
	}
	else 
  {
    errorlog << "Invalid Expression error status of " << cfg.rule << " status is: " << rtree->errorStatus
			<< " Location: " << errorlocation.str();
    delete rtree;
  }
}
#endif

/*****************************************************
**
**   YogaConfigLoader   ---   parseRule
**
******************************************************/
Rule *YogaConfigLoader::parseRule( const wxString &rulestr )
{
	rule = nullptr;

	yy_scan_string( rulestr.char_str() );
  const int ret = yyparse();
	if ( ret != 0 )
	{
		errorlog << "YogaConfigLoader::parseRule returns " << ret;
		if ( rule ) delete rule;
		return nullptr;
	}

	// rule is set now if parsing worked
	return rule;
}

#ifdef CPP26
/*****************************************************
**
**   YogaConfigLoader   ---   loadConfigs
**
******************************************************/
void YogaConfigLoader::loadConfigs( wxString filename, std::vector<YogaConfig> &yogas )
{
	debuglog << "YogaConfigLoader::loadConfigs: " << filename;

	JsonParser parser( filename );
	Json::Value root;

	int count = 0;
	if ( parser.parse( root ))
	{
		debuglog << "YogaConfigLoader::loadConfigs: parse successful for file" << filename;
		JsonWorker worker ( parser );
		for( const auto &v : root[ "configs" ] )
		{
			debuglog << "YogaConfigLoader::loadConfigs: item " << ++count;
			YogaConfig y;
			worker.jsonToObject( y, v );
			yogas.push_back( y );
		}
	}
	else
	{
		errorlog << "YogaConfigLoader::loadConfigs: cannot parse filename";
	}
	debuglog << "YogaConfigLoader::loadConfigs: File " << filename << " successfully loaded, vector has " << yogas.size() << " entries";
}

/*****************************************************
**
**   YogaConfigLoader   ---   saveConfigs
**
******************************************************/
void YogaConfigLoader::saveConfigs( wxString filename, std::vector<YogaConfig> &yogas )
{
	debuglog << "YogaConfigLoader::saveConfigs: " << filename << " with " << yogas.size() << " entries";
	JsonWorker( filename ).write_config_list_node( yogas );
	debuglog << "File " << filename << " saved";
}

#else
/*****************************************************
**
**   YogaConfigLoader   ---   loadConfigs ALT
**
******************************************************/
void YogaConfigLoader::loadConfigs( wxString filename, std::vector<YogaConfig> &yogas )
{
	// this method is called by Yoga Editor. There is no parsing of the rules. Incorrect rules shall be loaded.
	JsonHandler jh;

	debuglog << "YogaConfigLoader::loadConfigs: Start load config " << filename;
  const Json::Value root = jh.parseJsonFile( filename );

  for( const auto &a : root[ "configs" ] )
  {
		YogaConfig cfg;
		cfg.fromJsonValue( a, jh );
		yogas.push_back( cfg );
  }
	if ( jh.hasErrors() ) jh.reportErrors();
	else debuglog << "File " << filename << " successfully loaded, Json has " << root.size() << " entries";
}

/*****************************************************
**
**   YogaConfigLoader   ---   saveConfigs ALT
**
******************************************************/
void YogaConfigLoader::saveConfigs( wxString filename, std::vector<YogaConfig> &yogas )
{
	Json::Value root;
	JsonHandler js;
	
	for( const auto &yoga : yogas )
	{
		Json::Value v = yoga.toJsonValue( js );
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

