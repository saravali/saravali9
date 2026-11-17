/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Ephemeris.h
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

#ifndef _EPHEMERIS_H_
#define _EPHEMERIS_H_

#include "Expert.h"
#include "constants.h"
#include "datelocation.h"
#include "mstring.h"
#include "GenericDasa.h"

#include <list>
#include <map>
#include <vector>

class DasaExpert;
class Table;
struct TermPath;

/*************************************************//**
*
*  \brief Data for an ingress event in EphemView
*
******************************************************/
struct IngressEvent
{
	const double jd = 0;
	const PlanetId planet = PlanetId::None;
	const int which = 0;
	const int type = 0;
};

/*************************************************//**
*
*  \brief Data for an aspect event in EphemView
*
******************************************************/
struct AspectEvent
{
	const double jd = 0;
	const PlanetId planet1 = PlanetId::None;
	const PlanetId planet2 = PlanetId::None;
	const double angle = 0;
};

/*************************************************//**
*
*  \brief Data for a Lunar event in EphemView
*
******************************************************/
struct LunarEvent
{
	const double angle = 0;
	const double jd = 0;
	const double slen = 0;
	const double mlen = 0;
};

/*************************************************//**
*
*  \brief Calculation of Ephemeris and related reports
*
******************************************************/
class EphemExpert : public Expert
{
public:

	EphemExpert( ContextProps_ptr );

	void setShowHeader( const bool b = true ) { show_header = b; }

	/**
	*  Reset internal variables for a new month. Calculation will be lazy according to required data.
	*/
	void prepareMonth( const int &year, const int &month );

	int getMonth() const { return month; }
	int getYear() const { return year; }

	/**
	*  Returns JD of the start of the month.
	*/
	double getStartJD() const { return start_jd; }

	/**
	*  Returns JD of the end of the last day of the month (12PM).
	*/
	double getEndJD() const { return end_jd; }

	/**
	*  Copy the current ephemeris data to a TermPath list. This is used as base fro graphical ephemeris.
	*/
	void initTermPaths( std::list<TermPath>& );

	MString getPosFormatted( const double &longitude, const MovingDirection = MovingDirection::None,
		const DegPrecision = DegPrecision::Minute, const int division = 1 );

	int writeCoordinates( MDocument_ptr, const CoordinateMode = CoordinateMode::Longitude, const int division = 1 );
	int writeIngress( MDocument_ptr );
	void writeKp( MDocument_ptr, const DasaId& );
	void writeSunrise( MDocument_ptr );
	void writeLunar( MDocument_ptr );

	const Planets &getPlanets() const { return obs; }

	int calculateIngress();

	const std::list<IngressEvent> &getIngressEvents() { return ingressEvents; }

	void calculateLunar();

	const std::list<LunarEvent> getLunarEvents() const { return lunarEvents; }

private:

	void init();

	int calculateCoordinates();

	void calculateSunrise();

	void testIngressEvent( const int &t1, const int &t2, const PlanetId &planet, const int &type, const bool &r );

	void calculateKP( const DasaId&, DasaExpert_ptr );

	void addLunarEvent( const int &stdate, const double& target, const double& sdiff, const double &ediff );
	void addLunarSpecialEvent( const int &stdate, const double& target, const double& sdiff, const double &ediff );

	void writeHeaderInfo( MDocument_ptr );

	std::pair<wxString, bool> writeDayShortcut( const double &jd );

	wxString getFormattedDate( const double &jd );

	DateLocation dl;

	Planets obs;

	std::map<PlanetId, std::vector<PlanetPosition>> data;

	int nb_days, year, month, current_day, current_month, current_year;
	bool in_current_month;

	bool calc_coordinates,
		calc_ingress,
		calc_kp,
		calc_sunrise,
		calc_lunar;

	bool dstchange = false;
	double start_jd = 0, end_jd = 0;

	const static inline int MAX_DAY = 32;

	double jd[MAX_DAY],
		st[MAX_DAY],
		sunrise[MAX_DAY],
		sunset[MAX_DAY],
		sunrise_sunlen[MAX_DAY],
		sunrise_moonlen[MAX_DAY],
		sunrise_angle[MAX_DAY];

	int weekday[MAX_DAY], tithi[MAX_DAY];
	DasaId mydasa;

	std::list<KPEvent> kpevents;
	std::list<IngressEvent> ingressEvents;
	std::list<AspectEvent> aspectEvents;
	std::list<LunarEvent> lunarEvents;

	bool show_header;

};

#endif


