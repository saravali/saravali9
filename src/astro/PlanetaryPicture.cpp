/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/PlanetaryPicture.cpp
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

#include "PlanetaryPicture.h"

#include "astroconfig.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "Horoscope.h"
#include "Lang.h"
#include "logger.h"
#include "constants.h"
#include "Table.h"
#include "Uranian.h"

extern Astroconfig *astroconfig;

//#define DEBUG_PP

/*****************************************************
**
**   PlanetaryPicture   ---   calculateScore
**
******************************************************/
void PlanetaryPicture::calculateScore( const double &maxorbis )
{
	score = 0;
	for( auto &elem : elements )
	{
		score += fabs( maxorbis ) - fabs( elem.orbis );
	}
	//score = elements.size();
#ifdef DEBUG_PP
	debuglog << "PlanetaryPicture::calculateScore score " << score;
#endif
}

/*****************************************************
**
**   PlanetaryPictureExpert   ---   createPictures
**
******************************************************/
bool PlanetaryPictureExpert::createPictures()
{
#ifdef DEBUG_PP
	debuglog << "PlanetaryPictureExpert::createPictures";
#endif

	pictures.clear();
	totalMatchCount = 0;

	auto roles = props->getActiveRoles();
	if ( roles.size() == 0 )
	{
		errorlog << "PlanetaryPictureExpert::createPictures no active role in context, return";
		return true;
	}
#ifdef DEBUG_PP
	else
	{
		for( const auto &role : roles )
		{
			debuglog << "PlanetaryPictureExpert::createPictures role " << role;
		}
	}
#endif

	Horoscope_ptr h1;
	Horoscope_ptr h2;

	if ( props->getDataContext() == DataContext::Partner )
	{
		h1 = props->getHoroscope( HoroscopeRole::Partner1 );
		h2 = props->getHoroscope( HoroscopeRole::Partner2 );
		if ( !h1 || !h2 || h1 == h2 ) return true;
		createPicturesForPair( h2, h1, HoroscopeRole::Partner1 );
		createPicturesForPair( h1, h2, HoroscopeRole::Partner2 );
	}
	else
	{
		h1 = props->getRadixHoroscope();
		assert( h1 );
		if ( roles.size() > 1 ) h2 = props->getHoroscope( roles[1] );
		else h2 = h1;
		assert( h2 );
		createPicturesForPair( h1, h2, h2->getRole());
	}

	const Viewpoint viewpoint = getViewpoint();
	const Planets obs = props->getPlanets( viewpoint );

	// sort Pp list
	const EventSortOrder myOrder = usection->sortOrder;
	pictures.sort( [myOrder, obs] ( const PlanetaryPicture &p1, const PlanetaryPicture &p2 )
	{
		switch( myOrder )
		{
			case EventSortOrder::Longitude:
				return p1.leadingRole <= p2.leadingRole && p1.longitude < p2.longitude;
			break;
			case EventSortOrder::Score:
				return p1.leadingRole <= p2.leadingRole && p1.score > p2.score;
			break;

			case EventSortOrder::Term:
			default:
				if ( p1.leadingElem.term.getTermId() != TermId::None )
				{
					if ( p2.leadingElem.term.getTermId() != TermId::None )
					{
						return p1.leadingRole <= p2.leadingRole && TermComparator( obs )( p1.leadingElem.term, p2.leadingElem.term );
					}
					return true;
				}
				else
				{
					if ( p2.leadingElem.term.getTermId() == TermId::None )
					{
						return p1.leadingRole <= p2.leadingRole && p1.longitude < p2.longitude;
					}
					return false;
				}
			break;
		}
	});

	// sort pp elements by termId
	auto ElementSorter = [ obs ]( const PpElement &e1, const PpElement &e2 )
	{
		return e1.term.getTermId() < e2.term.getTermId();
	};

	for( auto &pic : pictures )
	{
		pic.elements.sort( ElementSorter );
	}

#ifdef DEBUG_PP
	debuglog << "PlanetaryPictureExpert::createPictures found " << pictures.size() << " planetary pictures";
#endif

	return ( totalMatchCount < astroconfig->uranian.maxMatchCount );
}

/*****************************************************
**
**   PlanetaryPictureExpert   ---   createPicturesForPair
**
******************************************************/
void PlanetaryPictureExpert::createPicturesForPair( Horoscope_ptr h1, Horoscope_ptr h2, const HoroscopeRole &leadingRole )
{
#ifdef DEBUG_PP
	debuglog << "PlanetaryPictureExpert::createPicturesForPair h1 " << h1 << " h2 " << h2;
#endif
	UranianExpert expert( props, usection );

	for( const auto &match : expert.findMatches( h1, h2, usection->orbis, &usection->filter ))
	{
		assert( match.e1 );
		assert( match.e2 );

		if ( totalMatchCount > astroconfig->uranian.maxMatchCount )
		{
			warnlog << "PlanetaryPictureExpert::createPicturesForPair: maximum number of matches (" << totalMatchCount << " exceeded";
			break;
		}

		// append match either to pitures with correct leading term or create a new one
		auto pic = std::find_if( pictures.begin(), pictures.end(), [ match, leadingRole ]( const PlanetaryPicture &p )
			{ return p.leadingElem.term == *match.e1->formula && p.leadingRole == leadingRole; } );
		if ( pic != pictures.end())
		{
			pic->elements.push_back( PpElement( *match.e2->formula, match.e2->longitude, match.division, match.orbis ));
		}
		else
		{
			PlanetaryPicture pp( *match.e1->formula, match.e1->longitude, match.division, leadingRole );
			pp.elements.push_back( PpElement( *match.e2->formula, match.e2->longitude, match.division, match.orbis ));
			pictures.push_back( pp );
		}
		totalMatchCount++;
	}

	for( auto &pic : pictures )
	{
		pic.calculateScore( usection->orbis );
	}
}

/*****************************************************
**
**   PlanetaryPictureExpert   ---   writePictureTablePart
**
******************************************************/
void PlanetaryPictureExpert::writePictureTablePart( MDocument_ptr doc, const uint &istart, const uint &iend )
{
	assert( iend > istart );
	uint maxrow = 0;

	auto iter = pictures.begin();
	std::advance( iter, istart );
	for ( uint i = istart; i < iend; i++ )
	{
		assert( iter != pictures.end());
		maxrow = std::max( (uint)iter->elements.size(), maxrow );
		++iter;
	}
	Table *table = new Table( iend - istart, maxrow + 1 );

	int col = 0;
	iter = pictures.begin();
	std::advance( iter, istart );
	for ( uint i = istart; i < iend; i++ )
	{
		assert( iter != pictures.end());
		{
			MString ms;
			ms << iter->leadingElem.term;
			table->setHeader( col, ms );
		}
		int row = 1;
#ifdef DEBUG_PP
		debuglog << "iter elems size " << iter->elements.size();
#endif
		for( const PpElement &elem : iter->elements )
		{
			MString ms;
			ms << elem.term;
			table->setEntry( col, row, ms );
			++row;
		}
		++iter;
		++col;
	}
	doc->addItem( table );
}

/*****************************************************
**
**   PlanetaryPictureExpert   ---   writePictureTable
**
******************************************************/
void PlanetaryPictureExpert::writePictureTable( MDocument_ptr doc )
{
	std::set<PlanetId> keys;

	doc->addHeader2( tr( "Planetary Pictures" ));
	if ( pictures.size() == 0 )
	{
		doc->addLine( tr( "No matches" ));
	}
	else
	{
		if ( pictures.size() > 10 )
		{
			uint pmid = (uint)pictures.size() / 2;
			writePictureTablePart( doc, 0, pmid );
			writePictureTablePart( doc, pmid, pictures.size());
		}
		else
		{
			writePictureTablePart( doc, 0, pictures.size());
		}
	}
}



