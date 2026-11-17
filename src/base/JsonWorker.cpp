
/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/json_worker.h
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

#ifdef CPP26

#include "JsonWorker.h"

#include "ConfigListItems.h"

#include <wx/filename.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>

#include <fstream>
#include <type_traits>

/*****************************************************
**
**   JsonParser   ---   parse
**
******************************************************/
bool JsonParser::parse( Json::Value &root )
{
	debuglog << "Start JsonParser::parse filename " << filename;

	try
	{
		if ( filename.IsEmpty() )
		{
			errorlog << "Json filename is empty";
			return false;
		}
		else if ( ! wxFileName::Exists( filename ))
		{
			warnlog << "Json file " << filename << " does not exist";
			return false;
		}
		else if ( ! wxFileName::IsFileReadable( filename ))
		{
			errorlog << "Json file " << filename << " not readable";
			return false;
		}
		else
		{
			std::ifstream ifs;
			ifs.open( filename.ToStdString() );
			if ( ! ifs.good())
			{
				errorlog << "Json file " << filename << " cannot be read";
				return false;
			}
			else
			{
				Json::OStringStream ssin;
				ssin << ifs.rdbuf();
				contents = ssin.str();
			}
		}

		debuglog << "File " << filename << " parsed.";

		char const* begin = contents.data();
		char const* end = begin + contents.size();

		Json::CharReader *reader = nullptr;
		Json::CharReaderBuilder builder;
		builder["collectComments"] = true;
		reader = builder.newCharReader();

		JSONCPP_STRING errs;
		if ( ! reader->parse( begin, end, &root, &errs ))
		{
			errorlog << "Json file " << filename << " cannot be parsed";
			return false;
		}
		if ( reader ) delete reader;
	}
	catch( Json::Exception &e )
	{
		errorlog << "Exception while parsing file '" << filename << "'. Reason was: " << e.what();
		return false;
	}
	debuglog << "Finished JsonParser::parse filename " << filename;
	return true;
}

/*****************************************************
**
**   JsonParser   ---   saveJsonToFile
**
******************************************************/
bool JsonParser::saveJsonToFile( const Json::Value &v )
{
	debuglog << "Start JsonParser::saveJsonToFile filename " << filename;
	Json::StreamWriterBuilder wbuilder;
	const std::unique_ptr<Json::StreamWriter> writer( wbuilder.newStreamWriter() );

	std::ofstream o( filename.ToStdString() );
	if ( ! o.good())
	{
		errorlog << "Cannot open stream for writing, filename " << filename;
		return false;
	}
	else
	{
		try
		{
			writer->write( v, &o );
		}
		catch( Json::Exception &e )
		{
			errorlog << "Exception while saving file '" << filename << "'. Reason was: " << e.what();
			return false;
		}
	}
	debuglog << "Finished JsonWorker::saveJsonToFile filename " << filename;
	return true;
}

/*****************************************************
**
**   JsonParser   ---   showNodePosition
**
******************************************************/
wxString JsonParser::showNodePosition( const Json::Value &v )
{
	const auto pos = calculateNodePosition( v );
	return std::format( "(line {}/column {} to line {}/col {})", pos.line1, pos.col1, pos.line2, pos.col2 );
}

/*****************************************************
**
**   JsonParser   ---   calculateNodePosition
**
******************************************************/
JsonNodePosition JsonParser::calculateNodePosition( const Json::Value &v )
{
	const int p1 = v.getOffsetStart();
	const int p2 = v.getOffsetLimit();

#ifdef DEBUG_JSONWORKER
	//debuglog << "calculateNodePosition start " << p1 << " end " << p2;
#endif

	int currentline = 1;
	int a = 0;
	JsonNodePosition pos;

	wxStringTokenizer tokenizer( contents, "\n" );
	while ( tokenizer.HasMoreTokens() )
	{
    wxString token = tokenizer.GetNextToken();
		const int tl = (int)token.Length();
		//debuglog << "Line " << currentline << " length " << tl << " a " << a << " s= " << token;

		if ( p1 >= a && p1 <= a + tl )
		{
			pos.line1 = currentline;
			pos.col1 = p1 - a;
		}

		if ( p2 >= a && p2 <= a + tl )
		{
			pos.line2 = currentline;
			pos.col2 = p2 - a;
		}

		if ( pos.line1 >= 0 && pos.line2 >= 0 ) break;

		a += tl + 1;
		currentline++;
	}
 
	return pos;
}

/*****************************************************
**
**   JsonParser   ---   reportError
**
******************************************************/
void JsonParser::reportError( const Json::Value &v, const wxString &s )
{
	constexpr int maxCount = 100;
	static int count = 0;
	if ( ++count == maxCount )
	{
		errorlog << "Too many Json errors (" << count << "), switching to silent mode";
	}
	if ( count < maxCount )
	{
		errorlog << "Json error in file: " << filename << " pos " << showNodePosition( v ) << ": " << s;
	}
}

/*****************************************************
**
**   JsonWorker   ---   read_node: int
**
******************************************************/
void JsonWorker::read_node( int &i, const Json::Value &v, const bool )
{
#ifdef DEBUG_JSONWORKER
	debuglog << "read integer " << parser.showNodePosition( v ) << " value " << v.asString() << " type " << v.type();
#endif

	if ( v.isInt()) { i = v.asInt(); }
	else { parser.reportError( v, "is not int" ); }
}

/*****************************************************
**
**   JsonWorker   ---   read_node: unsigned int
**
******************************************************/
void JsonWorker::read_node( uint &i, const Json::Value &v, const bool )
{
#ifdef DEBUG_JSONWORKER
	debuglog << "read unsigned integer " << parser.showNodePosition( v ) << " value " << v.asString() << " type " << v.type();
#endif

	if ( v.isUInt()) { i = v.asUInt(); }
	else { parser.reportError( v, "is not unsigned int" ); }
}

/*****************************************************
**
**   JsonWorker   ---   read_node: bool
**
******************************************************/
void JsonWorker::read_node( bool &b, const Json::Value &v, const bool )
{
#ifdef DEBUG_JSONWORKER
	debuglog << "read boolean " << parser.showNodePosition( v ) << " value " << v.asString() << " type " << v.type();
#endif
	if ( v.isBool()) { b = v.asBool(); }
	else { parser.reportError( v, "is not boolean" ); }
}

/*****************************************************
**
**   JsonWorker   ---   read_node: double
**
******************************************************/
void JsonWorker::read_node( double &d, const Json::Value &v, const bool )
{
#ifdef DEBUG_JSONWORKER
	debuglog << "read double " << parser.showNodePosition( v ) << " value " << v.asString() << " type " << v.type();
#endif
	if ( v.isDouble()) { d = v.asDouble(); }
	else { parser.reportError( v, "is not double" ); }
}

/*****************************************************
**
**   JsonWorker   ---   read_node: wxString
**
******************************************************/
void JsonWorker::read_node( wxString &s, const Json::Value &v, const bool )
{
#ifdef DEBUG_JSONWORKER
	debuglog << "read string " << parser.showNodePosition( v ) << " value " << v.asString() << " type " << v.type();
#endif
	if ( v.isString()) { s = v.asString(); }
	else { parser.reportError( v, "is not string" ); }
}

/*****************************************************
**
**   JsonWorker   ---   read_node: wxColour
**
******************************************************/
void JsonWorker::read_node( wxColour &color, const Json::Value &v, const bool )
{
#ifdef DEBUG_JSONWORKER
	debuglog << "read wxColour " << parser.showNodePosition( v ) << " value " << v.asString() << " type " << v.type();
#endif

	if ( v.isString())
	{
		// rgba is not properly converted (alpha missing), so do it manually
		const wxString s = v.asString();
		wxRegEx r("^\\s*rgba\\s*\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\)\\s*$");
		if ( r.Matches( s ))
		{
			assert( r.GetMatchCount() == 5 );
			long red, green, blue, alpha;
			r.GetMatch( s, 1 ).ToLong( &red );
			r.GetMatch( s, 2 ).ToLong( &green );
			r.GetMatch( s, 3 ).ToLong( &blue );
			r.GetMatch( s, 4 ).ToLong( &alpha );
			color.Set( (int)red, (int)green, (int)blue, (int)alpha );
		}
		else
		{
			color.Set( s );
		}
	}
	else { parser.reportError( v, "is not string" ); }
}

/*****************************************************
**
**   JsonWorker   ---   read_node: wxPen
**
******************************************************/
void JsonWorker::read_node( wxPen &pen, const Json::Value &v, const bool )
{
#ifdef DEBUG_JSONWORKER
	debuglog << "read wxPen";
#endif
  if ( v[ "color" ] )
	{
		wxColour color;
		read_node( color, v[ "color" ] );
		if ( color.IsOk() ) pen.SetColour( color );
	}
  if ( v[ "width" ] )
	{
		int width = 0;
		read_node( width, v[ "width" ] );
		pen.SetWidth( width );
	}

  if ( v[ "style" ] )
	{
		wxPenStyle style = wxPENSTYLE_INVALID;
		std::string s = v[ "style" ].asString();
		for( const auto &m : mapping_list_penstyle )
		{
			if ( m.second == s )
			{
				style = m.first;
				break;
			}
		}
		if ( style != wxPENSTYLE_INVALID ) pen.SetStyle( style );
	}
}

/*****************************************************
**
**   JsonWorker   ---   read_node: wxSize
**
******************************************************/
void JsonWorker::read_node( wxSize &size, const Json::Value &v, const bool )
{
	if ( v[ "x" ] && v[ "x" ].isInt() ) size.SetWidth( v[ "x" ].asInt());
	if ( v[ "y" ] && v[ "y" ].isInt() ) size.SetHeight( v[ "y" ].asInt());
}

/*****************************************************
**
**   JsonWorker   ---   read_node: MBrush
**
******************************************************/
void JsonWorker::read_node( MBrush &brush, const Json::Value &v, const bool )
{
  if ( v[ "style" ] )
  {
		const wxString s = v[ "style" ].asString();
		brush.style = wxBRUSHSTYLE_INVALID;
		for( const auto &m : mapping_list_brushstyle )
		{
			if ( m.second == s )
			{
				brush.style = m.first;
				break;
			}
		}
  }
  if ( v[ "filename" ] )
  {
    brush.style = wxBRUSHSTYLE_STIPPLE;

    read_node( brush.filename, v[ "filename" ] );

    if ( v[ "rotateHue" ] )
      read_node( brush.rotateHue, v[ "rotateHue" ] );
  }
  else
  {
    if ( v[ "color" ] ) read_node( brush.color, v[ "color" ] );
    //else brush.color = *wxRED;
		if ( brush.style == wxBRUSHSTYLE_INVALID || brush.style == wxBRUSHSTYLE_TRANSPARENT ) brush.style = wxBRUSHSTYLE_SOLID;
  }
}

/*****************************************************
**
**   JsonWorker   ---   read_node: DasaLord
**
******************************************************/
void JsonWorker::read_node( DasaLord &lord, const Json::Value &v, const bool )
{
	if ( v.isString())
	{
		const PlanetId o = string_to_enum<PlanetId>( v.asString());
		if ( o != PlanetId::None )
		{
			lord = o;
		}
		else
		{
			const Sign sign = string_to_enum<Sign>( v.asString());
			if ( sign != Sign::None ) lord = sign;
		}
	}
	else { parser.reportError( v, "enum cannot be converted to valid DasaLord" ); }
}

/*****************************************************
**
**   JsonWorker   ---   write_node: wxString
**
******************************************************/
Json::Value JsonWorker::write_node( const wxString &s )
{
	return Json::Value( s.ToStdString() );
}

/*****************************************************
**
**   JsonWorker   ---   write_node: wxSize
**
******************************************************/
Json::Value JsonWorker::write_node( const wxSize &s )
{
	Json::Value v;
	v[ "x" ] = s.GetWidth();
	v[ "y" ] = s.GetHeight();
	return v;
}

/*****************************************************
**
**   JsonWorker   ---   write_node: DasaLord
**
******************************************************/
Json::Value JsonWorker::write_node( const DasaLord &lord )
{
	Json::Value v;
	if ( std::holds_alternative<PlanetId>( lord ))
	{
		v = std::string( enum_to_string( std::get<PlanetId>( lord )));
	}
	else if ( std::holds_alternative<Sign>( lord ))
	{
		v = std::string( enum_to_string( std::get<Sign>( lord )));
	}
	return v;
}

/*****************************************************
**
**   JsonWorker   ---   write_node: wxColour
**
******************************************************/
Json::Value JsonWorker::write_node( const wxColour &color )
{
	Json::Value v;
	v = color.GetAsString( wxC2S_CSS_SYNTAX ).ToStdString();
	return v;
}

/*****************************************************
**
**   JsonWorker   ---   write_node: wxPen
**
******************************************************/
Json::Value JsonWorker::write_node( const wxPen &pen )
{
	Json::Value v;

	if ( pen.GetColour().IsOk())
	{
		v[ "color" ] = write_node( pen.GetColour() );
	}

  if ( pen.GetWidth() > 0 ) v[ "width" ] = write_node( pen.GetWidth() );

	if ( pen.GetStyle() >= 0 )
	{
		for( const auto &m : mapping_list_penstyle )
		{
			if ( m.first == pen.GetStyle() )
			{
				v[ "style" ] = write_node( m.second );
				break;
			}
		}
	}
	return v;
}

/*****************************************************
**
**   JsonWorker   ---   write_node: MBrush
**
******************************************************/
Json::Value JsonWorker::write_node( const MBrush &brush )
{
  Json::Value v;
	for( const auto &m : mapping_list_brushstyle )
	{
		if ( m.first == brush.style )
		{
			v[ "style" ] = write_node( m.second );
			break;
		}
	}

  if ( brush.color.IsOk()) v[ "color" ] = write_node( (wxColour)brush.color );

  if ( ! brush.filename.IsEmpty()) v[ "filename" ] = write_node( brush.filename );
  if ( brush.rotateHue ) v[ "rotateHue" ] = write_node( brush.rotateHue );

  return v;
}

#endif // CPP26

