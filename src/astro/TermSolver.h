/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/TermSolver.h
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
#ifndef _TERMSOLVER_H_
#define _TERMSOLVER_H_

#include "bootstrap.h"

class MTerm;
struct TermPath;
enum class HoroscopeRole;
enum class PlanetId;
enum class Viewpoint;

/*****************************************************
**
**   CLASS   ---   TermSolver
**
******************************************************/
class TermSolver
{
public:

	TermSolver( Horoscope_ptr horoscope, const Viewpoint &viewpoint )
		: horoscope( horoscope ),
		viewpoint( viewpoint )
	{
	}

	/**
	* Find the JD when a MTerm is at a given target longitude.
	*
	* Radix horoscope and viewpoint must be given in the constructor.
	*
	* Search starts at startjd that should be appropriatly near the solution.
	*
	* Used by:
	* DasaTool: calculate dates for Nakshatra and Pada ingress
	*
	* Return: match JD or zero on error
	*/
	double findRadixMatchDate( MTerm*, const double &startjd, const double &targetlen );

	/**
	* Find matchings for equalities with radix terms on both sides.
	*
	* The equation is interpreted like t1 - t2 = residuum
	*
	* Search starts at startjd that should be appropriatly near the solution.
	*
	* Used by:
	* UranianExpert (calculate due dates)
	*
	* Return: match JD or zero on error
	*/
	double findRadixMatchDate( MTerm*, MTerm*, const double &startjd, const double &residuum );

	/**
	* Find the JD when a planet is at a given target declination
	*
	* Radix horoscope and viewpoint must be given in the constructor.
	*
	* Search starts at startjd that should be appropriatly near the solution.
	*
	* Used by:
	* EphemDiagram via insertDeclinationZeros
	*
	* Restriction: only single terms (aka planets instead of full MTerms) are supported.

	* Return: match JD or zero on error
	*/
	double findDeclinationMatchDate( const PlanetId&, const double &startjd, const double &targetd );

	/**
	* Find matchings for equalities with progressive terms on both sides.
	*
	* Param startjd must progressive date.
	* 
	* Used by:
	* UranianExpert (calculate due dates)
	* 
	* Return: match JD or zero on error
	*/
	double findInterProgressionMatchDate( MTerm*, MTerm*, const double &startjd, const double &residuum );

	/**
	* Find matchings for equalities with direction terms.
	*
	* Param startjd must direction date.
	* 
	* Used by:
	* UranianExpert (calculate due dates)
	* 
	* Return: match JD or zero on error
	*/
	double findDirectionMatchDate( MTerm*, const HoroscopeRole &directionRole, const double &startjd, const double &targetlen );

	/**
	* Simple wrapper for the method above using single shifted term "Aries"
	*/
	double findJDForArc( const HoroscopeRole &shiftedRoleconst, const double &startJD, const double &targetArc );


	/**
	* Finds wraps in the given path, i.e. planetary positions crossing the boundaries of division chart.
	*
	* The dates of boundary crossings will be calculated and added to path.
	* 
	* Used by:
	* EphemDiagram
	*
	*/
	void fitTermPathToBoundaries( TermPath&, const int &division );

	/**
	* Search along the path. find declination sign changes and calculate the exact dates of declination zeros.
	*
	* The results will be added to the path.
	* 
	* Used by:
	* EphemDiagram
	*
	*/
	void insertDeclinationZeros( TermPath& );

protected:

	Horoscope_ptr horoscope;

	const Viewpoint viewpoint;

private:

	// maximum number of loops in Newton algorithm
	constexpr static inline int max_loop = 10;

	// required exactness for JD (due date) in Newton algorithm
	constexpr static inline double targetdiff = .00001;
};

#endif



