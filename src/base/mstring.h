/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/mstring.h
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

#ifndef _MSTRING_H_
#define _MSTRING_H_

#include "bootstrap.h"
#include "constants.h"
#include "Planet.h"
#include "Viewpoint.h"

#include <algorithm>
#include <variant>
#include <vector>
#include <wx/string.h>

// add bold, italic etc if required
enum class TokenFontPosition
{
	None = -1,
	Normal = 0,
	Subscriptum,
	Superscriptum
};

enum class TokenId
{
	//json:enum
	None = -1,
	Word = 0,
	Symbol,
	DegreeSymbol,
	MinuteSymbol,
	SecondSymbol,
	Space,
	NoBreakSpace,
	LineBreak
};

/*************************************************//**
*
*  This class incorporates the elements of MString, i.e.
*  atoms of strings that consist of text elements
*  and astrological symbols
*
******************************************************/
class MToken
{
public:

	MToken()
		: tokenId( TokenId::None ),
			symbol( PlanetId::None ),
			fontFormat( TokenFontPosition::None )
	{
	}

	MToken( const MToken &t )
		:	tokenId( t.tokenId ),
		contents( t.contents ),
		symbol( t.symbol ),
		fontFormat( t.fontFormat )
	{
	}

	MToken& operator= ( const MToken &t )
	{
		tokenId = t.tokenId;
		contents = t.contents,
		symbol = t.symbol;
		fontFormat = t.fontFormat;
		return *this;
	}

	MToken( const TokenId& );

	MToken( const wxString &s, const TokenFontPosition format = TokenFontPosition::Normal )
		: tokenId( TokenId::Word ),
			contents( s ),
			symbol( PlanetId::None ),
			fontFormat( format )
	{
	}

	// TODO raus
	MToken( const wxString &s, const TokenId &tokenId )
		: tokenId( tokenId ),
			contents( s ),
			symbol( PlanetId::None ),
			fontFormat( TokenFontPosition::Normal )
	{
	}

	MToken( const char &c, const TokenFontPosition format = TokenFontPosition::Normal )
		: tokenId( TokenId::Word ),
			contents( wxString( 1, c )),
			symbol( PlanetId::None ),
			fontFormat( format )
	{
	}

	MToken( const Sign&, const Viewpoint& );

	MToken( const Sign&, const PlanetPropertyConfig& );

	MToken( const PlanetId&, const Viewpoint& );

	MToken( const PlanetId&, const PlanetConfig& );

	MToken( const WesternAspectType&, const TextFormat = TextFormat::Long );

	MToken( const MovingDirection&, const TextFormat = TextFormat::Long,
		const TokenFontPosition = TokenFontPosition::Subscriptum );

	friend mostream& operator<< ( mostream&, const MToken& );

	wxString toWxString() const;

	wxString debugDump() const;

	// TODO das geht nicht wegen Table.o
	//const TokenId tokenId;
	TokenId tokenId;

	wxString contents;

	std::variant<PlanetId, Sign, WesternAspectType, MovingDirection> symbol;

	TokenFontPosition fontFormat;

private:

	void init( const Sign&, const PlanetPropertyConfig& );

	void init( const PlanetId&, const PlanetConfig& );
};

#define NBSP MToken( TokenId::NoBreakSpace )

/*************************************************//**
*
*  Builds strings that consist of text elements
*  and astrological symbols.
*
*  Output depends on the writer configuration and type, e.g. HTML or PDF.
*
*  Supported elements are
*  - text: word, space and line break
*  - astrological objects, i.e. planets
*  - astrological signs
*  - aspect symbols
*  - moving directions of objects (e.g. "R" for retrogression)
*
******************************************************/
class MString
{
public:
	explicit MString( const wxString = "" );

	MString( const MToken& );

	void operator=( const wxString );

	void add( const MToken& );
	void add( wxString );
	void add( const MString& );

	// TODO auch auf ein leeres Token prüfen (String = leer oder kein Symbol)
	bool isEmpty() const { return tokens.size() == 0; }
	void clear();

	std::vector<MToken> tokens;

	wxString debugDump() const;

	friend mostream& operator <<( mostream&, const MString& );

	wxString toWxString() const;

private:

	friend MString& operator <<( MString&, const wxString& );
	friend MString& operator <<( MString&, const MToken& );
	friend MString& operator <<( MString&, const MString& );
};

#endif


