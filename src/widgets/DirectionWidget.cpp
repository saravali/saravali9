/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/DirectionWidget.cpp
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

#include "ContextProperties.h"
#include "CommonControls.h"
#include "CombiWidget.h"
#include "Commands.h"
#include "DateFormatter.h"
#include "Direction.h"
#include "gdiconfig.h"
#include "Lang.h"
#include "MDocument.h"
#include "NotebookWidget.h"
#include "SpinCtrl.h"
#include "ToolPanel.h"
#include "WidgetFactory.h"

extern Gdiconfig *gdiconfig;

#define DEBUG_DIRECTIONWIDGET

/*************************************************//**
*
* Writes the direction report to the text widget
*
******************************************************/
class DirectionCombiWidgetDecorator : public CombiWidgetDecorator
{
public:

	DirectionCombiWidgetDecorator( DirectionExpert *expert ) :  expert( expert ) {}

	virtual int writePlanetListDoc( MDocument_ptr doc ) override
	{
#ifdef DEBUG_DIRECTIONWIDGET
		debuglog << "DirectionCombiWidgetDecorator::writePlanetListDoc";
#endif

		assert( doc );
		doc->clear();

		expert->writeDirectionReport( doc );
		return 0;
	}

private:

	DirectionExpert *expert;
};

/*************************************************//**
*
*  view container for various direction types
*
******************************************************/
class DirectionWidget : public NotebookWidget
{
public:

	/*****************************************************
	**
	**   DirectionWidget   ---   Constructor
	**
	******************************************************/
	DirectionWidget( wxWindow *parent, ContextProps_ptr props )
		: NotebookWidget( parent, props, WidgetCategory::DirectionWidget,
			gdiconfig->multipleView.extractItems( DataContext::Direction ),
			ToolbarWidgetId::DirectionWidget ),
		expert( props )
	{
#ifdef DEBUG_DIRECTIONWIDGET
		debuglog << "DirectionWidget: transit mode in constructor is " << (int)gdiconfig->prefs.directionMode;
#endif

		dateCtrl = getToolbarControl<DateInputCombiCtrl>( ToolbarCmd::DateSelectionCombiCtrl );
		assert( dateCtrl );
		dateCtrl->setIsLocaltime( gdiconfig->prefs.transitIsLocalTime );


		choiceMode = getToolbarControl<DirectionModeChoice>( ToolbarCmd::ChoiceDirectionMode );
		assert( choiceMode );
		choiceMode->setDirectionMode( gdiconfig->prefs.directionMode );

		for( auto *page : pages )
		{
			if ( page->getWidgetCategory() == WidgetCategory::PlanetListChartPair )
			{
				CombiWidget *textWidget = dynamic_cast<CombiWidget*>( page );
				assert( textWidget );
				textWidget->setDecorator( new DirectionCombiWidgetDecorator( &expert ));
			}
		}

		// must be done again after setIsLocaltime
		dateCtrl->setCurrentDate();
		OnDataChanged();

		Connect((int)CommandId::NewContextHoroscope, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DirectionWidget::OnCreateChart ));
	}

	/*****************************************************
	**
	**   DirectionWidget   ---   Destructor
	**
	******************************************************/
	~DirectionWidget()
	{
		gdiconfig->prefs.directionMode = transitRole;
		gdiconfig->prefs.transitIsLocalTime = dateCtrl->getIsLocaltime();
	}

	/**************************************************************
	***
	**   DirectionWidget   ---   getTitle
	***
	***************************************************************/
  virtual wxString getTitle( const bool /*shortname*/ = false ) override
	{
		return Lang().getHoroscopeRoleName( transitRole );
  }

	/*****************************************************
	**
	**   DirectionWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
#ifdef DEBUG_DIRECTIONWIDGET
		debuglog << "DirectionWidget::OnDataChanged transit date "
			<< DateFormatter::get()->formatDatetime( dateCtrl->getJD(), dateCtrl->getIsLocaltime() );
#endif

		props->isLocaltime = dateCtrl ? dateCtrl->getIsLocaltime() : false;

		transitRole = choiceMode->getDirectionMode();
		BasicWidget::OnDataChanged();
		expert.calculateDirections( dateCtrl->getJD(), transitRole );

		assert( toolbar );
		//toolbar->EnableTool( (int)ToolbarCmd::ButtonContextHoroscope, transitRole == HoroscopeRole::Progressive );
		toolbar->EnableTool( (int)CommandId::NewContextHoroscope, transitRole == HoroscopeRole::Progressive );

		setDirty();
		sendUpdateTitleEvent();
	}

protected:

	/*****************************************************
	**
	**   DirectionWidget   ---   OnCreateChart
	**
	******************************************************/
	void OnCreateChart( wxCommandEvent& )
	{
		debuglog << "DirectionWidget::OnCreateChart";
		DirectionCalculator *calculator = DirectionCalculator::get();

		Horoscope_ptr radixHoroscope = props->getRadixHoroscope();
		assert( radixHoroscope );
		Horoscope_ptr h = Horoscope_ptr( new Horoscope );

		h->dl.location = radixHoroscope->dl.location;

		h->setDate( calculator->transitToProgressiveJD( radixHoroscope->getJD(), dateCtrl->getJD()));

		wxString s;
		s << props->getRadixHoroscope()->dl.name << ": " << tr( "Direction Chart" );
		h->dl.name = s;

		wxCommandEvent e( APPWINDOW_CREATEVIEW, GetId());
		e.SetEventObject( new HoroscopeWrapper( h ));
		wxPostEvent( GetParent(), e );
	}

	DirectionExpert expert;

	DirectionModeChoice *choiceMode = nullptr;

	HoroscopeRole transitRole = HoroscopeRole::None;

	DateInputCombiCtrl *dateCtrl = nullptr;
};


/**************************************************************
***
**   WidgetFactory   ---   createDirectionWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createDirectionWidget( wxWindow *parent )
{
	return new DirectionWidget( parent, props );
}

