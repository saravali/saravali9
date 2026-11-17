/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/UranianTextLoader.cpp
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

#include "UranianTextLoader.h"

#include "astroconfig.h"
#include "enumserializer.h"
#include "FileConfig.h"
#include "logger.h"
#include "Term.h"
#include "Uranian.h"

#include <wx/filename.h>
#include <wx/textfile.h>

extern Astroconfig *astroconfig;

/*************************************************//**
*
* 
*
******************************************************/
struct PairTextItem
{
	PairTextItem( const PlanetId &p1, const PlanetId &p2, wxString text )
		: p1 ( p1 ), p2( p2 ), text( text ) {}

	const PlanetId p1, p2;
	wxString text;
};

/*************************************************//**
*
* 
*
******************************************************/
struct TripleTextItem
{
	TripleTextItem( const PlanetId &p, const PlanetId &p1, const PlanetId &p2, wxString text )
		: p( p ), p1 ( p1 ), p2( p2 ), text( text ) {}

	const PlanetId p, p1, p2;
	wxString text;
};

/*****************************************************
**
**   UranianTextLoader   ---   Constructor
**
******************************************************/
UranianTextLoader::UranianTextLoader()
	: wobjects(
	{
		{ "mc" , PlanetId::Meridian },
		{ "ar" , PlanetId::Aries },
		{ "so" , PlanetId::Sun },
		{ "as" , PlanetId::Ascendant },
		{ "mo" , PlanetId::Moon },
		{ "kn" , PlanetId::AscNode },
		{ "me" , PlanetId::Mercury },
		{ "ve" , PlanetId::Venus },
		{ "ma" , PlanetId::Mars },
		{ "ju" , PlanetId::Jupiter },
		{ "sa" , PlanetId::Saturn },
		{ "ur" , PlanetId::Uranus },
		{ "ne" , PlanetId::Neptune },
		{ "pl" , PlanetId::Pluto },
		{ "cu" , PlanetId::Cupido },
		{ "ha" , PlanetId::Hades },
		{ "ze" , PlanetId::Zeus },
		{ "kr" , PlanetId::Kronos },
		{ "ap" , PlanetId::Apollon },
		{ "ad" , PlanetId::Admetos },
		{ "vu" , PlanetId::Vulkanus },
		{ "po" , PlanetId::Poseidon }
	} )
{
}

/*****************************************************
**
**   UranianTextLoader   ---   Destructor
**
******************************************************/
UranianTextLoader::~UranianTextLoader()
{
	cleanup();
}

/*****************************************************
**
**   UranianTextLoader   ---   cleanup
**
******************************************************/
void UranianTextLoader::cleanup()
{
	for ( TripleTextItem *item : triple_e ) delete item;
	triple_e.clear();
	for ( TripleTextItem *item : triple_w ) delete item;
	triple_w.clear();
	for ( PairTextItem *item : pair_w ) delete item;
	pair_w.clear();
}

/*****************************************************
**
**   UranianTextLoader   ---   mapPlanetId
**
******************************************************/
PlanetId UranianTextLoader::mapPlanetId( const PlanetId &p ) const
{
	switch( p )
	{
		case PlanetId::Capricorn:
			return PlanetId::Aries;
		break;
		case PlanetId::MeanAscNode:
			return PlanetId:: AscNode;
		break;
		case PlanetId::MeanDescNode:
			return PlanetId:: DescNode;
		break;
		default:
		break;
	}
	return p;	
}

/*****************************************************
**
**   UranianTextLoader   ---   getTripleTextItem
**
******************************************************/
TripleTextItem *UranianTextLoader::getTripleTextItem( const PlanetId &pp, const PlanetId &pp1,
	const PlanetId &pp2, const int &mode )
{
	const PlanetId p = mapPlanetId( pp );
	const PlanetId p1 = mapPlanetId( pp1 );
	const PlanetId p2 = mapPlanetId( pp2 );
	if ( mode )
	{
		// Witte
		if ( triple_w.empty()) loadItems( mode );
		for ( const auto item : triple_w )
		{
			if ( item->p == p && (( item->p1 == p1 && item->p2 == p2 )
				|| ( item->p1 == p2 && item->p2 == p1 ))
			 ) return item;
		}
	}
	else
	{
		// Ebertin
		if ( triple_e.empty()) loadItems( mode );
		for ( const auto item : triple_e )
		{
			if ( item->p == p &&
			(( item->p1 == p1 && item->p2 == p2 )
			|| ( item->p1 == p2 && item->p2 == p1 ))
			 ) return item;
		}
	}
	return 0;
}

/*****************************************************
**
**   UranianTextLoader   ---   getPairTextItem
**
******************************************************/
PairTextItem *UranianTextLoader::getPairTextItem( const PlanetId &pp1, const PlanetId &pp2, const int &mode )
{
	debuglog << "UranianTextLoader::getPairTextItem p1 " << pp1 << " p2 " << pp2 << " mode " << mode;;
	const PlanetId p1 = mapPlanetId( pp1 );
	const PlanetId p2 = mapPlanetId( pp2 );
	if ( mode )
	{
		if ( pair_w.empty()) loadItems( mode );
		for ( const auto item : pair_w )
		{
			if (( item->p1 == p1 && item->p2 == p2 )
				|| ( item->p1 == p2 && item->p2 == p1 )) return item;
		}
	}
	return 0;
}

/*****************************************************
**
**   UranianTextLoader   ---   getText4Term
**
******************************************************/
wxString UranianTextLoader::getText4Term( const MTerm &term, const bool witteMode )
{
	if ( term.isTripleTerm())
	{
		const std::vector<PlanetId> &obs = term.getPlanetIds();
		TripleTextItem *mpitem = getTripleTextItem( obs[0], obs[1], obs[2], witteMode );
		if ( mpitem ) return mpitem->text;
		else return "ERROR";
	}
	else
	{
		const PlanetId ob1 = term.getFirstPlanet();
		assert( ob1 != PlanetId::None );
		const PlanetId ob2 = term.isSingleTerm() ? ob1 : term.getSecondPlanet();

		PairTextItem *citem = getPairTextItem( ob1, ob2, witteMode );
		if ( citem ) return citem->text;
		else return "ERROR";
	}
}

/*****************************************************
**
**   UranianTextLoader   ---   getText4Match
**
******************************************************/
wxString UranianTextLoader::getText4Match( const Match &match, const bool witteMode )
{
	return getText4Pair( match.e1->formula, match.e2->formula, witteMode );
}

/*****************************************************
**
**   UranianTextLoader   ---   getText4Pair
**
******************************************************/
wxString UranianTextLoader::getText4Pair( MTerm *f1, MTerm *f2, const bool witteMode )
{
	if ( ! f1 || ! f2 ) return "  no term - ";

	const std::vector<PlanetId> &obs1 = f1->getPlanetIds();
	const std::vector<PlanetId> &obs2 = f2->getPlanetIds();

	if ( obs1.size() == 1 && obs2.size() == 1 )
	{
		PairTextItem *citem = getPairTextItem( obs1[0], obs2[0], witteMode );
		if ( citem ) return citem->text;
	}
	else if ( obs1.size() == 1 && obs2.size() == 2 )
	{
		TripleTextItem *mpitem = getTripleTextItem( obs1[0], obs2[0], obs2[1], witteMode );
		if ( mpitem ) return mpitem->text;
		else
		{
			PairTextItem *citem = nullptr;
			if ( obs1[0] == obs2[0] ) citem = getPairTextItem( obs1[0], obs2[1], witteMode );
			else if ( obs1[0] == obs2[1] ) citem = getPairTextItem( obs1[0], obs2[0], witteMode );
			if ( citem ) return citem->text;
		}
	}
	else if ( obs1.size() == 2 && obs2.size() == 1 )
	{
		TripleTextItem *mpitem = getTripleTextItem( obs2[0], obs1[0], obs1[1], witteMode );
		if ( mpitem ) return mpitem->text;
		else
		{
			PairTextItem *citem = nullptr;
			if ( obs1[0] == obs2[0] ) citem = getPairTextItem( obs1[1], obs2[0], witteMode );
			else if ( obs1[1] == obs2[0] ) citem = getPairTextItem( obs1[0], obs2[0], witteMode );
			if ( citem ) return citem->text;
		}
	}
	return "";
}

/*****************************************************
**
**   UranianTextLoader   ---  getTextfileName
**
******************************************************/
wxString UranianTextLoader::getTextfileName( const int &mode )
{
	wxString filename = FileConfig::get()->getResourceDir( ConfigResource::Uranian );
	if ( mode )
	{
		filename += "witte.txt";
	}
	else
	{
  	filename += "ebertin.txt";
	}
	return filename;
}

/*****************************************************
**
**   UranianTextLoader   ---  hasItemsForMode
**
******************************************************/
bool UranianTextLoader::hasItemsForMode( const int &mode )
{
	wxString filename = getTextfileName( mode );
	const bool b = wxFileName::IsFileReadable( filename );
	debuglog << "UranianTextLoader::hasItemsForMode filename " << filename << " flag " << b;
	return b;
}

/*****************************************************
**
**   UranianTextLoader   ---   loadItems
**
******************************************************/
bool UranianTextLoader::loadItems( const int &mode )
{
	std::size_t tabpos;
	PlanetId p, p1, p2;
	wxString cp, cp1, cp2, text;
	wxString filename = getTextfileName( mode );

	if ( ! wxFileName::IsFileReadable( filename ))
	{
		return false;
	}

	wxTextFile file( filename );
	if( ! file.Open()) return false;

	for ( wxString line = file.GetFirstLine(); ! file.Eof(); line = file.GetNextLine() )
	{
		tabpos = line.find( wxString( "\t" ));
		if ( tabpos == 4 )
		{
			if ( mode == 1 )
			{
				// pair event
				cp1 = line.substr( 0, 2 );
				p1 = getTextItem( cp1 );
				cp2 = line.substr( 2, 2 );
				p2 = getTextItem( cp2 );
				//text = s.Mid( 5 );
				text = line.substr( 5 );
				if ( p1 != PlanetId::None && p2 != PlanetId::None && ! text.empty() )
					pair_w.push_back( new PairTextItem( p1, p2, text ));
			}
		}
		else if ( tabpos == 6 )
		{
			// triple event
			cp = line.substr( 0, 2 );
			p = getTextItem( cp );
			cp1 = line.substr( 2, 2 );
			p1 = getTextItem( cp1 );
			cp2 = line.substr( 4, 2 );
			p2 = getTextItem( cp2 );
			text = line.substr( 7 );
			if ( p != PlanetId::None && p1 != PlanetId::None && p2 != PlanetId::None && ! text.empty() )
			{
				if ( mode ) triple_w.push_back( new TripleTextItem( p, p1, p2, text ));
				else triple_e.push_back( new TripleTextItem( p, p1, p2, text ));
			}
		}
		else
		{
			errorlog << "Wrong tabpos " << tabpos << " in Uranian file " << line;
		}
	}
	return true;
}

/*****************************************************
**
**   UranianTextLoader   ---   getTextItem
**
******************************************************/
PlanetId UranianTextLoader::getTextItem( wxString s )
{
	const PlanetId ret = wobjects.at( s );
	if ( ret == PlanetId::Sun && s != wxString( "so" )) return PlanetId::None;
	else return ret;
}

