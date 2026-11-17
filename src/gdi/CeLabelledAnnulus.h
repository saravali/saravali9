/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/CeLabelledAnnulus.h
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

#ifndef CELABELLED_ANNULUS_H_
#define CELABELLED_ANNULUS_H_

#include "ChartElementImpl.h"

/*************************************************//**
*
* 
*
******************************************************/
class CeLabelledAnnulus : public ChartElement<CgLabelledAnnulus>
{
public:

	CeLabelledAnnulus( HybridChart *chart, CgLabelledAnnulus *annulus, const int &field_count );

	virtual void paint( Painter *painter, const MRect& /*refreshRect*/ ) override;

protected:

	/**
	* determine whether the element should be painted (depends on chart context)
	**/
	virtual bool isPaintable() const { return true; }

	void drawAnnulusSegment( Painter *painter, const double &alfa0, const double &alfa1 );

	virtual double getSegmentStartLongitude( const int &f ) = 0;

	virtual std::pair<wxString, wxColour> getLabel( const int &f ) = 0;

	virtual const ChartFieldPattern *getFieldPattern() const = 0;

	const int field_count;

};

#endif // CELABELLED_ANNULUS_H_

