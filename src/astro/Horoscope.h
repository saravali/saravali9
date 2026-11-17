/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Horoscope.h
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

#ifndef _HOROSCOPE_H_
#define _HOROSCOPE_H_

#include "Dasa.h"
#include "datelocation.h"
#include "Planet.h"

#include <array>

class ArabicPartsExpert;
class Calculator;
class VargaData;
struct VedicPlanetData;

/*************************************************//**
*
*   main astrological class for charts holding most features of Vedic astrology
*
******************************************************/
class Horoscope
{
public:

	Horoscope( const HoroscopeRole role = HoroscopeRole::Radix );

	Horoscope( const Horoscope& ) = delete;

	virtual ~Horoscope();

	HoroscopeRole getRole() const { return role; }
	void setRole( const HoroscopeRole &r ) { role = r; }

	// calculation of planetary positions
	virtual void update();

	// prepares internal storage for updated data. calculation is done lazy i.e. on demand
	void init_update();

	double getAyanamsa( const Viewpoint& );

	virtual double getLongitude( const PlanetId&, const Viewpoint& );

	virtual double getTropicalLongitude( const PlanetId &p )
		{ return getLongitude( p, Viewpoint::Astronomical ); }

	inline double getVedicLongitude( const PlanetId &p )
		{ return getLongitude( p, Viewpoint::Vedic ); }
	inline double getWesternLongitude( const PlanetId &p )
		{ return getLongitude( p, Viewpoint::Western ); }
	inline double getUranianLongitude( const PlanetId &p )
		{ return getLongitude( p, Viewpoint::Uranian ); }

	double getDeclination( const PlanetId&, const Viewpoint& );

	double getLatitude( const PlanetId &p, const Viewpoint& );

	/**
	* Speed of the object in deg/day. Must be corrected for directions (progressive, profctions).
	*/
	virtual double getLSpeed( const PlanetId&, const Viewpoint& );
	virtual double getBSpeed( const PlanetId&, const Viewpoint& );

	/**
	* MovingDirection depends on the Viewpoint because the Nodes (true/mean) can have different directions.
	*/
	MovingDirection getDirection( const PlanetId&, const Viewpoint& );

	/**
	* Retrogression depends on the Viewpoint because the Nodes (true/mean) can have different directions.
	*/
	bool isRetrograde( const PlanetId &p, const Viewpoint &viewpoint )
		{ return getDirection( p, viewpoint ) == MovingDirection::Retrograde; }

	bool hasSuspiciousLongitude( const PlanetId&, const Viewpoint& );

	// 2x delegate to viewpoint proxy
	double getHouseCusp( const House&, const Viewpoint& );
	double getHouseSandhi( const House&, const Viewpoint& );

	House getHousePos( const PlanetId& planet, const Viewpoint& );

	/**
	* House position for a given longitude.
	*
	* Return value is double. The fractional part reflects the portion of the house already consumed by the factor.
	*
	* Use the method below if House/integer values are required.
	*
	*/
	double getHouse4LongitudeDouble( const double &longitude, const Viewpoint& );

	House getHouse4Longitude( const double &longitude, const Viewpoint &viewpoint )
	{
		return (House)getHouse4LongitudeDouble( longitude, viewpoint );
	}

	double getArmc();

	double getSiderealTime();

	double getEps();

	// used by direction expert
	double getLongitudeOffset() const { return l_offset; }
	void setLongitudeOffset( const double &offset ) { l_offset = offset; }
	void setUniformMovingDirection( const MovingDirection &md ) { uniform_direction = md; }
	void setTransitJD( const double &jd ) { transitJD = jd; }
	double getTransitJD() const { return transitJD; }

	double getSunrise();
	double getSunset();
	bool getDayBirth();

	// 2xdelegate to viewpoint proxy
	const VargaData &getVargaData( const PlanetId&, const Varga&, const Viewpoint& );
	const VedicPlanetData &getVedicPlanetData( const PlanetId&, const Viewpoint& );

	bool isBenefic( const PlanetId&, const Viewpoint& );

	// Krishnamurti Paddhati: here always Vimsottari
	// 2x delegate to viewpoint proxy
	KpData getKPLords( const PlanetId&, const Viewpoint& );
	KpData getHouseKPLords( const House&, const Viewpoint& );

	DateLocation dl;

	// Date + location access simplifier
	double getJD() const { return dl.date.jd; }
	void setDate( const double &jd ) { dl.date.jd = jd; }

	double getLongitude() const { return dl.location.longitude; }
	double getLatitude() const { return dl.location.latitude; }
	double getTimeZone() const { return dl.location.tz; }

	// file methods
	void openFile( wxString filename );
	void saveAs( wxString filename );
	void save();

protected:

	HoroscopeRole role = HoroscopeRole::None;

	Calculator *calculator = nullptr;

	double siderealTime = 0.0;
	double armc = 0.0;
	double eps = 0.0;
	double sunrise = 0.0;
	double sunset = 0.0;
	bool isDayBirth = false;
	bool moonBenefic = false;

	bool isMercuryBenefic( const Viewpoint& );

	std::array<double, LAST_EXTENDED_FACTOR+1> longitude = { 0 };

	std::array<double, MAX_EPHEM_FACTORS> latitude;
	std::array<double, MAX_EPHEM_FACTORS> declination;
	std::array<double, MAX_EPHEM_FACTORS> lspeed;
	std::array<double, MAX_EPHEM_FACTORS> bspeed;

private:

	struct ViewpointData
	{
		double ayanamsa = 0.0;

		std::array<double, 12> house_cusp = { 0 };

		std::array<double, 12> house_sandhi = { 0 };

		// internal flags showing if items are already calculated
		bool calc_aya = false;

		bool calc_houses = false;

		bool calc_kp = false;

		bool calc_vargas = false;

		// will be initialized on demand if required
		std::map<PlanetId, VedicPlanetData> vedic_planets;
	};
	std::array<ViewpointData, VIEWPOINT_ARRAY_SIZE> vp_data;

	void calculateHouses( const Viewpoint& );

	void initVedicPlanets( const Viewpoint& );

	void updateVedicPlanet( const PlanetId&, const Viewpoint& );
	
	// special params for transit, shifted charts etc.
	double l_offset = 0.0;

	std::map<HoroscopeRole, double> longitudeOffsets;

	// uniform direction overrides calculated speed, e.g. for shifted transit methods
	MovingDirection uniform_direction = MovingDirection::None;

	double transitJD = 0.0;

	bool ok_calc_benefics = false;

	bool ok_calc_sunrise = false;

	bool ok_calc_vedic_planets = false;

	ArabicPartsExpert *arabic_expert = nullptr;

	// lazy calculation methods
	int calculate_object( const PlanetId& );

	virtual void calculate_angles();

	int getInternalObjectIndex( const PlanetId&, const Viewpoint& );

	void calculate_lagnas();
	void calculate_upagrahas();
	void calculate_sunrise_sunset();
	void calculate_benefics();

#ifdef DEBUG_SHARED_PTR_LIFECYLCE

	inline static int nb_created = 0;

	inline static int nb_deleted = 0;
#endif
};

#endif



