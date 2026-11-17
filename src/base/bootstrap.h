/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/bootstrap.h
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
#ifndef _BOOTSTRAP_H_
#define _BOOTSTRAP_H_

#include <assert.h>
#include <complex>
#include <format>
#include <list>
#include <memory>
#include <set>
#include <sstream>
#include <wx/translation.h>

#define tr(STRING) wxGetTranslation(STRING)

using namespace std::complex_literals;

typedef std::complex<double> MComplex;

#undef uint
typedef unsigned int uint;

// Some important forward declarations
class ContextProperties;
typedef std::shared_ptr<ContextProperties> ContextProps_ptr;
class DocItem;
typedef std::shared_ptr<DocItem> DocItem_ptr;
class MDocument;
typedef std::shared_ptr<MDocument> MDocument_ptr;
class Horoscope;
typedef std::shared_ptr<Horoscope> Horoscope_ptr;

#define DEBUG_SHARED_PTR_LIFECYLCE

/*****************************************************
*
*   Singleton
*
******************************************************/
template <typename T> class Singleton
{
public:

	static T* get()
	{
		if ( ! ego ) ego = new T();
		return ego;
	}

	virtual ~Singleton()
	{
		ego = nullptr;
	}

private:

	inline static T *ego = nullptr;

protected:

	Singleton() {}
};

/*****************************************************
*
*   Helper class for typed enum iteration
*
******************************************************/
template< typename T > class Enum
{
public:
	Enum( const T first = T::First, const T last = T::Last )
	: first( first ), last( last ) {}

	const T first;
	const T last;

	int size() const { return (int)last; }

	class Iterator
	{
	public:
		Iterator( const int value ) : m_value( value ) {} 

		T operator*() const { return (T)m_value; }

		void operator++() { ++m_value; }

		bool operator!=( Iterator rhs ) { return m_value != rhs.m_value; }

	private:
		int m_value;
	};
};

template< typename T > typename Enum<T>::Iterator begin( Enum<T> e )
{
	return typename Enum<T>::Iterator( (int)e.first );
}

template< typename T > typename Enum<T>::Iterator end( Enum<T> e )
{
	return typename Enum<T>::Iterator( ((int)e.last) + 1 );
}

/*****************************************************
*
*   enum operator operators and asserts
*
******************************************************/
// scoped enum plus int -> scoped enum
template<class E> E operator +( const E &p, const int &i ) requires std::is_scoped_enum_v<E>
{
  return (E)((int)p + i );
}

// increase scoped enum by int
template<class E> void operator +=( E &p, const int &i ) requires std::is_scoped_enum_v<E>
{
  p = (E)((int)p + i );
}

template<class E> bool operator<( const E &p, const E &q ) requires std::is_scoped_enum_v<E>
{
  return (int)p < (int)q;
}

template<class E> bool operator<=( const E &p, const E &q ) requires std::is_scoped_enum_v<E>
{
  return (int)p <= (int)q;
}

template<class E> bool operator>( const E &p, const E &q ) requires std::is_scoped_enum_v<E>
{
  return (int)p > (int)q;
}

template<class E> bool operator>=( const E &p, const E &q ) requires std::is_scoped_enum_v<E>
{
  return (int)p >= (int)q;
}

template <class E> bool is_valid_enum( const E &e ) requires std::is_scoped_enum_v<E>
{
  return( e >= E::First && e <= E::Last );
}

template <class E> void assert_valid_enum( const E &e ) requires std::is_scoped_enum_v<E>
{
  assert( is_valid_enum<E>( e ));
}

template <class E> bool is_valid_enum_or_none( const E &e ) requires std::is_scoped_enum_v<E>
{
  return( e >= E::None && e <= E::Last );
}

template <class E> void assert_valid_enum_or_none( const E &e ) requires std::is_scoped_enum_v<E>
{
  assert( is_valid_enum_or_none<E>( e ));
}

template <class E> constexpr int enum_count() requires std::is_scoped_enum_v<E>
{
  return( (int)E::Last + 1 );
}

/*****************************************************
*
*   basic string definitions
*
******************************************************/

#if defined _MINGW_ || defined _MSC_VER
#define _WINDOWS
#define USE_WSTRING
#endif

//#define USE_WSTRING
#ifdef USE_WSTRING
#define Endl "\r\n"

typedef std::wstringstream mstringstream;

typedef std::wostream mostream;

#define mcout std::wcout
#define mcerr std::wcerr

#else
#define Endl "\n"

typedef std::stringstream mstringstream;

typedef std::ostream mostream;

#define mcout std::cout
#define mcerr std::cerr

#endif  // USE_WSTRING

// allow wxString as input for std::format
template <> struct std::formatter<wxString>
{
	constexpr auto parse( std::format_parse_context& ctx )
	{
		return ctx.begin();
	}

	auto format( const wxString& s, std::format_context& ctx ) const
	{
		return std::format_to( ctx.out(), "{}", s.ToStdString() );
	}
};

// allow scoped enums as input for stream output
template<typename E > mostream& operator<<( mostream &o, E e )
requires std::is_scoped_enum_v<E>
{
  o << enum_to_string( e );
  return o;
}

#ifdef CPP26

#include <meta>

template<typename E, bool Enumerable = std::meta::is_enumerable_type(^^E)>
  requires std::is_scoped_enum_v<E>
  constexpr std::string enum_to_string( const E &value )
{
  if constexpr( Enumerable )
  {
    template for ( constexpr auto e : std::define_static_array( std::meta::enumerators_of( ^^E )))
    {
      if ( value == [:e:] )
      {
        return std::string( std::meta::identifier_of( e ));
      }
    }
  }

  return std::format("None ({})", (int)value);
}

template<typename E>
  requires std::is_scoped_enum_v<E>
  constexpr E string_to_enum( std::string_view value )
{
  template for ( constexpr auto e : std::define_static_array( std::meta::enumerators_of( ^^E )))
  {
    if ( std::meta::identifier_of( e ) == value )
    {
      return [:e:];
    }
  }
  return E::None;
}

// Annotation for class members: this member should not be serialized
struct SkipJsonAnnotation {};
inline constexpr SkipJsonAnnotation skip_json;

#endif // CPP26

/*****************************************************
*
*   Enum again (with stringstream)
*
******************************************************/
template<typename E> wxString reportEnumParams()
{
 mstringstream ms;
 int line = 0;
 for ( const auto &e : Enum<E>() )
 {
		if ( line++ > 0 ) ms << ", ";
		ms << enum_to_string( e );
	} 
	return ms.str();
}

/*****************************************************
*
*  Json section
*
******************************************************/
class JsonHandler;
namespace Json { class Value; }

/*************************************************//**
*
*  \brief Abstract class defining methods for Json serialization.
*
******************************************************/
class Jsonable
{
public:

	virtual ~Jsonable() {}

	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) = 0;

	virtual Json::Value toJsonValue( JsonHandler& ) const = 0;

};

#endif

