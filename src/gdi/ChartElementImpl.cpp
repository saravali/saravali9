/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/ChartElementImpl.cpp
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

#include "ChartElementImpl.h"
#include "DocHelper.h"
#include "HybridChart.h"

/*****************************************************
**
**   CeDisc   ---   paint
**
******************************************************/
void CeDisc::paint( Painter *painter, const MRect& /*refreshRect*/ )
{
	//debuglog << "CeDisc::paint pen ok " << cg->pen.isOk();
	CgDisc *disc = getCg();
	if ( ! disc->show ) return;

	setInheritedStyle( painter );
	paintDisc( painter, *disc );
}

/*****************************************************
**
**   CeError   ---   paint
**
******************************************************/
void CeError::paint( Painter *painter, const MRect& /*refreshRect*/ )
{
	//debuglog << "CeError::paint pen ok " << cg->pen.isOk();
	CgDisc *disc = getCg();
	DocHelper( painter ).formatAndDrawTextItem( MString( message ), chart->getRect(), Align::Center );

	if ( ! disc->show ) return;

	setInheritedStyle( painter );
	paintDisc( painter, *disc );
}

/*****************************************************
**
**   CeAnnulus   ---   paint
**
******************************************************/
void CeAnnulus::paint( Painter *painter, const MRect& /*refreshRect*/ )
{
	//debuglog << "ChartElementTorus::paint pen ok " << cg->pen.isOk();
	CgAnnulus *annulus = getCg();
	assert( annulus );
	if ( ! annulus->show ) return;

	assert( painter );
	setInheritedStyle( painter );

	if ( annulus->showOuterFrame )
	{
		paintDisc( painter, annulus );
	}
	if ( annulus->showInnerFrame )
	{
		paintDisc( painter, annulus->innerDisc() );
	}
}

/*****************************************************
**
**   Grid element factory methods
**
******************************************************/
ChartElementBase *ChartElementFactory::createDisc( CgElement *cge ) const
{
	CgDisc *disc = dynamic_cast<CgDisc*>( cge );
	assert( disc );
	return new CeDisc( chart, disc );
}

ChartElementBase *ChartElementFactory::createAnnulus( CgElement *cge ) const
{
	CgAnnulus *a = dynamic_cast<CgAnnulus*>( cge );
	assert( a );
	return new CeAnnulus( chart, a );
}

ChartElementBase *ChartElementFactory::createError( CgElement *cge ) const
{
	CgError *grid = dynamic_cast<CgError*>( cge );
	assert( grid );
	return new CeError( chart, grid, grid->message );
}

