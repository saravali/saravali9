/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/TimelineDiagram.h
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

#ifndef _TIMELINEDIAGRAM_H_
#define _TIMELINEDIAGRAM_H_

#include "GdiWidget.h"

/*************************************************//**
*
* 
*
******************************************************/
class TimelineDiagram : public GdiWidget
{
public:
	TimelineDiagram( ContextProps_ptr, const WidgetCategory& );

	virtual MComplex doLayout( Painter* ) override;

	void setDivision( const int& );

protected:

	void clear();

	double jd2x( const double& ) const;

	double valueToY( const double &l );

	void drawGrid( Painter* );

	void drawRuler( Painter* );

	std::list<double> calculateRulerJDList( const double &startJD, const double &endJD, const int &mode ) const;

	void dailyRuler( Painter* );

	void monthlyRuler( Painter* );

	void yearlyRuler( Painter* );

	void horizontalRuler( Painter* );

	void paintRulerMarker( Painter*, const double &jd, const wxString label, const bool &isLong );

	void drawLeftLabel( Painter*, const MString&, const double &y );

	void drawRightLabel( Painter*, const MString&, const double &y );

	int division = 1;

	double gkDegrees = 360.0;

	double startJD = 0.0;

	double endJD = 0.0;

	// min, max and zero value
	double vmin = 360;;
	double vmax = 0.0;
	double v0 = 0.0;

	MComplex textExtent;

	double shortTic = 3.0;

	double longTic = 5.0;

	MRect rcontent;

	wxPen gridPen;

	bool doPaintGrid = true;
};


#endif // _TIMELINEDIAGRAM_H_

