/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Yoga.cpp
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

#include "Yoga.h"

#include "ConfigLoaderImpls.h"
#include "enumserializer.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "Table.h"
#include "Varga.h"
#include "YdlInterpreter.h"

#include <iostream>
#include <math.h>
#include <assert.h>
#include <stdio.h>

extern struct yy_buffer_state * yy_scan_string (const char *yy_str  );
extern int yyparse( void );
extern Rule *rule;
extern ExpressionErrorStatus parserErrorStatus;

//#define YOGA_PRINT_STACK_TRACE_CONSOLE
#define YOGA_PRINT_STACK_TRACE_GUI

/*****************************************************
**
**   YogaExpert   ---   init
**
******************************************************/
void YogaExpert::init()
{
	debuglog << "YogaExpert::init";
	if ( ! initialized )
	{
		clear();
		YogaConfigLoader *loader = YogaConfigLoader::get();
		std::vector<YogaConfig> &defs = loader->getConfigs();
		debuglog << "YogaExpert::init size of configs is " << defs.size();
		for ( YogaConfig &cfg : defs )
		{
			if ( ! cfg.group.empty()) groups.insert( cfg.group );
			if ( ! cfg.source.empty()) sources.insert( cfg.source );
		}
	}
	initialized = true;
}

/*****************************************************
**
**   YogaExpert   ---   clear
**
******************************************************/
void YogaExpert::clear()
{
	sources.clear();
	groups.clear();


	// TODO anders
	YogaConfigLoader::get()->clear();
	initialized = false;
}

/*****************************************************
**
**   YogaExpert   ---   parseRule
**
******************************************************/
Rule *YogaExpert::parseRule( wxString s )
{
	static int count = 0;;
	debuglog << "YogaExpert::parseRule " << count++;

	if ( s.IsEmpty())
	{
		errorlog << "YogaExpert::parseRule: empty rule";
		return nullptr;
	}

  parserErrorStatus = ExpressionErrorStatus::Ok;
	yy_scan_string( s.char_str() );
	const int ret = yyparse();
	if ( ret != 0 )
	{
		errorlog << "YogaExpert::parseRule: parser returns error on rule " << s << ", error status: " << parserErrorStatus;
		if ( rule ) delete rule;
		return nullptr;
	}

	if ( ! rule )
	{
		errorlog << "YogaExpert::parseRule rule is null, error status: " << parserErrorStatus;
		return nullptr;
	}
	debuglog << "YogaExpert::parseRule expression size " << rule->expressions.size() 
		<< " higher varga support is " << std::boolalpha << rule->supportsHigherVargas();

	return rule;
}

/*****************************************************
**
**   YogaExpert   ---   resultToString
**
******************************************************/
wxString YogaExpert::resultToString( const double &r, const RuntimeTypeId &type )
{
	wxString s;
	Lang lang;
	debuglog << "YogaExpert::resultToString type " << type << " value " << r;

	switch( type )
	{
		case RuntimeTypeId::None:
				s = "[None]";
			break;
		case RuntimeTypeId::Integer:
		case RuntimeTypeId::FloatingPoint:
			s = std::format( "[{}] {}", enum_to_string( type ), r );
		break;
		case RuntimeTypeId::House:
			s = std::format( "{}", enum_to_string( (House)r ));
		break;
		case RuntimeTypeId::Planet:
			s = std::format( "{}", enum_to_string( (PlanetId)r ));
		break;
		case RuntimeTypeId::Sign:
			s = std::format( "{}", enum_to_string( (Sign)r ));
		break;
		case RuntimeTypeId::Boolean:
			s = std::format( "{}", (bool)r );
		break;
		case RuntimeTypeId::Nakshatra:
			if ( is_valid_enum( (Nakshatra27)r ))
			{
				s = lang.getNakshatra27Name( (Nakshatra27)r );
			}
			else
			{
				s = "[None]";
			}
		break;
		case RuntimeTypeId::Varga:
			if ( is_valid_enum_or_none( (Varga)r ))
			{
				s = std::format( "{}", enum_to_string( (Varga)r ));
			}
			else
			{
				s = std::format( "invalid varga {}", r );
			}
		break;
		case RuntimeTypeId::Void:
			s = "[Void]";
		break;
		default:
			s = std::format( "Invalid RuntimeTypeId {}", (int)type );
		break;

	}

	debuglog << "YogaExpert::resultToString result " << s;
	return s;
}

/*****************************************************
**
**   YogaExpert   ---   updateAndWrite
**
******************************************************/
void YogaExpert::updateAndWrite( MDocument_ptr doc, ContextProps_ptr props, const Varga varga,
	const Sign artificialLagna, const wxString groupFilter, const wxString sourceFilter )
{
	debuglog << "YogaExpert::updateAndWrite Varga " << varga << " artificialLagna "
		<< artificialLagna << " groupFilter " << groupFilter << " sourceFilter " << sourceFilter;
	if ( ! initialized ) init();

	std::list<YogaConfig*> ret;
	YogaConfigLoader *loader = YogaConfigLoader::get();
	wxString msg;

	if ( ! loader->getSize())
	{
		infolog << "YogaConfigLoader needs reload: clearing yogas and reloading ...";
		initialized = false;
		init();
	}
	else
	{
		debuglog << "YogaConfigLoader needs no reload, continue";
	}

	Table *table = new Table( 4, 1 );
	table->setHeader( 0, tr( "Group" ));
	table->setHeader( 1, tr( "Description" ));
	table->format.col_break[1] = true;
	table->setHeader( 2, tr( "Effect" ));
	table->format.col_break[2] = true;
	table->setHeader( 3, tr( "Source" ));

	YdlInterpreter interpreter( props, varga, artificialLagna );

	for ( YogaConfig &cfg : loader->getConfigs() )
	{
		if ( varga != Varga::Rasi && ! cfg.highervargas ) continue;

		if (  ! groupFilter.empty() && cfg.group != groupFilter )  continue;
		if (  ! sourceFilter.empty() && cfg.source != sourceFilter )  continue;

		debuglog << "YogaExpert::updateAndWrite rule is " << cfg.rule;

		if ( cfg.rtree == nullptr )
		{
				errorlog << "Rule " << cfg.rule << " is not compiled";
				continue;
		}
		else
		{
			if ( (bool)interpreter.evaluateRule( cfg.rtree.get() ))
			{
				TableRow row;
				row.addValue( cfg.group );
				row.addValue( cfg.description );
				row.addValue( cfg.effect );
				row.addValue( cfg.source );
				table->addRow( row );
			}
		}
	}

	if ( table->getNbRows() == 1 )
	{
		doc->addParagraph( tr( "No matching Yogas found." ));
		delete table;
		return;
	}
	else
	{
		doc->addItem( table );
	}
}


