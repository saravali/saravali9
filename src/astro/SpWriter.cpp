/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/SpWriter.cpp
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

#include "SpWriter.h"

#include "astroconfig.h"
#include "ContextProperties.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "Formulary.h"
#include "Horoscope.h"
#include "Lang.h"
#include "mathbase.h"
#include "Table.h"
#include "Term.h"
#include "TermList.h"
#include "logger.h"
#include "Uranian.h"
#include "UranianTextLoader.h"
#include "UranianHelper.h"
#include "Viewpoint.h"

#include <format>

extern Astroconfig *astroconfig;

//#define DEBUG_SPWRITER

bool compare_spitems( const std::tuple<const MTerm&, double, double> &e1,
	const std::tuple<const MTerm&, double, double> &e2 )
{
	return std::get<1>( e1 ) < std::get<1>( e2 );
}

/*****************************************************
**
**   SensitivePointWriter   ---   writeList
**
******************************************************/
void SensitivePointWriter::writeList( MDocument_ptr doc, const TermId &termId, 
	const bool &sortByLongitude, const PlanetId leadingTripleObject )
{
	Formulary *f = Formulary::get();
	MTerm *term = new MTerm( termId );
	UranianHelper *helper = UranianHelper::get();
	UranianTextLoader *loader = UranianTextLoader::get();

	const bool witteMode = true;
	bool showText = loader->hasItemsForMode( witteMode );
	const bool isD1 = usection->matchDivision == 1;

	int nbCols = 3;
	if ( ! isD1 ) ++nbCols;
	if ( showText ) ++nbCols;

#ifdef DEBUG_SPWRITER
	debuglog << "SensitivePointWriter::writeList termId " << termId;
#endif

	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Uranian );
	Formatter *formatter = Formatter::get();

	doc->addHeader2( f->getTermName( termId ));
	{
		MString s;
		s << tr( "Formula" ) << ": " << *term;
		doc->addParagraph( s );
	}

	Table *table = new Table( nbCols, 1 );

	{
		int col = 0;
		table->setHeader( col++, "No." );
		table->setHeader( col++, tr( "Term" ));
		table->setHeader( col++, helper->divisionDString( usection->matchDivision ));
		if ( ! isD1 ) table->setHeader( col++, "D-1" );
		if ( showText )
		{
			table->setHeader( col, tr( "Text" ));
			table->format.col_break[col++] = true;
		}
	}

	const auto obs = props->getPlanets( viewpoint );

	// list of triples: name, D-n longitude (sort criterion) and D-1 longitude
	std::list<std::tuple<MTerm,double,double>> items;

	if ( MTerm::isTripleTerm( termId ))
	{
		TermId remainingTermId = TermId::None;
		switch( termId )
		{
			case TermId::Triple:
				remainingTermId = TermId::Difference;
			break;
			case TermId::TripleSum:
				remainingTermId = TermId::Sum;
			break;
			default:
				assert( false );
			break;
		}
		assert( remainingTermId != TermId::None );
		MTerm term( termId );
		for( auto item : TermList( obs, remainingTermId ))
		{
			if ( termId != TermId::Triple || leadingTripleObject != item.getSecondPlanet())
			{
#ifdef DEBUG_SPWRITER
				debuglog << "SensitivePointWriter::writeList USE term " << item
					<< " termId " << termId
					<< " leadingTripleObject " << leadingTripleObject
					<< " planet 1 " << item.getFirstPlanet()
					<< " planet 2 " << item.getSecondPlanet();
#endif
				term.incarnate( leadingTripleObject, item.getFirstPlanet(), item.getSecondPlanet());
				if ( usection->filter.isPassThrough() || term.applyFilter( usection->filter ))
				{
					const double l = term.evaluate( horoscope, viewpoint );
					items.push_back( std::make_tuple( term, a_red( l, 360.0 / usection->matchDivision ), l ));
				}

			}
			else
			{
#ifdef DEBUG_SPWRITER
				debuglog << "SensitivePointWriter::writeList skip term "
					<< item << " for leadingTripleObject " << (int)leadingTripleObject
					<< " termId " << (int)termId;
#endif
			}
		}

	}
	else // not a triple term
	{
		double l = 0.0;
		for( auto term : TermList( obs, termId ))
		{
#ifdef DEBUG_SPWRITER
			debuglog << "SensitivePointWriter::writeList term " << term << " pass through "
				<< usection->filter.isPassThrough() << " apply " << term.applyFilter( usection->filter );
#endif
			if ( usection->filter.isPassThrough() || term.applyFilter( usection->filter ))
			{
				l = term.evaluate( horoscope, viewpoint );
				items.push_back( std::make_tuple( term, a_red( l, 360.0 / usection->matchDivision ), l ));
			}
		}
	}
	if ( sortByLongitude ) items.sort( compare_spitems );

	for( int count = 1; auto item : items )
	{
		TableRow row;
		row.addInteger( count++ );

		MString ms;
		ms << std::get<0>( item );
		row.addValue( ms );
		row.addValue( formatter->formatZodiacalLongitude( viewpoint, std::get<1>( item ), MovingDirection::None,
      usection->matchDivision ));

		if ( ! isD1 ) row.addValue( formatter->formatZodiacalLongitude( viewpoint, std::get<2>( item )));

		if ( showText ) row.addValue( loader->getText4Term( std::get<0>( item ), witteMode ));
		table->addRow( row );
	}

	if ( table->getNbRows() <= 1 )
	{
		doc->addParagraph( tr( "No events found" ));
		delete table;
	}
	else doc->addItem( table );
	delete term;
}

/*****************************************************
**
**   SensitivePointWriter   ---   writeHeader
**
******************************************************/
void SensitivePointWriter::writeHeader( MDocument_ptr doc, const TermId &termId, const PlanetId leadingTripleObject )
{
	Formulary *formulary = Formulary::get();
	Lang lang;
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Uranian );
	wxString s;

	//doc->addHeader2( formulary->getTermName( termId ));
	doc->addHeader2( std::format( "{} (D-{})", formulary->getTermName( termId ), usection->matchDivision ));

	if ( MTerm::isTripleTerm( termId ))
	{
		s << tr( "Formula" ) << ": "
			<< lang.getPlanetName( leadingTripleObject, viewpoint, TextFormat::Long );
		
		switch ( termId )
		{
			case TermId::Triple:
				s << " + " << tr( "line" ) << " - " << tr( "column" );
			break;
			case TermId::TripleSum:
				s << " + " << tr( "line" ) << " + " << tr( "column" );
			break;
			default:
				s << " error " << enum_to_string( termId );
			break;
		}
		doc->addParagraph( s );
	}
	else if ( ! MTerm::isSymmetric( termId ))
	{
		s << tr( "Formula" ) << ": ";
		switch ( termId )
		{
			case TermId::Difference:
				s << tr( "line" ) << " - " << tr( "column" );
			break;
			case TermId::ReflectionPoint:
				s << tr( "line" ) << " -> " << tr( "column" ) << " = 2 * " << tr( "line" ) << " - " << tr( "column" );
			break;
			default:
				s << " error " << enum_to_string( termId );
			break;
		}
		doc->addParagraph( s );
	}
}

/*****************************************************
**
**   SensitivePointWriter   ---   writeMatrix
**
******************************************************/
void SensitivePointWriter::writeMatrix( MDocument_ptr doc, const TermId &termId, const PlanetId leadingTripleObject )
{
	assert( horoscope );
	assert( doc );
	//assert_valid_enum<TermId>( termId );
	if ( ! is_valid_enum<TermId>( termId ))
	{
		doc->addParagraph( std::format( "Error: ivalid term id {}", enum_to_string( termId )));
		return;
	}
	MTerm term( termId );

	if ( term.isSingleTerm() ) writeSingleTermMatrix( doc, term );
	else writeDoubleTermMatrix( doc, term, leadingTripleObject );
}

/*****************************************************
**
**   SensitivePointWriter   ---   writeSingleTermMatrix
**
******************************************************/
void SensitivePointWriter::writeSingleTermMatrix( MDocument_ptr doc, MTerm &term )
{
	assert( horoscope );
	assert( doc );
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Uranian );

	assert( term.isSingleTerm());
	const bool isAntiTerm = term.getTermId() == TermId::AntiSingle;
	UranianHelper *uh = UranianHelper::get();
	const std::vector<int> divisions = uh->getAllDivisions();
	Formatter *formatter = Formatter::get();

	writeHeader( doc, term.getTermId() );

	Table *table = new Table( divisions.size() + 1, 1 );
	table->setHeader( 0, tr( "Name" ));
	for ( uint i = 0; i < divisions.size(); i++ )
	{
		table->setHeader( i+1, std::format( "D-{}", divisions[i] ));
	}

	for ( const auto p : props->getPlanets( viewpoint ))
	{
		term.incarnate( p );
		if ( usection->filter.isPassThrough() || term.applyFilter( usection->filter ))
		{
			TableRow row;
			row.addValue( MToken( p, viewpoint ));
			for ( const auto d : divisions )
			{
				MovingDirection dir = MovingDirection::None;
				if ( ! isAntiTerm && d == 1 && horoscope->getDirection( p, viewpoint ) == MovingDirection::Retrograde )
				{
					dir = MovingDirection::Retrograde;
				}
				row.addValue( 
					formatter->formatZodiacalLongitude(
						viewpoint,
						term.evaluate( horoscope, viewpoint ),
						dir,
						d
					)
				);
			}
			table->addRow( row );
		}
	}
	doc->addItem( table );
}

/*****************************************************
**
**   SensitivePointWriter   ---   writeDoubleTermMatrix
**
******************************************************/
void SensitivePointWriter::writeDoubleTermMatrix( MDocument_ptr doc, MTerm &term, const PlanetId leadingTripleObject )
{
	assert( horoscope );
	assert( doc );
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Uranian );

#ifdef DEBUG_SPWRITER
	debuglog << "SensitivePointWriter::writeDoubleTermMatrix termId " << (int)term.getTermId()
		<< " filter pass through " << std::boolalpha << usection->filter.isPassThrough();
#endif

	double value;
	Formatter *formatter = Formatter::get();

	PlanetConfig planetCfg( astroconfig->getViewpointConfig( viewpoint ).planets );
	PlanetPropertyConfig propertyCfg( astroconfig->getViewpointConfig( viewpoint ).planetProps );

	if ( astroconfig->uranian.doTrimTableItems )
	{
		trimTextFormat( planetCfg.textFormat, TextFormat::Medium );
		trimTextFormat( propertyCfg.textFormat, TextFormat::Medium );
		propertyCfg.precision = DegPrecision::Minute;
	}

	writeHeader( doc, term.getTermId(), leadingTripleObject );

	std::vector<PlanetId> planets;
	for ( const auto p : props->getPlanets( viewpoint ))
	{
		debuglog << "PLANET " << p;
		term.incarnate( p );
		if ( usection->filter.isPassThrough() || term.applyFilter( usection->filter ))
		{
			planets.push_back( p );
		}
	}

	const int pmax = planets.size();
	const bool isTriple = term.getVariableCount() == 3;

	Table *table = new Table( pmax + 2, pmax + 2 );
	table->setHeader( 0, "" );
	table->setHeader( pmax + 1, "" );
	table->setHeaderEntry( 0, pmax + 1,  "" );
	table->setHeaderEntry( pmax + 1, pmax + 1, "" );
	for ( int i = 0; i < pmax; i++ )
	{
		table->setHeader( i+1, MToken( planets[i], planetCfg ));
		table->setHeaderEntry( i + 1, pmax + 1, MToken( planets[i], planetCfg ));

		table->setHeaderEntry( 0, i + 1, MToken( planets[i], planetCfg ));
		table->setHeaderEntry( pmax + 1, i + 1, MToken( planets[i], planetCfg ));
	}

	const bool writesymmetric = term.isSymmetric();

	for ( int i = 0; i < pmax; i++ )
	{
		for ( int j = 0; j < pmax; j++ )
		{
			//if ( i > j && writesymmetric ) continue;
			if ( j > i && writesymmetric ) continue;

			if ( isTriple )
				term.incarnate( leadingTripleObject, planets[i], planets[j] );
			else
				term.incarnate( planets[i], planets[j] );

#ifdef DEBUG_SPWRITER
			if ( ! usection->filter.isPassThrough() && ! term.applyFilter( usection->filter )) continue;
			debuglog << "SensitivePointWriter::writeDoubleTermMatrix term " << term
				<< " filter apply " << term.applyFilter( usection->filter );
#endif

			value = term.evaluate( horoscope, viewpoint );
			table->setEntry( j + 1, i + 1,
				formatter->formatZodiacalLongitude( propertyCfg, value, MovingDirection::None, usection->matchDivision ));
		}
	}
	doc->addItem( table );
}

