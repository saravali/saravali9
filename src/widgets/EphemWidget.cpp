/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/EphemWidget.cpp
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

#include "Commands.h"
#include "CommonControls.h"
#include "ContextProperties.h"
#include "DateFormatter.h"
#include "datelocation.h"
#include "Ephemeris.h"
#include "EphemDiagram.h"
#include "gdiconfig.h"
#include "GdiWidgetWrapper.h"
#include "Lang.h"
#include "mathbase.h"
#include "TextWidget.h"
#include "ToolPanel.h"
#include "UranianControls.h"
#include "WidgetFactory.h"

#include <wx/choice.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/toolbar.h>

extern Gdiconfig *gdiconfig;

enum class EphemWidgetPage
{
	None = -1,
	Coordinates = 0,
	Graphical,
	Sunrise,
	Ingress,
	Lunar,
	Kp,
	First = Coordinates,
	Last = Kp
};

/*************************************************//**
*
*  ephemeris view containing various text tabs and graphical ephemeris
*
******************************************************/
class EphemWidget : public BasicWidget
{
public:

	/*****************************************************
	**
	**   EphemWidget   ---   Constructor
	**
	******************************************************/
	EphemWidget( wxWindow *parent, ContextProps_ptr props )
		: BasicWidget( parent, props, WidgetCategory::EphemWidget, ToolbarWidgetId::EphemWidget )
	{
		expert = new EphemExpert( props );
		notebook = new wxNotebook( this, -1 );

		// Coordinates
		coordWidget = new TextWidget( notebook, props, WidgetCategory::TextEphemCoordinates );
		notebook->AddPage( coordWidget, _( "Coordinates" ));

		// Graphical
		diagram = new EphemDiagram( props, expert );
		ewrapper = new GdiWidgetWrapper( notebook, diagram );
		notebook->AddPage( ewrapper, _( "Graphical" ));

		// Sunrise
		sunriseWidget = new TextWidget( notebook, props, WidgetCategory::TextEphemDetails );
		notebook->AddPage( sunriseWidget, _( "Sunrise and Sunset" ));

		// Ingress
		ingressWidget = new TextWidget( notebook, props, WidgetCategory::TextEphemIngress );
		notebook->AddPage( ingressWidget, _( "Ingress Events" ));

		// Lunar
		lunarWidget = new TextWidget( notebook, props, WidgetCategory::TextEphemLunar );
		notebook->AddPage( lunarWidget, _( "Lunar" ));

		// Kp
		kpWidget = new TextWidget( notebook, props, WidgetCategory::TextEphemKp );
		notebook->AddPage( kpWidget, _( "Krishnamurti Paddhati" ));
		notebook->SetSelection( gdiconfig->getActiveNotebookTab( "EphemWidget" ));

		mainsizer->Add( notebook, 1, wxEXPAND, 3 );
		SetSizer( mainsizer );
		assert( toolbar );

		// initialize year and month spin
		{
			auto[ y, m, dummy ] = DateFormatter::get()->calculateDateIntegers( MDate::getCurrentJD());
			spinYear = getToolbarControl<wxSpinCtrl>( ToolbarCmd::SpinYear );
			assert( spinYear );
			spinYear->SetValue( y );

			spinMonth = getToolbarControl<wxSpinCtrl>( ToolbarCmd::SpinMonth );
			assert( spinMonth );
			spinMonth->SetValue( m );
		}

		choiceCoordinateType = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceEphemCoordinateType );
		assert( choiceCoordinateType );
		choiceCoordinateType->SetSelection( gdiconfig->prefs.coordinateModeEphem );

		choiceDivision = getToolbarControl<DivisionChoice>( ToolbarCmd::ChoiceDivision );
		assert( choiceDivision );
		choiceDivision->SetSelection( gdiconfig->prefs.divisionEphem );

		choiceDasa = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceDasa );
		assert( choiceDasa );

		choiceTimezone = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceTimezone );
		assert( choiceTimezone );
		choiceTimezone->SetSelection( gdiconfig->prefs.ephemTimezone );

		setDirty();
		OnDataChanged();
		guiHasChanged();

		Connect( wxEVT_IDLE, wxIdleEventHandler( EphemWidget::OnIdle ));
		Connect( wxID_ANY, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( EphemWidget::OnNotebookEvent ));
		Connect( (int)CommandId::Now, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EphemWidget::OnNow ));
	}

	/*****************************************************
	**
	**   EphemWidget   ---   Destructor
	**
	******************************************************/
	~EphemWidget()
	{
	  gdiconfig->setActiveNotebookTab( "EphemWidget", notebook->GetSelection());

		gdiconfig->prefs.ephemTimezone = choiceTimezone->GetSelection();
		gdiconfig->prefs.divisionEphem = choiceDivision->GetSelection();
		gdiconfig->prefs.coordinateModeEphem = choiceCoordinateType->GetSelection();
		delete expert;
	}

	void setDirty( const bool b = true )
	{
		for ( EphemWidgetPage p : Enum<EphemWidgetPage>() )
		{
			dirty[p] = b;
		}
		diagram->setDirty( b );
	}

	/*************************************************//**
	*
	*     EphemWidget   ---   OnDataChanged
	*
	******************************************************/
	virtual void OnDataChanged() override
	{
		const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::None );
		debuglog << "EphemWidget::OnDataChanged viewpoint " << (int)viewpoint
			<< " division " << choiceDivision->getDivision();

		// find out if wrap event took place
		if ( spinMonth->GetValue() == 12 && lastMonth == 1 )
		{
			spinYear->SetValue( spinYear->GetValue() - 1 );
		}
		else if ( spinMonth->GetValue() == 1 && lastMonth == 12 )
		{
			spinYear->SetValue( spinYear->GetValue() + 1 );
		}
		lastMonth = spinMonth->GetValue();

		setDirty();
		expert->prepareMonth( spinYear->GetValue(), spinMonth->GetValue());
		BasicWidget::OnDataChanged();
	}

protected:

	/*****************************************************
	**
	**   EphemWidget   ---   guiHasChanged
	**
	******************************************************/
	virtual void guiHasChanged() override
	{
		const EphemWidgetPage activePage = (EphemWidgetPage)notebook->GetSelection();
		assert_valid_enum<EphemWidgetPage>( activePage );

		const bool hasCoordinates = activePage == EphemWidgetPage::Coordinates || activePage == EphemWidgetPage::Graphical;
		choiceCoordinateType->Enable( hasCoordinates );

		const CoordinateMode coordMode = (CoordinateMode)choiceCoordinateType->GetSelection();
		choiceDivision->Enable( coordMode == CoordinateMode::Longitude && hasCoordinates );

		choiceDasa->Enable( activePage == EphemWidgetPage::Kp );
	}

	/*****************************************************
	**
	**   EphemWidget   ---   OnNotebookEvent
	**
	******************************************************/
	void OnNotebookEvent( wxNotebookEvent& )
	{
		const EphemWidgetPage activePage = (EphemWidgetPage)notebook->GetSelection();
		assert_valid_enum<EphemWidgetPage>( activePage );
		debuglog << "OnNotebook " << (int)activePage;
		guiHasChanged();
		if ( dirty[activePage] ) updatePage( activePage );
	}

	/**************************************************************
	***
	**   EphemWidget   ---   updatePage
	***
	***************************************************************/
	void updatePage( const EphemWidgetPage &page )
	{
		debuglog << "Start updatePage " << (int)page;
		StopWatch sw;
		int ret = 0;
		const CoordinateMode coordMode = (CoordinateMode)choiceCoordinateType->GetSelection();

		if ( choiceTimezone )
		{
			props->isLocaltime = (bool)choiceTimezone->GetSelection();
		}

		switch( page )
		{
			using enum EphemWidgetPage;

			case Coordinates:
			{
				MDocument_ptr doc = coordWidget->getEmptyDoc();
				doc->clear();
				ret = expert->writeCoordinates( doc, coordMode, choiceDivision->getDivision() );
				coordWidget->OnDataChanged();
			}
			break;
			case Graphical:
			{
				diagram->setDivision( choiceDivision->getDivision() );
				diagram->setCoordinateMode( coordMode );
				diagram->OnDataChanged();
				ewrapper->Refresh();
			}
			break;
			case Sunrise:
			{
				MDocument_ptr doc = sunriseWidget->getEmptyDoc();
				expert->writeSunrise( doc );
				sunriseWidget->OnDataChanged();
			}
			break;
			case Ingress:
			{
				MDocument_ptr doc = ingressWidget->getEmptyDoc();
				ret = expert->writeIngress( doc );
				ingressWidget->OnDataChanged();
			}
			break;
			case Lunar:
			{
				MDocument_ptr doc = lunarWidget->getEmptyDoc();
				expert->writeLunar( doc );
				lunarWidget->OnDataChanged();
			}
			break;
			case Kp:
			{
				MDocument_ptr doc = kpWidget->getEmptyDoc();
				expert->writeKp( doc, (DasaId)choiceDasa->GetSelection());
				kpWidget->OnDataChanged();
			}
			break;
			default:
				assert( false );
			break;
		}
		dirty[page] = false;
		Refresh();

		if ( ret )
		{
			showMissingEphemDataMessage( this );
		}
		infolog << "Finished updatePage " << (int)page << " in " << sw.getElapsedMillisec() << " millisec";
	}

	/*************************************************//**
	*
	*     EphemWidget   ---   OnIdle
	*
	******************************************************/
	void OnIdle( wxIdleEvent& )
	{
		const EphemWidgetPage activePage = (EphemWidgetPage)notebook->GetSelection();
		assert_valid_enum<EphemWidgetPage>( activePage );
		if ( dirty[activePage] ) updatePage( activePage );
	}

	/*****************************************************
	**
	**   EphemWidget   ---   OnNow
	**
	******************************************************/
	void OnNow( wxCommandEvent& )
	{
		auto[ y, m, dummy ] = DateFormatter::get()->calculateDateIntegers( MDate::getCurrentJD());
		spinYear->SetValue( y );
		spinMonth->SetValue( m );
		OnDataChanged();
	}

	EphemExpert *expert = nullptr;

	std::map<EphemWidgetPage, bool> dirty;

  wxNotebook *notebook = nullptr;

	wxSpinCtrl *spinYear = nullptr;

	wxSpinCtrl *spinMonth = nullptr;

	int lastMonth = -1;

	wxChoice *choiceTimezone = nullptr;

	wxChoice *choiceCoordinateType = nullptr;

	wxChoice *choiceDasa = nullptr;

	DivisionChoice *choiceDivision = nullptr;

	EphemDiagram *diagram = nullptr;

	GdiWidgetWrapper *ewrapper = nullptr;

	TextWidget *coordWidget, *sunriseWidget, *ingressWidget, *lunarWidget, *kpWidget;
};

/**************************************************************
***
**   WidgetFactory   ---   createEphemWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createEphemWidget( wxWindow *parent )
{
	return new EphemWidget( parent, props );
}

