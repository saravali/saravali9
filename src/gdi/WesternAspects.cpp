/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/WesternAspects.cpp
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

#include "ChartElementImpl.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "GdiHelper.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "Painter.h"
#include "WesternAspect.h"

extern Gdiconfig *gdiconfig;

/*************************************************//**
*
*   Chart element showing aspects in Western charts. 
*
******************************************************/
class WesternAspects : public ChartElement<CgDisk>
{
public:

	/*****************************************************
	**
	**   WesternAspects   ---   Constructor
	**
	******************************************************/
	WesternAspects( HybridChart *chart, CgDisk *disc )
		: ChartElement( chart, disc )
	{
		drawStackPosition = 3;
	}

	/*****************************************************
	**
	**   WesternAspects   ---   paint
	**
	******************************************************/
	virtual void paint( Painter *painter, const MRect& /*refreshRect*/ ) override
	{
		WesternAspectConfig &wacfg = gdiconfig->westernAspects;
		if ( ! wacfg.showAspects ) return;

		const double aries = chart->getAries();
		ContextProps_ptr props = chart->getProps();
		ChartConfig *cfg = chart->getChartConfig( WidgetCategory::WesternChart );

		std::vector<HoroscopeRole> roles = chart->getChartRoles();
		debuglog << "WesternAspects::paint";
		for( const auto &role: roles )
		{
			debuglog << "WesternAspects::paint role " << role;
		}

		if ( roles.size() == 0 ) return; // no horoscope, nothing to do

		CgDisk *disc = getGeom();
		if ( ! disc->show ) return;

		if ( ! aspects.size() && chart->getFirstHoroscope())
		{
			setupAspects();
		}

		setInheritedStyle( painter );
		paintDisk( painter, disc );

		const ActiveTerm &activeTerm = chart->getLeadingActiveTerm();
		const PlanetId pActive = activeTerm.first.getTermId() == TermId::Single
			? activeTerm.first.getFirstPlanet() : PlanetId::None;

		// collect all aspecting/aspected degrees here
		std::set<double> endpoints;

		wxColour color = getTextColor();
		chart->setGraphicFont( painter );

		for ( const auto &event : aspects )
		{
			debuglog << "WesternAspects::paint type " << event.type.first;

			double zoom = 1;

			if ( cfg->highlightOnMouseOver )
			{
				if ( roles.size() == 1 )
				{
					// event matches whenever planet is part of the event
					if ( event.planet1 == pActive || event.planet2 == pActive )
						zoom = 2;
				}
				else // roles.size() must be at least two
				{
					// event matches only if the right horoscope is involved
					if (( event.planet1 == pActive
						&& chart->getHoroscope( activeTerm.second )
						&& chart->getHoroscope( activeTerm.second ) == chart->getFirstHoroscope())
					|| ( event.planet2 == pActive
						&& chart->getHoroscope( activeTerm.second ) == chart->getSecondHoroscope()))
						zoom = 2;
				}
			}

			const MComplex p1 = diskPointToXy( aries + event.l1, disc );
			const MComplex p2 = diskPointToXy( aries + event.l2, disc );
			debuglog << "Aspect " << (int)event.type.first << " aries " << aries << " l1 " << event.l1
				<< " l2 " << event.l2 << " p1 " << p1 << " p2 " << p2 ;
			AspectPaintHelper( painter ).paintWesternAspect( p1, p2, event.type, event.orbis, zoom );

			endpoints.insert( aries + event.l1 );
			endpoints.insert( aries + event.l2 );
		}
		setInheritedStyle( painter );

		for ( const double l : endpoints )
		{
			paintMarkerLine( painter, l, *disc );
		}
	}

	/*****************************************************
	**
	**   WesternAspects   ---   onResponsiveChartEvent
	**
	******************************************************/
	WidgetResponse onResponsiveChartEvent( const MComplex&, const int /*keymod*/ ) override
	{
		if ( ! chart->hasActiveTerm())
		{
			const ActiveTerm &at = chart->getLeadingActiveTerm();
			std::list<ActiveTerm> terms;

			// first active term is added
			terms.push_back( at );

			if ( at.first.getTermId() != TermId::None && at.first.isSingleTerm() && chart->getHoroscope( at.second ))
			{
				for ( const auto &event : aspects )
				{
					/*
					* Partner, transit and shifted radix have mixed relations.
					*/
					const bool useMixedAspects = chart->getSecondHoroscope() != nullptr;
#if 0
					debuglog << "WesternAspects::onResponsiveChartEvent term " << at.first
						<< " useMixedAspects " << useMixedAspects
						<< " event.planet1 " << (int)event.planet1
						<< " event.planet2 " << (int)event.planet2;
#endif

					if ( useMixedAspects )
					{
						// if the active planet belongs to radix chart it must match event.planet1
						if ( at.second == HoroscopeRole::Radix
							&& event.planet1 == at.first.getFirstPlanet() )
						{
							terms.push_back( std::make_pair( event.planet2, HoroscopeRole::Radix ));
						}
						// if active planet is from transitchart it must match planet2
						else if ( at.second == chart->getSecondHoroscope()->getRole()
							&& event.planet2 == at.first.getFirstPlanet() )
						{
							terms.push_back( std::make_pair( event.planet1, HoroscopeRole::Radix ));
						}
					}
					else
					{
						if ( at.first.getFirstPlanet() == event.planet1 )
						{
							terms.push_back( std::make_pair( event.planet2, HoroscopeRole::Radix ));
						}
						else if ( at.first.getFirstPlanet() == event.planet2 )
						{
							terms.push_back( std::make_pair( event.planet1, HoroscopeRole::Radix ));
						}
					}
				}
			}
			terms.unique();
			chart->setActiveTerms( terms );
			debuglog << "WesternAspects::onResponsiveChartEvent activeTerms size " << terms.size();
			return true;
		}
		else
		{
			return false;
		}
	}

protected:

	/*****************************************************
	**
	**   WesternAspects   ---   setupAspects
	**
	******************************************************/
	void setupAspects()
	{
		WesternAspectExpert expert( chart->getProps() );
		aspects = expert.calculate( chart->isCompositeChart() );
		expert.sort( aspects, EventSortOrder::TypeReverse );
		debuglog << "WesternAspects::setupAspects size " << aspects.size();
	}

	std::list<WesternAspectEvent> aspects;
};

/*****************************************************
**
**   Grid element factory 
**
******************************************************/
ChartElementBase *ChartElementFactory::createAspects( CgElement *cge ) const
{
	CgDisk *disc = dynamic_cast<CgDisk*>( cge );
	assert( disc );
	return new WesternAspects( chart, disc );
}

