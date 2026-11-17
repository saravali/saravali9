/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Uranian.h
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
#ifndef _URANIAN_H_
#define _URANIAN_H_

#include "astroconfig.h"
#include "Expert.h"
#include "constants.h"
#include "mstring.h"

#include <list>

class MTerm;
struct MatchFilter;
struct UranianSection;

/*************************************************//**
*
* \brief Container for a single element of a Uranian match
*
******************************************************/
struct MatchElement
{
	MatchElement( const HoroscopeRole &role, MTerm *formula, const double &longitude )
	 : role( role ),
	 formula( formula ),
	 longitude( longitude )
	{
	}

	MatchElement( const MatchElement& );

	MatchElement& operator= ( const MatchElement& ) = delete;

	~MatchElement();
	
	friend mostream& operator<< ( mostream&, const MatchElement& );

	friend MString& operator<< ( MString&, const MatchElement& );

	wxString debugDump();

	const HoroscopeRole role;

	MTerm *formula;

	const double longitude;
};

/*************************************************//**
*
* \brief Uranian event between two elements
*
******************************************************/
struct Match
{
	Match( const int division = 0, const double orbis = 0, const double residuum = 0 )
		: division( division ),
		orbis( orbis ),
		residuum( residuum ) {}

	Match( const Match& );

	Match& operator= ( const Match& ) = default;

	~Match()
	{
		if ( e1 ) delete e1;
		if ( e2 ) delete e2;
	}

	bool isValid() const { return division > 0; }

	uint formulaId = UINT_FOR_NOT_FOUND;

	/**
	* \brief Positive integer describing the division, e.g. 1, 2, 4, 8 etc.
	*/
	int division = 0;

	/**
	* \brief Orbis in degrees
	*/
	double orbis = 0;

	/**
	* \brief Angle of the exact match (i.e. 24, 90, 180 etc.)
	*/
	double residuum = 0;

	/**
	* \brief Julian Date of due date (if set)
	*/
	double duedate = 0;

	/**
	* \brief longitude of the match in degrees
	*/
	double lspeed = 0;

	/**
	* \brief Speed of declination
	*/
	double bspeed = 0;

	/**
	* \brief Arc between longitudes of first and second match
	*/
	double arc = 0;

	MatchElement *e1 = nullptr;

	MatchElement *e2 = nullptr;
};

/*************************************************//**
*
*  \brief Match calculation for Uranian astrology 
*
******************************************************/
class UranianExpert : public Expert
{
public:
	UranianExpert( ContextProps_ptr props, UranianSection *usection )
		: Expert( props, Viewpoint::Uranian ),
			usection( usection )
	{
	}

	void calculateRadix();

	void calculateDirections( const double &transitjd );

	void calculateMultiDirections( const double &transitjd, const std::list<RoleRestrictor>& );

	void calculateSolar();

	void calculatePartner();

	std::list<Match> &findMatches( Horoscope_ptr, Horoscope_ptr, const double &orbis, const MatchFilter* = nullptr );

	void clear();

	int getMatchCount() const { return matches.size(); }

	const std::list<Match> &getMatches() { return matches; }

	UranianSection *getUranianSection() const { return usection; }

	void sort();

private:

	void calculateDirectionDueDates( Horoscope_ptr, const double &basejd );

	double calculateSpeed( Horoscope_ptr horoscope1, MTerm *lterm, Horoscope_ptr horoscope2, MTerm *rterm, const Viewpoint& );

	// TODO raus
	double getCorrectedDirectionSpeed( const HoroscopeRole&, const double &speed );

	Planets obs;

	std::list<Match> matches;

	UranianSection *usection;
};

/*************************************************//**
*
*  \brief sorts Uranian matches according to the given order
*
******************************************************/
class MatchSorter
{
public:
	MatchSorter( const EventSortOrder &order, const Planets &obs )
		: order( order ), obs( obs ) {}

	bool operator()( const Match&, const Match& ) const;

	const EventSortOrder order;

	const Planets &obs;
};

#endif



