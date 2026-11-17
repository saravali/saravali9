/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Calculator.h
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

#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include "constants.h"
#include "Planet.h"
#include "Viewpoint.h"

#include <array>
#include <utility>
#include <wx/string.h>

struct DateLocation;
struct CustomAyanamsaConfig;

/*************************************************//**
*
*  data for a single eclipse
*
******************************************************/
struct EclipseData
{
	double jd = 0;
	double sunpos = 0;
	double moonpos = 0;
	PlanetId planet = PlanetId::None;
	int type = 0;
	wxString description;
};

/*************************************************//**
*
*  abstract class encapsulates all planetary calculations
*
******************************************************/
class Calculator
{
public:

	virtual ~Calculator() {}

	virtual void init() = 0;

	/**
	*  Return the version of the built-in calculator software.
	*/
	virtual wxString getVersion() = 0;

	virtual std::list<wxString> getBuiltinAyanamsaNames() const = 0;

	virtual wxString getBuiltinAyanamsaName( const int& ) const = 0;

	virtual int getBuiltinAyanamsaSize() const = 0;

	/**
	* type as follows:
	* - 0: tropical
	* - 1: custom
	* - other: use swe builtin (index type -2)
	*
	*/
	virtual double calcAyanamsa( const double &jd, const int &type ) = 0;

	virtual double calcCustomAyanamsa( const double &jd, const CustomAyanamsaConfig& ) = 0;

	virtual int getBuiltinPrecessionModelsSize() const = 0;

	virtual std::list<wxString> getPrecessionModeNames() const = 0;

	virtual CustomAyanamsaConfig getBuiltinAyanamsaParams( const int &type ) const = 0;

	virtual double calcSiderealTime( const double &jd , const double &longitude ) = 0;
	virtual void calcEps( double &eps, const double &jd ) = 0;
	virtual double calcDeclination( const double &longitude, const double &latitude, const double &eps ) = 0;

	virtual std::pair<double, double> calcSunRiseSunSet( const DateLocation& ) = 0;

	virtual double calcNextSolarEvent( const SolarEventType&, const double &jd, const double &lat, const double &lon ) = 0;

	/**
	*  Calculate the Julian date for the input params.
	*
	* The flag determines whether Gregorian Calendar (default) or Julian Calendar will be used.
	*/
	virtual double calc_jd( const int &year, const int &month, const int &day,
		const double &time, const bool gregflag = true ) = 0;

	virtual bool calc_and_check_jd( const int &year, const int &month, const int &day, const double &time, double &jd ) = 0;
	virtual bool isGregorianCalendarDate( const int &year, const int &month, const int &day ) = 0;
	virtual bool isGregorianCalendarDate( const double &jd ) = 0;

	/**
	*  Reverse calculation of day/month/year/time from Julian date.
	*/
	virtual std::tuple<int,int,int,double> getDateIntsFromJD( const double &jd ) = 0;

	virtual double calcSolarReturnJD( const DateLocation&, const int &year, const Viewpoint& ) = 0;

	/**
	*  Calculate the longitude of an ephemeris object.
	*/
	virtual double calcLongitude( const DateLocation&, const PlanetId&, const Viewpoint& ) = 0;


	/**
	*  Calculate the position and speed of an ephemeris object.
	*/
	virtual PlanetPosition calcPositionSpeed( const DateLocation&, const PlanetId&, const Viewpoint& ) = 0;

	/**
	*  Calculate the position and speed of an ephemeris object.
	*/
	virtual void calcPositionSpeed( const DateLocation&, const PlanetId &planet,
		double &slen, double &slat, double &sspeed, const Viewpoint &viewpoint ) = 0;

	/**
	*  Ascendant and meridian will ve calculated by armc (no speed).
	*
	* Order in return pair is
	* - ascendant
	* - mc
	*/
	virtual std::pair<double, double> calcAnglesByArmc( const double &armc, const double &geolat, const double &eps ) = 0;

	virtual std::array<double, 12> calcHousesByArmc( const double &armc, const double &geolat,
		const double &eps, const HouseSystem& ) = 0;

	/**
	*  Calculates armc, longitude and speed of ascendant and MC.
	*
	* Order of returned items is
	* - 0: amrc
	* - 1: longitude ascendant
	* - 2: speed of ascendant
	* - 3: longitude mc
	* - 4: speed of mc
	*/
	virtual std::array<double, 5> calcAnglesSpeed( const double &jd, const double &geolat, const double &geolon ) = 0;

	/** TODO private
	*  Calculate Ascendant for a Julian date, geographical latitude and longitude.
	*/
	virtual double calcAscendant( const double &jd, const double &lat, const double &lon, const Viewpoint& ) = 0;

	/**
	*  Find the next eclipse after the start date.
	*/
	virtual EclipseData calcNextEclipse( const double &jd, const PlanetId &planet, const int &type, const Viewpoint& ) = 0;

	/**
	*  Find out when an object reaches the target longitude.
	*/
	virtual double calcPlanetaryEvent( const DateLocation&, const double &targetlen, const PlanetId&, const Viewpoint& ) = 0;

	/**
	*  Find out when the angle between Sun and Moon will have the target value.
	*/
	virtual double calcSunMoonEvent( const DateLocation&, const double &targetdiff, double &slen, double &mlen ) = 0;

	virtual double getMinJD() const = 0;

	virtual double getMaxJD() const = 0;
};

/*************************************************//**
*
*  creation of Calculator
*
******************************************************/
class CalculatorFactory
{
public:
	Calculator *getCalculator();

private:
	static 	Calculator *calculator;
};


#endif

