/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/UranianChartWidget.cpp
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

#include "astroconfig.h"
#include "BasicWidget.h"
#include "ContextProperties.h"
#include "Commands.h"
#include "enumserializer.h"
#include "GdiWidgetWrapper.h"
#include "HybridChart.h"
#include "Sidebar.h"
#include "Uranian.h"
#include "UranianHelper.h"
#include "WidgetFactory.h"

#include <wx/sizer.h>
#include <wx/slider.h>

extern Astroconfig *astroconfig;

#define DEBUG_URANIANCHARTWIDGET

enum { UCW_SLIDER = wxID_HIGHEST + 1300 };

/*************************************************//**
*
* 
*
******************************************************/
class UranianChartWidget : public BasicWidget
{
public:

	UranianChartWidget( wxWindow *parent, ContextProps_ptr props )
		: BasicWidget( parent, props, WidgetCategory::UranianChartWidget ),
		usetting( UranianHelper::get()->calculateUranianContext( props, category ))
	{
		usection = new UranianSection( astroconfig->getUranianSection( usetting ));

		// Uranian chart radix views need a second chart
		if ( props->getDataContext() == DataContext::Radix )
		{
			assert( props->getActiveRoles().size() == 1 );
			Horoscope_ptr h2 = Horoscope_ptr( new Horoscope( HoroscopeRole::HandShifted ));
			props->setHoroscope( HoroscopeRole::HandShifted, h2 );
		}

		leftpanel = SidebarPanelFactory().createSidebarPanel( this, props, usetting, usection );

		chart = new HybridChart( props, WidgetCategory::UranianChart, usection );
		wrapper = new GdiWidgetWrapper( this, chart );
		wrapper->setChartActions();

		slider = new wxSlider( this, UCW_SLIDER, 0, -180, 180, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
		slider->Show( usection->offsetToggle );
		//slider->Show( false );

    wxBoxSizer *hsizer = new wxBoxSizer( wxHORIZONTAL );
    hsizer->Add( leftpanel, 0, wxEXPAND );
    hsizer->Add( wrapper, 1, wxEXPAND );

		mainsizer->Add( hsizer, 1, wxEXPAND, 3 );
    mainsizer->Add( slider, 0, wxEXPAND );
		SetSizer( mainsizer );
		mainsizer->Fit( this );
		//Layout();

		SetAutoLayout( true );

		Connect( wxID_ANY, COMMAND_SIDEPANEL_CHANGED, wxCommandEventHandler( UranianChartWidget::OnSidePanel ));
		Connect( wxID_ANY, COMMAND_SIDEPANEL_REFRESH, wxCommandEventHandler( UranianChartWidget::OnSidePanel ));
		Connect( wxID_ANY, COMMAND_SIDEPANEL_SORT, wxCommandEventHandler( UranianChartWidget::OnSidePanel ));
		Connect( wxID_ANY, COMMAND_SIDEPANEL_OFFSET, wxCommandEventHandler( UranianChartWidget::OnToggleOffset ));

		//Connect( wxID_ANY, wxEVT_COMMAND_SCROLL, wxCommandEventHandler( UranianChartWidget::OnSlider ));
		//Connect( wxID_ANY, wxEVT_SCROLL_CHANGED, wxCommandEventHandler( UranianChartWidget::OnSlider ));
		Connect( UCW_SLIDER, wxEVT_SCROLL_THUMBTRACK, wxCommandEventHandler( UranianChartWidget::OnSlider ));

		OnDataChanged();
	}

	/*****************************************************
	**
	**   UranianChartWidget   ---   Destructor
	**
	******************************************************/
	~UranianChartWidget()
	{
		usection->offset = 0;
		usection->offsetToggle = false;
		props->setHoroscope( HoroscopeRole::HandShifted, nullptr );

		astroconfig->getUranianSection( usetting ) = *usection;
		delete usection;
	}

	UranianSection *getUranianSection() const { return usection; }

	/*****************************************************
	**
	**   UranianChartWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		const auto roles = props->getActiveRoles();
#ifdef DEBUG_URANIANCHARTWIDGET
		debuglog << "UranianChartWidget::OnDataChanged role count " << roles.size() 
			<< " offset " << usection->offset;

		for( const auto &role : roles )
		{
			debuglog << "UranianChartWidget::OnDataChanged role " << role;
		}
#endif

		if ( roles.size() > 0 )
		{
			// handling for hand shifted second chart
			if ( props->getDataContext() == DataContext::Radix
				&& roles.size() > 1 )
			{
				Horoscope_ptr h1 = props->getHoroscope( roles[0] );
				assert( h1 );
				Horoscope_ptr h2 = props->getHoroscope( roles[1] );

				if ( h1->getRole() == HoroscopeRole::Radix 
					&& h2
					&& h2->getRole() == HoroscopeRole::HandShifted )
				{
	#ifdef DEBUG_URANIANCHARTWIDGET
					debuglog << "UranianChartWidget::OnDataChanged update second horoscope";
	#endif
					h2->setDate( h1->getJD());
					h2->dl.location = h1->dl.location;
					h2->update();
					h2->setLongitudeOffset( getUranianSection()->offset );
				}
			}
		}

		wrapper->OnDataChanged();
		wrapper->Refresh();
	}

protected:

	/*****************************************************
	**
	**   UranianChartWidget   ---   OnSidePanel
	**
	******************************************************/
	void OnSidePanel( wxCommandEvent& )
	{
#ifdef DEBUG_URANIANCHARTWIDGET
		debuglog << "UranianChartWidget::OnSidePanel";
#endif
		OnDataChanged();
	}

	/*****************************************************
	**
	**   UranianChartWidget   ---   OnToggleOffset
	**
	******************************************************/
	void OnToggleOffset( wxCommandEvent &event )
	{
#ifdef DEBUG_URANIANCHARTWIDGET
		debuglog << "UranianChartWidget::OnToggleOffset value " << event.IsChecked()
			<< " usection->offsetToggle " << usection->offsetToggle
			<< " usection->offset " << usection->offset;
#endif
		//slider->Show( event.IsChecked());

		if ( ! event.IsChecked())
		{
			slider->SetValue( 0 );
			usection->offset = 0;
			leftpanel->setDegreeSpinValue( 0 );
		}
		slider->Show( event.IsChecked() );
		Layout();
		//Refresh();
		OnDataChanged();

#if 0
		if ( slider->IsShown())
		{
			if ( ! event.IsChecked())
			{
				slider->Show( false );
				slider->SetValue( 0 );
				//expert->getUranianSection()->offset = 0;
				usection->offset = 0;
			}
		}
		else
		{
			if ( event.IsChecked())
			{
				slider->Show( true );
			}
		}
#endif
	}

	/*****************************************************
	**
	**   UranianChartWidget   ---   OnSlider
	**
	******************************************************/
	void OnSlider( wxCommandEvent &event )
	{
		const int v = event.GetInt();
		const int d = usection->chartDivision;
		const double offset = (double)v/ (double)d;
		usection->offset = offset;


		// TODO DegreeSpin setzen double value ist dummy
		leftpanel->setDegreeSpinValue( 0 );

		OnDataChanged();
#ifdef DEBUG_URANIANCHARTWIDGET
		debuglog << "UranianChartWidget::OnSlider value " << v << " division " << d << " result " << offset;
#endif
	}

	const UranianContext usetting;

	UranianSection *usection = nullptr;

	HybridChart *chart = nullptr;

	GdiWidgetWrapper *wrapper = nullptr;

	SidebarPanel *leftpanel = nullptr;

	wxSlider *slider = nullptr;
};

/*****************************************************
**
**   WidgetFactory   ---   createUranianChartWidget
**
******************************************************/
BasicWidget *WidgetFactory::createUranianChartWidget( wxWindow *parent )
{
	return new UranianChartWidget( parent, props );
}



