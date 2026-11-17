/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/DasaTool.cpp
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

#include "DasaTool.h"
#include "astrobase.h"
#include "astroconfig.h"
#include "ConfigListItems.h"
#include "ConfigLoaderImpls.h"
#include "ContextProperties.h"
#include "Dasa.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "FileConfig.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "constants.h"
#include "mathbase.h"
#include "MDocument.h"
#include "Table.h"
#include "Term.h"
#include "TermSolver.h"

#define DEBUG_DASATOOL

#ifdef DEBUG_DASATOOL
#include "enumserializer.h"
#include "Lang.h"
#endif

#include <exception>
#include <format>
#include <fstream>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   DasaTool   ---   getDasaName
**
******************************************************/
wxString DasaTool::getDasaName( const DasaId &dasa )
{
	// similar to enum_to_string but with spaces
	assert_valid_enum<DasaId>( dasa );
	static wxString k_dasa_name[enum_count<DasaId>()]  = 
	{
		tr( "Vimsottari" ),
		tr( "Yogini" ),
		tr( "Kalachakra" ),
		tr( "Ashtottari" ),
		tr( "Shodshottari" ),
		tr( "Dvadashottari" ),
		tr( "Panchottari" ),
		tr( "Shatabdika" ),
		tr( "Chaturashiti Sama" ),
		tr( "Dvisaptati Sama" ),
		tr( "Shat Trimsat Sama" ),
		tr( "Lagna Vimsottari" )
	};
	return k_dasa_name[(int)dasa];
}

/*****************************************************
**
**   DasaTool   ---   getDasaLevelName
**
******************************************************/
wxString DasaTool::getDasaLevelName( const DasaLevel &level )
{
	static wxString k_levelname[5] = {
		tr( "Mahadasa" ),
		tr( "Antardasa" ),
		tr( "Pratyantardasa" ),
		tr( "Sooksmantardasa" ),
		tr( "Pranadasa" ) };

	if ( level >= DasaLevel::TreeRoot && level <= DasaLevel::Pranadasa ) return enum_to_string( level );

	return std::format( "{} {}", tr( "Antardasa Level" ), ((int)level + 1 ));
}

/*****************************************************
**
**   DasaTool   ---   getDasaLordChain
**
******************************************************/
wxString DasaTool::getDasaLordChain( Dasa_ptr dasa, const bool addStartAndEndDate )
{
	if ( ! dasa  ) return "";

	//DasaExpert_ptr expert = DasaExpertFactory().getDasaExpert( dasa->getDasaId());
	Dasa *d = dasa.get();
	DasaExpert_ptr expert = d->getExpert();
	assert( expert );
	constexpr bool isLocaltime = false;

	std::list<MString> lords;
	while( d && d->getLevel() >= DasaLevel::Mahadasa )
	{
		assert( expert == d->getExpert());
		lords.push_front( expert->getDasaLordName( d ));
		d = d->getParent().get();
	}

	wxString t;
	int count = 0;
	for( const MString &s : lords )
	{
		if ( count ) t << "-";
		t << s.toWxString();
		count++;
	}

	if ( addStartAndEndDate )
	{
		DateFormatter *formatter = DateFormatter::get();
		t << " " << formatter->formatDate( dasa->getStartJD(), isLocaltime )
			<< " - "  << formatter->formatDate( dasa->getEndJD(), isLocaltime );
	}
	return t;
}

/*****************************************************
**
**   DasaTool   ---   calculateMahaAntarLords
**
******************************************************/
void DasaTool::calculateMahaAntarLords( Dasa_ptr d, DasaLord &lord, DasaLord &sublord )
{
	Dasa *dasa = d.get();
	lord = sublord = DasaLord();
	if ( dasa )
	{
		lord = dasa->getDasaLord();
		while( dasa->getParent() )
		{
			sublord = lord;
			dasa = dasa->getParent().get();
			lord = dasa->getDasaLord();
		}
	}
}

/*****************************************************
**
**   DasaTool   ---   calculateDasaInfluenceTime
**
******************************************************/
double DasaTool::calculateDasaInfluenceTime( ContextProps_ptr props, const DasaId &dasaId )
{
	assert( props );
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );
	DasaExpert_ptr expert( DasaExpertFactory().getDasaExpert( dasaId ));

	Dasa_ptr rootdasa = expert->createBlankRoot( expert );
	auto mahadasas = expert->getFirstLevel( props, rootdasa );
	assert( mahadasas.size() > 0 );

	Dasa_ptr mahadasa = mahadasas.front();
	const double dur = mahadasa->getEndJD() - horoscope->getJD();
	assert( dur >= 0 );

#ifdef DEBUG_DASATOOL
	debuglog << "DasaTool::calculateDasaInfluenceTime influence time is " << dur;
#endif	
	return dur;
}

/*****************************************************
**
**   DasaTool   ---   getDasaInfluenceTimeAsString
**
******************************************************/
wxString DasaTool::getDasaInfluenceTimeAsString( ContextProps_ptr props, const DasaId &dasaId )
{
	assert( props );
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );

	const double dur = calculateDasaInfluenceTime( props, dasaId );
	return getDurationYmd( dur );
}

/*****************************************************
**
**   DasaTool   ---   getNakshatraPortion
**
******************************************************/
std::pair<double, Nakshatra27> DasaTool::getNakshatraPortion(
	Horoscope_ptr horoscope,
	const PlanetId &ob,
	const Viewpoint &viewpoint )
{
	double portion = 0;
	const double mlen = horoscope->getLongitude( ob, viewpoint );
	const Nakshatra27 nak = (Nakshatra27)(int)( mlen / NAKSHATRA_LEN );
	const double nak_len = mlen - (int)nak * NAKSHATRA_LEN;

	if ( astroconfig->dasa.nakshatraPortionMode == NakshatraPortionMode::EntryTime )
	{
		const double len1 = (int)nak * NAKSHATRA_LEN;
		const double len2 = len1 + NAKSHATRA_LEN;

		MTerm term( TermId::Single );
		term.incarnate( ob );
		TermSolver solver( horoscope, viewpoint );
		const double jd1 = solver.findRadixMatchDate( &term, horoscope->getJD(), len1 );
		const double jd2 = solver.findRadixMatchDate( &term, horoscope->getJD(), len2 );
		portion = ( horoscope->getJD() - jd1 ) / ( jd2 - jd1 );

#ifdef DEBUG_DASATOOL
		debuglog << std::format( "DasaTool::getNakshatraPortion mlen {} len1 {} len2 {} jd {} jd1 {} jd2 {} portion {}",
			mlen, len1, len2,
			DateFormatter::get()->formatUTDatetime( horoscope->getJD()).ToStdString(),
			DateFormatter::get()->formatUTDatetime( jd1 ).ToStdString(),
			DateFormatter::get()->formatUTDatetime( jd2 ).ToStdString(),
			portion );
#endif

	}
	else // according to length
	{
		portion = nak_len / NAKSHATRA_LEN;
	}
#ifdef DEBUG_DASATOOL
	debuglog << "DasaTool::getNakshatraPortion nak " << (int)nak << " portion " << portion;
#endif	
	return std::make_pair( portion, nak );
}

/*****************************************************
**
**   DasaTool   ---   getPadaPortion
**
******************************************************/
double DasaTool::getPadaPortion(
	Horoscope_ptr horoscope,
	const PlanetId &ob,
	const Viewpoint &viewpoint )
{
	double portion = 0;
	const double mlen = horoscope->getLongitude( ob, viewpoint );
	const Nakshatra27 nak = (Nakshatra27)(int)( mlen / NAKSHATRA_LEN );
	const double nak_len = mlen - (int)nak * NAKSHATRA_LEN;
	const int pada = (int)(nak_len / PADA_LEN );
	const double p_len = mlen - ( (int)nak * NAKSHATRA_LEN + pada * PADA_LEN );

	if ( astroconfig->dasa.nakshatraPortionMode == NakshatraPortionMode::EntryTime )
	{
		const double len1 = (int)nak * NAKSHATRA_LEN + pada * PADA_LEN;
		const double len2 = len1 + PADA_LEN;

		MTerm term( TermId::Single );
		term.incarnate( ob );
		TermSolver solver( horoscope, viewpoint );
		const double jd1 = solver.findRadixMatchDate( &term, horoscope->getJD(), len1 );
		const double jd2 = solver.findRadixMatchDate( &term, horoscope->getJD(), len2 );
		portion = ( horoscope->getJD() - jd1 ) / ( jd2 - jd1 );

#ifdef DEBUG_DASATOOL
		debuglog << std::format( "DasaTool::getNakshatraPortion mlen {} len1 {} len2 {} jd {} jd1 {} jd2 {} portion {}",
			mlen, len1, len2,
			DateFormatter::get()->formatUTDatetime( horoscope->getJD()).ToStdString(),
			DateFormatter::get()->formatUTDatetime( jd1 ).ToStdString(),
			DateFormatter::get()->formatUTDatetime( jd2 ).ToStdString(),
			portion );
#endif

	}
	else // according to length
	{
		portion = p_len / PADA_LEN;
	}
#ifdef DEBUG_DASATOOL
	debuglog << "DasaTool::getPadaPortion nak " << (int)nak
		<< " pada " << pada
		<< " portion " << portion;
#endif	
	return portion;
}

/*****************************************************
**
**   DasaTool   ---   dasaConfigMatchesDasa
**
******************************************************/
bool DasaTool::dasaConfigMatchesDasa( const DasaConfig &cfg, Dasa *dasa ) const
{
	if ( ! dasa ) return false;

#ifdef DEBUG_DASATOOL
		debuglog << "DasaTool::dasaConfigMatchesDasa dasa id " << dasa->getDasaId() << " lord " << dasa->getDasaLord()
			<< " sublord " << dasa->getParent()->getDasaLord()
			<< " cfglord " << cfg.lord << " sublord " << cfg.sublord << " sslord " << cfg.subsublord;
#endif	

	if ( cfg.dasaId != dasa->getDasaId() ) return false;
	if ( dasa->getLevel() == DasaLevel::Mahadasa )
	{
		//return cfg.lord == dasa->getDasaLord() && std::get<PlanetId>( cfg.sublord ) == PlanetId::None;
		return (int)std::get<PlanetId>( cfg.lord ) == (int)std::get<PlanetId>( dasa->getDasaLord())
			&& std::get<PlanetId>( cfg.sublord ) == PlanetId::None;
	}
	else if ( dasa->getLevel() == DasaLevel::Antardasa )
	{
		Dasa_ptr parent = dasa->getParent();
		if ( parent )
		{
			//return cfg.lord == parent->getDasaLord() && cfg.sublord == dasa->getDasaLord();
			return (int)std::get<PlanetId>( cfg.lord ) == (int)std::get<PlanetId>( parent->getDasaLord())
				&& (int)std::get<PlanetId>( cfg.sublord ) == (int)std::get<PlanetId>( dasa->getDasaLord());
		}
	}
	return false;
}

/*****************************************************
**
**   DasaTool   ---   writeDasaEffect
**
******************************************************/
void DasaTool::writeDasaEffect( MDocument_ptr doc, Dasa_ptr dasa, const wxString sourcefilter  )
{
	wxString token;
	assert( dasa );

	for ( const auto &def : DasaConfigLoader::get()->getConfigs())
	{
		#ifdef DEBUG_DASATOOL
		debuglog << "DasaTool::writeDasaEffect def " << def.description << " lord " << dasa->getDasaLord()
			<< " deflord " << def.lord;
		#endif	
		if (( sourcefilter.empty() || sourcefilter == def.source ) && dasaConfigMatchesDasa( def, dasa.get()))
		{
			doc->addHeader2( def.source );

			// find paragraph "<p>" tokens and split lines
			wxString s = def.effect;
			int i = s.Find( "<p>" );
			while( i != wxNOT_FOUND )
			{
				//debuglog << "Found paragraph token at pos " << i << " substring " << s.Left( i );
				doc->addLine( s.Left( i ) );
				s = s.Mid( i + 3 );
				i = s.Find( "<p>" );
			}
			if ( s.Length() > 0 )
			{
				doc->addLine( s );
			}
			#ifdef DEBUG_DASATOOL
			debuglog << " DasaTool::writeDasaEffect source :  " << def.source << " effect: " << def.effect;
			#endif	
		}
	}
}

/*****************************************************
**
**   DasaTool   ---   writeDasaDocumentation
**
******************************************************/
void DasaTool::writeDasaDocumentation( MDocument_ptr doc, Horoscope_ptr horoscope, Dasa_ptr dasa, const wxString sourcefilter  )
{
	assert( dasa );
	DasaLord lord, sublord;

	DasaExpert_ptr expert = dasa->getExpert();
	if ( expert )
	{
		const DasaId dasaId = expert->getDasaId();
		assert_valid_enum<DasaId>( dasaId );
		expert->writeDasaHeader( doc, horoscope, dasa );

		if ( dasa )
		{
			calculateMahaAntarLords( dasa, lord, sublord );
			writeDasaEffect( doc, dasa, sourcefilter );
		}
	}
}

/*****************************************************
**
**   DasaTool   ---   writeReport
**
******************************************************/
void DasaTool::writeReport( MDocument_ptr doc, const DasaReportType &reportType, const DasaId &dasa,
	ContextProps_ptr props, const bool show_header, const TextFormat maxTextFormat )
{
	switch( reportType )
	{
		case DasaReportType::Short:
			writeShortReport( doc, dasa, props, show_header, maxTextFormat );
		break;
		case DasaReportType::Compact:
			writeCompactReport( doc, dasa, props, show_header, maxTextFormat );
		break;
		case DasaReportType::Comprehensive:
			writeComprehensiveReport( doc, dasa, props, show_header, maxTextFormat );
		break;
		case DasaReportType::None:
		default:
			errorlog << "invalid dasa report id " << (int)reportType;
		break;
	}
}

/*****************************************************
**
**   DasaTool   ---   writeShortReport
**
******************************************************/
void DasaTool::writeShortReport( MDocument_ptr doc, const DasaId &dasaId,
	ContextProps_ptr props, const bool show_header, const TextFormat /*maxTextFormat*/ )
{
	DateFormatter *formatter = DateFormatter::get();
	double startjd;
	DasaExpert_ptr expert( DasaExpertFactory().getDasaExpert( dasaId ));
	constexpr bool isLocaltime = false;

	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );

	if ( show_header )
	{
		doc->addHeader2( std::format( "{} {}", this->getDasaName( dasaId ), tr( "Dasa" )));
	}
	Dasa_ptr rootdasa = expert->createBlankRoot( expert );

	for ( auto &mahadasa : expert->getFirstLevel( props, rootdasa ))
	{
		startjd = std::max( mahadasa->getStartJD(), horoscope->getJD() );

		{
			MString ms1;
			ms1 << expert->getDasaLordName( mahadasa )
				<< ": " <<  formatter->formatDate( startjd, isLocaltime )
				<< " - " << formatter->formatDate( mahadasa->getEndJD(), isLocaltime );
			doc->addHeader2( ms1 );
		}

		auto children = expert->getNextLevel( props, mahadasa );
		if ( children.size() == 0 ) continue;
#ifdef DEBUG_DASATOOL
		debuglog << "DasaTool::writeShortReport mahadasa start " << mahadasa->getStartJD()
			<< " children " << children.size();
#endif	

		startjd = std::max( children.front()->getStartJD(), horoscope->getJD() );

		{
			MString ms;
			ms << " * " << formatter->formatDate( startjd, isLocaltime ) << " ";

			for ( auto &antardasa : children )
			{
				if ( antardasa->getEndJD() >= horoscope->getJD())
				{
					ms << expert->getDasaLordName( antardasa->getDasaLord())
						<<  " "
						<< formatter->formatDate( antardasa->getEndJD(), isLocaltime )
						<<  " ";
				}
			}
			doc->addParagraph( ms );
		}
	}
}

/*****************************************************
**
**   DasaTool   ---   writeCompactReport
**
******************************************************/
void DasaTool::writeCompactReport( MDocument_ptr doc, const DasaId &dasaId,
	ContextProps_ptr props, const bool show_header, const TextFormat maxTextFormat )
{
	DateFormatter *formatter = DateFormatter::get();
	int line;
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );

#ifdef DEBUG_DASATOOL
	debuglog << "DasaTool::writeCompactReport DasaId " << dasaId << " maxTextFormat " << maxTextFormat;
#endif	

	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Vedic );
	PlanetConfig planetCfg = astroconfig->getViewpointConfig( viewpoint ).planets;
	PlanetPropertyConfig planetPropCfg = astroconfig->getViewpointConfig( viewpoint ).planetProps;
	if ( maxTextFormat != TextFormat::Long )
	{
		trimTextFormat( planetCfg.textFormat, maxTextFormat );
		trimTextFormat( planetPropCfg.textFormat, maxTextFormat );
	}

	Dasa_ptr currentMD;
	Dasa_ptr currentAD;
	DasaExpert_ptr expert( DasaExpertFactory().getDasaExpert( dasaId ));
	constexpr bool isLocaltime = false;

	const double currentJD = MDate::getCurrentJD();

	const bool show_enddate = astroconfig->dasa.dasaDateDisplayMode == DasaShowDateMode::EndDate;

	Dasa_ptr rootdasa = expert->createBlankRoot( expert );
	const uint imax = expert->getNbLords();

	Table *table = new Table( 5, 2 * imax + 2 );
	if ( show_header )
	{
		table->setTitle( this->getDasaName( dasaId ) + " " + tr( "Dasa" ));
	}

	auto mahadasas = expert->getFirstLevel( props, rootdasa, 9 );
	auto mahadasa = mahadasas.begin();
	uint col = 0;
	while( col < 5 && mahadasa != mahadasas.end() )
	{
		table->format.col_alignment[col] = Align::West;

		{
			MString ms;
			ms << expert->getDasaLordName( (*mahadasa)->getDasaLord(), planetCfg, planetPropCfg ) << " " 
				<< formatter->formatDate(
					show_enddate ? (*mahadasa)->getEndJD() : std::max( (*mahadasa)->getStartJD(), horoscope->getJD()),
					isLocaltime );
			table->setHeader( col, ms );
		}

		line = 1;
		for( auto &antardasa : expert->getNextLevel( props, *mahadasa ))
		{
			// antardasas ending before birth are skipped
			if( antardasa->getEndJD() < horoscope->getJD() ) continue;

			// test if this is current mahadasa
			if( antardasa->getStartJD() < currentJD && antardasa->getEndJD() > currentJD )
			{
				currentMD = *mahadasa;
				currentAD = antardasa;
			}

			{
				MString ms;
				ms << expert->getDasaLordName( antardasa->getDasaLord(), planetCfg, planetPropCfg ) << " "
					<< formatter->formatDate( show_enddate ? antardasa->getEndJD()
						: std::max( antardasa->getStartJD(), horoscope->getJD()),
						isLocaltime );
				table->setEntry( col, line, ms );
			}
			line++;
		}
		mahadasa++;
		col++;
	}

	const int startline = line;
	col = 0;
	while( col < 5 && col < imax && mahadasa != mahadasas.end() )
	{
		line = startline;

		{
			MString ms;
			ms << expert->getDasaLordName( (*mahadasa)->getDasaLord(), planetCfg, planetPropCfg )
			 <<  " "
				<< formatter->formatDate(
					show_enddate ? (*mahadasa)->getEndJD() : std::max( (*mahadasa)->getStartJD(), horoscope->getJD()),
					isLocaltime );
			table->setHeaderEntry( col, line, ms );
		}

		line++;
		for( auto &antardasa : expert->getNextLevel( props, *mahadasa ))
		//for ( Dasa *antardasa : (*mahadasa)->children )
		{
			// antardasas ending before birth are skipped
			if( antardasa->getEndJD() < horoscope->getJD() ) continue;

			// test if this is current mahadasa
			if( antardasa->getStartJD() < currentJD && antardasa->getEndJD() > currentJD )
			{
				currentMD = *mahadasa;
				currentAD = antardasa;
			}

			{
				MString ms;
				ms << expert->getDasaLordName( antardasa->getDasaLord(), planetCfg, planetPropCfg )
					<< " "
					<< formatter->formatDate( show_enddate ? antardasa->getEndJD()
					: std::max( antardasa->getStartJD(), horoscope->getJD()),
					isLocaltime );
				table->setEntry( col, line, ms );
			}
			line++;
		}
		mahadasa++;
		col++;
	}

	// Current Pratyantardasa
	if( currentMD && currentAD )
	{
		line = startline;

		{
			MString ms;
			ms << tr( "Current" ) << ": " << expert->getDasaLordName( currentMD->getDasaLord(), planetCfg, planetPropCfg )
				<< "/" << expert->getDasaLordName( currentAD->getDasaLord(), planetCfg, planetPropCfg );
			table->setHeaderEntry( 4, line, ms );
		}

		line++;
		for( auto &pratyantardasa : expert->getNextLevel( props, currentAD ))
		{
			MString ms;
			ms << expert->getDasaLordName( pratyantardasa->getDasaLord(), planetCfg, planetPropCfg ) << " "
				<< formatter->formatDate(
					show_enddate ? pratyantardasa->getEndJD() : std::max( pratyantardasa->getStartJD(), horoscope->getJD()),
					isLocaltime );
			table->setEntry( 4, line, ms );
			line++;
		}
	}
	else warnlog << "DasaTool::writeCompact no current antardasa found";

	doc->addItem( table );
}

/*****************************************************
**
**   DasaTool   ---   writeComprehensiveReport
**
******************************************************/
void DasaTool::writeComprehensiveReport( MDocument_ptr doc, const DasaId &dasa,
	ContextProps_ptr props, const bool /*show_header*/, const TextFormat /*maxTextFormat*/ )
{
	DateFormatter *formatter = DateFormatter::get();
	double startjd;
	MString t;
	DasaExpert_ptr expert( DasaExpertFactory().getDasaExpert( dasa ));
	constexpr bool isLocaltime = false;

	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );

	Dasa_ptr rootdasa = expert->createBlankRoot( expert );
	auto firstLevel = expert->getFirstLevel( props, rootdasa );

	expert->writeDasaHeader( doc, horoscope, rootdasa );

	uint col = 0;
	//for ( auto &mahadasa : expert->getFirstLevel( props, rootdasa ))
	for ( auto &mahadasa : firstLevel )
	{
		{
			MString ms;
			ms << std::format( "{}", col + 1 ) << ". " << tr( "Mahadasa" ) << " "
				<< expert->getDasaLordName( mahadasa->getDasaLord());
			doc->addHeader2( ms );
		}

		startjd = std::max( mahadasa->getStartJD(), horoscope->getJD() );
		{
			doc->addLine( std::format( "{}: {}", tr( "Start Date" ), formatter->formatDate( startjd, isLocaltime )));
		}

		{
			doc->addLine( std::format( "{}: {}", tr( "End Date" ), formatter->formatDate( mahadasa->getEndJD(), isLocaltime )));
		}

		{
			doc->addLine( std::format( "{}: {}", tr( "Duration" ),
				getDurationYmd( mahadasa->getEndJD() - std::max( horoscope->getJD(), mahadasa->getStartJD() ))));
		}

		auto children = expert->getNextLevel( props, mahadasa );
		if ( children.size() == 0 ) continue;

		doc->addHeader2( tr( "Antardasas" ) );

		// Get the number of lines
		int lines = 1;
		for ( auto &antardasa : children )
		{
			if ( antardasa->getEndJD() >= horoscope->getJD()) lines++;
		}

		Table *table = new Table( 4, lines );
		table->setHeader( 0, tr( "Lord" ));
		table->setHeader( 1,  tr( "Start Date" ));
		table->setHeader( 2,  tr( "End Date" ));
		table->setHeader( 3,  tr( "Duration" ));

		int line = 1;
		for ( auto &antardasa : children )
		{
			if ( antardasa->getEndJD() >= horoscope->getJD())
			{
				table->setEntry( 0, line, expert->getDasaLordName( antardasa->getDasaLord()));

				startjd = std::max( antardasa->getStartJD(), horoscope->getJD() );
				table->setEntry( 1, line, formatter->formatDate( startjd, isLocaltime ));

				table->setEntry( 2, line, formatter->formatDate( antardasa->getEndJD(), isLocaltime ));

				table->setEntry( 3, line,
					getTimeSpanFormatted( antardasa->getEndJD() - antardasa->getStartJD(), true, TextFormat::Medium ));
				line++;
			}
		}
		doc->addItem( table );
		col++;
	}
}

