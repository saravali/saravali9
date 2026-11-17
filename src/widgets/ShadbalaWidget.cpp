/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/ShadbalaWidget.cpp
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

#include "BarDiagram.h"
#include "BasicWidget.h"
#include "constants.h"
#include "ContextProperties.h"
#include "Commands.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "GridWrapper.h"
#include "Horoscope.h"
#include "GdiWidgetGrid.h"
#include "Shadbala.h"
#include "ShadbalaDiagrams.h"
#include "TextWidget.h"
#include "WidgetFactory.h"

#include <wx/notebook.h>
#include <wx/sizer.h>

extern Gdiconfig *gdiconfig;

/*************************************************//**
*
*  view containing diagrams and text output for Shad Bala
*
******************************************************/
class ShadbalaWidget : public BasicWidget
{
public:

	/*****************************************************
	**
	**   ShadbalaWidget   ---   Constructor
	**
	******************************************************/
	ShadbalaWidget( wxWindow *parent, ContextProps_ptr props )
		: BasicWidget( parent, props, WidgetCategory::ShadbalaWidget )
	{
		expert = new ShadbalaExpert( props );
		notebook = new wxNotebook( this, -1 );

		overviewWrapper = new GridWrapper( notebook, props, WidgetCategory::BalaOverviewDiagram, WidgetGridFormat::OptimizedFloat );
		const std::list<BalaId> overviewIds =
		{
			BalaId::Sthaana,
			BalaId::Dig,
			BalaId::Kala,
			BalaId::Cheshta,
			BalaId::Naisargika,
			BalaId::Drig
		};
		for ( BalaId id : overviewIds )
		{
			overviewWrapper->addWidget( new ShadbalaDiagram( overviewWrapper->getProps(), expert, id ));
		}
		notebook->AddPage( overviewWrapper, _( "6 Balas" ));

		sthanaWrapper = new GridWrapper( notebook, props, WidgetCategory::SthanabalaDiagram, WidgetGridFormat::OptimizedFloat );
		const std::list<BalaId> sthanaIds =
		{
			BalaId::Sthaana,
			BalaId::Uchcha,
			BalaId::Samasaptavargaja,
			BalaId::Ojhajugma,
			BalaId::Kendradi,
			BalaId::Drekkana
		};
		for ( BalaId id : sthanaIds )
		{
			sthanaWrapper->addWidget( new ShadbalaDiagram( sthanaWrapper->getProps(), expert, id ));
		}
		notebook->AddPage( sthanaWrapper, _( "Sthana Bala" ));

		saptaWrapper = new GridWrapper( notebook, props, WidgetCategory::SaptabalaDiagram, WidgetGridFormat::OptimizedFloat );
		saptaWrapper->addWidget(
			new ShadbalaDiagram( saptaWrapper->getProps(), expert, BalaId::Samasaptavargaja ));
		for ( int v = 0; v < 7; ++v )
		{
			saptaWrapper->addWidget( new SaptavargajaDiagram( saptaWrapper->getProps(), expert, v ));
		}
		notebook->AddPage( saptaWrapper, _( "Saptavargaja Bala" ));

		kalaWrapper = new GridWrapper( notebook, props, WidgetCategory::KalabalaDiagram, WidgetGridFormat::OptimizedFloat );
		const std::list<BalaId> kalaIds =
		{
			BalaId::Kala,
			BalaId::Nathonatha,
			BalaId::Paksha,
			BalaId::Tribhaga,
			BalaId::Varshamasadinahora,
			BalaId::Ayana,
			BalaId::Yudhdha
		};
		for ( BalaId id : kalaIds )
		{
			kalaWrapper->addWidget( new ShadbalaDiagram( kalaWrapper->getProps(), expert, id ));
		}
		notebook->AddPage( kalaWrapper, _( "Kala Bala" ));

		summaryWrapper = new GridWrapper( notebook, props, WidgetCategory::BalaSummaryDiagram, WidgetGridFormat::OptimizedFloat );
		const std::list<BalaId> summaryIds =
		{
			BalaId::ShadTotal,
			BalaId::ShadPercent
		};
		for ( BalaId id : summaryIds )
		{
			summaryWrapper->addWidget( new ShadbalaDiagram( summaryWrapper->getProps(), expert, id ));
		}
		notebook->AddPage( summaryWrapper, _( "Summary" ));

		text = new TextWidget( notebook, props, WidgetCategory::TextShadbala );
		notebook->AddPage( text, _( "Text" ));
		notebook->SetSelection( gdiconfig->getActiveNotebookTab( enum_to_string( category )));

		mainsizer->Add( notebook, 1, wxEXPAND, 3 );
		SetSizer( mainsizer );

		OnDataChanged();
	}

	/*****************************************************
	**
	**   ShadbalaWidget   ---   Destructor
	**
	******************************************************/
	~ShadbalaWidget()
	{
	  gdiconfig->setActiveNotebookTab( enum_to_string( category ), notebook->GetSelection());
		delete expert;
	}

	/*****************************************************
	**
	**   ShadbalaWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		expert->updateAllBalas();
		writeText();

		summaryWrapper->OnDataChanged();
		overviewWrapper->OnDataChanged();
		sthanaWrapper->OnDataChanged();
		saptaWrapper->OnDataChanged();
		kalaWrapper->OnDataChanged();

		notebook->GetCurrentPage()->Refresh();
	}

protected:

	/*****************************************************
	**
	**   ShadbalaWidget   ---   writeText
	**
	******************************************************/
	void writeText()
	{
		text->clearDoc();
		expert->write( text->getDoc());
		text->OnDataChanged();
	}

  wxNotebook *notebook;

	GridWrapper *overviewWrapper;

	GridWrapper *sthanaWrapper;

	GridWrapper *kalaWrapper;

	GridWrapper *saptaWrapper;

	GridWrapper *summaryWrapper;

	TextWidget *text;

	ShadbalaExpert *expert;
};

/*****************************************************
**
**   WidgetFactory  ---   createShadbalaWidget
**
******************************************************/
BasicWidget *WidgetFactory::createShadbalaWidget( wxWindow *parent )
{
	return new ShadbalaWidget( parent, props );
}


