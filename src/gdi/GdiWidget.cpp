/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/GdiWidget.cpp
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

#include "GdiWidget.h"

#include "astroconfig.h"
#include "ConfigLoaderImpls.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "logger.h"
#include "mathbase.h"
#include "Painter.h"
#include "DocHelper.h"

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

//#define DEBUG_GDIWIDGET

/*****************************************************
**
**   GdiWidget   ---   doLayout
**
******************************************************/
MComplex GdiWidget::doLayout( Painter* )
{
	return getRect().getExtent();
}

MComplex GdiWidget::doLayout( Painter *painter, const MRect &rect )
{
	setRect( rect );
	return doLayout( painter );
}

/*****************************************************
**
**   GdiWidget   ---   getSkin
**
******************************************************/
const WidgetSkin &GdiWidget::getSkin() const
{
	//const WidgetCategory cat =  getSuperiorWidgetCategory( category );

	const unsigned skin = props->getSkin( category );
	WidgetSkinLoader *loader = WidgetSkinLoader::get();

#ifdef DEBUG_GDIWIDGET
	debuglog << "GdiWidget::getSkin category " << category << " skin id " << skin;
#endif

	return loader->getConfig( skin );
}

/*****************************************************
**
**   GdiWidget   ---   getSkinnedPen
**
******************************************************/
wxPen GdiWidget::getSkinnedPen() const
{
	wxPen pen =  gdiconfig->colors.getDefaultPen();
	const WidgetSkin &skin = getSkin();

#ifdef DEBUG_GDIWIDGET 
	debuglog << "GdiWidget::getSkinnedPen skin " << props->getSkin( category )
		<< " ok " << skin.pen.IsOk();
	if ( skin.pen.IsOk() )
	{
		debuglog << "GdiWidget::getSkinnedPen skin (2) " << props->getSkin( category )
			<< " color " << skin.pen.GetColour().GetAsString()
			<< " width " << skin.pen.GetWidth()
			<< " style " << skin.pen.GetStyle();
	}
#endif

	softOverridePen( pen, skin.pen );
	return pen;
}

/*****************************************************
**
**   GdiWidget   ---   getSkinnedBrush
**
******************************************************/
MBrush GdiWidget::getSkinnedBrush() const
{
	const WidgetSkin &skin = getSkin();
#ifdef DEBUG_GDIWIDGET
	debuglog << "GdiWidget::getSkinnedBrush category " << category << " brush " << skin.brush << " brush ok " << std::boolalpha << skin.brush.isOk();
#endif
	if ( skin.brush.isOk()) return skin.brush;
	else return MBrush( gdiconfig->colors.bgColor );
}

/*****************************************************
**
**   GdiWidget   ---   getSkinnedMarkedFieldBrush
**
******************************************************/
MBrush GdiWidget::getSkinnedMarkedFieldBrush() const
{
	const WidgetSkin &skin = getSkin();
	if ( skin.markedFieldBrush.isOk()) return skin.markedFieldBrush;
	else return MBrush( gdiconfig->colors.markedFieldsBgColor );
}

/*****************************************************
**
**   GdiWidget   ---   getSkinnedTextColor
**
******************************************************/
wxColour GdiWidget::getSkinnedTextColor() const
{
	const WidgetSkin &skin = getSkin();
	if ( skin.textColor.IsOk()) { return skin.textColor; }
	else return gdiconfig->colors.textFgColor;
}

/*****************************************************
**
**   GdiWidget   ---   getRelativeFontSize
**
******************************************************/
double GdiWidget::getRelativeFontSize() const
{
	const int fontsize = gdiconfig->fonts.graphicFontSize;
	//return fontsize * 100.0 / 12.0;
	return fontsize * 20;
}

/*****************************************************
**
**   GdiWidget   ---   setGraphicFont
**
******************************************************/
void GdiWidget::setGraphicFont( Painter *painter, const int labelSizePercent )
{
	//painter->setGraphicFont( calculateFontSize( labelSizePercent ));
	painter->setGraphicFont( labelSizePercent * gdiconfig->fonts.graphicFontSize * props->getZoom() / 100.0 );
}

/*****************************************************
**
**   GdiWidget   ---   setSymbolFont
**
******************************************************/
void GdiWidget::setSymbolFont( Painter *painter, const int labelSizePercent )
{
	//painter->setSymbolFont( calculateFontSize( labelSizePercent ));
	painter->setSymbolFont( labelSizePercent * gdiconfig->fonts.symbolFontSize * props->getZoom() / 100.0 );
}

/*****************************************************
**
**   GdiWidget   ---   calculateFontSize
**
******************************************************/
double GdiWidget::calculateFontSize( const int &labelSizePercent )
{
#if 1
	return labelSizePercent * gdiconfig->fonts.graphicFontSize * props->getZoom() / 100.0;

#else
	const int widgetFactor = std::min( rect.width, rect.height );
	constexpr double scalingFactor = 400000;

	return labelSizePercent * getRelativeFontSize() * widgetFactor * props->getZoom() / scalingFactor;
#endif
}

/*****************************************************
**
**   GdiWidget   ---   getViewpoint
**
******************************************************/
Viewpoint GdiWidget::getViewpoint() const
{
	ViewpointHelper *vh = ViewpointHelper::get();
  const Viewpoint stickyVp = vh->getStickyVp( category );
  const Viewpoint viewpoint = props->getEffectiveViewpoint( stickyVp );
#ifdef DEBUG_GDIWIDGET 
  debuglog << "GdiWidget::getViewpoint category " << category << " stickyVp " << stickyVp
    << " result vp " << viewpoint;
#endif
  return viewpoint;
}

/*****************************************************
**
**   ErrorDiagram   ---   Constructor
**
******************************************************/
ErrorDiagram::ErrorDiagram( ContextProps_ptr props, const wxString message, const LogLevel loglevel )
	: GdiWidget( props, WidgetCategory::EmptyWidget ),
	loglevel( loglevel )
{
	text.align = Align::Center;
	text.tf = MString( message );
}

/*****************************************************
**
**   ErrorDiagram   ---   doPaint
**
******************************************************/
void ErrorDiagram::doPaint( Painter *painter, const MRect& /*refreshRect*/ )
{
#ifdef DEBUG_GDIWIDGET 
	debuglog << "ErrorDiagram::doPaint rect " << rect;
#endif
	painter->setDefaultPen();
	painter->setTransparentBrush();
	painter->setTextColor( *wxBLUE );

	painter->setPen( wxColour( "orange" ));
	painter->drawLine( rect.x, rect.y, rect.width, rect.height );
	painter->drawLine( rect.x, rect.height, rect.width, rect.y );

	text.rect = rect;
	DocHelper( painter ).formatTextItem( text, rect.width );
	DocHelper( painter ).drawTextItem( text );
}


