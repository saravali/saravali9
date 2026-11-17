/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/GdiHelper.cpp
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

#include "GdiHelper.h"

#include "AspectConfig.h"
#include "DocHelper.h"
#include "enumserializer.h"
#include "Formulary.h"
#include "gdiconfig.h"
#include "logger.h"
#include "Painter.h"
#include "SymbolProvider.h"
#include "Term.h"
#include "WesternAspect.h"

#include <format>

//#define DEB_GDIHELPER

/*****************************************************
**
**    AspectPaintHelper   ---   paintWesternAspect
**
******************************************************/
void AspectPaintHelper::paintWesternAspect( const MComplex &p1, const MComplex &p2, const WesternAspectType &type,
	const double &orbis, const double zoom )
{
	MString s1, s2;
	assert_valid_enum<WesternAspectId>( type.first );
	const WesternAspectConfig *wcfg = getWesternAspectConfig();
	const SingleWesternAspectConfig aspcfg = ((WesternAspectConfig*)wcfg)->getConfig( type.first );

	if ( ! wcfg->showAspects ) return;

	const wxColour color = wcfg->useColors ? aspcfg.pen.GetColour() : painter->getPenColor();

	painter->setPen(
		wxPen( color,
		zoom * aspcfg.pen.GetWidth(),
		aspcfg.pen.GetStyle() ));
		painter->setTextColor( color );

	if ( type.first != WesternAspectId::None && type.first != WesternAspectId::Conjunction )
	{
		if ( wcfg->showSymbols )
		{
			s2 <<  MToken( type, TextFormat::Symbol );
		}

		if ( wcfg->showOrbis && orbis )
		{
			s1 << std::format( "{:.1f}", fabs( orbis ));
		}
	}
	paintAspectLine( p1, p2, s1, s2 );
}

/*****************************************************
**
**    AspectPaintHelper   ---   paintUranianAspect
**
******************************************************/
void AspectPaintHelper::paintUranianAspect( const TermId &termId, const MComplex &proot, const MComplex &p1, const MComplex& p2,
	const int &division, const double &orbis )
{
	TermId styleId = termId;
	if ( MTerm::isAntiTermId( termId ))
	{
		styleId = Formulary::get()->getAntiTerm( termId );
	}

	const UranianAspectStyle *stylecfg = getUranianAspectStyle();
	const UranianAspectPenStyle &penstyle = stylecfg->getPenStyle( styleId );

	MString s1;
	if ( stylecfg->showDivision )
	{
		s1 << "D-" << std::format( "{}", division );
	}

	MString s2;
	if ( stylecfg->showOrbis )
	{
		s2 << std::format( "{}", (int)( orbis * 60 ));
	}

#ifdef DEB_GDIHELPER
	debuglog << "AspectPaintHelper::paintUranianAspect termId " << termId << " proot " << proot << " p1 " << p1 << " p2 " << p2
		<< " division " << division << " orbis " << orbis;
#endif

	if ( ! stylecfg->useColors )
	{
		painter->setDefaultPen();
		painter->setDefaultTextColor();
	}

	if ( MTerm::isSingleTerm( termId ))
	{
		if ( stylecfg->useColors )
		{
			painter->setPen( penstyle.mainPen );
		}
		paintAspectLine( proot, p1, s1, s2 );
	}
	else
	{
		if ( stylecfg->drawConnector )
		{
			if ( stylecfg->useColors )
			{
				painter->setPen( penstyle.connectorPen );
			}
			const MComplex pm = .5 * ( p1 + p2 );
			painter->drawLine( proot, pm );
		}
		if ( stylecfg->useColors )
		{
			painter->setPen( penstyle.mainPen );
		}
		paintAspectLine( p1, p2, s1, s2 );
	}
}

/*****************************************************
**
**    AspectPaintHelper   ---   paintAspectLine
**
******************************************************/
void AspectPaintHelper::paintAspectLine( const MComplex &p1, const MComplex &p2, const MString &s1, const MString &s2 )
{
#ifdef DEB_GDIHELPER
	debuglog << "AspectPaintHelper::paintAspectLine p1 " << p1 << " p2 " << p2 << " s1 " << s1 << " s2 " << s2;
#endif

	constexpr double first_break = .3;
	constexpr double second_break = .7;

	constexpr double linetol = .04;
	DocHelper dh( painter );

	auto pointAt = [p1, p2]( const double &l ) { return l * p2 + ( 1 - l ) * p1; };
	auto rectAt = [p1, p2]( const double &a )
	{
		constexpr int texttol = 10;
		const MComplex p = a * p2 + ( 1 - a ) * p1;
		return MRect( p.real() - texttol, p.imag() - texttol, 2 * texttol, 2 * texttol );
	};

	// TODO temp
	//painter->setTextColor( *wxRED );

	if ( ! s1.isEmpty() )
	{
		DocItemText item( s1, rectAt( first_break ), Align::Center );
		dh.formatAndDrawTextItem( item );
		debuglog << "AspectPaintHelper::paintAspectLine s1 " << s1 << " rect " << rectAt( first_break )
			<< " formatted rect " << item.rect;

		painter->drawLine( p1, pointAt( first_break - linetol ));
		painter->drawLine( pointAt( first_break + linetol ), pointAt( second_break - linetol ));
	}
	else
	{
		painter->drawLine( p1, pointAt( second_break - linetol ));
	}

	if ( ! s2.isEmpty() )
	{
		DocItemText item( s2, rectAt( second_break ), Align::Center );
		dh.formatAndDrawTextItem( item );
		debuglog << "AspectPaintHelper::paintAspectLine s2 " << s2 << " rect " << rectAt( second_break )
			<< " formatted rect " << item.rect;

		painter->drawLine( pointAt( second_break + linetol ), p2 );
	}
	else
	{
		painter->drawLine( pointAt( second_break - linetol ), p2 );
	}
}

/*****************************************************
**
**    AspectPaintHelper   ---   getUranianAspectStyle
**
******************************************************/
const UranianAspectStyle *AspectPaintHelper::getUranianAspectStyle() const
{
	extern Gdiconfig *gdiconfig;
	return uranianCfg ? uranianCfg : &gdiconfig->uranianAspectStyle;
}

/*****************************************************
**
**    AspectPaintHelper   ---   getWesternAspectConfig
**
******************************************************/
const WesternAspectConfig *AspectPaintHelper::getWesternAspectConfig() const
{
	extern Gdiconfig *gdiconfig;
	return westernCfg ? westernCfg : &gdiconfig->westernAspects;
}


