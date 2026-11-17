/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Horoscope.cpp
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

#include "Horoscope.h"

#include "ArabicParts.h"
#include "astrobase.h"
#include "astroconfig.h"
#include "Calculator.h"
#include "Dignity.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "logger.h"
#include "mathbase.h"
#include "Varga.h"

extern Astroconfig *astroconfig;

//#define DEBUG_HOROSCOPE

/*****************************************************
**
**   Horoscope   ---   Constructor
**
******************************************************/
Horoscope::Horoscope( const HoroscopeRole role )
 : role( role )
{
#ifdef DEBUG_SHARED_PTR_LIFECYLCE
	++nb_created;
#endif

	calculator = CalculatorFactory().getCalculator();
	arabic_expert = new ArabicPartsExpert( this );

	dl.location = astroconfig->standardLocation;
	dl.location.dst = DateFormatter::get()->getSystemDstOffset( dl.date.jd );
}

/*****************************************************
**
**   Horoscope   ---   Destructor
**
******************************************************/
Horoscope::~Horoscope()
{
	delete arabic_expert;

#ifdef DEBUG_SHARED_PTR_LIFECYLCE
	++nb_deleted;
	debuglog << "Destructor class Horoscope: " << nb_created << " objects created, " << nb_deleted << " objects deleted";
#endif
}

/*****************************************************
**
**   Horoscope   ---   init_update
**
******************************************************/
void Horoscope::init_update()
{
	for( const Viewpoint v : Enum<Viewpoint>())
	{
		vp_data[(int)v] = ViewpointData();
	}

	int i;
	for( i = 0; i <= LAST_EXTENDED_FACTOR; i++ ) longitude[i] = 0;
	for( i = 0; i < MAX_EPHEM_FACTORS; i++ )
	{
		latitude[i] = 0;
		lspeed[i] = bspeed[i] = 1;
	}
	l_offset = 0.0;
	uniform_direction = MovingDirection::Indefinite;

	armc = 0;
	ok_calc_benefics = ok_calc_sunrise = false;

	siderealTime = 0.0;
	eps = 0.0;
	sunrise = sunset = 0;

	// Arabic parts
	arabic_expert->clear();
}

/*****************************************************
**
**   Horoscope   ---   update
**
******************************************************/
void Horoscope::update()
{
#ifdef DEBUG_HOROSCOPE
	static int count = 0;
	debuglog <<  "Horoscope::update role " << role << " count " << ++count;
#endif
	init_update();
}

/*****************************************************
**
**   Horoscope   ---   getLongitude
**
******************************************************/
double Horoscope::getLongitude( const PlanetId &p, const Viewpoint &viewpoint )
{
#ifdef DEBUG_HOROSCOPE
	debuglog << "Horoscope::getLongitude planet " << p;
#endif
	if ( PlanetHelper::isAngle( p ))
	{
		if ( ! longitude[(int)p] ) calculate_angles();
		return red_deg( longitude[(int)p] + l_offset - getAyanamsa( viewpoint ));
	}

	if ( PlanetHelper::isNode( p ))
	{
		const PlanetId pnode = PlanetHelper::resolveNode( p, viewpoint );
		if ( ! longitude[(int)pnode] ) calculate_object( pnode );
		return red_deg( longitude[(int)pnode] + l_offset - getAyanamsa( viewpoint ));
	}

	if ( PlanetHelper::isEphemPlanet( p ))
	{
		if ( ! longitude[(int)p] ) calculate_object( p );
		return red_deg( longitude[(int)p] + l_offset - getAyanamsa( viewpoint ));
	}

	if ( PlanetHelper::isHouse( p ))
	{
		if ( ! longitude[(int)PlanetId::Meridian] ) calculate_angles();

		// ayanamsa is added in viewpoint proxy
		if ( astroconfig->getViewpointConfig( viewpoint ).calculation.houseStart == HouseStartMode::Cusp )
			return getHouseCusp( (House)((int)p - (int)PlanetId::House1), viewpoint );
		else
			return getHouseSandhi( (House)((int)p - (int)PlanetId::House1 ), viewpoint );
	}

	if ( PlanetHelper::isSpecialLagna( p ))
	{
		if ( ! longitude[(int)p] ) calculate_lagnas();
		return red_deg( longitude[(int)p] + l_offset - getAyanamsa( viewpoint ));
	}

	if ( PlanetHelper::isUpagraha( p ) || PlanetHelper::isKalavela( p ))
	{
		if ( ! longitude[(int)p] ) calculate_upagrahas();
		switch( p )
		{
			case PlanetId::Vyatipata:
			case PlanetId::Parivesha:
				return red_deg( longitude[(int)p] + l_offset + getAyanamsa( viewpoint ));
			break;
			default:
				return red_deg( longitude[(int)p] + l_offset - getAyanamsa( viewpoint ));
			break;
		}
	}

	if ( PlanetHelper::isArabicPart( p ))
	{
		return red_deg( arabic_expert->getLongitude( p, viewpoint ) + l_offset );
	}

	// remaining cases
	switch( p )
	{
		case PlanetId::D9Lagna:
			if ( ! longitude[(int)PlanetId::Ascendant] ) calculate_angles();
			return red_deg( 9 * ( longitude[(int)PlanetId::Ascendant] - getAyanamsa( viewpoint )) + l_offset );
		break;
		case PlanetId::Aries:
			return red_deg( l_offset + getAyanamsa( viewpoint ));
		break;
		case PlanetId::Cancer:
			return red_deg( l_offset + 90 + getAyanamsa( viewpoint ));
		break;
		case PlanetId::Libra:
			return red_deg( l_offset + 180 + getAyanamsa( viewpoint ));
		break;
		case PlanetId::Capricorn:
			return red_deg( l_offset + 270 + getAyanamsa( viewpoint ));
		break;
		case PlanetId::Earth:
			if ( ! longitude[(int)PlanetId::Sun] ) calculate_object( PlanetId::Sun );
			return red_deg( longitude[(int)PlanetId::Sun] + 180.0 + l_offset - getAyanamsa( viewpoint ));
		break;

		default:
			errorlog << "invalid factor id " << p << " int value was " << (int)p;
			return 0.0;
		break;
	}
	assert( false );
	return 0.0;
}

/*****************************************************
**
**   Horoscope   ---   hasSuspiciousLongitude
**
******************************************************/
bool Horoscope::hasSuspiciousLongitude( const PlanetId &p, const Viewpoint &viewpoint )
{
	// fix: set first to moon in order to avoid warning on sun pos in heliocentric mode
	if (( p >= PlanetId::Moon && p <= PlanetId::LastEphem )
		&& ( ! PlanetHelper::isNode( p )))
	{
		const int pindex = getInternalObjectIndex( p, viewpoint );
		if ( longitude[pindex] == 0 )
		{
			debuglog << "Horoscope::hasSuspiciousLongitude object " << (int)p << "/" << pindex;
			return true;
		}
		else return false;
	}
	return false;
}

/*****************************************************
**
**   Horoscope   ---   getBSpeed
**
******************************************************/
double Horoscope::getBSpeed( const PlanetId &p, const Viewpoint &viewpoint )
{
	if ( PlanetHelper::isEphemPlanet( p ) && ( ! PlanetHelper::isAngle( p )))
	{
		const int pindex = getInternalObjectIndex( p, viewpoint );
		if ( ! longitude[pindex] ) calculate_object( (PlanetId)pindex );
		return bspeed[pindex];
	}
	//debuglog << " GETBSPEED return " << bspeed[p];
	return 0.0;
}

/*****************************************************
**
**   Horoscope   ---   getLSpeed
**
******************************************************/
double Horoscope::getLSpeed( const PlanetId &p, const Viewpoint &viewpoint )
{
	if ( PlanetHelper::isAngle( p ))
	{
		if ( ! longitude[(int)p] ) calculate_angles();
		return lspeed[(int)p];
	}
	if ( PlanetHelper::isEphemPlanet( p ))
	{
		const int pindex = getInternalObjectIndex( p, viewpoint );
		if ( ! longitude[pindex] ) calculate_object( (PlanetId)pindex );
		return lspeed[pindex];
	}
	//debuglog << " GETLSPEED return " << lspeed[p];
	return 0.0;
}

/*****************************************************
**
**   Horoscope   ---   getLatitude
**
******************************************************/
double Horoscope::getLatitude( const PlanetId &p, const Viewpoint &viewpoint )
{
	if ( PlanetHelper::isEphemPlanet( p )
		&& ( ! PlanetHelper::isAngle( p ))
		&& ( ! PlanetHelper::isNode( p )))
	{
		const int pindex = getInternalObjectIndex( p, viewpoint );
		if ( ! longitude[pindex] ) calculate_object( (PlanetId)pindex );
		return latitude[pindex];
	}
	return 0.0;
}

/*****************************************************
**
**   Horoscope   ---   getDeclination
**
******************************************************/
double Horoscope::getDeclination( const PlanetId &p, const Viewpoint &viewpoint )
{
	return calculator->calcDeclination( getLongitude( p, viewpoint ), getLatitude( p, viewpoint ), getEps() );
}

/*****************************************************
**
**   Horoscope   ---   getDirection
**
******************************************************/
MovingDirection Horoscope::getDirection( const PlanetId &p, const Viewpoint &viewpoint )
{
	if ( uniform_direction != MovingDirection::Indefinite ) return uniform_direction;

	//debuglog << "getDirection " << p << " speed " << lspeed[p];
	return getLSpeed( p, viewpoint ) >= 0 ? MovingDirection::Direct : MovingDirection::Retrograde;	
}

/*****************************************************
**
**   Horoscope   ---   calculate_object
**
******************************************************/
int Horoscope::calculate_object( const PlanetId &p )
{
#ifdef DEBUG_HOROSCOPE
	debuglog << "Horoscope::calculate_object planet " << p;
#endif

	const PlanetPosition op = calculator->calcPositionSpeed( dl, p, Viewpoint::Astronomical );
	longitude[(int)p] = op.longitude;
	latitude[(int)p] = op.latitude;
	lspeed[(int)p] = op.lspeed;
	bspeed[(int)p] = op.bspeed;
	return op.longitude ? 1 : 0;
}

/*****************************************************
**
**   Horoscope   ---   getHousePos
**
******************************************************/
House Horoscope::getHousePos( const PlanetId& planet, const Viewpoint &viewpoint )
{
	return (House)getHouse4Longitude( getLongitude( planet, viewpoint ), viewpoint );
}

/*****************************************************
**
**   Horoscope   ---   calculate_angles
**
******************************************************/
void Horoscope::calculate_angles()
{
#ifdef DEBUG_HOROSCOPE
	debuglog << "Horoscope::calculate_angles";
#endif
	const std::array<double, 5> a = calculator->calcAnglesSpeed( getJD(), 
    dl.location.latitude,
    dl.location.longitude );

	armc = a[0];
	longitude[(int)PlanetId::Ascendant] = a[1];
	longitude[(int)PlanetId::Descendant] = red_deg( a[1] + 180 );
	longitude[(int)PlanetId::Meridian] = a[3];
	longitude[(int)PlanetId::ImumCoeli] = red_deg( a[3] + 180 );
	lspeed[(int)PlanetId::Ascendant] = lspeed[(int)PlanetId::Descendant] = a[2];
	lspeed[(int)PlanetId::Meridian] = lspeed[(int)PlanetId::ImumCoeli] = a[4];
}

/*****************************************************
**
**   Horoscope   ---   getHouseForLongitudeDouble
**
******************************************************/
double Horoscope::getHouse4LongitudeDouble( const double &longitude, const Viewpoint &viewpoint )
{
	//debuglog << "Horoscope::getHouse4Longitude longitude " << longitude;
	double hstart, hend;
	const bool vedic = viewpoint == Viewpoint::Vedic;
	const CalculationConfig &cconfig = astroconfig->getViewpointConfig( viewpoint ).calculation;

	for( const House i : Enum<House>() )
	{
		if ( ! vedic || cconfig.houseStart == HouseStartMode::Cusp )
		{
			hstart = getHouseCusp( i, viewpoint );
			hend = getHouseCusp( (House)red12( (int)i + 1 ), viewpoint );
		}
		else // sandhi is the end of the house
		{
			hstart = getHouseSandhi( (House)red12( (int)i - 1 ), viewpoint );
			hend = getHouseSandhi( i, viewpoint );
		}
		//debuglog << "Horoscope::getHouse4Longitude i " << (int)i << " hstart " << hstart << " hend " << hend;
		if (( hstart <= longitude && longitude < hend ) || ( hstart > hend && ( longitude >= hstart || longitude < hend )))
    {
      const double hlen = red_deg( hend - hstart );
			assert( hlen );
      const double hpart = red_deg( longitude - hstart );
      const double ret = (int)i + hpart / hlen;
      assert( ret >= 0 && ret < 12 );
      return ret;
    }

	}
	assert( 0 );
	return 0;
}

/*****************************************************
**
**   Horoscope   ---   calculate_sunrise_sunset
**
******************************************************/
void Horoscope::calculate_sunrise_sunset()
{
	if ( ! longitude[(int)PlanetId::Sun] ) calculate_object( PlanetId::Sun );
	if ( ! longitude[(int)PlanetId::Ascendant] ) calculate_angles();

	double startjdrise, startjdset;

	double diff_ascsun = red_deg( longitude[(int)PlanetId::Ascendant] -  longitude[(int)PlanetId::Sun] );
	isDayBirth = ( diff_ascsun < 180 );
	
	startjdrise = startjdset = getJD();
	if ( isDayBirth ) startjdrise--;
	else startjdset--;

	sunrise = calculator->calcNextSolarEvent( SolarEventType::Sunrise, startjdrise,
    dl.location.latitude,
    dl.location.longitude );
	sunset = calculator->calcNextSolarEvent( SolarEventType::Sunset, startjdset,
    dl.location.latitude,
    dl.location.longitude );

#ifdef DEBUG_HOROSCOPE
	debuglog << "Horoscope::calculate_sunrise_sunset sunrise " << sunrise << " sunset " << sunset;
#endif
}

/*****************************************************
**
**   Horoscope   ---   calculate_lagnas
**
******************************************************/
void Horoscope::calculate_lagnas()
{
	if ( ! sunrise || ! sunset ) calculate_sunrise_sunset();
	longitude[(int)PlanetId::BhavaLagna] = red_deg( 360 * ( getJD() - sunrise ) + longitude[(int)PlanetId::Sun] + l_offset );
	longitude[(int)PlanetId::HoraLagna] = red_deg( 720 * ( getJD() - sunrise ) + longitude[(int)PlanetId::Sun] + l_offset );
	longitude[(int)PlanetId::GhatikaLagna] = red_deg( 1800 * ( getJD() - sunrise ) + longitude[(int)PlanetId::Sun] + l_offset );
}

//#define DEBUG_UPAGRAHAS
/*****************************************************
**
**   Horoscope   ---   calculate_upagrahas
**
******************************************************/
void Horoscope::calculate_upagrahas()
{
	if ( ! sunrise || ! sunset ) calculate_sunrise_sunset(); // calculation of sun longitude included

	// Upagrahas
	if ( ! longitude[(int)PlanetId::Sun] ) calculate_object( PlanetId::Sun );
	longitude[(int)PlanetId::Dhuma] =  red_deg( longitude[(int)PlanetId::Sun] + 133.333333333 );
	longitude[(int)PlanetId::Vyatipata] = red_deg( 360 - longitude[(int)PlanetId::Dhuma] );
	longitude[(int)PlanetId::Parivesha] = red_deg( 180 - longitude[(int)PlanetId::Dhuma] );
	longitude[(int)PlanetId::Chapa] = red_deg( longitude[(int)PlanetId::Sun] + 313.333333333 );
	longitude[(int)PlanetId::Upaketu] = red_deg( longitude[(int)PlanetId::Sun] + 330 );

#ifdef DEBUG_UPAGRAHAS
	for ( const auto &k : astroconfig->kalavelaCfg )
	{
		debuglog << "Horoscope::calculate_upagrahas ob " << (int)k.first
			<< " kalavela lord " << (int)k.second.lord
			<< " portion " << (int)k.second.portion;
	}
#endif

	// Kalavelas
	KalavelaLord lord;
	double step_len, thejd;
	for ( int i = 0; i <= 8; i++ )
	{
		if ( isDayBirth )
		{
			lord = (KalavelaLord)(( dl.getWeekDay() + i ) % 8);
			step_len = .125 * fabs( sunset - sunrise );
			thejd = sunrise + i * step_len;
		}
		else
		{
			lord = (KalavelaLord)(( dl.getWeekDay() + i + 4 ) % 8);
			step_len = .125 * fabs( sunrise - sunset );
			// FIXME
			if ( step_len < 0 ) step_len *= -1;
			thejd = sunset + i * step_len;
		}

#ifdef DEBUG_UPAGRAHAS
		debuglog << "Horoscope::calculate_upagrahas START index " << i
			<< " lord " << (int)lord 
			<< " Date " << DateFormatter::get()->formatUtDatetime( thejd )
			<< " steplen " << step_len;
#endif

		for( const auto &[ ob, kcfg ]: astroconfig->kalavela.elements )
		{
			double realjd = thejd;
			if ( kcfg.lord == lord )
			{
#ifdef DEBUG_HOROSCOPE
				debuglog << " Horoscope::calculate_upagrahas portion " << (int)kcfg.portion;
#endif
				switch( kcfg.portion )
				{
					case KalavelaPortion::Middle:
						realjd += .5* step_len;
#ifdef DEBUG_HOROSCOPE
						debuglog << " Horoscope::calculate_upagrahas MIDDLE " << realjd;
#endif
					break;
					case KalavelaPortion::End:
						realjd += step_len;
#ifdef DEBUG_HOROSCOPE
						debuglog << " Horoscope::calculate_upagrahas END " << realjd;
#endif
					break;
					case KalavelaPortion::Beginning: // default, already done
					default:
#ifdef DEBUG_HOROSCOPE
						debuglog << " Horoscope::calculate_upagrahas START " << realjd;
#endif
					break;
				}

				longitude[(int)ob] = red_deg(
					calculator->calcAscendant( realjd, 
						dl.location.latitude,
						dl.location.longitude,
						Viewpoint::Astronomical ));

#ifdef DEBUG_UPAGRAHAS
					debuglog << "Horoscope::calculate_upagrahas CALCULATED ob " << (int)ob
					<< " lord " << (int)lord
					<< " portion " << (int)kcfg.portion
					<< " step len " << step_len
					<< " realjd " << realjd
					<< " Date " << DateFormatter::get()->formatUtDatetime( realjd )
					<< " longitude " << longitude[(int)ob];
#endif
			}
		}
	}
}

/*****************************************************
**
**   Horoscope   ---   calculate_benefics
**
******************************************************/
void Horoscope::calculate_benefics()
{
	const double angle = red_deg( getVedicLongitude( PlanetId::Moon ) - getVedicLongitude( PlanetId::Sun ));
	switch ( astroconfig->varga.moonBenefic )
	{
		case 1: // angle < 180
			moonBenefic = ( angle < 180 );
		break;
		case 2: // 120 < angle < 240
			moonBenefic = ( angle > 120 && angle < 240 );
		break;
		case 3: // always
			moonBenefic =  true;
		break;
		case 0: // never
		default:
			moonBenefic = false;
		break;
	}
	ok_calc_benefics = true;
}

/*****************************************************
**
**   Horoscope   ---   isMercuryBenefic
**
******************************************************/
bool Horoscope::isMercuryBenefic( const Viewpoint &viewpoint )
{
	switch ( astroconfig->varga.mercuryBenefic )
	{
		case 1: // no malefic conjunction
		{
			const Sign mpos = getRasi( getLongitude( PlanetId::Mercury, viewpoint ));
			if ( ! moonBenefic && getRasi( getVedicLongitude( PlanetId::Moon )) == mpos ) return false;

			if ( getRasi( getLongitude( PlanetId::AscNode, viewpoint )) == mpos ) return false;
			if ( getRasi( getLongitude( PlanetId::DescNode, viewpoint )) == mpos ) return false;

			const std::list<PlanetId> malefics =
			{
				PlanetId::Sun,
				PlanetId::Mars,
				PlanetId::Saturn
			};
			for( const PlanetId &ob : malefics )
			{
				if ( getRasi( getLongitude( ob, viewpoint )) == mpos ) return false;
			}
			return true;
		}
		break;
		case 2: // always
			return true;
		break;
		case 0: // never
		default:
			return false;
		break;
	}
}

/*****************************************************
**
**   Horoscope   ---   isBenefic
**
******************************************************/
bool Horoscope::isBenefic( const PlanetId &planet, const Viewpoint &viewpoint )
{
	if ( ! ok_calc_benefics ) calculate_benefics();

	assert_valid_enum<PlanetId>( planet );
	switch ( planet )
	{
	case PlanetId::Moon:
		return moonBenefic;
		break;
	case PlanetId::Mercury:
		return isMercuryBenefic( viewpoint );
		break;
	case PlanetId::Venus:
	case PlanetId::Jupiter:
		return true;
		break;
	default:
		return false;
		break;
	}
	return false;
}

/*****************************************************
**
**   Horoscope   ---   sunrise etc. getter
**
******************************************************/
double Horoscope::getSunrise()
{
	if ( ! ok_calc_sunrise ) calculate_sunrise_sunset();
	return sunrise;
}
double Horoscope::getSunset()
{
	if ( ! ok_calc_sunrise ) calculate_sunrise_sunset();
	return sunset;
}
bool Horoscope::getDayBirth()
{
	if ( ! ok_calc_sunrise ) calculate_sunrise_sunset();
	return isDayBirth;
}

/*****************************************************
**
**   Horoscope   ---   getHouseKPLords
**
******************************************************/
KpData Horoscope::getHouseKPLords( const House &house, const Viewpoint &viewpoint )
{
	assert_valid_enum<House>( house );
	return getKPLords( (PlanetId)((int)PlanetId::House1 + (int)house), viewpoint );
}

/*****************************************************
**
**   Horoscope   ---   getAyanamsa
**
******************************************************/
double Horoscope::getAyanamsa( const Viewpoint &viewpoint )
{
	assert_valid_enum<Viewpoint>( viewpoint );
	ViewpointData &vpd = vp_data[ (int)viewpoint ];
	if ( ! vpd.calc_aya )
	{
		vpd.ayanamsa = calculator->calcAyanamsa( getJD(), astroconfig->getViewpointConfig( viewpoint ).calculation.ayanamsa );
		vpd.calc_aya = true;
	}
	//debuglog << "Viewpoint " << (int)viewpoint << " has ayanamsa " << ayanamsa;
	return vpd.ayanamsa;
}

/*****************************************************
**
**   Horoscope   ---   getHouseCusp
**
******************************************************/
double Horoscope::getHouseCusp( const House &house, const Viewpoint& viewpoint )
{
	assert_valid_enum<Viewpoint>( viewpoint );
	assert_valid_enum<House>( house );
	ViewpointData &vpd = vp_data[ (int)viewpoint ];

	if ( ! vpd.calc_houses ) calculateHouses( viewpoint );

	return red_deg( vpd.house_cusp[(int)house] + getLongitudeOffset() - getAyanamsa( viewpoint ));
}

/*****************************************************
**
**   Horoscope   ---   getHouseSandhi
**
******************************************************/
double Horoscope::getHouseSandhi( const House &house, const Viewpoint& viewpoint )
{
	assert_valid_enum<House>( house );
	assert_valid_enum<Viewpoint>( viewpoint );
	ViewpointData &vpd = vp_data[ (int)viewpoint ];

	if ( ! vpd.calc_houses ) calculateHouses( viewpoint );

	return red_deg( vpd.house_sandhi[(int)house] + getLongitudeOffset() - getAyanamsa( viewpoint ));
}

/*****************************************************
**
**   Horoscope   ---   calculateHouses
**
******************************************************/
void Horoscope::calculateHouses( const Viewpoint &viewpoint )
{
	assert_valid_enum<Viewpoint>( viewpoint );
	ViewpointData &vpd = vp_data[ (int)viewpoint ];
	const CalculationConfig &cconfig = astroconfig->getViewpointConfig( viewpoint ).calculation;

	vpd.house_cusp = calculator->calcHousesByArmc( getArmc(), dl.location.latitude, getEps(), cconfig.houseSystem );

	// sandhi calculation
	double houselen;
	for( const House i : Enum<House>() )
	{
		houselen = red_deg( vpd.house_cusp[( i != House::h12 ? ((int)i) + 1 : 0)] - vpd.house_cusp[(int)i] );
		vpd.house_sandhi[(int)i] = red_deg( vpd.house_cusp[(int)i] + .5 * houselen );
	}
	vpd.calc_houses = true;
}

/*****************************************************
**
**   Horoscope   ---   initVedicPlanets
**
******************************************************/
void Horoscope::initVedicPlanets( const Viewpoint &viewpoint )
{
	assert_valid_enum<Viewpoint>( viewpoint );
	ViewpointData &vpd = vp_data[ (int)viewpoint ];
	DignityExpert expert( this, viewpoint );
	const Planets obs = PlanetHelper::createVedicOnlyPlanets();

	// factors that will likely be used later
	for( const PlanetId ob : obs )
	{
		vpd.vedic_planets[ob] = VedicPlanetData();
		vpd.vedic_planets[ob].kpdata = expert.calcKpData( ob );

		vpd.vedic_planets[ob].vargadata = expert.calcVargaData( ob );
	}
	vpd.calc_kp = true;
	vpd.calc_vargas = true;

	for( const PlanetId ob : obs )
	{
		expert.calculateDignities( ob, vpd.vedic_planets[ob].vargadata );
		expert.calculateVimsopakaBala( vpd.vedic_planets[ob] );
	}
	//calc_dignities = true;
}

/*****************************************************
**
**   Horoscope  ---   updateVedicPlanet
**
******************************************************/
void Horoscope::updateVedicPlanet( const PlanetId &ob, const Viewpoint &viewpoint )
{
	assert_valid_enum<Viewpoint>( viewpoint );
	ViewpointData &vpd = vp_data[ (int)viewpoint ];
	debuglog << "Horoscope::updateVedicPlanet " << (int)ob << " viewpoint " << viewpoint;

	DignityExpert expert( this, viewpoint );
	vpd.vedic_planets[ob] = VedicPlanetData();

	vpd.vedic_planets[ob].kpdata = expert.calcKpData( ob );
	vpd.calc_kp = true;

	vpd.vedic_planets[ob].vargadata = expert.calcVargaData( ob );
	expert.calculateVimsopakaBala( vpd.vedic_planets[ob] );
	vpd.calc_vargas = true;
}

/*****************************************************
**
**   Horoscope  ---   getKPLords
**
******************************************************/
KpData Horoscope::getKPLords( const PlanetId &p, const Viewpoint &viewpoint )
{
	assert_valid_enum<PlanetId>( p );
	assert_valid_enum<Viewpoint>( viewpoint );
	ViewpointData &vpd = vp_data[ (int)viewpoint ];

	if ( vpd.vedic_planets.find ( p ) == vpd.vedic_planets.end())
	{
		updateVedicPlanet( p, viewpoint );
	}
	//assert( vedic_planets.find ( ob ) != vedic_planets.end());
	auto vp = vpd.vedic_planets.find ( p );
	assert( vp != vpd.vedic_planets.end());
	assert( vpd.calc_kp );

	return vp->second.kpdata;
}

/*****************************************************
**
**   Horoscope  ---   getVargaData
**
******************************************************/
const VargaData &Horoscope::getVargaData( const PlanetId &ob, const Varga &varga, const Viewpoint &viewpoint )
{
	assert_valid_enum<PlanetId>( ob );
	assert_valid_enum<Viewpoint>( viewpoint );
	assert_valid_enum<Varga>( varga );
	ViewpointData &vpd = vp_data[ (int)viewpoint ];

	if ( vpd.vedic_planets.size() == 0 )
	{
		initVedicPlanets( viewpoint );
	}
	assert( vpd.vedic_planets.size() > 0 );

	if ( vpd.vedic_planets.find ( ob ) == vpd.vedic_planets.end())
	{
		updateVedicPlanet( ob, viewpoint );
	}
	assert( vpd.vedic_planets.find ( ob ) != vpd.vedic_planets.end());
	assert( vpd.calc_vargas );

	return vpd.vedic_planets[ob].vargadata[(int)varga];
}

/*****************************************************
**
**   Horoscope  ---   getVedicPlanetData
**
******************************************************/
const VedicPlanetData &Horoscope::getVedicPlanetData( const PlanetId &ob, const Viewpoint &viewpoint )
{
	assert_valid_enum<PlanetId>( ob );
	assert_valid_enum<Viewpoint>( viewpoint );
	ViewpointData &vpd = vp_data[ (int)viewpoint ];

	if ( vpd.vedic_planets.size() == 0 )
	{
		initVedicPlanets( viewpoint );
	}
	assert( vpd.vedic_planets.size() > 0 );

	if ( vpd.vedic_planets.find ( ob ) == vpd.vedic_planets.end())
	{
		updateVedicPlanet( ob, viewpoint );
	}
	assert( vpd.vedic_planets.find ( ob ) != vpd.vedic_planets.end());
	assert( vpd.calc_vargas );

	return vpd.vedic_planets[ob];
}

/*****************************************************
**
**   Horoscope   ---   getInternalObjectIndex
**
******************************************************/
int Horoscope::getInternalObjectIndex( const PlanetId &ob, const Viewpoint &viewpoint )
{
	return (int)PlanetHelper::resolveNode( ob, viewpoint );
}

/*****************************************************
**
**   Horoscope   ---   file methods
**
******************************************************/
void Horoscope::openFile( wxString filename )
{
	dl = DateLocationLoader().loadFile( filename ); 
}
void Horoscope::saveAs( wxString filename )
{
	dl.filename = filename;
	save();
}
void Horoscope::save()
{
	DateLocationLoader().save( dl ); 
}

/*****************************************************
**
**   Horoscope   ---   various lazy access methods
**
******************************************************/
double Horoscope::getSiderealTime()
{
	if ( ! siderealTime ) siderealTime = calculator->calcSiderealTime( getJD(), dl.location.longitude );
	return siderealTime;
}
double Horoscope::getArmc()
{
	if ( ! armc ) calculate_angles();
	return armc;
}
double Horoscope::getEps()
{
	if ( ! eps ) calculator->calcEps( eps, getJD() );
	return eps;
}

