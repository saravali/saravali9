/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/DasaChart.cpp
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

#include "DasaChart.h"

#include "astroconfig.h"
#include "ContextProperties.h"
#include "ColorConfig.h"
#include "Commands.h"
#include "Dasa.h"
#include "DasaTool.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "Lang.h"
#include "mathbase.h"
#include "Painter.h"
#include "DocHelper.h"

#include <format>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define DEBUG_DASACHART

/*****************************************************
**
**   DasaChart   ---   Constructor
**
******************************************************/
DasaChart::DasaChart( ContextProps_ptr props )
	: GdiWidget( props, WidgetCategory::DasaChart ),
	birthx( hborder )
{
	std::list<DasaId> dasaIds = gdiconfig->prefs.dasaChartSelectedDasas;
#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::DasaChart dasa size " << dasas.size();
#endif

	// add default Dasas if the list is empty
	if ( dasaIds.size() == 0 )
	{
		dasaIds =
		{
			DasaId::Vimsottari,
			DasaId::Yogini,
			DasaId::Kalachakra
		};
	}
	setDasas( dasaIds );

	currentItem = nullptr;
	adlevel = gdiconfig->prefs.dasaChartAntarDasaLevel;
}

/*****************************************************
**
**   DasaChart   ---   Destructor
**
******************************************************/
DasaChart::~DasaChart()
{
#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::~DasaChart dasa size " << dasas.size();
#endif
	gdiconfig->prefs.dasaChartSelectedDasas = dasas;
}

/*****************************************************
**
**   DasaChart   ---   calculateViewport
**
******************************************************/
MComplex DasaChart::calculateViewport()
{
	// x position of birth and the last date that will be displayed (100 years)
	endjdx = xviewport - hborder;
	const double duration = endjd - props->getRadixHoroscope()->getJD();

#ifdef DEBUG_DASACHART
	static int count = 0;
	debuglog << "DasaChart::calculateViewport START " << ++count << " zoom " << props->getZoom() << " log zoom " << props->getLogZoom()
		<< " total duration " << duration << " jdtotalduration " << jdtotalduration << " xviewport " << xviewport;
#endif

	viewportZoom = props->getLogZoom();
	const double zoom = props->getZoom();
#ifdef DEBUG_DASACHART
	debuglog << " percentual zoom " << zoom << " props log zoom " << props->getLogZoom();
#endif

	//xviewport = 10000 * zoom;
	xviewport = zoom * duration;
	yviewport = 5 * ybar +  ybar * ( adlevel + 2 ) * dasas.size();

	rect = MRect( 0, 0, xviewport, yviewport );
	sheetRect = MRect( 0, 0, xviewport, yviewport );
#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::calculateViewport FINISHED " << count << " log zoom " << props->getLogZoom()
		<< " sheetRect " << sheetRect;
#endif

#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::doLayout viewport size " << xviewport << "/" << yviewport << " endjdx " << endjdx;
#endif
	return MComplex( xviewport, yviewport );
}

/*****************************************************
**
**   DasaChart   ---   calculateRawLevels
**
******************************************************/
int DasaChart::calculateRawLevels()
{
#ifdef DEBUG_DASACHART
	StopWatch sw;
	static int count = 0;
	debuglog << "DasaChart::calculateRawLevels START " << ++count;
#endif

	int dasaCount = 0;

	endjd = props->getRadixHoroscope()->getJD();
	rootItems.clear();

	for ( const DasaId dasaId : dasas ) // First loop generates Dasas like Vimsottari, Yogini, Kalachakra
	{
		DasaExpert_ptr expert = experts[dasaId];
		assert( expert );

		GraphicalDasa_ptr item( new GraphicalDasa(( expert->createBlankRoot( expert ))));

		for( auto &d : expert->getFirstLevel( props, item->dasa ))
		{
			dasaCount++;
			endjd = std::max( endjd, d->getEndJD());
			GraphicalDasa_ptr child( new GraphicalDasa( d ));
			item->children.push_back( child );

			// Antardasas
			if ( adlevel > 0 )
			{
				dasaCount += calculateNextRawLevel( child, expert );
			}
		}
		rootItems.push_back( item );

	}

	endjd += 10.0;
	jdtotalduration = endjd - props->getRadixHoroscope()->getJD();

#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::calculateRawLevels FINISHED " << count
		<< " calculation time " << sw.getElapsedMillisec() << " millisec, totally "
		<< dasaCount << " raw dasas calculated end date "
		<< DateFormatter::get()->formatUTDate( endjd )
		<< " totalduration " << jdtotalduration << " days";
#endif

	return dasaCount;
}

/*****************************************************
**
**   DasaChart   ---   calculateNextRawLevel
**
******************************************************/
int DasaChart::calculateNextRawLevel( GraphicalDasa_ptr item, DasaExpert_ptr expert )
{
	assert( item );
	assert( item->dasa );
	assert( item->children.size() == 0 );
	assert( item->getLevel() >= DasaLevel::Mahadasa );
	assert( expert );

#ifdef DEBUG_DASACHART
	static int count = 0;
	debuglog << "DasaChart::calculateNextRawLevel START " << ++count << " level " << item->getLevel()
		<< " adlevel " << adlevel;
#endif

	int dasaCount = 0;
	for( auto &d : expert->getNextLevel( props, item->dasa ))
	{
		dasaCount++;
		GraphicalDasa_ptr child( new GraphicalDasa( d ));
		item->children.push_back( child );

		if ( (int)d->getLevel() < adlevel )
		{
			dasaCount += calculateNextRawLevel( child, expert );
		}
	}

#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::calculateNextRawLevel FINISHED " << count << " dasaCount " << dasaCount;
#endif

	return dasaCount;
}

/*****************************************************
**
**   DasaChart   ---   OnDataChanged
**
******************************************************/
void DasaChart::OnDataChanged()
{
#ifdef DEBUG_DASACHART
	static int count = 0;
	debuglog << "DasaChart::OnDataChanged START " << ++count << " log zoom " << props->getLogZoom();
#endif

	currentItem  = nullptr;
	calculateRawLevels();
}

/*****************************************************
**
**   DasaChart   ---   doPaint
**
******************************************************/
void DasaChart::doPaint( Painter *painter, const MRect &refreshRect )
{
#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::doPaint refreshRect " << refreshRect
		<< " rect " << rect << " viewport " << xviewport << "/" << yviewport;
#endif
	StopWatch sw;

	wxString s;
	int x, y;
	MRect titleRect;
	DocHelper util( painter );
	DasaTool *dasatool = DasaTool::get();

	painter->setGraphicFont();
	assert( painter );

	const MBrush brush = getSkinnedBrush();
	painter->setBrush( brush );
	painter->drawRectangle( refreshRect );

	paintRuler( painter, refreshRect );

	for( auto &item : rootItems )
	{
		DasaExpert_ptr expert = item->dasa->getExpert();
		assert( expert );
		s = dasatool->getDasaName( expert->getDasaId() );

		if ( item->children.size() == 0 )
		{
			errorlog << "DasaChart::doPaint: root item for dasa " << s << " has no children";
		}
		else
		{
			auto child1 = item->children.begin();
			if ( (*child1)->rect.isNil())
			{
				const int count = calcItemRectangles( item );
				debuglog << "DasaChart::doPaint " << count << " rectangles calculated for Dasa " << s;
			}
		}

		// repeat set font because it will be set during Dasa paint
		painter->setGraphicFont();
		painter->setTextColor( gdiconfig->colors.textFgColor );
		MComplex p = painter->getCharacterExtent();

		y = calcYTopForDasa( item );
		x = 100;
		const double xMax = jd2x( item->dasa->getEndJD());

		//while( x + (int)p.real() < xviewport )
		while( x + (int)p.real() < xMax )
		{
			titleRect = MRect( x, y - ybar, (int)p.real(), ybar );
#ifdef DEBUG_DASACHART
			debuglog << "DasaChart::doPaint titleRect " << titleRect << " xMax " << xMax << " s " << s;
#endif
			
			if ( refreshRect.intersects( titleRect ))
			//if ( rect.intersects( titleRect ))
			{
				util.drawTextFormatted(
					s,
					titleRect,
					Align::SouthEast );
#ifdef DEBUG_DASACHART
				debuglog << "DasaChart::doPaint paint title, titleRect " << titleRect;
#endif
			}
			x += DASA_TITLE_STEP_WIDTH;
		}

		// second task: print Dasa
		for( auto &child : item->children )
		{
			paintDasaItem( painter, child, refreshRect, /*doIterate*/ true,
				gdiconfig->bardiagram.doItemZoom ); // skipCurrentItem if zoom is activated, will be painted last (on top)
		}
	}

	// 3rd: print current item last
	if ( currentItem && gdiconfig->bardiagram.doItemZoom )
	{
		paintDasaItem( painter, currentItem, refreshRect, /*doIterate*/ false, /*skipCurrentItem*/ false );
	}
	infolog << "FINISHED DasaChart::doPaint " << sw.getElapsedMillisec() << " millisec";
}

/*****************************************************
**
**   DasaChart   ---   paintDasaItem
**
******************************************************/
void DasaChart::paintDasaItem( Painter *painter,
	GraphicalDasa_ptr item,
	const MRect& refreshRect,
	const bool doIterate,
	const bool skipCurrentItem )
{
	assert( item );
	DasaBarPaintHelper helper( gdiconfig->bardiagram );

	const bool zoomItem = gdiconfig->bardiagram.doItemZoom && item == currentItem;
#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::paintDasaItem refreshRect " << refreshRect
		<< " level " << item->dasa->getLevel()
		<< " item rect " << item->rect
		<< " zoomItem " << zoomItem;
#endif

	if (( item != currentItem || ! skipCurrentItem ) && refreshRect.intersects( item->rect ))
	//if (( item != currentItem || ! skipCurrentItem ))
	{
		if ( item->rect.width > DASA_BAR_MIN_ITEM_WIDTH )
		{
#ifdef DEBUG_DASACHART
			debuglog << "DasaChart::paintDasaItem DOPAINT";
#endif
			helper.paintDasaBar( painter, item, zoomItem );
		}
		else // item too small
		{
			painter->setPenColor( *wxLIGHT_GREY );
			painter->setTransparentBrush();
			painter->drawRectangle( item->rect );
		}
	}

	const int level = (int)item->dasa->getLevel() + 1;

	// attention: sublevel must be painted if rect contains a deeper antardasa
	// y is special: paint can be required even if y value of refresh rect does not intersect item rect -> item is in sublevel
	if ( doIterate
		&& level <= adlevel
		&& item->rect.x + item->rect.width >= refreshRect.x
		&& item->rect.x <= refreshRect.x + refreshRect.width
		&& item->rect.y + item->rect.height >= refreshRect.y - ( adlevel - level ) * ybar
		&& item->rect.y <= refreshRect.y + refreshRect.height
		)
	{
		// calculate dependent items if not available (first time paint)
		//if ( item->children.size() == 0 ) calcDasaLevel( item );

		for( auto &child: item->children ) paintDasaItem( painter, child, refreshRect );
	}
}

/*****************************************************
**
**   DasaBarPaintHelper   ---   paintDasaBar
**
******************************************************/
void DasaBarPaintHelper::paintDasaBar( Painter *painter, GraphicalDasa_ptr gd, const bool &doZoom )
{
	assert( painter );
	assert( gd );
#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::paintDasaBar rect " << gd->rect << " zoomItem " << doZoom;
#endif

	Dasa_ptr dasa = gd->dasa;
	assert( dasa );
	DasaExpert_ptr expert = gd->getExpert();
	assert( expert );
#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::paintDasaBar expert " << expert->getDasaId();
#endif

	const int styleid = BarDiagramConfig::getConfigId( gd->dasa->getDasaLord());
	const BarDiagramStyle &style = barconfig.styles[styleid];
	painter->setPen( style.pen );
	painter->setTextColor( style.pen.GetColour() );
	painter->setBrush( style.brush );

	//painter->drawRectangle( dasa->rect );

	MRect rr = gd->rect;
	if ( doZoom ) rr.grow( barconfig.itemZoom );
	painter->drawRectangle( rr, barconfig.cornerRadius );

	painter->setGraphicFont();
	const double w = painter->getCharacterExtent().real();
#ifdef DEBUG_DASACHART
	debuglog << "DasaBarPaintHelper::paintDasaBar standard width is " << w;
#endif
	
	PlanetConfig planetCfg = astroconfig->getViewpointConfig( Viewpoint::Vedic ).planets;
	PlanetPropertyConfig propertyCfg = astroconfig->getViewpointConfig( Viewpoint::Vedic ).planetProps;

	TextFormat format = std::holds_alternative<Sign>( dasa->getDasaLord())
		? propertyCfg.textFormat
		: planetCfg.textFormat;

	const bool isSymbol = ( format == TextFormat::Symbol );

	if ( gd->rect.width < 8 * w ) format = TextFormat::Short;
	else if ( gd->rect.width < 15 * w ) format = TextFormat::Medium;
	else format = TextFormat::Long;

	if ( isSymbol )
	{
		gd->tf = expert->getDasaLordName( dasa, planetCfg, propertyCfg );
	}

	propertyCfg.textFormat = format;
	planetCfg.textFormat = format;
	if ( ! isSymbol )
	{
		gd->tf = expert->getDasaLordName( dasa, planetCfg, propertyCfg );
	}
	const wxString extraLord = expert->getExtraLordName( dasa, planetCfg, propertyCfg );
#if 0
	debuglog << "DasaBarPaintHelper::paintDasaBar rect " << dasa->rect << " w " << w
		<< " lord " << dasa->tf << " extraLord " << extraLord;
#endif

	if ( ! extraLord.empty() && gd->rect.width > 6 * w )
	{
		gd->tf << "/" << expert->getExtraLordName( dasa, planetCfg, propertyCfg );
	}

	gd->align = Align::Center;

	DocHelper util( painter );
	util.formatAndDrawTextItem( *gd );
}

/*****************************************************
**
**   DasaChart   ---   calcItemRectangles
**
******************************************************/
int DasaChart::calcItemRectangles( GraphicalDasa_ptr item )
{
	assert( item );
	double x1 = -1;
	double x2 = 0;
	const int y = calcYTopForDasa( item );
	int count = 0;

#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::calcItemRectangles";
#endif

	for( auto &child : item->children )
	{
		assert( child->dasa );
		child->tf.clear();
		count++;

		if ( x1 == -1 ) x1 = jd2x( std::max( child->dasa->getStartJD(), props->getRadixHoroscope()->getJD() ));
		x2 = jd2x( std::min( child->dasa->getEndJD(), endjd ));

		if ( child->rect.isNil() ) child->rect = MRect( x1, y, x2 - x1, ybar );
		else child->rect = MRect( x1, y, x2 - x1, ybar );

		x1 = x2;
		if ( child->children.size() > 0 ) count += calcItemRectangles( child );
	}
	return count;
}

/*****************************************************
**
**   DasaChart   ---   paintRuler
**
******************************************************/
void DasaChart::paintRuler( Painter *painter, const MRect &refreshRect )
{

	// y value of the ruler
	const int yruler = yviewport - 3 * ybar;

	// tolerances for centered text output
	constexpr int xtext = 50;
	constexpr int ytext = 10;

	// half length of the tickers
	constexpr int yearticker = 20;
	constexpr int quarteryearticker = 15;
	constexpr int monthticker = 10;

	int ylength;
	double x, xm;
	wxString s;
	Lang lang;
	DocHelper util( painter );

	painter->setTransparentBrush();
	painter->setPen( getSkinnedPen());
	painter->setTextColor( getSkinnedTextColor());

	MRect r( birthx, 0, (int)( endjdx - birthx ), yviewport );
	r = r.intersect( refreshRect );

	painter->drawLine( r.x, yruler, r.x + r.width, yruler );

	DateFormatter *formatter = DateFormatter::get();
	const int startyear = formatter->calculateYearFromJD( x2jd( r.x ));
	MDate d( startyear, 1, 1 );
	const double startx = jd2x( d.jd );

	const int endyear = formatter->calculateYearFromJD( x2jd( r.x + r.width ));
	d.set( endyear + 1, 1, 1, 0 );
	const double endx = jd2x( d.jd );

	if ( endx <= startx )
	{
		warnlog << "invalid values for start and end of Dasas " << startx << "/" << endx;
		return;
	}
	assert( endyear >= startyear );

	const double steplen = ( endx - startx ) / ( endyear + 1 - startyear );
	const double xmonth = steplen / 12.0;

#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::paintRuler startyear " << startyear << " endyear " << endyear
		<< " startx " << startx << " endx " << endx
		<< " steplen " << steplen << " xmonth " << xmonth;
#endif

	for ( int i = 0; i <= endyear + 1 - startyear; i++ )
	{
		x = startx + i * steplen;

		if ( x > birthx && x <= endjdx && x >= r.x - xtext && x <= r.x + r.width + xtext )
		{
			painter->drawLine( x, (int)( 2 * hborder), x, yruler + yearticker );

			// print year if large enough, otherwise only 10 year steps
			if ( steplen > 40 || ! (( startyear + i ) % 10 ))
			{
				s = std::format( "{}", startyear + i );
				util.drawTextFormatted(
					s,
					MRect( x - xtext, yruler + yearticker + ytext, 2 * xtext, 2 * ytext ),
					Align::Center );
			}
		}

		// print month if month has a minimal x length
		if ( xmonth > 3 )
		{
			for ( int j = 1; j < 12; j++ )
			{
				xm = x + j * xmonth;
				if ( xm < birthx || xm > endjdx ) continue;

				if ( xm >= r.x - xtext && xm <= r.x + r.width + xtext )
				{
					ylength = j % 3 ? monthticker : quarteryearticker;
					painter->drawLine( xm, yruler - ylength, xm, yruler + ylength );
					if ( xmonth > 50 )
					{
						util.drawTextFormatted(
							lang.getMonthName( j, TextFormat::Short ),
							MRect( xm-xtext, yruler + ytext, 2*xtext, 2*ytext ),
							Align::Center );
					}
					else if  ( xmonth > 25 )
					{
						s = std::format( "{}", j + 1 );
						util.drawTextFormatted(
							s,
							MRect( xm-xtext, yruler + ytext, 2*xtext, 2*ytext ),
							Align::Center );
					}
				}
			}
		}

	}

	// mark current date
	painter->setPen( wxPen( *wxBLUE, 2, wxPENSTYLE_SOLID ));
	x = jd2x( MDate::getCurrentJD());
	if ( x >= refreshRect.x && x <= refreshRect.getRight())
	{
		painter->drawLine( x, hborder, x, yruler + 2 * yearticker );
	}
}

/*****************************************************
**
**   DasaChart   ---   setDasas
**
******************************************************/
void DasaChart::setDasas( const std::list<DasaId> &d )
{
	dasas = d;

	DasaExpertFactory factory;
	for( const DasaId id : dasas )
	{
		if ( experts.find( id ) == experts.end())
		{
			experts[id] = DasaExpert_ptr( factory.getDasaExpert( id ));
		}
	}
}

/*****************************************************
**
**   DasaChart   ---   onResponsiveChartEvent
**
******************************************************/
WidgetResponse DasaChart::onResponsiveChartEvent( const MComplex &p, const int& /*keymod*/ )
{
#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::onResponsiveChartEvent p " << p;
#endif

	GraphicalDasa_ptr item = findItemForPoint( p );
#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::onResponsiveChartEvent item " << item.get();
#endif

	WidgetResponse response( false );
	if ( item )
	{
#ifdef DEBUG_DASACHART
		debuglog << "DasaChart::onResponsiveChartEvent case 1";
#endif

		if ( item != currentItem ) // item has changed
		{
			response.doRefresh = true;
			response.refreshRect = zoomRect( item->rect );
#ifdef DEBUG_DASACHART
			debuglog << "DasaChart::onResponsiveChartEvent (1) " << response.refreshRect;
#endif
			if ( currentItem ) 
			{
				response.refreshRect = response.refreshRect.join( zoomRect( currentItem->rect ));
#ifdef DEBUG_DASACHART
				debuglog << "DasaChart::onResponsiveChartEvent (2) " << zoomRect( currentItem->rect )
					<< " R " << response.refreshRect;
#endif
			}
#ifdef DEBUG_DASACHART
			debuglog << "DasaChart::onResponsiveChartEvent (3) " << response.refreshRect;
#endif
			currentItem = item;
		}
		else {} // nothing to

		response.message = DasaTool::get()->getDasaLordChain( item->dasa, true );
	}
	else // no item found at point p
	{
#ifdef DEBUG_DASACHART
		debuglog << "DasaChart::onResponsiveChartEvent case 2";
#endif
		if ( currentItem ) 
		{
			response.doRefresh = true;
			response.refreshRect = zoomRect( currentItem->rect );
			currentItem = nullptr;
		}
		//if( gdiconfig->bardiagram.showToolbarMessages && sheetRect.containsPoint( p ))
		if( sheetRect.containsPoint( p ))
		{
#ifdef DEBUG_DASACHART
			debuglog << "DasaChart::onResponsiveChartEvent case 3";
#endif
			DateFormatter *formatter = DateFormatter::get();
			response.message = formatter->formatUTDatetime( x2jd( p.real() ));
		}
	}
#ifdef DEBUG_DASACHART
	debuglog << "DasaChart::onResponsiveChartEvent doRefresh " << response.doRefresh
		<< " refreshRect " << response.refreshRect;
#endif
	return response;
}

/**************************************************************
***
**   DasaChart   ---   x2jd
***
***************************************************************/
double DasaChart::x2jd( const double &x )
{
	assert( endjd != birthx );
	return props->getRadixHoroscope()->getJD() + jdtotalduration * ( x - birthx ) / ( endjdx - birthx );
}

/*****************************************************
**
**   DasaChart   ---   jd2x
**
******************************************************/
double DasaChart::jd2x( const double &jd )
{
	assert( jdtotalduration > 0 );
	assert( endjd != birthx );

	const double portion = std::max( 0.0, jd - props->getRadixHoroscope()->getJD()) / jdtotalduration;
	const double x = birthx + portion * ( endjdx - birthx );

#ifdef DEBUG_DASACHART
	const double duration = jd - props->getRadixHoroscope()->getJD();
	debuglog << std::format( "DasaChart::jd2x jd {:.12f}", jd )
		<< " jdtotalduration " << jdtotalduration
		<< " birthjd " << props->getRadixHoroscope()->getJD()
		<< " duration " << duration
		<< " endjd " << endjd
		<< " endjdx " << endjdx
		<< " birthx " << birthx
		<< " portion " << portion
		<< " x " << x;
#endif
	return x;
}

/*****************************************************
**
**   DasaChart   ---   calcYTopForDasa
**
******************************************************/
int DasaChart::calcYTopForDasa( GraphicalDasa_ptr item )
{
	assert( item.get() );
	DasaExpert_ptr expert = item->getExpert();
	assert( expert );
	const DasaId dasaId = expert->getDasaId();

	int level = 0;
	int i = 0;
	for ( const DasaId &d : dasas )
	{
		if ( d == dasaId )
		{
			level = i;
			break;
		}
		i++;
	}
	//debuglog << "DasaChart::calcYTopForDasa dasaLevel " << level;
	const int step = (int)level * ( adlevel + 2 )  + (int)item->getLevel() + 2;
	//debuglog << "DasaChart::calcYTopForDasa dasaLevel " << level << " step " << step;
	return ybar * step;
}

/**************************************************************
***
**   DasaChart   ---   zoomRect
***
***************************************************************/
MRect DasaChart::zoomRect( const MRect &rect )
{
	return MRect( rect ).grow( gdiconfig->bardiagram.itemZoom );
}

/**************************************************************
***
**   DasaChart   ---   findItemForPoint
***
***************************************************************/
GraphicalDasa_ptr DasaChart::findItemForPoint( const MComplex &p )
{
	if ( ! sheetRect.containsPoint( p )) return nullptr;

	for ( auto &rootitem : rootItems )
	{
		// find a Dasa candidat for y value
		const int y0 = calcYTopForDasa( rootitem );
		if ( p.imag() >= y0 && p.imag() <= y0 + ybar + adlevel * ybar )
		{
			GraphicalDasa_ptr item = findItemForPointInDasa( rootitem, p );
			if ( item ) return item;
		}
	}
	return nullptr;
}

/**************************************************************
***
**   DasaChart   ---   findItemForPointInDasa
***
***************************************************************/
GraphicalDasa_ptr DasaChart::findItemForPointInDasa( GraphicalDasa_ptr item, const MComplex &p )
{
	for( auto &subitem : item->children )
	{
		assert( subitem.get() );
#ifdef DEBUG_DASACHART
		debuglog << "DasaChart::findItemForPointInDasa p " << p << " rect " << subitem->rect
			<< " match " << subitem->rect.containsPoint( p );
#endif
		if ( subitem->rect.containsPoint( p ))
		{
			return subitem;
		}

		// x coordiante matches but y value is deeper
		else if ( subitem->rect.x <= p.real() && p.real() <= subitem->rect.getRight() )
		{
			return findItemForPointInDasa( subitem, p );
		}
	}
	warnlog << "findItemForPointInDasa not found for p " << p;
	return nullptr;
}

/*****************************************************
**
**   DasaChart   ---   setAntardasaLevel
**
******************************************************/
void DasaChart::setAntardasaLevel( const int &a )
{
	adlevel = a;
}


