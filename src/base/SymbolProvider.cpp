/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/SymbolProvider.cpp
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

#include "SymbolProvider.h"

#include "logger.h"
#include "Planet.h"

//#define DEBUG_SYMBOLPROVIDER

/*****************************************************
**
**   SymbolProvider   ---   getSignCode
**
******************************************************/
char SymbolProvider::getSignCode( const Sign &id, const int &capricornSymbol ) const
{
	if ( ! is_valid_enum( id ) || id == Sign::None )
	{
		errorlog << "invalid sign " << (int)id;
		return SYMBOL_CODE_ERROR;
	}

	static constexpr char k_sign_default[12] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L' };
	static constexpr char cp_alternative = 'V';

	if ( id == Sign::Capricorn && capricornSymbol ) return cp_alternative;
	else return k_sign_default[static_cast<int>( id )];
}

/*****************************************************
**
**   SymbolProvider   ---   getPlanetCode
**
******************************************************/
char SymbolProvider::getPlanetCode( const PlanetId &id, const int &uranusSymbol, const int &plutoSymbol ) const
{
	if ( id == PlanetId::None || id == PlanetId::Error ) return SYMBOL_CODE_ERROR;
	assert_valid_enum<PlanetId>( id );

#ifdef DEBUG_SYMBOLPROVIDER
	debuglog << " SymbolProvider::getPlanetCode " << id;
#endif

	switch ( id )
	{
		case PlanetId::Sun:
			return 'a';
		break;
		case PlanetId::Moon:
			return 'b';
		break;
		case PlanetId::Mercury:
			return 'c';
		break;
		case PlanetId::Venus:
			return 'd';
		break;
		case PlanetId::Mars:
			return 'e';
		break;
		case PlanetId::Jupiter:
			return 'f';
		break;
		case PlanetId::Saturn:
			return 'g';
		break;
		case PlanetId::Uranus:
			return uranusSymbol ? 'W' : 'h';
		break;
		case PlanetId::Neptune:
			return 'i';
		break;
		case PlanetId::Pluto:
			switch( plutoSymbol )
			{
				case 2:
					return  'Y';
				break;
				case 1:
					return  'X';
				break;
				case 0:
				default:
					return 'j';
				break;
			}
		break;

		case PlanetId::AscNode:
		case PlanetId::MeanAscNode:
		case PlanetId::TrueAscNode:
			return 'T';
		break;
		case PlanetId::DescNode:
		case PlanetId::MeanDescNode:
		case PlanetId::TrueDescNode:
			return 'U';
		break;

		case PlanetId::Ascendant:
			return 'M';
		break;
		case PlanetId::Meridian:
			return 'N';
		break;
		case PlanetId::Descendant:
			return 'O';
		break;
		case PlanetId::ImumCoeli:
			return 'P';
		break;

		case PlanetId::Cupido:
			return 'k';
		break;
		case PlanetId::Hades:
			return 'l';
		break;
		case PlanetId::Zeus:
			return 'm';
		break;
		case PlanetId::Kronos:
			return 'n';
		break;
		case PlanetId::Apollon:
			return 'o';
		break;
		case PlanetId::Admetos:
			return 'p';
		break;
		case PlanetId::Vulkanus:
			return 'q';
		break;
		case PlanetId::Poseidon:
			return 'r';
		break;

		case PlanetId::Aries:
			return 'A';
		break;
		case PlanetId::Cancer:
			return 'D';
		break;
		case PlanetId::Libra:
			return 'G';
		break;
		case PlanetId::Capricorn:
			return 'J';
		break;

		case PlanetId::Chiron:
			return 's';
		break;
		case PlanetId::Pholus:
			return 't';
		break;
		case PlanetId::Ceres:
			return 'u';
		break;
		case PlanetId::Pallas:
			return 'v';
		break;
		case PlanetId::Juno:
			return 'w';
		break;
		case PlanetId::Vesta:
			return 'x';
		break;
		case PlanetId::Lilith:
			return 'y';
		break;
		case PlanetId::Earth:
			return 'Z';
		break;

		default:
		break;
	}
	return SYMBOL_CODE_ERROR;
}

/*****************************************************
**
**   SymbolProvider   ---   getRetroCode
**
******************************************************/
char SymbolProvider::getRetroCode( const MovingDirection &id ) const
{
	static constexpr char direct = 'Q';
	static constexpr char retro = 'R';
	
	switch( id )
	{
		case MovingDirection::Direct:
			return direct;
		break;
		case MovingDirection::Retrograde:
			return retro;
		break;
		case MovingDirection::None:
		case MovingDirection::Stationary:
		default:
			return SYMBOL_CODE_ERROR;
		break;
	}
}

/*****************************************************
**
**   SymbolProvider   ---   getAspectCode
**
******************************************************/
char SymbolProvider::getAspectCode( const WesternAspectId &id ) const
{
	switch( id )
	{
		case WesternAspectId::Conjunction:
			return '0';
		break;
		case WesternAspectId::Opposition:
			return '1';
		break;
		case WesternAspectId::Trine:
			return '2';
		break;
		case WesternAspectId::Square:
			return '3';
		break;
		case WesternAspectId::Sextile:
			return '4';
		break;
		case WesternAspectId::Quincunx:
			return '6';
		break;
		case WesternAspectId::Semisquare:
			return '7';
		break;
		case WesternAspectId::Sesquisquare:
			return '8';
		break;
		case WesternAspectId::Semisextile:
			return '5';
		break;
		/*
		case WesternAspectId::Quartersquare:
			return '9';
		break;
		case WesternAspectId::Quaver:
			return ':';
		break;
		case WesternAspectId::Semiquaver:
			return ';';
		break;
		*/
		case WesternAspectId::Parallele:
			return '<';
		break;
		case WesternAspectId::Contraparallele:
			return '=';
		break;
		case WesternAspectId::Quintile:
		case WesternAspectId::Septile:
		case WesternAspectId::Novile:
		default:
			return SYMBOL_CODE_ERROR;
		break;
	}
}

