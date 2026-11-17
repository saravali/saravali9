/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Expert.h
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

#ifndef _EXPERT_H_
#define _EXPERT_H_

#include "bootstrap.h"
#include "constants.h"
#include "Varga.h"

enum class PlanetId;
enum class Viewpoint;

/*************************************************//**
*
* Base class for Expert instances, i.e. classes implementing astrological logic.
*
* ## Viewpoint Handling
* Expert has a constant sticky viewpoint parameter.
+
+ TODO
* The viewpoint param is important for retrieving the right view of the underlying data
* (see comment to getViewpoint method).
*
* ## Access to planet properties
*
* TODO
*
******************************************************/
class Expert 
{
public:
	Expert( ContextProps_ptr, const Viewpoint &stickyVp );

	/**
	* Actual Viewpoint is calculated from
	* - the sticky Viewpoint (constant characteristic for that kind of Expert)
	* - the Viewpoint dynamically selected by the user
	* - the Viewpoint policy stored in astroconfig->general.
	*
	*/
	Viewpoint getViewpoint() const;

	ContextProps_ptr getProps() const { return props; }

	/**
	* Retrieves the factor's longitude relative to the actual Viewpoint
	*/
	double getLongitude( const PlanetId&, const HoroscopeRole = HoroscopeRole::Radix ) const;

	Nakshatra27 getNakshatra27( const PlanetId&, const HoroscopeRole = HoroscopeRole::Radix ) const;

	Nakshatra28 getNakshatra28( const PlanetId&, const HoroscopeRole = HoroscopeRole::Radix ) const;

	/**
	* Retrieves the factor's retrograssion status relative to the actual Viewpoint
	*/
	bool isRetrograde( const PlanetId&, const HoroscopeRole = HoroscopeRole::Radix ) const;

	MovingDirection getDirection( const PlanetId &factorId, const HoroscopeRole = HoroscopeRole::Radix ) const;

protected:

	ContextProps_ptr props;

	const Viewpoint stickyVp;
};

/*************************************************//**
*
* Base class for Expert subclasses having Vedic sticky Viewpoint and a fixed Varga.
*
******************************************************/
class VedicExpert : public Expert
{
public:

	VedicExpert( ContextProps_ptr props, const Varga varga = Varga::Rasi )
		: Expert( props, Viewpoint::Vedic ),
		varga( varga )
		{
		}

	VargaData getVargaData( const PlanetId&, const HoroscopeRole = HoroscopeRole::Radix ) const;

	VargaData getVargaData( const PlanetId&, const Varga&, const HoroscopeRole = HoroscopeRole::Radix ) const;

	Sign getRasi( const PlanetId&, const HoroscopeRole = HoroscopeRole::Radix ) const;

	Varga varga;

};

#endif // _EXPERT_H_


