/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/DoubleTermList.h
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

#ifndef _DOUBLETERMLIST_H_
#define _DOUBLETERMLIST_H_

#include "TermList.h"

#include <array>

struct UranianSection;

/*****************************************************
**
**  
**
******************************************************/
struct DoubleTerm
{
	MTerm *lterm = nullptr;

	MTerm *rterm = nullptr;

	uint formulaId = UINT_FOR_NOT_FOUND;

	friend bool operator==( const DoubleTerm&, const DoubleTerm& );

	friend bool operator!=( const DoubleTerm &p1, const DoubleTerm &p2 )
		{ return ! (  p1 == p2 ); }
};

/*****************************************************
**
** 
**
******************************************************/
class DoubleTermList
{
public:

	DoubleTermList( const Planets&, UranianSection*, const HoroscopeRole &role1,
		const HoroscopeRole &role2, const MatchFilter* = nullptr );

	/*****************************************************
	**
	** 
	**
	******************************************************/
	struct Iterator
	{

		Iterator() {}

		Iterator( const Planets&, UranianSection*, const HoroscopeRole &role1,
			const HoroscopeRole &role2, const MatchFilter* = nullptr );

		DoubleTerm *begin();
	
		const DoubleTerm *operator->() const { return current; }

		const DoubleTerm &operator* () const { return *current; }

		Iterator &operator++() // pre increment
		{
			nextValid();
			return *this;
		}

		Iterator operator++( int count ) // post increment
		{
			for( int i = 0; i < count; i++ )
			{
				nextValid();
			}
			return *this;
		}

		bool operator!= ( const Iterator& other ) const
		{
				return current != other.current;
		}

	private:

		const Planets planets;
		
		const HoroscopeRole role1 = HoroscopeRole::None;

		const HoroscopeRole role2 = HoroscopeRole::None;

		const MatchFilter *filter = nullptr;

		std::map<PlanetId, int> reverse_obs;

		DoubleTerm *current = nullptr;

		DoubleTerm term;

		TermList left;

		TermList right;

		std::vector<TermIdPair> formTemplates;

		uint currentFormula = UINT_FOR_NOT_FOUND;

		void next();

		void nextValid();

		void initIteratorPair();

		void setCurrent();

		bool formobsHaveCommonObjects() const;

		bool objectCombinationIsValid() const;

		// returns true if one of the objects in cfg.filter is found
		bool applyFilter() const;

		bool isExcessTrianglePair( const PlanetId&, const PlanetId& ) const;

	};

	Iterator iterator;

	Iterator begin()
	{
		iterator.begin();
		return iterator;
	}

	Iterator end() { return Iterator(); }
};

#endif // _DOUBLETERMLIST_H_



