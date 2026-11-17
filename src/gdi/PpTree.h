/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/PpTree.h
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

#ifndef _PPTREE_H_
#define _PPTREE_H_

#include "GdiWidget.h"
#include "MDocument.h"
#include "PlanetaryPicture.h"

#include <list>

/*************************************************//**
*
* 
*
******************************************************/
struct PpTreeElement
{
	PpTreeElement( const TermId &id ) : termId( id ) {}

	const TermId termId = TermId::None;

	DocItemText lTerm, rTerm, lLabel, rLabel;

};

/*************************************************//**
*
*  Base class for Uranian tree cluster diagram widgets.
*
*  PpTree widgets are arranged in grids.
*
******************************************************/
class PpTree :  public GdiWidget
{

public:
  PpTree( ContextProps_ptr, const PlanetaryPicture, UranianSection* );

  virtual MComplex doLayout( Painter* ) override;

	virtual void OnDataChanged() override;

	virtual void doPaint( Painter *painter, const MRect &refreshRect ) override;

protected:

	void clear();

	void setGraphicFont( Painter* );

	virtual void onCoordinatesChanged() override;

	const PlanetaryPicture picture;

	DocItemText ltitle;

	DocItemText mtitle;

	DocItemText rtitle;

	std::list<PpTreeElement> telements;

	MRect rcontent;

	MComplex bestLayout = 0;

	// step widths in x and y direction, derived from font size
	double xs;
	double ys;

	// width of the columns on both sides
	double labelColWidth;

	// x center of the widget
	double xm;

	// total width (max of the items) of the term strings on left, right and max (wTerm)
	double wlTerm;
	double wrTerm;
	double wTerm;

	// total width of the label strings on left, right and max (wLabel)
	double wlLabel;
	double wrLabel;
	double wLabel;

	// global index of the widget instance
	const int index;

	inline static int global_index = 0;

};

#endif // _PPTREE_H_

