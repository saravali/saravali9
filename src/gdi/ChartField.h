/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/ChartField.h
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

#ifndef _CHARTFIELD_H_
#define _CHARTFIELD_H_

#include "gdibase.h"
#include "mstring.h"

class HybridChart;
class Painter;
struct VedicChartConfig;

enum class FieldPart { All = 0, Inner, Outer };

/*************************************************//**
*
*  Field in Vedic charts are cassified by their field type
*
* Fields are interpreted as parts of rectangles.
* The type is either full (rectangle), diamond or triangle.
*
* Triangle types are named according to the corner determinig the triangle.
*
******************************************************/
enum class FieldType
{
	Full = 0, // all fields in south indian chart, sbc, fixed rasis in east indian chart
	NorthEast, // east: taurus and sag
	SouthWest, // east: gemini and scorpio
	NorthWest, // east: leo and pisces
	SouthEast, // east: virgo and aquarius

	North, // north: houses 2 and 12
	West, // north: houses 3 and 5
	South, // north: houses 6 and 8
	East, // north: houses 9 and 11

	Diamond // north: houses 1, 4, 7, 10
};

/*************************************************//**
*
*  Represents a chart field in Vedic chart (Varga or SBC).
* This class contains the field geometric, alignment of items and the contents in radix vs. transit mode.
*
******************************************************/
class ChartField
{
public:

	/**
	 *  creates a field according to rectangle geometry, field type and chart center position (graviation)
	 */
	ChartField( HybridChart*, const MRect&, const Align&, const FieldType = FieldType::Full );

	ChartField( HybridChart*, const Align&, const FieldType = FieldType::Full );

	void paint( Painter*, const bool doZoom = false );

	void drawText( Painter*, const FieldPart = FieldPart::All, const MComplex baseTransitRef = MComplex( 0, 0 ));

	void drawLabel( Painter* );

	/**
	 *  gets the field contents (planets and text items) according to the mode (radix/transit)
	 */
	MString &getContents( const HoroscopeRole& );

	/**
	 *  resets all field values, clears the associated lists
	 */
	void clear();

	/**
	 *  calculates the text alignment for ths field in radix/transit mode.
	 The align depends on the geometric form and the position of the chart center (graviation)
	 */
	void calculateAlign();

	/**
	 *  center of the rect for arrow drawing. This may be distinct from the center of the rectangle,
	 * for triangular field geometric
	 */
	MComplex getCenter() const;

	MComplex getCenter( const FieldPart&, const MComplex &pFieldSplit ) const;

	bool isTriangle();

	bool containsPoint( const MComplex& ) const;

	/**
	 *  calculates the rectangle for text output in Sign/transit mode, according to field type.
	 * outerMargin is the size on the outer side if rectangle has to be modified
	 */
	MRect calculateModifiedRect( const FieldPart&, const MComplex outerMargin = MComplex ( 0, 0 )) const;

	/**
	 *  underlying rectangle. Some field types (triangle, diamond) exlcude some parts of the rectangle
	 */
	MRect rect;

	/**
	 *  geometric type of the field (rectangle, diamond, triangle)
	 */
	FieldType fieldtype;

	/**
	 *  text alignemnt for output. This value is calculated depending upon the geometric type
	 */
	Align align;

	/**
	 *  direction of the chart center
	 */
	Align gravitation;

	/**
	 *  text contents and planet indices in radix/transit mode
	 */
	MString contents, tcontents, label;

	HybridChart *chart;
};

#endif

