/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/UranianAspects.cpp
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

#include "astroconfig.h"
#include "ChartElementImpl.h"
#include "ContextProperties.h"
#include "DocHelper.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "GdiHelper.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "logger.h"
#include "mathbase.h"
#include "Painter.h"
#include "Uranian.h"

extern Gdiconfig *gdiconfig;

//#define DEBUG_URANIAN_ASPECTS

/*************************************************//**
*
*  Paint Uranian aspects. The match data is contained in the referenced chart.
*
******************************************************/
class UranianAspects : public ChartElement<CgDisk>
{
public:

	/*****************************************************
	**
	**   UranianAspects   ---   Constructor
	**
	******************************************************/
	UranianAspects( HybridChart *chart, CgDisk *disc )
	: ChartElement( chart, disc )
	{
		drawStackPosition = 1;
		doShowAllEvents = false;
		expert = new UranianExpert( chart->getProps(), chart->getUranianSection());
	}

	/*****************************************************
	**
	**   UranianAspects   ---   Destructor
	**
	******************************************************/
	~UranianAspects()
	{
		delete expert;
	}

	/*****************************************************
	**
	**   UranianAspects   ---   paint
	**
	******************************************************/
	void paint( Painter *painter, const MRect& /*refreshRect*/ ) override
	{
		debuglog << "UranianAspects::paint Start";
		//const Viewpoint viewpoint = chart->getViewpoint();

		if ( ! gdiconfig->uranianAspectStyle.showAspects ) return;

		const auto roles = chart->getChartRoles();
		if ( roles.size() == 0 ) return;

		CgDisk *disc = getGeom();
		if ( ! disc->show ) return;

		assert( chart );
		UranianSection *usection = expert->getUranianSection();
		assert( usection );

		if ( ! chart->getChartConfig()->showMatchesOnMouseOver ) return;

		// typedef std::pair<MTerm, HoroscopeRole> ActiveTerm;
		const ActiveTerm at = chart->getLeadingActiveTerm();

		HoroscopeRole firstRole = roles[0];
		HoroscopeRole secondRole = roles[0];
		if ( roles.size() > 1 )
		{
			if ( at.second == roles[0] )
			{
				// multitransit chart: if radix is active, aspected planet is in radix, too
				firstRole = roles.size() > 2 ? roles[0] : roles[1];
				secondRole = roles[0];
			}
			else //if ( at.second == roles[1] )
			{
				firstRole = roles[0];
				secondRole = at.second;
			}
		}

#ifdef DEBUG_URANIAN_ASPECTS
		debuglog << "UranianAspects::paint active term " << at.first << " role " << at.second
			<< " expert has " <<  expert->getMatchCount() << " events, doShowAllEvents " << std::boolalpha << doShowAllEvents
			<< " firstRole " << firstRole << " secondRole " << secondRole;
#endif

		Horoscope_ptr h1 = chart->getHoroscope( firstRole );
		Horoscope_ptr h2 = chart->getHoroscope( secondRole );
		if ( ! h1 || ! h2 ) return;

		if ( ! expert->getMatchCount() || at != lastMatchTerm )
		{
#ifdef DEBUG_URANIAN_ASPECTS
			debuglog << "UranianAspects::paint update uranian matches. lastMatchTerm "
				<< lastMatchTerm.first << "/" << lastMatchTerm.second;
#endif

			expert->clear();
			expert->findMatches( h2, h1, usection->orbis );

			lastMatchTerm = at;
			chart->setLeadingActiveTerm( at );
			chart->setActiveTerms( { at } );
#ifdef DEBUG_URANIAN_ASPECTS
			debuglog << "UranianAspects::paint AFTER update: expert has now " << expert->getMatchCount() << " matches";
			debuglog << "UranianAspects::paint chart has " << chart->getActiveTerms().size() << " active terms";
#endif

		}

		if ( doShowAllEvents )
		{
			for( const Match &match : expert->getMatches())
			{
				assert( match.e1->formula );
				assert( match.e2->formula );
				drawMatch( painter, h1, match );
			}
		}
		else if ( at.first.getTermId() != TermId::None )
		{
#ifdef DEBUG_URANIAN_ASPECTS
			debuglog << "UranianAspects::paintAspects found active: " << at.first;
#endif

			for( const Match &match : expert->getMatches())
			{
				assert( match.e1->formula );
				assert( match.e2->formula );

				if ( at.first == *match.e1->formula
					|| ( match.e1->formula->isSingleTerm() && match.e2->formula->isSingleTerm() && at.first == *match.e2->formula )
					)
				{
#ifdef DEBUG_URANIAN_ASPECTS
					debuglog << "UranianAspects::paintAspects found: [first = e1] "
						<< at.first << " ---- " << *match.e1->formula << " = " << *match.e2->formula
						<< " division " << match.division;
#endif

					if ( match.division == 1 && match.e2->formula->isSingleTerm())
					{
						drawD1Match( painter, h1, match );
					}
					else
					{
						drawMatch( painter, h1, match );
					}
				}
				else
				{
					debuglog << "UranianAspects::paintAspects no match "
						<< at.first << " ---- " << *match.e1->formula << " = " << *match.e2->formula;
				}
			}
		}
		debuglog << "UranianAspects::paintAspects finished";
	}

	/*****************************************************
	**
	**   UranianAspects   ---   onResponsiveChartEvent
	**
	******************************************************/
	virtual WidgetResponse onResponsiveChartEvent( const MComplex &p, const int keymod ) override
	{
		const ActiveTerm &oldActive = chart->getLeadingActiveTerm();
		const bool  oldDoShowAllEvents = doShowAllEvents;
		doShowAllEvents = keymod & KeyboardModfier::KbAlt;

		doShowAllEvents = keymod & KeyboardModfier::KbAlt;

#ifdef DEBUG_URANIAN_ASPECTS
		debuglog << "UranianAspects::onResponsiveChartEvent " << p
			<< " OLD ob " << oldActive.first << " OLD pointer " << oldActive.second << " keymod " << keymod
			<< " OLD element " << oldActive.second << " this " << this
			<< " doShowAllEvents " << doShowAllEvents;
#endif
		return oldDoShowAllEvents != doShowAllEvents;
	}

protected:

	/*****************************************************
	**
	**   UranianAspects   ---   drawD1Match
	**
	******************************************************/
	void drawD1Match( Painter *painter, Horoscope_ptr horoscope, const Match &match )
	{
		assert( match.e1 );
		assert( match.e2 );

		CgDisk *disc = getGeom();
		const Viewpoint viewpoint = chart->getViewpoint();

#ifdef DEBUG_URANIAN_ASPECTS
					debuglog << "TODO UranianAspects::drawD1Match "
						<< *match.e1->formula << " = " << *match.e2->formula
						<< " division " << match.division;
#endif

		// TODO
	}

	/*****************************************************
	**
	**   UranianAspects   ---   drawMatch
	**
	******************************************************/
	void drawMatch( Painter *painter, Horoscope_ptr horoscope, const Match &match )
	{
		assert( match.e1 );
		assert( match.e2 );

		CgDisk *disc = getGeom();
		const Viewpoint viewpoint = chart->getViewpoint();

#ifdef DEBUG_URANIAN_ASPECTS
					debuglog << "UranianAspects::drawMatch "
						<< *match.e1->formula << " = " << *match.e2->formula
						<< " division " << match.division;
#endif

		const MComplex proot = diskPointToXy( chart->getEffectiveLongitude( match.e1->longitude ), disc->radius, disc->geometryType );
		MComplex p1 = 0;
		MComplex p2 = 0;
		if ( match.e2->formula->isSingleTerm())
		{
			p1 = diskPointToXy( chart->getEffectiveLongitude( match.e2->longitude ), disc->radius, disc->geometryType );
		}
		else
		{
			const double a = chart->getEffectiveLongitude( horoscope->getLongitude( match.e2->formula->getFirstPlanet(), viewpoint ));
			p1 = diskPointToXy( a, disc->radius, disc->geometryType );
			const double b = chart->getEffectiveLongitude( horoscope->getLongitude( match.e2->formula->getSecondPlanet(), viewpoint ));
			p2 = diskPointToXy( b, disc->radius, disc->geometryType );
		}
		AspectPaintHelper( painter ).paintUranianAspect(
			match.e2->formula->getTermId(),
			proot,
			p1,
			p2,
			match.division,
			match.orbis );
	}

protected:

	UranianExpert *expert = nullptr;

	ActiveTerm lastMatchTerm = std::make_pair( MTerm( TermId::None ), HoroscopeRole::None );

	bool doShowAllEvents;
};

/*****************************************************
**
**   Grid element factory
**
******************************************************/
ChartElementBase *ChartElementFactory::createUranianAspects( CgElement *cge ) const
{
	CgDisk *disc = dynamic_cast<CgDisk*>( cge );
	assert( disc );
	return new UranianAspects( chart, disc );
}

