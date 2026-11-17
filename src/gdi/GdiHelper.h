/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/GdiHelper.h
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

#ifndef GDIHELPER_H_
#define GDIHELPER_H_

#include "constants.h"
#include "mstring.h"

class Painter;
struct WesternAspectConfig;
struct UranianAspectStyle;

/*************************************************//**
*
* 
*
******************************************************/
class AspectPaintHelper
{
public:
	AspectPaintHelper( Painter *painter, const UranianAspectStyle *style = nullptr )
		: painter( painter ), uranianCfg( style ) {}

	AspectPaintHelper( Painter *painter, const WesternAspectConfig *style )
		: painter( painter ), westernCfg( style ) {}

	void paintWesternAspect( const MComplex &p1, const MComplex &p2, const WesternAspectType&,
		const double &orbis, const double zoom = 1.0 );

	void paintUranianAspect( const TermId&, const MComplex &proot, const MComplex &p1, const MComplex& p2,
		const int &division, const double &orbis );

	void setUranianAspectStyle( UranianAspectStyle *cfg ) { uranianCfg = cfg; }

	void setWesternAspectConfig( WesternAspectConfig *cfg ) { westernCfg = cfg; }

protected:

	const UranianAspectStyle *getUranianAspectStyle() const;

	const WesternAspectConfig *getWesternAspectConfig() const;

	void paintAspectLine( const MComplex &p1, const MComplex &p2, const MString&, const MString& );

	Painter *painter = nullptr;

	const UranianAspectStyle *uranianCfg = nullptr;

	const WesternAspectConfig *westernCfg = nullptr;

};



#endif // GDIHELPER_H_

