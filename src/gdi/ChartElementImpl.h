/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/ChartElementImpl.h
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

#ifndef CHART_ELEMTENT_IMPL_H_
#define CHART_ELEMTENT_IMPL_H_

#include "ChartElement.h"
#include "gdibase.h"
#include "geometry.h"
#include "constants.h"

struct ChartFieldPattern;
class HybridChart;
class Painter;

/*************************************************//**
*
*  
*
******************************************************/
template<typename T> class ChartElement : public ChartElementBase
{
public:

	ChartElement( HybridChart *chart, T *cg )
		: ChartElementBase( chart )
	{
		assert( cg );
		this->cg = (T*)cg->clone();
	}

	virtual ~ChartElement()
	{
		delete cg;
	}

	virtual double getRadius() const override { assert( cg ); return cg->radius; }

	virtual double getOuterRadius( const int &id ) override
	{
		assert( cg ); 
		return cg->getOuterRadius( id );
	}

	virtual CgElementType getElementType() const override { return cg->getElementType(); }

	T *getCg() { return cg; }

protected:

	virtual wxPen &getCgPen() const override
	{
		return cg->pen;
	}

	virtual MBrush &getCgBrush() const override
	{
		return cg->brush;
	}

	virtual wxColour &getCgTextColor() const override
	{
		return cg->textColor;
	}

	T *cg;
};

/*************************************************//**
*
*   Simple chart element containing only a disc.
*
******************************************************/
class CeDisc : public ChartElement<CgDisc>
{
public:

	CeDisc( HybridChart *chart, CgDisc *disc )
		: ChartElement( chart, disc ) {}

	virtual void paint( Painter*, const MRect& refreshRect ) override;
};


/*************************************************//**
*
*   Chart element showing an annular region.
*
******************************************************/
class CeAnnulus : public ChartElement<CgAnnulus>
{
public:

	CeAnnulus( HybridChart *chart, CgAnnulus *a )
		: ChartElement( chart, a ) {}

	virtual void paint( Painter*, const MRect& refreshRect ) override;

};

/*************************************************//**
*
*  
*
******************************************************/
class CeError : public ChartElement<CgDisc>
{
public:

	CeError( HybridChart *chart, CgDisc *disc, wxString message )
		: ChartElement( chart, disc ),
		message( message ){}

	virtual void paint( Painter*, const MRect& refreshRect ) override;

protected:

	wxString message;
};


#endif

