/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Jaimini.cpp
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

#include "Jaimini.h"

#include "astrobase.h"
#include "astroconfig.h"
#include "ContextProperties.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "MDocument.h"
#include "Table.h"
#include "Varga.h"

#include <format>

extern Astroconfig *astroconfig;

/*****************************************************
**
**   JaiminiExpert   ---   getRasiLongitude
**
******************************************************/
double JaiminiExpert::getRasiLongitude( const PlanetId &p )
{
	const Viewpoint viewpoint = getViewpoint();
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );

	const bool reverse = PlanetHelper::isNode( p );
	return ::getRasiLen( horoscope->getLongitude( p, viewpoint ), reverse );
}

/*****************************************************
**
**   JaiminiExpert   ---   update
**
******************************************************/
void JaiminiExpert::update()
{
	//const Viewpoint viewpoint = getViewpoint();

	// number of planets in the 12 Rasis
	// based on the seven planets Sun..Saturn
	for ( int i = 0; i < 12; i++ ) nbplanets[i] = 0;

	for ( const PlanetId p : PlanetHelper::createJaiminiPlanets())
	{
		nbplanets[(int)this->getRasi(p)]++;
	}

	// number of aspects on Saturn, Rahu, Mars and Ketu
	aspSat = calcPlanetAspects( PlanetId::Saturn );
	aspRah = calcPlanetAspects( PlanetId::Rahu );
	aspMar = calcPlanetAspects( PlanetId::Mars );
	aspKet = calcPlanetAspects( PlanetId::Ketu );

	calcNodeStrength();

	for ( Sign i : Enum<Sign>() )
	{
		nbAspectsOnRasi[(int)i] = calcSignAspects( i );
	}
	calcYears();
	calcCharaKarakas();

}

/*****************************************************
**
**   JaiminiExpert   ---   calcPada
**
******************************************************/
int JaiminiExpert::calcPada( const int &bhava )
{
	assert(( bhava < 12 ) && ( bhava >= 0 ));

	const Sign rasi = (Sign)red12( (int)this->getRasi( PlanetId::Ascendant ) + bhava );
	const PlanetId lrd = getJaiminiLord( rasi );
	int diff = red_sign( this->getRasi( lrd ), rasi );
	int pada = red12( (int)rasi + 2 * diff );
	if ( ! astroconfig->jaimini.arudhaMode && ! ( diff % 3 )) pada = red12( pada + 9 );
	return pada;
}


/*****************************************************
**
**   JaiminiExpert   ---   getStrongerRasi
**
******************************************************/
Sign JaiminiExpert::getStrongerRasi( const Sign &r1, const Sign &r2 )
{
	assert_valid_enum<Sign>( r1 );
	assert_valid_enum<Sign>( r2 );

	const int rasi1 = (int)r1;
	const int rasi2 = (int)r2;

	// Rule 1a
	if ( nbplanets[rasi1] > nbplanets[rasi2] )
	{
		return r1;
	}
	// Rule 1b
	else if ( nbplanets[rasi1] < nbplanets[rasi2] )
	{
		return r2;
	}
	// Rule 2a
	else if ( nbAspectsOnRasi[rasi1] > nbAspectsOnRasi[rasi2] )
	{
		return r1;
	}
	// Rule 2b
	else if ( nbAspectsOnRasi[rasi1] < nbAspectsOnRasi[rasi2] )
	{
		return r2;
	}
	// **** TODO RULE3
	// Rule 4a
	else if ( this->getRasiLongitude(getJaiminiLord( r1 )) > this->getRasiLongitude(getJaiminiLord( r2 )) )
	{
		return r1;
	}
	// Rule 4b
	else if ( this->getRasiLongitude(getJaiminiLord( r1 )) < this->getRasiLongitude(getJaiminiLord( r2 )) )
	{
		return r2;
	}
	else assert( false );
	return Sign::None;
}

/*****************************************************
**
**   JaiminiExpert   ---   getJaiminiLord
**
******************************************************/
PlanetId JaiminiExpert::getJaiminiLord( const Sign &rasi )
{
	PlanetId lrd = ::getLord( rasi );
	if (( rasi == Sign::Scorpio ) && ( ketuStrongerThanMars )) lrd = PlanetId::Ketu;
	if (( rasi == Sign::Aquarius ) && ( rahuStrongerThanSaturn )) lrd = PlanetId::Rahu;

	return lrd;
}

/*****************************************************
**
**   JaiminiExpert   ---   isKetuStrongerThanMars
**
******************************************************/
bool JaiminiExpert::isKetuStrongerThanMars()
{
	return ketuStrongerThanMars;
}

/*****************************************************
**
**   JaiminiExpert   ---   isRahuStrongerThanSaturn
**
******************************************************/
bool JaiminiExpert::isRahuStrongerThanSaturn()
{
	return rahuStrongerThanSaturn;
}

/*****************************************************
**
**   JaiminiExpert   ---   write
**
******************************************************/
void JaiminiExpert::write( MDocument_ptr doc, const bool show_header )
{
	int i;
	PlanetId k_id;
	Lang lang;
	const Viewpoint viewpoint = getViewpoint();

	if ( show_header )
	{
		doc->addHeader2 ( tr( "Jaimini" ));
		doc->addLine( std::format( "{}: {}", tr( "Varga"), lang.getVargaName( varga )));
	}

	// 7 planets in Raman mode, Rahu included otherwise
	//const int max_karakas = astroconfig->vedicCalculation.charaKarakaMode ? CharaKaraka::Gnati : CharaKaraka::Dara;
	//const int max_karakas = astroconfig->vedicCalculation.charaKarakaMode ? 7 : 8;

	Table *t1 = new Table( 2, 9 );
	t1->setTitle( tr( "Chara Karakas" ));
	t1->setHeader( 0, tr( "Karaka" ));
	t1->setHeader( 1, tr( "Planet" ));
	int line = 1;
	//for ( i = CharaKaraka::Atma; i <= max_karakas; i++ )
	for( const CharaKaraka ck : Enum<CharaKaraka>())
	{
		if ( astroconfig->jaimini.charaKarakaMode && ck == CharaKaraka::Dara ) continue;
		t1->setEntry( 0, line, lang.getKarakaName( ck) );
		k_id = getCharaKaraka( ck);
		if ( k_id > PlanetId::Saturn ) k_id = PlanetId::Rahu;
		t1->setEntry( 1, line, MToken( k_id, viewpoint ));
		line++;
	}
	doc->addItem( t1 );

	Table *t2 = new Table( 2, 3 );
	t2->setTitle( tr( "Stronger Planets" ));
	t2->setHeader( 0, tr( "Pair" ));
	t2->setHeader( 1, tr( "Stronger" ));
	t2->setEntry( 0, 1, tr( "Rahu/Saturn" ));
	t2->setEntry( 0, 2, tr( "Ketu/Mars" ));

	if ( isRahuStrongerThanSaturn()) t2->setEntry( 1, 1, MToken( PlanetId::Rahu, viewpoint ));
	else t2->setEntry( 1, 1, MToken( PlanetId::Saturn, viewpoint ) );

	if ( isKetuStrongerThanMars()) t2->setEntry( 1, 2, MToken( PlanetId::Ketu, viewpoint ));
	else t2->setEntry( 1, 2, MToken( PlanetId::Mars, viewpoint ));
	doc->addItem( t2 );

	Table *t3 = new Table( 2, 13 );
	t3->setTitle( tr( "Arudha Padas" ));
	t3->setHeader( 0, tr( "House" ));
	t3->setHeader( 1, tr( "Sign" ));
	for ( i = 0; i < 12; i++ )
	{
		t3->setEntry( 0, i+1, std::format( "{}", i + 1 ));
		t3->setEntry( 1, i+1, MToken( (Sign)calcPada( i ), viewpoint ));
	}
	doc->addItem( t3 );

	Table *t4 = new Table( 2, 13 );
	t4->setTitle( tr( "Years" ));
	t4->setHeader( 0, tr( "Sign" ));
	t4->setHeader( 1, tr( "Years" ));
	for ( i = 0; i < 12; i++ )
	{
		t4->setEntry( 0, i+1, MToken( (Sign)i, viewpoint ));
		t4->setEntry( 1, i+1, std::format( "{}", getRasiYears( (Sign)i )));
	}
	doc->addItem( t4 );
}

/*****************************************************
**
**   JaiminiExpert   ---   calcPlanetAspects
**
******************************************************/
int JaiminiExpert::calcPlanetAspects( const PlanetId &planet )
{
	int ret = 0;

	if ( hasJaiminiAspect( this->getRasi(PlanetId::Jupiter), this->getRasi(planet) )) ret++;
	if ( hasJaiminiAspect( this->getRasi(PlanetId::Mercury), this->getRasi(planet) )) ret++;

	Sign rasi1 = this->getRasi(planet);
	PlanetId lord1 = getJaiminiLord( rasi1 );
	if ( hasJaiminiAspect( this->getRasi(lord1), this->getRasi(planet) )) ret++;

	return ret;
}


/*****************************************************
**
**   JaiminiExpert   ---   calcNodeStrength
**
******************************************************/
void JaiminiExpert::calcNodeStrength()
{
	ketuStrongerThanMars = false;
	rahuStrongerThanSaturn = false;

	const PlanetId inode = PlanetId::Rahu;
	const PlanetId descnode = PlanetId::Ketu;

	/**********************************************
	** RAHU -- SATURN
	**********************************************/
	// ** RULE 1: Jupiter aspects rahu -> rahu in fall
	if ( hasJaiminiAspect( this->getRasi(PlanetId::Jupiter), this->getRasi(inode) ))
	{
		rahuStrongerThanSaturn = false;
	}
	// RULE 2a
	else if (( this->getRasi(PlanetId::Saturn) == Sign::Aquarius ) && ( this->getRasi(inode) != Sign::Aquarius ))
	{
		rahuStrongerThanSaturn = true;
	}
	// RULE 2b
	else if (( this->getRasi(PlanetId::Saturn) != Sign::Aquarius ) && ( this->getRasi(inode) == Sign::Aquarius ))
	{
		rahuStrongerThanSaturn = false;
	}
	// RULE 3a
	else if ( nbplanets[(int)this->getRasi(PlanetId::Saturn)] > nbplanets[(int)this->getRasi(inode)] )
	{
		rahuStrongerThanSaturn = false;
	}
	// RULE 3b
	else if ( nbplanets[(int)this->getRasi(PlanetId::Saturn)] < nbplanets[(int)this->getRasi(inode)])
	{
		rahuStrongerThanSaturn = true;
	}
	// RULE 4a
	else if ( aspSat > aspRah )
	{
		rahuStrongerThanSaturn = false;
	}
	// RULE 4b
	else if ( aspSat < aspRah )
	{
		rahuStrongerThanSaturn = true;
	}
	// RULE 5a
	else if ( hasExaltationRasi( PlanetId::Saturn, this->getRasi(PlanetId::Saturn) ) && ! hasExaltationRasi( inode, this->getRasi(inode) ))
	{
		rahuStrongerThanSaturn = false;
	}
	// RULE 5b
	else if ( ! hasExaltationRasi( PlanetId::Saturn, this->getRasi(PlanetId::Saturn) ) && hasExaltationRasi( inode, this->getRasi(inode) ))
	{
		rahuStrongerThanSaturn = true;
	}
	// RULE 8a
	else if ( this->getRasiLongitude(PlanetId::Saturn) > this->getRasiLongitude(inode) )
	{
		rahuStrongerThanSaturn = false;
	}
	// RULE 8b
	else if ( this->getRasiLongitude(PlanetId::Saturn) < this->getRasiLongitude(inode) )
	{
		rahuStrongerThanSaturn = true;
	}
	else
	{
		rahuStrongerThanSaturn = false;
		//exit(1);
	}

	/**********************************************
	** KETU -- MARS
	**********************************************/
	//** RULE 1: Jupiter aspects ketu -> ketu in fall
	if ( hasJaiminiAspect( this->getRasi(PlanetId::Jupiter), this->getRasi(descnode) ))
	{
		ketuStrongerThanMars = false;
	}
	// RULE 2a
	else if (( this->getRasi(PlanetId::Mars) == Sign::Scorpio ) && ( this->getRasi(descnode) != Sign::Scorpio ))
	{
		ketuStrongerThanMars = true;
	}
	// RULE 2b
	else if (( this->getRasi(PlanetId::Mars) != Sign::Scorpio ) && ( this->getRasi(descnode) == Sign::Scorpio ))
	{
		ketuStrongerThanMars = false;
	}
	// RULE 3a
	else if ( nbplanets[(int)this->getRasi(PlanetId::Mars)] > nbplanets[(int)this->getRasi(descnode)] )
	{
		ketuStrongerThanMars = false;
	}
	// RULE 3b
	else if ( nbplanets[(int)this->getRasi(PlanetId::Mars)] < nbplanets[(int)this->getRasi(descnode)] )
	{
		ketuStrongerThanMars = true;
	}
	// RULE 4a
	else if ( aspMar > aspKet )
	{
		ketuStrongerThanMars = false;
	}
	// RULE 4b
	else if ( aspMar < aspKet )
	{
		ketuStrongerThanMars = true;
	}
	// RULE 5a
	else if ( hasExaltationRasi( PlanetId::Mars, this->getRasi(PlanetId::Mars) ) && ! hasExaltationRasi( descnode, this->getRasi(descnode) ))
	{
		ketuStrongerThanMars = false;
	}
	// RULE 5b
	else if ( ! hasExaltationRasi( PlanetId::Mars, this->getRasi(PlanetId::Mars) ) && hasExaltationRasi( descnode, this->getRasi(descnode) ))
	{
		ketuStrongerThanMars = true;
	}
	// **** TODO: RULE 6 + 7
	// RULE 8a
	else if ( this->getRasiLongitude(PlanetId::Mars) > this->getRasiLongitude(descnode) )
	{
		ketuStrongerThanMars = false;
	}
	// RULE 8b
	else if ( this->getRasiLongitude(PlanetId::Mars) < this->getRasiLongitude(descnode) )
	{
		ketuStrongerThanMars = true;
	}
	else
	{
		ketuStrongerThanMars = false;
		//exit(1);
	}
}

/*****************************************************
**
**   JaiminiExpert   ---   calcSignAspects
**
******************************************************/
int JaiminiExpert::calcSignAspects( const Sign &rasi )
{
	int ret = 0;

	if ( hasJaiminiAspect( this->getRasi(PlanetId::Jupiter), rasi )) ret++;
	if ( hasJaiminiAspect( this->getRasi(PlanetId::Mercury), rasi )) ret++;

	PlanetId lrd = getJaiminiLord( rasi );

	if ( hasJaiminiAspect( this->getRasi(lrd), rasi )) ret++;

	return ret;
}


/*****************************************************
**
**   JaiminiExpert   ---   calcYears
**
******************************************************/
void JaiminiExpert::calcYears()
{
	for ( Sign rasi : Enum<Sign>() )
	{

		PlanetId ll = getJaiminiLord( rasi );
		Sign pos = this->getRasi( ll );

		if ( isOddFootedRasi( rasi ))
		{
			rasiYears[(int)rasi] = red_sign( pos, rasi );
		}
		else
		{
			rasiYears[(int)rasi] = red_sign( rasi, pos );
		}

		if ( rasiYears[(int)rasi] == 0 )
		{
			rasiYears[(int)rasi] = 12;
		}
		else
		{
			if (( hasExaltationRasi( ll, pos  )) && ( ll != PlanetId::Mercury  ))
			{
				rasiYears[(int)rasi]++;
			}
			else if ( hasDebilationRasi( ll, pos  ))
			{
				rasiYears[(int)rasi]--;
			}
		}
	}
}

/*****************************************************
**
**   JaiminiExpert   ---   getRasiYears
**
******************************************************/
int JaiminiExpert::getRasiYears( const Sign &rasi )
{
	assert_valid_enum<Sign>( rasi );
	return rasiYears[(int)rasi];
}

/*****************************************************
**
**   JaiminiExpert   ---   getCharaKaraka
**
******************************************************/
PlanetId JaiminiExpert::getCharaKaraka( const CharaKaraka &ck )
{
	assert_valid_enum<CharaKaraka>( ck );
	return chara_karaka[(int)ck];
}

/*****************************************************
**
**   JaiminiExpert   ---   getCharaKarakaProperty
**
******************************************************/
CharaKaraka JaiminiExpert::getCharaKarakaProperty( const PlanetId &planet )
{
	// Planet index is 0..6 (PlanetId::Sun...PlanetId::Saturn) or 7 standing for Rahu
	assert(( (int)planet >= 0 && (int)planet < 8 ));

	// if Chara karaka mode is Raman (seven planets only) Rahu will be no Karaka
	if ( astroconfig->jaimini.charaKarakaMode && (int)planet == 8 ) return CharaKaraka::None;

	return p_chara_karaka[(int)planet];
}

/*****************************************************
**
**   JaiminiExpert   ---   calcCharaKarakas
**
******************************************************/
void JaiminiExpert::calcCharaKarakas()
{
	PlanetId p;
	double max_len;

	const int max_karakas = astroconfig->jaimini.charaKarakaMode ? 7 : 8;
	double rl[8];

	// set Rahu, in Raman mode no Karaka property
	p_chara_karaka[7] = CharaKaraka::None;

	const PlanetId inode = PlanetId::Rahu;

	for ( int i = 0; i < max_karakas; i++ )
	{
		p = ( i <= (int)PlanetId::Saturn ? (PlanetId)i : inode );
		rl[i] = this->getRasiLongitude(p);
	}
	for ( int i = 0; i < max_karakas; i++ )
	{
		max_len = rl[0];
		chara_karaka[i] = PlanetId::Sun;
		for ( int j = (int)PlanetId::Moon; j < max_karakas; j++ )
		{
			if ( rl[j] > max_len )
			{
				max_len = rl[j];
				chara_karaka[i] = (PlanetId)j;
			}
		}
		rl[(int)chara_karaka[i]] = 0;
		p_chara_karaka[(int)chara_karaka[i]] = (CharaKaraka)i;
	}
}

