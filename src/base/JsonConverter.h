/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/JsonConverter.h
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
#ifndef _JSONCONVERTER_H_
#define _JSONCONVERTER_H_

#include "bootstrap.h"
#include "enumserializer.h"
#include "JsonHandler.h"
#include "logger.h"
#include "json/json.h"
#include "Planet.h"

#include <array>
#include <concepts>
#include <list>
#include <map>
#include <set>
#include <vector>

#include <wx/colour.h>
#include <wx/defs.h>
#include <wx/log.h>
#include <wx/pen.h>

/**************************************************************
***
**  EXTRACT JSON
***
***************************************************************/
void extractFromJson( const Json::Value &v, bool &b, JsonHandler &err );

void extractFromJson( const Json::Value &v, int &i, JsonHandler &err );

void extractFromJson( const Json::Value &v, uint &i, JsonHandler &err );

void extractFromJson( const Json::Value &v, double &d, JsonHandler &err );

void extractFromJson( const Json::Value &v, wxString &s, JsonHandler &err );

void extractFromJson( const Json::Value &v, wxSize &s, JsonHandler &err );

void extractFromJson( const Json::Value &v, Jsonable&, JsonHandler &err );

void extractFromJson( const Json::Value &v, DasaLord&, JsonHandler &err );

template<typename L, typename T> concept StandardExtractibleList = std::is_same_v<L, std::list<T>>;

//template<typename L, typename T> concept StandardExtractibleList = std::is_same_v<L, std::list<T>>;
// matcht nicht
template<typename L, typename T> requires StandardExtractibleList<L,T>
void extractFromJson( const Json::Value &v, L &l, JsonHandler &err )
{
	l.clear();
	if ( v.isArray())
	{
		for( size_t i = 0; i < v.size(); i++ )
		{
			T t;
			extractFromJson( v[ (Json::ArrayIndex)i ], t, err );
			l.push_back( t );
		}
	}
	else err.addReadValueError( v, "is not array" );
}

template<typename T>
void extractFromJson( const Json::Value &v, std::list<T> &l, JsonHandler &err )
{
	l.clear();
	if ( v.isArray())
	{
		for( const auto &a : v )
		{
			T t;
			extractFromJson( a, t, err );
			l.push_back( t );
		}
	}
	else err.addReadValueError( v, "is not array" );
}

template<typename T, const size_t N>
void extractFromJson( const Json::Value &v, std::array<T, N> &a, JsonHandler &err )
{
	if ( v.isArray())
	{
		for( size_t i = 0; i < v.size(); i++ )
		{
			if ( i < N )
			{
				extractFromJson( v[ (Json::ArrayIndex)i ], a[i], err );
			}
		}
	}
	else err.addReadValueError( v, "is not array" );
}

template<typename T>
void extractFromJson( const Json::Value &v, std::vector<T> &l, JsonHandler &err )
{
	l.clear();
	if ( v.isArray())
	{
		for( const auto &a : v )
		{
			T t;
			extractFromJson( a, t, err );
			l.push_back( t );
		}
	}
	else err.addReadValueError( v, "is not array" );
}


template<typename T>
void extractFromJson( const Json::Value &v, std::set<T> &l, JsonHandler &err )
{
	l.clear();
	if ( v.isArray())
	{
		for( const auto &a : v )
		{
			T t;
			extractFromJson( a, t, err );
			l.insert( t );
		}
	}
	else err.addReadValueError( v, "is not array" );
}

template<typename T, typename S>
void extractFromJson( const Json::Value &v, std::pair<T, S> &p, JsonHandler &err )
{
	if ( v.isObject() )
	{
		if ( v[ "first" ].isNull())
		{
			err.addReadValueError( v, "pair has NULL first value" );
		}
		else
		{
			extractFromJson( v[ "first" ], p.first, err );
		}
		if ( v[ "second" ].isNull())
		{
			err.addReadValueError( v, "pair has NULL second value" );
		}
		else
		{
			extractFromJson( v[ "second" ], p.second, err );
		}
	}
	else err.addReadValueError( v, "is not pair" );
}


#include "enumserializer.h"

template<typename K, typename V>
void extractFromJson( const Json::Value &v, std::map<K, V> &m, JsonHandler &err )
{
	m.clear();
	if ( v.isObject() )
	{
		for( const auto &mb : v.getMemberNames())
		{
			if ( v[ mb ].isNull())
			{
				err.addReadValueError( v, "map has NULL value" );
				continue;
			}

			Json::Value w = v[ mb ];
			K k;
			extractFromJson( mb, k, err );
			V v;
			extractFromJson( w, v, err );
			m[ k ] = v;
		}
	}
	else err.addReadValueError( v, "is not object (needed for map)" );
}

void extractFromJson( const Json::Value&, wxColour&, JsonHandler& );
void extractFromJson( const Json::Value&, wxPen&, JsonHandler& );

template<typename E> void extractFromJson( const Json::Value &v,  E &e, JsonHandler &jh )
	requires std::is_scoped_enum_v<E>
{
#ifdef CPP26
	if ( v.isString()) { e = string_to_enum<E>( std::string_view( v.asString())); }
#else
	if ( v.isString()) { string_to_enum( v.asString(), e ); }
#endif
	else { jh.addReadValueError( v, "cannot be converted to enum" ); }
}

/**************************************************************
***
**  JSON OUTPUT
***
***************************************************************/
template<typename E> Json::Value writeToJson( const E &e, JsonHandler& )
	requires std::is_enum_v<E>
{
	return Json::Value( std::string( enum_to_string( e )));
}

Json::Value writeToJson( const int&, JsonHandler& );

Json::Value writeToJson( const uint&, JsonHandler& );

Json::Value writeToJson( const bool&, JsonHandler& );

Json::Value writeToJson( const double&, JsonHandler& );

Json::Value writeToJson( const wxString&, JsonHandler& );

Json::Value writeToJson( const wxSize&, JsonHandler& );

Json::Value writeToJson( const DasaLord&, JsonHandler& );

Json::Value writeToJson( const Jsonable&, JsonHandler& );

template< typename T, const size_t N>
Json::Value writeToJson( const std::array<T, N> &l, JsonHandler &jh )
{
	Json::Value v;
	for( const auto &a : l )
	{
		v.append( writeToJson( a, jh ));
	}
	return v;
}

template< typename T>
Json::Value writeToJson( const std::list<T> &l, JsonHandler &jh )
{
	Json::Value v;
	for( const auto &a : l )
	{
		v.append( writeToJson( a, jh ));
	}
	return v;
}

template< typename T>
Json::Value writeToJson( const std::vector<T> &l, JsonHandler &jh )
{
	Json::Value v;
	for( const auto &a : l )
	{
		v.append( writeToJson( a, jh ));
	}
	return v;
}

template< typename T>
Json::Value writeToJson( const std::set<T> &l, JsonHandler &jh )
{
	Json::Value v;
	for( const auto &a : l )
	{
		v.append( writeToJson( a, jh ));
	}
	return v;
}

template< typename T, typename S>
Json::Value writeToJson( const std::pair<T, S> &p, JsonHandler &jh )
{
	Json::Value v;
	v[ "first" ] = writeToJson( p.first, jh );
	v[ "second" ] = writeToJson( p.second, jh );
	return v;
}

template<typename K, typename V> Json::Value writeToJson( const std::map<K, V> &m, JsonHandler &jh )
{
	Json::Value v;
	for( const auto &[ key, value ] : m )
	{
		//debuglog << "writeToJson key " << key;
		if constexpr ( std::is_enum<K>::value )
		{
			v[ std::string( enum_to_string( key )) ] = writeToJson( value, jh );
		}
		else if constexpr ( std::is_same< K, bool>::value
			|| std::is_same<K, int>::value
			|| std::is_same<K, unsigned int>::value
			|| std::is_same<K, wxString>::value
			|| std::is_base_of<Jsonable, V>::value )
		{
			v[ key ] = writeToJson( value, jh );
		}
	}
	//debuglog << "writeToJson MAP " << v.asString();
	return v;
}

Json::Value writeToJson( const wxColour&, JsonHandler& );
Json::Value writeToJson( const wxPen&, JsonHandler& );
Json::Value writeToJson( const wxPaperSize&, JsonHandler& );

#endif // _JSONCONVERTER_H_

