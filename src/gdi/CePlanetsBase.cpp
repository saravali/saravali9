/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/CePlanetsBase.cpp
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

#include "CePlanetsBase.h"

#include "astroconfig.h"
#include "ChartGeometry.h"
#include "ContextProperties.h"
#include "DocHelper.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Formatter.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "Lang.h"
#include "mathbase.h"
#include "Painter.h"
#include "SymbolProvider.h"

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define DEBUG_CEPLANETS

/*****************************************************
**
**   GraphicalChartItem   ---   Constructor
**
******************************************************/
GraphicalChartItem::GraphicalChartItem( ChartElement<CgPlanetScale> *elem, const HoroscopeRole &role, const MComplex &point,
	const MTerm& term, const double &longitude )
	: DocItemText( Align::Center ),
	elem( elem ),
	role( role ),
	point( point ),
	term( term ),
	longitude( longitude )
{

	assert( term.getVariableCount() > 0 );

	CgPlanetScale *scale = elem->getCg();
	if ( term.isSingleTerm() )
	{
		extern Astroconfig *astroconfig;
		const PlanetConfig &cfg = astroconfig->getViewpointConfig( elem->getChart()->getViewpoint() ).planets;
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
		baseFontSize = scale->labelSize;
	}
	else
	{
		align = ( longitude > 90 && longitude < 270 ) ? Align::West : Align::East;
		//baseFontSize = 100;
		baseFontSize = 70;
	}

#ifdef DEBUG_CEPLANETS
	debuglog << "GraphicalChartItem constructor tf " << tf << " longitude " << longitude << " role " << role << " pos " << point
		<< " symbol " << isSymbol << " align " << align;
#endif
	
	writeText();
}

/*****************************************************
**
**   GraphicalChartItem   ---   writeText
**
******************************************************/
void GraphicalChartItem::writeText()
{
#ifdef DEBUG_CEPLANETS
	debuglog << "GraphicalChartItem::writeText " << term << " longitude " << longitude;
#endif
	HybridChart *chart = elem->getChart();
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
**   GraphicalChartItem   ---   format
**
******************************************************/
void GraphicalChartItem::format( Painter *painter )
{
#ifdef DEBUG_CEPLANETS
	debuglog << "GraphicalChartItem::format " << term << " longitude " << longitude << " isSymbol " << isSymbol;
#endif

	HybridChart *chart = elem->getChart();
	ChartConfig *cfg = chart->getChartConfig( chart->getWidgetCategory());
	DocHelper util( painter );

	CgPlanetScale *scale = elem->getCg();
	assert( scale );
	//double fontsize = isSymbol ? scale->labelSize : 100;
	//double fontsize = isSymbol ? scale->labelSize : 200;
	//gdiconfig->fonts.symbolFontSize

	Horoscope_ptr horoscope = chart->getHoroscope( role );
	if ( ! horoscope ) return;

#ifdef DEBUG_CEPLANETS
	debuglog << "GraphicalChartItem::format term " << term << " ZOOOM " << cfg->zoomOnMouseOver
		<< " isSymbol " << isSymbol
		//<< " active " << chart->isActiveTerm( std::make_pair( term, chart_id ))
		<< " active count " << chart->getActiveTerms().size();
#endif

	double fontsize = baseFontSize;

	// font size for zoomed items
	if ( cfg->zoomOnMouseOver 
		&& chart->isActiveTerm( std::make_pair( term, role )))
	{
		fontsize *= .01 * cfg->zoomFactor;
	}
#ifdef DEBUG_CEPLANETS
	debuglog << "GraphicalChartItem::format " << term << " fontsize " << fontsize
		<< " active " << chart->getLeadingActiveTerm().first;
#endif

	chart->setGraphicFont( painter, fontsize );
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
**   GraphicalChartItem   ---   paint
**
******************************************************/
void GraphicalChartItem::paint( Painter *painter )
{
#ifdef DEBUG_CEPLANETS
	debuglog << "GraphicalChartItem::paint " << term << " longitude " << longitude;
#endif
	assert( painter );

	// TODO falsch wird sonst falsch gezeichnet
	//format( painter );

	HybridChart *chart = elem->getChart();
	DocHelper util( painter );

// paint a rectangle around the item (dev only)
#if 0
	painter->setTransparentBrush();
	painter->setPen( *wxBLUE );
	painter->drawRectangle( rect );
	
	painter->drawLine( rect.x, rect.y, rect.x + rect.width, rect.y + rect.height );
	painter->drawLine( rect.x, rect.y + rect.height, rect.x + rect.width, rect.y );

#endif

	if ( chart->getChartConfig( chart->getWidgetCategory() )->showPlanetColors && term.isSingleTerm() )
		{ painter->setTextColor( gdiconfig->chartColors.getPlanetColor( term.getFirstPlanet() )); }
	else
	{ painter->setTextColor( elem->getTextColor()); }

	util.drawTextItem( *this );
}

/*****************************************************
**
**   CePlanetsBase   ---   Constructor
**
******************************************************/
CePlanetsBase::CePlanetsBase( HybridChart *chart, CgPlanetScale *a )
	: ChartElement( chart, a ),
	chartRoles( chart->getChartRoles())
{
}

/*****************************************************
**
**   CePlanetsBase   ---   Destructor
**
******************************************************/
CePlanetsBase::~CePlanetsBase()
{
	clearItems();
}

/*****************************************************
**
**   CePlanetsBase   ---   clearItems
**
******************************************************/
void CePlanetsBase::clearItems()
{
	for( auto &a : citems )
	{
		for( auto &item : citems[a.first] ) { delete item; }
	}
	citems.clear();
}

/*****************************************************
**
**   CePlanetsBase   ---   addItem
**
******************************************************/
void CePlanetsBase::addItem( ChartElement<CgPlanetScale> *elem, const HoroscopeRole &role, const MComplex &point,
	const MTerm& term, const double &longitude )
{
	citems[role].push_back( new GraphicalChartItem( elem, role, point, term, longitude ));
}

/*****************************************************
**
**   CePlanetsBase   ---   paint
**
******************************************************/
void CePlanetsBase::paint( Painter *painter, const MRect& /*refreshRect*/ )
{
	CgPlanetScale *scale = getCg();
	if ( ! scale->show ) return;
	setInheritedStyle( painter );
	painter->setPen( getPen() );

	for( const auto &a : citems )
	{
		painter->setPen( chart->getSkinnedPen());

		const auto [ radius, rLabels ] = scale->getParams4ChartId( getRoleIndex( a.first ));
		const CgDisc disc( CgElementType::Disc, radius, scale->geometryType );
		//paintDisc( painter, disc );

		if (  scale->markerPen.IsOk()) painter->setPen( scale->markerPen );

		for( const auto &item : a.second )
		{
			const double longitude = item->getLongitude();
#ifdef DEBUG_CEPLANETS
			debuglog << "CePlanetsBase::paint " << item->getTerm() << " role " << a.first << " id " << getRoleIndex( a.first )
				<< " rect " << item->rect << " radius " << radius << " rLabels " << rLabels;
#endif
			item->format( painter );

			if ( doPaintMarker )
			{
				paintDisc( painter, disc );
				paintMarkerLine( painter, longitude, disc );
				const MComplex p1 = discPointToXy( longitude, radius - scale->markerLength, scale->geometryType );
				const double p2y = item->rect.y + .5 * item->rect.height;

				double p2x = item->rect.x;
				if ( item->align & Align::HCenter ) p2x += .5 * item->rect.width;
				else if ( item->align & Align::Right ) p2x += item->rect.width;

				painter->drawLine( p1, MComplex( p2x, p2y ));
			}

			item->paint( painter );
		}
	}
}

/*****************************************************
**
**   CePlanetsBase   ---   drawAnglesRequired
**
******************************************************/
bool CePlanetsBase::drawAnglesRequired( const HoroscopeRole &role ) const
{
	const ChartGeometry &geom = chart->getGeometry();
	const bool hasAnglesElement  = geom.containsElementType( CgElementType::Angles );

	return ( hasAnglesElement && role != chart->getFirstRole() );
}

/*****************************************************
**
**   CePlanetsBase   ---   onResponsiveChartEvent
**
******************************************************/
WidgetResponse CePlanetsBase::onResponsiveChartEvent( const MComplex &p, const int keymod )
{
	const ActiveTerm &oldActive = chart->getLeadingActiveTerm();

	//typedef std::pair<MTerm, TermId> ActiveTerm;
#ifdef DEBUG_CEPLANETS
	debuglog << "CePlanetsBase::onResponsiveChartEvent " << p
		<< " OLD " << oldActive.first << " keymod " << keymod
		<< " OLD element " << oldActive.second << " this " << this;
#endif

	// find out which item is under the mouse pointer
	for( const auto &a : citems )
	{
		for( const auto &item : citems[ a.first ]  )
		{
			if ( item->rect.containsPoint( p ))
			{
				MTerm term = item->getTerm();
#ifdef DEBUG_CEPLANETS
				debuglog << "CePlanetsBase::onResponsiveChartEvent MATCH role " << a.first <<  " term " << term;
#endif

				// set active object only if changed (otherwise obsolete refresh
				if ( term != oldActive.first || a.first != oldActive.second )
				{
					chart->setLeadingActiveTerm( std::make_pair( term, a.first ));
#ifdef DEBUG_CEPLANETS
					debuglog << "CePlanetsBase::onResponsiveChartEvent UPDATE REQUIRED role " << a.first <<  " term " << term;
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
#ifdef DEBUG_CEPLANETS
	debuglog << "CePlanetsBase::onResponsiveChartEvent NO MATCH";
#endif

	// active term must not be reset if it was set by another element.
	// Workaround: find out if the active element is not in this container 
	if ( oldActive.first != PlanetId::None && oldActive.second != HoroscopeRole::None )
	{
		for( const auto &item : citems[ oldActive.second ] )
		{
			if ( item->getTerm() == oldActive.first )
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
**   CePlanetsBase   ---   writeTooltip
**
******************************************************/
wxString CePlanetsBase::writeTooltip() const
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
**   CePlanetsBase   ---   getCoordinatesForItem
**
******************************************************/
MComplex CePlanetsBase::getCoordinatesForItem( const HoroscopeRole &role, const double &glen )
{
	double dist, l;
	const int factor = 1;
	double delta = 0;
	//const double psize = chart->getRMax() / 60;
	const double psize = chart->getRMax() / 100;
	CgPlanetScale *scale = getCg();
	assert( scale );

	auto [ radius, rLabels ] = scale->getParams4ChartId( getRoleIndex( role ));

#ifdef DEBUG_CEPLANETS
	debuglog << "CePlanetsBase::getCoordinatesForItem role " << role << " glen " << glen << " radius orig "
		<< scale->radius << " rLabels orig " << scale->rLabels 
		<< " new radius " << radius << " new rLabels " << rLabels << " rect " << chart->getRect();;
#endif

	const bool isCentrifugal = radius < rLabels;
	for( const auto &item : citems[ role ] )
	{
		l = item->getLongitude();
		dist = ::fabs( glen - l );
		if ( dist > 300 ) dist = fabs( dist - 360 );

#ifdef DEBUG_CEPLANETS
		debuglog << "CePlanetsBase::getCoordinatesForItem glen " << glen
			<< " ref " << item->tf << " l " << l << " dist " << dist;
#endif
		//if ( dist < 5 )
		if ( dist < 3 )
		{
			delta = std::min( factor * psize, factor * psize * ( 5 - dist)/5 );
			//debuglog << "MATCH " << (int)planet << " p " << (int)p << " dist " << dist;
			if ( isCentrifugal )
				rLabels += delta;
			else
				rLabels -= delta;
		}
	}
	return discPointToXy( glen, rLabels, scale->geometryType );
}

/*****************************************************
**
**   CePlanetsBase   ---   getModuloLongitude
**
******************************************************/
double CePlanetsBase::getModuloLongitude( const double &l, const int division )
{
	assert( division > 0 );
	return division * d_red( l + chart->getAries(), division );
}

/*****************************************************
**
**   CePlanetsBase   ---   getRoleIndex
**
******************************************************/
int CePlanetsBase::getRoleIndex( const HoroscopeRole &role ) const
{
	int id = 0;
	switch( role )
	{
		case HoroscopeRole::Radix:
		case HoroscopeRole::Partner1:
		case HoroscopeRole::Composite:
		break;
		case HoroscopeRole::Partner2:
			id = 1;
		break;
		default:
		{
			ContextProps_ptr props = chart->getProps();
			for( const auto &r : props->getActiveRoles())
			{
				if ( r == role ) break;
				id++;
			}
		}
		break;
	}

#ifdef DEBUG_CEPLANETS
	debuglog << "CePlanetsBase::getRoleIndex role " << role << " index " << id;
#endif
	return id;
}

/*****************************************************
**
**   CePlanetsBase   ---   paintDegreeScales
**
******************************************************/
void CePlanetsBase::paintDegreeScales( Painter *painter, const MRect& /* refreshRect */, const int division )
{
	CgPlanetScale *scale = getCg();
	assert( scale );

	const ChartShape shape = scale->geometryType;
	std::list<Dmt> rule;
	double stepWidth = 1.0;

	if ( division < 16 )
	{
		rule = { Dmt::t1, Dmt::t1, Dmt::t1, Dmt::t1, Dmt::t5, Dmt::t1, Dmt::t1, Dmt::t1, Dmt::t1, Dmt::t10 };
	}
	else
	{
		//rule = { Dmt::t1, Dmt::t1, Dmt::t1, Dmt::t1, Dmt::t1, Dmt::t1 };
		rule = { Dmt::t1, Dmt::t1, Dmt::t5, Dmt::t1, Dmt::t1, Dmt::t10 };
		stepWidth = 1.0 / 6.0;
		/*
		rule = { Dmt::t1, Dmt::t1, Dmt::t1, Dmt::t1, Dmt::t1, Dmt::t10 };
		stepWidth = 1.0 / 6.0;
		*/
	}

	// paint degree marker for chart one even if this is not in the role list (i.e. empty chart)
	const double aries = chart->getAries();
	paintSingleScale( painter, scale->radius, division, shape, scale->firstScaleMarker, stepWidth, rule, aries );

	for( int id = 1; id < (int)chart->getChartRoles().size(); id++ )
	{
		if( id == 1 )
		{
			paintSingleScale( painter, scale->radius + scale->secondaryScaleOffset, division, shape,
				scale->secondaryScaleMarker, stepWidth, rule, aries );
		}
		else
		{
			paintSingleScale( painter, scale->getOuterRadius( id ) - scale->furtherScaleOffset, division,
				shape, scale->furtherScaleMarker, stepWidth, rule, aries );
		}
	}
}

/*****************************************************
**
**   CePlanetsBase   ---   paintSingleScale
**
******************************************************/
void CePlanetsBase::paintSingleScale( Painter *painter, const double &radius, const int &division, const ChartShape &shape,
	CgDegreeMarker &markerStyle, const double &stepWidth, const std::list<Dmt> &rule, const double &aries )
{
	assert( stepWidth > 0 );
	assert( rule.size() > 0 );

#ifdef DEBUG_CEPLANETS
	debuglog << "CePlanets::paintSingleScale radius " << radius << " division " << division
		<< " stepWidth " << stepWidth << " aries " << aries;
#endif

	double d = division * stepWidth;
	double lmarker = division;
	auto iter = rule.begin();
	wxPen pen = chart->getSkinnedPen();
	while( d <= 360 )
	{
		pen = chart->getSkinnedPen();
		switch( *iter )
		{
			case Dmt::t1:
				lmarker = markerStyle.wDegree1;
				softOverridePen( pen, markerStyle.pen1 );
				//if (  markerStyle.pen1.IsOk()) pen = markerStyle.pen1;
			break;
			case Dmt::t5:
				lmarker = markerStyle.wDegree5;
				softOverridePen( pen, markerStyle.pen5 );
				//if (  markerStyle.pen5.IsOk()) pen = markerStyle.pen5;
			break;
			case Dmt::t10:
			default:
				lmarker = markerStyle.wDegree10;
				softOverridePen( pen, markerStyle.pen10 );
				//if (  markerStyle.pen10.IsOk()) pen = markerStyle.pen10;
			break;
		};
		//debuglog << "CePlanets::paintSingleScale angle " << d;

		painter->setPen( pen );
		paintAnnulusConnector( painter, d + aries, radius, radius - lmarker, shape );
		painter->setPen( chart->getSkinnedPen());

		iter = std::ranges::next( iter );
		if ( iter == rule.end() ) iter = rule.begin();
		d += division * stepWidth;
	}

	//paintDisc( painter, CgDisc( CgElementType::Disc, radius - markerStyle.wDegree1, shape ));

	// finally draw circles if required
	if ( markerStyle.show1DegreeFrame )
	{
		paintDisc( painter, CgDisc( CgElementType::Disc, radius - markerStyle.wDegree1, shape ));
	}
	if ( markerStyle.show5DegreeFrame )
	{
		paintDisc( painter, CgDisc( CgElementType::Disc, radius - markerStyle.wDegree5, shape ));
	}
	if ( markerStyle.show10DegreeFrame )
	{
		paintDisc( painter, CgDisc( CgElementType::Disc, radius - markerStyle.wDegree10, shape ));
	}
}


