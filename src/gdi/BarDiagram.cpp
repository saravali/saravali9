/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/BarDiagram.cpp
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

#include "BarDiagram.h"

#include "astroconfig.h"
#include "ColorConfig.h"
#include "ContextProperties.h"
#include "DocHelper.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Lang.h"
#include "Painter.h"
#include "SymbolProvider.h"

#include <format>
#include <variant>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   BarDiagram   ---   Constructor
**
******************************************************/
BarDiagram::BarDiagram( ContextProps_ptr props, wxString title,
	const BarDiagramConfig *const bconfig )
	: GdiWidget( props, WidgetCategory::BardiagramGeneric ),
	title( title ),
	barconfig( bconfig ? bconfig :  &gdiconfig->bardiagram )
{
	minvalue = maxvalue = 0;
}

/*****************************************************
**
**   BarDiagram   ---   Destructor
**
******************************************************/
BarDiagram::~BarDiagram()
{
	clear();
}

/*****************************************************
**
**   BarDiagram   ---   clear
**
******************************************************/
void BarDiagram::clear()
{
	items.clear();
	maxvalue = minvalue = 0;
}

/*****************************************************
**
**   BarDiagram   ---   addItem
**
******************************************************/
void BarDiagram::addItem( const std::variant<PlanetId, Sign> &a, const double value )
{
	items.push_back({ a, value, gdiconfig->bardiagram.getConfigId( a ) });
}

#define BAR_DIAGRAM_CONTENTRECT_TOL .01
#define BAR_DIAGRAM_CONTENTRECT_CORNER_RADIUS .01

/*****************************************************
**
**   BarDiagram   ---   doPaint
**
******************************************************/
void BarDiagram::doPaint( Painter *painter, const MRect& /*refreshRect*/ )
{
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Vedic );
	debuglog << "Start BarDiagram::doPaint viewpoint " << viewpoint;

	//const int fontsize = gdiconfig->fonts.graphicFontSize;

	double value, currenty;
	DocHelper util( painter );

	minvalue = maxvalue = 0.0;
	for( auto &item : items )
	{
		maxvalue = std::max( item.value, maxvalue );
		minvalue = std::min( item.value, minvalue );
	}

	// Find a reasonable value for maxvalue
	double dmaxvalue =  maxvalue * 1.1;
	if ( dmaxvalue > 40 ) dmaxvalue = 20 * ceil( dmaxvalue / 20 );
	else dmaxvalue = 8 * ceil( dmaxvalue / 8 );

	// dmaxvalue = 0 may happen if all values are zero (e.g. Yuddha Bala)
	if ( dmaxvalue == 0 ) dmaxvalue = 60;

	// Total y value
	const double valuespan = dmaxvalue - minvalue;

	// calculate content rect
	const double xm0 = rect.width * BAR_DIAGRAM_CONTENTRECT_TOL;
	const double ym0 = rect.height * BAR_DIAGRAM_CONTENTRECT_TOL;
	MRect contentRect = rect;
	contentRect.x += xm0;
	contentRect.y += ym0;
	contentRect.width -= 2 * xm0;
	contentRect.height -= 2 * ym0;
	const double cornerRadius = rect.width * BAR_DIAGRAM_CONTENTRECT_CORNER_RADIUS;

	// draw frame on content rect
	painter->setTransparentBrush();
	painter->setPen( getSkinnedPen());
	painter->setTextColor( getSkinnedTextColor());
	painter->drawRectangle( contentRect, cornerRadius );

	// font scaling
	//setGraphicFont( painter );
	painter->setGraphicFont();
	MComplex p = painter->getCharacterExtent();
	text_height = p.imag();

	const MRect rulerRect(
		contentRect.x + deltasteps * xdmargin,
		contentRect.y + 2 * ymargin,
		contentRect.width - ( 1 + deltasteps ) * xdmargin,
		contentRect.height - 4 * ydmargin
	);

	const double xstep = rulerRect.width / items.size();
	const double xtol = xstep / 10;
	const double ystep = rulerRect.height / 4;

	// paint ruler
	painter->drawLine( rulerRect.x, rulerRect.y + rulerRect.height,
		rulerRect.x + rulerRect.width, rulerRect.y + rulerRect.height );
	painter->drawLine( rulerRect.x, rulerRect.y + rulerRect.height, rulerRect.x, rulerRect.y );

	// draw another horizontal line in case of negative y values
	if ( minvalue < 0 )
	{
		const double y0line = rulerRect.y + rulerRect.height * ( 1 + minvalue / valuespan );
		painter->drawLine( rulerRect.x, y0line, rulerRect.x + rulerRect.width, y0line );
	}

	// numbers at left side of ruler
	for ( int i = 0; i <= 4; i++ )
	{
		currenty = rulerRect.y + rulerRect.height - i * ystep;
		painter->drawLine( rulerRect.x - ydmargin, currenty, rulerRect.x, currenty );
		util.drawTextFormatted(
			std::format( "{}", (int)( minvalue + .25 * i * valuespan )),
			MRect( rulerRect.x - xstep - ydmargin, currenty - ymargin, xstep, 2 * ymargin ),
			Align::West );
	}

	// draw title
	if ( ! title.empty())
	{
		util.drawTextFormatted(
			title,
			MRect( contentRect.x, rulerRect.y, contentRect.width, contentRect.height - 2 * ymargin ),
			Align::North );
	}

	// draw bar items
	double xcurrent = rulerRect.x;
	for ( auto &item : items )
	{
		value = fabs( rulerRect.height * item.value / valuespan );
		double ybarbottom = rulerRect.y + rulerRect.height;

		if ( minvalue < 0 )
		{
			if ( item.value >= 0 )
			{
				// shift bars for positive values
				ybarbottom += rulerRect.height * minvalue / valuespan;
			}
			else
			{
				ybarbottom += rulerRect.height * ( minvalue - item.value ) / valuespan;
			}
		}
		item.rect = MRect( xcurrent + xtol, ybarbottom - value, xstep - 2 * xtol, value );
		xcurrent += xstep;

		painter->setGraphicFont();
		//setGraphicFont( painter );
		drawBar( painter, item );
	}
	painter->setDefaults();
	//setGraphicFont( painter );
}

/*****************************************************
**
**   BarDiagram   ---   drawBar
**
******************************************************/
void BarDiagram::drawBar( Painter *painter, const BarDiagramItem &item )
{
	wxString s;
	DocHelper util( painter );
	const Viewpoint viewpoint = getViewpoint();
	const ViewpointConfig &vCfg = astroconfig->getViewpointConfig( viewpoint );

	if ( item.value != 0 )
	{
		const BarDiagramStyle &barstyle = barconfig->styles[item.configId];

		painter->setPen( barstyle.pen );
		painter->setTextColor( barstyle.pen.GetColour() );
		painter->setBrush( barstyle.brush );
	}
	else { painter->setDefaults(); }

	const double xright = item.rect.x + item.rect.width;
	const double ybottom = item.rect.y + item.rect.height;

	std::vector<MComplex> rightRhomb = {
		MComplex( xright, ybottom ),
		MComplex( xright + x3d, ybottom - y3d ),
		MComplex( xright + x3d, item.rect.y - y3d ),
		MComplex( xright, item.rect.y  )
	};
	painter->drawPolygon( rightRhomb );

	std::vector<MComplex> topRhomb = {
		MComplex( item.rect.x, item.rect.y ),
		MComplex( item.rect.x + x3d, item.rect.y - y3d ),
		MComplex( xright + x3d, item.rect.y - y3d ),
		MComplex( xright, item.rect.y )
	};
	painter->drawPolygon( topRhomb );
	painter->drawRectangle( item.rect );

	s = std::format( "{}", (int)( item.value > 0 ?  item.value + .5 : item.value - .5 ) ); // rounded value

	if ( item.rect.height > text_height ) util.drawTextFormatted( s, item.rect, Align::Center );
	else
	{
		util.drawTextFormatted(
			s,
			MRect( item.rect.x, item.rect.y - 2 * text_height, item.rect.width, text_height ),
			Align::South );
	}

	// Finally draw object name at the bottom of the bar
	painter->setTextColor( getSkinnedTextColor());
	const MRect trect( item.rect.x, ybottom, item.rect.width, text_height );

	if ( std::holds_alternative<PlanetId>( item.object ))
	{
		if ( vCfg.planets.textFormat == TextFormat::Symbol )
		{
			painter->switchToSymbolFont( true );
			util.drawTextFormatted(
				SymbolProvider().getPlanetCode( std::get<PlanetId>( item.object ), 0, 0 ),
				trect, 
				Align::North );
			painter->switchToSymbolFont( false );
		}
		else
		{
			util.drawTextFormatted(
				Lang().getPlanetName( std::get<PlanetId>( item.object ), viewpoint, TextFormat::Short ),
				trect, 
				Align::North );
		}
	}
	else if ( std::holds_alternative<Sign>( item.object ))
	{
		if ( vCfg.planetProps.textFormat == TextFormat::Symbol )
		{
			painter->switchToSymbolFont( true );
			util.drawTextFormatted(
				SymbolProvider().getSignCode( std::get<Sign>( item.object ), 0 ),
				trect, 
				Align::North );
			painter->switchToSymbolFont( false );
		}
		else
		{
			util.drawTextFormatted(
				Lang().getSignName( std::get<Sign>( item.object ), viewpoint, TextFormat::Short ),
				trect, 
				Align::North );
		}
	}
}


