/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/AshtottariDasa.cpp
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
#include "ContextProperties.h"
#include "Dasa.h"
#include "Horoscope.h"
#include "logger.h"

/*************************************************//**
*
*  calculation of Ashtottari Dasa
*
******************************************************/
class AshtottariDasaExpert : public DasaExpert
{
public:

	AshtottariDasaExpert() : DasaExpert( DasaId::Ashtottari ) {}

	/*****************************************************
	**
	**   AshtottariDasaExpert   ---   getFirstLevel
	**
	******************************************************/
	std::list<Dasa_ptr> getFirstLevel( ContextProps_ptr props, Dasa_ptr root, const int maxMahadasaCount = 0 ) override
	{
		assert( root );
		assert( root->getLevel() == DasaLevel::Root );
		assert( root->getExpert()->getDasaId() == DasaId::Ashtottari );

		constexpr int max_count = 100;
		const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Vedic );
		Horoscope_ptr horoscope = props->getRadixHoroscope();
		assert( horoscope );
		const Nakshatra28 mnak = getNakshatra28( horoscope->getLongitude( PlanetId::Moon, viewpoint ));

		int startn = (int)mnak - 5;
		if ( startn < 0 ) startn += 28;

		double nportion, portion;
		nportion = ::getNakshatraLongitude28( horoscope->getLongitude( PlanetId::Moon, viewpoint )) / 13.33333333;

		int pindex;

		// Ardra, Punarvasu, Pusyami, Ashlesha
		if ( startn < 4 )
		{
			// Sun dasa
			pindex = 0;
			portion = ( startn + nportion ) / 4;
		}
		// Makha, P.Phalguni, U.Phalguni
		else if ( startn < 7 )
		{
			// Moon dasa
			pindex = 1;
			portion = ( startn - 4 + nportion ) / 4;
		}
		// Hasta, Chitta, Swati, Vishakha
		else if ( startn < 11 )
		{
			// Mars dasa
			pindex = 2;
			portion = ( startn - 7 + nportion ) / 4;
		}
		// Anuradha, Jyeshta, Moola
		else if ( startn < 14 )
		{
			// Mercury dasa
			pindex = 3;
			portion = ( startn - 11 + nportion ) / 4;
		}
		// P.Shadha, U.Shadha, Abhijit, Sravana
		else if ( startn < 18 )
		{
			// Saturn dasa
			pindex = 4;
			portion = ( startn - 14 + nportion ) / 4;
		}
		// Dhanista, Satabhish, P.Bhadra
		else if ( startn < 21 )
		{
			// Jupiter dasa
			pindex = 5;
			portion = ( startn - 18 + nportion ) / 4;
		}
		// U.Bhadra, Revati, Aswini, Bharani
		else if ( startn < 25 )
		{
			// Rahu dasa
			pindex = 6;
			portion = ( startn - 21 + nportion ) / 4;
		}
		// Krittika, Rohini, Mrigasira
		else if ( startn < 28 )
		{
			// Venus dasa
			pindex = 7;
			portion = ( startn - 25 + nportion ) / 4;
		}
		else
		{
			errorlog << "Error Nakshatra index in Ashtottari is " << startn;
			exit(1);
		}

		DasaLord dlord = dasa_lord[pindex];
		double start_jd = horoscope->getJD() - portion * dasa_dur[pindex] * getYearLength();
		double end_jd;

		std::list<std::shared_ptr<Dasa>> r;
		Dasa_ptr mahadasa( new Dasa( root ));
		int count = 0;
		while( ++count < max_count )
		{
			end_jd = start_jd + dasa_dur[pindex] * getYearLength();

			assert( mahadasa );
			mahadasa->lord = dlord;
			mahadasa->start_jd = start_jd;
			mahadasa->end_jd = end_jd;

			pindex = ( pindex + 1 ) % 8;
			dlord = dasa_lord[pindex];
			start_jd = end_jd;

			r.push_back( mahadasa );

			// break condition
			if ( maxMahadasaCount > 0 )
			{
				if ( count >= maxMahadasaCount ) break;
			}
			else
			{
				if (  start_jd > getMaxEndJD( horoscope )) break;
			}
			mahadasa = Dasa_ptr( new Dasa( root ));
		}

		if ( count > max_count )
		{
			errorlog << "AshtottariDasaExpert::getFirstLevel: loop level " << count << " too high";
		}
		return r;
	}

	/*****************************************************
	**
	**   AshtottariDasaExpert   ---   getNextLevel
	**
	******************************************************/
	std::list<Dasa_ptr> getNextLevel( ContextProps_ptr, Dasa_ptr dasa ) override
	{
		assert( dasa );
		assert( dasa->getLevel() >= DasaLevel::Mahadasa );
		assert( dasa->getExpert()->getDasaId() == DasaId::Ashtottari );
		int i;

		double start_jd = dasa->getStartJD();
		double end_jd = dasa->getEndJD();
		double dasa_len = end_jd - start_jd;
		double antar_len = 0;
		DasaLord dlord = dasa->getDasaLord();
		int lordindex = -1;
		for ( i = 0; i < 8; i++ )
		{
			if ( dlord == dasa_lord[i] ) lordindex = i;
		}
		assert( lordindex >= 0 );

		// Guru Venus is left handed
		lordindex -= 1;
		if ( lordindex < 0 ) lordindex = 7;

		std::list<std::shared_ptr<Dasa>> r( getNbLords());
		for( auto &a : r ) a = Dasa_ptr( new Dasa( dasa ));
		
		for ( auto &antardasa : r )
		//for( Dasa *antardasa : dasa->children )
		{
			antar_len = ( dasa_len * dasa_dur[lordindex] ) / 108;
			end_jd = start_jd + antar_len;
			
			antardasa->lord = dlord;
			antardasa->start_jd = start_jd;
			antardasa->end_jd = end_jd;

			lordindex -= 1;
			if ( lordindex < 0 ) lordindex = 7;
			dlord = DasaLord( dasa_lord[lordindex] );
			start_jd = end_jd;
		}
		return r;
	}

	/*****************************************************
	**
	**   AshtottariDasaExpert   ---   getKPEventList
	**
	******************************************************/
	virtual std::list<KPEvent> getKPEventList( const double& /*startlen*/,
		const double& /*endlen*/, const double& /*startjd*/ ) override
	{
		// never used
		assert( false );
		return std::list<KPEvent>();
	}

	virtual KpData getKPLords( const double& ) override { return KpData(); }

private:

	virtual int getNbLords() const override { return 8; }

	const inline static DasaLord dasa_lord[8] =
	{
		PlanetId::Sun,
		PlanetId::Moon,
		PlanetId::Mars,
		PlanetId::Mercury,
		PlanetId::Saturn,
		PlanetId::Jupiter,
		PlanetId::Rahu,
		PlanetId::Venus
	};

	const inline static int dasa_dur[8] = { 6, 15, 8, 17, 10, 19, 12, 21  };
};

/*****************************************************
**
**   DasaExpertFactory   ---   create
**
******************************************************/
DasaExpert *DasaExpertFactory::getAshtottariDasaExpert() { return new AshtottariDasaExpert(); }
