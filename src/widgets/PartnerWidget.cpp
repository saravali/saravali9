/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/PartnerWidget.cpp
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

#include "Ashtakoota.h"
#include "BasicWidget.h"
#include "ContextProperties.h"
#include "CompositeHoroscope.h"
#include "CombiWidget.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "MDocument.h"
#include "NotebookWidget.h"
#include "Sidebar.h"
#include "TextHelper.h"
#include "TextWidget.h"
#include "ToolPanel.h"
#include "UranianControls.h"
#include "Varga.h"
#include "WindowDirectory.h"
#include "WidgetFactory.h"

#include <wx/choice.h>
#include <wx/sizer.h>

extern Gdiconfig *gdiconfig;

#define DEBUG_PARTNERWIDGET

/*************************************************//**
*
* Writes the partner composite report to the text widget
*
******************************************************/
class CompositeCombiWidgetDecorator : public CombiWidgetDecorator
{
public:

	CompositeCombiWidgetDecorator( ContextProps_ptr props ) :  props( props ) {}

	virtual int writePlanetListDoc( MDocument_ptr doc ) override
	{
#ifdef DEBUG_PARTNERWIDGET
		debuglog << "CompositeCombiWidgetDecorator::writePlanetListDoc";
#endif
		assert( doc );
		doc->clear();
		Horoscope_ptr h3 = props->getHoroscope( HoroscopeRole::Composite );

		if ( h3 ) 
		{
			h3->update();
			return TextHelper( props, doc ).writePlanets( h3 );
		}
		else
		{
			TextHelper( props, doc ).writeInvalidPartnerDoc();
			return 0;
		}
	}

private:
	ContextProps_ptr props;
};


/*************************************************//**
*
* Writes the partner synastry report to the text widget
*
******************************************************/
class SynastryCombiWidgetDecorator : public CombiWidgetDecorator
{
public:

	SynastryCombiWidgetDecorator( ContextProps_ptr props ) :  props( props ) {}

	virtual int writePlanetListDoc( MDocument_ptr doc ) override
	{
#ifdef DEBUG_PARTNERWIDGET
		debuglog << "SynastryCombiWidgetDecorator::writePlanetListDoc";
#endif
		assert( doc );
		doc->clear();
		Horoscope_ptr h1 = props->getHoroscope( HoroscopeRole::Partner1 );
		Horoscope_ptr h2 = props->getHoroscope( HoroscopeRole::Partner2 );

		if ( h1 && h2 )
		{
			return TextHelper( props, doc ).writeSynastryReport();
		}
		else
		{
			TextHelper( props, doc ).writeInvalidPartnerDoc();
			return 0;
		}
	}

private:
	ContextProps_ptr props;
};

/*************************************************//**
*
* 
*
******************************************************/
class AshtakootaWidget : public TextWidget
{
public:

	AshtakootaWidget( wxWindow *parent, ContextProps_ptr props )
		: TextWidget( parent, props, WidgetCategory::TextAshtakoota )
	{
	}

	/*****************************************************
	**
	**   AshtakootaWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
#ifdef DEBUG_PARTNERWIDGET
		debuglog << "AshtakootaWidget::OnDataChanged";
#endif
		MDocument_ptr doc = getDoc();
		doc->clear();

    Horoscope_ptr h1 = props->getHoroscope( HoroscopeRole::Partner1 );
    Horoscope_ptr h2 = props->getHoroscope( HoroscopeRole::Partner2 );

		if ( ! h1 || ! h2 || h1 == h2 ) 
		{
			doc->addParagraph( tr( "Charts either not set or identical." ));
		}
		else
		{
			AshtakootaExpert expert( props );
			expert.update();
			expert.writeComprehensive( doc );
		}
		TextWidget::OnDataChanged();
	}
};

/*****************************************************
**
**   WidgetFactory   ---   createAshtakootaWidget
**
******************************************************/
BasicWidget *WidgetFactory::createAshtakootaWidget( wxWindow *parent )
{
	return new AshtakootaWidget( parent, props );
}


/*************************************************//**
*
*  view for widgets related to partner charts
*
******************************************************/
class PartnerWidget : public NotebookWidget
{
public:

	/*****************************************************
	**
	**   PartnerWidget   ---   Constructor
	**
	******************************************************/
	PartnerWidget( wxWindow *parent, ContextProps_ptr props )
		: NotebookWidget( parent, props, WidgetCategory::PartnerWidget,
			gdiconfig->multipleView.extractItems( DataContext::Partner ),
			ToolbarWidgetId::PartnerWidget )
	{
		WindowDirectory::get()->subscribe( this );

		for( auto *page : pages )
		{
			switch( page->getWidgetCategory())
			{
				case WidgetCategory::CompositePair:
				{
					Horoscope_ptr ch( new CompositeHoroscope( props ));
					props->setHoroscope( HoroscopeRole::Composite, ch );

					CombiWidget *textWidget = dynamic_cast<CombiWidget*>( page );
					assert( textWidget );
					textWidget->setDecorator( new CompositeCombiWidgetDecorator( props ));
				}
				break;

				case WidgetCategory::SynastryPair:
				{
					CombiWidget *textWidget = dynamic_cast<CombiWidget*>( page );
					assert( textWidget );
					textWidget->setDecorator( new SynastryCombiWidgetDecorator( props ));
				}
				break;
				default:
				break;
			}
		}

		assert( toolbar );
		choice_partner1 = getToolbarControl<wxChoice>( ToolbarCmd::ChoicePartner1 );
		assert( choice_partner1 );
		choice_partner2 = getToolbarControl<wxChoice>( ToolbarCmd::ChoicePartner2 );
		assert( choice_partner2 );

		updatePartnerChoices();
		OnDataChanged();

		Connect( (int)ToolbarCmd::ChoicePartner1, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PartnerWidget::OnPartnerChoice ));
		Connect( (int)ToolbarCmd::ChoicePartner2, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PartnerWidget::OnPartnerChoice ));
		Connect( (int)ToolbarCmd::ChoicePartnerOrigin, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PartnerWidget::OnPartnerChoice ));

		Connect( wxID_ANY, COMMAND_SIDEPANEL_CHANGED, wxCommandEventHandler( PartnerWidget::OnToolbarCommand ));
		Connect( wxID_ANY, COMMAND_SIDEPANEL_SORT, wxCommandEventHandler( PartnerWidget::OnToolbarCommand ));

		Connect( wxID_ANY, WD_HOROSCOPE_LIST_CHANGED, wxCommandEventHandler( PartnerWidget::OnHoroscopeListChanged ));
		Connect( wxID_ANY, WD_HOROSCOPE_DATA_CHANGED, wxCommandEventHandler( PartnerWidget::OnHoroscopeDataChanged ));

	}

	/*****************************************************
	**
	**   PartnerWidget   ---   Destructor
	**
	******************************************************/
	~PartnerWidget()
	{
		WindowDirectory::get()->unsubscribe( this );
		props->setHoroscope( HoroscopeRole::Partner1, nullptr );
		props->setHoroscope( HoroscopeRole::Partner2, nullptr );
		props->setHoroscope( HoroscopeRole::Composite, nullptr );
	}

protected:

	void OnGeneralCommand( wxCommandEvent& ) { OnDataChanged(); }

	/*****************************************************
	**
	**   PartnerWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		Horoscope_ptr horoscope1 = props->getHoroscope( HoroscopeRole::Partner1 );
		Horoscope_ptr horoscope2 = props->getHoroscope( HoroscopeRole::Partner2 );
#ifdef DEBUG_PARTNERWIDGET
		debuglog << "PartnerWidget::OnDataChanged "
			<< " h1 " << horoscope1
			<< " h2 " << horoscope2;
#endif

		Horoscope_ptr ch = props->getHoroscope( HoroscopeRole::Composite );
		if ( horoscope1 && horoscope2 && horoscope1 != horoscope2 )
		{
			if ( ch ) ch->update();
		}
		else
		{
			if ( ch ) props->setHoroscope( HoroscopeRole::Composite, nullptr );
			props->setHoroscope( HoroscopeRole::Partner1, nullptr );
			props->setHoroscope( HoroscopeRole::Partner2, nullptr );
		}
		setDirty();
	}

	/*****************************************************
	**
	**   PartnerWidget   ---   setupCharts
	**
	******************************************************/
	void setupCharts()
	{
		WindowDirectory *directory = WindowDirectory::get();

		Horoscope_ptr horoscope1;
		Horoscope_ptr horoscope2;

		const int sel1 = choice_partner1->GetSelection();
		if ( sel1 > -1 ) horoscope1 = directory->getHoroscope( sel1 );

		const int sel2 = choice_partner2->GetSelection();
		if ( sel2 > -1 ) horoscope2 = directory->getHoroscope( sel2 );

		if ( horoscope1 && horoscope2 && horoscope1 != horoscope2 )
		{
			props->setHoroscope( HoroscopeRole::Partner1, horoscope1 );
			props->setHoroscope( HoroscopeRole::Partner2, horoscope2 );
		}
		else
		{
			props->setHoroscope( HoroscopeRole::Partner1, nullptr );
			props->setHoroscope( HoroscopeRole::Partner2, nullptr );
			props->setHoroscope( HoroscopeRole::Composite, nullptr );
		}
#ifdef DEBUG_PARTNERWIDGET
		debuglog << "PartnerWidget::setupCharts " << " h1 " << horoscope1 << " h2 " << horoscope2;
#endif
	}

	/*****************************************************
	**
	**   PartnerWidget   ---   OnPartnerChoice
	**
	******************************************************/
	void OnPartnerChoice( wxCommandEvent& )
	{
		setupCharts();
		OnDataChanged();
	}

	/*****************************************************
	**
	**   PartnerWidget   ---   updatePartnerChoices
	**
	******************************************************/
	void updatePartnerChoices()
	{
		WindowDirectory *directory = WindowDirectory::get();

		const int docnum = (int)directory->getHoroscopeCount();

		int sel1 = choice_partner1->GetSelection();
		int sel2 = choice_partner2->GetSelection();

#ifdef DEBUG_PARTNERWIDGET
		debuglog << "PartnerWidget::updatePartnerChoices docnum " << docnum << " #1 sel1 " << sel1 << " sel2 " << sel2;
#endif
		//if ( sel2 >= docnum ) sel2 = sel1;

		// set first choice box to first element of not set
		if ( sel1 == -1 && docnum > 0 ) sel1 = 0;

		// set second choice box to last element of not set
		if ( ( sel2 == sel1 || sel2 == -1 ) && docnum > 0 ) sel2 = docnum - 1;

#ifdef DEBUG_PARTNERWIDGET
		debuglog << "PartnerWidget::updatePartnerChoices #2 sel1 " << sel1 << " sel2 " << sel2;
#endif
		choice_partner1->Clear();
		choice_partner2->Clear();
		for( const wxString &name : directory->getNames())
		{
			choice_partner1->Append( name );
			choice_partner2->Append( name );
		}

		if ( sel1 >= 0 ) choice_partner1->SetSelection( sel1 );
		if ( sel2 >= 0 ) choice_partner2->SetSelection( sel2 );

		Horoscope_ptr horoscope1;
		Horoscope_ptr horoscope2;
		if ( docnum > 0 && sel2 != -1 && sel2 != -1 && sel1 != sel2 )
		{
			horoscope1 = directory->getHoroscope( std::min( sel1, docnum ));
			horoscope2 = directory->getHoroscope( std::min( sel2, docnum ));
		}
		
		props->setHoroscope( HoroscopeRole::Partner1, horoscope1 );
		props->setHoroscope( HoroscopeRole::Partner2, horoscope2 );

#ifdef DEBUG_PARTNERWIDGET
		debuglog << "PartnerWidget::updatePartnerChoices "
			<< " h1 " << horoscope1.get()
			<< " h2 " << horoscope2.get();
#endif
	}

	/*****************************************************
	**
	**   PartnerWidget   ---   OnHoroscopeListChanged
	**
	******************************************************/
	void OnHoroscopeListChanged( wxCommandEvent& )
	{
#ifdef DEBUG_PARTNERWIDGET
		debuglog << "PartnerWidget::OnHoroscopeListChanged";
#endif
		updatePartnerChoices();
		OnDataChanged();
	}

	/*****************************************************
	**
	**   PartnerWidget   ---   OnHoroscopeDataChanged
	**
	******************************************************/
	void OnHoroscopeDataChanged( wxCommandEvent& )
	{
#ifdef DEBUG_PARTNERWIDGET
		debuglog << "PartnerWidget::OnHoroscopeDataChanged";
#endif
		OnDataChanged();
	}

	wxChoice *choice_partner1 = nullptr;

	wxChoice *choice_partner2 = nullptr;
};

/**************************************************************
***
**    WidgetFactory   ---   createPartnerWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createPartnerWidget( wxWindow *parent )
{
	return new PartnerWidget( parent, props );
}

