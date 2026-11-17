/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/astrobase.h
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
#ifndef _ASTROBASE_H_
#define _ASTROBASE_H_

#include "constants.h"
#include "mstring.h"
#include "Planet.h"
#include "Viewpoint.h"

#include <wx/string.h>

/**************************************************************
***
**  Variuos mathematical funtions
***
***************************************************************/
//double calcYearLength( const int sel, const bool vedic );

// whether len is between to deg values
int between( const double &len, const double &min, const double &max );

// return the number of days for a given month and an given year, including leap years
int calcMonthLength( int monat, int jahr );

// year - month - day
void getTimeSpanInts( int &year, int &month, int &day, const double &length, const bool &vedic );
wxString getTimeSpanFormatted( const double &length, const bool &vedic, const TextFormat = TextFormat::Long  );
wxString getDurationYmd( const double &length, const bool highPrecision = false );

/**************************************************************
***
**  Basic astrological funtions
***
***************************************************************/
// get the rasi for a given length (0..11)
Sign getRasi( const double &len );

// get the longitude in a specific rasi - reverse will be counted from the end (0..30)
double getRasiLen( const double &len, const bool &reverse = false );

Element getElement( const double &len );
SignQuality getSignQuality( const double &len );
std::pair<ChartSide,ChartSide> getChartSides( const House& );

// get the opposite point
double getOpposition( const double& );

double getAntiscium( const double& );

// whether len is in odd, even or odd footed rasi
int isOddRasi( const double &len );
int isEvenRasi( const double &len );
int isOddFootedRasi( const Sign &rasi );

// get the varna of a longitude
Varna getVarna( const double &len );

// whether len is in ascendant, kona, kendra resp. to ascendant length
int inAscendant( const double &len, const double &asc );
int inTrikona( const double &len, const double &asc );
int inKendra( const double &len, const double &asc );

// whether len is in movable, fixed or dual sign
int inMovableSign( const double &len );
int inFixedSign( const double &len );
int inDualSign( const double &len );

// get the lord of a rasi (without nodes, otherwise use JaiminiExpert)
PlanetId getLord( const Sign&, const Viewpoint& );
PlanetId getLord( const Sign&, const bool vedic = true );

// Get the rasi where planet is exalted
Sign getExaltationRasi( const PlanetId& );

// whether planet is exalted or debilated in rasi
bool hasExaltationRasi( const PlanetId&, const Sign& );
bool hasDebilationRasi( const PlanetId&, const Sign& );

// return the mulatrikona rasi for planet
Sign getMulatrikonaRasi( const PlanetId& );

// whether planet with len is in mulatrikona (only for valid deg longitude)
bool isInMulatrikona( const PlanetId&, const double &len );

// The same if only the rasi is known
bool hasMulatrikonaRasi( const PlanetId&, const Sign& );

// naisargika mitra
PlanetaryFriendship getPlanetaryFriendship( const PlanetId&, const PlanetId& );
bool isPlanetaryFriend( const PlanetId&, const PlanetId& );
bool isPlanetaryEnemy( const PlanetId&, const PlanetId& );

// tatkalika mitra
PlanetaryFriendship getRasiTemporaryFriendship( const Sign&, const Sign& );
bool isRasiTempFriend( const Sign&, const Sign& ); 

// Name of the avastha ( Sayana, Upavesana, etc.)
const wxString getAvasthaName( const int &i , const int avastha = 0 );

/**************************************************************
***
**  Jaimini and Nakshatras
***
***************************************************************/
// get the longitude of a planet in his nakshatra
double getNakshatraLongitude27( const double& );
double getNakshatraLongitude28( const double& );

// get the nakshatra for a given length
Nakshatra27 getNakshatra27( const double &len );
Nakshatra28 getNakshatra28( const double &len );

// number of pada for a given length
int getPada( const double &len );

// whether 2 rasi positions have a rasi drishti
bool hasJaiminiAspect( const Sign&, const Sign& );

// whether 2 rasis are vishama of sama pada
bool isVishamapadaRasi( const Sign &rasi );
bool isSampadaRasi( const Sign &rasi );

double getNavamsaPortion( const double &len, const PlanetId &pindex, const bool &retro = false );

Vashya getVashya( const double &len );

/**************************************************************
***
**  Western astrology
***
***************************************************************/
double calcGeometricalMidpoint( const double &len1, const double &len2 );

#endif


