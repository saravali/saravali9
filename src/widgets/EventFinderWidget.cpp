/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/EventFinderWidget.cpp
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
#include "constants.h"
#include "enumserializer.h"
#include "EventFinder.h"
#include "gdiconfig.h"
#include "GdiWidgetWrapper.h"
#include "Horoscope.h"
#include "MatchWriter.h"
#include "MDocument.h"
#include "Sidebar.h"
#include "SpinCtrl.h"
#include "TextWidget.h"
#include "ToolPanel.h"
#include "Uranian.h"
#include "UranianControls.h"
#include "WidgetFactory.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/toolbar.h>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

//#define DEBUG_EVENTFINDERWIDGET

/*************************************************//**
*
*  view for Uranian astrology (midpoint analysis)
*
******************************************************/
class EventFinderWidget : public BasicWidget
{
public:

	/*****************************************************
	**
	**   EventFinderWidget   ---   Constructor
	**
	******************************************************/
	EventFinderWidget( wxWindow *parent, ContextProps_ptr props )
		: BasicWidget( parent, props, WidgetCategory::EventFinderWidget, ToolbarWidgetId::EventFinderWidget )
	{
		usection = new UranianSection( astroconfig->getUranianSection( UranianContext::EventFinder ));
		usection->matchesShowDuedate = true;
		usection->matchesShowOrbis = false;

		assert( usection );
		if ( usection->roleRestrictors.size() == 0 )
		{
			usection->roleRestrictors.push_back( RoleRestrictor( { HoroscopeRole::Transit, false } ));
		}

		startdate = MDate::getCurrentJD();

		finder = new EventFinder( props, usection );
		writer = new MatchWriter( props, usection, Viewpoint::Uranian );

		durationSelection = gdiconfig->prefs.periodVWidgetDurationChoice;

		leftpanel = SidebarPanelFactory().createSidebarPanel( this, props, UranianContext::EventFinder, usection );
		updateButton = leftpanel->getUpdateButton();
		assert( updateButton );

		textCtrl = new TextWidget( this, props, WidgetCategory::TextUranianMatches );
    wxBoxSizer *hsizer = new wxBoxSizer( wxHORIZONTAL );
    hsizer->Add( leftpanel, 0, wxEXPAND );
    hsizer->Add( textCtrl, 1, wxEXPAND );
    mainsizer->Add( hsizer, 1, wxEXPAND );
		SetSizer( mainsizer );

		assert( toolbar );
		spin_date = getToolbarControl<DateSpin>( ToolbarCmd::InputDate );
		if ( spin_date )
		{
			spin_date->changeValuePointer( &startdate );
		}
		else errorlog << "NO date ctrl in EventFinderWidget::init";

		choice_duration = getToolbarControl<PeriodDurationChoice>( ToolbarCmd::ChoicePeriodDuration );
		{
			choice_duration->SetSelection( durationSelection );
			duration = choice_duration->getDuration();
		}
		toolbar->TransferDataToWindow();

		OnDataChanged();

		Connect( (int)CommandId::Now, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EventFinderWidget::OnSetToCurrent ));

		Connect( wxID_ANY, COMMAND_SIDEPANEL_CHANGED, wxCommandEventHandler( EventFinderWidget::OnCommand ));
		Connect( wxID_ANY, COMMAND_SIDEPANEL_REFRESH, wxCommandEventHandler( EventFinderWidget::OnSidePanelRefresh ));
		Connect( wxID_ANY, COMMAND_SIDEPANEL_SORT, wxCommandEventHandler( EventFinderWidget::OnSidePanelSort ));
		Connect( wxEVT_IDLE, wxIdleEventHandler( EventFinderWidget::OnIdle ));

		Connect( SBE_BUTTON_UPDATE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EventFinderWidget::OnButtonUpdate ));
	}

	/*****************************************************
	**
	**   EventFinderWidget   ---   Destructor
	**
	******************************************************/
	~EventFinderWidget()
	{

		gdiconfig->prefs.periodVWidgetDurationChoice = durationSelection;
		astroconfig->getUranianSection( UranianContext::EventFinder ) = *usection;

		delete writer;
		delete finder;
	}

protected:

	/*****************************************************
	**
	**   EventFinderWidget   ---   OnToolbarCommand
	**
	******************************************************/
	virtual void OnToolbarCommand() override
	{
		if ( toolbar )
		{
			toolbar->TransferDataFromWindow();
		}
		if ( choice_duration )
		{
			durationSelection = choice_duration->GetSelection();
			duration = choice_duration->getDuration();
		}
		guiHasChanged();
		OnDataChanged();
	}

	/*****************************************************
	**
	**   EventFinderWidget   ---   OnButtonUpdate 
	**
	******************************************************/
	void OnButtonUpdate ( wxCommandEvent& )
	{
#ifdef DEBUG_EVENTFINDERWIDGET
		debuglog << "EventFinderWidget::OnButtonUpdate isCalculationRunning " << std::boolalpha << isCalculationRunning;
#endif
		assert( updateButton );

		// cancel operation
		if ( isCalculationRunning )
		{
			finder->cancelCalculation();
			isCalculationRunning = false;
			updateButton->SetLabel( tr( "Start Update" ));
		}
		else
		{
			isCalculationRunning = true;
			updateButton->SetLabel( tr( "Cancel" ));

			finder->startCalculation( startdate, startdate + duration );

		}
		OnDataChanged();
	}

	/*****************************************************
	**
	**   EventFinderWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
#ifdef DEBUG_EVENTFINDERWIDGET
		debuglog << "EventFinderWidget::OnDataChanged viewpoint " << getViewpoint();
#endif
		write();
		BasicWidget::OnDataChanged();
	}

	/**************************************************************
	***
	**   EventFinderWidget   ---   OnIdle
	***
	***************************************************************/
	void OnIdle( wxIdleEvent& )
	{
#ifdef DEBUG_EVENTFINDERWIDGET
		debuglog << "EventFinderWidget::OnIdle finder is calculating: " << std::boolalpha << isCalculationRunning
			<< " finder is running: " << finder->isCalculationRunning();
#endif

		if ( isCalculationRunning && ! finder->isCalculationRunning())
		{
#ifdef DEBUG_EVENTFINDERWIDGET
			debuglog << "EventFinderWidget::OnIdle finder has finished";
#endif
			isCalculationRunning = false;
			finder->waitForCalculation();

			assert( updateButton );
			updateButton->SetLabel( tr( "Start Update" ));

			write();
		}
	}

	/*****************************************************
	**
	**   EventFinderWidget   ---   OnSidePanelRefresh
	**
	******************************************************/
	void OnSidePanelRefresh( wxCommandEvent& )
	{
#ifdef DEBUG_EVENTFINDERWIDGET
		debuglog << "EventFinderWidget::OnSidePanelRefresh";
#endif
		if ( ! isCalculationRunning ) write();
	}

	/*****************************************************
	**
	**   EventFinderWidget   ---   OnSidePanelSort
	**
	******************************************************/
	void OnSidePanelSort( wxCommandEvent& )
	{
#ifdef DEBUG_EVENTFINDERWIDGET
		debuglog << "EventFinderWidget::OnSidePanelSort";
#endif
		if ( ! isCalculationRunning ) finder->sort();
		write();
	}

	/*****************************************************
	**
	**   EventFinderWidget   ---   OnSetToCurrent
	**
	******************************************************/
	void OnSetToCurrent( wxCommandEvent& )
	{
		startdate = MDate::getCurrentJD();
		if ( toolbar ) toolbar->TransferDataToWindow();
		OnToolbarCommand();
	}

	/*****************************************************
	**
	**   EventFinderWidget   ---   write
	**
	******************************************************/
	void write()
	{
#ifdef DEBUG_EVENTFINDERWIDGET
		debuglog << "EventFinderWidget::write startdate " << startdate << " duration "
			<< duration << " order " << (int)usection->sortOrder;
#endif

		textCtrl->clearDoc();
		if ( isCalculationRunning )
		{
			textCtrl->getDoc()->addParagraph( tr( "Calculation is running" ));
		}
		else
		{
			writer->writeMatches( textCtrl->getDoc(), finder->getMatches() );
		}
		textCtrl->OnDataChanged();
	}

	bool isCalculationRunning = false;

	double startdate = 0.0;

	double duration = 3;

	int durationSelection = 0;

	EventFinder *finder = nullptr;

	MatchWriter *writer = nullptr;

	UranianSection *usection = nullptr;

	DateSpin *spin_date = nullptr;

	PeriodDurationChoice *choice_duration = nullptr;

	wxButton *updateButton = nullptr;

	SidebarPanel *leftpanel = nullptr;

	TextWidget *textCtrl = nullptr;
};


/**************************************************************
***
**   WidgetFactory   ---   createEventFinderWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createEventFinderWidget( wxWindow *parent )
{
	return new EventFinderWidget( parent, props );
}

