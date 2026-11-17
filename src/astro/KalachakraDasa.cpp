/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/KalachakraDasa.cpp
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

#include "KalachakraDasa.h"

#include "astrobase.h"
#include "astroconfig.h"
#include "ContextProperties.h"
#include "DasaTool.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "Table.h"

#include <format>

extern Astroconfig *astroconfig;

#define DEBUG_KALACHAKRADASA

/*************************************************//**
*
* Expert class for Kalachakra encapsulates calculation and more.
*
* Note that the associated KalachakraDasa class is richer than the original one.
*
******************************************************/
class KalachakraDasaExpert : public DasaExpert
{
public:

	KalachakraDasaExpert() : DasaExpert( DasaId::Kalachakra ) {}

	/*****************************************************
	**
	**   KalachakraDasaExpert   ---   getFirstLevel
	**
	******************************************************/
	virtual std::list<Dasa_ptr> getFirstLevel( ContextProps_ptr props, Dasa_ptr root, const int maxMahadasaCount = 0 ) override
	{
		constexpr int max_count = 100;
		const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Vedic );
		assert( props );
		Horoscope_ptr h = props->getRadixHoroscope();
		assert( h );

		assert( root );
		assert( root->getLevel() == DasaLevel::Root );

		#ifdef DEBUG_KALACHAKRADASA
		debuglog << "KalachakraDasaExpert::getFirstLevel (1) viewpoint " << viewpoint << " calculation mode " << astroconfig->dasa.kalachakraMode;
		#endif

		KalachakraDasa *kroot = dynamic_cast<KalachakraDasa*>( root.get() );
		assert( kroot );

		KalachakraGati gati = KalachakraGati::Default;
		Sign lord = Sign::None;
		Sign oldlord = Sign::None;

		const double mlen = h->getLongitude( PlanetId::Moon, viewpoint );

		const double portion = ( astroconfig->dasa.kalachakraMode == KalachakraMode::PortionZero ? 0
			: DasaTool::get()->getPadaPortion( h, PlanetId::Moon, props->getEffectiveViewpoint( Viewpoint::Vedic )));

		int pada = (int)( mlen / PADA_LEN );
		pada %= 4;
		kroot->pada = pada;
		int nakshatra = (int)::getNakshatra27( mlen );
		
		/*
		* Here some groups are identified since they have same order of signs and Paramayus (but NOT Amsa) -> Krittika=Bharani and Mrigasira=Ardra
		*/
		kroot->subgroup = K_NAKSHATRA_GROUP[nakshatra];
		KalachakraSubgroup subgroup = kroot->subgroup;
		const bool savya = kroot->getGroup() == KalachakraGroup::Savya;

		kroot->amsa = (Sign)(( nakshatra % 3  ) * 4 + pada );
		if ( ! savya ) kroot->amsa = K_APSAVYA_AMSA[(int)kroot->amsa];
		#ifdef DEBUG_KALACHAKRADASA
		debuglog << "KalachakraDasaExpert::getFirstLeveli (2) portion " << portion << " nakshatra " << nakshatra << " pada " << pada 
			<< " savya " << savya << " amsa1 " << ((Sign)(( nakshatra % 3  ) * 4 + pada )) << " amsa result " << kroot->amsa
			<< " subgroup " << kroot->subgroup;
		#endif

		kroot->paramayus = K_TOTALYEARS[(int)subgroup][pada];

		kroot->deha = savya ? K_KALA[(int)subgroup][pada][0] : K_KALA[(int)subgroup][pada][8];
		kroot->jeeva = savya ? K_KALA[(int)subgroup][pada][8] : K_KALA[(int)subgroup][pada][0];

		double elapsed = portion * kroot->paramayus;
		kroot->padaBalance = ( 1 - portion ) * kroot->paramayus;
		int column = 0;
		while ( elapsed - K_RASIYEARS[(int)K_KALA[(int)subgroup][pada][column]] > 0 )
		{
			elapsed -= K_RASIYEARS[(int)K_KALA[(int)subgroup][pada][column]];
			assert( column++ < 8 );
		}
		double start_jd = h->getJD() - elapsed  * getYearLength();
		kroot->firstMdBalance = K_RASIYEARS[(int)K_KALA[(int)subgroup][pada][column]] - elapsed;
		double end_jd = 0.0;

		#ifdef DEBUG_KALACHAKRADASA
		debuglog << "KalachakraDasaExpert::getFirstLevel (3) paramayus " << kroot->paramayus
			<< " deha " << kroot->deha << " jeeva " << kroot->jeeva;
		#endif

		std::list<Dasa_ptr> r;
		Dasa_ptr dasa( new KalachakraDasa( root ));
		int count = 0;
		while( ++count < max_count )
		{
			KalachakraDasa *mahadasa = dynamic_cast<KalachakraDasa*>( dasa.get() );
			assert( mahadasa );

			end_jd = start_jd + K_RASIYEARS[(int)K_KALA[(int)subgroup][pada][column]] * getYearLength();

			lord = K_KALA[(int)subgroup][pada][column];
			gati = calculateGati( lord, oldlord, isMarkatiField( subgroup, pada, column ));

			mahadasa->lord = lord;
			mahadasa->start_jd = start_jd;
			mahadasa->end_jd = end_jd;
			mahadasa->pada = pada;
			mahadasa->subgroup = subgroup;
			mahadasa->gati = gati;

			column++;
			if ( column > 8 )
			{
				column = 0;

				// KalachakraMode == 1 doesn't jump to next row
				// mode 0 goes to next row or next nakshatra if last pada.
				// Last pada of savya or apsavya goes to first nakshatra of same group
				if ( astroconfig->dasa.kalachakraMode == KalachakraMode::Progressive )
				{
					if ( pada < 3 ) pada++;
					else
					{
						pada  = 0;
						nakshatra++;
						if ( nakshatra > 27 ) nakshatra = 0;

						// On change of savya/apsava -> don't add one nakshatra, but go back to first nakshatra of same cycle
						if (( savya && (int)K_NAKSHATRA_GROUP[nakshatra] > 1 )
							|| ( ! savya && (int)K_NAKSHATRA_GROUP[nakshatra] <= 1 )) nakshatra -= 3;
						if ( nakshatra < 0 ) nakshatra += 27;
						subgroup = K_NAKSHATRA_GROUP[nakshatra];
					}
				}
				// Mode 2 stays in the same nakshatra
				else if ( astroconfig->dasa.kalachakraMode == KalachakraMode::ProgressiveSameNakshatra )
				{
					if ( pada < 3 ) pada++;
					else pada  = 0;
				}
				// ELSE: only column set to zero, pada unchanged = mode cyclic or Portion zero
			}
			start_jd = end_jd;
			oldlord = lord;

			r.push_back( dasa );

			// break condition
			if ( maxMahadasaCount > 0 )
			{
				if ( count >= maxMahadasaCount ) break;
			}
			else
			{
				if (  start_jd > getMaxEndJD( h )) break;
			}
			dasa = Dasa_ptr( new KalachakraDasa( root ));
		}
		if ( count > max_count )
		{
			errorlog << "KalachakraDasaExpert::getFirstLevel: loop level " << count << " too high";
		}
		return r;
	}

	/*****************************************************
	**
	**   KalachakraDasaExpert   ---   getNextLevel
	**
	******************************************************/
	virtual std::list<Dasa_ptr> getNextLevel( ContextProps_ptr, Dasa_ptr dasa ) override
	{
		assert( dasa );
		KalachakraDasa *kdasa = dynamic_cast<KalachakraDasa*>( dasa.get() );
		assert( kdasa );
		assert( kdasa->getLevel() >= DasaLevel::Mahadasa );
		assert( kdasa->getDasaId() == DasaId::Kalachakra );

		const KalachakraSubgroup subgroup = kdasa->getSubgroup();
		const int pada = kdasa->getPada();
		Sign lord = std::get<Sign>( kdasa->getDasaLord());

		// get correct column
		int column = 0;
		int count = 0;
		while ( lord != K_KALA[(int)subgroup][pada][column] )
		{
			if ( ++column > K_CHILDREN_COUNT ) column = 0;
			assert( ++count < 10 );
		}
		const double total_years = K_TOTALYEARS[(int)subgroup][pada];

		double start_jd = kdasa->getStartJD();
		double end_jd = kdasa->getEndJD();
		const double dasa_dur = end_jd - start_jd;

		int currentcol = column;
		Sign oldlord = Sign::None;

		std::list<std::shared_ptr<Dasa>> r( K_CHILDREN_COUNT );
		for( auto &a : r ) a = Dasa_ptr( new KalachakraDasa( dasa ));

		for( auto &child : r )
		{
			KalachakraDasa *antardasa = dynamic_cast<KalachakraDasa*>( child.get() );
			assert( antardasa );

			const Sign ad_lord = K_KALA[(int)subgroup][pada][currentcol];
			const double ad_dur = dasa_dur * (double)K_RASIYEARS[(int)ad_lord] / total_years;
			end_jd = start_jd + ad_dur;

			antardasa->lord = ad_lord;
			antardasa->start_jd = start_jd;
			antardasa->end_jd = end_jd;
			antardasa->pada = pada;
			antardasa->subgroup = subgroup;
			antardasa->gati = calculateGati( ad_lord, oldlord, isMarkatiField( subgroup, pada, currentcol ));

			start_jd = end_jd;
			oldlord = ad_lord;

			if ( ++currentcol >= K_CHILDREN_COUNT ) currentcol = 0;
		}
		return r;
	}

	/*****************************************************
	**
	**   KalachakraDasaExpert   ---   getKPLords
	**
	******************************************************/
	virtual KpData getKPLords( const double &len ) override
	{
		KpData kp;
		int mnak = (int)( len / NAKSHATRA_LEN );
		KalachakraSubgroup group = K_NAKSHATRA_GROUP[mnak];
		//const bool savya = ( (int)group <= 1 );

		int pada = (int)( len / PADA_LEN );
		pada %= 4;
		double portion = ( len - ( mnak * NAKSHATRA_LEN + pada * PADA_LEN ) ) / PADA_LEN;

		const int totalyears = K_TOTALYEARS[(int)group][pada];
		//double total_len = totalyears;

		double elapsed = portion * totalyears;
		int spalte = 0;

		// Lord of Dasa
		while ( elapsed - K_RASIYEARS[(int)K_KALA[(int)group][pada][spalte]] > 0 )
		{
			elapsed -= K_RASIYEARS[(int)K_KALA[(int)group][pada][spalte]];
			spalte++;
			if ( spalte > 8 ) { assert( 0 ); }
		}
		kp.lord = K_KALA[(int)group][pada][spalte]; // that's it

		double maha_dur = K_RASIYEARS[(int)K_KALA[(int)group][pada][spalte]];

		double aloop = K_RASIYEARS[(int)K_KALA[(int)group][pada][spalte]] * maha_dur / totalyears;
		int count = 0;
		while ( elapsed - aloop > 0 )
		{
			elapsed -= aloop;
			spalte++;
			if ( spalte > 8 ) spalte = 0;
			aloop = K_RASIYEARS[(int)K_KALA[(int)group][pada][spalte]] * maha_dur / totalyears;
			assert( ++count <= 8 );
		}

		assert( spalte >= 0 && spalte < K_CHILDREN_COUNT );
		kp.sublord = K_KALA[(int)group][pada][spalte];

		return kp;
	}

	virtual bool hasKpFeatures() const override { return true; }

	/*****************************************************
	**
	**   KalachakraDasaExpert   ---   getExtraLordName
	**
	******************************************************/
	virtual wxString getExtraLordName( Dasa_ptr dasa, const PlanetConfig&,
    const PlanetPropertyConfig &propertyCfg ) override
	{
		assert( dasa );
		KalachakraDasa *kdasa = dynamic_cast<KalachakraDasa*>( dasa.get() );

		const TextFormat format = propertyCfg.textFormat;
		wxString suffix;

		if ( kdasa )
		{
			if( kdasa->getGati() != KalachakraGati::Default )
			{
				suffix = enum_to_string( kdasa->getGati());
			}

			const Sign lordSign = std::get<Sign>( kdasa->getDasaLord());
			if ( lordSign == kdasa->jeeva ) suffix = tr( "Jeeva" );
			else if ( lordSign == kdasa->deha ) suffix = tr( "Deha" );
			if ( lordSign == kdasa->amsa ) suffix = tr( "Amsa" );

			if ( ! suffix.empty())
			{
				switch( format )
				{
					case TextFormat::Short:
						return suffix.substr( 0, 2 );
					break;
					case TextFormat::Medium:
						return suffix.substr( 0, 3 );
					break;
					case TextFormat::Long:
					default:
					break;
				}
			}
		}
		return suffix;
	}

	/*****************************************************
	**
	**   KalachakraDasaExpert   ---   describeDasa
	**
	******************************************************/
	virtual void describeDasa( MDocument_ptr doc, Horoscope_ptr horoscope, Dasa_ptr dasa ) override
	{
		Lang lang;
		const Viewpoint viewpoint = Viewpoint::Vedic;
		KalachakraDasa *kdasa = dynamic_cast<KalachakraDasa*>( dasa.get() );
		assert( kdasa );

#ifdef DEBUG_KALACHAKRADASA
		debuglog << "KalachakraDasaExpert::describeDasa level " << kdasa->getLevel();
#endif

		Table *table = new Table( 2 );
		Nakshatra27 nakshatra = ::getNakshatra27( horoscope->getVedicLongitude( PlanetId::Moon ));
		table->addRow( { tr( "Nakshatra" ),
			//lang.getNakshatra27Name( ::getNakshatra27( horoscope->getVedicLongitude( PlanetId::Moon ))) });
			lang.getNakshatra27Name( nakshatra ) });
		table->addRow( { tr( "Pada" ), std::format( "{}",  kdasa->pada + 1 ) });

		if ( kdasa->getLevel() == DasaLevel::Root )
		{
			table->addRow( { tr( "Pada Balance" ), getDurationYmd( getYearLength() * kdasa->padaBalance ) } );
			table->addRow( { tr( "First MD Balance" ), getDurationYmd( getYearLength() * kdasa->firstMdBalance ) });
		}

		table->addRow( { tr( "Group" ), enum_to_string( kdasa->getGroup()) });

		// subgroup in kdasa is reduced, so calculate again to avoid confusion
		table->addRow( { tr( "Subgroup" ), enum_to_string( (Nakshatra27)( (int)nakshatra % 6 )) });

		if ( kdasa->paramayus )
		{
			table->addRow( { tr( "Paramayus" ), std::format( "{} {}", kdasa->paramayus, tr( "years" )) });
		}

		if ( kdasa->amsa != Sign::None )
		{
			table->addRow( { tr( "Amsa" ), lang.getSignName( kdasa->amsa, viewpoint ) });
		}
		if ( kdasa->deha != Sign::None )
		{
			table->addRow( { tr( "Deha" ), lang.getSignName( kdasa->deha, viewpoint ) });
		}
		if ( kdasa->jeeva != Sign::None )
		{
		table->addRow( { tr( "Jeeva" ), lang.getSignName( kdasa->jeeva, viewpoint ) });
		}

		if ( kdasa->getLevel() > DasaLevel::Root )
		{
			DasaTool *tool = DasaTool::get();
			table->addRow( { tr( "Lords" ), tool->getDasaLordChain( dasa, false )} );

			// Start and end of Dasa
			DateFormatter *dh = DateFormatter::get();
			const bool isLocaltime = true;
			table->addRow( { tr( "Start Date" ), dh->formatDatetime( dasa->getStartJD(), isLocaltime ) } );

			table->addRow( { tr( "End Date" ), dh->formatDatetime( dasa->getEndJD(), isLocaltime ) } );
			table->addRow( { tr( "Duration" ),
					getDurationYmd( dasa->getEndJD() - std::max( horoscope->getJD(), dasa->getStartJD()))} );
		}  

		doc->addItem( table );
	}

	/*****************************************************
	**
	**   KalachakraDasaExpert   ---   getKPEventList
	**
	******************************************************/
	virtual std::list<KPEvent> getKPEventList( const double &startlen,
		const double &endlen, const double &startjd ) override
	{
		std::list<KPEvent> events;
		int i, j, pada, column, param_ayus, antarcolumn;
		int currentnak;
		KalachakraSubgroup group;
		DasaLord lord, sublord;
		double mahayears, antaryears;
		double len, mahalen, approxjd, inc_antarlen, dist;
		bool append;
		StopWatch sw;

		int index = 0;
		int startnak = (int)::getNakshatra27( startlen );
		for ( i = 0; i < 32; i++ )
		{
			currentnak = (startnak + i ) % 27;
			for ( pada = 0; pada < 4; pada++ )
			{
				group = K_NAKSHATRA_GROUP[currentnak];
				len = currentnak * NAKSHATRA_LEN + pada * PADA_LEN;
				param_ayus = K_TOTALYEARS[(int)group][pada];
				for ( column = 0; column < K_CHILDREN_COUNT; column++ )
				{
					index++;
					lord = K_KALA[(int)group][pada][column];
					mahayears = K_RASIYEARS[(int)std::get<Sign>( lord )];
					mahalen = mahayears * PADA_LEN / (double)param_ayus;
					inc_antarlen = 0;
					for ( j = 0; j < K_CHILDREN_COUNT; j++ )
					{
						append = true;
						antarcolumn = column + j;
						if ( antarcolumn >= K_CHILDREN_COUNT ) antarcolumn -= K_CHILDREN_COUNT;
						sublord = K_KALA[(int)group][pada][antarcolumn];
						antaryears = K_RASIYEARS[(int)std::get<Sign>( sublord )];
						approxjd = startjd + i + pada/4;
						if ( i == 0 ) // cut off at the beginning (antardasas before starting point)
						{
							dist = len + inc_antarlen - startlen;
							if ( dist < -300 ) dist += 360;  // correct if 360-deg shift
							if ( dist < 0 ) append = false;
						}
						if ( i >= 28 )  // cut off at the end (if event belongs to next time interval)
						{
							dist = len + inc_antarlen - endlen;
							if ( dist > 300 ) dist -= 360;
							if ( dist < -300 ) dist += 360;
							if ( dist > 0 ) append = false;     // could also be return ?
						}
						if ( append == true )
						{
							events.push_back({ lord, sublord, len + inc_antarlen, approxjd, index });
						}

						inc_antarlen += (double)antaryears * mahalen / (double)param_ayus;
					}
					len += mahalen;
				}

			}
		}
#ifdef DEBUG_KALACHAKRADASA
		debuglog << "FINISHED KalachakraDasaExpert::getKPEventList " << sw.getElapsedMillisec() << " millisec";
#endif
		return events;
	}

protected:

	virtual int getNbLords() const override { return K_CHILDREN_COUNT; }

	/*****************************************************
	**
	**   KalachakraDasaExpert   ---   createNewDasa
	**
	******************************************************/
	virtual Dasa_ptr createNewDasa( DasaExpert_ptr expert_ptr, Dasa_ptr parent, const DasaLevel &level ) override
	{
		return Dasa_ptr( new KalachakraDasa( expert_ptr, parent, level ));
	}

	/*****************************************************
	**
	**   KalachakraDasaExpert   ---   isMarkatiField
	**
	******************************************************/
	bool isMarkatiField( const KalachakraSubgroup &sg, const int &pada, const int &column )
	{
		assert_valid_enum<KalachakraSubgroup>( sg );
		assert( pada >= 0 && pada < 4 );
		assert( column >= 0 && column < K_CHILDREN_COUNT );

		const static int MARKATI_LOCATIONS[7][3] =
		{
			{ 0, 1, 7 },
			{ 1, 0, 4 },
			{ 1, 3, 1 },
			{ 2, 0, 8 },
			{ 2, 3, 5 },
			{ 3, 0, 8 },
			{ 3, 2, 2 }
		};
		for( int i = 0; i < 7; i++ )
		{
			if (
				MARKATI_LOCATIONS[i][(int)sg] == (int)sg
				&& MARKATI_LOCATIONS[i][pada] == pada
				&& MARKATI_LOCATIONS[i][column] == column
			)
			return true;
		}
		return false;
	}

	/*****************************************************
	**
	**   KalachakraDasaExpert   ---   calculateGati
	**
	******************************************************/
	KalachakraGati calculateGati( const DasaLord &ad_lord, const DasaLord &oldlord, const bool &markatiField )
	{
		KalachakraGati gati = KalachakraGati::Default;
		if ( std::holds_alternative<Sign>( oldlord ))
		{
			switch( std::get<Sign>( ad_lord ))
			{
				case Sign::Aries:
					if ( std::get<Sign>( oldlord ) == Sign::Sagittarius ) gati = KalachakraGati::Simhavalokan;
				break;
				case Sign::Gemini:
					if ( std::get<Sign>( oldlord ) == Sign::Leo ) gati = KalachakraGati::Mandooka;
				break;
				case Sign::Cancer:
					if ( std::get<Sign>( oldlord ) == Sign::Virgo ) gati = KalachakraGati::Mandooka;
					else if ( markatiField ) gati = KalachakraGati::Markati;
				break;
				case Sign::Leo:
					if ( std::get<Sign>( oldlord ) == Sign::Gemini ) gati = KalachakraGati::Mandooka;
					else if ( markatiField ) gati = KalachakraGati::Markati;
				break;
				case Sign::Virgo:
					if ( std::get<Sign>( oldlord ) == Sign::Cancer ) gati = KalachakraGati::Mandooka;
				break;
				case Sign::Scorpio:
					if ( std::get<Sign>( oldlord ) == Sign::Pisces ) gati = KalachakraGati::Simhavalokan;
				break;
				case Sign::Sagittarius:
					if ( std::get<Sign>( oldlord ) == Sign::Aries ) gati = KalachakraGati::Simhavalokan;
				break;
				case Sign::Pisces:
					if ( std::get<Sign>( oldlord ) == Sign::Scorpio ) gati = KalachakraGati::Simhavalokan;
				break;
				default:
				break;
			}
		}
#ifdef DEBUG_KALACHAKRADASA
		debuglog << "KalachakraDasaExpert::calculateGati adlord " << ad_lord << " oldlord " << oldlord << " gati " << gati ;
#endif
		return gati;
	}

	/*****************************************************
	**
	**   KalachakraDasaExpert   ---   getKPLordRecursive
	**
	******************************************************/
	DasaLord getKPLordRecursive( const int &group, const int &pada, int &spalte, double &elapsed, double &total_len )
	{
		assert( group >= 0 && group < 4 );
		assert( pada >= 0 && pada < 4 );
		assert( spalte >= 0 && group < K_CHILDREN_COUNT );

		const int totalyears = K_TOTALYEARS[group][pada];
		double maha_dur = K_RASIYEARS[(int)K_KALA[group][pada][spalte]];

		double aloop = K_RASIYEARS[(int)K_KALA[group][pada][spalte]] * maha_dur / totalyears;
		int count = 0;
		while ( elapsed - aloop > 0 )
		{
			elapsed -= aloop;
			spalte++;
			if ( spalte > 8 ) spalte = 0;
			aloop = K_RASIYEARS[(int)K_KALA[group][pada][spalte]] * maha_dur / totalyears;
			assert( ++count <= 8 );
		}

		total_len /= aloop;
		return K_KALA[group][pada][spalte];
	}

	inline static const int K_CHILDREN_COUNT = 9;

	inline static const int K_RASIYEARS[12] =
	{
		7, 16, 9, 21, 5, 9,
		16, 7, 10, 4, 4, 10
	};

	/*
	* Attention: there are only 4 subgroups instead of 6. Reason:
	* Order of signs etc is the same for Krittika=Aswini (Savya) and Mrigasira=Aridra in Apsavya
	* (but NOT Amsa)
	*/
	inline static const KalachakraSubgroup K_NAKSHATRA_GROUP[27] =
	{
		KalachakraSubgroup::Aswini,
		KalachakraSubgroup::Bharani,
		KalachakraSubgroup::Aswini,
		KalachakraSubgroup::Rohini,
		KalachakraSubgroup::Mrigasira,
		KalachakraSubgroup::Mrigasira,
		KalachakraSubgroup::Aswini,
		KalachakraSubgroup::Bharani,
		KalachakraSubgroup::Aswini,
		KalachakraSubgroup::Rohini,
		KalachakraSubgroup::Mrigasira,
		KalachakraSubgroup::Mrigasira,
		KalachakraSubgroup::Aswini,
		KalachakraSubgroup::Bharani,
		KalachakraSubgroup::Aswini,
		KalachakraSubgroup::Rohini,
		KalachakraSubgroup::Mrigasira,
		KalachakraSubgroup::Mrigasira,
		KalachakraSubgroup::Aswini,
		KalachakraSubgroup::Bharani,
		KalachakraSubgroup::Aswini,
		KalachakraSubgroup::Rohini,
		KalachakraSubgroup::Mrigasira,
		KalachakraSubgroup::Mrigasira,
		KalachakraSubgroup::Aswini,
		KalachakraSubgroup::Bharani,
		KalachakraSubgroup::Aswini
	};

	// Lord per group/pada/column
	inline const static Sign K_KALA[4][4][9] =
	{
		{
			{ /*?*/ Sign::Aries, Sign::Taurus, Sign::Gemini, Sign::Cancer, Sign::Leo,
				Sign::Virgo, Sign::Libra, Sign::Scorpio, Sign::Sagittarius },
			{ Sign::Capricorn, Sign::Aquarius, Sign::Pisces, /*S*/ Sign::Scorpio, Sign::Libra,
				Sign::Virgo, /*MAND*/ Sign::Cancer, /*MARK*/ Sign::Leo, /*MAND*/ Sign::Gemini },
			{ Sign::Taurus, Sign::Aries, Sign::Pisces, Sign::Aquarius, Sign::Capricorn,
				Sign::Sagittarius, /*S*/ Sign::Aries, Sign::Taurus, Sign::Gemini },
			{ Sign::Cancer, Sign::Leo, Sign::Virgo, Sign::Libra, Sign::Scorpio,
				Sign::Sagittarius, Sign::Capricorn, Sign::Aquarius, Sign::Pisces },
		},
		{
			{ /*?*/ Sign::Scorpio, Sign::Libra, Sign::Virgo, /*MAND*/ Sign::Cancer, /*MARK*/ Sign::Leo,
				/*MAND*/ Sign::Gemini, Sign::Taurus, Sign::Aries, Sign::Pisces },
			{ Sign::Aquarius, Sign::Capricorn, Sign::Sagittarius, /*S*/ Sign::Aries, Sign::Taurus,
				Sign::Gemini, Sign::Cancer, Sign::Leo, Sign::Virgo },
			{ Sign::Libra, Sign::Scorpio, Sign::Sagittarius, Sign::Capricorn, Sign::Aquarius,
				Sign::Pisces, /*S*/ Sign::Scorpio, Sign::Libra, Sign::Virgo },
			{ /*MAND*/ Sign::Cancer, /*MARK*/ Sign::Leo, /*MAND*/ Sign::Gemini, Sign::Taurus, Sign::Aries,
				Sign::Pisces, Sign::Aquarius, Sign::Capricorn, Sign::Sagittarius },
		},
		{
			{ /*?*/ Sign::Sagittarius, Sign::Capricorn, Sign::Aquarius, Sign::Pisces, Sign::Aries,
				Sign::Taurus, Sign::Gemini, /*MAND*/ Sign::Leo, /*MARK*/ Sign::Cancer },
			{ /*MAND*/ Sign::Virgo, Sign::Libra, Sign::Scorpio, /*S*/ Sign::Pisces, Sign::Aquarius,
				Sign::Capricorn, Sign::Sagittarius, Sign::Scorpio, Sign::Libra },
			{ Sign::Virgo, Sign::Leo, Sign::Cancer, Sign::Gemini, Sign::Taurus, Sign::Aries,
				/*S*/ Sign::Sagittarius, Sign::Capricorn, Sign::Aquarius },
			{ Sign::Pisces, Sign::Aries, Sign::Taurus, Sign::Gemini, /*MAND*/ Sign::Leo,
				/*MARK*/ Sign::Cancer, /*MAND*/ Sign::Virgo, Sign::Libra, Sign::Scorpio }
		},
		{
			{ /*?*/ Sign::Pisces, Sign::Aquarius, Sign::Capricorn, Sign::Sagittarius, Sign::Scorpio,
				Sign::Libra, Sign::Virgo, /*MAND*/ Sign::Leo, /*MARK*/ Sign::Cancer },
			{ /*MAND*/ Sign::Gemini, Sign::Taurus, Sign::Aries, /*S*/ Sign::Sagittarius, Sign::Capricorn,
				Sign::Aquarius, Sign::Pisces, Sign::Aries, Sign::Taurus },
			{ Sign::Gemini, /*MAND*/ Sign::Leo, /*MARK*/ Sign::Cancer, /*MAND*/ Sign::Virgo, Sign::Libra,
				Sign::Scorpio, /*S*/ Sign::Pisces, Sign::Aquarius, Sign::Capricorn },
			{ Sign::Sagittarius, Sign::Scorpio, Sign::Libra, Sign::Virgo, Sign::Leo, Sign::Cancer,
				Sign::Gemini, Sign::Taurus, Sign::Aries }
		}
	};

	// Paramayus per Group/Pada
	inline const static int K_TOTALYEARS[4][4] =
	{
		{ 100, 85, 83,  86 },
		{ 100, 85, 83,  86 },
		{  86, 83, 85, 100 },
		{  86, 83, 85, 100 }
	};

	// mapping from savya to apsavya amsa lord
	inline const static Sign K_APSAVYA_AMSA[12] =
	{
		Sign::Scorpio, Sign::Libra, Sign::Gemini, Sign::Leo, Sign::Cancer, Sign::Gemini,
		Sign::Taurus, Sign::Aries, Sign::Pisces, Sign::Aquarius, Sign::Capricorn, Sign::Sagittarius
	};

};

/*****************************************************
**
**   DasaExpertFactory   ---   create
**
******************************************************/
DasaExpert *DasaExpertFactory::getKalachakraDasaExpert() { return new KalachakraDasaExpert(); }

