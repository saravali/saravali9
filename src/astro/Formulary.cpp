/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Formulary.cpp
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

#include "Formulary.h"

#include "enumserializer.h"
#include "logger.h"

#include <algorithm>

#define DEBUG_FORMULARY

/****************************************************
**
**   Formulary   ---   Constructor
**
******************************************************/
Formulary::Formulary()
	: defs (
	{
		{ TermId::Single, TermId::Single },
		{ TermId::Single, TermId::Midpoint },
		{ TermId::Single, TermId::ReflectionPoint },
		{ TermId::Single, TermId::Sum },
		{ TermId::Single, TermId::Difference },
		{ TermId::Sum, TermId::Sum },
		{ TermId::Difference, TermId::Difference },
		{ TermId::Single, TermId::Triple },
		{ TermId::Single, TermId::TripleSum },
	} ),
	termnames (
	{
		{ TermId::Single, tr( "Single" ) },
		{ TermId::Midpoint, tr( "Midpoints" ) },
		{ TermId::ReflectionPoint, tr( "Reflection Points" ) },
		{ TermId::Sum, tr( "Sums" ) },
		{ TermId::Difference, tr( "Differences" ) },
		{ TermId::Triple, tr( "Triples" ) },
		{ TermId::TripleSum, tr( "Triple Sums" ) },
		{ TermId::AntiSingle, tr( "Anti Single" ) },
		{ TermId::AntiMidpoint, tr( "Anti Midpoints" ) },
		{ TermId::AntiReflectionPoint, tr( "Anti ReflectionPoints" ) },
		{ TermId::AntiSum, tr( "Anti Sums" ) },
		{ TermId::AntiDifference, tr( "Anti Differences" ) },
		{ TermId::AntiTriple, tr( "Anti Triples" ) },
		{ TermId::AntiTripleSum, tr( "Anti Triple Sums" ) }
	})
{
}

/****************************************************
**
**   Formulary   ---   getFormulaNames
**
******************************************************/
std::vector<wxString> Formulary::getFormulaNames( const UranianContext &ctx, const TextFormat format ) const
{
	std::vector<wxString> l;
	for( uint i = 0; i < defs.size(); ++i )
	{
#ifdef DEBUG_FORMULARY
		debuglog << "Formulary::getFormulaNames ctx " << ctx << " i " << i << " bool " << std::boolalpha << IS_CHART_CONTEXT( ctx );
#endif

		l.push_back( getFormulaName( i, format ));
		if ( i >= 4 && IS_CHART_CONTEXT( ctx )) break;
	}
	return l;
}


/****************************************************
**
**   Formulary   ---   getFormulaName
**
******************************************************/
wxString Formulary::getFormulaName( const int &i, const TextFormat format ) const
{
	const static std::vector<std::pair<wxString, wxString>> names =
	{
		{ tr( "Planets" ), "conj" },
		{ tr( "Midpoints" ), "mid" },
		{ tr( "Reflection Points" ), "refl" },
		{ tr( "Sums" ), "sum" },
		{ tr( "Differences" ), "dif" },
		{ tr( "Mixed Sums" ), "msum2" },
		{ tr( "Mixed Differences" ), "mdif" },
		{ tr( "Triples" ), "trip" },
		{ tr( "Triple Sums" ), "tsum" },
	};
	assert( i >= 0 && i < (int)names.size());
	return format == TextFormat::Long ?  names[i].first : names[i].second;
}

/****************************************************
**
**   Formulary   ---   getFormDefPairs
**
******************************************************/
std::vector<TermIdPair> Formulary::getFormDefPairs( const std::list<int> &formulae, const bool includeAntiscia,
	const bool includeLrPermutations )
{
#ifdef DEBUG_FORMULARY
	debuglog << "Formulary::getFormDefPairs " << formulae.size() << " formulas, includeAntiscia "
		<< std::boolalpha << includeAntiscia;
#endif

	std::vector<TermIdPair> v;
	for( const int f : formulae )
	{
		if ( f >= 0 && f < (int)defs.size())
		{
			v.push_back( defs[f] );
			if ( includeAntiscia )
			{
				v.push_back( std::make_pair( defs[f].first, getAntiTerm( defs[f].second )));
			}
			if ( includeLrPermutations )
			{
				// combinations single/single are alredy included above
				if ( defs[f].first != TermId::Single || defs[f].second != TermId::Single )
				{
					v.push_back( { defs[f].second, defs[f].first } );
				}
			}
		}
		else
		{
			errorlog << "Formulary::getFormDefPairs called with invalid parameter " << f;
		}
	}
#ifdef DEBUG_FORMULARY
	debuglog << "Formulary::getFormDefPairs " << v.size() << " formulas found";
#endif
	return v;
}

/****************************************************
**
**   Formulary   ---   getAntiTerm
**
******************************************************/
TermId Formulary::getAntiTerm( const TermId &id ) const
{
	assert_valid_enum<TermId>( id );

	// total number of terms in one group of term/antiterm
	constexpr int reflectionIndex = (int)TermId::LastSingleTerm + 1;

	if ( id >= TermId::First && id <= TermId::LastSingleTerm )
	{
		return (TermId)( (int)id + reflectionIndex );
	}
	else
	{
		return (TermId)( (int)id - reflectionIndex );
	}
}
/****************************************************
**
**   Formulary   ---   getTermName
**
******************************************************/
wxString Formulary::getTermName( const TermId &id, const TextFormat /*format*/ ) const
{
	assert_valid_enum<TermId>( id );
	auto a = termnames.find( id );
	if ( a != termnames.end()) return a->second;
	else return "invalid term";
}

/****************************************************
**
**   Formulary   ---   getTermIds
**
******************************************************/
std::list<TermId> Formulary::getTermIds( const bool includeAntiTerms ) const
{
	std::list<TermId> ids;
	for ( const TermId &id : Enum<TermId>( TermId::First, includeAntiTerms ? TermId::Last : TermId::LastSingleTerm ))
	{
		ids.push_back( id );
	}
	return ids;
}

