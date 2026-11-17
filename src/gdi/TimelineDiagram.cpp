/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/TimelineDiagram.cpp
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

#include "TimelineDiagram.h"

#include "Calculator.h"
#include "ContextProperties.h"
#include "DocHelper.h"
#include "Lang.h"
#include "Painter.h"
#include "DocHelper.h"

#include <format>

/**************************************************************
***
**   TimelineDiagram   ---   Constructor
***
***************************************************************/
TimelineDiagram::TimelineDiagram( ContextProps_ptr props, const WidgetCategory &category )
	: GdiWidget( props, category )
{
	clear();
	rcontent = MRect( 100, 100, 500, 600 );

	gridPen = wxPen( *wxLIGHT_GREY, 1, wxPENSTYLE_DOT );
	setDivision( 1 );
}

/*****************************************************
**
**   TimelineDiagram   ---   clear
**
******************************************************/
void TimelineDiagram::clear()
{
	startJD = endJD = 0.0;
	rcontent.reset();
	shortTic = 3;
	longTic = 5;
}

/*****************************************************
**
**   TimelineDiagram   ---   doLayout
**
******************************************************/
MComplex TimelineDiagram::doLayout( Painter *painter )
{
	debuglog << "doLayout";
	//painter->setGraphicFont( props->getZoom() );
	painter->setGraphicFont();
	textExtent = painter->getCharacterExtent();
	debuglog << "TimelineDiagram::initParams text extent " << textExtent;

	const double xmargin = 5 * textExtent.real();
	const double topMargin = textExtent.imag();
	const double bottomMargin = 3 * textExtent.imag();
	rcontent = MRect( xmargin, topMargin, rect.width - 2 * xmargin, rect.height - ( topMargin + bottomMargin ));

	return rect.getCenter();
}

/**************************************************************
***
**   TimelineDiagram   ---   setDivision
***
***************************************************************/
void TimelineDiagram::setDivision( const int& d )
{
	division = d;
	assert( division );
  gkDegrees = 360.0 / division;
	vmin = v0 = 0.0;
	vmax = gkDegrees;
}

/*****************************************************
**
**   TimelineDiagram   ---   jd2x
**
******************************************************/
double TimelineDiagram::jd2x( const double &jd ) const
{
	assert( endJD > startJD );
	assert( ! rcontent.isNil());

	const double delta = ( jd - startJD ) / ( endJD - startJD );
	debuglog << "TimelineDiagram::jd2x delta " << delta;

	if ( jd < startJD || jd > endJD )
	{
		warnlog << std::format( "TimelineDiagram::jd2x has jd out of bounds jd {:15.6f} startJD {:15.6f} endJD {:15.6f} ",
			jd, startJD, endJD );
	}
	return rcontent.x + rcontent.width * ( jd - startJD ) / ( endJD - startJD );
}

/*****************************************************
**
**   TimelineDiagram   ---   drawGrid
**
******************************************************/
void TimelineDiagram::drawGrid( Painter *painter )
{
	painter->setPen( getSkinnedPen());
	painter->setTransparentBrush();
	painter->drawRectangle( rcontent );

	drawRuler( painter );
}

/*****************************************************
**
**   TimelineDiagram   ---   drawRuler
**
******************************************************/
void TimelineDiagram::drawRuler( Painter *painter )
{
	const double duration = endJD - startJD;
	assert( duration > 0 );
	debuglog << " TimelineDiagram::drawRuler duration is " << duration;

	if ( duration < 50 )
	{
		dailyRuler( painter );
	}
	else if ( duration < 400 )
	{
		monthlyRuler( painter );
	}
	else yearlyRuler( painter );

	horizontalRuler( painter );
}

/*****************************************************
**
**   TimelineDiagram   ---   horizontalRuler
**
******************************************************/
void TimelineDiagram::horizontalRuler( Painter *painter )
{

	//const int steplen = ( vmax - vmin ) / 10;
	int steplen = 5;
	if ( vmax >= 180 )
	{
		steplen = 30;
	}
	else if ( vmax >= 90 )
	{
		steplen = 10;
	}
	debuglog << "horizontalRuler vmin " << vmin << " vmax " << vmax << " v0 " << v0 << " Step len " << steplen;

	const double tic = longTic;

	double ystep = vmin + steplen;
	double y = 0;
	DocHelper helper( painter );

	while ( ystep < vmax )
	{
		y = valueToY( ystep );

		if ( doPaintGrid )
		{
			painter->setPen( gridPen );
			painter->drawLine( rcontent.x + tic, y, rcontent.getRight() - tic, y );
		}
		
		painter->setPen( getSkinnedPen());
		painter->drawLine( rcontent.x - tic, y, rcontent.x + tic, y );
		painter->drawLine( rcontent.getRight() - tic, y, rcontent.getRight() + tic, y );

		
	helper.formatAndDrawTextItem( MString( std::format( "{}", (int)ystep )),
		MRect( rcontent.getRight(), y - tic, rect.getRight() - rcontent.getRight(), 2 * tic ));
		ystep += steplen;
	}
}

/*****************************************************
**
**   TimelineDiagram   ---   dailyRuler
**
******************************************************/
void TimelineDiagram::dailyRuler( Painter *painter )
{
	Lang lang;
	wxString s;
	Calculator *calculator = CalculatorFactory().getCalculator();

	double currentJD = startJD;
	auto[ year, month, day, ut ] = calculator->getDateIntsFromJD( currentJD );

	currentJD = calculator->calc_jd( year, month, day, 0 );
	double x;

	while( currentJD <= endJD )
	{
		if ( doPaintGrid && currentJD != startJD && currentJD != endJD )
		{
			x = jd2x( currentJD );
			painter->setPen( gridPen );
			painter->drawLine( x, rcontent.y, x, rcontent.getBottom());
		}

		if ( day == 1 )
		{
			s = lang.getMonthName( month - 1, TextFormat::Short );
		}
		else
		{
			s = std::format( "{}", day );
		}
		painter->setPen( getSkinnedPen());
		paintRulerMarker( painter, currentJD, s, day == 1 || ! ( day % 10 ) );

		++currentJD;
		std::tie( year, month, day, ut ) = calculator->getDateIntsFromJD( currentJD );
	}
}

/*****************************************************
**
**   TimelineDiagram   ---   monthlyRuler
**
******************************************************/
void TimelineDiagram::monthlyRuler( Painter *painter )
{
	DocHelper spt( painter );
	wxString s;
	Lang lang;
	Calculator *calculator = CalculatorFactory().getCalculator();

	double currentJD = startJD;
	auto[ year, month, day, dummy ] = calculator->getDateIntsFromJD( currentJD );
	currentJD = calculator->calc_jd( year, month, 1, 0 );

	while( currentJD < endJD )
	{
		if ( month == 1 )
		{
			s = std::format( "{}", year );
		}
		else
		{
			s = lang.getMonthName( month - 1, TextFormat::Short );
		}
		paintRulerMarker( painter, currentJD, s, month % 3 == 0 );

		if ( ++month > 12 )
		{
			month = 1;
			++year;
		}
		currentJD = calculator->calc_jd( year, month, day, 0 );
	}
}

/*****************************************************
**
**   TimelineDiagram   ---   yearlyRuler
**
******************************************************/
void TimelineDiagram::yearlyRuler( Painter* )
{
	// TODO
}

/*****************************************************
**
**   TimelineDiagram   ---   calculateRulerJDList
**
******************************************************/
std::list<double>  TimelineDiagram::calculateRulerJDList( const double &startJD, const double &endJD,
	const int &mode ) const
{
	std::list<double> l;
	Calculator *calculator = CalculatorFactory().getCalculator();

	double jd = startJD;
	auto[ year, month, day, dummy ] = calculator->getDateIntsFromJD( jd );

	if ( mode == 0 ) // yearly
	{
		// start of current month
		jd = calculator->calc_jd( year, month, 1, 0 );
	}
	else
	{
		assert( false );
	}

	while( jd < endJD )
	{
		l.push_back( jd );

		if ( mode == 0 ) // yearly
		{
			if ( ++month > 12 )
			{
				month = 1;
				++year;
			}
		}
		else
		{
			assert( false );
		}
		jd = calculator->calc_jd( year, month, day, 0 );
	}
	return l;
}

/*****************************************************
**
**   TimelineDiagram   ---   paintRulerMarker
**
******************************************************/
void TimelineDiagram::paintRulerMarker( Painter *painter, const double &jd,
	const wxString label, const bool &isLong )
{
	if ( jd >= startJD && jd <= endJD )
	{
		DocHelper spt( painter );
		constexpr double xr = 10;
		//constexpr double yr = 20;

		const double x0 = jd2x( jd );
		const double y0 = rcontent.y + rcontent.height;
		const double ticlen = isLong ? longTic : shortTic;
		painter->drawLine( x0, y0 - ticlen, x0, y0 + ticlen );
		debuglog << "paintRulerMarker x0 " << x0 << " y0 " <<  y0 << " ticlen " << ticlen;

		spt.drawTextFormatted(
			label,
			MRect( x0 - xr, rect.y + rect.height - 2 * textExtent.imag(), 2 * xr, textExtent.imag() ),
			Align::North );
	}
	else
	{
		debuglog << std::format( "TimelineDiagram::paintRulerMarker jd out of range: {:.10f}", jd );
	}
}

/*****************************************************
**
**   TimelineDiagram   ---   drawLeftLabel
**
******************************************************/
void TimelineDiagram::drawLeftLabel( Painter *painter, const MString &ms, const double &y )
{
	DocHelper spt( painter );
	constexpr double rtol = 20;
	const double tic = longTic;

	painter->setPen( getSkinnedPen());
	painter->drawLine( rcontent.x - tic, y, rcontent.x + tic, y );

	spt.formatAndDrawTextItem( ms,
		MRect( rcontent.x - 2 * rtol, y - rtol, 2 * rtol, 2 * rtol ), Align::East );
}

/*****************************************************
**
**   TimelineDiagram   ---   drawRightLabel
**
******************************************************/
void TimelineDiagram::drawRightLabel( Painter *painter, const MString &ms, const double &y )
{
	DocHelper spt( painter );
	constexpr double rtol = 20;
	const double tic = longTic;

	painter->setPen( getSkinnedPen());
	painter->drawLine( rcontent.getRight() - tic, y, rcontent.getRight() + tic, y );

	spt.formatAndDrawTextItem( ms,
		MRect( rcontent.getRight(), y - rtol, 2 * rtol, 2 * rtol ), Align::West );
}

/**************************************************************
***
**   TimelineDiagram   ---   valueToY
***
***************************************************************/
double TimelineDiagram::valueToY( const double &l )
{
	if ( vmin == vmax )
	{
		warnlog << "cannot determine y value because vmin and vmax are identical " << vmax;
		return 0;
	}
	const double portion = ( l - vmin) / ( vmax - vmin );

	debuglog << " EphemDiagram::valueToY " << l << " vmax "
		<< vmax << " vmin " << vmin << " portion " << portion;

	const double yb = rcontent.getBottom();
	return yb - portion * ( yb - rcontent.y );
}



