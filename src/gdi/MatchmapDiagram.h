/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/MatchmapDiagram.h
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

#ifndef _MATCHMAPDIAGRAM_H_
#define _MATCHMAPDIAGRAM_H_

#include "TimelineDiagram.h"
#include "MDocument.h"

#include <list>

class EventFinder;
struct Match;

/*************************************************//**
*
* 
*
******************************************************/
struct MatchmapDiagramElement
{
	MatchmapDiagramElement( const MComplex &a ) : anchor( a ) {}

	MComplex anchor;

	DocItemText contents;
};

/*************************************************//**
*
*  Base class for Uranian cluster diagram widgets.
*
******************************************************/
class MatchmapDiagram :  public TimelineDiagram
{
public:

  MatchmapDiagram( ContextProps_ptr, EventFinder* );

  virtual MComplex doLayout( Painter* ) override;

	virtual void OnDataChanged() override;

	virtual void doPaint( Painter *painter, const MRect &refreshRect ) override;

protected:

	double x2jd( const double &x ) const ;

	double calculateY( const uint &formulaId, const int &divisor ) const;

	double match2y( const Match& ) const;

	//void drawCross( Painter*, const MComplex &p, const double &w ) const;

	MComplex bestLayout = 0;

	std::vector<int> divisors;

	std::list<MatchmapDiagramElement> elements;

	EventFinder *finder;

};

#endif // _MATCHMAPDIAGRAM_H_

