/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/GdiWidgetGrid.cpp
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

#include "GdiWidgetGrid.h"

#include "astroconfig.h"
#include "ConfigLoaderImpls.h"
#include "ContextProperties.h"
#include "logger.h"
#include "mathbase.h"
#include "Painter.h"

extern Astroconfig *astroconfig;

#define DEBUG_MWIDGETGRID

/*****************************************************
**
**   GdiWidgetGrid   ---   Destructor
**
******************************************************/
GdiWidgetGrid::~GdiWidgetGrid()
{
	clearWidgets();
}

/*****************************************************
**
**   GdiWidgetGrid   ---   addWidget
**
******************************************************/
void GdiWidgetGrid::addWidget( GdiWidget *widget )
{
	widgets.push_back( widget );
}

/*****************************************************
**
**   GdiWidgetGrid   ---   calculatePreferredWidgetSize
**
******************************************************/
// TODO immer by parent
MComplex GdiWidgetGrid::calculatePreferredWidgetSize( Painter*, const double &xmax )
{
	double width = std::max( widgetXMin, xmax );
	width = std::min( width, widgetXMax );
	double height = width;

	if ( widgetX2yratio ) height = width / widgetX2yratio;

#ifdef DEBUG_MWIDGETGRID
	debuglog << " GdiWidgetGrid::calculatePreferredWidgetSize xmax " << xmax
		<< " minwidth " << widgetXMin
		<<  " ratio " << widgetX2yratio
		<< " width " << width
		<< " height " << height;
#endif

	return MComplex( width, height );
}

/*****************************************************
**
**   GdiWidgetGrid   ---   getWidgetAreaSize
**
******************************************************/
MComplex GdiWidgetGrid::getWidgetAreaSize()
{
	if ( widgets.size() == 0 ) return 0;
	assert( columnCount > 0 );

  const int nb_rows = ( widgets.size() + columnCount - 1 ) / columnCount;
	assert( nb_rows > 0 );
	const MRect r =  widgets.front()->getRect();
	return MComplex( columnCount * r.width, nb_rows * r.height );
}

/*****************************************************
**
**   GdiWidgetGrid   ---   doLayout
**
******************************************************/
MComplex GdiWidgetGrid::doLayout( Painter *painter )
{
  if ( widgets.size() == 0 ) return 0;

	MComplex p;
	switch( gridFormat )
	{
		case WidgetGridFormat::OptimizedFloat:
			p = layoutWidgetsOptimizedFloat( painter );
		break;
		case WidgetGridFormat::FloatFixedColWidth:
		case WidgetGridFormat::FloatVariableColWidth:
			p = layoutWidgetsFloatCluster( painter );
		break;
		case WidgetGridFormat::FixedCols:
			p = layoutWidgetsFixedCol( painter );
		break;
		case WidgetGridFormat::FixedColsScaled:
			p = layoutWidgetsFixedColsScaled( painter );
		break;
		default:
			fatallog << "invalid WidgetGridFormat " << (int)gridFormat;
			assert( false );
		break;
	}
	onCoordinatesChanged();
#ifdef DEBUG_MWIDGETGRID
	debuglog << "doLayout result " << p;
#endif	
	return p;
}

/*****************************************************
**
**   GdiWidgetGrid   ---   layoutWidgetsFixedColsScaled
**
******************************************************/
MComplex GdiWidgetGrid::layoutWidgetsFixedColsScaled( Painter* )
{
	assert( columnCount > 0 );
  assert( widgets.size() > 0 );
	assert( widgetX2yratio > 0 );
	if ( rect.isNil()) return 0;

  const int nb_rows = ( widgets.size() + columnCount - 1 ) / columnCount;
	assert( nb_rows > 0 );

	double xstep = std::max( rect.width / columnCount, widgetXMin );
	double ystep = std::max( rect.height / nb_rows, widgetXMin * widgetX2yratio );

  double x0 = rect.x; // current x value
  double y0 = rect.y; // current y
	uint col = 0;

  for( GdiWidget *w : widgets )
  {
		w->setRect( x0, y0, xstep, ystep );
#ifdef DEBUG_MWIDGETGRID
		debuglog << "GdiWidgetGrid::layoutWidgetsFixedCol Rect " << w->getRect();
#endif	

		if ( ++col >= columnCount )
		{
#ifdef DEBUG_MWIDGETGRID
			debuglog << "GdiWidgetGrid::layoutWidgetsFixedColsScaled break " << x0 << " xstep " << xstep << " y0 " << y0;
#endif	
			x0 = 0.0;
			col = 0;
			y0 += ystep;
		}
		else // proceed in this line
		{
#ifdef DEBUG_MWIDGETGRID
			debuglog << "GdiWidgetGrid::layoutWidgetsFixedColsScaled next x0 " << x0 << " xstep " << xstep << " y0 " << y0;
#endif	
			x0 += xstep;
		}
  }
	if ( x0 > 0 ) y0 += ystep;

  const double xtotal = std::max( rect.width, columnCount * xstep );
  const double ytotal = std::max( rect.height, y0 );
#ifdef DEBUG_MWIDGETGRID
  debuglog << "Finished GdiWidgetGrid::layoutWidgetsFixedColsScaled xtotal " << xtotal << " ytotal " << ytotal;
#endif	
	return MComplex( xtotal, ytotal );
}

/*****************************************************
**
**   GdiWidgetGrid   ---   layoutWidgetsFixedCol
**
******************************************************/
MComplex GdiWidgetGrid::layoutWidgetsFixedCol( Painter *painter )
{
	assert( columnCount > 0 );
  assert( widgets.size() > 0 );
	assert( rect.width > 0 );
#ifdef DEBUG_MWIDGETGRID
	debuglog << "GdiWidgetGrid::layoutWidgetsFixedCol START cols " << columnCount << " rect " << rect;
#endif	

	double xstep = rect.width / columnCount;

  double x0 = rect.x; // current x value
  double y0 = rect.y; // current y
	const MComplex prefSize = calculatePreferredWidgetSize( painter, xstep );
	xstep = std::max( xstep, prefSize.real() );
#ifdef DEBUG_MWIDGETGRID
	debuglog << "GdiWidgetGrid::layoutWidgetsFixedCol xstep " << xstep << " prefSize " << prefSize;
#endif	

	uint col = 0;
  for( GdiWidget *w : widgets )
  {
		w->setRect( x0, y0, prefSize.real(), prefSize.imag() );
#ifdef DEBUG_MWIDGETGRID
		debuglog << "GdiWidgetGrid::layoutWidgetsFixedCol Rect " << w->getRect();
#endif	

		if ( ++col >= columnCount )
		{
#ifdef DEBUG_MWIDGETGRID
			debuglog << "GdiWidgetGrid::layoutWidgetsFixedCol break " << x0 << " xstep " << xstep << " y0 " << y0;
#endif	
			x0 = 0.0;
			col = 0;
			y0 += prefSize.imag();
		}
		else // proceed in this line
		{
#ifdef DEBUG_MWIDGETGRID
			debuglog << "GdiWidgetGrid::layoutWidgetsFixedCol next x0 " << x0 << " xstep " << xstep << " y0 " << y0;
#endif	
			x0 += xstep;
		}
  }
	// add one line to y value if the line is partly consumed
	if ( x0 > 0 ) y0 += prefSize.imag();

  const double xtotal = std::max( rect.width, columnCount * prefSize.real());
  const double ytotal = std::max( rect.height, y0 );
#ifdef DEBUG_MWIDGETGRID
  debuglog << "Finished GdiWidgetGrid::layoutWidgetsFixedCol xtotal " << xtotal << " ytotal " << ytotal;
#endif	
	return MComplex( xtotal, ytotal );
}

/*****************************************************
**
**   GdiWidgetGrid   ---   calculateOptimizedFloatLayout
**
******************************************************/
MComplex GdiWidgetGrid::calculateOptimizedFloatLayout( const double &containerRatio, const uint &totalsize )
{
	assert( containerRatio > 0 );
	assert( widgetX2yratio > 0 );
	assert( totalsize > 0 );

#ifdef DEBUG_MWIDGETGRID
	debuglog << "Start GdiWidgetGrid::calculateOptimizedFloatLayout containerRatio " << containerRatio
		<< " widgetRatio " << widgetX2yratio << " totalsize " << totalsize;
#endif	

	int nb_cols = 1;
	double xscore = 0.0;
	double realx = 0.0;

	for( uint tcols = 1; tcols <= totalsize; ++tcols )
	{
		const uint trows = ( totalsize + tcols - 1 ) / tcols;

		// assume width of container is 1.0
		const double xsize = 1.0 / tcols;
		const double ysize = 1.0 / ( trows * containerRatio );
#ifdef DEBUG_MWIDGETGRID
		debuglog << "GdiWidgetGrid::calculateOptimizedFloatLayout cols " << tcols << " rows "
			<< trows << " xsize " << xsize << " ysize " << ysize;
#endif	

		if ( xsize / widgetX2yratio > ysize )
		{
			realx = widgetX2yratio * ysize;
		}
		else
		{
			realx = xsize;
		}
		if ( realx > xscore )
		{
			xscore = realx;
			nb_cols = tcols;
		}
	}
#ifdef DEBUG_MWIDGETGRID
	debuglog << "Finished GdiWidgetGrid::calculateOptimizedFloatLayout xscore " << xscore << " nb_cols " << nb_cols;
#endif	
	return MComplex( nb_cols, xscore );
}

/*****************************************************
**
**   GdiWidgetGrid   ---   layoutWidgetsOptimizedFloat
**
******************************************************/
MComplex GdiWidgetGrid::layoutWidgetsOptimizedFloat( Painter *painter )
{
#ifdef DEBUG_MWIDGETGRID
	debuglog << "GdiWidgetGrid::layoutWidgetsOptimizedFloat rect " << rect;
#endif	
  if ( widgets.size() == 0 ) return 0;
  if ( rect.width == 0 || rect.height == 0 ) return 0;
	assert( rect.width > 0 );
	assert( rect.height > 0 );

	// ratio of widget grid
	const double r1 = rect.width / rect.height;

#ifdef DEBUG_MWIDGETGRID
  debuglog << "------------- GdiWidgetGrid::arrangeWidgets ----------------- ";
  debuglog << "GdiWidgetGrid::layoutWidgetsOptimizedFloat widgetsize " << widgets.size() << " rect " << rect;
  debuglog << "GdiWidgetGrid::layoutWidgetsOptimizedFloat widget ratio rect " << r1 << " widgets " << widgetX2yratio;
#endif	
	
	// do layout, returns nb_cols and relative width
	MComplex p = calculateOptimizedFloatLayout( r1, widgets.size());

	const int nb_cols = p.real();
  //const int nb_rows = ( widgets.size() + nb_cols - 1 ) / nb_cols;
#ifdef DEBUG_MWIDGETGRID
	debuglog << "GdiWidgetGrid::layoutWidgetsOptimizedFloat FINAL COLS " << nb_cols << " score " << p.imag() * rect.width;
#endif	

  double xstep = std::max( widgetXMin, p.imag() * rect.width );
	double xright = std::max( rect.width, xstep * nb_cols ); // can be outside rect if minimum size is exceeded
#ifdef DEBUG_MWIDGETGRID
  debuglog << "GdiWidgetGrid::layoutWidgetsOptimizedFloat widget size " << rect.width << "/" << rect.height
		<< " cols " << nb_cols;
  debuglog << "GdiWidgetGrid::layoutWidgetsOptimizedFloat xstep " << xstep << " xright " << xright;
#endif	

  double x0 = rect.x; // current x value
  double y0 = rect.y; // current y
  double ymax = 0; // max y per row

	const MComplex prefSize = calculatePreferredWidgetSize( painter, xstep );
	xstep = prefSize.real();

  for( GdiWidget *w : widgets )
  {
		// TODO
    //w->setPreferredSize( xstep );
		//w->rect.moveTo( x0, y0 );
		w->setRect( x0, y0, prefSize.real(), prefSize.imag() );
#ifdef DEBUG_MWIDGETGRID
		debuglog << "GdiWidgetGrid::layoutWidgetsOptimizedFloat Rect " << w->getRect();
#endif	
		ymax = std::max( ymax, w->getRect().height );

		// see if the next in this row will be too much
		if ( x0 + 2 * xstep  - .001 > xright ) // break
		{
#ifdef DEBUG_MWIDGETGRID
			debuglog << "GdiWidgetGrid::layoutWidgetsOptimizedFloat break " << x0 << " xstep " << xstep
				<< " xright " << xright << " y0 " << y0 << " ymax " << ymax;
#endif	
			x0 = 0.0;
			y0 += ymax;
		}
		else // proceed in this line
		{
#ifdef DEBUG_MWIDGETGRID
			debuglog << "GdiWidgetGrid::layoutWidgetsOptimizedFloat next x0 " << x0 << " xstep " << xstep
				<< " xright " << xright << " y0 " << y0;
#endif	
			x0 += xstep;
		}
  }
	if ( x0 > 0 ) y0 += ymax;

  const double xtotal = std::max( rect.width, nb_cols * xstep );
  const double ytotal = std::max( rect.height, y0 );
#ifdef DEBUG_MWIDGETGRID
  debuglog << "Finished GdiWidgetGrid::layoutWidgetsOptimizedFloat xtotal " << xtotal << " ytotal " << ytotal;
#endif	
	return MComplex( xtotal, ytotal );
}

/*****************************************************
**
**   GdiWidgetGrid   ---   layoutWidgetsFloatCluster
**
******************************************************/
MComplex GdiWidgetGrid::layoutWidgetsFloatCluster( Painter *painter )
{
#ifdef DEBUG_MWIDGETGRID
	debuglog << "GdiWidgetGrid::layoutWidgetsFloatCluster rect " << rect;
#endif	
  if ( widgets.size() == 0 ) return 0;
  if ( rect.width == 0 || rect.height == 0 ) return 0;

	const bool useCommonWidth = gridFormat == WidgetGridFormat::FloatFixedColWidth;
	// false means WidgetGridFormat::FloatVariableColWidth

	double xtotal = rect.width;
	double ytotal = rect.height;

	const double maxwcol = std::max( rect.width, 600.0 );

	double x0 = 0;
	double xx = 0;
	double xstep = 0;
	double hcurrentrow = 0;
	double ycurrent = 0;
	MComplex widgetsize = 0;

	double commonwidth = 0;
	for( auto *widget : widgets )
	{
		widget->doLayout( painter );
		commonwidth = std::max( commonwidth, widget->getRect().width );
	}

	auto iter = widgets.begin();
	auto colstartiter = iter;
  while( iter != widgets.end() )
  {
		(*iter)->doLayout( painter );
		widgetsize = MComplex( (*iter)->getRect().width, (*iter)->getRect().height );
		if ( useCommonWidth )
		{
			xstep = commonwidth;
		}
		else
		{
			xstep = widgetsize.real();
		}

		// line break: row is full, set the row's rects and start with new row
		if ( x0 + xstep > maxwcol )
		{
#ifdef DEBUG_MWIDGETGRID
			debuglog << "GdiWidgetGrid::layoutWidgetsFloatCluster break widgetsize " << widgetsize << " xstep " << xstep
				<< " x0 " << x0 << " maxwcol " << maxwcol;
#endif	
			xx = 0;
			for( auto w = colstartiter; w != iter; ++w )
			{
				//const double ww = useCommonWidth ? commonwidth : (*w)->doLayout( painter ).real();
				double ww = commonwidth;
				if ( ! useCommonWidth )
				{
					(*w)->doLayout( painter );
					ww = (*w)->getRect().x;
				}
				(*w)->setRect( xx, ycurrent, ww, hcurrentrow );
				xx += xstep;
#ifdef DEBUG_MWIDGETGRID
				debuglog << "GdiWidgetGrid::layoutWidgetsFloatCluster break commonwidth " << commonwidth << " ww " << ww
					<< " widget rect " << (*w)->getRect() << " xx " << xx;
#endif	
			}
			//++iter;

			// reset for next row
			xtotal = std::max( xtotal, xx );
			x0 = 0;
			ycurrent += hcurrentrow;
			hcurrentrow = 0;
			colstartiter = iter;
		}

#ifdef DEBUG_MWIDGETGRID
		debuglog << "GdiWidgetGrid::layoutWidgetsFloatCluster step2 widgetsize " << widgetsize << " xstep " << xstep
			<< " x0 " << x0 << " maxwcol " << maxwcol;
#endif	
		x0 += xstep;
		hcurrentrow = std::max( hcurrentrow, widgetsize.imag());
		++iter;
#ifdef DEBUG_MWIDGETGRID
		debuglog << "GdiWidgetGrid::layoutWidgetsFloatCluster endloop xtotal " << xtotal;
#endif	
	}

	// add pending items of last row
	xx = 0;
	while( colstartiter != widgets.end() )
	{
		//const double ww = useCommonWidth ? commonwidth : (*colstartiter)->doLayout( painter ).real();
		double ww = commonwidth;
		if ( ! useCommonWidth )
		{
			(*colstartiter)->doLayout( painter );
			ww = (*colstartiter)->getRect().x;
		}
		(*colstartiter)->setRect( xx, ycurrent, ww, hcurrentrow );
		xx += commonwidth;
		++colstartiter;
	}
	xtotal = std::max( xtotal, xx );
	ytotal = ycurrent + hcurrentrow;

#ifdef DEBUG_MWIDGETGRID
  debuglog << "Finished GdiWidgetGrid::layoutWidgetsFloatCluster xtotal " << xtotal << " ytotal " << ytotal;
#endif	
	return MComplex( xtotal, ytotal );
}

/*****************************************************
**
**   GdiWidgetGrid   ---   clearWidgets
**
******************************************************/
void GdiWidgetGrid::clearWidgets()
{
	for( GdiWidget *w : widgets )
	{
		delete w;
	}
  widgets.clear();
}

/*****************************************************
**
**   GdiWidgetGrid   ---   doPaint
**
******************************************************/
void GdiWidgetGrid::doPaint( Painter *painter, const MRect& refreshRect )
{
#ifdef DEBUG_MWIDGETGRID
	debuglog << "GdiWidgetGrid::doPaint refreshRect " << refreshRect << " widget count " << widgets.size();
#endif	
	for( GdiWidget *w : widgets )
	{
		assert( w );
#ifdef DEBUG_MWIDGETGRID
		debuglog << "GdiWidgetGrid::doPaint widget rect " << w->rect;
#endif	
		if ( w->getRect().intersects( refreshRect ))
		{
			w->doPaint( painter, refreshRect );
		}
	}
}

/*****************************************************
**
**   GdiWidgetGrid   ---   OnDataChanged
**
******************************************************/
void GdiWidgetGrid::OnDataChanged()
{
#ifdef DEBUG_MWIDGETGRID
	debuglog << "GdiWidgetGrid::OnDataChanged size " << widgets.size();
#endif	
	for( GdiWidget *w : widgets )
	{
		w->OnDataChanged();
	}
}

/*****************************************************
**
**   GdiWidgetGrid   ---   onCoordinatesChanged
**
******************************************************/
void GdiWidgetGrid::onCoordinatesChanged()
{
#ifdef DEBUG_MWIDGETGRID
	debuglog << "GdiWidgetGrid::onCoordinatesChanged size " << widgets.size();
#endif	
	for( GdiWidget *w : widgets )
	{
		w->onCoordinatesChanged();
	}
}

/*****************************************************
**
**   GdiWidgetGrid   ---   onResponsiveChartEvent
**
******************************************************/
WidgetResponse GdiWidgetGrid::onResponsiveChartEvent( const MComplex &p, const int &keymod )
{
	/*
	* This method runs through all widget because reset may be required if mouse pointer
	* move from one widget rect to another.
	*/
	WidgetResponse response( false );
	for( GdiWidget *w : widgets )
	{
		const WidgetResponse wr = w->onResponsiveChartEvent( p, keymod );
		if ( wr.doRefresh )
		{
			response.doRefresh = true;
			response.refreshRect = response.refreshRect.join( wr.refreshRect );
			if ( lastActiveResponseWidget &&  w != lastActiveResponseWidget )
			{
				response.refreshRect = response.refreshRect.join( lastActiveResponseWidget->getRect() );
				lastActiveResponseWidget->OnDataChanged();
				lastActiveResponseWidget->onCoordinatesChanged();
			}
			lastActiveResponseWidget = w;
		}
		if ( ! wr.message.empty()) response.message = wr.message;
	}
#ifdef DEBUG_MWIDGETGRID
	debuglog << "GdiWidgetGrid::onResponsiveChartEvent doRefresh " << response.doRefresh
		<< " refreshRect " << response.refreshRect;
#endif	
	return response;
}

/*****************************************************
**
**   GdiWidgetGrid   ---   handleMouseLeftEvent
**
******************************************************/
WidgetResponse GdiWidgetGrid::handleMouseLeftEvent( const MComplex &p )
{
	GdiWidget *w = findWidgetForPosition( p );
	if ( w ) return w->handleMouseLeftEvent( p );
	return false;
}

/*****************************************************
**
**   GdiWidgetGrid   ---   findWidgetForPosition
**
******************************************************/
GdiWidget *GdiWidgetGrid::findWidgetForPosition( const MComplex &p )
{
	for( GdiWidget *w : widgets )
	{
		if ( w->getRect().containsPoint( p )) return w;
	}
	return nullptr;
}

/*****************************************************
**
**   GdiWidgetGrid   ---   getSkin
**
******************************************************/
const WidgetSkin &GdiWidgetGrid::getSkin() const
{
	WidgetSkinLoader *loader = WidgetSkinLoader::get();
	if ( widgets.size() > 0 )
	{
		return loader->getConfig( props->getSkin( widgets.front()->getWidgetCategory() ));
	}
	return loader->getDefaultConfig();
	/*
	const unsigned skin = context->getProps()->getSkin( getWidgetCategory() );
	WidgetSkinLoader *loader = WidgetSkinLoader::get();
	return loader->getConfig( skin );
	*/
}

