/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Term.cpp
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

#include "Term.h"

#include "astrobase.h"
#include "astroconfig.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "mathbase.h"
#include "Lang.h"
#include "logger.h"

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

//#define DEBUG_MTERM

/*****************************************************
**
**   TermComparator
**
******************************************************/
bool TermComparator::operator()( const MTerm &t1, const MTerm &t2 ) const
{
	const PlanetId ob1 = t1.getFirstPlanet();
	const PlanetId ob2 = t2.getFirstPlanet();
	if ( ob1 != ob2 ) return PlanetComparator( obs )( ob1,  ob2 );

	if ( t1.getVariableCount() > 1 )
	{
		const PlanetId ob1a = t1.getSecondPlanet();
		const PlanetId ob2a = t2.getSecondPlanet();
		if ( ob1a != ob2a ) return PlanetComparator( obs )( ob1a,  ob2a );
	}

	if ( t1.getVariableCount() > 2 )
	{
		const PlanetId ob1b = t1.getThirdPlanet();
		const PlanetId ob2b = t2.getThirdPlanet();
		if ( ob1b != ob2b ) return PlanetComparator( obs )( ob1b,  ob2b );
	}
	return t1.getTermId() < t2.getTermId();
}

/*****************************************************
**
**   MTerm   ---   Constructor
**
******************************************************/
MTerm::MTerm( const PlanetId &ob1, const PlanetId &ob2, const PlanetId &ob3, const TermId &termId )
	: termId( termId )
{
	incarnate( ob1, ob2, ob3 );
}

/****************************************************
**
**   MTerm   ---   changeTermId
**
******************************************************/
void MTerm::changeTermId( const TermId &id )
{
	unincarnate();
	termId = id;
}

/****************************************************
**
**   MTerm   ---   getVariableCount (static)
**
******************************************************/
int MTerm::getVariableCount( const TermId &id )
{
	switch( id )
	{
		case TermId::Single:
		case TermId::AntiSingle:
			return 1;
		break;
		case TermId::Midpoint:
		case TermId::ReflectionPoint:
		case TermId::Sum:
		case TermId::Difference:
		case TermId::AntiMidpoint:
		case TermId::AntiReflectionPoint:
		case TermId::AntiSum:
		case TermId::AntiDifference:
			return 2;
		break;
		case TermId::Triple:
		case TermId::TripleSum:
		case TermId::AntiTriple:
		case TermId::AntiTripleSum:
			return 3;
		break;
		case TermId::None:
			return 0;
		break;
		default:
			errorlog << "Invalid TermId " << static_cast<int>( id );
			assert( false );
		break;
	}
	return 0;
}

/****************************************************
**
**   MTerm   ---   isSymmetric (static)
**
******************************************************/
bool MTerm::isSymmetric( const TermId &id )
{
	switch( id )
	{
		case TermId::Single:
		case TermId::AntiSingle:
		case TermId::Sum:
		case TermId::AntiSum:
		case TermId::Midpoint:
		case TermId::AntiMidpoint:
		case TermId::TripleSum:
		case TermId::AntiTripleSum:
			return true;
		break;
		case TermId::ReflectionPoint:
		case TermId::Difference:
		case TermId::AntiReflectionPoint:
		case TermId::AntiDifference:
		case TermId::Triple:
		case TermId::AntiTriple:
			return false;
		break;
		default:
			errorlog << "Invalid TermId " << static_cast<int>( id );
			assert( false );
		break;
	}
	return true;
}

/*****************************************************
**
**   MTerm   ---   isAntiTermId (static)
**
******************************************************/
bool MTerm::isAntiTermId( const TermId &id )
{
	return ( id >= TermId::FirstAntiTerm && id <= TermId::Last );
}

/*****************************************************
**
**   MTerm   ---   isSimpleMinusFormula() (static)
**
******************************************************/
bool MTerm::isSimpleMinusFormula( const TermId &id )
{
	return id == TermId::Difference;
}

/*****************************************************
**
**   MTerm   ---   incarnate
**
******************************************************/
void MTerm::incarnate( const PlanetId &ob0, const PlanetId ob1, const PlanetId ob2 )
{
	obs[0] = ob0;
	obs[1] = ob1;
	obs[2] = ob2;
}

/*****************************************************
**
**   MTerm   ---   incarnate
**
******************************************************/
void MTerm::incarnate( const std::vector<PlanetId> &o )
{
	assert( o.size() > 0 );
	obs[0] = o[0];
	if ( o.size() > 1 )
	{
		obs[1] = o[1];
		if ( o.size() > 2 )
		{
			obs[2] = o[2];
		}
		else
		{
			obs[2] = PlanetId::None;
		}
	}
	else
	{
		obs[1] = PlanetId::None;
		obs[2] = PlanetId::None;
	}
}

/*****************************************************
**
**   MTerm   ---   getPlanetId
**
******************************************************/
PlanetId MTerm::getPlanetId( const uint &id ) const
{
	assert( id < MAX_TERM_VARS );
	return obs[id];
}

/*****************************************************
**
**   MTerm   ---   getPlanetIds
**
******************************************************/
// returns all valid object ids. wer braucht das?
std::vector<PlanetId> MTerm::getPlanetIds() const
{
	std::vector<PlanetId> ids;
	for( const PlanetId ob : obs )
	{
		if ( ob != PlanetId::None ) ids.push_back( ob );
		else break;
	}
	return ids;
}

/*****************************************************
**
**   MTerm   ---   containsOneOf
**
******************************************************/
bool MTerm::containsOneOf( const std::set<PlanetId> &planets ) const
{
	if ( ! isIncarnated()) return false;
	for( const PlanetId &ob : obs )
	{
		if ( planets.contains( ob ))
		{
			return true;
		}
	}
	return false;
}

/*****************************************************
**
**   MTerm   ---   hasDuplicates
**
******************************************************/
bool MTerm::hasDuplicates() const
{
	const int varcount = getVariableCount();
	if ( varcount <= 1 ) return false;
	if ( varcount == 2 )
	{
		return obs[0] == obs[1];
	}
	//assert( varcount ==3 );
	return( obs[0] == obs[1] || obs[1] == obs[2] || obs[0] == obs[2] );
}

/*****************************************************
**
**   MTerm   ---   unincarnate
**
******************************************************/
void MTerm::unincarnate()
{
	for( int i = 0; i < MAX_TERM_VARS; i++ )
	{
		obs[i] = PlanetId::None;
	}
}

/*****************************************************
**
**   MTerm   ---   applyFilter
**
******************************************************/
bool MTerm::applyFilter( const MatchFilter &filter ) const
{
#ifdef DEBUG_MTERM
	debuglog << "MTerm::applyFilter rule #" << filter.rule;
#endif

	if ( filter.isPassThrough() ) return true;

	switch( filter.rule )
	{
		case 0: // one must match
			for( const PlanetId o : obs )
			{
				if ( o != PlanetId::None && filter.selectedPlanets.contains( o )) return true;
			}
		break;
		case 1: // all in filter must be contained in obs
			for( const PlanetId &filterob : filter.selectedPlanets )
			{
				bool found = false;
				for( const PlanetId o : obs )
				{
					if ( o == filterob )
					{
						found = true;
						break;
					}
				}
				if ( ! found ) return false;
			}
			return true;
		break;
		case 2: // only these in term, i.e. all planets in obs are selected in filter
			for( const PlanetId o : obs )
			{
				if ( o != PlanetId::None && ! filter.selectedPlanets.contains( o )) return false;
			}
			return true;
		break;
		default:
			assert( false );
		break;
	}
	return false;
}

/*****************************************************
**
**   MTerm   ---   evaluateSpeed
**
******************************************************/
std::pair<double, double> MTerm::evaluateSpeed( Horoscope_ptr h, const Viewpoint &viewpoint,
	const PlanetId &ob1, const PlanetId ob2, const PlanetId ob3 )
{
	incarnate( ob1, ob2, ob3 );
	return evaluateSpeed( h, viewpoint );
}

/*****************************************************
**
**   MTerm   ---   evaluateSpeed
**
******************************************************/
std::pair<double, double> MTerm::evaluateSpeed( Horoscope_ptr h, const Viewpoint &viewpoint )
{
	assert( h );
	double r = 0;
	double s = 0;

	switch( termId )
	{
		case TermId::Single:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] == PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = h->getLongitude( obs[0], viewpoint );
			s = h->getLSpeed( obs[0], viewpoint );
		break;
		case TermId::Midpoint:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = getMidpoint( h->getLongitude( obs[0], viewpoint ),
				h->getLongitude( obs[1], viewpoint ));
			s = .5 * ( h->getLSpeed( obs[0], viewpoint ) + h->getLSpeed( obs[1], viewpoint ));
		break;
		case TermId::ReflectionPoint: // 2*q + p
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = 2 * h->getLongitude( obs[1], viewpoint ) - h->getLongitude( obs[0], viewpoint );
			s = 2 * h->getLSpeed( obs[1], viewpoint ) - h->getLSpeed( obs[0], viewpoint );
		break;
		case TermId::Sum:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = h->getLongitude( obs[0], viewpoint ) + h->getLongitude( obs[1], viewpoint );
			s = h->getLSpeed( obs[0], viewpoint ) + h->getLSpeed( obs[1], viewpoint );
		break;
		case TermId::Difference:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = h->getLongitude( obs[0], viewpoint ) - h->getLongitude( obs[1], viewpoint );
			s = h->getLSpeed( obs[0], viewpoint ) - h->getLSpeed( obs[1], viewpoint );
		break;

		case TermId::Triple:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] != PlanetId::None );
			r = h->getLongitude( obs[0], viewpoint ) + h->getLongitude( obs[1], viewpoint )
				- h->getLongitude( obs[2], viewpoint );
			s = h->getLSpeed( obs[0], viewpoint ) + h->getLSpeed( obs[1], viewpoint ) - h->getLSpeed( obs[2], viewpoint );
		break;
		case TermId::TripleSum:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] != PlanetId::None );
			r = h->getLongitude( obs[0], viewpoint ) + h->getLongitude( obs[1], viewpoint )
				+ h->getLongitude( obs[2], viewpoint );
			s = h->getLSpeed( obs[0], viewpoint ) + h->getLSpeed( obs[1], viewpoint ) + h->getLSpeed( obs[2], viewpoint );
		break;

		case TermId::AntiSingle:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] == PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = getAntiscium( h->getLongitude( obs[0], viewpoint ));
			s = - h->getLSpeed( obs[0], viewpoint );
		break;
		case TermId::AntiMidpoint:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = getAntiscium( getMidpoint( h->getLongitude( obs[0], viewpoint ),
				h->getLongitude( obs[1], viewpoint )));
			s = - .5 * ( h->getLSpeed( obs[0], viewpoint ) + h->getLSpeed( obs[1], viewpoint ));
		break;
		case TermId::AntiReflectionPoint:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = getAntiscium( 2 * h->getLongitude( obs[0], viewpoint ) - h->getLongitude( obs[1], viewpoint ));
			s = -2 * h->getLSpeed( obs[0], viewpoint ) + h->getLSpeed( obs[1], viewpoint );
		break;
		case TermId::AntiSum:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = getAntiscium( h->getLongitude( obs[0], viewpoint ) + h->getLongitude( obs[1], viewpoint ));
			r = - h->getLSpeed( obs[0], viewpoint ) - h->getLSpeed( obs[1], viewpoint );
		break;
		case TermId::AntiDifference:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = getAntiscium( h->getLongitude( obs[0], viewpoint ) - h->getLongitude( obs[1], viewpoint ));
			s = - h->getLSpeed( obs[0], viewpoint ) + h->getLSpeed( obs[1], viewpoint );
		break;

		case TermId::AntiTriple:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] != PlanetId::None );
			r = getAntiscium( h->getLongitude( obs[0], viewpoint ) + h->getLongitude( obs[1], viewpoint )
				- h->getLongitude( obs[2], viewpoint ));
			s = - h->getLSpeed( obs[0], viewpoint ) - h->getLSpeed( obs[1], viewpoint ) + h->getLSpeed( obs[2], viewpoint );
		break;
		case TermId::AntiTripleSum:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] != PlanetId::None );
			r = getAntiscium( h->getLongitude( obs[0], viewpoint ) + h->getLongitude( obs[1], viewpoint )
				+ h->getLongitude( obs[2], viewpoint ));
			s = - h->getLSpeed( obs[0], viewpoint ) - h->getLSpeed( obs[1], viewpoint ) - h->getLSpeed( obs[2], viewpoint );
		break;

		case TermId::None:
		default:
			assert( false );
		break;
	}
	return std::make_pair( red_deg( r ), s );
}

/*****************************************************
**
**   MTerm   ---   evaluate
**
******************************************************/
double MTerm::evaluate( Horoscope_ptr h, const Viewpoint &viewpoint,
	const PlanetId &ob1, const PlanetId ob2, const PlanetId ob3 )
{
	incarnate( ob1, ob2, ob3 );
	return evaluate( h, viewpoint );
}

/*****************************************************
**
**   MTerm   ---   evaluate
**
******************************************************/
double MTerm::evaluate( Horoscope_ptr h, const Viewpoint &viewpoint )
{
	assert( h );
	double r = 0;

	switch( termId )
	{
		case TermId::Single:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] == PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = h->getLongitude( obs[0], viewpoint );
		break;
		case TermId::Midpoint:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = getMidpoint( h->getLongitude( obs[0], viewpoint ),
				h->getLongitude( obs[1], viewpoint ));
		break;
		case TermId::ReflectionPoint:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = 2 * h->getLongitude( obs[1], viewpoint ) - h->getLongitude( obs[0], viewpoint );
		break;
		case TermId::Sum:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = h->getLongitude( obs[0], viewpoint ) + h->getLongitude( obs[1], viewpoint );
		break;
		case TermId::Difference:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = h->getLongitude( obs[0], viewpoint ) - h->getLongitude( obs[1], viewpoint );
		break;

		case TermId::Triple:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] != PlanetId::None );
			r = h->getLongitude( obs[0], viewpoint ) + h->getLongitude( obs[1], viewpoint )
				- h->getLongitude( obs[2], viewpoint );
		break;
		case TermId::TripleSum:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] != PlanetId::None );
			r = h->getLongitude( obs[0], viewpoint ) + h->getLongitude( obs[1], viewpoint )
				+ h->getLongitude( obs[2], viewpoint );
		break;

		case TermId::AntiSingle:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] == PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = getAntiscium( h->getLongitude( obs[0], viewpoint ));
		break;
		case TermId::AntiMidpoint:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = getAntiscium( getMidpoint( h->getLongitude( obs[0], viewpoint ),
				h->getLongitude( obs[1], viewpoint )));
		break;
		case TermId::AntiReflectionPoint:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = getAntiscium( 2 * h->getLongitude( obs[0], viewpoint ) - h->getLongitude( obs[1], viewpoint ));
		break;
		case TermId::AntiSum:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = getAntiscium( h->getLongitude( obs[0], viewpoint ) + h->getLongitude( obs[1], viewpoint ));
		break;
		case TermId::AntiDifference:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] == PlanetId::None );
			r = getAntiscium( h->getLongitude( obs[0], viewpoint ) - h->getLongitude( obs[1], viewpoint ));
		break;

		case TermId::AntiTriple:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] != PlanetId::None );
			r = getAntiscium( h->getLongitude( obs[0], viewpoint ) + h->getLongitude( obs[1], viewpoint )
				- h->getLongitude( obs[2], viewpoint ));
		break;
		case TermId::AntiTripleSum:
			assert( obs[0] != PlanetId::None );
			assert( obs[1] != PlanetId::None );
			assert( obs[2] != PlanetId::None );
			r = getAntiscium( h->getLongitude( obs[0], viewpoint ) + h->getLongitude( obs[1], viewpoint )
				+ h->getLongitude( obs[2], viewpoint ));
		break;

		case TermId::None:
		default:
			assert( false );
		break;
	}
	return red_deg( r );
}

/*****************************************************
**
**   MTerm   ---   firstMString
**
******************************************************/
MString MTerm::firstMString( const Viewpoint &viewpoint, const int *antisciaCfg ) const
{
	return firstMString( astroconfig->getViewpointConfig( viewpoint ).planets, antisciaCfg );
}

/*****************************************************
**
**   MTerm   ---   firstMString
**
******************************************************/
MString MTerm::firstMString( const PlanetConfig &cfg, const int *antisciaCfg ) const
{
	const MToken s0 = isIncarnated() ?  MToken( obs[0], cfg ) : wxString( "p" );
	MString s;
	switch( getTermId())
	{
		case TermId::Single:
		case TermId::Midpoint:
		case TermId::ReflectionPoint:
		case TermId::Sum:
		case TermId::Difference:
		case TermId::Triple:
		case TermId::TripleSum:
			s << s0;
		break;

		case TermId::AntiSingle:
		case TermId::AntiMidpoint:
		case TermId::AntiReflectionPoint:
		case TermId::AntiSum:
		case TermId::AntiDifference:
		case TermId::AntiTriple:
		case TermId::AntiTripleSum:
		{
			const int aCfg = antisciaCfg ? *antisciaCfg : astroconfig->uranian.antisciaDisplayStyle;
			// * % @ -
			switch ( aCfg )
			{
				case 1:
					s << "%" << s0;
				break;
				case 2:
					s << "@" << s0;
				break;
				case 3:
					s << "-" << s0;
				break;
				default:
					s << "*" << s0;
				break;
			}
		}
		break;

		case TermId::None:
			s << "[None]";
		break;
		default:
			assert( false );
		break;
	}
	return s;
}

/*****************************************************
**
**   MTerm   ---   secondMString
**
******************************************************/
MString MTerm::secondMString( const Viewpoint &viewpoint ) const
{
	return secondMString( astroconfig->getViewpointConfig( viewpoint ).planets);
}

/*****************************************************
**
**   MTerm   ---   secondMString
**
******************************************************/
MString MTerm::secondMString( const PlanetConfig &cfg ) const
{
	MString s;
	MToken s1, s2;
	if ( isIncarnated())
	{
		s1 = MToken( obs[1], cfg );
		s2 = MToken( obs[2], cfg );
	}
	else
	{
		s1 = wxString( "q" );
		s2 = wxString( "r" );
	}

	const int varcount = getVariableCount();
	switch( getTermId())
	{
		case TermId::Single:
		case TermId::AntiSingle:
		case TermId::None:
		break;

		case TermId::Midpoint:
		case TermId::ReflectionPoint:
		case TermId::Sum:
		case TermId::Difference:
		case TermId::AntiMidpoint:
		case TermId::AntiReflectionPoint:
		case TermId::AntiSum:
		case TermId::AntiDifference:
			s << s1;
		break;

		case TermId::Triple:
		case TermId::AntiTriple:
			assert( varcount == 3 );
			s << s1 << "-" << s2;
		break;
		case TermId::TripleSum:
		case TermId::AntiTripleSum:
			assert( varcount == 3 );
			s << s1 << "+" << s2;
		break;

		default:
			assert( false );
		break;
	}
	return s;
}

/*****************************************************
**
**   MTerm   ---   getOperatorRepresentation STATIC
**
******************************************************/
wxString MTerm::getOperatorRepresentation( const TermId &tid, const int position )
{
	switch( tid )
	{
		case TermId::Single:
		case TermId::AntiSingle:
		case TermId::None:
			return "";
		break;
		case TermId::Midpoint:
		case TermId::AntiMidpoint:
			return "|";
		break;
		case TermId::Sum:
		case TermId::TripleSum:
		case TermId::AntiSum:
		case TermId::AntiTripleSum:
			return "+";
		break;
		case TermId::ReflectionPoint:
			return "→";
		break;
		case TermId::Difference:
		case TermId::AntiReflectionPoint:
		case TermId::AntiDifference:
			return "-";
		break;

		case TermId::Triple:
		case TermId::AntiTriple:
			return position ? "-" : "+";
		break;

		default:
			assert( false );
		break;
	}
  return "<error>";
}

/*****************************************************
**
**   MTerm   ---   getOperatorRepresentation
**
******************************************************/
/*
wxString MTerm::getOperatorRepresentation( const int position ) const
{
	switch( getTermId())
	{
		case TermId::Single:
		case TermId::AntiSingle:
		case TermId::None:
			return "";
		break;
		case TermId::Midpoint:
		case TermId::AntiMidpoint:
			return "|";
		break;
		case TermId::Sum:
		case TermId::TripleSum:
		case TermId::AntiSum:
		case TermId::AntiTripleSum:
			return "+";
		break;
		case TermId::ReflectionPoint:
			return "→";
		break;
		case TermId::Difference:
		case TermId::AntiReflectionPoint:
		case TermId::AntiDifference:
			return "-";
		break;

		case TermId::Triple:
		case TermId::AntiTriple:
			return position ? "-" : "+";
		break;

		default:
			assert( false );
		break;
	}
  return "<error>";
}
*/

/*****************************************************
**
**   MTerm   ---   toMString
**
******************************************************/
MString MTerm::toMString( const PlanetConfig &cfg ) const
{
	MString s;
	s << firstMString( cfg ) << getOperatorRepresentation() << secondMString( cfg );
	return s;
}

/*****************************************************
**
**   MTerm   ---   toMString
**
******************************************************/
MString MTerm::toMString( const Viewpoint viewpoint ) const
{
	MString s;
	s << firstMString( viewpoint ) << getOperatorRepresentation() << secondMString( viewpoint );
	return s;
}

/*****************************************************
**
**   MTerm   ---   operator==
**
******************************************************/
bool operator==( const MTerm &t1, const MTerm &t2 )
{
	if ( t1.getTermId() != t2.getTermId()) return false;
	if ( t1.getVariableCount() != t2.getVariableCount()) return false;

	for( int i = 0; i < MTerm::MAX_TERM_VARS; i++ )
	{
		if ( t1.obs[i] != t2.obs[i] ) return false;
	}
	return true;
}

/*****************************************************
**
**   MTerm   ---   operator <<
**
******************************************************/
mostream& operator<< ( mostream& stream, const MTerm &f )
{
	constexpr Viewpoint viewpoint = Viewpoint::Uranian;
	stream << f.firstMString( viewpoint ) << f.getOperatorRepresentation() << f.secondMString( viewpoint );
	return stream;
}

/*****************************************************
**
**   MTerm   ---   operator <<
**
******************************************************/
MString& operator<< ( MString &s, const MTerm &term )
{
	s << term.toMString();
	return s;
}

/*****************************************************
**
**   TermPath   ---   sort
**
******************************************************/
void TermPath::sort()
{
	steps.sort( []( const auto &s1, const auto &s2 ) { return s1.jd < s2.jd; });

}

