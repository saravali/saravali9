/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/astrobase.cpp
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

#include "astrobase.h"

#include "astroconfig.h"
#include "Formatter.h"
#include "logger.h"
#include "mathbase.h"

#include <format>
#include <math.h>
#include <sstream>
#include <string.h>

extern Astroconfig *astroconfig;

#define DEBUG_ASTROBASE

/**************************************************************
***
**
***
***************************************************************/
void getTimeSpanInts( int &year, int &month, int &day, const double &length, const bool &vedic )
{
	double yeard, monthd, dayd;

	yeard = length/ ( vedic ? astroconfig->dasa.yearLength.getValue() : astroconfig->directions.yearLength.getValue() );
	year = (int)yeard;
	monthd = ( yeard - (double)year ) * 12;
	month = (int)monthd;
	dayd = ( monthd - (double)month ) * 30;
	dayd += .000000000001;
	day = (int)dayd;
}

/**************************************************************
***
**
***
***************************************************************/
wxString getTimeSpanFormatted( const double &timespan, const bool &vedic, const TextFormat format )
{
	int years, months, days;
	wxString s;

	getTimeSpanInts( years, months, days, timespan, vedic );
	
	if ( format == TextFormat::Short )
	{
		return std::format( "{}-{}-{}", years, months, days );
	}
	if ( years > 0 )
	{
		s << std::format( "{} ", years );
		if ( format == TextFormat::Long )
		{
			if ( years == 1 ) s << tr( "year" );
			else s << tr( "years" );
		}
		else
		{
			s << "y";
		}
		s << " ";
	}

	if ( months > 0 )
	{
		s << std::format( " {}", months );
		if ( format == TextFormat::Long )
		{
			if ( months == 1 ) s << tr( "month" );
			else s << tr( "months" );
		}
		else
		{
			s << "m";
		}
		s << " ";
	}

	if ( days > 0 )
	{
		s << std::format( "{} ", days );
		if ( format == TextFormat::Long )
		{
			if ( days == 1 ) s << tr( "day" );
			else s << tr( "days" );
		}
		else
		{
			s << "d";
		}
		s << " ";
	}
	if ( format == TextFormat::Long )
	{
		const double hours = 24.0 * ceil( timespan );
		//if ( ! s.empty() ) s << " ";
		if ( fabs( hours ) > .001 )
		{
			s << " ";
			s << Formatter::get()->formatTime( 24.0 * floor( timespan ));
		}
	}

	return s;
}

/**************************************************************
***
**
***
***************************************************************/
void getDurationInts( int &year, int &month, int &day, double length )
{
	double yeard, monthd, dayd;

	yeard = length/ astroconfig->dasa.yearLength.getValue();
	year = (int)yeard;
	monthd = ( yeard - (double)year ) * 12;
	month = (int)monthd;
	dayd = ( monthd - (double)month ) * 30;
	dayd += .00001;
	day = (int)dayd;
}

/**************************************************************
***
**
***
***************************************************************/
wxString getDurationYmd( const double &length, const bool highPrecision )
{
	constexpr double QUASI_ZERO = .0001;
	wxString s;

	if ( ::fabs( length ) < QUASI_ZERO ) return "(zero)";

	const double yeard = length/ astroconfig->dasa.yearLength.getValue();
	const int years = (int)( yeard + QUASI_ZERO );

	const double monthd = ( yeard - (int)yeard ) * 12.0;
	const int months = (int)(monthd + QUASI_ZERO );

	const double dayd = ( monthd - (int)monthd ) * 30.0;
	const int days = (int)( dayd + QUASI_ZERO );

	const double hourd = ( dayd - (int)dayd ) * 24.0;

	if ( years > 0 ) s += std::format( "{}Y ", years );
	if ( months > 0 ) s += std::format( "{}M ", months);
	if ( days > 0 ) s += std::format( "{}D ", days );

#ifdef DEBUG_ASTROBASE
	debuglog << std::format( "getDurationYmd {:.10f} yeard {} monthd {} dayd {} hourd {}", length, yeard, monthd, dayd, hourd );
#endif

	if ( ( highPrecision || ( years == 0 && months == 0 )) && ::fabs( hourd ) > QUASI_ZERO && ::fabs( hourd - 24 ) > QUASI_ZERO )
	{
		s += Formatter::get()->formatTime( hourd );
	}
	return s;
}

/**************************************************************
***
**
***
***************************************************************/
double getNavamsaPortion( const double &len, const PlanetId &pindex, const bool &retro )
{
	double navamsavastha = 0;
	double nav_portion;
	int start_nav;

	assert( PlanetHelper::isInnerPlanet( pindex ));

	if ( ! retro )
	{
		nav_portion = a_red( len, 30 ) / 3.3333333333;
		start_nav = red12( (int)getRasi( len ) * 9 );
	}
	else
	{
		nav_portion = ( 30 - a_red( len, 30 )) / 3.3333333333;
		start_nav = red12( (int)getRasi( len ) * 9 + 8 );
	}

	int inav_portion = (int)nav_portion;
	Sign nav_rasi;
	double portion;
	navamsavastha = 0;

	for ( int i = 0; i <= inav_portion; i++ )
	{
		if ( i < inav_portion ) portion = 1;
		else portion = nav_portion - (int)nav_portion;

		if ( ! retro )
		{
			nav_rasi = (Sign)red12( start_nav + i );
		}
		else
		{
			nav_rasi = (Sign)red12( start_nav - i );
		}

		if ( hasExaltationRasi( pindex, nav_rasi ))
		{
			navamsavastha += 2 * portion;
		}
		else if ( getLord( nav_rasi ) == pindex )
		{
			navamsavastha += 2 * portion;
		}
		else if ( hasDebilationRasi( pindex, nav_rasi ))
		{
			navamsavastha -= portion;
		}
		else if ( isPlanetaryEnemy( pindex, getLord( nav_rasi )))
		{
			navamsavastha -= portion;
		}
		else
		{
			navamsavastha += portion;
		}
	}
	if ( navamsavastha  < 0 ) navamsavastha = 0;
	return navamsavastha;
}

/**************************************************************
***
**
***
***************************************************************/
double getOpposition( const double &v )
{
	return red_deg( v + 180.0 );
}
/**************************************************************
***
**
***
***************************************************************/
double getAntiscium( const double &len )
{
	return red_deg( 90.0 + ( 90.0 - len ));
}

/**************************************************************
***
**  gtRasi, element, sign quality
***
***************************************************************/
Sign getRasi( const double &len )
{
	return( (Sign)(int)( red_deg( len ) / 30 )  );
}
Element getElement( const double &len )
{
	const int sign = (int)( red_deg( len ) / 30 );
	return (Element)(sign % 4 );
}
SignQuality getSignQuality( const double &len )
{
	const int sign = (int)( red_deg( len ) / 30 );
	return (SignQuality)(sign % 3 );
}
std::pair<ChartSide,ChartSide> getChartSides( const House &house )
{
	assert_valid_enum<House>( house );
	std::pair<ChartSide,ChartSide> p;

	if ( house >= House::h1 && house <= House::h6 )
		{ p.first = ChartSide::South; }
	else
		{ p.first = ChartSide::North; }

	if (( house >= House::h10 && house <= House::h12 )
		|| ( house >= House::h1 && house <= House::h3 ))
		{ p.second = ChartSide::East; }
	else
		{ p.second = ChartSide::West; }

	return p;
}

#define USHADHA_BEGIN 266.6666666667
#define ABHIJIT_BEGIN 276.6666666667
#define SRAVANA_BEGIN 280.8888888889
#define DHANISHTA_BEGIN 293.6666666667

#define USHADHA_LEN 10
#define ABHIJIT_LEN 4.2222222222 // = 4:13:20;
#define SRAVANA_LEN 12.444444444 // = 12:26:40
/**************************************************************
***
**
***
***************************************************************/
double getCorrectedNakshatraLength( const double &len )
{
	// incl. P. Shadha
	if ( len <= USHADHA_BEGIN ) return len;

	// U. Shadha
	else if (( len > USHADHA_BEGIN ) && ( len <= ABHIJIT_BEGIN  ))
		return (int)Nakshatra28::Uttarashadha * NAKSHATRA_LEN + NAKSHATRA_LEN / USHADHA_LEN * ( len - USHADHA_BEGIN );

	// Abhijit: upto 280:53:20
	else if (( len > ABHIJIT_BEGIN ) && ( len <= SRAVANA_BEGIN  ))
		return (int)Nakshatra28::Abhijit * NAKSHATRA_LEN + NAKSHATRA_LEN / ABHIJIT_LEN * ( len - ABHIJIT_BEGIN );

	// Sravana
	else if (( len > SRAVANA_BEGIN ) && ( len <= DHANISHTA_BEGIN  ))
		return (int)Nakshatra28::Sravana * NAKSHATRA_LEN + 1.071428571 * ( len - SRAVANA_BEGIN );

	// rest add one nakshatra
	else return len + NAKSHATRA_LEN;
}

/**************************************************************
***
**
***
***************************************************************/
Nakshatra27 getNakshatra27( const double &len )
{
	return( (Nakshatra27)(int)a_red( red_deg( len ) / NAKSHATRA_LEN, 27 ));
}

/**************************************************************
***
**
***
***************************************************************/
Nakshatra28 getNakshatra28( const double &len )
{
	return( (Nakshatra28)(int)( getCorrectedNakshatraLength( len ) / NAKSHATRA_LEN ));
}

/**************************************************************
***
**
***
***************************************************************/
double getNakshatraLongitude27( const double &len )
{
	return a_red( len, NAKSHATRA_LEN );
}

/**************************************************************
***
**
***
***************************************************************/
double getNakshatraLongitude28( const double &len )
{
	return( getCorrectedNakshatraLength( len ), NAKSHATRA_LEN );
}

/**************************************************************
***
**
***
***************************************************************/
int getPada( const double &len )
{
	return( (int)a_red( red_deg( len ) * 4 / NAKSHATRA_LEN, 108 ));
}

/**************************************************************
***
**
***
***************************************************************/
int isOddFootedRasi( const Sign &rasi )
{
	assert_valid_enum<Sign>( rasi );

	if (( rasi == Sign::Aries ) || ( rasi == Sign::Taurus ) || ( rasi == Sign::Gemini )
		|| ( rasi == Sign::Libra ) || ( rasi == Sign::Scorpio ) || ( rasi == Sign::Sagittarius ))
		return true;

	return false;
}

/**************************************************************
***
**   getVarna
***
***************************************************************/
Varna getVarna( const double &len )
{
	assert( len >= 0 && len <= 360 );
	switch ( (int)getRasi( len ) % 4 )
	{
	case 0:
		return Varna::Kshattriya;
		break;
	case 1:
		return Varna::Vaishya;
		break;
	case 2:
		return Varna::Shudra;
		break;
	case 3:
		return Varna::Brahmin;
		break;
	default:
		assert( 0 );
		return Varna::None;
		break;
	}
	return Varna::None;
}

/**************************************************************
***
**
***
***************************************************************/
bool hasJaiminiAspect( const Sign &pos1, const Sign &pos2 )
{
	int diff = red12( (int)pos2 - (int)pos1 );

	// CHARA
	if ( ! ( (int)pos1 % 3 ))
	{
		if (( diff == 4 ) || ( diff == 7 ) || ( diff == 10 )) return true;
	}
	// STHIRA
	else if ( ( (int)pos1 % 3 ) == 1 )
	{
		if (( diff == 2 ) || ( diff == 5 ) || ( diff == 8 )) return true;
	}
	// DWISWA
	else
	{
		if (( diff == 3 ) || ( diff == 3 ) || ( diff == 9 )) return true;
	}

	return false;
}


/**************************************************************
***
**
***
***************************************************************/
int isOddRasi( const double &len )
{
	return( (int)getRasi( len + 30 ) % 2 );
}

/**************************************************************
***
**
***
***************************************************************/
int isEvenRasi( const double &len )
{
	return( (int)getRasi( len ) % 2 );
}

/**************************************************************
***
** isSampadaRasi
***
***************************************************************/
bool isSampadaRasi( const Sign &rasi )
{
	if ( rasi == Sign::Cancer ) return true;
	if ( rasi == Sign::Leo ) return true;
	if ( rasi == Sign::Virgo ) return true;
	if ( rasi == Sign::Capricorn ) return true;
	if ( rasi == Sign::Aquarius ) return true;
	if ( rasi == Sign::Pisces ) return true;
	return false;
}

/**************************************************************
***
** isVishamapadaRasi
***
***************************************************************/
bool isVishamapadaRasi( const Sign &rasi )
{
	if ( rasi == Sign::Aries ) return true;
	if ( rasi == Sign::Taurus ) return true;
	if ( rasi == Sign::Gemini ) return true;
	if ( rasi == Sign::Libra ) return true;
	if ( rasi == Sign::Scorpio ) return true;
	if ( rasi == Sign::Sagittarius ) return true;
	return false;
}

/**************************************************************
***
**
***
***************************************************************/
double getRasiLen( const double &len, const bool &reverse )
{
	if ( ! reverse )	return( a_red( len, 30 ));
	else return( 30 - a_red( len, 30 ));
}

/**************************************************************
***
**
***
***************************************************************/
int between( const double &len, const double &min, const double &max )
{
	return( ( len >= min ) && ( len <= max )  );
}

/**************************************************************
***
**
***
***************************************************************/
int inAscendant( const double &len, const double &asc )
{
	return( a_red( (int)getRasi( len ) - (int)getRasi( asc ), 12 ) == 0 );

	return 0;
}

/**************************************************************
***
**
***
***************************************************************/
int inTrikona( const double &len, const double &asc )
{
	const int b = red_sign( getRasi( len ), getRasi( asc ));

	if (( b == (int)House::h5 ) || ( b == (int)House::h9 )) return 1;
	else return 0;
}

/**************************************************************
***
**
***
***************************************************************/
int inKendra( const double &len, const double &asc )
{

	const int b = red_sign( getRasi( len ), getRasi( asc ));

	if (( b == (int)House::h4 ) || ( b == (int)House::h7 ) || ( b == (int)House::h10 )) return 1;
	else return 0;
}

/**************************************************************
***
**
***
***************************************************************/
int inMovableSign( const double &len )
{
	return( (int)getRasi( len ) % 3 ) == 0;
}

/**************************************************************
***
**
***
***************************************************************/
int inFixedSign( const double &len )
{
	return( (int)getRasi( len ) % 3 ) == 1;
}

/**************************************************************
***
**
***
***************************************************************/
int inDualSign( const double &len )
{
	return( (int)getRasi( len ) % 3 ) == 2;
}

/**************************************************************
***
** getAvasthaName
***
***************************************************************/
// TODO wat is the 2nd param
const wxString getAvasthaName( const int &i , const int /* avastha */ )
{
	const char *sayanadi[12] = { "Sayana", "Upavesana", "Netrapani",
		"Prakasana", "Gamana", "Aagamana", "Sabha", "Agama",
		"Bhojama", "Nrityalipsa", "Kautuka", "Nidra" };

	if ( i > 1 ) return sayanadi[i - 1];
	return sayanadi[11];
}

/**************************************************************
***
**
***
***************************************************************/
PlanetId getLord( const Sign &sign, const bool vedic )
{
	assert_valid_enum<Sign>( sign );
	const static PlanetId k_vlord[12] = { PlanetId::Mars, PlanetId::Venus, PlanetId::Mercury, PlanetId::Moon, PlanetId::Sun, PlanetId::Mercury, PlanetId::Venus, PlanetId::Mars, PlanetId::Jupiter, PlanetId::Saturn, PlanetId::Saturn, PlanetId::Jupiter };
	const static PlanetId k_wlord[12] = { PlanetId::Mars, PlanetId::Venus, PlanetId::Mercury, PlanetId::Moon, PlanetId::Sun, PlanetId::Mercury, PlanetId::Venus, PlanetId::Pluto, PlanetId::Jupiter, PlanetId::Saturn, PlanetId::Uranus, PlanetId::Neptune };
	return vedic ? k_vlord[(int)sign] : k_wlord[(int)sign];
}

/**************************************************************
***
**
***
***************************************************************/
PlanetId getLord( const Sign &sign, const Viewpoint &viewpoint )
{
	return getLord( sign, viewpoint == Viewpoint::Vedic );
}

/**************************************************************
***
**
***
***************************************************************/
PlanetaryFriendship getPlanetaryFriendship( const PlanetId &planet1, const PlanetId &planet2 )
{
	assert( PlanetHelper::isInnerPlanet( planet1 ));
	assert( PlanetHelper::isInnerPlanet( planet2 ));


	/********************************************************
	       Friends         Enemies        Neutrals
	---------------------------------------------------------
	 Sun   Moo, Mar, Jup   Ve, Sat        Mer
	 Moo   Sun, Mer        -              Mar, Jup, Ven, Sat
	 Mar   Sun, Moo, Jup   Mer            Ven, Sat
	 Mer   Sun, Ven        Moo            Mar, Jup, Sat
	 Jup   Sun, Moo, Mar   Mer, Ven       Sat
	 Ven   Mer, Sat        Moo, Sun       Mar, Jup
	 Sat   Mer, Ven        Sun, Moo, Mar  Jup
	*********************************************************/

	const PlanetaryFriendship k_friend[7][7] =
	{
		{ PlanetaryFriendship::Friend, PlanetaryFriendship::Friend, PlanetaryFriendship::Neutral, PlanetaryFriendship::Enemy,
			PlanetaryFriendship::Friend, PlanetaryFriendship::Friend, PlanetaryFriendship::Enemy },      // Sun
		{ PlanetaryFriendship::Friend, PlanetaryFriendship::Friend, PlanetaryFriendship::Friend, PlanetaryFriendship::Neutral,
			PlanetaryFriendship::Neutral, PlanetaryFriendship::Neutral, PlanetaryFriendship::Neutral }, // Moon
		{ PlanetaryFriendship::Friend, PlanetaryFriendship::Enemy, PlanetaryFriendship::Friend, PlanetaryFriendship::Friend,
			PlanetaryFriendship::Neutral, PlanetaryFriendship::Neutral, PlanetaryFriendship::Neutral },   // Mercury
		{ PlanetaryFriendship::Enemy, PlanetaryFriendship::Enemy, PlanetaryFriendship::Friend, PlanetaryFriendship::Friend,
			PlanetaryFriendship::Neutral, PlanetaryFriendship::Neutral, PlanetaryFriendship::Friend },     // Venus
		{ PlanetaryFriendship::Friend, PlanetaryFriendship::Friend, PlanetaryFriendship::Enemy, PlanetaryFriendship::Neutral,
			PlanetaryFriendship::Friend, PlanetaryFriendship::Friend, PlanetaryFriendship::Neutral },    // Mars
		{ PlanetaryFriendship::Friend, PlanetaryFriendship::Friend, PlanetaryFriendship::Enemy, PlanetaryFriendship::Enemy,
			PlanetaryFriendship::Friend, PlanetaryFriendship::Friend, PlanetaryFriendship::Neutral },      // Jupiter
		{ PlanetaryFriendship::Enemy, PlanetaryFriendship::Enemy, PlanetaryFriendship::Friend, PlanetaryFriendship::Friend,
			PlanetaryFriendship::Enemy, PlanetaryFriendship::Neutral, PlanetaryFriendship::Friend }        // Saturn
	};
	return k_friend[(int)planet1][(int)planet2];
}
bool isPlanetaryFriend( const PlanetId &planet1, const PlanetId &planet2 )
{
	return getPlanetaryFriendship( planet1, planet2 ) == PlanetaryFriendship::Friend;
}
bool isPlanetaryEnemy( const PlanetId &planet1, const PlanetId &planet2 )
{
	return getPlanetaryFriendship( planet1, planet2 ) == PlanetaryFriendship::Enemy;
}

/**************************************************************
***
**
***
***************************************************************/
PlanetaryFriendship getRasiTemporaryFriendship( const Sign &rasi1, const Sign &rasi2 )
{
	assert_valid_enum<Sign>( rasi1 );
	assert_valid_enum<Sign>( rasi2 );

	int diff = red12( (int)rasi2 - (int)rasi1 );
	if ( diff == 1 || diff == 2 || diff == 3 || diff == 11 || diff == 10 || diff == 9 ) return PlanetaryFriendship::Friend;
	else return PlanetaryFriendship::Enemy;
}
bool isRasiTempFriend( const Sign &rasi1, const Sign &rasi2 )
{
	return getRasiTemporaryFriendship( rasi1, rasi2 ) == PlanetaryFriendship::Friend;
}

/**************************************************************
***
**
***
***************************************************************/
bool isInMulatrikona( const PlanetId &planet, const double &len )
{
	if ( planet == PlanetId::Sun )
	{
		if (( len >= 120 ) && ( len <= 140 )) return true;
		else return false;
	}
	else if ( planet == PlanetId::Moon )
	{
		if (( len >= 34 ) && ( len <= 60 )) return true;
		else return false;
	}
	else if ( planet == PlanetId::Mars )
	{
		if (( len >= 0 ) && ( len <= 12 )) return true;
		else return false;
	}
	else if ( planet == PlanetId::Mercury )
	{
		if (( len >= 165 ) && ( len <= 170 )) return true;
		else return false;
	}
	else if ( planet == PlanetId::Jupiter )
	{
		if (( len >= 240 ) && ( len <= 250 )) return true;
		else return false;
	}
	else if ( planet == PlanetId::Venus )
	{
		if (( len >= 180 ) && ( len <= 195 )) return true;
		else return false;
	}
	else if ( planet == PlanetId::Saturn )
	{
		if (( len >= 300 ) && ( len <= 320 )) return true;
		else return false;
	}
	else
	{
		errorlog << "ERROR isInMulatrikona planet " << (int)planet << " longitude " << len;
		exit(1);
	}
	return false;
}

/**************************************************************
***
**
***
***************************************************************/
Sign getMulatrikonaRasi( const PlanetId &planet )
{
	int p = (int)planet;
	if ( planet == PlanetId::AscNode ) p = 7;
	if ( planet == PlanetId::DescNode ) p = 8;
	assert( p >= 0 && p < 9 );

	// su=LEO, mo=TAURUS, ma=ARIES, me=VIRGO, Ju=SAGITTARIUS, ve=LIBRA, sa=AQUARIUS, ra=AQUARIUS, ke=SCORPIO
	const Sign k_mulatrikona_sign[9] =
	{
		Sign::Leo, Sign::Taurus, Sign::Virgo, Sign::Libra, Sign::Aries,
		Sign::Sagittarius, Sign::Aquarius, Sign::Aquarius, Sign::Scorpio
	};
	return k_mulatrikona_sign[p];
}

/**************************************************************
***
**
***
***************************************************************/
Vashya getVashya( const double &len )
{
	Vashya	vashya = Vashya::None;
	Sign rasi = ::getRasi( len );
	double rasilen = ::getRasiLen( len );
	//enum { Vashya::Quadruped, Vashya::Human, Vashya::Jalachara, Vashya::Leo, Vashya::Scorpio };
	switch ( rasi )
	{
	case Sign::Aries:
	case Sign::Taurus:
		vashya = Vashya::Quadruped;
		break;
	case Sign::Gemini:
	case Sign::Virgo:
	case Sign::Libra:
	case Sign::Aquarius:
		vashya = Vashya::Human;
		break;
	case Sign::Cancer:
	case Sign::Pisces:
		vashya = Vashya::Jalachara;
		break;
	case Sign::Leo:
		vashya = Vashya::Leo;
		break;
	case Sign::Scorpio:
		vashya = Vashya::Scorpio;
		break;
	case Sign::Sagittarius:
		vashya = rasilen < 15 ? Vashya::Human : Vashya::Quadruped;
		break;
	case Sign::Capricorn:
		vashya = rasilen < 15 ? Vashya::Quadruped : Vashya::Jalachara;
		break;
	default:
		assert( 0 );
		break;
	}
	assert( vashya != Vashya::None );
	return vashya;
}

/**************************************************************
***
**
***
***************************************************************/
bool hasMulatrikonaRasi( const PlanetId &planet, const Sign &rasi )
{
	assert_valid_enum<Sign>( rasi );
	return( getMulatrikonaRasi( planet ) == rasi );
}

/**************************************************************
***
**
***
***************************************************************/
Sign getExaltationRasi( const PlanetId &planet )
{
	const static Sign exalt_rasi[9] =
	{
		Sign::Aries, Sign::Taurus, Sign::Virgo,
		Sign::Pisces, Sign::Capricorn, Sign::Cancer,
		Sign::Libra, Sign::Gemini, Sign::Sagittarius
	};
	int ret = 0;
	assert( PlanetHelper::isInnerPlanet( planet ) || PlanetHelper::isNode( planet ));

	switch ( planet )
	{
	case PlanetId::AscNode:
		ret = 7;
		break;
	case PlanetId::DescNode:
		ret = 8;
		break;
	default:
		ret = (int)planet;
		break;
	}
	return exalt_rasi[ret];
}

/**************************************************************
***
**
***
***************************************************************/
bool hasExaltationRasi( const PlanetId &planet, const Sign &rasi )
{
	return ( getExaltationRasi( planet ) == rasi );
}

/**************************************************************
***
**
***
***************************************************************/
bool hasDebilationRasi( const PlanetId &planet, const Sign &rasi )
{
	int erasi = (int)rasi + 6;
	if ( erasi > 11 ) erasi -= 12;
	return ( (int)getExaltationRasi( planet ) == erasi );
}

/**************************************************************
***
**   Calculate geometrical midpoint for composite horoscope
***
***************************************************************/
double calcGeometricalMidpoint( const double &len1, const double &len2 )
{
	double dist = red_deg( len2 - len1 );
	return( dist < 180 ? red_deg( len1 + dist/2 ) : red_deg( len2 + ( 360 - dist ) /2 ) );
}

/**************************************************************************
***
** 
***
**************************************************************************/
int calcMonthLength( int monat, int jahr )
{
	if ( monat == 1 || monat == 3 || monat == 5 || monat == 7 || monat == 8 || monat == 10 || monat == 12 ) return 31;
	if ( monat == 4 || monat == 6 || monat == 9 || monat == 11 ) return 30;
	assert( monat == 2 );
	if ( jahr % 4 )  return 28;
	else
	{
		if (( jahr % 100 ) != 0 ) return 29;
		if (( jahr % 100 ) == 0 && ( jahr % 400 ) != 0 ) return 28;
		return 29;
	}
}

/*****************************************************
**
**   getGrahaDrishtiValue
**
******************************************************/
#if 0
double getGrahaDrishtiValue( const PlanetId &aspectingPlanet, const int &rasidiff )
{
	if ( rasidiff == 6 ) return 1;
	else if ( rasidiff == 3 || rasidiff == 7 )
	{
		return ( aspectingPlanet == PlanetId::Mars ? 1 : .75 );
	}
	else if ( rasidiff == 4 || rasidiff == 8 )
	{
		return ( aspectingPlanet == PlanetId::Jupiter ? 1 : .5 );
	}
	else if ( rasidiff == 2 || rasidiff == 9 )
	{
		return ( aspectingPlanet == PlanetId::Saturn ? 1 : .25 );
	}
	return 0;
}
#endif

