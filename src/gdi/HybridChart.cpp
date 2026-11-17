/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/HybridChart.cpp
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

#include "HybridChart.h"

#include "astroconfig.h"
#include "ChartConfig.h"
#include "ChartElement.h"
#include "ChartGeometry.h"
#include "ConfigListItems.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "logger.h"
#include "constants.h"
#include "mathbase.h"
#include "Painter.h"

#include <algorithm>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

//#define DEBUG_HYBRIDCHART

/*****************************************************
**
**   HybridChart   ---   Constructor
**
******************************************************/
HybridChart::HybridChart( ContextProps_ptr props, const WidgetCategory &category, const WidgetCategory parentCategory )
	: GdiWidget( props, category ),
		varga( Varga::Rasi ),
		parentCategory( parentCategory )
{
	avtype = AvType::None;
	avobject = PlanetId::None;
	init();
}

/*****************************************************
**
**   HybridChart   ---   Constructor
**
******************************************************/
HybridChart::HybridChart( ContextProps_ptr props, const WidgetCategory &category, const Varga &varga )
	: GdiWidget( props, category ),
		varga( varga )
{
	avtype = AvType::None;
	avobject = PlanetId::None;
	init();
}

/*****************************************************
**
**   HybridChart   ---   Constructor
**
******************************************************/
HybridChart::HybridChart( ContextProps_ptr props, const WidgetCategory &category, UranianSection *ucfg )
	: GdiWidget( props, category, ucfg ),
		varga( Varga::Rasi )
{
	avtype = AvType::None;
	avobject = PlanetId::None;
	init();
}

/*****************************************************
**
**   HybridChart   ---   Constructor
**
******************************************************/
HybridChart::HybridChart( ContextProps_ptr props, AshtakavargaExpert *avexpert, const AvType &avtype,
	const PlanetId &ob )
	: GdiWidget( props, WidgetCategory::VargaChart ),
		avexpert( avexpert ),
		avtype( avtype ),
		avobject( ob ),
		varga( Varga::Rasi )
{
	init();
}

/*****************************************************
**
**   HybridChart   ---   Destructor
**
******************************************************/
HybridChart::~HybridChart()
{
	clearWidgets();
	if ( usection )
	{
		usection->offset = 0.0;
		usection->offsetToggle = false;
	}
}

/*****************************************************
**
**   HybridChart   ---   init()
**
******************************************************/
void HybridChart::init()
{
	activeTerms.clear();
	leadingActiveTerm = std::make_pair( MTerm( TermId::None ), HoroscopeRole::None );

	aries = 0.0;
}

/*****************************************************
**
**   HybridChart   ---   OnDataChanged
**
******************************************************/
void HybridChart::OnDataChanged()
{
#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::OnDataChanged rect " << rect
		<< " h0 " << getFirstHoroscope().get()
		<< " h1 " << getSecondHoroscope().get()
		<< "category " << category;
	if ( usection ) debuglog << "HybridChart::OnDataChanged division " << usection->chartDivision;
	if ( getFirstHoroscope()) debuglog << "HybridChart::OnDataChanged role "
		<< getFirstRole();
	if ( getSecondHoroscope()) debuglog << "HybridChart::OnDataChanged role "
		<< getSecondRole();
#endif

	calculateChartRoles();

	activeTerms.clear();
	leadingActiveTerm = std::make_pair( MTerm( TermId::None ), HoroscopeRole::None );

	clearWidgets();
	setupWidgets();
}

/*****************************************************
**
**   HybridChart   ---   doLayout
**
******************************************************/
MComplex HybridChart::doLayout( Painter *painter )
{
#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::doLayout chart has " << elements.size() << " elements";
#endif
	onCoordinatesChanged();

	return GdiWidget::doLayout( painter );
}

/*****************************************************
**
**   HybridChart   ---   onCoordinatesChanged
**
******************************************************/
void HybridChart::onCoordinatesChanged()
{
#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::onCoordinatesChanged size " << elements.size();
#endif
	aries = calculateAries();

#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::onCoordinatesChanged element size " << elements.size()
		<< " aries " << aries;
#endif

	for( auto &elem : elements )
	{
		elem->onCoordinatesChanged();
	}
}

/*****************************************************
**
**   HybridChart   ---   setupWidgets
**
******************************************************/
void HybridChart::setupWidgets()
{
	const ChartGeometry &geom = getGeometry();
#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::setupWidgets (1) element size: " << geom.elements.size();
#endif

	elements = ChartElementFactory( this ).createElements( geom );
#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::setupWidgets (2) element size: " << geom.elements.size();
#endif
}

/*****************************************************
**
**   HybridChart   ---   paintWidgets
**
******************************************************/
void HybridChart::paintWidgets( Painter *painter, const MRect& refreshRect )
{
#if 0
	debuglog << "HybridChart::paintWidgets " << elements.size() << " rect " << refreshRect;
	static int count = 0;
	debuglog << "---- HybridChart::paintWidgets() ---- " << elements.size();
#endif
	try
	{
	for( auto &elem : elements )
	{
#if 0
			debuglog << "HybridChart::paintWidgets() " << count++;
#endif
			elem->paint( painter, refreshRect );
		}
	}
	catch( std::exception &e )
	{
		errorlog << "Exception: " << e.what();
	}
}

/*****************************************************
**
**   HybridChart   ---   clearWidgets
**
******************************************************/
void HybridChart::clearWidgets()
{
	//debuglog << "HybridChart::clearWidgets " << elements.size() ;
	elements.clear();
}

/*****************************************************
**
**   HybridChart   ---   doPaint
**
******************************************************/
void HybridChart::doPaint( Painter *painter, const MRect& refreshRect )
{
#ifdef DEBUG_HYBRIDCHART
	StopWatch sw;
	WidgetCategory cat = getWidgetCategory();
	ChartGeometryLoader *loader = ChartGeometryLoader::get();

	debuglog << "START HybridChart::paint category " << cat
		<< " viewpoint " << (int)getViewpoint()
		<< " skin " << props->getSkin( cat )
		<< " geom " << props->getGeometry( cat )
		<< " geom name " << loader->getConfig( props->getGeometry( cat )).name
		<< " RECT " << rect
		<< " refreshRect " << refreshRect;
#endif

	const WidgetSkin &skin = getSkin();
	setGraphicFont( painter );

	paintWidgets( painter, refreshRect );

	if ( skin.textColor.IsOk() ) painter->setTextColor( skin.textColor );
	else painter->setDefaultTextColor();

#ifdef DEBUG_HYBRIDCHART
	debuglog << "FINISHED HybridChart::paint " << sw.getElapsedMillisec() << " millisec";
#endif
}

/*****************************************************
**
**   HybridChart   ---   setLeadingActiveTerm
**
******************************************************/
void HybridChart::setLeadingActiveTerm( const ActiveTerm &at )
{
#ifdef DEBUG_HYBRIDCHART
	debuglog << "Start HybridChart::setLeadingActiveTerm term " << at.first;
#endif
	leadingActiveTerm = at;
	activeTerms.clear();
}

/*****************************************************
**
**   HybridChart   ---   isActiveTerm
**
******************************************************/
bool HybridChart::isActiveTerm( const ActiveTerm &term ) const
{
#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::isActiveTerm " << term.first << " " << term.second << " activeTerms size " << activeTerms.size();
#endif
	
#if 0
	for( auto &a : activeTerms )
	{
		debuglog << "HybridChart::isActiveTerm() item " << a.first << " " << a.second;
	}
#endif

	for( const auto &so : activeTerms )
	{
		if ( so.first == term.first && props->getHoroscope( term.second ) && term.second == so.second )
		{
			return true;
		}
	}
	return false;
}

/*****************************************************
**
**   HybridChart   ---   setRect
**
******************************************************/
void HybridChart::setRect( const MRect &r )
{
#ifdef DEBUG_HYBRIDCHART
	debuglog << " HybridChart::setRect input " << r;
#endif

#if 0
	rect = r;

#else

	const double zoom = props->getChartZoom();
	const double w = zoom * r.width;
	const double h = zoom * r.height;
	//rect = MRect( r.x - .5 * w, r.y - .5 * h, w, h );
	rect = MRect( r.x, r.y, w, h );
#endif

	double decrease = 1.0;
	double maxradius = 90;

	const int lastRoleId = chartRoles.size() + 1;
	for( auto &elem : elements )
	{
#ifdef DEBUG_HYBRIDCHART
		debuglog << "HybridChart::setRect element radius " << elem->getOuterRadius( lastRoleId );
#endif
		maxradius = std::max( maxradius, elem->getOuterRadius( lastRoleId ));
	}

	//if ( maxradius > 100 ) decrease = 100.0 / ( maxradius - 20 );
	if ( maxradius > 100 ) decrease = 100.0 / maxradius;

	rxymax = MComplex( .5 * decrease * rect.width, .5 * decrease * rect.height );
	rmax = std::min( rxymax.real(), rxymax.imag() );
#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::setRect new radius " << rmax << " maxradius " << maxradius << " decrease " << decrease;
#endif

	center = rect.getCenter();
}

/*****************************************************
**
**   HybridChart   ---   getFirstRole
**
******************************************************/
HoroscopeRole HybridChart::getFirstRole() const
{
	if ( chartRoles.size() > 0 ) return chartRoles[0];
	else return HoroscopeRole::None;
}

/*****************************************************
**
**   HybridChart   ---   getFirstHoroscope
**
******************************************************/
Horoscope_ptr HybridChart::getFirstHoroscope() const
{
	return props->getHoroscope( getFirstRole());
}

/*****************************************************
**
**   HybridChart   ---   getSecondRole
**
******************************************************/
HoroscopeRole HybridChart::getSecondRole() const
{
	if ( chartRoles.size() > 1 ) return chartRoles[1];
	else return HoroscopeRole::None;
}

/*****************************************************
**
**   HybridChart   ---   getSecondHoroscope
**
******************************************************/
Horoscope_ptr HybridChart::getSecondHoroscope() const
{
	return props->getHoroscope( getSecondRole());
}

/*****************************************************
**
**   HybridChart   ---   getHoroscope
**
******************************************************/
Horoscope_ptr HybridChart::getHoroscope( const HoroscopeRole &role )
{
	assert( props );
	return props->getHoroscope( role );
}

/*****************************************************
**
**   HybridChart   ---   isDoubleChart
**
******************************************************/
bool HybridChart::isDoubleChart() const
{
#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::isDoubleChart category " << category;
	for( const auto &role : getChartRoles() )
	{
		debuglog << "HybridChart::isDoubleChart role " << role;
	}
#endif

	switch( props->getDataContext())
	{
		case DataContext::Partner:
			for( const auto &role : getChartRoles() )
			{
				if ( role == HoroscopeRole::Partner1 ) return true;
				if ( role == HoroscopeRole::Partner2 ) return true;
			}
		break;
		case DataContext::Solar:
		case DataContext::Direction:
		case DataContext::MultiDirection:
			return true;
		break;
		default:
		break;
	}
	return false;
}

/*****************************************************
**
**   HybridChart   ---   calculateChartRoles
**
******************************************************/
void HybridChart::calculateChartRoles()
{
#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::calculateChartRoles isComposite " << isCompositeChart();
#endif

	chartRoles.clear();

	switch( props->getDataContext())
	{
		case DataContext::Radix:
			chartRoles.push_back( HoroscopeRole::Radix );
			if ( props->getHoroscope( HoroscopeRole::HandShifted )
				&& usection
				&& usection->offsetToggle )
				//&& usection->offset )
			{
				chartRoles.push_back( HoroscopeRole::HandShifted );
			}
		break;

		case DataContext::Ashtakavarga:
			chartRoles.push_back( HoroscopeRole::Radix );
		break;

		case DataContext::Solar:
			chartRoles.push_back( HoroscopeRole::Radix );
			chartRoles.push_back( HoroscopeRole::Solar );
		break;

		case DataContext::Partner:
			if ( isCompositeChart() )
			{
				chartRoles.push_back( HoroscopeRole::Composite );
			}
			else
			{
				chartRoles.push_back( HoroscopeRole::Partner1 );
				chartRoles.push_back( HoroscopeRole::Partner2 );
			}
		break;

		case DataContext::Direction:
		case DataContext::MultiDirection:
			chartRoles.push_back( HoroscopeRole::Radix );
		  for( const auto &role : Enum<HoroscopeRole>( HoroscopeRole::FirstDirection, HoroscopeRole::LastDirection ))
			{
				if ( props->getHoroscope( role ))
				{
					chartRoles.push_back( role );
				}
			}
		break;
		default:
			assert( false );
		break;
	}

#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::calculateChartRoles found " << chartRoles.size() << " chartRoles";
	for( const auto &a : chartRoles )
	{
		debuglog << "HybridChart::calculateChartRoles role: " << a;
	}
#endif
}

/*****************************************************
**
**   HybridChart   ---   OnResponsiveChartEvent
**
******************************************************/
WidgetResponse HybridChart::onResponsiveChartEvent( const MComplex &p, const int &keymod )
{
#ifdef DEBUG_HYBRIDCHART
  debuglog << "HybridChart::onResponsiveChartEvent p " << p << " keymod " << keymod;
#endif

	// may be in initial state
	if ( rect.isNil()) return false;

	WidgetResponse response( false );
  for( auto &e: elements )
  {
		const WidgetResponse wr = e->onResponsiveChartEvent( p, keymod );
		if ( wr.doRefresh )
		{
			response.doRefresh = true;
			response.refreshRect = response.refreshRect.join( wr.refreshRect );
			if ( ! wr.message.empty() )
			{
				response.message += wr.message;
			}
		}
  }
	if ( response.doRefresh && response.refreshRect.isNil() )
	{
		response.refreshRect = getRect();
	}

#ifdef DEBUG_HYBRIDCHART
  debuglog << "Finished HybridChart::onResponsiveChartEvent doRefresh " << response.doRefresh
		<< " rect " << response.refreshRect;
#endif
	return response;
}

/*****************************************************
**
**   HybridChart   ---   handleMouseLeftEvent
**
******************************************************/
WidgetResponse HybridChart::handleMouseLeftEvent( const MComplex &p )
{
#ifdef DEBUG_HYBRIDCHART
  debuglog << "HybridChart::handleMouseLeftEvent p " << p;
#endif

	// may be in initial state
	if ( rect.isNil()) return false;

	WidgetResponse response( false );
  for( auto &e: elements )
  {
		const WidgetResponse wr = e->handleMouseLeftEvent( p );
		if ( wr.doRefresh )
		{
			response.doRefresh = true;
			response.refreshRect = response.refreshRect.join( wr.refreshRect );
			if ( ! wr.message.empty() )
			{
				response.message += wr.message;
			}
		}
  }
#ifdef DEBUG_HYBRIDCHART
  debuglog << "HybridChart::handleMouseLeftEvent p " << p << " response " << response.doRefresh
		<< " rect " << response.refreshRect;
#endif
	return response;
}

/*****************************************************
**
**   HybridChart   ---   getRelativeFontSize
**
******************************************************/
double HybridChart::getRelativeFontSize() const
{
	ChartConfig *cfg = getChartConfig( getWidgetCategory() );
	return cfg->relativeFontSize;
}

/*****************************************************
**
**   HybridChart   ---    getAyanamsa
**
******************************************************/
double HybridChart::getAyanamsa() const
{
	if ( getFirstHoroscope()) return getFirstHoroscope()->getAyanamsa( getViewpoint() );
	else return 0.0;
}

/*****************************************************
**
**   HybridChart   ---    calculateAries
**
******************************************************/
double HybridChart::calculateAries()
{
#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::calculateAries category " << category;
#endif

	// orientation only used in western chart. otherwise Cancer0 will be used, i.e. aries is one the left
	const WesternChartOrientation orient = category == WidgetCategory::WesternChart
		? getChartConfig( WidgetCategory::WesternChart )->chartOrientation
		: WesternChartOrientation::Cancer0;
	assert_valid_enum<WesternChartOrientation>( orient );

	switch( orient )
	{
		case WesternChartOrientation::Ascendant:
			return getFirstHoroscope()
				? red_deg( 180 - getFirstHoroscope()->getLongitude( PlanetId::Ascendant, getViewpoint() )) : 180;
		break;
		case WesternChartOrientation::House1:
			return getFirstHoroscope()
				? red_deg( 180 - getFirstHoroscope()->getHouseCusp( House::h1, getViewpoint() ) ) : 180;
		break;
		case WesternChartOrientation::Meridian:
			return getFirstHoroscope()
				? red_deg( 90 - getFirstHoroscope()->getLongitude( PlanetId::Meridian, getViewpoint() )) : 180;
		break;
		default: 
		break;
	}
	static const std::list<std::pair<WesternChartOrientation,int>> fixed_orientations =
	{
		{ WesternChartOrientation::Aries0, 90 },
		{ WesternChartOrientation::Aries15, 75 },
		{ WesternChartOrientation::Cancer0, 0 },
		{ WesternChartOrientation::Cancer15, 345 },
		{ WesternChartOrientation::Leo0, 330 },
		{ WesternChartOrientation::Libra0, 270 },
		{ WesternChartOrientation::Libra15, 255 },
		{ WesternChartOrientation::Capricorn0, 180 },
		{ WesternChartOrientation::Capricorn15, 165 }
	};
	for( auto &o : fixed_orientations )
	{
		if ( orient == o.first ) return (double)o.second;
	}
	assert( false );
	return 0.0;
}

/*****************************************************
**
**   HybridChart   ---   getChartConfig
**
******************************************************/
ChartConfig *HybridChart::getChartConfig( const WidgetCategory &cat ) const
{
	if ( overiddenChartConfig.first == cat )
	{
#ifdef DEBUG_HYBRIDCHART
		debuglog << "HybridChart::getChartConfig cat " << cat << " FOUND";
#endif
		return overiddenChartConfig.second;
	}
#ifdef DEBUG_HYBRIDCHART
	debuglog << "HybridChart::getChartConfig cat " << cat << " NOT FOUND";
#endif

	switch( cat )
	{
		case WidgetCategory::WesternChart:
			return &gdiconfig->chartWestern;
		break;
		case WidgetCategory::UranianChart:
			return &gdiconfig->chartUranian;
		break;
		case WidgetCategory::VargaChart:
			return &gdiconfig->chartVarga;
		break;
		case WidgetCategory::SarvatobhadraChart:
			return &gdiconfig->chartSbc;
		break;
		case WidgetCategory::UniversalChart:
			return &gdiconfig->chartUniversal;
		break;
		default:
			assert( false );
		break;
	}
	return nullptr;
}

/*****************************************************
**
**   HybridChart   ---   getGeometry
**
******************************************************/
const ChartGeometry &HybridChart::getGeometry()
{
	ChartGeometryLoader *loader = ChartGeometryLoader::get();
	const auto cat = getWidgetCategory();

//#ifdef DEBUG_HYBRIDCHART
#if 0
	static int count = 0;
	debuglog << "HybridChart::getGeometry cat " << cat << " count " << ++count;
#endif

	/*
	* idea of cache is to avoid many calls of findFirstGeometry if geom is not set
	*/
	uint geom = props->getGeometry( cat );
	if ( geom == UINT_FOR_LAZY_WAITING || geom == UINT_FOR_NOT_FOUND )
	{
		if ( auto a = geomCache.find( cat ); a != geomCache.end())
		{
			geom = a->second;
		}
		else
		{
			geom = loader->findFirstGeometry( cat, geom );
			geomCache[ cat ] = geom;

#ifdef DEBUG_HYBRIDCHART
			debuglog << "HybridChart::getGeometry setup cache for cat " << cat << ", value is " << geom;
#endif
		}
	}
	assert ( geom != UINT_FOR_LAZY_WAITING && geom != UINT_FOR_NOT_FOUND );
	return loader->getConfig( geom );
}


