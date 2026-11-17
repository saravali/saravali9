/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/VedicAspect.h
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
#ifndef _VEDICASPECT_H_
#define _VEDICASPECT_H_

#include "AspectConfig.h"
#include "Expert.h"

#include <list>

struct ChartConfig;
class Table;
enum class PlanetId;

/*************************************************//**
*
* \brief Data container for Vedic aspects
*
******************************************************/
struct VedicAspect
{
	bool isVargaChartDrishti() const
		{ return type == VedicAspectId::GrahaDrishti || type == VedicAspectId::RasiDrishti; }

	bool isVedha() const { return type == VedicAspectId::Vedha; }

	// can be Sign or Nashatra
	const int aspecting_f = -1;

	// can be Sign or Nashatra
	const int aspected_f = -1;

	const VedicAspectId type = VedicAspectId::None;

	const VedicAspectStrength strength = VedicAspectStrength::None;

	const bool benefic = true;
};

/*************************************************//**
*
*  Calculation of Vedic aspects and corresponding reports
*
******************************************************/
class VedicAspectExpert : public Expert
{
public:

	/**
	* The ChartConfigVarga param can be used to override the default configuration
	* in gdiconfig (used in ChartConflet with temporary config)..
	*/
	VedicAspectExpert( ContextProps_ptr, ChartConfig* = nullptr );

	/**
	* Rasi drishti: returns three aspected signs from a given sign
	*/
	static std::array<Sign, 3> getRasiDrishtis( const Sign &aspectingSign );

	/**
	* Graha drishti: returns the strength of the Graha drishti of a given planet to a given rasi difference.
	*
	* Return value is enum class VedicAspectStrength (=Full, P75 etc.)
	*/
	static VedicAspectStrength getSingleGrahaDrishti( const PlanetId &aspectingPlanet, const int &rasidiff );

	/**
	* Graha drishti: returns the strength of the Graha drishti of a given planet to a given rasi difference.
	*
	* Return value is numeric, e.g. 0.75 for a 3/4 aspect
	*/
	static double getGrahaDrishtiValue( const PlanetId &aspectingPlanet, const int &rasidiff );

	/**
	* Graha drishti: returns a list of all aspected signs for a given planet in given sign.
	*
	* The parameter minimumStrength limits the output to aspects having at least that strength.
	*/
	static std::list<std::pair<Sign, VedicAspectStrength>>
		getGrahaDrishtis( const PlanetId &aspectingPlanet, const Sign &sign, const VedicAspectStrength &minimumStrength );

	/**
	* Sputa drishti: returns a numeric value (between 0 and 60) for a given planet on a given longitude difference.
	*/
	static double getSputaDrishti( const PlanetId &aspectingPlanet, const double &ldiff );

	std::list<VedicAspect> calculateVargaChartAspects( const Varga&, Horoscope_ptr aspecting_h, Horoscope_ptr aspected_h );

	std::list<VedicAspect> calculateSbcAspects( Horoscope_ptr ) const;

	void writeVedicAspectarium( MDocument_ptr, Horoscope_ptr, const bool show_header = true );

protected:

	ChartConfig *getChartConfig() const;

	ChartConfig *myChartConfig = nullptr;

};

#endif // _VEDICASPECT_H_

