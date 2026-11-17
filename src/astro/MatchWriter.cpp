/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/MatchWriter.cpp
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

#include "MatchWriter.h"
#include "astrobase.h"
#include "astroconfig.h"
#include "ContextProperties.h"
#include "DateFormatter.h"
#include "EventFinder.h"
#include "Formatter.h"
#include "Horoscope.h"
#include "logger.h"
#include "constants.h"
#include "mathbase.h"
#include "Table.h"
#include "Term.h"
#include "Uranian.h"
#include "UranianTextLoader.h"
#include "UranianHelper.h"

//#define DEBUG_MATCHWRITER

/*****************************************************
**
**   MatchFormatter   ---   formatMatch
**
******************************************************/
MString MatchFormatter::formatMatch( const Match &match, const Viewpoint &viewpoint, const UranianMatchFormat &format )
{
	MString s;

	assert( match.e1 && match.e2 );
	assert( match.e1->formula && match.e1->formula );
	assert( match.division > 0 );
	assert( match.residuum >= 0 && match.residuum < 360 );

#ifdef DEBUG_MATCHWRITER
	debuglog << "MatchFormatter::formatMatch division " << match.division
  	<< " orbis " << match.orbis
		<< " residuum " << match.residuum;
#endif

	s << *match.e1 << " = " << *match.e2;

	if ( match.division != 1 && ( format == UranianMatchFormat::Numbers || format == UranianMatchFormat::Signs ))
	{
		const bool greater180 = match.residuum > 180.0;
		const wxUniChar op = greater180 ? '+' : '-';
		s << " " << op << " ";

		if ( format == UranianMatchFormat::Numbers )
		{
			s << formatNumber( match.residuum );
		}
		else // symbols
		{
			switch( match.division )
			{
				case 2:
					s << MToken( Sign::Libra, viewpoint );
				break;
				case 3:
					s << MToken( Sign::Leo, viewpoint );
				break;
				case 4:
					s << MToken( Sign::Cancer, viewpoint );
				break;
				case 6:
					s << MToken( Sign::Gemini, viewpoint );
				break;
				case 8:
					if ( match.residuum < 90 )
					{
						s << MToken( Sign::Cancer, viewpoint ) << "/2";
					}
					else
					{
						s << MToken( Sign::Capricorn, viewpoint ) << "/2";
					}
				break;
				case 12:
					s << MToken( Sign::Taurus, viewpoint );
				break;
				case 24:
					s << MToken( Sign::Taurus, viewpoint ) << "/2";
				break;
				default: // use numbers only if division is not supported
					s << formatNumber( match.residuum );
				break;
			}
		}
	}

	return s;
}

/*****************************************************
**
**   MatchFormatter   ---   formatNumber
**
******************************************************/
wxString MatchFormatter::formatNumber( const double &x )
{
	const double y = x - .0000001;
	return Formatter::get()->formatDegrees( y, DegPrecision::FlexibleCutMinute );
}

/*****************************************************
**
**   MatchWriter   ---   writeMatches
**
******************************************************/
void MatchWriter::writeMatches( MDocument_ptr doc, const std::list<Match> &matches )
{
	DateFormatter *dtf= DateFormatter::get();
	Formatter *formatter = Formatter::get();
	UranianTextLoader *loader = UranianTextLoader::get();
	const MatchFilter filter = usection->filter;
	UranianHelper *uh = UranianHelper::get();
	//constexpr bool isLocaltime = false;
	MatchFormatter *mf = MatchFormatter::get();
	extern Astroconfig *astroconfig;

  const PlanetPropertyConfig &pcfg( astroconfig->getViewpointConfig( viewpoint ).planetProps );

	if ( matches.size() == 0 )
	{
		doc->addLine( tr( "No Matches" ));
		return;
	}

	const bool isFiltered = ! filter.isPassThrough();

	{
		wxString s = tr( "Matches" );
		if ( isFiltered )
		{
			s += " (" + tr( "filtered" ) + ")";
		}
		doc->addHeader2( s );
	}

	if ( matches.size() >= astroconfig->uranian.maxMatchCount )
	{
		doc->addParagraph( std::format( "Warning: result list incomplete, maximum number of matches ({}) exceeded.",
			astroconfig->uranian.maxMatchCount ));
	}

	const bool showorbis = usection->matchesShowOrbis;
	const bool showdate = usection->matchesShowDuedate;
	const bool showspeed = usection->matchesShowSpeed;
	const bool showresiduum = usection->matchesShowResiduum;
	const bool showrole = usection->matchesShowRole;

	uint nbcols = 3; // number, term, division
	if ( showorbis ) nbcols++;
	if ( showresiduum ) nbcols++;
	if ( showrole ) nbcols++;
	if ( showspeed ) nbcols++;
	if ( showdate ) nbcols++;

	const bool witteMode = true;
	bool showtext = usection->matchesShowText && loader->hasItemsForMode( witteMode );
	if ( showtext ) nbcols++;
	
	Table *table = new Table( nbcols, 1 );

	int col = 0;
	table->setHeader( col, "No." );
	table->format.col_alignment[col++] = Align::West;

	table->setHeader( col, tr( "Planets" ));
	table->format.col_break[col++] = false;

	table->setHeader( col++, tr( "Type" ));

	if ( showorbis ) table->setHeader( col++, tr( "Orbis" ));
	if ( showresiduum ) table->setHeader( col++, tr( "Residuum" ));
	if ( showspeed ) table->setHeader( col++, tr( "deg/day" ));
	if ( showdate ) table->setHeader( col++, tr( "Due Date" ));
	if ( showrole ) table->setHeader( col++, tr( "Role" ));

	if ( showtext )
	{
		table->setHeader( col,  tr( "Effect" ));
		table->format.col_break[col++] = true;
	}

	for( int count = 1; auto m : matches )
	{
		col = 0;
		TableRow row;
		row.addInteger( count++ );

		// Term
		assert( m.e1 && m.e2 );
		assert( m.e1->formula && m.e1->formula );

		row.addValue( mf->formatMatch( m, viewpoint, astroconfig->uranian.matchFormat ));

		row.addValue( uh->divisionDString( m.division ));

		if ( showorbis ) row.addValue( formatter->formatSignedDegreeString( m.orbis, pcfg.precision ));
		if ( showresiduum )
		{
			row.addValue( std::format( "{}", ( m.division == 1 ? 0 : m.residuum ) ));
		}
		if ( showspeed ) row.addValue( formatter->formatSignedDegreeString( m.lspeed, pcfg.precision ));

		//if ( usection->matchesShowDuedate )
		//if ( showdate ) row.addValue( dtf->formatUTDatetime( m.duedate ));
		if ( showdate )
		{
			if ( usection->matchesShowDuedateTime )
			{
				row.addValue( dtf->formatDatetime( m.duedate, props->isLocaltime ));
			}
			else
			{
				row.addValue( dtf->formatDate( m.duedate, props->isLocaltime ));
			}
		}

		if ( showrole ) row.addValue( uh->getRoleTagForMatch( m ));

		if ( showtext ) row.addValue( loader->getText4Match( m, witteMode ));

		assert( row.getSize() == nbcols );
		table->addRow( row );
	}

	if ( table->getNbRows() <= 1 )
	{
		doc->addParagraph( tr( "No events found" ));
		delete table;
	}
	else
	{
		if ( isFiltered )
		{
			doc->addParagraph( std::format( "{}/{} {}", matches.size(), table->getNbRows(), tr( "entries" ) ));
		}
		doc->addItem( table );
	}
}

