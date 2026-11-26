/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/CeLabelledAnnulus.cpp
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

#include "CeLabelledAnnulus.h"

#include "astroconfig.h"
#include "ChartGeometry.h"
#include "ConfigListItems.h"
#include "HybridChart.h"
#include "mathbase.h"
#include "Painter.h"
#include "Uranian.h"

#define CE_LABELLEDANNULUS

/*****************************************************
**
**   CeLabelledAnnulus   ---   Constructor
**
******************************************************/
CeLabelledAnnulus::CeLabelledAnnulus( HybridChart *chart, CgLabelledAnnulus *annulus, const int &field_count )
	: ChartElement( chart, annulus ),
	field_count( field_count )
{
}

/*****************************************************
**
**   CeLabelledAnnulus   ---   paint
**
******************************************************/
void CeLabelledAnnulus::paint( Painter *painter, const MRect& /*refreshRect*/ )
{
	if ( ! isPaintable()) return;

	CgLabelledAnnulus *annulus = getGeom();
	assert( annulus );
	if ( ! annulus->show ) return;

	UranianSection *usection = chart->getUranianSection();
#ifdef CE_LABELLEDANNULUS
	debuglog << "CeLabelledAnnulus::paint type " << (int)annulus->getElementType()
		<< " field_count " << field_count
		<< " useSymbols " << annulus->useSymbols
		<< " showInnerFrame " << annulus->showInnerFrame
		<< " labelSize " << annulus->labelSize
		<< " usection is set " << std::boolalpha << ( usection != nullptr );
#endif

	if( usection && usection->chartDivision != 1 )
	{
#ifdef CE_LABELLEDANNULUS
		debuglog << "CeLabelledAnnulus::paint division " << usection->chartDivision;
#endif
		return;
	}

	setInheritedStyle( painter );
	painter->setPen( getPen() );

	std::vector<double> field_cusp( field_count, 0 );
	std::vector<double> field_center( field_count, 0 );

	// cusps
	for( int i = 0; i < field_count; i++ )
	{
		field_cusp[i] = getSegmentStartLongitude( i );
	}
	// centers
	for( int i = 0; i < field_count; i++ )
	{
		const double len = red_deg( field_cusp[( i + 1 != field_count ? i + 1 : 0)] - field_cusp[i] );
		field_center[i] = red_deg( field_cusp[i] + .5 * len );
	}

	// Field patterns
	const ChartFieldPattern *pattern = getFieldPattern();
	if ( pattern && pattern->isValid())
	{
		double d0 = field_cusp[0];
		double d1;
		for ( int i = 0; i < field_count; i++ )
		{
			painter->setBrush( pattern->getBrush( i ));

			d1 = field_cusp[ i == field_count - 1 ? 0 : i + 1 ];
			//drawAnnulusSegment( painter, d0, d1 );
			ChartElementBase::drawAnnulusSegment( painter, annulus, d0, d1 );
			d0 = d1;
		}
	}
	setInheritedStyle( painter );
	painter->setPen( getPen() );

	if ( annulus->useSymbols )
	{
		chart->setSymbolFont( painter, annulus->labelSize );
	}
	else
	{
		chart->setGraphicFont( painter, annulus->labelSize );
	}

	MComplex p;
	for( int i = 0; i < field_count; i++ )
	{
#ifdef CE_LABELLEDANNULUS
		debuglog << "paintAnnulusConnector i " << i <<" cusp " << field_cusp[i] << " "
			<< annulus->innerRadius << " " << annulus->radius;
#endif

		paintAnnulusConnector( painter, field_cusp[i], *annulus, annulus->innerRadius - annulus->radius );
		p = diskPointToXy( field_center[i], annulus->rLabels, annulus->geometryType );

		const std::pair<wxString, wxColour> label = getLabel( i );

		if ( label.second.IsOk())
		{
			painter->setTextColor( label.second );
		}
#ifdef CE_LABELLEDANNULUS
		debuglog << "paintAnnulusConnector drawText " << label.first << " color " << label.second.GetAsString();
#endif
		painter->drawRotatedText( label.first, p.real(), p.imag(),
			annulus->rotateLabels ? field_center[i] - 90 : 0 );
	}
	chart->setGraphicFont( painter );

	if ( annulus->showOuterFrame ) paintDisk( painter, *annulus );
	if ( annulus->showInnerFrame ) paintDisk( painter, annulus->innerDisc() );
}

/*****************************************************
**
**   CeLabelledAnnulus   ---   drawAnnulusSegment
**
******************************************************/
void CeLabelledAnnulus::drawAnnulusSegment( Painter *painter, const double &alfa0, const double &alfa1 )
{
	int i;
	const double a0 = red_deg( alfa0 );
	const double a1 = red_deg( alfa1 );

#ifdef CE_LABELLEDANNULUS
	debuglog << "CeLabelledAnnulus::drawAnnulusSegment " << a0 << " " << a1;
#endif
	ASSERT_VALID_DEGREE( a0 );
	ASSERT_VALID_DEGREE( a1 );

	CgAnnulus *annulus = getGeom();
	const ChartGeometry &parentGeom = chart->getGeometry();
	const ChartShape geom = ( annulus->geometryType == ChartShape::Inherited ? parentGeom.geometryType
		: annulus->geometryType );
	switch( geom )
	{
		case ChartShape::Circle:
		case ChartShape::Ellipse:
		{
			constexpr int nbsteps = 6;
			constexpr int ntotal =  2 * ( nbsteps + 1 );
			std::vector<MComplex> points;
			double a = a0;
			const double astep = red_deg( a1 - a0 ) / nbsteps;

			for ( i = 0; i <= nbsteps; i++ )
			{
				//points.push_back( diskPointToXy( a, annulus->innerDisc() ));
				points.push_back( diskPointToXy( a, annulus->innerRadius, annulus->geometryType ));
				a += astep;
			}
			a = a1;
			for ( i = nbsteps + 1; i < ntotal; i++ )
			{
				//points.push_back( diskPointToXy( a, *annulus ));
				points.push_back( diskPointToXy( a, annulus ));
				a -= astep;
			}
			painter->drawPolygon( points );
		}
		break;

		case ChartShape::Square:
		case ChartShape::Rectangle:
		case ChartShape::Rect2:
		{
			const int quadrant0 = (int)((a0 + 45 ) / 90 );
			const int quadrant1 = (int)((a1 + 45 ) / 90 );
			const bool qdrchange = (bool)(quadrant0 != quadrant1 );
			const double qdrcorner = 90 * (int)( std::max( a0, a1 ) / 90 ) + 45;

			std::vector<MComplex> points;

			points.push_back( diskPointToXy( a0, annulus->innerRadius, annulus->geometryType ));
			if ( qdrchange ) points.push_back( diskPointToXy( qdrcorner, annulus->innerRadius, annulus->geometryType ));
			points.push_back( diskPointToXy( a1, annulus->innerRadius, annulus->geometryType ));
			points.push_back( diskPointToXy( a1, annulus ));
			if ( qdrchange ) points.push_back( diskPointToXy( qdrcorner, annulus ));
			points.push_back( diskPointToXy( a0, annulus ));
			painter->drawPolygon( points );
		}
		break;
		case ChartShape::Triangle:
			warnlog << "CeLabelledAnnulus TODO: Invalid chart frame type " << (int)geom;
		break;
		default:
			warnlog << "CeLabelledAnnulus::drawChartFrameField Invalid chart frame type " << (int)geom;
			assert( false );
		break;
	}
}

