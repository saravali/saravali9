/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/AshtakavargaWidget.cpp
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

#include "Ashtakavarga.h"
#include "AshtakavargaDiagrams.h"
#include "ContextProperties.h"
#include "BasicWidget.h"
#include "gdiconfig.h"
#include "GridWrapper.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "GdiWidgetGrid.h"
#include "TextWidget.h"
#include "ToolPanel.h"
#include "Varga.h"
#include "WidgetFactory.h"

#include <wx/choice.h>
#include <wx/notebook.h>
#include <wx/sizer.h>

extern Gdiconfig *gdiconfig;

/*************************************************//**
*
*  view containing Ashtaka Varga charts, diagrams and text view
*
******************************************************/
class AshtakavargaWidget : public BasicWidget
{
public:

	/*****************************************************
	**
	**   AshtakavargaWidget   ---   Constructor
	**
	******************************************************/
	AshtakavargaWidget( wxWindow *parent, ContextProps_ptr props )
		: BasicWidget( parent, props, WidgetCategory::AvWidget, ToolbarWidgetId::AshtakavargaWidget )
	{
		expert = new AshtakavargaExpert( props, Varga::Rasi );
		notebook = new wxNotebook( this, -1 );

		// chart grids for Rekha, Trikona and Ekadhipatya Shodana
		for( const AvType type : Enum<AvType>( AvType::Rekha, AvType::Ekadhi ))
		{
			GridWrapper *grid = new GridWrapper( notebook, props, WidgetCategory::VargaChart,
				WidgetGridFormat::OptimizedFloat );

			grid->addWidget( new HybridChart( props, expert, type, PlanetId::None ));
			for ( const PlanetId p : PlanetHelper::createAshtakaVargaPlanets())
			{
				grid->addWidget( new HybridChart( props, expert, type, p ));
			}

			chartgrids[type] = grid;
			notebook->AddPage( grid, expert->getAvTypeName( type ));
		}

		// Rasi Pinda diagram
		rasigrid = new GridWrapper( notebook, props, WidgetCategory::AvRasiDiagram, WidgetGridFormat::OptimizedFloat );
		for ( AvType type : Enum<AvType>( AvType::Rekha, AvType::Ekadhi ))
		{
			rasigrid->addWidget( new AvRasiDiagram( props, expert, type ));
		}
		notebook->AddPage( rasigrid, tr( "Rasi Diagram" ));

		// Graha Pinda diagram
		pindagrid = new GridWrapper( notebook, props, WidgetCategory::AvPindaDiagram, WidgetGridFormat::OptimizedFloat );
		for ( PindaType type : Enum<PindaType>() )
		{
			pindagrid->addWidget( new AvPindaDiagram( props, expert, type ));
		}
		notebook->AddPage( pindagrid, tr( "Pinda Diagram" ));

		// text widget
		twidget = new TextWidget( notebook, props, WidgetCategory::TextAshtakavarga );
		notebook->AddPage( twidget, tr( "Text" ));
		notebook->SetSelection( gdiconfig->getActiveNotebookTab( "AvWidget" ));

		mainsizer->Add( notebook, 1, wxEXPAND, 3 );
		SetSizer( mainsizer );

		assert( toolbar );
		choiceVarga = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceVarga );
		assert( choiceVarga );
		choiceVarga->SetSelection( 0 );

		OnDataChanged();
	}

	/*****************************************************
	**
	**   AshtakavargaWidget   ---   Destructor
	**
	******************************************************/
	~AshtakavargaWidget()
	{
	  gdiconfig->setActiveNotebookTab( "AvWidget", notebook->GetSelection());
		delete expert;
	}

	/*****************************************************
	**
	**   AshtakavargaWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		expert->varga = (Varga)choiceVarga->GetSelection();
		expert->update();
		twidget->clearDoc();

		for( auto grid : chartgrids )
		{
			grid.second->OnDataChanged();
		}

		rasigrid->OnDataChanged();
		pindagrid->OnDataChanged();

		expert->write( twidget->getDoc(), gdiconfig->view.showTextViewHeaders );
		twidget->OnDataChanged();

		notebook->GetCurrentPage()->Refresh();
	}

protected:

  wxNotebook *notebook = nullptr;

	wxChoice *choiceVarga = nullptr;

	std::map<AvType, BasicWidget*> chartgrids;

	GridWrapper *pindagrid = nullptr;

	GridWrapper *rasigrid = nullptr;

	TextWidget *twidget = nullptr;

	AshtakavargaExpert *expert = nullptr;
};

/*****************************************************
**
**   WidgetFactory  ---   createAshtakavargaWidget
**
******************************************************/
BasicWidget *WidgetFactory::createAshtakavargaWidget( wxWindow *parent )
{
	return new AshtakavargaWidget( parent, props );
}

