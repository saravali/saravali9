/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/SpWidget.cpp
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

#include "Commands.h"
#include "ContextProperties.h"
#include "PlanetFilterDialog.h"
#include "gdiconfig.h"
#include "SpWriter.h"
#include "TextWidget.h"
#include "ToolPanel.h"
#include "Term.h"
#include "UranianControls.h"
#include "WidgetFactory.h"

#include <wx/choice.h>

extern Gdiconfig *gdiconfig;

/*************************************************//**
*
*
*
******************************************************/
class SpWidget : public TextWidget
{
	public:

	SpWidget( wxWindow *parent, ContextProps_ptr props )
		: TextWidget( parent, props, WidgetCategory::SpWidget, ToolbarWidgetId::SpWidget )
	{
		assert( props->getRadixHoroscope());
		usection = new UranianSection;
		usection->filter.rule = 2; // only these in terms
		const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Uranian );

		// get references and init toolbar items
		choiceTerm = getToolbarControl<SpTermChoice>( ToolbarCmd::ChoiceSpTerm );
		assert( choiceTerm );
		choiceTerm->setSelectedTerm( (TermId)gdiconfig->prefs.termSpWidget );

		choiceMatchDivision = getToolbarControl<DivisionChoice>( ToolbarCmd::ChoiceDivision );
		assert( choiceMatchDivision );
		choiceMatchDivision->setDivision( gdiconfig->prefs.divisionSpWidget );

		choiceDisplayMode = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceSpDisplaymode );
		assert( choiceDisplayMode );
		choiceDisplayMode->Append( tr( "Matrix" ));
		choiceDisplayMode->Append( tr( "By Term" ));
		choiceDisplayMode->Append( tr( "By Longitude" ));
		choiceDisplayMode->SetSelection( gdiconfig->prefs.modeSpWidget );

		choiceObject = getToolbarControl<SpObjectSelectionChoice>( ToolbarCmd::ChoiceSpObject );
		assert( choiceObject );
		choiceObject->initPlanets( viewpoint );

		OnDataChanged();
		Connect((int)CommandId::Filter, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SpWidget::OnFilter ));
	}

	/**************************************************************
	***
	**   SpWidget   ---   Destructor
	***
	***************************************************************/
	~SpWidget()
	{
		gdiconfig->prefs.termSpWidget = (int)choiceTerm->getSelectedTermId();
		gdiconfig->prefs.divisionSpWidget = choiceMatchDivision->getDivision();
		gdiconfig->prefs.modeSpWidget = choiceDisplayMode->GetSelection();
		delete usection;
	}

	protected:

	/*****************************************************
	**
	**   SpWidget   ---   OnFilter
	**
	******************************************************/
	void OnFilter( wxCommandEvent& )
	{
		PlanetFilterDialog dialog( this, props, UranianContext::SensitivePoints, HoroscopeRole::Radix, usection->filter );
		if ( dialog.ShowModal() == wxID_OK )
		{
			usection->filter = dialog.getFilter();
			OnDataChanged();
		}
	}

	/*****************************************************
	**
	**   SpWidget   ---   guiHasChanged
	**
	******************************************************/
	virtual void guiHasChanged() override
	{
		debuglog << "SpWidget::guiHasChanged";
		choiceObject->Enable( MTerm::isTripleTerm( choiceTerm->getSelectedTermId() ));
	}

	/*****************************************************
	**
	**   SpWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		debuglog << "SpWidget::OnDataChanged mode " << choiceDisplayMode->GetSelection();

		clearDoc();
		usection->matchDivision = choiceMatchDivision->getDivision();
		const PlanetId ob = choiceObject->getSelectedPlanetId();
		const TermId termId = choiceTerm->getSelectedTermId();

		Horoscope_ptr h;
		if ( props->getDataContext() == DataContext::Direction )
		{
			const auto roles = props->getActiveRoles();
			if (  roles.size() > 1 )
			{
				h = props->getHoroscope( roles[1] );
			}
		}
		else
		{
			h = props->getRadixHoroscope();
		}

		if ( h )
		{
			SensitivePointWriter spwriter( h, props, usection );
			switch( choiceDisplayMode->GetSelection() )
			{
				case 1:
					spwriter.writeList( getDoc(), termId, /*sortByLongitude*/ false, ob );
				break;
				case 2:
					spwriter.writeList( getDoc(), termId, /*sortByLongitude*/ true, ob );
				break;
				case 0:
				default:
					spwriter.writeMatrix( getDoc(), termId, ob );
				break;
			}
		}
		TextWidget::OnDataChanged();
	}

	SpTermChoice *choiceTerm = nullptr;

	DivisionChoice *choiceMatchDivision = nullptr;

	wxChoice *choiceDisplayMode = nullptr;

	UranianSection *usection = nullptr;

	SpObjectSelectionChoice *choiceObject = nullptr;
};

/**************************************************************
***
**   WidgetFactory   ---   createSpWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createSpWidget( wxWindow *parent ) { return new SpWidget( parent, props ); }


