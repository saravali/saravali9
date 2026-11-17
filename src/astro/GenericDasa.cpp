/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/GenericDasa.cpp
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

#include "GenericDasa.h"

#include "astrobase.h"
#include "ContextProperties.h"
#include "DasaTool.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "Horoscope.h"
#include "logger.h"
#include "mathbase.h"

#define DEBUG_GENERICDASA

/*****************************************************
**
**   GenericNakshatraDasaExpert   ---   getPortion
**
******************************************************/
std::pair<double, Nakshatra27> GenericNakshatraDasaExpert::getPortion( ContextProps_ptr props, Horoscope_ptr h )
{
	return DasaTool::get()->getNakshatraPortion( h, PlanetId::Moon, props->getEffectiveViewpoint( Viewpoint::Vedic ));
}

/*****************************************************
**
**   GenericNakshatraDasaExpert   ---   getFirstLevel
**
******************************************************/
std::list<Dasa_ptr> GenericNakshatraDasaExpert::getFirstLevel( ContextProps_ptr props, Dasa_ptr root, const int maxMahadasaCount  )
{
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Vedic );
	assert( props );
	Horoscope_ptr h = props->getRadixHoroscope();
	assert( h );
	assert( root );
	assert( root->getLevel() == DasaLevel::Root );

#ifdef DEBUG_GENERICDASA
	debuglog << "GenericNakshatraDasaExpert::getFirstLevel viewpoint " << viewpoint
		<< " Start of last mahadasa " << DateFormatter::get()->formatUTDatetime( getMaxEndJD( h ));
#endif

	const std::pair<double, Nakshatra27> p = getPortion( props, h );
	const int pindex = getDasaIndex4Nakshatra( p.second );
	root->lord = getDasaLordByIndex( pindex );
	root->paramayus = getParamayus();

	root->start_jd = h->getJD() - p.first * getDasaDuration( pindex ) * getYearLength();
	root->end_jd = root->start_jd + getParamayus() * getYearLength();

	int lord = pindex;

	std::list<std::shared_ptr<Dasa>> r;
	double start_jd = root->getStartJD();
	const double dasa_len = root->getEndJD() - start_jd;
	assert( dasa_len > 0.0 );

	double end_jd = 0.0;
	double antar_len = 0.0;
	int count = 0;

	while( true )
	{
		antar_len = ( dasa_len * getDasaDuration( lord )) / getParamayus();
		end_jd = start_jd + antar_len;

		Dasa_ptr child( new Dasa( root ));

		child->lord = getDasaLordByIndex( lord );
		child->start_jd = start_jd;
		child->end_jd = end_jd;
		r.push_back( child );

		lord = ( lord + 1 ) % getNbLords();
		start_jd = end_jd;

		// avoid endless loop if something goes wrong
		assert( ++count < 100 );

		// break condition
		if ( maxMahadasaCount > 0 )
		{
			if ( count >= maxMahadasaCount ) break;
		}
		else
		{
			if (  start_jd > getMaxEndJD( h )) break;
		}
	}

	assert( 	r.size() > 0 );
	root->balance = r.front()->getEndJD() - h->getJD();
	return r;
}

/*****************************************************
**
**   GenericNakshatraDasaExpert   ---   getNextLevel
**
******************************************************/
std::list<Dasa_ptr> GenericNakshatraDasaExpert::getNextLevel( ContextProps_ptr props, Dasa_ptr dasa )
{
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Vedic );
	assert( props );
	Horoscope_ptr h = props->getRadixHoroscope();
	assert( h );
	assert( dasa );
	assert( dasa->getLevel() > DasaLevel::Root );

	debuglog << "GenericNakshatraDasaExpert::getNextLevel viewpoint " << viewpoint;

	std::list<std::shared_ptr<Dasa>> r( getNbLords());
	for( auto &a : r ) a = Dasa_ptr( new Dasa( dasa ));

	updateChildren( dasa, r );
	return r;
}

/*****************************************************
**
**   GenericNakshatraDasaExpert   ---   findDasaLordIndex
**
******************************************************/
int GenericNakshatraDasaExpert::findDasaLordIndex( const DasaLord &lord )
{
	int index = -1;
	for ( int i = 0; i < getNbLords(); i++ )
	{
		if ( getDasaLordByIndex( i ) == lord )
		{
			index = i;
			break;
		}
	}
	assert( index != -1 );
	return index;
}

/*****************************************************
**
**   GenericNakshatraDasaExpert   ---   updateChildren
**
******************************************************/
void GenericNakshatraDasaExpert::updateChildren( Dasa_ptr dasa, std::list<Dasa_ptr> &children )
{
	assert( dasa );
	assert( dasa->getLevel() >= DasaLevel::Root );

	assert( children.size() > 0 );

	double start_jd = dasa->getStartJD();
	const double dasa_len = dasa->getEndJD() - start_jd;
	assert( dasa_len > 0.0 );

	double end_jd = 0.0;
	double antar_len = 0.0;

#if 1
	int lord = findDasaLordIndex( dasa->getDasaLord());

#else
	int lord = -1;
	for ( int i = 0; i < getNbLords(); i++ )
	{
		if ( getDasaLordByIndex( i ) == dasa->getDasaLord())
		{
			lord = i;
			break;
		}
	}
	assert( lord != -1 );
	debuglog << "level " << dasa->getLevel() << " parent " << dasa.get();
#endif

	for ( auto &child : children )
	{
		antar_len = ( dasa_len * getDasaDuration( lord )) / getParamayus();
		end_jd = start_jd + antar_len;

		child->lord = getDasaLordByIndex( lord );
		child->start_jd = start_jd;
		child->end_jd = end_jd;

		lord = ( lord + 1 ) % getNbLords();
		start_jd = end_jd;
	}
}

/*****************************************************
**
**   GenericNakshatraDasaExpert   ---   getKPLordRecursive
**
******************************************************/
void GenericNakshatraDasaExpert::getKPLordRecursive( int &lord_index, double &elapsed, double &total_len )
{
	int count = 0;
	constexpr int max_loop = 10; // maximum number of antardasas in cycle
	double antar_len; // in years

	if ( elapsed >= total_len ) errorlog << "GenericNakshatraDasaExpert::getKPLordRecursive: elapsed time too big, "
		<< elapsed << " total was " << total_len;
	assert ( elapsed < total_len );

	antar_len = getDasaDuration( lord_index ) * total_len;
	antar_len /= getParamayus();

	// count so many antardasas that elapsed time is more than antarlen
	while (( elapsed > antar_len ) && ( count++ < max_loop  ))
	{
		lord_index++;
		if ( lord_index >= getNbLords() ) lord_index -= getNbLords();
		elapsed -= antar_len;

		antar_len = getDasaDuration( lord_index ) * total_len;
		antar_len /= getParamayus();
	}
	if ( count >= max_loop )
	{
		warnlog << "GenericNakshatraDasaExpert::getKPLordRecursive: count " << count << " amx_loop " << max_loop;
	}
	assert( count < max_loop );
	total_len = antar_len;
}

/*****************************************************
**
**   GenericNakshatraDasaExpert   ---   getKPLords
**
******************************************************/
KpData GenericNakshatraDasaExpert::getKPLords( const double &len )
{
	KpData kp;

	// 0...27 - ASWINI = 0 etc
	Nakshatra27 nak = getNakshatra27( len );

	// index of the first lord for that Nakshatra in the sequence of lords, e.g. Vimsottari has Sun, Moon, Mars, Rahu, Jupiter, Saturn, Mercury, Ketu, Venus
	int lord_index = getDasaIndex4Nakshatra( nak );

	// elapsed portion in the Nakshatra (0...1)
	double nakshatraPortion = getNakshatraLongitude27( len ) / NAKSHATRA_LEN;

	// elapsed time in years
	double elapsed = nakshatraPortion * getDasaDuration( lord_index );

	// total length in years (e.g. 120 for Vimsottari)
	double total_len = getParamayus();

	getKPLordRecursive( lord_index, elapsed, total_len );
	kp.lord = getDasaLordByIndex( lord_index );

	getKPLordRecursive( lord_index, elapsed, total_len );
	kp.sublord = getDasaLordByIndex( lord_index );

	getKPLordRecursive( lord_index, elapsed, total_len );
	kp.subsublord = getDasaLordByIndex( lord_index );

	return kp;
}

/*****************************************************
**
**   GenericNakshatraDasaExpert   ---   getKPEventList
**
******************************************************/
std::list<KPEvent> GenericNakshatraDasaExpert::getKPEventList( const double &startlen, const double &endlen,
	const double &startjd )
{
	std::list<KPEvent> events;
	Nakshatra27 currentnak;
	int currentlord, i, j, antarlord, antardasatime;
	double dasastartlen, antarstartlen, dist;
	bool append;

	Nakshatra27 startnak = getNakshatra27( startlen );

	for ( i = 0; i < 32; i++ )
	{
		currentnak = (Nakshatra27)(((int)startnak + i ) % 27 );
		currentlord = getDasaIndex4Nakshatra( currentnak );
		dasastartlen = red_deg( ( (int)startnak + i ) * 13.33333333 );
		antarstartlen = dasastartlen;

		for ( j = 0; j < getNbLords(); j++ )
		{
			append = true;
			antarlord = ( currentlord  + j ) % getNbLords();
			antardasatime = getDasaDuration( antarlord ); // years

			if ( i == 0 ) // cut off at the beginning (antardasas before starting point)
			{
				dist = antarstartlen - startlen;
				if ( dist < -300 ) dist += 360;  // correct if 360-deg shift
				if ( dist < 0 ) append = false;
			}
			if ( i >= 28 )  // cut off at the end (if event belongs to next time interval)
			{
				dist = antarstartlen - endlen;
				if ( dist > 300 ) dist -= 360;
				if ( dist < -300 ) dist += 360;
				if ( dist > 0 ) append = false;     // could also be return ?
			}
			if ( append )
				events.push_back({ getDasaLordByIndex( currentlord ), getDasaLordByIndex( antarlord ),
					antarstartlen + .0000001, startjd + i + (double)j/getNbLords(), i });
			antarstartlen += 13.333333333 * (double)antardasatime / getParamayus();
		}
	}

	return events;
}

/*****************************************************
**
**   GenericNakshatraDasaExpert   ---   createSolarDasas
**
******************************************************/
std::list<Dasa_ptr> GenericNakshatraDasaExpert::createSolarDasas( ContextProps_ptr props, const double &solarjd, const int &year )
{
	int currentdasa;
	double start_jd, end_jd;
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Vedic );
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );

	// lenght of generic dasas is in days. Solar Dasa shifts from e.g. 120 y to 365.25 days
	//const double duration_factor = 365.25 / (double)getParamayus();
	const double duration_factor = getYearLength() / (double)getParamayus();

	const Nakshatra27 birthnaks = getNakshatra27( horoscope->getLongitude( PlanetId::Moon, viewpoint ));

	const int numyears = year - getYearForJD( horoscope->getJD());
	int basedasa = ( (int)birthnaks + numyears - 2 ) % 9;
	if ( basedasa < 0 ) basedasa += 9;
	assert( basedasa >= 0 && basedasa < 9 );

	start_jd = solarjd;

	Dasa_ptr rootItem( new Dasa( nullptr, nullptr, DasaLevel::Root  ));
	std::list<Dasa_ptr> r;
	for( int i = 0; i < 9; i++ )
	{
		currentdasa = ( basedasa + i ) % 9;
		end_jd = start_jd + duration_factor * getDasaDuration( currentdasa );

		r.push_back( Dasa_ptr( new Dasa( rootItem, getDasaLordByIndex( currentdasa ), start_jd, end_jd  )));
		start_jd = end_jd;
	}
	return r;
}

/*************************************************//**
*
*   Basic calculation for Vimsottari related Dasas
*
******************************************************/
class VimsottariBaseDasaExpert : public GenericNakshatraDasaExpert
{
public:

	VimsottariBaseDasaExpert( const DasaId &dasaId ) : GenericNakshatraDasaExpert( dasaId ) {}

protected:
	virtual int getNbLords() const override { return 9; }
	virtual int getParamayus() const override { return 120; }

	virtual DasaLord getDasaLordByIndex( const int &i ) override
	{
		assert( i >= 0 && i < 9 );
		static const PlanetId dasa_lords[9] =
		{
			PlanetId::Sun,
			PlanetId::Moon,
			PlanetId::Mars,
			PlanetId::Rahu,
			PlanetId::Jupiter,
			PlanetId::Saturn,
			PlanetId::Mercury,
			PlanetId::Ketu,
			PlanetId::Venus
		};
		return dasa_lords[i];
	}

	virtual int getDasaIndex4Nakshatra( const Nakshatra27 &nak ) override { return ((int)nak + 7 ) % 9; }

	virtual int getDasaDuration( const int &i ) const override
	{
		assert( i >= 0 && i < 9 );
		static const int dasa_dur[9] = { 6, 10, 7, 18, 16, 19, 17, 7, 20  };
		return dasa_dur[i];
	}
};

/*************************************************//**
*
*  calculation of Vimsottari Dasa
*
******************************************************/
class VimsottariDasaExpert : public VimsottariBaseDasaExpert
{
public:

	VimsottariDasaExpert() : VimsottariBaseDasaExpert( DasaId::Vimsottari ) {}

protected:
};

/*************************************************//**
*
*  calculation of Lagna Vimsottari Dasa
*
******************************************************/
class LagnaVimsottariDasaExpert : public VimsottariBaseDasaExpert
{
public:

	LagnaVimsottariDasaExpert() : VimsottariBaseDasaExpert( DasaId::LagnaVimsottari ) {}

protected:

	// owerwrites Moon based calculation
	virtual std::pair<double, Nakshatra27> getPortion( ContextProps_ptr props, Horoscope_ptr horoscope ) override
	{
		return DasaTool::get()->getNakshatraPortion( horoscope, PlanetId::Ascendant,
			props->getEffectiveViewpoint( Viewpoint::Vedic ) );
	}
};

/*************************************************//**
*
*  expert class for Yogini Dasa
*
******************************************************/
class YoginiDasaExpert : public GenericNakshatraDasaExpert
{
public:

	YoginiDasaExpert() : GenericNakshatraDasaExpert( DasaId::Yogini ) {}

protected:
	virtual int getNbLords() const override { return 8; }

	virtual int getParamayus() const override { return 36; }

	virtual DasaLord getDasaLordByIndex( const int &index ) override
	{
		assert( index >= 0 && index < 8 );
		static constexpr PlanetId dasa_lords[8] = {
			PlanetId::Moon,
			PlanetId::Sun,
			PlanetId::Jupiter,
			PlanetId::Mars,
			PlanetId::Mercury,
			PlanetId::Saturn,
			PlanetId::Venus,
			PlanetId::Rahu
		};
		return dasa_lords[index];
	}

	virtual int getDasaIndex4Nakshatra( const Nakshatra27 &nak ) override
	{
		return (int)a_red( (int)nak + 3, 8 );
	}

	virtual int getDasaDuration( const int &index ) const override
	{
		assert( index >= 0 && index < 8 );
		return index+1;
	}

	virtual wxString getExtraLordName( Dasa_ptr dasa, const PlanetConfig &factorCfg,
		const PlanetPropertyConfig& ) override
	{
		assert( dasa );
		debuglog << "YoginiDasaExpert::getExtraLordName planet textformat " << (int)factorCfg.textFormat;

		const static wxString k_yogini[8] =
		{
			tr( "Mangala" ),
			tr( "Pingala" ),
			tr( "Dhanya" ),
			tr( "Bhramari" ),
			tr( "Bhadrika" ),
			tr( "Ulka" ),
			tr( "Siddha" ),
			tr( "Sankata" )
		};

		int i = 0;
		for( i = 0; i < 8; i++ )
		{
			if ( getDasaLordByIndex( i ) == dasa->getDasaLord())
			{
				break;
			}
		}
		//debuglog << i;
		assert( i >= 0 && i < 8 );
		wxString s = k_yogini[i];
		if ( ! s.empty())
		{
			switch( factorCfg.textFormat )
			{
				case TextFormat::Short:
					return s.substr( 0, 2 );
				break;
				case TextFormat::Medium:
					return s.substr( 0, 3 );
				break;
				case TextFormat::Long:
				default:
				break;
			}
		}
		return s;
	}
};

/*************************************************//**
*
*  expert class for Shodshottari Dasa
*
******************************************************/
class ShodshottariDasaExpert : public GenericNakshatraDasaExpert
{
public:

	ShodshottariDasaExpert() : GenericNakshatraDasaExpert( DasaId::Shodshottari ) {}

protected:
	virtual int getNbLords() const override { return 8; }

	virtual int getParamayus() const override { return 116; }

	virtual DasaLord getDasaLordByIndex( const int &index ) override
	{
		assert( index >= 0 && index < 8 );
		static constexpr PlanetId dasa_lords[8] =
		{
			PlanetId::Sun,
			PlanetId::Mars,
			PlanetId::Jupiter,
			PlanetId::Saturn,
			PlanetId::Ketu,
			PlanetId::Moon,
			PlanetId::Mercury,
			PlanetId::Venus
		};
		return dasa_lords[index];
	}

	virtual int getDasaIndex4Nakshatra( const Nakshatra27 &nak ) override
	{
		int diff = (int)a_red( (int)nak - (int)Nakshatra27::Pushyami, 27 );
		return (int)a_red( diff, 8 );
	}

	virtual int getDasaDuration( const int &index ) const override
	{
		assert( index >= 0 && index < 8 );
		static constexpr int dasa_dur[8] = { 11, 12, 13, 14, 15, 16, 17, 18 };
		return dasa_dur[index];
	}
};

/*************************************************//**
*
*  expert class for Dvadashottari Dasa
*
******************************************************/
class DvadashottariDasaExpert : public GenericNakshatraDasaExpert
{
public:

	DvadashottariDasaExpert() : GenericNakshatraDasaExpert( DasaId::Dvadashottari ) {}

protected:
	virtual int getNbLords() const override { return 8; }
	virtual int getParamayus() const override { return 112; }

	virtual DasaLord getDasaLordByIndex( const int &index ) override
	{
		assert( index >= 0 && index < 8 );
		static constexpr PlanetId dasa_lords[8] =
		{
			PlanetId::Sun,
			PlanetId::Jupiter,
			PlanetId::Ketu,
			PlanetId::Mercury,
			PlanetId::Rahu,
			PlanetId::Mars,
			PlanetId::Saturn,
			PlanetId::Moon
		};
		return dasa_lords[index];
	}

	virtual int getDasaIndex4Nakshatra( const Nakshatra27 &nak ) override
	{
		int diff = (int)a_red( 26 - (int)nak, 27 ); // 26 is Revati
		return (int)a_red( diff, 8 );
	}

	virtual int getDasaDuration( const int &index ) const override
	{
		assert( index >= 0 && index < 8 );
		static constexpr int dasa_dur[8] = { 7, 9, 11, 13, 15, 17, 19, 21 };
		return dasa_dur[index];
	}
};

/*************************************************//**
*
*  expert class for Panchottari Dasa
*
******************************************************/
class PanchottariDasaExpert : public GenericNakshatraDasaExpert
{
public:

	PanchottariDasaExpert() : GenericNakshatraDasaExpert( DasaId::Panchottari ) {}

protected:
	virtual int getNbLords() const override { return 7; }
	virtual int getParamayus() const override { return 105; }

	virtual DasaLord getDasaLordByIndex( const int &index ) override
	{
		assert( index >= 0 && index < 7 );
		static constexpr PlanetId dasa_lords[7] =
		{
			PlanetId::Sun,
			PlanetId::Mercury,
			PlanetId::Saturn,
			PlanetId::Mars,
			PlanetId::Venus,
			PlanetId::Moon,
			PlanetId::Jupiter
		};
		return dasa_lords[index];
	}

	virtual int getDasaIndex4Nakshatra( const Nakshatra27 &nak ) override
	{
		int diff = (int)a_red( (int)nak - (int)Nakshatra27::Anuradha, 27 );
		return (int)a_red( diff, 7 );
	}

	virtual int getDasaDuration( const int &index ) const override
	{
		assert( index >= 0 && index < 7 );
		static constexpr int dasa_dur[7] = { 12, 13, 14, 15, 16, 17, 18 };
		return dasa_dur[index];
	}
};

/*************************************************//**
*
*  expert class for Shatabdika Dasa
*
******************************************************/
class ShatabdikaDasaExpert : public GenericNakshatraDasaExpert
{
public:

	ShatabdikaDasaExpert() : GenericNakshatraDasaExpert( DasaId::Shatabdika ) {}

protected:
	virtual int getNbLords() const override { return 7; }
	virtual int getParamayus() const override { return 100; }

	virtual DasaLord getDasaLordByIndex( const int &index ) override
	{
		assert( index >= 0 && index < 7 );
		static constexpr PlanetId dasa_lords[7] =
		{
			PlanetId::Sun,
			PlanetId::Moon,
			PlanetId::Venus,
			PlanetId::Mercury,
			PlanetId::Jupiter,
			PlanetId::Mars,
			PlanetId::Saturn
		};
		return dasa_lords[index];
	}

	virtual int getDasaIndex4Nakshatra( const Nakshatra27 &nak ) override
	{
		int diff = (int)a_red( (int)nak - 26, 27 ); // 26 is Revati
		return (int)a_red( diff, 7 );
	}

	virtual int getDasaDuration( const int &index ) const override
	{
		assert( index >= 0 && index < 7 );
		static constexpr int dasa_dur[7] = { 5, 5, 10, 10, 20, 20, 30 };
		return dasa_dur[index];
	}
};

/*************************************************//**
*
*  expert class for Chaturashiti Sama Dasa
*
******************************************************/
class ChaturashitiSamaDasaExpert : public GenericNakshatraDasaExpert
{
public:

	ChaturashitiSamaDasaExpert() : GenericNakshatraDasaExpert( DasaId::ChaturashitiSama ) {} 
protected:
	virtual int getNbLords() const override { return 7; }
	virtual int getParamayus() const override { return 84; }

	virtual DasaLord getDasaLordByIndex( const int &index ) override
	{
		assert( index >= 0 && index < 7 );
		static constexpr PlanetId dasa_lords[7] =
		{
			PlanetId::Sun,
			PlanetId::Moon,
			PlanetId::Mars,
			PlanetId::Mercury,
			PlanetId::Jupiter,
			PlanetId::Venus,
			PlanetId::Saturn
		};
		return dasa_lords[index];
	}

	virtual int getDasaIndex4Nakshatra( const Nakshatra27 &nak ) override
	{
		int diff = (int)a_red( (int)nak - (int)Nakshatra27::Swati, 27 );
		return (int)a_red( diff, 7 );
	}

	virtual int getDasaDuration( const int& ) const override { return 12; }
};

/*************************************************//**
*
*  expert class for Dvisaptati Sama Dasa
*
******************************************************/
class DvisaptatiSamaDasaExpert : public GenericNakshatraDasaExpert
{
public:

	DvisaptatiSamaDasaExpert() : GenericNakshatraDasaExpert( DasaId::DvisaptatiSama ) {}

protected:
	virtual int getNbLords() const override { return 8; }
	virtual int getParamayus() const override { return 72; }

	virtual DasaLord getDasaLordByIndex( const int &index ) override
	{
		assert( index >= 0 && index < 8 );
		static constexpr PlanetId dasa_lords[8] =
		{
			PlanetId::Sun,
			PlanetId::Moon,
			PlanetId::Mars,
			PlanetId::Mercury,
			PlanetId::Jupiter,
			PlanetId::Venus,
			PlanetId::Saturn,
			PlanetId::Rahu
		};
		return dasa_lords[index];
	}

	virtual int getDasaIndex4Nakshatra( const Nakshatra27 &nak ) override
	{
		int diff = (int)a_red( (int)nak - 18, 27 );
		return (int)a_red( diff, 8 );
	}

	virtual int getDasaDuration( const int &index ) const override
	{
		assert( index >= 0 && index < 8 );
		return 9;
	}
};

/*************************************************//**
*
*  expert class for ShatTrimshat Sama Dasa
*
******************************************************/
class ShatTrimshatSamaDasaExpert : public GenericNakshatraDasaExpert
{
public:

	ShatTrimshatSamaDasaExpert() : GenericNakshatraDasaExpert( DasaId::ShatTrimsatSama ) {}

protected:
	virtual int getNbLords() const override { return 8; }
	virtual int getParamayus() const override { return 36; }

	virtual DasaLord getDasaLordByIndex( const int &index ) override
	{
		assert( index >= 0 && index < 8 );
		static constexpr PlanetId dasa_lords[8] =
		{
			PlanetId::Moon,
			PlanetId::Sun,
			PlanetId::Jupiter,
			PlanetId::Mars,
			PlanetId::Mercury,
			PlanetId::Saturn,
			PlanetId::Venus,
			PlanetId::Rahu
		};
		return dasa_lords[index];
	}

	virtual int getDasaIndex4Nakshatra( const Nakshatra27 &nak ) override
	{
		int diff = (int)a_red( (int)nak - 21, 27 );
		return (int)a_red( diff, 8 );
	}

	virtual int getDasaDuration( const int &index ) const override
	{
		assert( index >= 0 && index < 8 );
		return index + 1;
	}
};

/*****************************************************
**
**   DasaExpertFactory   ---   getGenericDasaExpert
**
******************************************************/
DasaExpert *DasaExpertFactory::getGenericDasaExpert( const DasaId &id )
{
	switch( id )
	{
		case DasaId::Vimsottari:
			return new VimsottariDasaExpert();
		break;
		case DasaId::Yogini:
			return new YoginiDasaExpert();
		break;
		case DasaId::Shodshottari:
			return new ShodshottariDasaExpert();
		break;
		case DasaId::Dvadashottari:
			return new DvadashottariDasaExpert();
		break;
		case DasaId::Panchottari:
			return new PanchottariDasaExpert();
		break;
		case DasaId::Shatabdika:
			return new ShatabdikaDasaExpert();
		break;
		case DasaId::ChaturashitiSama:
			return new ChaturashitiSamaDasaExpert();
		break;
		case DasaId::DvisaptatiSama:
			return new DvisaptatiSamaDasaExpert();
		break;
		case DasaId::ShatTrimsatSama:
			return new ShatTrimshatSamaDasaExpert();
		break;
		case DasaId::LagnaVimsottari:
			return new LagnaVimsottariDasaExpert();
		break;
		default:
			assert( false );
		break;
	}
	return nullptr;
}

