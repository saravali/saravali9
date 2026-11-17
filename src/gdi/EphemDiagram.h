/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/EphemDiagram.h
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

#ifndef _EPHEMWIDGET_H_
#define _EPHEMWIDGET_H_

#include "TimelineDiagram.h"
#include "Term.h"

class EphemExpert;

/*************************************************//**
*
*  Used for the Graphical ephemeris view.
*
******************************************************/
class EphemDiagram : public TimelineDiagram
{
public:

	EphemDiagram( ContextProps_ptr, EphemExpert* );

	virtual void doPaint( Painter*, const MRect &refreshRect ) override;

	virtual void OnDataChanged() override;

	void setCoordinateMode( const CoordinateMode &mode ) { coordMode = mode; }

	void setDirty( const bool b = true ) { dataDirty = b; }

protected:

	void setupPaths();

	void updateGrid();

	wxColour getTermColor( const MTerm& );

	void paintObjects( Painter*, const MRect& );

	void paintPath( Painter *painter, const TermPath &path, const MRect& /*refreshRect*/ );

	// extract the right value from PlanetPosition
	double getValue( const PlanetPosition& );

	// combination: find the right y value for an PlanetPosition
	inline double getYValue( const PlanetPosition &pos ) { return valueToY( getValue( pos )); }

	bool dataDirty = true;;

	std::list<TermPath> paths;

	CoordinateMode coordMode = CoordinateMode::Longitude;;

	EphemExpert *expert = nullptr;
};


#endif // _EPHEMWIDGET_H_

