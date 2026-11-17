/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/SpWriter.h
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
#ifndef _SPWRITER_H_
#define _SPWRITER_H_

#include "bootstrap.h"
#include "Planet.h"

class MTerm;
struct Match;
struct UranianSection;
enum class TermId;

/*************************************************//**
*
* 
*
******************************************************/
class SensitivePointWriter
{
public:
	SensitivePointWriter( Horoscope_ptr h, ContextProps_ptr props, UranianSection *section )
		: horoscope( h ),
		props( props ),
		usection( section )
	{
		assert( h );
		assert( props );
		assert( usection );
	}


	void writeMatrix( MDocument_ptr, const TermId&, 
		const PlanetId leadingTripleObject = PlanetId::None );

	void writeList( MDocument_ptr, const TermId&, const bool &sortByLongitude,
		const PlanetId leadingTripleObject = PlanetId::None );

private:

	void writeSingleTermMatrix( MDocument_ptr, MTerm& );

	void writeDoubleTermMatrix( MDocument_ptr, MTerm&, const PlanetId leadingTripleObject = PlanetId::None );

	void writeHeader( MDocument_ptr, const TermId&, const PlanetId leadingTripleObjecti = PlanetId::None );
	
	Horoscope_ptr horoscope;

	ContextProps_ptr props;

	UranianSection *usection;
};

#endif // _SPWRITER_H_



