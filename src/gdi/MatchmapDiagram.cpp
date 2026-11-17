/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/MatchmapDiagram.cpp
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

#include "MatchmapDiagram.h"

#include "Calculator.h"
#include "ConfigListItems.h"
#include "EventFinder.h"
#include "Formulary.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "mathbase.h"
#include "Painter.h"
#include "Term.h"
#include "DocHelper.h"
#include "Uranian.h"

#include <format>

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   MatchmapDiagram   ---   Constructor
**
******************************************************/
MatchmapDiagram::MatchmapDiagram( ContextProps_ptr props, EventFinder *finder )
	: TimelineDiagram( props, WidgetCategory::PpTree ),
	finder( finder )
{
	assert( finder );
	usection = finder->getUranianSection();
}

/*****************************************************
**
**   MatchmapDiagram   ---   doLayout
**
******************************************************/
MComplex MatchmapDiagram::doLayout( Painter *painter )
{ 
	debuglog << "Start MatchmapDiagram::doLayout pmax " << " bestlayout " << bestLayout;
	
	TimelineDiagram::doLayout( painter );

	UranianHelper *uh = UranianHelper::get();
	UranianSection *usection = getUranianSection();

	divisors = uh->getDivisors( usection->matchDivision );

	painter->setGraphicFont();
	DocHelper spt( painter );

	elements.clear();
	for( const auto &match : finder->getMatches() )
	{

		double y0 = match2y( match );

		MatchmapDiagramElement elem( MComplex( jd2x( match.duedate ), match2y( match )));
		debuglog << "Match " << match.formulaId << " y " << y0 << " anchor " << elem.anchor;

		elem.contents.tf << *match.e1 << "=" << *match.e2;
		spt.formatAndFitTextItem( elem.contents );
		elem.contents.moveTo( elem.anchor );

		elements.push_back( elem );
	}


	if ( bestLayout.real() && bestLayout.imag() ) bestLayout = MComplex( 800, 600 );
	return bestLayout;
}

/*****************************************************
**
**   MatchmapDiagram   ---   OnDataChanged
**
******************************************************/
void MatchmapDiagram::OnDataChanged()
{
	clear();

	assert( finder );
	startJD = finder->getStartJD();
	endJD = finder->getEndJD();
	const double duration = endJD - startJD;
	debuglog << "MatchmapDiagram::OnDataChanged duration " << duration;
}

/*****************************************************
**
**   MatchmapDiagram   ---   calculateY
**
******************************************************/
double MatchmapDiagram::calculateY( const uint &formulaId, const int &divisor ) const
{
	UranianSection *usection = getUranianSection();
	assert( usection );

	//double yPerFormula = rcontent.height - ( topMargin + bottomMargin );
	double yPerFormula = rcontent.height;
	if ( usection->matchFormulae.size() > 0 ) yPerFormula /= usection->matchFormulae.size();

	double yPerGk = yPerFormula;
	//if ( gradkreises.size() > 0 ) yPerGk /= gradkreises.size();
	if ( divisors.size() > 0 ) yPerGk /= divisors.size();

	int pos = 0;
	for( int i = 0; i < (int)divisors.size(); ++i )
	{
		if( divisors[i] == divisor )
		{
			pos = i;
			break;
		}
	}

	debuglog << "MatchmapDiagram::calculateY formulaId " << formulaId << " divisor " << divisor
		<< "formulae.size() " << usection->matchFormulae.size() << " divisors.size() " << divisors.size()
		<< " yPerFormula " << yPerFormula << " yPerGk " << yPerGk << " pos " << pos;

	//return rcontent.y + topMargin + formulaId * yPerFormula + pos * yPerGk;
	return rcontent.y + formulaId * yPerFormula + pos * yPerGk;
}

/*****************************************************
**
**   MatchmapDiagram   ---   match2y
**
******************************************************/
double MatchmapDiagram::match2y( const Match &match ) const
{
	return calculateY( match.formulaId, match.division );
}

/*****************************************************
**
**   MatchmapDiagram   ---   drawCross
**
******************************************************/
/*
void MatchmapDiagram::drawCross( Painter *painter, const MComplex &p, const double &w ) const
{
	painter->drawLine( p.real() - w, p.imag() - w, p.real() + w, p.imag() + w );
	painter->drawLine( p.real() - w, p.imag() + w, p.real() + w, p.imag() - w );
}
*/

/*****************************************************
**
**   MatchmapDiagram   ---   x2jd
**
******************************************************/
double MatchmapDiagram::x2jd( const double& ) const
{
	// TODO
	return 0;
}

/*****************************************************
**
**   MatchmapDiagram   ---   doPaint
**
******************************************************/
void MatchmapDiagram::doPaint( Painter *painter, const MRect& /*refreshRect*/ )
{
	debuglog << "MatchmapDiagram::doPaint rect " << rect;
	UranianSection *usection = getUranianSection();
	assert( usection );
	Formulary *formulary = Formulary::get();

	painter->setTransparentBrush();

	painter->setPen( getSkinnedPen());

	painter->drawRectangle( rcontent );
	drawRuler( painter );

	wxColour pencolor;
	const WidgetSkin &skin = getSkin();
  if ( skin.pen.IsOk() ) { pencolor = skin.pen.GetColour(); }
	else pencolor = gdiconfig->colors.defaultPen.GetColour();

	wxPen pen1( pencolor, 1, wxPENSTYLE_SHORT_DASH );
	wxPen pen2( pencolor, 1, wxPENSTYLE_DOT );
	
	for( const int i : usection->matchFormulae )
	{
		for( const auto &d : divisors )
		{
			painter->setPen( d == 1 ? pen1 : pen2 );
			const double y0 = calculateY( i, d );
			painter->drawLine( rcontent.x, y0, rcontent.x + rcontent.width, y0 );
		}
	}

	painter->setPen( getSkinnedPen());

	DocHelper spt( painter );
	for( int def : usection->matchFormulae )
	{
		for( const auto &d : divisors )
		{
			const wxString label = d == 1 ?
				formulary->getFormulaName( def, TextFormat::Short )
				: std::format( "{}", d );

			const double y = calculateY( def, d );
			drawLeftLabel( painter, MString( label ), y );
			drawRightLabel( painter, MString( label ), y );
		}
	}

	//constexpr double aa = 5;
	for( const auto &elem : elements )
	{
		painter->drawCrossMarker( elem.anchor );
		/*
		painter->drawLine( elem.anchor.real() - aa, elem.anchor.imag() - aa, elem.anchor.real() + aa, elem.anchor.imag() + aa );
		painter->drawLine( elem.anchor.real() - aa, elem.anchor.imag() + aa, elem.anchor.real() + aa, elem.anchor.imag() - aa );
		*/
		spt.drawTextItem( elem.contents );
	}

	painter->setDefaults();
	painter->setGraphicFont();
}


