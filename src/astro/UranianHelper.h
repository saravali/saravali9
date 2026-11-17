/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/UranianHelper.h
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

#ifndef _URANIANHELPER_H_
#define _URANIANHELPER_H_

#include "astroconfig.h"
#include "bootstrap.h"
#include "constants.h"
#include "Planet.h"

#include <list>
#include <vector>

struct Match;
class MTerm;

/*************************************************//**
*
* 
*
******************************************************/
struct DivisorResiduum
{
	const int division = 1;

	const double residuum = 0;
};

/*************************************************//**
*
*  Various tools for symmetric astrology
*
******************************************************/
class UranianHelper : public Singleton<UranianHelper>
{
  friend class Singleton<UranianHelper>;
public:

	/**
	* All possible divisions depending on divisor primes and maximum division (configured in UranianConfig/UranianConflet)
	*/
	std::vector<int> getAllDivisions() const;

	std::vector<int> getAllDivisionPrimes() const;

	std::list<int> calculateDivisionsForPrimes( const std::set<int> &basePrimes, const int &maxValue ) const;

	/**
	* Returns the subset of divisors for a given division.
	*/
	std::vector<int> getDivisors( const int &division ) const;

	/**
	* Simply adds "D" to the division, e.g. 4 -> D-4
	*/
	wxString divisionDString( const int &division );

	/**
	* Number of degrees for a division, e.g. D-4 -> 90, D-16 -> 22:30
	*/
	wxString divisionDegreeString( const int &division );

	/**
	* Returns division and number of degrees, e.g. 8 -> D-8: 45
	*/
	wxString divisionFullString( const int &division );

	/**
	*  Creates a list of zodiacal points and divisors connected to a given division.
	*/
	std::list<DivisorResiduum> getDivisionResidua( const int &division );

	UranianContext calculateUranianContext( const ContextProps_ptr, const WidgetCategory& );

	/**
	* Creates a Match object if 2 longitudes match within a given orbis and division.
	*/
	Match calculateMatch( const double &len1, const double &len2, const double &orbis, const int &division );
	
	/**
	*  Creates a string description for a filter (shown on the sidebar below the filter buttons)
	*/
	wxString getFilterStringRepresentation( const MatchFilter& );

	/**
	*  Decides whether an MatchFilter applies to a pair of MTerm objects.
	*/
	bool applyMatchFilter( const MatchFilter&, MTerm*, MTerm* ) const;

	/**
	*  Display role context of a match. Used in MatchWriter.
	*/
	wxString getRoleTagForMatch( const Match& );

	/**
	*  Calculate house cusps according the reference PlanetId.
	*  Details for param PlanetId
	*  - Meridian: normal houses, Meridian is 10th cusp
	*  - Earth: sign based with Libra as first house cusp
	*  - Moon will be cusp of 10th
	*  - all other: planet as cusp of 4th house
	*
	*  house system is the same as configure for this viewpoint
	*/
	std::array<double, 12> calculateHouses( Horoscope_ptr, const Viewpoint&, const PlanetId = PlanetId::Meridian );

	void writeHouseReport( ContextProps_ptr, MDocument_ptr, const Viewpoint& );

private:

	std::list<int> calculateAllowedMultiples( const int &prime, const int &maxValue, const std::list<int> &multiples ) const;

};

#endif // _URANIANHELPER_H_

