/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/EventFinder.h
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
#ifndef _EVENTFINDER_H_
#define _EVENTFINDER_H_

#include "Uranian.h"
#include "UranianHelper.h"

#include <atomic>
#include <list>
#include <thread>

class MTerm;
struct DoubleTerm;
struct UranianSection;

/*************************************************//**
*
* 
*
******************************************************/
class EventFinder // : public Expert
{
public:

	EventFinder( ContextProps_ptr, UranianSection* );

	~EventFinder();

	/**
	* This wrapper starts calculation for a time span from begin to end of the given year.
	*/
	void startCalculation( const int &year );

	/**
	* Creates a new thread for calculation and returns when calculation is started.
	*/
	void startCalculation( const double &jd1, const double &jd2 );

	/**
	* Cancels calculation and returns after cancellation. Event list is cleared.
	*/
	void cancelCalculation();

	/**
	* Returns when calculation is finished. 
	*/
	void waitForCalculation();

	/**
	* This method is used from EventFinderWidget to determine whether the event list widget can be updated.
	*/
	bool isCalculationRunning() const { return isCalculating.load(); }

	/**
	* Sorts the results according to sortOrder variable in UranianSection. sort() works only if calculation is not running (call isCalculationRunning() before)
	*/
	void sort();

	double getStartJD() const { return startJD; }

	double getEndJD() const { return endJD; }

	UranianSection *getUranianSection() const { return usection; }

	/**
	* getMatches() works only if calculation is not running (call isCalculationRunning() before). Otherwise empty list is returned
	*/
	const std::list<Match> &getMatches() const;

protected:

	/**
	* Main calculation method is excuted in a separate thread
	*/
	void calculate();

	void findIntervalls( const HoroscopeRole &role1, const HoroscopeRole &role2, const MatchFilter* );

	void calculateDRPart( DoubleTerm&, Horoscope_ptr h1, const double &lradix, const double &jd1,
		const double &jd2 );

	void calculateDDPart( DoubleTerm &a, Horoscope_ptr, const double &jd1, const double &jd2 );

	/**
	* Preparation of resources contains synchronization of ContextProperties, creation of Horoscope Instances etc.
	*/
	void prepare();

	Viewpoint getViewpoint() const;

	double startJD = 0.0;

	double endJD = 0.0;

	double duration = 0.0;

	/**
	* ContextProps are a copy of the original one (radix horoscope is separate)
	*/
	ContextProps_ptr props;

	/**
	* ContextProps given from outside.
	*/
	ContextProps_ptr orig_props;

	UranianSection *usection = nullptr;

	Planets obs;

	/**
	* Radix horoscope is a copy of the one original one because both can be accessed in read/write mode from different threads.
	*/
	Horoscope_ptr hradix;

	std::list<DivisorResiduum> reslist;

	std::list<Match> matches;

	/**
	* dummy list will be returned if getMatches is called during calculation
	*/
	const std::list<Match> dummy_matches;

	/**
	* Is set to true during calculation.
	*/
	std::atomic_bool isCalculating = false;

	/**
	* This variable will be checked during calculation which is canceled shortly if set to true.
	*/
	std::atomic_bool doCancel = false;

	// ?
	//std::atomic_int matchCount = 0;

	std::thread t;

};

#endif



