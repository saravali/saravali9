/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/PpTree.cpp
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

#include "PpTree.h"

#include "ContextProperties.h"
#include "enumserializer.h"
#include "Formatter.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "Lang.h"
#include "mathbase.h"
#include "Painter.h"
#include "Term.h"
#include "DocHelper.h"
#include "UranianHelper.h"

#include <format>

extern Gdiconfig *gdiconfig;

//#define DEBUG_PPTREE

/*****************************************************
**
**   PpTree   ---   Constructor
**
******************************************************/
PpTree::PpTree( ContextProps_ptr props,
	const PlanetaryPicture p, UranianSection *usection )
	: GdiWidget( props, WidgetCategory::PpTree, usection ),
	picture( p ),
	index( ++global_index )
{
	clear();
}

/*****************************************************
**
**   PpTree   ---   clear
**
******************************************************/
void PpTree::clear()
{
	telements.clear();
	ltitle.tf.clear();
	mtitle.tf.clear();
	rtitle.tf.clear();

	bestLayout = 0;
	xs = ys = xm = labelColWidth = 0;
	wTerm = wlTerm = wrTerm = wlLabel = wrLabel = wLabel = 0;
}

/*****************************************************
**
**   PpTree   ---   doLayout
**
**  calculates sizes and writes text items
**
******************************************************/
MComplex PpTree::doLayout( Painter *painter )
{ 
#ifdef DEBUG_PPTREE
	debuglog << "Start PpTree::doLayout #" << index << " bestlayout " << bestLayout << " Viewpoint " << getViewpoint();
#endif

	const Viewpoint viewpoint = getViewpoint();
	UranianSection *usection = getUranianSection();
	const int division = usection ? usection->matchDivision : 1;
	Formatter *formatter = Formatter::get();
	DocHelper util( painter );
	UranianHelper *uh = UranianHelper::get();

	setGraphicFont( painter );
	const MComplex te = painter->getCharacterExtent();
	xs = te.real();
	ys = 1.5 * te.imag();

	MString longitude;
	extern Astroconfig *astroconfig;
	const PlanetConfig planetCfg = astroconfig->getViewpointConfig( viewpoint ).planets;

	// write longitude (of leading term) string
	{
		PlanetPropertyConfig propCfg = astroconfig->getViewpointConfig( viewpoint ).planetProps;
		trimTextFormat( propCfg.textFormat, TextFormat::Short );

		propCfg.positionStyle = PositionStyle::Compact;
		propCfg.precision = DegPrecision::Minute;
		longitude << formatter->formatZodiacalLongitude( propCfg, picture.longitude, MovingDirection::None, division );
	}

	ltitle = DocItemText( Align::West );
	mtitle = DocItemText( Align::Center );
	rtitle = DocItemText( Align::East );

	if ( picture.leadingElem.term.getTermId() == TermId::None )
	{
		mtitle.tf = longitude;
	}
	else
	{
		mtitle.tf = picture.leadingElem.term.toMString( planetCfg );


		const auto roles = props->getActiveRoles();
		mtitle.tf << MToken( Lang().getRoleContextSubscriptum( picture.leadingRole ), TokenFontPosition::Subscriptum );

		if ( usection->showPpLongitude )
		{
			ltitle.tf = longitude;
			util.formatAndFitTextItem( ltitle );
		}
	}
	util.formatAndFitTextItem( mtitle );

	if ( usection->showPpScore )
	{
		rtitle.tf = std::format( "{:.1f}", picture.score );
		util.formatAndFitTextItem( rtitle );
	}

	wTerm = 0;
	wlTerm = 0;
	wrTerm = 0;
	wlLabel = 0;
	wrLabel = 0;
	wLabel = 0;

	telements.clear();
	for( const PpElement &elem : picture.elements )
	{
		PpTreeElement welem( elem.term.getTermId());

		// left term
		if ( elem.term.getVariableCount() > 0 )
		{
			welem.lTerm.tf << elem.term.firstMString( planetCfg );
			util.formatAndFitTextItem( welem.lTerm );
			wlTerm = std::max( wlTerm, welem.lTerm.width );
		}

		// right term
		if ( elem.term.getVariableCount() > 1 )
		{
			welem.rTerm.tf << elem.term.secondMString( planetCfg );
			util.formatAndFitTextItem( welem.rTerm );
			wrTerm = std::max( wrTerm, welem.rTerm.width );
		}

		// division
		welem.lLabel.tf = uh->divisionDString( elem.division );
		util.formatAndFitTextItem( welem.lLabel );
		wlLabel = std::max( wlLabel, welem.lLabel.width );

		// orbis
		{
			welem.rLabel.tf << std::format( "{}", (int)(elem.orbis * 60 ));
			util.formatAndFitTextItem( welem.rLabel );
			wrLabel = std::max( wrLabel, welem.rLabel.width );
		}
		telements.push_back( welem );
	}

	// extend right column if empty
	if ( wrTerm == 0 ) wrTerm = wlTerm;
	wTerm = std::max( wlTerm, wrTerm );
	wLabel = std::max( wlLabel, wrLabel );

	/*
	* additional x space is calculated as follows
	* both sides of the labels (=4)
	* two in the center and one on both sides of the center (=4)
	*/
	//rcontent = MRect( 0, 0, wlTerm + wrTerm + wlLabel + wrLabel + /*see comment*/ 7 * xs,
	rcontent = MRect( 0, 0, 2 * ( wTerm + wLabel ) + /*see comment*/ 7 * xs,
		( 2 + picture.elements.size()) * ys );
	bestLayout = MComplex( rcontent.width + 2 * xs, rcontent.height + 2 * ys );
	rect = MRect( 0, 0, bestLayout.real(), bestLayout.imag());

#ifdef DEBUG_PPTREE
	debuglog << "Finished PpTree::doLayout #" << index << " bestlayout " << bestLayout;
#endif
	return bestLayout;
}

/*****************************************************
**
**   PpTree   ---   OnDataChanged
**
******************************************************/
void PpTree::OnDataChanged()
{
	clear();
}

/*****************************************************
**
**   PpTree   ---   setGraphicFont
**
******************************************************/
void PpTree::setGraphicFont( Painter *painter )
{
	const int pointSize = gdiconfig->fonts.graphicFontSize * props->getZoom();
	painter->setGraphicFont( pointSize );
}

/*****************************************************
**
**   PpTree   ---   onCoordinatesChanged
**
**  moves the items to te right place
**
******************************************************/
void PpTree::onCoordinatesChanged()
{
	rcontent.moveTo( rect.x + xs, rect.y + ys );
#ifdef DEBUG_PPTREE
	debuglog << "PpTree::onCoordinatesChanged #" << index << " rect " << rect
		<< " rcontent " << rcontent;
#endif

	xm = rcontent.x + .5 * rcontent.width;
	double y0 = rcontent.y + .5 * ys;

	// distance of label end from outer side on content rect
	labelColWidth =  .5 * rcontent.width - wLabel - 3 * xs;
#ifdef DEBUG_PPTREE
	debuglog << "PpTree::onCoordinatesChanged labelColWidth " << labelColWidth
		<< " wLabel " << wLabel << " wTerm " << wTerm << " xs " << xs;
#endif

	const double xlcol = rcontent.x + xs;
	const double xrcol = rcontent.x + rcontent.width - xs;

	ltitle.moveTo( xlcol, y0 );
	mtitle.moveTo( xm - .5 * mtitle.width, y0 );
	rtitle.moveTo( xrcol - rtitle.width, y0 );
	/*
	score.moveTo( rcontent.x + rcontent.width - score.width, y0 );
	orbisheader.moveTo( xrcol - orbisheader.width, y0 );
	*/
	y0 += ys;

	// set item positions
	for( PpTreeElement &telem : telements )
	{
		telem.lLabel.moveTo( xlcol, y0 );

		telem.lTerm.moveTo( xm - 2 * xs - telem.lTerm.width, y0 );
		telem.rTerm.moveTo( xm + 2 * xs, y0 );

		telem.rLabel.moveTo( xrcol - telem.rLabel.width, y0 );
		y0 += ys;
	}
	rcontent.moveTo( rect.x + xs, rect.y + ys );
}

/*****************************************************
**
**   PpTree   ---   doPaint
**
******************************************************/
void PpTree::doPaint( Painter *painter, const MRect& /*refreshRect*/ )
{
#ifdef DEBUG_PPTREE
	debuglog << "PpTree::doPaint #" << index << " rect " << rect << " element count " << telements.size();
#endif
	painter->setTransparentBrush();

	painter->setPen( *wxLIGHT_GREY_PEN );
	painter->drawRectangle( rect );

#if 0
	{
		double xline = rcontent.x + labelColWidth;
		painter->setPen( wxColour( "orange" ));
		painter->drawLine( xline, rcontent.y, xline, rcontent.y + rcontent.height );
		xline = rcontent.x + rcontent.width - labelColWidth;
		painter->drawLine( xline, rcontent.y, xline, rcontent.y + rcontent.height );
	}
#endif

	painter->setPen( getSkinnedPen());
	painter->setTextColor( getSkinnedTextColor());
	setGraphicFont( painter );

	DocHelper util( painter );

	const double cornerRadius = xs;
	painter->drawRectangle( rcontent, cornerRadius );

	if ( ! ltitle.tf.isEmpty())
	{
		util.drawTextItem( ltitle );
	}
	util.drawTextItem( mtitle );
	if ( ! rtitle.tf.isEmpty())
	{
		util.drawTextItem( rtitle );
	}

	for( PpTreeElement &elem : telements )
	{
		switch( elem.termId )
		{
			using enum TermId;

			case Single:
			{
				const double y = elem.lTerm.rect.y + .5 * elem.lTerm.rect.height;

				// vertical line
				painter->drawLine( xm, y - 1.5 * xs, xm, y + 1.5 * xs );

				// half horizontal line
				painter->drawLine( xm - 1.5 * xs, y, xm, y );
			}
			break;
			case Midpoint:
			{
				const double y = elem.lTerm.rect.y + .5 * elem.lTerm.rect.height;

				// vertical line
				painter->drawLine( xm, y - 1.5 * xs, xm, y + 1.5 * xs );

				// full horizontal line
				painter->drawLine( xm - 1.5 * xs, y, xm + 1.5 * xs, y );
			}
			break;
			default:
			{
				// paint only operator sign
				const MRect r( elem.lTerm.rect.getRight(), elem.lTerm.rect.y, elem.rTerm.rect.x - elem.lTerm.rect.getRight(), elem.lTerm.rect.height );
				DocItemText op( MTerm::getOperatorRepresentation( elem.termId ), r, Align::Center );
				util.formatAndDrawTextItem( op );
			}
			break;
		}

		util.drawTextItem( elem.lLabel );
		util.drawTextItem( elem.lTerm );
		util.drawTextItem( elem.rTerm );
		util.drawTextItem( elem.rLabel );

#if 0
		painter->drawRectangle( elem.lTerm.rect );
		painter->drawRectangle( elem.rTerm.rect );
#endif
	}

	painter->setDefaults();
	setGraphicFont( painter );
}


