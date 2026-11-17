/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/mstring.cpp
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

#include "mstring.h"

#include "AspectConfig.h"
#include "astroconfig.h"
#include "enumserializer.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "SymbolProvider.h"

#include <wx/tokenzr.h>

extern Astroconfig *astroconfig;

//#define MSTRING_FINE_DEBUG 

/*****************************************************
**
**   MToken   ---   Constructor
**
******************************************************/
MToken::MToken( const TokenId &id )
	: tokenId( id ),
	symbol( PlanetId::None ),
	fontFormat( TokenFontPosition::None )
{
	assert( id >= TokenId::DegreeSymbol && id <= TokenId::LineBreak );
	switch( tokenId )
	{
		case TokenId::DegreeSymbol:
			contents = "\u00B0";
			//contents = "°";
		break;

		case TokenId::MinuteSymbol:
			contents = "\u0027";
			//contents = "'";
		break;

		case TokenId::SecondSymbol:
			contents = "\u0022";
			//contents = "\"";
		break;

		default:
		break;
	}
}

/*****************************************************
**
**   MToken   ---   Sign Constructor
**
******************************************************/
MToken::MToken( const Sign &sign, const PlanetPropertyConfig &cfg )
{
	init( sign, cfg );
}

MToken::MToken( const Sign &sign, const Viewpoint &viewpoint )
{
	const PlanetPropertyConfig &cfg = astroconfig->getViewpointConfig( viewpoint ).planetProps;
	init( sign, cfg );
}

/*****************************************************
**
**   MToken   ---   init by Sign
**
******************************************************/
void MToken::init( const Sign &sign, const PlanetPropertyConfig &cfg )
{
	if ( cfg.textFormat == TextFormat::Symbol )
	{
		tokenId = TokenId::Symbol;
		contents = SymbolProvider().getSignCode( sign, cfg.capricornSymbol );
		symbol = sign;
	}
	else
	{
		tokenId = TokenId::Word;
		wxString plainText = Lang().getSignName( sign, cfg );
		plainText.Trim( true ).Trim( false );
		contents = plainText;
	}
	fontFormat = TokenFontPosition::Normal;
}

/*****************************************************
**
**   MToken   ---   Object Constructor
**
******************************************************/
MToken::MToken( const PlanetId &ob, const Viewpoint &viewpoint )
{
	init ( ob, astroconfig->getViewpointConfig( viewpoint ).planets );
}

MToken::MToken( const PlanetId &ob, const PlanetConfig &cfg )
{
	init ( ob, cfg );
}

/*****************************************************
**
**   MToken   ---   init by planet
**
******************************************************/
void MToken::init( const PlanetId &ob, const PlanetConfig &cfg )
{
	char c = SYMBOL_CODE_ERROR;
	if ( cfg.textFormat == TextFormat::Symbol )
	{
		c = SymbolProvider().getPlanetCode( ob, cfg.uranusSymbol, cfg.plutoSymbol );
	}
	if ( c == SYMBOL_CODE_ERROR )
	{
		tokenId = TokenId::Word;
		contents = Lang().getPlanetName( ob, cfg );
	}
	else
	{
		tokenId = TokenId::Symbol;
		contents = c;
		symbol = ob;
	}
	fontFormat = TokenFontPosition::Normal;
}

/*****************************************************
**
**   MToken   ---   Direction Constructor
**
******************************************************/
MToken::MToken( const MovingDirection &dir, const TextFormat format, const TokenFontPosition fontFormat )
{
	if ( format == TextFormat::Symbol )
	{
		tokenId = TokenId::Symbol;
		contents = SymbolProvider().getRetroCode( dir );
		symbol = dir;
	}
	else
	{
		tokenId = TokenId::Word;
		contents = Lang().getMovingDirectionName( dir );
	}
	this->fontFormat = fontFormat;
}

/*****************************************************
**
**   MToken   ---   Aspect Constructor
**
******************************************************/
MToken::MToken( const WesternAspectType &type, const TextFormat format )
{
	char c = SYMBOL_CODE_ERROR;
	if ( format == TextFormat::Symbol )
	{
		c = SymbolProvider().getAspectCode( type.first );
	}
	if ( c == SYMBOL_CODE_ERROR )
	{
		tokenId = TokenId::Word;
		contents = AspectConfigLoader::get()->getAspectShortDescription( type );
	}
	else
	{
		tokenId = TokenId::Symbol;
		contents = c;
		symbol = type;
	}
	fontFormat = TokenFontPosition::Normal;
}

/*****************************************************
**
**   MToken   ---   operator <<
**
******************************************************/
mostream& operator<< ( mostream &stream, const MToken &token )
{
	stream << token.toWxString();
	return stream;
}

/*****************************************************
**
**   MToken   ---   toWxString
**
******************************************************/
wxString MToken::toWxString() const
{
	wxString s;
	constexpr Viewpoint viewpoint = Viewpoint::Western;
	switch( tokenId )
	{
		case TokenId::Word:
		case TokenId::DegreeSymbol:
		case TokenId::MinuteSymbol:
		case TokenId::SecondSymbol:
			switch( fontFormat )
			{
				case TokenFontPosition::Subscriptum:
				case TokenFontPosition::Superscriptum:
					s << "(" << contents << ")";
				break;
				default:
					s << contents;
				break;
			}
		break;

		// symbols cannot be shown in text mode, so escape them in curly brackets
		case TokenId::Symbol:
#if 0
			stream << "{" << token.contents << "}";
#else
			if ( std::holds_alternative<PlanetId>( symbol ))
			{
				const PlanetId ob = std::get<PlanetId>( symbol );
				if ( ob != PlanetId::None )
				{
					s << Lang().getPlanetName( std::get<PlanetId>( symbol ), viewpoint );
				}
				else
				{
					s << "<ERROR>";
				}
			}
			else if ( std::holds_alternative<Sign>( symbol ))
			{
				s << Lang().getSignName( std::get<Sign>( symbol ), viewpoint );
			}
		else if ( std::holds_alternative<MovingDirection>( symbol ))
			{
				s << Lang().getMovingDirectionName( std::get<MovingDirection>( symbol ));
			}
			else if ( std::holds_alternative<WesternAspectType>( symbol ))
			{
				s << AspectConfigLoader::get()->getAspectShortDescription( std::get<WesternAspectType>( symbol ));
			}
#endif
		break;
		case TokenId::LineBreak:
			s << LINE_BREAK;
		break;
		case TokenId::Space:
		case TokenId::NoBreakSpace:
			s << " ";
		break;
		default:
			errorlog << "Invalid token id " << (int)tokenId;
			assert( false );
		break;
	}
	return s;
}

/*****************************************************
**
**   MToken   ---   debugDump
**
******************************************************/
wxString MToken::debugDump() const
{
	wxString s;
	switch( tokenId )
	{
		case TokenId::Word:
			s << "{" << contents << "}";
		break;
		case TokenId::Symbol:
			s << "{sym:" << contents << "}";
		break;
		case TokenId::Space:
			s << "{space}";
		break;
		case TokenId::NoBreakSpace:
			s << "{nbsp}";
		break;
		case TokenId::LineBreak:
			s << "{break}";
		break;
		default:
			fatallog << "invalid token id " << tokenId;
			assert( false );
		break;
	}
	return s;
}

/*****************************************************
**
**   MString   ---   Constructor
**
******************************************************/
MString::MString( const wxString s )
{
#ifdef MSTRING_FINE_DEBUG 
	debuglog << "MString::MString( const wxString s ) " << s;
#endif
	if ( ! s.empty()) add( s );
}

/*****************************************************
**
**   MString   ---   Constructor
**
******************************************************/
MString::MString( const MToken &token )
{
#ifdef MSTRING_FINE_DEBUG 
	debuglog << "MString::MString( const MToken s ) " << token;
#endif
	add( token );
}

/*****************************************************
**
**   MString   ---   operator=
**
******************************************************/
void MString::operator=( const wxString s )
{
	clear();
	add( MToken( s ));
}

/*****************************************************
**
**   MString   ---   clear
**
******************************************************/
void MString::clear()
{
	tokens.clear();
}

bool matches( char c)
{
	return c == ' ' || c == '\n';
}

/*****************************************************
**
**   MString   ---   add
**
******************************************************/
void MString::add( wxString s )
{
#ifdef MSTRING_FINE_DEBUG 
	debuglog << "MString ADD #" << s << "#";
#endif
	if ( s.empty()) return;

	if ( s.Length() > 1 && s.StartsWith( " " )) 
	{
		tokens.push_back( MToken( TokenId::Space ));
	}

	wxStringTokenizer tokenizer( s );
	while( tokenizer.HasMoreTokens() )
	{
		tokens.push_back( tokenizer.GetNextToken());
		if ( tokenizer.HasMoreTokens())
		{
			switch( tokenizer.GetLastDelimiter() )
			{
				case ' ':
				case '\t':
					tokens.push_back( MToken( TokenId::Space ));
				break;
				default:
					tokens.push_back( MToken( TokenId::LineBreak ));
				break;
			}
		}
	}
	if ( s.EndsWith( " " )) 
	{
		tokens.push_back( MToken( TokenId::Space ));
	}
#ifdef MSTRING_FINE_DEBUG 
	debuglog << "Finished MString ADD token count " << tokens.size();
#endif
}

/*****************************************************
**
**   MString   ---   add
**
******************************************************/
void MString::add( const MToken &token )
{
#ifdef MSTRING_FINE_DEBUG 
	debuglog << "MString::add " << token ;
#endif
	tokens.push_back( token );
}

/*****************************************************
**
**   MString   ---   add
**
******************************************************/
void MString::add( const MString &f )
{
for( const auto &token : f.tokens )
{
		tokens.push_back( token );
	}
}

/*****************************************************
**
**   MString   ---   operator <<
**
******************************************************/
mostream& operator <<( mostream &stream, const MString &ms )
{
	stream << ms.toWxString();
	return stream;
}

/*****************************************************
**
**   MString   ---   toWxString
**
******************************************************/
wxString MString::toWxString() const
{
	wxString s;
	for( const MToken &token : tokens )
	{
		s << token.toWxString();
	}
	return s;
}

/*****************************************************
**
**   MString   ---   debugDump
**
******************************************************/
wxString MString::debugDump() const
{
	wxString s;
	for( const MToken &t : tokens ) { s << t.debugDump(); }
	return s;
}

MString& operator <<( MString &o, const wxString &s )
{
	o.add( s );
	return o;
}

MString& operator <<( MString &o, const MToken &token )
{
	o.add( token );
	return o;
}

MString& operator <<( MString &o, const MString &s )
{
	o.add( s );
	return o;
}

