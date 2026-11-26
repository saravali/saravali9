/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/PlanetRing.cpp
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

#include "PlanetRing.h"

#include "ContextProperties.h"
#include "Direction.h"
#include "DocHelper.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "Formulary.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "mathbase.h"
#include "Painter.h"
#include "SymbolProvider.h"
#include "TermList.h"
#include "Uranian.h"

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define DEBUG_PLANETRING

/*****************************************************
**
**   PlanetRingItem   ---   Constructor
**
******************************************************/
PlanetRingItem::PlanetRingItem( PlanetRing *ring, const HoroscopeRole &role, const MTerm &term, const double &longitude )
: ring( ring ),
	role( role ),
	term( term ),
	longitude( longitude )
{
	correctedLongitude = longitude;
	// TEST
	//correctedLongitude = longitude + 3;
	//PlanetConfig cfg( astroconfig->getViewpointConfig( Viewpoint::Uranian ).planets );
	//tf << term.toMString( cfg ) << MToken( MovingDirection::Retrograde, TextFormat::Symbol );

	assert( term.getVariableCount() > 0 );

	auto *geom = ring->getGeom();
	if ( term.isSingleTerm() )
	{
		const PlanetConfig &cfg = astroconfig->getViewpointConfig( ring->getChart()->getViewpoint() ).planets;
		isSymbol = SymbolProvider().getPlanetCode( term.getPlanetId( 0 ), cfg.uranusSymbol, cfg.plutoSymbol ) != SYMBOL_CODE_ERROR;
	}
	else
	{
		isSymbol = false;
	}
	//baseFontSize = isSymbol ? scale->labelSize : 100;
	if ( isSymbol )
	{
		align = Align::Center;
		baseFontSize = geom->labelSize;
	}
	else
	{
		align = ( longitude > 90 && longitude < 270 ) ? Align::West : Align::East;
		//baseFontSize = 100;
		baseFontSize = 70;
	}

#ifdef DEBUG_PLANETRING
	debuglog << "PlanetRingItem constructor tf " << tf << " longitude " << longitude << " role " << role << " pos " << point
		<< " symbol " << isSymbol << " align " << align;
#endif
	
	writeText();
}

/*****************************************************
**
**   PlanetRingItem   ---   writeText
**
******************************************************/
void PlanetRingItem::writeText()
{
#ifdef DEBUG_PLANETRING
	debuglog << "GraphicalChartItem::writeText " << term << " longitude " << longitude;
#endif
	HybridChart *chart = ring->getChart();
	const Viewpoint &viewpoint = chart->getViewpoint();
	ChartConfig *chartCfg = chart->getChartConfig( chart->getWidgetCategory());
	const auto & calcCfg = astroconfig->getViewpointConfig( viewpoint ).calculation;

	PlanetConfig cfg( astroconfig->getViewpointConfig( viewpoint ).planets );
	cfg.textFormat = chartCfg->textFormat;
	//debuglog << "Text format " << cfg.textFormat;

	Horoscope_ptr horoscope = chart->getHoroscope( role );
	if ( ! horoscope ) return;

	const MComplex baseSize( 12, 12 );
	rect = MRect( point.real() - baseSize.real(), point.imag() - baseSize.imag(), 2 * baseSize.real(), 2 * baseSize.imag());

	/* conditions for display of retrogression 
	*  - chartCfg->retroStyle must be set to display retrogression
	*  - term must be single term
	*  - planet (i.e. first term  must be retrograde)
	*  - planet must not be mean node
	*
	*
	*/
	const bool retro =
		chartCfg->retroStyle != RetroDisplayStyle::Nothing
		&& term.isSingleTerm()
		&& horoscope->isRetrograde( term.getFirstPlanet(), viewpoint )
		&& ( ! PlanetHelper::isNode( term.getFirstPlanet()) || calcCfg.lunarNodeMode == LunarNodeMode::True );

	if ( retro )
	{
		switch( chartCfg->retroStyle )
		{
			case RetroDisplayStyle::SimpleR: // planet R
				tf << term.toMString( cfg ) << MToken( MovingDirection::Retrograde );
			break;
			case RetroDisplayStyle::Brackets: // (planet)
				tf << "(" << term.toMString( cfg ) << ")";
			break;
			case RetroDisplayStyle::Symbol:
			default:
				//tf = term.toMString( cfg );
				tf << term.toMString( cfg ) << MToken( MovingDirection::Retrograde, TextFormat::Symbol );
			break;
		}
	}
	else
	{
		tf = term.toMString( cfg );
	}
}

/*****************************************************
**
**   PlanetRingItem   ---   format
**
******************************************************/
void PlanetRingItem::format( Painter *painter )
{
#ifdef DEBUG_PLANETRING
	debuglog << "PlanetRingItem::format " << term << " longitude " << longitude << " isSymbol " << isSymbol;
#endif

	HybridChart *chart = ring->getChart();
	//ChartConfig *cfg = chart->getChartConfig( chart->getWidgetCategory());
	DocHelper util( painter );

	CgLabelledAnnulus *geom = ring->getGeom();
	assert( geom );
	//double fontsize = isSymbol ? geom->labelSize : 200;

	Horoscope_ptr horoscope = chart->getHoroscope( role );
	if ( ! horoscope ) return;

#ifdef DEBUG_PLANETRING
	debuglog << "PlanetRingItem::format term " << term << " isSymbol " << isSymbol
		<< " active count " << chart->getActiveTerms().size();
#endif

	chart->setGraphicFont( painter, baseFontSize );
	util.formatAndFitTextItem( *this );

	if( width <= 0 || height <= 0 ) return;

	double xleft = point.real();
	if ( align & Align::HCenter ) xleft -= width / 2;
	else if ( align & Align::Right ) xleft -= width;
	rect = MRect( xleft, point.imag() - height / 2, width, height );
	util.calculateTextAlign( *this );
}

/*****************************************************
**
**   PlanetRingItem   ---   paint
**
******************************************************/
void PlanetRingItem::paint( Painter *painter )
{
#ifdef DEBUG_PLANETRING
	debuglog << "PlanetRingItem::paint " << term << " longitude " << longitude;
#endif
	assert( painter );

	// TODO falsch wird sonst falsch gezeichnet
	//format( painter );

	HybridChart *chart = ring->getChart();
	ChartConfig *cfg = chart->getChartConfig( chart->getWidgetCategory());
	DocHelper util( painter );

// paint a rectangle around the item (dev only)
#if 0
	painter->setTransparentBrush();
	painter->setPen( *wxBLUE );
	painter->drawRectangle( rect );
	
	painter->drawLine( rect.x, rect.y, rect.x + rect.width, rect.y + rect.height );
	painter->drawLine( rect.x, rect.y + rect.height, rect.x + rect.width, rect.y );

#endif
	double fontsize = baseFontSize;

	// font size for zoomed items
	if ( cfg->zoomOnMouseOver 
		&& chart->isActiveTerm( std::make_pair( term, role )))
	{
		fontsize *= .01 * cfg->zoomFactor;
	}
#ifdef DEBUG_PLANETRING
	debuglog << "PlanetRingItem::format " << term << " fontsize " << fontsize
		<< " active " << chart->getLeadingActiveTerm().first;
#endif

	//baseFontSize = fontsize;
	chart->setGraphicFont( painter, fontsize );

	//chart->setGraphicFont( painter, baseFontSize );
	if ( chart->getChartConfig( chart->getWidgetCategory() )->showPlanetColors && term.isSingleTerm() )
		{ painter->setTextColor( gdiconfig->chartColors.getPlanetColor( term.getFirstPlanet() )); }
	else
	{ painter->setTextColor( ring->getTextColor()); }

	util.drawTextItem( *this );
}

/*****************************************************
**
**   PlanetRing   ---   Constructor
**
******************************************************/
PlanetRing::PlanetRing( HybridChart *chart, CgPlanetRing *a )
	: ChartElement( chart, a ),
	chartRoles( chart->getChartRoles())
{
	CgPlanetRing *geom = getGeom();
	assert( geom );
	reverseFirstRole = geom->rLabels < geom->radius;
}

/*****************************************************
**
**   PlanetRing   ---   Destructor
**
******************************************************/
PlanetRing::~PlanetRing()
{
	clearItems();
}

/*****************************************************
**
**   PlanetRing   ---   onCoordinatesChanged
**
******************************************************/
void PlanetRing::onCoordinatesChanged()
{
	const Viewpoint viewpoint = chart->getViewpoint();
	CgPlanetRing *geom = getGeom();
	assert( geom );
	const ChartShape shape = geom->geometryType;
	UranianSection *usection = chart->getUranianSection();

	clearItems();
	formatRequired = true;

	// handle shifted charts
	std::vector<double> shiftValues;
	for( uint i = 0; i < chartRoles.size(); i++ )
	{
		const auto role = chartRoles[i];
		double v = 0;
		if ( usection  && usection->enablePlanetShift )
		{
			const auto a = usection->shiftPlanetIds.find( role );
			if ( a != usection->shiftPlanetIds.end())
			{
				v = chart->getHoroscope( role )->getLongitude( a->second, viewpoint );
			}
		}
		shiftValues.push_back( v );
	}
	for( uint i = 0; i < chartRoles.size(); i++ )
	{
		const auto role = chartRoles[i];
		Horoscope_ptr h = chart->getHoroscope( role );
		if ( ! h ) continue;

		for( auto &term : createTerms( role ))
		{
			const double longitude = chart->getEffectiveLongitude( term.evaluate( h, viewpoint ) - shiftValues[ i ] );
			const MComplex p = diskPointToXy( longitude, getLabelRadiusForRoleIndex( i ), shape );
#ifdef DEBUG_PLANETRING
	debuglog << "PlanetRing::onCoordinatesChanged role " << role << " longitude " << longitude
		<< " radius " << getLabelRadiusForRoleIndex( i ) << " p " << p;
#endif
			items[role].push_back( new PlanetRingItem( this, role, term, longitude ));
		}
		adjustLongitudes( i );
	}
}
/*
	Idee:
	1. liste mit Elementsn, die die Zeiger auf die Items enthalten
	2. durchiterieren: wenn der nachbar besetzt ist, versucht man, beide items zu verschieben (Achtung Übergang 0/360)
	3. evtl mehrere durchgänge

*/

struct RingCorrectionRuler
{
	RingCorrectionRuler()
	{
		ruler.fill( 0 );
	}

	PlanetRingItem *getNextItem( const int &i )
	{
		assert( i >= 0 && i < 360 );
		if ( i < 359 ) return ruler[i+1];
		return ruler[0];
	}

	PlanetRingItem *getPreviousItem( const int &i )
	{
		assert( i >= 0 && i < 360 );
		if ( i > 0 ) return ruler[i-1];
		return ruler[359];
	}

	// TODO
	void shiftItemUp();
	void shiftItemDown();

	std::array<PlanetRingItem*, 360> ruler;

};

/*****************************************************
**
**   PlanetRing   ---   adjustLongitudes
**
******************************************************/
void PlanetRing::adjustLongitudes( const uint &id )
{
	CgPlanetRing *geom = getGeom();
	assert( geom );
	//const ChartShape shape = geom->geometryType;

	const HoroscopeRole role = chartRoles[id];
	std::list<PlanetRingItem*> &roleItems = items[role];
	const double radius = getLabelRadiusForRoleIndex( id );
#ifdef DEBUG_PLANETRING
	debuglog << "PlanetRing::adjustLongitudes role " << role << " radius " << radius;
#endif

	// TODO correctedLongitude nach Füllstand berechnen

	for( auto &item : roleItems )
	{
	 	item->point = diskPointToXy( item->correctedLongitude, radius, geom->geometryType );
	}
	
}

/*****************************************************
**
**   PlanetRing   ---   paint
**
******************************************************/
void PlanetRing::paint( Painter *painter, const MRect& /*refreshRect*/ )
{
	CgPlanetRing *geom = getGeom();
	assert( geom );
	if ( ! geom->show ) return;
	const ChartShape shape = geom->geometryType;
	DocHelper util( painter );

#ifdef DEBUG_PLANETRING
	debuglog << "PlanetRing::paint radius " << geom->radius << " inner " << geom->innerRadius << " rLabels " << geom->rLabels
		<< " reverseFirstRole " << std::boolalpha << reverseFirstRole;
#endif

	setInheritedStyle( painter );
	const wxPen defaultPen = getPen();
	const wxPen dotPen( painter->getPenColor(), 1,  wxPENSTYLE_DOT );
	painter->setPen( defaultPen );
	double p1radius = 0;
	double p2radius = 0;

	for( uint i = 0; i < chartRoles.size(); i++ )
	{
		const HoroscopeRole role = chartRoles[i];
		const double radius = getLabelRadiusForRoleIndex( i );

		// paint disk as level delimiter
		// TODO remove last disk
		//if ( ( i > 0 || ! reverseFirstRole ) && i < chartRoles.size() - 1 )
		if ( i > 0 || ! reverseFirstRole )
		{
			painter->setPen( dotPen );
			paintDisk( painter, getOuterRadiusForRoleIndex( i ));
			painter->setPen( defaultPen );
		}

		for( PlanetRingItem *item : items[ role ] )
		{
			p1radius = geom->radius;
			p2radius = geom->radius + geom->markerLength;
			if ( i > 1 )
			{
				// buckle in marker begins on outer circle of last role
				p2radius = getOuterRadiusForRoleIndex( i - 1 );
			}
			// planets inside for first role (radix)
			if ( reverseFirstRole && role == chart->getFirstRole() )
			{
				p1radius = geom->innerRadius;
				p2radius = geom->innerRadius - geom->markerLength;
			}

			const MComplex p0 = diskPointToXy( item->longitude, p1radius, shape );
			const MComplex p1 = diskPointToXy( item->longitude, p2radius, shape );
			const MComplex p2 = diskPointToXy( item->correctedLongitude, radius, shape );
#ifdef DEBUG_PLANETRING
	debuglog << "PlanetRing::paint role " << role << " p1radius " << p1radius << " p2radius " << p2radius;
#endif
			painter->drawLine( p0, p1 );
			painter->drawLine( p1, p2 );

			if ( formatRequired )
			{
				item->format( painter );
			}
#ifdef DEBUG_PLANETRING
			debuglog << "PlanetRing::paint item " << item->term << " longitude " << item->correctedLongitude
				<< " point " << item->point << " rect " << item->rect << " tf " << item->tf;
#endif

			//painter->drawEllipse(  item->point.real(), item->point.imag(), 10, 10 );
			// TODO das ist falsch. erst am Ende malen, nach den strichen
			// TODO wohl nicht für inneren bereich wegen House Colors usw
			#if 0
			painter->setTransparentPen();
			painter->setBrush( chart->getSkinnedBrush());
			painter->drawRectangle( item->rect );
			painter->setPen( chart->getSkinnedPen());
			#endif
			item->paint( painter );
		}
	}
	formatRequired = false;
	chart->setGraphicFont( painter );
}

/*****************************************************
**
**   PlanetRing   ---   getLabelRadiusForRoleIndex
**
******************************************************/
double PlanetRing::getLabelRadiusForRoleIndex( const uint &id )
{
	assert( id < chartRoles.size());

	CgPlanetRing *geom = getGeom();
	assert( geom );
	double radius = 100.0;

	if ( reverseFirstRole )
	{
		if ( id > 0 )
		{
			// delta that has to added to the base radius
			const double delta = geom->innerRadius - geom->rLabels;
			//const double delta = geom->rLabels - geom->innerRadius;
			radius = geom->radius + delta + ( id - 1 ) * geom->nextOuterRadiusDelta;
		}
		else
		{
			radius = geom->rLabels;
		}
	}
	else
	{
		radius = geom->rLabels + id * geom->nextOuterRadiusDelta;
	}

#ifdef DEBUG_PLANETRING
			debuglog << "PlanetRing::getLabelRadiusForRoleIndex id " << id << " reverseFirstRole " << std::boolalpha << reverseFirstRole
				<< " result " << radius;
#endif
	return radius;
}

/*****************************************************
**
**   PlanetRing   ---   getOuterRadiusForRoleIndex
**
******************************************************/
double PlanetRing::getOuterRadiusForRoleIndex( const uint &id )
{
	if ( id >= chartRoles.size()) return 0;

	CgPlanetRing *geom = getGeom();
	assert( geom );
	//reverseFirstRole = geom->rLabels < geom->innerRadius;
	double radius = 100.0;
	if ( reverseFirstRole )
	{
		if ( id == 0 )
		{
			radius = geom->innerRadius;
		}
		else
		{
			radius = geom->radius + id * geom->nextOuterRadiusDelta;
		}
	}
	else
	{
		return geom->radius + ( id + 1 ) * geom->nextOuterRadiusDelta;
	}
#ifdef DEBUG_PLANETRING
			debuglog << "PlanetRing::getOuterRadiusForRoleIndex id " << id << " reverseFirstRole " << std::boolalpha << reverseFirstRole
				<< " result " << radius;
#endif
	return radius;
}

/*****************************************************
**
**   PlanetRing   ---   getOuterRadius
**
******************************************************/
double PlanetRing::getOuterRadius()
{
	CgPlanetRing *geom = getGeom();
	assert( geom );
	if ( chartRoles.size() > 1 )
	{
		return getOuterRadiusForRoleIndex( chartRoles.size() - 1 );
	}
	return 100.0;
}

/*****************************************************
**
**   PlanetRing   ---   onResponsiveChartEvent
**
******************************************************/
WidgetResponse PlanetRing::onResponsiveChartEvent( const MComplex &p, const int keymod )
{
	const ActiveTerm &oldActive = chart->getLeadingActiveTerm();

	//typedef std::pair<MTerm, TermId> ActiveTerm;
#ifdef DEBUG_PLANETRING
	debuglog << "PlanetRing::onResponsiveChartEvent " << p
		<< " OLD " << oldActive.first << " keymod " << keymod
		<< " OLD element " << oldActive.second << " this " << this;
#endif

	// find out which item is under the mouse pointer
	for( const auto &a : items )
	{
		for( const auto &item : items[ a.first ]  )
		{
			if ( item->rect.containsPoint( p ))
			{
				MTerm &term = item->term;
#ifdef DEBUG_PLANETRING
				debuglog << "PlanetRing::onResponsiveChartEvent MATCH role " << a.first <<  " term " << term;
#endif

				// set active object only if changed (otherwise obsolete refresh
				if ( term != oldActive.first || a.first != oldActive.second )
				{
					chart->setLeadingActiveTerm( std::make_pair( term, a.first ));
#ifdef DEBUG_PLANETRING
					debuglog << "PlanetRing::onResponsiveChartEvent UPDATE REQUIRED role " << a.first <<  " term " << term;
#endif
					//return WidgetResponse( true, writeTooltip());
				}
				/*
				else
				{
					// term already active
					return false;
				}
				*/
				return WidgetResponse( true, writeTooltip());
			}
		}
	}
#ifdef DEBUG_PLANETRING
	debuglog << "PlanetRing::onResponsiveChartEvent NO MATCH";
#endif

	// active term must not be reset if it was set by another element.
	// Workaround: find out if the active element is not in this container 
	if ( oldActive.first != PlanetId::None && oldActive.second != HoroscopeRole::None )
	{
		for( const auto &item : items[ oldActive.second ] )
		{
			if ( item->term == oldActive.first )
			{
				chart->setLeadingActiveTerm( std::make_pair( TermId::None, HoroscopeRole::None ));
				return true;
			}
		}
	}
	return false;
}

/*****************************************************
**
**   PlanetRing   ---   writeTooltip
**
******************************************************/
wxString PlanetRing::writeTooltip() const
{
	const Viewpoint &viewpoint = chart->getViewpoint();
  PlanetPropertyConfig propcfg = astroconfig->getViewpointConfig( viewpoint ).planetProps;
	propcfg.textFormat = TextFormat::Long;
  PlanetConfig planetCfg = astroconfig->getViewpointConfig( viewpoint ).planets;
	planetCfg.textFormat = TextFormat::Long;

	wxString s;
	Formatter *formatter = Formatter::get();
	const ActiveTerm &at = chart->getLeadingActiveTerm();
	Horoscope_ptr h = chart->getHoroscope( at.second );
	MTerm term = at.first;

	s << enum_to_string( h->getRole()) << ": "
		<< term.toMString( planetCfg ).toWxString() << " "
		<< formatter->formatZodiacalLongitude(
			propcfg,
			term.evaluate( h, viewpoint ),
			term.isSingleTerm() && h->isRetrograde( term.getFirstPlanet(), viewpoint )
			? MovingDirection::Retrograde : MovingDirection::None
		).toWxString();
	return s;
}

/*****************************************************
**
**   PlanetRing   ---   clearItems
**
******************************************************/
void PlanetRing::clearItems()
{
	for( auto &a : items )
	{
		for( auto &item : items[a.first] ) { delete item; }
	}
	items.clear();
}
