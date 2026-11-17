/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ChartPreviewPanel.cpp
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

#include "ChartPreviewPanel.h"

#include "ChartGeometry.h"
#include "ConfigListItems.h"
#include "ContextProperties.h"
#include "Direction.h"
#include "gdiconfig.h"
#include "GdiWidgetWrapper.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "logger.h"
#include "Uranian.h"
#include "UranianControls.h"

#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>

enum { CD_SHOW_SECONDCHART = wxID_HIGHEST + 1, SBE_CHOICE_DIVISION };

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   ChartPreviewPanel   ---   Constructor
**
******************************************************/
ChartPreviewPanel::ChartPreviewPanel( wxWindow* parent, ContextProps_ptr props,
	ChartConfig* chartConfig, const WidgetCategory &category )
	: wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxTAB_TRAVERSAL | wxBORDER_SUNKEN | wxFULL_REPAINT_ON_RESIZE ),
			props( props ),
			category( category ),
	cfg( chartConfig )
{
	directionExpert = new DirectionExpert( props );

	if ( category == WidgetCategory::UranianChart )
	{
		usection = new UranianSection;
	}

	horoscope = Horoscope_ptr( new Horoscope());
	props->setRadixHoroscope( horoscope );
	horoscope->update();

	hdirection = Horoscope_ptr( new Horoscope(
		category == WidgetCategory::UranianChart ? HoroscopeRole::SolarArc : HoroscopeRole::Transit ));

	props->setDataContext( DataContext::Radix );

	chart = new HybridChart( props, category, usection );
	setChartConfig( cfg );

	cw = new GdiWidgetWrapper( this, chart );
	cw->setChartActions();

	TransferDataToWindow();

	check_secondchart = new wxCheckBox(this, CD_SHOW_SECONDCHART, _("Show Secondary Chart"));
	check_secondchart->SetValue( gdiconfig->prefs.chartPreviewShowTransits );

	if ( category == WidgetCategory::UranianChart )
	{
		assert( usection );
		usection->chartDivision = gdiconfig->prefs.chartPreviewDivision;
		choice_division = new DivisionChoice( this, SBE_CHOICE_DIVISION );
		choice_division->setDivision( usection->chartDivision );
	}

	recreateChart();
	do_layout();
	recreateChart();

	Connect( CD_SHOW_SECONDCHART, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChartPreviewPanel::OnCheckBox ));
	Connect( SBE_CHOICE_DIVISION, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ChartPreviewPanel::OnChoiceDivision ));
	Connect( wxID_ANY, COMMAND_CHART_SKIN_CHANGED, wxCommandEventHandler( ChartPreviewPanel::OnWidgetChanged ));
	Connect( wxEVT_MOTION, wxMouseEventHandler( ChartPreviewPanel::OnMouseMotion ));
}

/*****************************************************
**
**   ChartPreviewPanel   ---   Destructor
**
******************************************************/
ChartPreviewPanel::~ChartPreviewPanel()
{
	gdiconfig->prefs.chartPreviewShowTransits = check_secondchart->GetValue();
	if ( choice_division ) gdiconfig->prefs.chartPreviewDivision = choice_division->getDivision();
	if ( usection ) delete usection;
	delete directionExpert;
}

/*****************************************************
**
**   ChartPreviewPanel   ---   setChartConfig
**
******************************************************/
void ChartPreviewPanel::setChartConfig( ChartConfig *cfg )
{
	chart->setChartConfig( category, cfg );
}

/*****************************************************
**
**   ChartPreviewPanel   ---   OnMouseMotion
**
******************************************************/
void ChartPreviewPanel::OnMouseMotion( wxMouseEvent& )
{
	debuglog << "ChartPreviewPanel::OnMouseMotion";
}

/*****************************************************
**
**   ChartPreviewPanel   ---   reset
**
******************************************************/
void ChartPreviewPanel::reset()
{
	check_secondchart->SetValue( false );
	recreateChart();
}

/*****************************************************
**
**   ChartPreviewPanel   ---   OnDataChanged
**
******************************************************/
void ChartPreviewPanel::OnDataChanged()
{
	cw->OnDataChanged();
}

/*****************************************************
**
**   ChartPreviewPanel   ---   OnCheckBox
**
******************************************************/
void ChartPreviewPanel::OnCheckBox( wxCommandEvent& )
{
	debuglog << "ChartPreviewPanel::OnCheckBox " << check_secondchart->GetValue();
	recreateChart();
}

/*****************************************************
**
**   ChartPreviewPanel   ---   recreateChart
**
******************************************************/
void ChartPreviewPanel::recreateChart()
{
	setChartConfig( cfg );

	const bool isDirection = (bool)check_secondchart->GetValue();
	debuglog << "ChartPreviewPanel::recreateChart isDirection " << isDirection;
	const DataContext datatype = isDirection ? DataContext::Direction: DataContext::Radix;

	const HoroscopeRole role2 = ( category == WidgetCategory::UranianChart ? HoroscopeRole::SolarArc : HoroscopeRole::Transit );

	props->setDataContext( datatype );
	props->setRadixHoroscope( horoscope );

	if ( isDirection )
	{
		props->setHoroscope( role2, hdirection );
		directionExpert->calculateDirections( MDate::getCurrentJD() + 36000, role2 );
	}
	else
	{
		props->setHoroscope( role2, nullptr );
	}

	//hdirection->setDate( MDate::getCurrentJD() + 36000 );
	cw->OnDataChanged();
	cw->Refresh();
}

/*****************************************************
**
**   ChartPreviewPanel   ---   OnChoiceDivision
**
******************************************************/
void ChartPreviewPanel::OnChoiceDivision( wxCommandEvent& )
{
	debuglog << "ChartPreviewPanel::OnChoiceDivision " << check_secondchart->GetValue()
		<< " division " << choice_division->getDivision();

	assert( chart );
	assert( usection );
	usection->chartDivision = choice_division->getDivision();
	chart->getUranianSection()->chartDivision = usection->chartDivision;
	cw->OnDataChanged();
	Refresh();
	emitChangeEvent();
}

/*****************************************************
**
**   ChartPreviewPanel   ---   OnMouseWheelEvent
**
******************************************************/
void ChartPreviewPanel::OnMouseWheelEvent( wxMouseEvent& )
{
	debuglog << "ChartPreviewPanel::OnMouseWheelEvent";
}

/*****************************************************
**
**   ChartPreviewPanel   ---   OnWidgetChanged
**
******************************************************/
void ChartPreviewPanel::OnWidgetChanged( wxCommandEvent &event )
{
	debuglog << "ChartPreviewPanel::OnWidgetChanged ID " << event.GetId();
	emitChangeEvent();
}

/*****************************************************
**
**   ChartPreviewPanel   ---   emitChangeEvent
**
******************************************************/
void ChartPreviewPanel::emitChangeEvent()
{
	debuglog << "ChartPreviewPanel::emitChangeEvent";
	wxCommandEvent e( GEOMETRY_CHANGED );
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   ChartPreviewPanel   ---   do_layout
**
******************************************************/
void ChartPreviewPanel::do_layout()
{
		wxBoxSizer* sizer_main = new wxBoxSizer( wxVERTICAL);

		//wxStaticBoxSizer* sizer_chart = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Chart Configuration")), wxVERTICAL);
		//sizer_main->Add( sizer_chart, 0, wxALL|wxEXPAND, 3);

		wxStaticBoxSizer* sizer_preview = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Preview")), wxVERTICAL);
		sizer_preview->Add( cw, 1, wxALL|wxEXPAND, 3);

		if ( choice_division ) sizer_preview->Add( choice_division, 0, wxALL, 3);
		sizer_preview->Add(check_secondchart, 0, wxALL, 3);
		sizer_main->Add( sizer_preview, 1, wxALL|wxEXPAND, 3);

		SetSizer(sizer_main);
		sizer_main->Fit(this);
}

