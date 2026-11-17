/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Term.h
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
#ifndef _TERM_H_
#define _TERM_H_

#include "constants.h"
#include "mstring.h"

#include <vector>

enum class PlanetId;
enum class Viewpoint;
struct MatchFilter;

typedef std::pair<TermId, TermId> TermIdPair;

/*************************************************//**
*
* \brief Arithmetical set of factors, i.e. planetay objects, angles etc. (Uranian astrology).
*
* There are two states of terms
*  - abstract: like an abstract formula like p|q (midpoint of p and q)
*  - incarnated: the term is connected to specific objects in a chart e.g. Su|Ma.
*     Incarnated terms can be evaluated, i.e. longitude etc. can be determined.
*
* The status of MTerm consists of an enum TermId representing the kind of term
* and an array of PlanetId for the specific incarnation.
*
******************************************************/
class MTerm
{
public:

	/**
	* Create unincarnated term
	*/
	MTerm( const TermId termId = TermId::None )
		: MTerm( PlanetId::None, PlanetId::None, PlanetId::None, termId ) {}

	/**
	* Create term with one parameter
	*/
	MTerm( const PlanetId &ob, const TermId termId = TermId::Single )
		: MTerm( ob, PlanetId::None, PlanetId::None, termId ) {}

	/**
	* Create term with two parameters
	*/
	MTerm( const PlanetId &ob1, const PlanetId &ob2, const TermId &termId )
		: MTerm( ob1, ob2, PlanetId::None, termId ) {}

	/**
	* Create term with three parameters
	*/
	MTerm( const PlanetId&, const PlanetId&, const PlanetId&, const TermId& );

	TermId getTermId() const { return termId; }

	void changeTermId( const TermId& );

	static int getVariableCount( const TermId& );
	static bool isSingleTerm( const TermId &id ) { return getVariableCount( id ) == 1; }
	static bool isDoubleTerm( const TermId &id ) { return getVariableCount( id ) == 2; }
	static bool isTripleTerm( const TermId &id ) { return getVariableCount( id ) == 3; }
	static bool isSymmetric( const TermId& );
	static bool isAntiTermId( const TermId& );
	// minus without deeper nesting
	static bool isSimpleMinusFormula( const TermId& );

	int getVariableCount() const { return getVariableCount( termId ); }
	bool isSingleTerm() const { return getVariableCount() == 1; }
	bool isDoubleTerm() const { return getVariableCount() == 2; }
	bool isTripleTerm() const { return getVariableCount() == 3; }
	bool isSymmetric() const { return isSymmetric( termId ); }
	bool isAntiTerm() const { return isAntiTermId( termId ); }
	bool isSimpleMinusFormula() { return isSimpleMinusFormula( termId ); }

	bool isIncarnated() const { return obs[0] != PlanetId::None; }

	/**
	*  Find out if the incarnated term contains one of the planets.
	*
	* Return: true if found. False if not incarnated
	*/
	bool containsOneOf( const std::set<PlanetId>& ) const;

	/**
	*  Find out if Term is incarnated and has only one factor (e.g. "Sun", "Mars")
	*/
	bool isPrimitive() const { return isSingleTerm() && isIncarnated(); }

	/**
	*  Determine whether objects appear twice (if incarnated, otherwise false).
	*/
	bool hasDuplicates() const;

	/**
	*   Calculate the longitude (if incarnated).
	*/
	double evaluate( Horoscope_ptr, const Viewpoint& );

	/**
	*   Calculate the longitude for a given incarnation.
	*/
	double evaluate( const Horoscope_ptr, const Viewpoint&,
		const PlanetId &ob1, const PlanetId ob2 = PlanetId::None,
		const PlanetId ob3 = PlanetId::None );

	/**
	* evalulate the term and return longitude and speed packed into std::pair
	*/
	std::pair<double, double> evaluateSpeed( Horoscope_ptr, const Viewpoint&,
		const PlanetId &ob1, const PlanetId ob2, const PlanetId ob3 );

	/**
	* See above
	*/
	std::pair<double, double> evaluateSpeed( Horoscope_ptr, const Viewpoint& );

	/**
	* returns true if the filter conditions match.
	*/
  bool applyFilter( const MatchFilter& ) const;

	/**
	*   Replaces the abstract object placeholders with specific objects that can be evaluated.
	*/
	void incarnate( const PlanetId &ob1, const PlanetId ob2 = PlanetId::None, const PlanetId ob3 = PlanetId::None );

	/**
	*   Replaces the abstract object placeholders with specific objects (vector version).
	*/
	void incarnate( const std::vector<PlanetId>& );

	PlanetId getFirstPlanet() const { return obs[0]; }

	PlanetId getSecondPlanet() const { return obs[1]; }

	PlanetId getThirdPlanet() const { return obs[2]; }

	PlanetId getPlanetId( const uint& ) const;

	std::vector<PlanetId> getPlanetIds() const;

	/**
	*   Make the term abstract again.
	*/
	void unincarnate();

	/***
	* MString representation of the term, i.e. including potential symbols
	**/
	MString toMString( const Viewpoint = Viewpoint::Uranian ) const;

	/***
	* See above
	**/
	MString toMString( const PlanetConfig& ) const;

	/***
	* Left part of the string representation, e.g. Sun/Sat -> Sun
	**/
	MString firstMString( const Viewpoint&, const int *antisciaCfg = nullptr ) const;

	/***
	* See above
	**/
	MString firstMString( const PlanetConfig&, const int *antisciaCfg = nullptr ) const;

	/***
	* Right part of the string representation, e.g. Sun/Sat -> Sat
	**/
	MString secondMString( const Viewpoint& ) const;

	/***
	* See above
	**/
	MString secondMString( const PlanetConfig& ) const;

	/***
	* Operator of the string representation, e.g. Sun/Sat -> "/"
	**/
	static wxString getOperatorRepresentation( const TermId&, const int position = 0 );

	wxString getOperatorRepresentation( const int position = 0 ) const { return getOperatorRepresentation( getTermId(), position ); }

	friend MString& operator<< ( MString&, const MTerm& );

	friend mostream& operator<< ( mostream&, const MTerm& );

	friend bool operator==( const MTerm&, const MTerm& );

	friend bool operator!=( const MTerm &t1, const MTerm &t2 ) { return !( t1 == t2 ); }

	inline static constexpr int MAX_TERM_VARS = 3;

private:

	PlanetId obs[MAX_TERM_VARS];

	TermId termId;

};

/*****************************************************
**
**   TermComparator
**
******************************************************/
class TermComparator
{
public:
	TermComparator( const Planets &obs ) : obs( obs ) {}

	bool operator()( const MTerm&, const MTerm& ) const;

private:
	const Planets &obs;
};

/*****************************************************
**
**   ENUM   ---   TermStepType
**
******************************************************/
enum class TermStepType
{
	None = -1,
	Forward,
	Backward,
	Zero,
	Wrap
};

/*****************************************************
**
**   STRUCT   ---   TermStep
**
******************************************************/
struct TermStep
{
	TermStep( const double &jd, const TermStepType &type, const PlanetPosition &pos )
		: jd ( jd ),
		type( type ),
		pos( pos )
	{
	}

	double jd;

	TermStepType type;

	PlanetPosition pos;
};

/*****************************************************
**
**   STRUCT   ---   TermPath
**
******************************************************/
struct TermPath
{
	TermPath( const MTerm &term ) : term( term ) {}

	void sort();

	MTerm term;

	std::list<TermStep> steps;
};

#endif


