/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/GdiWidgetWrapper.cpp
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

#include "GdiWidgetWrapper.h"

#include "ConfigLoaderImpls.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "logger.h"
#include "mathbase.h"
#include "GdiWidget.h"
#include "Painter.h"

extern Gdiconfig *gdiconfig;

#define DEBUG_GDIWIDGETWRAPPER

/*****************************************************
**
**   GdiWidgetWrapper   ---   Constructor
**
******************************************************/
GdiWidgetWrapper::GdiWidgetWrapper( wxWindow *parent, GdiWidget *widget )
	: BasicWidget( parent, widget->getProps(), widget->getWidgetCategory()),
	widget( widget )
{
	init();
}

/*****************************************************
**
**   GdiWidgetWrapper   ---   Constructor
**
******************************************************/
GdiWidgetWrapper::GdiWidgetWrapper( wxWindow *parent, ContextProps_ptr props, const WidgetCategory &cat )
	: BasicWidget( parent, props, cat )
{
	init();
}

/*****************************************************
**
**   GdiWidgetWrapper   ---   Destructor
**
******************************************************/
GdiWidgetWrapper::~GdiWidgetWrapper()
{
	if ( widget ) delete widget;
}

/*****************************************************
**
**   GdiWidgetWrapper   ---   init
**
******************************************************/
void GdiWidgetWrapper::init()
{
#ifdef DEBUG_GDIWIDGETWRAPPER
	debuglog << "GdiWidgetWrapper::init";
#endif
	layoutValid = false;
	doPaintBackground = true;

	initMouseEventhandlers();
	Connect( wxEVT_SIZE, wxSizeEventHandler( GdiWidgetWrapper::OnSize ));
	Connect( wxEVT_PAINT, wxPaintEventHandler( BasicWidget::OnPaint ));
	Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( GdiWidgetWrapper::OnClose ));
	Connect( wxEVT_ERASE_BACKGROUND, wxEraseEventHandler( BasicWidget::OnEraseBackground ));
}

/*****************************************************
**
**   GdiWidgetWrapper   ---   OnClose
**
******************************************************/
void GdiWidgetWrapper::OnClose( wxCloseEvent &event )
{
#ifdef DEBUG_GDIWIDGETWRAPPER
	debuglog << "GdiWidgetWrapper::OnClose";
#endif
	event.Skip();
}

/*****************************************************
**
**   GdiWidgetWrapper   ---   clearWidget
**
******************************************************/
void GdiWidgetWrapper::clearWidget()
{
	assert( widget );
	delete widget;
	widget = nullptr;
}

/*****************************************************
**
**   GdiWidgetWrapper   ---   setWidget
**
******************************************************/
void GdiWidgetWrapper::setWidget( GdiWidget *w )
{
	if ( widget ) delete widget;
	widget = w;
}

/*****************************************************
**
**   GdiWidgetWrapper   ---   getWidgetCategory
**
******************************************************/
WidgetCategory GdiWidgetWrapper::getWidgetCategory() const
{
	if ( widget ) return widget->getWidgetCategory();
	else return category;
}

/*****************************************************
**
**   GdiWidgetWrapper   ---   OnDataChanged
**
******************************************************/
void GdiWidgetWrapper::OnDataChanged()
{
#ifdef DEBUG_GDIWIDGETWRAPPER
	debuglog << "GdiWidgetWrapper::OnDataChanged";
#endif
	if ( widget )
	{
		widget->OnDataChanged();
	}
	layoutValid = false;

	// = guiHasChanged + Refresh();
	BasicWidget::OnDataChanged();
}

/**************************************************************
***
**   GdiWidgetWrapper   ---   handleMouseLeftClick
***
***************************************************************/
WidgetResponse GdiWidgetWrapper::handleMouseLeftClick( const MComplex &p, const int& /*keymod*/ )
{
	if( widget ) { return widget->handleMouseLeftEvent( p ); }
	else { return WidgetResponse( false ); }
}

/**************************************************************
***
**   GdiWidgetWrapper   ---   handleLeftDoubleClick
***
***************************************************************/
WidgetResponse GdiWidgetWrapper::handleLeftDoubleClick( const MComplex &p, const int& /*keymod*/ )
{
	if( widget ) { return widget->handleLeftDoubleClickEvent( p ); }
	else { return WidgetResponse( false ); }
}

/**************************************************************
***
**   GdiWidgetWrapper   ---   handleResponsiveChartEvent
***
***************************************************************/
WidgetResponse GdiWidgetWrapper::handleResponsiveChartEvent( const MComplex &p, const int &keymod )
{
	if( widget ) { return widget->onResponsiveChartEvent( p, keymod ); }
	else { return WidgetResponse( false ); }
}

/*****************************************************
**
**   GdiWidgetWrapper   ---   OnSize
**
******************************************************/
void GdiWidgetWrapper::OnSize( wxSizeEvent &event )
{
	const wxSize oldsize = GetSize();
	const wxSize vsize = GetVirtualSize();
#ifdef DEBUG_GDIWIDGETWRAPPER
	debuglog << "GdiWidgetWrapper::OnSize x " << event.GetSize().x << " y " << event.GetSize().y
		<< " old size " << oldsize.x << "/" << oldsize.y
		<< " virtual size " << vsize.x << "/" << vsize.y;
#endif

	layoutValid = false;
	Refresh();
	//event.Skip();
}

/*****************************************************
**
**   GdiWidgetWrapper   ---   onPaint
**
******************************************************/
void GdiWidgetWrapper::doPaint( Painter *painter, const MRect &refreshRect )
{
#ifdef DEBUG_GDIWIDGETWRAPPER
	static int count = 0;
	debuglog << "GdiWidgetWrapper::onPaint " << ++count << " layoutValid " << std::boolalpha << layoutValid;
#endif

	assert( painter );
	assert( widget );
	StopWatch sw;
	const WidgetCategory cat = getWidgetCategory();

	if ( ! layoutValid )
	{
		const wxSize size = GetSize();
		MComplex a = widget->doLayout( painter, MRect( 0, 0, size.x, size.y ));
#ifdef DEBUG_GDIWIDGETWRAPPER
		debuglog << "GdiWidgetWrapper::doPaint size " << size << " layout from widget " << a;
#endif

		//SetVirtualSize( a.real(), a.imag());
		SetVirtualSize( std::max( (int)a.real(), size.x ), std::max( (int)a.imag(), size.y ));
		layoutValid = true;
	}

	if( doPaintBackground )
	{
		const uint skinId = props->getSkin( cat );
		const WidgetSkin &skin = WidgetSkinLoader::get()->getConfig( skinId );

		debuglog << "GdiWidgetWrapper::onPaint category " << cat << " skin " <<  props->getSkin( cat )
			<< " brush ok " << skin.brush.isOk() << " brush transparent " << ( skin.brush.style != wxBRUSHSTYLE_TRANSPARENT )
			<< " repaint background rect on " << refreshRect;

		painter->setTransparentPen();

		/* Floodfill section
		*
		* Caution: do not call getSkinnedBrush because bg color will be set in case of transparent brush.
		* Printout should have transparent brush
		*/

		if ( skin.brush.isOk() && skin.brush.style != wxBRUSHSTYLE_TRANSPARENT )
		{
			painter->setBrush( skin.brush );
		}
		else
		{
			painter->setBrush( gdiconfig->colors.bgColor );
		}
		painter->drawRectangle( refreshRect );
	}

	widget->doPaint( painter, refreshRect );
	infolog << "GdiWidgetWrapper::doPaint finished in " << sw.getElapsedMillisec() << " millisec";
}

