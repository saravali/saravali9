/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/WesternAspect.h
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
#ifndef _WESTERNASPECT_H_
#define _WESTERNASPECT_H_

#include "AspectConfig.h"
#include "Expert.h"
#include "mstring.h"
#include "Planet.h"

#include <list>

class Table;

/*************************************************//**
*
* \brief Data container for Western aspects
*
******************************************************/
struct WesternAspectEvent
{
	const PlanetId planet1 = PlanetId::None;

	const double l1 = 0;

	const PlanetId planet2 = PlanetId::None;

	const double l2 = 0;

	const WesternAspectType type = { WesternAspectId::None, 1 };

	const double orbis = 0;
};

/*************************************************//**
*
*  \brief Calculation of Western aspects and corresponding reports
*
******************************************************/
class WesternAspectExpert : public Expert
{
public:

	WesternAspectExpert( ContextProps_ptr props ) : Expert( props, Viewpoint::Western ) {}

	std::list<WesternAspectEvent> calculate( const bool isCompositeChart = false );

	static void sort( std::list<WesternAspectEvent>&, const EventSortOrder& );

	void writeAspectList( MDocument_ptr, const std::list<WesternAspectEvent>&, const bool show_header = true );

	void writeAspectariumTable( Table*, const uint &colOffset, const uint &rowOffset,
		const std::list<WesternAspectEvent>&, PlanetConfig );
	
	Table *createAspectariumTable( const std::list<WesternAspectEvent>&, PlanetConfig, const bool show_header = true );

private:

	Planets planets;

	std::pair<WesternAspectType, double> getMatch( const double &lendiff ) const;

};

#endif // _WESTERNASPECT_H_

