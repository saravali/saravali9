/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/SolarWidget.cpp
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

#include "BasicWidget.h"
#include "ContextProperties.h"
#include "CombiWidget.h"
#include "Commands.h"
#include "DateFormatter.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "MDocument.h"
#include "NotebookWidget.h"
#include "SolarHoroscope.h"
#include "ToolPanel.h"
#include "WidgetFactory.h"

#include <wx/sizer.h>
#include <wx/spinctrl.h>

extern Gdiconfig *gdiconfig;

/*************************************************//**
*
* Writes the solar report to the text widget
*
******************************************************/
class SolarCombiWidgetDecorator : public CombiWidgetDecorator
{
public:

	SolarCombiWidgetDecorator( SolarHoroscopeExpert *expert ) :  expert( expert ) {}

	virtual int writePlanetListDoc( MDocument_ptr doc ) override
	{
		debuglog << "SolarCombiWidgetDecorator::writePlanetListDoc";
		assert( doc );
		doc->clear();
		expert->writeSolarReturnData( doc );
		return 0;
	}

private:
	SolarHoroscopeExpert *expert;
};


/*************************************************//**
*
*  Widget for Solar (annual) chart
*
******************************************************/
class SolarWidget : public NotebookWidget
{
public:

	/*****************************************************
	**
	**   SolarWidget   ---   Constructor
	**
	******************************************************/
	SolarWidget( wxWindow *parent, ContextProps_ptr props )
		: NotebookWidget( parent, props, WidgetCategory::SolarWidget,
			gdiconfig->multipleView.extractItems( DataContext::Solar ),
			ToolbarWidgetId::SolarWidget )
	{
		expert = new SolarHoroscopeExpert( props );
		setCurrentYear();
		expert->update();

#if 0
	  widget = new CombiWidget( parent, props, WidgetCategory::PlanetListChartPair );
		widget->setDecorator( new SolarCombiWidgetDecorator( expert ));
		mainsizer->Add( widget, 1, wxEXPAND, 3 );
		SetSizer( mainsizer );
#else

		for( auto *page : pages )
		{
			if ( page->getWidgetCategory() == WidgetCategory::PlanetListChartPair )
			{
				CombiWidget *textWidget = dynamic_cast<CombiWidget*>( page );
				assert( textWidget );
				textWidget->setDecorator( new SolarCombiWidgetDecorator( expert ));
			}
		}
#endif

		assert( toolbar );
		spin_year = getToolbarControl<wxSpinCtrl>( ToolbarCmd::SpinYear );
		if ( spin_year )
		{
			spin_year->SetValue( year );
		}
		OnDataChanged();

		Connect( (int)ToolbarCmd::SpinYear, wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SolarWidget::OnYearSpin ));
		Connect( (int)CommandId::Now, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolarWidget::OnNow ));
		Connect( (int)CommandId::NewContextHoroscope, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolarWidget::OnCreateChart ));
	}

	/*****************************************************
	**
	**   SolarWidget   ---   Destructor
	**
	******************************************************/
	~SolarWidget()
	{
		delete expert;
	}

	/*****************************************************
	**
	**   SolarWidget   ---   postCreate
	**
	******************************************************/
#if 0
	virtual void postCreate() override
	{
		assert( widget );
		widget->postCreate();
	}
#endif

protected:

	/*****************************************************
	**
	**   SolarWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		debuglog << "SolarWidget::OnDataChanged";
		expert->setSolarReturnYear( year );
		expert->update();
		setDirty();
		/*
		widget->OnDataChanged();
		guiHasChanged();
		Refresh();
		*/
	}

	/*****************************************************
	**
	**   SolarWidget   ---   setCurrentYear
	**
	******************************************************/
	void setCurrentYear()
	{
		DateFormatter *formatter = DateFormatter::get();

		int aday, amonth;
		std::tie( year, amonth, aday ) = formatter->calculateDateIntegers( MDate::getCurrentJD());

		const auto[ byear, bmonth, bday ] = formatter->calculateDateIntegers( props->getRadixHoroscope()->getJD());
		if ( amonth < bmonth || ( amonth == bmonth && aday < bday )) year--;
	}

	/*****************************************************
	**
	**   SolarWidget   ---   OnNow
	**
	******************************************************/
	void OnNow( wxCommandEvent& )
	{
		setCurrentYear();
		if ( wxSpinCtrl *spin = getToolbarControl<wxSpinCtrl>( ToolbarCmd::SpinYear ); spin )
		{
			spin->SetValue( year );
		}
		OnDataChanged();
	}

	/*****************************************************
	**
	**   SolarWidget   ---   OnCreateChart
	**
	******************************************************/
	void OnCreateChart( wxCommandEvent& )
	{
		Horoscope_ptr h = Horoscope_ptr( new Horoscope );
		h->dl.location = props->getRadixHoroscope()->dl.location;
		h->setDate( expert->getSolarHoroscope()->getJD() );
		wxString s;
		s << props->getRadixHoroscope()->dl.name << ": " << tr( "Annual Chart" ) << " (" << year << ")";
		h->dl.name = s;

		wxCommandEvent e( APPWINDOW_CREATEVIEW, GetId());
		e.SetEventObject( new HoroscopeWrapper( h ));
		wxPostEvent( GetParent(), e );
	}

	/*****************************************************
	**
	**   SolarWidget   ---   OnYearSpin
	**
	******************************************************/
	void OnYearSpin( wxSpinEvent& )
	{
		assert( spin_year );
		year = spin_year->GetValue();
		OnDataChanged();
	}

	SolarHoroscopeExpert *expert;

	//CombiWidget *widget = nullptr;

	wxSpinCtrl *spin_year = nullptr;

	int year;
};

/**************************************************************
***
**   WidgetFactory   ---   createSolarWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createSolarWidget( wxWindow *parent )
{
	return new SolarWidget( parent, props );
}


