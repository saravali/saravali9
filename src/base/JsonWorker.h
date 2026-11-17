
/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/JsonWorker.h
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
#ifndef _JSONWORKER_H_
#define _JSONWORKER_H_

#ifdef CPP26

#include "bootstrap.h"
#include "logger.h"
#include "Planet.h"
#include "json/json.h"

#include <type_traits>

//#define DEBUG_JSONWORKER

class MBrush;

class wxColour;
class wxPen;
class wxSize;

struct JsonNodePosition
{
	int col1 = -1;
	int line1 = -1;
	int col2 = -1;
	int line2 = -1;
};

/*************************************************//**
*
* 
*
******************************************************/
struct JsonParser
{
	JsonParser( const wxString &filename ) : filename( filename ) {}

	bool parse( Json::Value& );

	bool saveJsonToFile( const Json::Value& );

	wxString showNodePosition( const Json::Value& );

	JsonNodePosition calculateNodePosition( const Json::Value& );

	void reportError( const Json::Value&, const wxString& );

	wxString filename;

	// string copy of file contents is used for error handling
	wxString contents;

};

/*************************************************//**
*
* 
*
******************************************************/
class JsonWorker
{
public:

	JsonWorker( JsonParser parser ) : parser( parser ) {}

	/*************************************************//**
	*
	*    JsonWorker   ---   jsonToObject
	*
	******************************************************/
	template<class T> void jsonToObject( T &a, const Json::Value &root )
	{
		try
		{
			read_node( a, root );
		}
		catch( Json::Exception &e )
		{
			errorlog << "Exception while converting object to json file '" << parser.filename << "'. Reason was: " << e.what();
		}
	}

	/*************************************************//**
	*
	*    JsonWorker   ---   objectToJson
	*
	******************************************************/
	template<class T> void objectToJson( T &a )
	{
		try
		{
			Json::Value value = write_node( a );
			parser.saveJsonToFile( value );
		}
		catch( Json::Exception &e )
		{
			errorlog << "Exception while converting object to json file '" << parser.filename << "'. Reason was: " << e.what();
		}
	}

	/*************************************************//**
	*
	*    JsonWorker   ---   write_config_list_node
	*
	******************************************************/
	template<typename T> void write_config_list_node( const std::vector<T> t )
	{
#ifdef DEBUG_JSONWORKER
		const std::string_view classname = typeid( T ).name();
		debuglog << "Start JsonWorker::write_config_list_node class " << classname << " size of vector " << t.size() << " filename " << parser.filename;
#endif

		try
		{
			Json::Value value = write_node( t );
			Json::Value root;
			root[ "configs" ] = value;
			parser.saveJsonToFile( root );
		}
		catch( Json::Exception &e )
		{
			errorlog << "Exception while converting object to json file '" << parser.filename << "'. Reason was: " << e.what();
		}

#ifdef DEBUG_JSONWORKER
		debuglog << "Finished JsonWorker::write_config_list_node, " << classname << " written to file " << parser.filename;
#endif
	}

private:

	// primitive types implemented in JsonWorker.cpp
	void read_node( int &i, const Json::Value &v, const bool = false );
	void read_node( uint&, const Json::Value&, const bool = false );
	void read_node( bool&, const Json::Value&, const bool = false );
	void read_node( double&, const Json::Value&, const bool = false );
	void read_node( wxString&, const Json::Value&, const bool = false );

	/*************************************************//**
	*
	* read scoped enums
	*
	******************************************************/
	template<typename E> requires std::is_scoped_enum_v<E> void read_node( E &e, const Json::Value &v, const bool = false )
	{
#ifdef DEBUG_JSONWORKER
		debuglog << "read ENUM value " << e << " type " << v.type();
#endif
		if ( v.isString()) { e = string_to_enum<E>( v.asString()); }
		else { parser.reportError( v, "is not valid scoped enum" ); }
	}

	//gdi types
	void read_node( wxColour&, const Json::Value&, const bool doArrayAppend = false );
	void read_node( wxPen&, const Json::Value&, const bool doArrayAppend = false );
	void read_node( wxSize&, const Json::Value&, const bool doArrayAppend = false );
	void read_node( MBrush&, const Json::Value&, const bool doArrayAppend = false );
	void read_node( DasaLord&, const Json::Value&, const bool doArrayAppend = false );

	/*************************************************//**
	*
	* read std::list
	*
	******************************************************/
	template<typename T> void read_node( std::list<T> &l, const Json::Value &v, const bool doArrayAppend = false )
	{
		#ifdef DEBUG_JSONWORKER
		debuglog << "Start read std::list " << parser.showNodePosition( v );
		#endif
		if ( v.isArray())
		{
			if ( ! doArrayAppend ) l.clear();
			for( const auto &a : v )
			{
				T t;
				read_node( t, a, doArrayAppend );
				l.push_back( t );
			}
		}
		#ifdef DEBUG_JSONWORKER
		debuglog << "Finished read std::list, has now " << l.size() << " elements";
		#endif
	}

	/*************************************************//**
	*
	* read std::vector
	*
	******************************************************/
	template<typename T> void read_node( std::vector<T> &l, const Json::Value &v, const bool doArrayAppend = false )
	{
		#ifdef DEBUG_JSONWORKER
		debuglog << "Start read std::vector " << parser.showNodePosition( v );
		#endif
		if ( v.isArray())
		{
			#ifdef DEBUG_JSONWORKER
			debuglog << "Vector is array, size is " << v.size();
			#endif
			if ( ! doArrayAppend ) l.clear();
			for( const auto &a : v )
			{
				T t;
				read_node( t, a, doArrayAppend );
				l.push_back( t );
			}
		}
		else
		{
			#ifdef DEBUG_JSONWORKER
			warnlog << "Vector is NOT array, size is " << v.size();
			#endif
		}
		#ifdef DEBUG_JSONWORKER
		debuglog << "Finished read std::vector, has now " << l.size() << " elements";
		#endif
	}

	/*************************************************//**
	*
	* read std::set
	*
	******************************************************/
	template<typename T> void read_node( std::set<T> &l, const Json::Value &v, const bool doArrayAppend = false )
	{
		#ifdef DEBUG_JSONWORKER
		debuglog << "Start read std::set " << parser.showNodePosition( v );
		#endif
		if ( v.isArray())
		{
			if ( ! doArrayAppend ) l.clear();
			for( const auto &a : v )
			{
				T t;
				read_node( t, a, doArrayAppend );
				l.insert( t );
			}
		}
		#ifdef DEBUG_JSONWORKER
		debuglog << "Finished read set, has now " << l.size() << " elements";
		#endif
	}

	/*************************************************//**
	*
	* read std::array
	*
	******************************************************/
	template<typename T, const size_t N> void read_node( std::array<T, N> &a, const Json::Value &v, const bool doArrayAppend = false )
	{
		#ifdef DEBUG_JSONWORKER
		debuglog << "Start read array " << parser.showNodePosition( v ) << " of size " << N;
		#endif
		if ( v.isArray())
		{
			for( size_t i = 0; i < v.size(); i++ )
			{
				if ( i < N )
				{
					read_node( a[i], v[ (Json::ArrayIndex)i ], doArrayAppend );
				}
			}
		}
		#ifdef DEBUG_JSONWORKER
		debuglog << "Finished read array " << parser.showNodePosition( v ) << " of size " << N;
		#endif
	}

	/*************************************************//**
	*
	* read std::pair
	*
	******************************************************/
	template<typename T, typename S> void read_node( std::pair<T, S> &p, const Json::Value &v, const bool doArrayAppend = false )
	{
		#ifdef DEBUG_JSONWORKER
		debuglog << "Start read std::pair " << parser.showNodePosition( v );
		#endif
		if ( v.isObject() )
		{
			if ( v[ "first" ].isNull())
			{
				parser.reportError( v, "pair has NULL first value" );
			}
			else
			{
				read_node( p.first, v[ "first" ], doArrayAppend );
			}
			if ( v[ "second" ].isNull())
			{
				parser.reportError( v, "pair has NULL second value" );
			}
			else
			{
				read_node( p.second, v[ "second" ], doArrayAppend );
			}
		}
		#ifdef DEBUG_JSONWORKER
		debuglog << "Finished read std::pair " << parser.showNodePosition( v );
		#endif
	}

	/*************************************************//**
	*
	* read std::map
	*
	******************************************************/
	template<typename K, typename V> void read_node( std::map<K, V> &m, const Json::Value &v, const bool doArrayAppend = false )
	{
		#ifdef DEBUG_JSONWORKER
		debuglog << "Start read std::map " << parser.showNodePosition( v );
		#endif
		if ( v.isObject() )
		{
			if ( ! doArrayAppend ) m.clear();
			for( const auto &mb : v.getMemberNames())
			{
				if ( v[ mb ].isNull())
				{
					parser.reportError( v, "map has NULL value" );
					continue;
				}

				Json::Value w = v[ mb ];
				K k;
				read_node( k, mb, doArrayAppend );
				V v;
				read_node( v, w, doArrayAppend );
				m[ k ] = v;
			}
		}
		#ifdef DEBUG_JSONWORKER
		debuglog << "Finished read map, has now " << m.size() << " elements";
		#endif
	}

	/*************************************************//**
	*
	* generic read_node for class types: the object is scliced by std::meta reflection methods
	*
	******************************************************/
	template<typename T> requires std::is_class_v<T> void read_node( T &obj, const Json::Value &v, const bool doArrayAppend = false  )
	{
		#ifdef DEBUG_JSONWORKER
		const std::string_view classname = typeid( T ).name();
		debuglog << "JsonWorker::read_node: START class " << classname << " at pos " << parser.showNodePosition( v );
		#endif

		constexpr auto ctx = std::meta::access_context::current();
		template for ( constexpr auto a : define_static_array ( std::meta::nonstatic_data_members_of( ^^T, ctx )))
		{
			const std::string name( std::meta::identifier_of( a ));

			#ifdef DEBUG_JSONWORKER
			debuglog << "JsonWorker::read_node: Start attribute " << classname << "::" << name << " at pos " << parser.showNodePosition( v );
			#endif

			if constexpr ( std::meta::annotations_of_with_type( a, ^^SkipJsonAnnotation).size() > 0 )
			{
				#ifdef DEBUG_JSONWORKER
				debuglog << "JsonWorker::read_node: SKIP attribute " << classname << "::" << name;
				#endif
			}
			else
			{
				read_node( obj.[:a:], v[ name ], doArrayAppend );
			}

#if 1
			template for (constexpr auto base : define_static_array(bases_of(^^T, ctx)))
			{
				const std::string name( std::meta::identifier_of( base ));
				#ifdef DEBUG_JSONWORKER
				debuglog << "JsonWorker::read_node: Start base class " << classname << "::" << name << " at pos " << parser.showNodePosition( v );
				#endif
				read_node( obj.[:base:], v, doArrayAppend );
			}
#endif

			#ifdef DEBUG_JSONWORKER
			debuglog << "JsonWorker::read_node: Finished attribute " << classname << "::" << name << " at pos " << parser.showNodePosition( v );
			#endif
		}

		#ifdef DEBUG_JSONWORKER
		debuglog << "JsonWorker::read_node: FINISHED class " << classname << " at pos " << parser.showNodePosition( v );
		#endif
	}

	/*************************************************//**
	*
	* Start of write section
	*
	******************************************************/
	/* geht nicht, crasht mit wxColour bei double und wxString
	template<typename T> requires std::is_integral_v<T> Json::Value write_node( const T& t )
	{
		return Json::Value( t );
	}
	*/
	Json::Value write_node( const int& t ) { return Json::Value( t ); }
	Json::Value write_node( const uint& t ) { return Json::Value( t ); }
	Json::Value write_node( const double& t ) { return Json::Value( t ); }
	Json::Value write_node( const bool& t ) { return Json::Value( t ); }

	template<typename E> Json::Value write_node( const E &e ) requires std::is_scoped_enum_v<E>
	{
		return Json::Value( std::string( enum_to_string( e )));
	}

	Json::Value write_node( const DasaLord& );

	Json::Value write_node( const wxString& );

	Json::Value write_node( const wxSize &s );

	Json::Value write_node( const wxColour& );

	Json::Value write_node( const wxPen& );

	Json::Value write_node( const MBrush& );

	template< typename T> Json::Value write_node( const std::list<T> &l )
	{
		Json::Value v;
		for( const auto &a : l )
		{
			v.append( write_node( a ));
		}
		return v;
	}

	template< typename T> Json::Value write_node( const std::vector<T> &l )
	{
		Json::Value v;
		for( const auto &a : l )
		{
			v.append( write_node( a ));
		}
		return v;
	}

	template< typename T> Json::Value write_node( const std::set<T> &l )
	{
		Json::Value v;
		for( const auto &a : l )
		{
			v.append( write_node( a ));
		}
		return v;
	}

	template< typename T, const size_t N> Json::Value write_node( const std::array<T, N> &l )
	{
		Json::Value v;
		for( const auto &a : l )
		{
			v.append( write_node( a ));
		}
		return v;
	}

	template< typename T, typename S> Json::Value write_node( const std::pair<T, S> &p )
	{
		Json::Value v;
		v[ "first" ] = write_node( p.first );
		v[ "second" ] = write_node( p.second );
		return v;
	}

	template<typename K, typename V> Json::Value write_node( const std::map<K, V> &m )
	{
		Json::Value v;
		for( const auto &[ key, value ] : m )
		{
			//debuglog << "write_node key " << key;
			if constexpr ( std::is_enum<K>::value )
			{
				v[ std::string( enum_to_string( key )) ] = write_node( value );
			}
			else if constexpr ( std::is_same< K, bool>::value
				|| std::is_same<K, int>::value
				|| std::is_same<K, unsigned int>::value
				|| std::is_same<K, wxString>::value
				|| std::is_base_of<Jsonable, V>::value )
			{
				v[ key ] = write_node( value );
			}
		}
		return v;
	}

	/*************************************************//**
	*
	* slicer for write methods
	*
	******************************************************/
	template<typename T> requires std::is_class_v<T> Json::Value write_node( const T &obj )
	{
		#ifdef DEBUG_JSONWORKER
		const std::string_view classname = typeid( T ).name();
		debuglog << "JsonWorker::write_node: START class " << classname;
		#endif

		Json::Value v;
		constexpr auto ctx = std::meta::access_context::current();

		template for ( constexpr auto a : define_static_array ( std::meta::nonstatic_data_members_of( ^^T, ctx )))
		{
			const std::string name( std::meta::identifier_of( a ));

			#ifdef DEBUG_JSONWORKER
			debuglog << "JsonWorker::write_node: Start attribute " << classname << "::" << name;
			#endif

			if constexpr ( std::meta::annotations_of_with_type( a, ^^SkipJsonAnnotation).size() > 0 )
			{
				#ifdef DEBUG_JSONWORKER
				debuglog << "JsonWorker::write_node: SKIP attribute " << classname << "::" << name;
				#endif
			}
			else
			{
				v[ name ] = write_node( obj.[:a:] );
			}

			#ifdef DEBUG_JSONWORKER
			debuglog << "JsonWorker::write_node: Finished attribute " << classname << "::" << name;
			#endif
		}

		#ifdef DEBUG_JSONWORKER
		debuglog << "JsonWorker::write_node: FINISHED class " << classname;
		#endif

		return v;
	}

	JsonParser parser;

};


#endif // CPP26

#endif // _JSONWORKER_H_

