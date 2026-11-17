/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Transit.h
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
#ifndef _DIRECTION_H_
#define _DIRECTION_H_

#include "bootstrap.h"
#include "Expert.h"
#include "mstring.h"

enum class HoroscopeRole;
class MTerm;

/*************************************************//**
*
*  Frontend class for Direction calculation.
*
* Params: the constructor ContextProps_ptr is expected to have
* - Radix horoscope with HoroscopeRoles:: Radix
* - At least one Direction Role.
*
*
******************************************************/
class DirectionExpert : public Expert
{
public:

	/**
	*
	* Params: ContextProps_ptr is expected to have
	* - Radix horoscope with HoroscopeRoles:: Radix
	* - At least one Direction Role.
	*
	*/
	DirectionExpert( ContextProps_ptr );

	/**
	* Calculate directions for a single role. Calculation is delegated to DirectionCalculator.
	*
	* The horoscope with direction role can be used to get longitudes etc.
	*
	* Restriction: speed does not alwyas work correctly.
	*/
	void calculateDirections( const double &transitJD, const HoroscopeRole& );

	/**
	*  Set roles for multi Direction calculation.
	*
	*  The required Horoscope will be set up if necessary.
	*
	*/
	void setDirectionModes( const std::list<HoroscopeRole>& );

	/**
	*  Update all direction horoscopes (1..n) in ContextProps.
	*/
	void calculateMultiDirections( const double &jd );

	/**
	*  Writes the direction header shown e.g. in Direction widget text control.
	*/
	void writeDirectionReport( MDocument_ptr );

	/**
	*  Writes the multi direction header shown e.g. in Direction widget text control.
	*/
	void writeMultiDirectionReport( MDocument_ptr );

	// for debugging only
	void showAllHoroscopeRoles();

protected:

	void writeDirectionHeader( MDocument_ptr );

	MString describeRule( const HoroscopeRole&, const Viewpoint& );

	std::list<HoroscopeRole> getValidDirectionRoles() const;
};

/*************************************************//**
*
* Stateless calculation class for direction.
*
* Main users are DirectionExpert and TermSolver.
*
*
******************************************************/
class DirectionCalculator : public Singleton<DirectionCalculator>
{
  friend class Singleton<DirectionCalculator>;

public:

	/**
	* Finds out if a HoroscopeRole is Direction based, e.g. Transit, Progression, Solar Arc etc,
	*/
	bool isDirectionRole( const HoroscopeRole& );

	/**
	* Finds out if a HoroscopeRole is a shifted Direction role, e.g. Solar Arc, Profection.
	*/
	bool isShiftedRole( const HoroscopeRole& );

	/**
	* Simple wrapper for transformation of transit to progressive date.
	*/
	double transitToProgressiveJD( const double &birthJD, const double &transitJD );

	/**
	* TODO comment
	* TODO braucht man das überhaupt
	*/
	double transitToDirectionJD( const Horoscope_ptr radixHoroscope, const Viewpoint&, const double &transitJD,
		const HoroscopeRole &directionRole );

	/**
	* Simple wrapper for transformation of progressive to transit date.
	*/
	double progressiveToTransitJD( const double &birthJD, const double &progressiveJD );

	/**
	* Main method for Direction calculation. Both Horoscope params must be valid (not null and valid roles).
	*
	* Return: speed in case of shifted roles, zero otherwise.
	*
	*/
	double calculateDirection( Horoscope_ptr radixHoroscope, Horoscope_ptr directionHoroscope, const Viewpoint& );

	/**
	* This method encapsulated arc calculation (including speed).
	*
	* Param radixHoroscope is read only.
	*
	* DirJD must be set correctly
	* - progressive JD for most roles like progressive, solar arc etc
	* - lunation JD for profections
	*
	* Return: pair of arc value and speed.
	* TODO speed multiplicator?
	*
	*/
	std::pair<double, double> calcArcSpeed( Horoscope_ptr radixHoroscope, const Viewpoint&,
		const double &transitJD, const double &dirJD, const HoroscopeRole &directionRole );

	/**
	* Calculates JD for Synodic Lunations.
	*/
	double calculateLunationDate( Horoscope_ptr radixHoroscope, const Viewpoint&, const double &transitJD );

	// TODO commment
	double getSpeedMultiplicator( const HoroscopeRole& ) const;

private:

	DirectionCalculator();

	const double yearLength = 0.0;

	constexpr static inline double synodic_period = 29.53;

};


#endif // _DIRECTION_H_

