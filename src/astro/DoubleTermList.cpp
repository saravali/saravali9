/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/DoubleTermList.cpp
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

#include "DoubleTermList.h"

#include "astroconfig.h"
#include "enumserializer.h"
#include "Formulary.h"
#include "logger.h"
#include "UranianHelper.h"

#include <algorithm>

//#define DEBUG_DOUBLETERMLIST

/****************************************************
**
**   DoubleTerm   ---   operator==
**
******************************************************/
bool operator==( const DoubleTerm &p1, const DoubleTerm &p2 )
{
	if ( p1.lterm != p2.lterm ) return false;
	if ( p1.rterm != p2.rterm ) return false;
	return ( p1.formulaId == p2.formulaId );
}

/****************************************************
**
**   DoubleTermList   ---  Constructor
**
******************************************************/
DoubleTermList::DoubleTermList( const Planets &planets, UranianSection *usection,
	const HoroscopeRole &role1, const HoroscopeRole &role2, const MatchFilter *filter )
		: iterator( planets, usection, role1, role2, filter )
{
}

/****************************************************
**
**   DoubleTermList::Iterator   ---  Constructor
**
******************************************************/
DoubleTermList::Iterator::Iterator( const Planets &planets, UranianSection *usection,
	const HoroscopeRole &role1, const HoroscopeRole &role2, const MatchFilter *filter )
	: planets( planets ),
	role1( role1 ),
	role2( role2 ),
	filter( filter )
{
#ifdef DEBUG_DOUBLETERMLIST
	debuglog << "DoubleTermList::Iterator::Iterator";
#endif

	int i = 0;
	for( const auto &p : planets )
  {
		reverse_obs[ p ] = i;
		++i;
  }

	currentFormula = 0;
	formTemplates = Formulary::get()->getFormDefPairs( usection->matchFormulae, usection->includeAntiscia,
		usection->eventsIncludeLrPermutations );

#ifdef DEBUG_DOUBLETERMLIST
	for(  const auto &a : formTemplates )
	{
		debuglog << "DoubleTermList::Iterator::Iterator form template pair: " << a.first << ":" << a.second;
	}
#endif
	initIteratorPair();
}

/****************************************************
**
**   DoubleTermList::iterator   ---  begin
**
******************************************************/
DoubleTerm *DoubleTermList::Iterator::begin()
{
	return current;
}

/****************************************************
**
**   DoubleTermList::Iterator   ---  next
**
******************************************************/
void DoubleTermList::Iterator::next()
{
	++left.iterator;
	if( left.iterator == left.end())
	{
		++right.iterator;
		if( right.iterator == right.end())
		{
			// left and right are at their end, go to next formula
			++currentFormula;
			initIteratorPair();
			return;
		}
		else
		{
			// right was incremented successfully and left must restart from the beginning
			left.begin();
		}
	}
	setCurrent();
}

/****************************************************
**
**   DoubleTermList::Iterator   ---  nextValid
**
******************************************************/
void DoubleTermList::Iterator::nextValid()
{
	do
	{
		next();
		if ( current == nullptr ) return;
	}
	while( ! objectCombinationIsValid());
}

/****************************************************
**
**   DoubleTermList::Iterator   ---  initIteratorPair
**
******************************************************/
void DoubleTermList::Iterator::initIteratorPair()
{
#ifdef DEBUG_DOUBLETERMLIST
	debuglog << "DoubleTermList::Iterator::initIteratorPair current " << currentFormula
		<< " size " << formTemplates.size();
#endif

	if ( currentFormula >= formTemplates.size())
	{
		current = nullptr;
	}
	else
	{
#ifdef DEBUG_DOUBLETERMLIST
		debuglog << "INIT from form templates index " << currentFormula
			<< " left " << (int)formTemplates[currentFormula].first
			<< " right " << (int)formTemplates[currentFormula].second;
#endif
		left = TermList( planets, formTemplates[currentFormula].first );
		left.begin();
		right = TermList( planets, formTemplates[currentFormula].second );
		right.begin();

		setCurrent();
		if ( ! objectCombinationIsValid() ) nextValid();
	}
}

/****************************************************
**
**   DoubleTermList::Iterator   ---  setCurrent
**
******************************************************/
void DoubleTermList::Iterator::setCurrent()
{
	// warning, none of the iters should be at end
	if( left.iterator == left.end())
	{
#ifdef DEBUG_DOUBLETERMLIST
		debuglog << "DoubleTermList::Iterator::setCurrent: left iterator already at end, ending pair list";
#endif
		current = nullptr;
		return;
	}
	if( right.iterator == right.end())
	{
#ifdef DEBUG_DOUBLETERMLIST
		debuglog << "DoubleTermList::Iterator::setCurrent:: right iterator already at end, ending pair list";
#endif
		current = nullptr;
		return;
	}
	term.formulaId = currentFormula;
	term.lterm = &*left.iterator;
	term.rterm = &*right.iterator;
	current = &term;
#ifdef DEBUG_DOUBLETERMLIST
	debuglog << "DoubleTermList::Iterator::setCurrent: left " << *term.lterm << " right " << *term.rterm
		<< " formulaId " << currentFormula;
#endif
}

/****************************************************
**
**   DoubleTermList::Iterator   ---  formobsHaveCommonObjects
**
******************************************************/
bool DoubleTermList::Iterator::formobsHaveCommonObjects() const
{
	for( const PlanetId &v1 : current->lterm->getPlanetIds())
	{
		for( const PlanetId &v2 : current->rterm->getPlanetIds())
		{
			if ( v1 == v2 ) return true;
		}
	}
	return false;
}

/****************************************************
**
**   DoubleTermList::Iterator   ---  objectCombinationIsValid
**
******************************************************/
bool DoubleTermList::Iterator::objectCombinationIsValid() const
{
	assert( current );
	assert( current->lterm );
	assert( current->rterm );

#ifdef DEBUG_DOUBLETERMLIST
	debuglog << "DoubleTermList::Iterator::objectCombinationIsValid role1 " << role1 << " role2 " << role2
		<< "lTerm " << *current->lterm << " rTerm " << *current->rterm << " formulaId " << current->formulaId;
#endif

	if ( ! applyFilter()) return false;

	// Aries should not appear on left side in transits and directions
	if ( role1 == HoroscopeRole::Transit || role1 == HoroscopeRole::Progressive )
	{
		if ( current->lterm->isSingleTerm() && current->lterm->getFirstPlanet() == PlanetId::Aries ) return false;
		if ( current->lterm->isSingleTerm() && current->lterm->getFirstPlanet() == PlanetId::Capricorn ) return false;
	}

	// no aries = aries 
	if ( current->lterm->isSingleTerm() && current->lterm->getFirstPlanet() == PlanetId::Aries
	 && current->rterm->isSingleTerm() && current->rterm->getFirstPlanet() == PlanetId::Aries
	) return false;
	if ( current->lterm->isSingleTerm() && current->lterm->getFirstPlanet() == PlanetId::Capricorn
	 && current->rterm->isSingleTerm() && current->rterm->getFirstPlanet() == PlanetId::Capricorn
	) return false;

	// no common object if only one context
	// TODO ist das so, zb triple
	/*
	if ( role1 == role2 && formobsHaveCommonObjects())
	{
		return false;
	}
	*/

	// shifted modes need special treatment because symmetries are duplicates
	if ( role1 == HoroscopeRole::SolarArc )
	{
		// single/single anti relations only in one triangle
		if ( current->lterm->isSingleTerm() && current->rterm->isSingleTerm()
			&& ( current->lterm->isAntiTerm() || current->rterm->isAntiTerm()))
		{
			if ( isExcessTrianglePair( current->lterm->getFirstPlanet(), current->rterm->getFirstPlanet()))
				return false;
		}
		if ( current->lterm->isSingleTerm()
			&& current->rterm->isTripleTerm()
			&& current->lterm->getFirstPlanet() > current->rterm->getThirdPlanet())
		{
			return false;
		}
	}

	// no trivial symmetries if contexts are the same
	if ( role1 == role2
		&& current->lterm->isSingleTerm()
		&& current->rterm->isSingleTerm())
	{
		if ( isExcessTrianglePair( current->lterm->getFirstPlanet(),
			current->rterm->getFirstPlanet())) return false;
	}

	// avoid trivial duplicates in arc contexts e.g. Mo - Ma (s) = Mo - Ma (r)
	if ( role1 == HoroscopeRole::SolarArc
		&& current->lterm->isDoubleTerm()
		&& current->rterm->isDoubleTerm()
		&& current->lterm->getFirstPlanet() == current->rterm->getFirstPlanet() 
		&& current->lterm->getSecondPlanet() == current->rterm->getSecondPlanet() ) return false;

	// avoid duplicates in mixed differences (radix) like So-Ma=Mo-Ne and Ma-So=Ne-Mo
	if ( role1 == HoroscopeRole::Radix && role2 == HoroscopeRole::Radix
		&& current->lterm->getTermId() == TermId::Difference
		&& current->rterm->getTermId() == TermId::Difference
		&& ! PlanetComparator( planets )( current->lterm->getFirstPlanet(),  current->lterm->getSecondPlanet() )
		)
	{
		return false;
	}

	// avoid eg Sun = Sun + Moon
	if ( role1 == role2
		&& current->lterm->getTermId() == TermId::Single
		&& current->lterm->getFirstPlanet() == current->rterm->getFirstPlanet() )
	{
		return false;
	}

	// same as above but reverse
	if ( role1 == role2
		&& current->rterm->getTermId() == TermId::Single
		&& current->lterm->getFirstPlanet() == current->rterm->getFirstPlanet() )
	{
		return false;
	}

	return true;
}

/****************************************************
**
**   DoubleTermList::Iterator   ---  applyFilter
**
******************************************************/
bool DoubleTermList::Iterator::applyFilter() const
{
	if ( ! filter ) return true;
	return UranianHelper::get()->applyMatchFilter( *filter, &*left.iterator, &*right.iterator );
}

/****************************************************
**
**   DoubleTermList::Iterator   ---  isExcessTrianglePair
**
******************************************************/
bool DoubleTermList::Iterator::isExcessTrianglePair( const PlanetId &p1, const PlanetId &p2 ) const
{
	const auto i1 = reverse_obs.find( p1 );
	assert( i1 != reverse_obs.end());
	const auto i2 = reverse_obs.find( p2 );
	assert( i2 != reverse_obs.end());

	return *i1 >= *i2;
}

