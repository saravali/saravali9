/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/CombiWidget.cpp
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

#include "CombiWidget.h"

#include "ContextProperties.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "GdiWidgetWrapper.h"
#include "HybridChart.h"
#include "TextHelper.h"
#include "TextWidget.h"
#include "WidgetFactory.h"

#include <wx/sizer.h>
#include <wx/splitter.h>

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   CombiWidget   ---   Constructor
**
******************************************************/
CombiWidget::CombiWidget( wxWindow *parent, ContextProps_ptr props, const WidgetCategory &category )
	: BasicWidget( parent, props, category )
{
	const Viewpoint viewpoint = getViewpoint();
	oldvp = viewpoint;
	debuglog << "CombiWidget category " << category << " viewpoint " << viewpoint;

	splitter = new wxSplitterWindow( this );
	twidget = new TextWidget( splitter, props,
		category == WidgetCategory::SynastryPair ?
		WidgetCategory::TextSynastry
		: WidgetCategory::TextPlanetListUniversal );

	wrapper = new GdiWidgetWrapper( splitter, props, category );
	wrapper->setChartActions();
	recreateChart();

	splitter->SetMinimumPaneSize( 150 );
	splitter->SetSashGravity( 0.5 );
	splitter->SplitVertically( twidget, wrapper );

	mainsizer->Add( splitter, 1, wxEXPAND, 3 );
	SetSizer( mainsizer );
}

/*****************************************************
**
**   CombiWidget   ---   Destructor
**
******************************************************/
CombiWidget::~CombiWidget()
{
	gdiconfig->prefs.splitterSashPos[ configkey ] = splitter->GetSashPosition();
	if ( decorator ) delete decorator;
}

/*****************************************************
**
**   CombiWidget   ---   postCreate
**
******************************************************/
void CombiWidget::postCreate()
{
	debuglog << "CombiWidget::postCreate";
	if ( const auto p = gdiconfig->prefs.splitterSashPos.find( configkey );
		p != gdiconfig->prefs.splitterSashPos.end())
	{
		if ( p->second )
		{
			debuglog << "CombiWidget::postCreate set sash to " << p->second;
			splitter->SetSashPosition( p->second );
		}
	}
	else splitter->SetSashPosition( 250 );
}

/*****************************************************
**
**   CombiWidget   ---   setDecorator
**
******************************************************/
void CombiWidget::setDecorator( CombiWidgetDecorator *d )
{
	decorator = d;
}

/*****************************************************
**
**   CombiWidget   ---   OnDataChanged
**
******************************************************/
void CombiWidget::OnDataChanged()
{
	debuglog << "CombiWidget::OnDataChanged viewpoint " << getViewpoint();
	if ( getViewpoint() != oldvp )
	{
		recreateChart();
		oldvp = getViewpoint();
	}

	wrapper->OnDataChanged();
	wrapper->Refresh();

	MDocument_ptr doc = twidget->getDoc();
	assert( doc );
	doc->clear();
	int ret = 0;

	// delegate Document writing to decorator is possible
	if ( decorator )
	{
		ret = decorator->writePlanetListDoc( doc );
	}
	else // default behaviour if decorator not set
	{
		Horoscope_ptr h = props->getRadixHoroscope();
		if ( h )
		{
			ret = TextHelper( props, doc ).writePlanets( h );
		}
		else
		{
			doc->addLine( "Empty" );
		}
	}
	static bool shown = false;
	if ( ret && ! shown )
	{
		shown = true;
		showMissingEphemDataMessage( this );
	}
	twidget->OnDataChanged();
	Refresh();
}

/*****************************************************
**
**   CombiWidget   ---   recreateChart
**
******************************************************/
void CombiWidget::recreateChart()
{
	WidgetCategory ccat = WidgetCategory::None;
	switch( getViewpoint())
	{
		case Viewpoint::Vedic:
			ccat = WidgetCategory::VargaChart;
		break;

		case Viewpoint::Western:
		case Viewpoint::Astronomical:
		case Viewpoint::Uranian:
		default:
			ccat = WidgetCategory::WesternChart;
		break;
	}
	HybridChart *chart = new HybridChart( props, ccat, category );
	wrapper->setWidget( chart );
}

/*************************************************//**
*
* 
*
******************************************************/
BasicWidget *WidgetFactory::createCombiWidget( wxWindow *parent, const WidgetCategory &category )
{
	return new CombiWidget( parent, props, category );
}


