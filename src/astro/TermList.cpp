/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/TermList.cpp
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

#include "TermList.h"

#include "enumserializer.h"
#include "Lang.h"
#include "logger.h"

#include <algorithm>

//#define DEBUG_TERMLIST

/****************************************************
**
**   TermList   ---   Constructor
**
******************************************************/
TermList::TermList( const Planets planets, const TermId termId )
	: iterator( planets, termId )
{
}

/****************************************************
**
**   TermList::Iterator   ---   Constructor
**
******************************************************/
TermList::Iterator::Iterator( const Planets planets, const TermId termId )
  : planets( planets ),
    term( termId ),
    varcount( term.getVariableCount())
{
#ifdef DEBUG_TERMLIST
  debuglog << "TermList::Iterator::Iterator planets size " << planets.size()
    << " termid " << termId
    << " varcount " << varcount;
#endif
}

/****************************************************
**
**   TermList::Iterator   ---   begin
**
******************************************************/
MTerm *TermList::Iterator::begin()
{
	loop_status.fill( 0 );
	incarnate();
	current = &term;
#ifdef DEBUG_TERMLIST
	debuglog << "TermList::Iterator::begin current " << current;
#endif
	if ( ! isValid()) nextValid();
	return current;
}

/****************************************************
**
**   TermList::Iterator   ---   nextValid
**
******************************************************/
void TermList::Iterator::nextValid()
{
#ifdef DEBUG_TERMLIST
	debuglog << "TermList::Iterator::Iterator nextValid current " << current;
#endif
	do
	{
		if ( current == nullptr )
		{
			return;
		}
		next();
	}
	// repeat until a valid combination is found (or return condition above is fulfilled)
	while ( ! isValid() );
}

/****************************************************
**
**   TermList::Iterator   ---   next
**
******************************************************/
void TermList::Iterator::next()
{
#ifdef DEBUG_TERMLIST
	debuglog << "TermList::Iterator::Iterator next current " << current;
	dumpLoopStatus();
#endif

	const uint obsize = planets.size();
	term.unincarnate();
	for( int i = varcount - 1; i >= 0; i-- )
	{
		assert( i >= 0 && i < MTerm::MAX_TERM_VARS );

		// simple case:
		// loop is running until obsize, so just increment if obsize-1 is not reached
		if ( loop_status[i] < obsize -1 )
		{
			++loop_status[i];
			current = &term;
			return;
		}
		else
		{
			if ( i > 0 )
			{
				if ( loop_status[i-1] < obsize -1 )
				{
					// go one element left and increment index
					++loop_status[i-1];

					// reset all fields right of i-1
					for( uint j = i; j < varcount; j++ )
					{
						if ( nextLoopStartsFromDiagonal( term.getTermId(), j ))
						{
							loop_status[j] = loop_status[j-1];
							// start after diagonal (no duplicate) is wrong, e.g. Sun+Sun
							//if ( loop_status[j] + 1 < obsize ) ++loop_status[j];
						}
						else
						{
							loop_status[j] = 0;
						}
					}
					return;
				}
				else
				{
					// goto next loop
					//debuglog << "nothing to do " << i;
				}
			}
			else // i == 0
			{
				current = nullptr;
			}
		}
	}
}

/****************************************************
**
**   TermList::Iterator   ---   isValid
**
******************************************************/
bool TermList::Iterator::isValid()
{
	incarnate();
#ifdef DEBUG_TERMLIST
	debuglog << "Term isvalid " << term << " termid " << term.getTermId();
#endif

	switch( term.getTermId())
	{
		case TermId::Sum:
		case TermId::AntiSum:
		case TermId::TripleSum:
		case TermId::AntiTripleSum:
		break;
		case TermId::Triple:
#ifdef DEBUG_TERMLIST
			debuglog << "TermList::Iterator::isValid Term " << term << " 1st " << term.getFirstPlanet() << " second " << term.getSecondPlanet() << " 3rd "  << term.getThirdPlanet();
#endif
			// only third must not be duplicate, e.g. sun + sun - moon is allowed
			if ( term.getSecondPlanet() == term.getThirdPlanet()) return false;
			if ( term.getFirstPlanet() == term.getThirdPlanet()) return false;
		break;
		default:
			if ( term.hasDuplicates()) return false;
		break;
	}
	return true;
}

/****************************************************
**
**   TermList::Iterator   ---   incarnate
**
******************************************************/
void TermList::Iterator::incarnate()
{
	const PlanetId ob1 = varcount > 0 ? planets[ loop_status[0] ] : PlanetId::None;
	const PlanetId ob2 = varcount > 1 ? planets[ loop_status[1] ] : PlanetId::None;
	const PlanetId ob3 = varcount > 2 ? planets[ loop_status[2] ] : PlanetId::None;
	term.incarnate( ob1, ob2, ob3 );
}

/****************************************************
**
**   TermList::Iterator   ---   nextLoopStartsFromDiagonal
**
******************************************************/
bool TermList::Iterator::nextLoopStartsFromDiagonal( const TermId &termId, const int &i )
{
#ifdef DEBUG_TERMLIST
	debuglog << "TermList::Iterator::nextLoopStartsFromDiagonal termId " << termId << " i " << i;
#endif

	assert( i > 0 );

	switch( i )
	{
		case 1: // creating second element
			switch( termId )
			{
				case TermId::Midpoint:
				case TermId::AntiMidpoint:
				case TermId::Sum:
				case TermId::AntiSum:
				case TermId::Triple:
				case TermId::TripleSum:
				case TermId::AntiTriple:
				case TermId::AntiTripleSum:
					return true;
				break;

				case TermId::ReflectionPoint:
				case TermId::Difference:
				case TermId::AntiReflectionPoint:
				case TermId::AntiDifference:
					return false;
				break;

				case TermId::Single:
				case TermId::AntiSingle:
				default:
					fatallog << "TermList::Iterator::nextLoopStartsFromDiagonal:Invalid input (1) " << i
						<< " for next loop, termId " << (int)termId;
					assert( false );
				break;
			}
		break;
		case 2: // third element of triple starts from zero because of non-commutative minus relations
			switch( termId )
			{
				case TermId::Triple:
				case TermId::AntiTriple:
					return false;
				break;
				case TermId::TripleSum:
				case TermId::AntiTripleSum:
					return true;
				break;
				default:
					fatallog << "TermList::Iterator::nextLoopStartsFromDiagonal:Invalid input (2) " << i
						<< " for next loop, termId " << (int)termId;
					assert( false );
				break;
			}
		break;

		default:
			fatallog << "TermList::Iterator::nextLoopStartsFromDiagonal:Invalid input (3) " << i
				<< " for next loop, termId " << (int)termId;
			assert( false );
		break;
	}
	return false;
}

/****************************************************
**
**   TermList::Iterator   ---   dumpLoopStatus
**
******************************************************/
void TermList::Iterator::dumpLoopStatus()
{
	Lang lang;
	wxString s;
	s = "LOOP: ";
	for( uint i = 0; i < varcount; i++ )
	{
		assert( loop_status[i] < planets.size());
		s << lang.getPlanetName( planets[loop_status[i]], Viewpoint::Western ) << "-";
	}
	debuglog << s;
}

