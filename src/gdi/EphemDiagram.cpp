/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/EphemDiagram.cpp
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

#include "EphemDiagram.h"

#include "ContextProperties.h"
#include "enumserializer.h"
#include "Ephemeris.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "mathbase.h"
#include "Painter.h"
#include "DocHelper.h"
#include "TermSolver.h"

extern Gdiconfig *gdiconfig;

#define DEBUG_EPHEMWIDGET

/**************************************************************
***
**   EphemDiagram   ---   Constructor
***
***************************************************************/
EphemDiagram::EphemDiagram( ContextProps_ptr props, EphemExpert *expert )
	: TimelineDiagram( props, WidgetCategory::EphemGraphical ),
	 expert( expert )
{
	division = 1;
	gkDegrees = 360.0;
}

/**************************************************************
***
**   EphemDiagram   ---   OnDataChanged
***
***************************************************************/
void EphemDiagram::OnDataChanged()
{
#ifdef DEBUG_EPHEMWIDGET
	debuglog << "EphemDiagram::OnDataChanged rect " << rect << " viewpoint " << getViewpoint();
#endif
	updateGrid();

	vmin = vmax = v0 = 0;

	double maxElongation = 0;

	if ( coordMode != CoordinateMode::Longitude )
	{
		for( const TermPath &path : paths )
		{
			for( const TermStep &step : path.steps )
			{
				maxElongation = std::max( maxElongation, fabs( step.pos.declination ));
			}
		}
		// increase elongation because min an max values are likely not included
		maxElongation = (int)maxElongation + 2;
	}
	switch( coordMode )
	{
		case CoordinateMode::Longitude:
			vmax = gkDegrees;
		break;
		case CoordinateMode::Declination:
			vmax = maxElongation;
			v0 = maxElongation;
			vmin = - maxElongation;
		break;
		case CoordinateMode::AbsDeclination:
			vmax = maxElongation;
		break;
		default:
			assert( false );
		break;
	}
#ifdef DEBUG_EPHEMWIDGET
	debuglog << "EphemDiagram::OnDataChanged: mode " << (int)coordMode << " vmin " << vmin
		<< " v0 " << v0 << " vmax " << vmax << " maxElongation " << maxElongation;
#endif
	dataDirty = false;
}

/**************************************************************
***
**   EphemDiagram   ---   doPaint
***
***************************************************************/
void EphemDiagram::doPaint( Painter *painter, const MRect& /*refreshRect*/ )
{
#ifdef DEBUG_EPHEMWIDGET
	debuglog << "EphemDiagram::doPaint dataDirty " << std::boolalpha << dataDirty;
#endif
	assert( painter );

	if ( dataDirty )
	{
		OnDataChanged();
	}

	painter->setGraphicFont();
	drawGrid( painter );
	paintObjects( painter, rcontent );
}

/**************************************************************
***
**   EphemDiagram   ---   getValue
***
***************************************************************/
double EphemDiagram::getValue( const PlanetPosition &pos )
{
	switch( coordMode )
	{
		case CoordinateMode::Longitude:
#ifdef DEBUG_EPHEMWIDGET
			debuglog << " EphemDiagram::getValue " << pos.longitude << " " << a_red( pos.longitude, 360.0 / division );
#endif
			return a_red( pos.longitude, 360.0 / division );
		break;
		case CoordinateMode::Declination:
#ifdef DEBUG_EPHEMWIDGET
			debuglog << " EphemDiagram::getValue " << pos.declination;
#endif
			return pos.declination;
		break;
		case CoordinateMode::AbsDeclination:
#ifdef DEBUG_EPHEMWIDGET
			debuglog << " EphemDiagram::getValue " << pos.declination;
#endif
			return fabs( pos.declination );
		break;
		default:
			assert( false );
		break;
	}
	return 0.0;
}

/**************************************************************
***
**   EphemDiagram   ---   paintPath
***
***************************************************************/
void EphemDiagram::paintPath( Painter *painter, const TermPath &path, const MRect& /*refreshRect*/ )
{
	MComplex p, q;
	auto iter = path.steps.begin();
  if ( iter == path.steps.end())
	{
		warnlog << "Path for term " << path.term << " is empty";
		return;
	}

	p = MComplex( jd2x( iter->jd ), getYValue( iter->pos ));
#ifdef DEBUG_EPHEMWIDGET
	debuglog << "EphemDiagram::paintPath " << path.term
		<< " FIRST POINT " << p
		<< " count " << path.steps.size();
	debuglog << "EphemDiagram::paintPath MIN POINT " << rcontent.getBottom() << " max " << rcontent.y;
#endif

	auto previous = iter;

  while( ++iter != path.steps.end())
  {
		q = MComplex( jd2x( iter->jd ), getYValue( iter->pos ));
#ifdef DEBUG_EPHEMWIDGET
		debuglog << "ITER POINT " << q;
#endif

		if ( iter->type == TermStepType::Wrap )
		{
			// das geht nur wenn speed > 0
			q = MComplex( jd2x( iter->jd ), rcontent.y );
#ifdef DEBUG_EPHEMWIDGET
			debuglog << "JUMPER  POINT1 " << q << " longitude " << iter->pos.longitude << " type " << (int)previous->type;
#endif
			painter->drawLine( p, q );
			q = MComplex( q.real(), rcontent.getBottom() );
#ifdef DEBUG_EPHEMWIDGET
			debuglog << "JUMPER  POINT1 " << q << " longitude " << iter->pos.longitude << " type " << (int)previous->type;
#endif
		}
		else
		{
			painter->drawLine( p, q );
#ifdef DEBUG_EPHEMWIDGET
			debuglog << "ITER POINT " << q << " longitude " << iter->pos.longitude << " type " << (int)previous->type;
#endif
		}

		p = q;
		previous = iter;
	}
	painter->setDefaults();
	painter->setTransparentBrush();
}

/**************************************************************
***
**   EphemDiagram   ---   getTermColor
***
***************************************************************/
wxColour EphemDiagram::getTermColor( const MTerm &term )
{
	if ( term.getTermId() == TermId::Single )
	{
		return gdiconfig->chartColors.getPlanetColor( term.getFirstPlanet() );
	}
	return gdiconfig->colors.getDefaultFgColour();
}

/**************************************************************
***
**   EphemDiagram   ---   paintObjects
***
***************************************************************/
void EphemDiagram::paintObjects( Painter *painter, const MRect& refreshRect )
{
	constexpr int lw = 2;
	painter->setTransparentBrush();
	for( const TermPath &path : paths )
	{
#ifdef DEBUG_EPHEMWIDGET
		debuglog << "NEW object " << path.term << " size " << path.steps.size();
#endif
		if ( ! path.steps.size() ) continue;

		painter->setPen( wxPen( getTermColor( path.term ), lw, wxPENSTYLE_SOLID ));
		paintPath( painter, path, refreshRect );

		// draw labels and tics after painting the path because tics may override the graph
		drawLeftLabel( painter, path.term.toMString( getViewpoint() ), getYValue( path.steps.front().pos ));
		drawRightLabel( painter, path.term.toMString( getViewpoint()), getYValue( path.steps.back().pos ));
	}
}

/**************************************************************
***
**   EphemDiagram   ---   updateGrid
***
***************************************************************/
void EphemDiagram::updateGrid()
{
	startJD = expert->getStartJD();
	endJD = expert->getEndJD();
	Horoscope_ptr h( new Horoscope );;
	TermSolver solver( h, getViewpoint() );

	paths.clear();
	expert->initTermPaths( paths );

	for( TermPath &path : paths )
	{
		switch( coordMode )
		{
			case CoordinateMode::Longitude:
				solver.fitTermPathToBoundaries( path, division );
			break;
			case CoordinateMode::AbsDeclination:
				solver.insertDeclinationZeros( path );
			break;
			default:
			break;
		}
#ifdef DEBUG_EPHEMWIDGET
		debuglog << "EphemDiagram::updateGrid item count " << path.steps.size();
#endif
	}
#ifdef DEBUG_EPHEMWIDGET
	debuglog << "EphemDiagram::updateGrid finished, path count " << paths.size();
#endif
}

