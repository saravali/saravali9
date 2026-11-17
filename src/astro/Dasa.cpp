/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Dasa.cpp
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


#include "Dasa.h"

#include "astrobase.h"
#include "astroconfig.h"
#include "DasaTool.h"
#include "DateFormatter.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "constants.h"
#include "MDocument.h"
#include "SymbolProvider.h"
#include "Table.h"

extern Astroconfig *astroconfig;

/*****************************************************
**
**   Dasa   ---   Constructor
**
******************************************************/
Dasa::Dasa( DasaExpert_ptr expert, Dasa_ptr parent, const DasaLevel level, const DasaLord lord )
	: expert( expert ),
	parent( parent ),
	lord( lord ),
	level( level )
{
#ifdef DEBUG_SHARED_PTR_LIFECYLCE
	++nb_created;
#endif
}

/*****************************************************
**
**   Dasa   ---   Constructor
**
******************************************************/
Dasa::Dasa( Dasa_ptr parent )
	: expert( parent->getExpert() ),
	parent( parent ),
	level( parent->getLevel() + 1 )
{
#ifdef DEBUG_SHARED_PTR_LIFECYLCE
	++nb_created;
#endif
}

/*****************************************************
**
**   Dasa   ---   Constructor
**
******************************************************/
Dasa::Dasa( Dasa_ptr parent, const DasaLord &lord, const double &startjd, const double &endjd )
	: parent( parent ),
	lord( lord ),
	start_jd( startjd ),
	end_jd( endjd ),
	level( parent ? parent->getLevel() +  1 : DasaLevel::Mahadasa )
{
	assert( parent );
	expert = parent->getExpert();
#ifdef DEBUG_SHARED_PTR_LIFECYLCE
	++nb_created;
#endif
}

/*****************************************************
**
**   Dasa   ---   Destructor
**
******************************************************/
Dasa::~Dasa()
{
#ifdef DEBUG_SHARED_PTR_LIFECYLCE
	++nb_deleted;
	debuglog << "Destructor class Dasa: " << nb_created << " objects created, " << nb_deleted << " objects deleted";
#endif
}

/*****************************************************
**
**   Dasa   ---   getDasaId
**
******************************************************/
DasaId Dasa::getDasaId() const
{
	if( expert ) return expert->getDasaId();
	else return DasaId::None;
}

/*****************************************************
**
**   KpData   ---   operator==
**
******************************************************/
bool operator==( const KpData &kp1, const KpData &kp2 )
{
	if ( kp1.lord != kp2.lord ) return false;
	if ( kp1.sublord != kp2.sublord ) return false;
	return ( kp1.subsublord == kp2.subsublord );
}

/*****************************************************
**
**   DasaExpert   ---   Destructor
**
******************************************************/
DasaExpert::~DasaExpert()
{
	debuglog << "Destructor ~DasaExpert()";
}

/*****************************************************
**
**   DasaExpert   ---   getMaxEndJD
**
******************************************************/
double DasaExpert::getMaxEndJD( Horoscope_ptr h )
{
	return h->getJD() + astroconfig->dasa.startOfLastMahadasa * astroconfig->dasa.yearLength.getValue();
}

/*****************************************************
**
**   DasaExpert   ---   getMaxEndJD
**
******************************************************/
double DasaExpert::getYearLength()
{
	return astroconfig->dasa.yearLength.getValue();
}

/*****************************************************
**
**   DasaExpert   ---   createBlankRoot
**
******************************************************/
Dasa_ptr DasaExpert::createBlankRoot( DasaExpert_ptr expert_ptr, Dasa_ptr parent )
{
	assert( expert_ptr );
	assert( expert_ptr.get() == this );
	assert( ! parent || ! parent->getExpert() || parent->getExpert() == expert_ptr );
	return createNewDasa( expert_ptr, parent, DasaLevel::Root );
}

/*****************************************************
**
**   DasaExpert   ---   createNewDasa
**
******************************************************/
Dasa_ptr DasaExpert::createNewDasa( DasaExpert_ptr expert_ptr, Dasa_ptr parent, const DasaLevel &level )
{
	return Dasa_ptr( new Dasa( expert_ptr, parent, level ));
}

/*****************************************************
**
**   DasaExpert   ---   getDasaLordName
**
******************************************************/
MString DasaExpert::getDasaLordName( Dasa *dasa )
{
	const ViewpointConfig &cfg = astroconfig->getViewpointConfig( Viewpoint::Vedic );
	return getDasaLordName( dasa, cfg.planets, cfg.planetProps );
}

MString DasaExpert::getDasaLordName( Dasa *dasa, const PlanetConfig &planetCfg,
	const PlanetPropertyConfig &propertyCfg )
{
	assert( dasa );
	return getDasaLordName( dasa->getDasaLord(), planetCfg, propertyCfg );
}

MString DasaExpert::getDasaLordName( const DasaLord &lord )
{
	const ViewpointConfig &cfg = astroconfig->getViewpointConfig( Viewpoint::Vedic );
	return DasaExpert::getDasaLordName( lord, cfg.planets, cfg.planetProps );
}

MString DasaExpert::getDasaLordName( const DasaLord &lord, const PlanetConfig &planetCfg,
	const PlanetPropertyConfig &propertyCfg )
{
	return( std::holds_alternative<PlanetId>( lord ) ?
		MToken( std::get<PlanetId>( lord ), planetCfg ) : MToken( std::get<Sign>( lord ), propertyCfg ));
}

/*****************************************************
**
**   DasaExpert   ---   getDasaDescription
**
******************************************************/
MString DasaExpert::getDasaDescription( Dasa_ptr dasa, const PlanetConfig &planetCfg,
  const PlanetPropertyConfig &propertyCfg )
{
	assert( dasa );
	MString s = getDasaLordName( dasa, planetCfg, propertyCfg );
	wxString extraLord = getExtraLordName( dasa, planetCfg, propertyCfg );
	if ( ! extraLord.empty())
	{
		s << "/" << extraLord;
	}
	return s;
}

/*****************************************************
**
**   DasaExpert   ---   getBirthMahaDasaLord
**
******************************************************/
DasaLord DasaExpert::getBirthMahaDasaLord( DasaExpert_ptr pprt, ContextProps_ptr props  )
{
	Dasa_ptr rootdasa = createBlankRoot( pprt, nullptr );
	auto mahadasas = getFirstLevel( props, rootdasa );
	assert( mahadasas.size() > 0 );
	const DasaLord lord = mahadasas.front()->getDasaLord();

	return lord;
}

/*****************************************************
**
**   DasaExpert   ---   writeDasaHeader
**
******************************************************/
void DasaExpert::writeDasaHeader( MDocument_ptr doc, Horoscope_ptr horoscope, Dasa_ptr dasa )
{
	assert( doc );
	DasaTool *tool = DasaTool::get();

	{
		wxString s;
		s = tool->getDasaName( dasa->getDasaId() );
		if ( dasa && (int)dasa->getLevel() >= 0 )
		{
			s << " (" <<  tool->getDasaLevelName( dasa->getLevel() ) << ")";
		}
		doc->addHeader2( s );
	}
	describeDasa( doc, horoscope, dasa );
}

/*****************************************************
**
**   DasaExpert   ---   describeDasa
**
******************************************************/
void DasaExpert::describeDasa( MDocument_ptr doc, Horoscope_ptr horoscope, Dasa_ptr dasa )
{
	assert( doc );
	Lang lang;

	Table *table = new Table( 2 );
	table->addRow( { tr( "Nakshatra" ),
		lang.getNakshatra27Name( ::getNakshatra27( horoscope->getVedicLongitude( PlanetId::Moon ))) });


	if ( dasa->getLevel() == DasaLevel::Root )
	{
		table->addRow( { tr( "Paramayus" ), getDurationYmd( astroconfig->dasa.yearLength.getValue() * dasa->getParamayus() ) } );
		table->addRow( { tr( "Balance at birth" ), getDurationYmd( dasa->getBalance() ) } );
	}
	else
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
**   DasaExpertFactory   ---   getDasaExpert
**
******************************************************/
DasaExpert *DasaExpertFactory::getDasaExpert( const DasaId &id )
{
	// impls are in the respective *.cpp source files.
	switch( id )
	{
		case DasaId::Kalachakra:
			return getKalachakraDasaExpert();
		break;
		case DasaId::Ashtottari:
			return getAshtottariDasaExpert();
		break;
		default:
			return getGenericDasaExpert( id );
		break;
	}
}

