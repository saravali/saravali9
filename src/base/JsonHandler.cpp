/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/JsonHandler.cpp
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

#include "JsonHandler.h"

#include "gdibase.h"
#include "gdiconfig.h"
#include "JsonConverter.h"
#include "logger.h"
#include "json/json.h"

#include <fstream>
#include <wx/filename.h>

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   JsonHandler   ---   parseJsonFile
**
******************************************************/
Json::Value JsonHandler::parseJsonFile( wxString filename )
{
	this->filename = filename;
	Json::Value root;
	if ( ! wxFileName::Exists( filename ))
	{
		errors.push_back( JsonErrorItem( JsonErrorType::FileExistsError, "file does not exist" ));
	}
	else if ( ! wxFileName::IsFileReadable( filename ))
	{
		errors.push_back( JsonErrorItem( JsonErrorType::FileReadPermissionError, "file not readable" ));
	}
	else
	{
		std::ifstream ifs;
		ifs.open( filename.ToStdString() );
		if ( ! ifs.good())
		{
			errors.push_back( JsonErrorItem( JsonErrorType::FileReadError, "input stream is not good" ));
		}
		else
		{
			Json::OStringStream ssin;
			ssin << ifs.rdbuf();
			contents = ssin.str();
		}
		debuglog << "JsonHandler::parseJsonFile: file " << filename << " parsed.";
		//errorlog << "JsonHandler::parseJsonFile: contents " << contents;

		char const* begin = contents.data();
		char const* end = begin + contents.size();

		{
			Json::CharReader *reader = nullptr;
			{
				Json::CharReaderBuilder builder;
				builder["collectComments"] = true;
				reader = builder.newCharReader();
			}

			{
				JSONCPP_STRING errs;
				if ( ! reader->parse( begin, end, &root, &errs ))
				{
					errors.push_back( JsonErrorItem( JsonErrorType::ParseError, errs ));
				}
			}
			delete reader;
		}
	}
	return root;
}

/*****************************************************
**
**   JsonHandler   ---   loadJson
**
******************************************************/
bool JsonHandler::loadJson( wxString filename, Jsonable *jsonable )
{
	this->filename = filename;

	Json::Value root = parseJsonFile( filename );

	jsonable->fromJsonValue( root, *this );

	if ( hasErrors() ) reportErrors();
	else debuglog << "Json file " << filename << " successfully loaded";
	return ! hasErrors();
}

/*****************************************************
**
**   JsonHandler   ---   saveJson
**
******************************************************/
void JsonHandler::saveJson( wxString filename, Jsonable *jsonable )
{
	this->filename = filename;
	Json::StreamWriterBuilder wbuilder;
	const std::unique_ptr<Json::StreamWriter> writer(wbuilder.newStreamWriter());

	std::ofstream o( filename.ToStdString() );
	if ( ! o.good())
	{
		errorlog << "Cannot open stream for writing, filename " << filename;
	}
	Json::Value value = jsonable->toJsonValue( *this );
	writer->write( value, &o );
}

/*****************************************************
**
**   JsonHandler   ---   addReadValueError
**
******************************************************/
void JsonHandler::addReadValueError( const Json::Value &v, const wxString &s )
{
	errors.push_back( JsonErrorItem( JsonErrorType::ReadValueError, s, v.getOffsetStart(), v.getOffsetLimit()));
}

/*****************************************************
**
**   JsonHandler   ---   reportErrors
**
******************************************************/
void JsonHandler::reportErrors()
{
	for( const auto &item : errors )
	{
		mstringstream ms;
		ms << "File " << filename << ": " << item.errorType << ": ";
		if ( item.offsetLimit )
		{
			ms << "at line " << item.line << ", columns " << item.column
				<< " to " << ( item.column + item.offsetLimit - item.offsetStart - 1 ) << ": ";
		}
		ms << item.message;
		errorlog << ms.str();
	}
}

/*****************************************************
**
**   JsonHandler   ---   calculateErrorLinesAndColums
**
******************************************************/
void JsonHandler::calculateErrorLinesAndColums()
{
	for( auto &item : errors )
	{
		if ( item.offsetStart )
		{
			const char *location = contents.data() + item.offsetStart;
			std::tie( item.line, item.column ) = calculateLineAndColumn( location );
		}
	}
}

/*****************************************************
**
**   JsonHandler   ---   calculateLineAndColumn
**
******************************************************/
std::pair<int,int> JsonHandler::calculateLineAndColumn( const char *location )
{
  using Char = char;
  using Location = const Char*;

	Location current = contents.data();
	Location lastLineStart = current;
	Location end_ = contents.data() + contents.size();

	int line = 0;
	int column = 0;

	while( current < location && current != end_ )
	{
		char c = *current++;
		if (c == '\r')
		{
			if (*current == '\n')
				++current;
			lastLineStart = current;
			++line;
		}
		else if (c == '\n')
		{
			lastLineStart = current;
			++line;
		}
	}
  // column & line start at 1
  column = int(location - lastLineStart) + 1;
  ++line;

	return std::make_pair( line,column );
}

/*****************************************************
**
**   JsonHandler   ---   calculateLineAndColumn
**
******************************************************/
wxString JsonHandler::getJsonCppVersion()
{
	return JSONCPP_VERSION_STRING;
}

/*****************************************************
**
**   MBrush   ---   fromJsonValue
**
******************************************************/
void MBrush::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
  if ( v[ "style" ] )
  {
		const wxString s = v[ "style" ].asString();
		style = wxBRUSHSTYLE_INVALID;
		for( const auto &m : mapping_list_brushstyle )
		{
			if ( m.second == s )
			{
				style = m.first;
				break;
			}
		}
  }
  if ( v[ "filename" ] )
  {
    style = wxBRUSHSTYLE_STIPPLE;

    extractFromJson( v[ "filename" ], filename, jh );

    if ( v[ "rotateHue" ] )
      extractFromJson( v[ "rotateHue" ], rotateHue, jh );
  }
  else
  {
    if ( v[ "color" ] ) extractFromJson( v[ "color" ], color, jh );
    else color = *wxRED;
		if ( style == wxBRUSHSTYLE_INVALID || style == wxBRUSHSTYLE_TRANSPARENT ) style = wxBRUSHSTYLE_SOLID;
  }
}

/*****************************************************
**
**   MBrush   ---   toJsonValue
**
******************************************************/
Json::Value MBrush::toJsonValue( JsonHandler &jh ) const
{
  Json::Value v;
	for( const auto &m : mapping_list_brushstyle )
	{
		if ( m.first == style )
		{
			v[ "style" ] = writeToJson( m.second, jh );
			break;
		}
	}

  if ( color.IsOk()) v[ "color" ] = writeToJson( (wxColour)color, jh );

  if ( ! filename.IsEmpty()) v[ "filename" ] = writeToJson( filename, jh );
  if ( rotateHue ) v[ "rotateHue" ] = writeToJson( rotateHue, jh );

  return v;
}

