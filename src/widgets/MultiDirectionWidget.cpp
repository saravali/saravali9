/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/MultiDirectionWidget.cpp
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
#include "ContextProperties.h"
#include "CommonControls.h"
#include "CombiWidget.h"
#include "ConfigurationDialog.h"
#include "DateFormatter.h"
#include "Direction.h"
#include "gdiconfig.h"
#include "MDocument.h"
#include "NotebookWidget.h"
#include "SpinCtrl.h"
#include "ToolPanel.h"
#include "WidgetFactory.h"

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define DEBUG_MULTIDIRECTIONWIDGET

/*************************************************//**
*
* Writes the transit report to the text widget
*
******************************************************/
class MultitransitCombiWidgetDecorator : public CombiWidgetDecorator
{
public:

	MultitransitCombiWidgetDecorator( DirectionExpert *expert ) :  expert( expert ){}

	virtual int writePlanetListDoc( MDocument_ptr doc ) override
	{
		debuglog << "MultitransitCombiWidgetDecorator::writePlanetListDoc";
		assert( doc );
		doc->clear();

		expert->writeMultiDirectionReport( doc );
		return 0;
	}

private:
	DirectionExpert *expert = nullptr;
};

/*************************************************//**
*
*  view container for transits
*
******************************************************/
class MultiDirectionWidget : public NotebookWidget
{
public:

	/*****************************************************
	**
	**   MultiDirectionWidget   ---   Constructor
	**
	******************************************************/
	MultiDirectionWidget( wxWindow *parent, ContextProps_ptr props )
		: NotebookWidget( parent, props, WidgetCategory::MultiDirectionWidget,
			gdiconfig->multipleView.extractItems( DataContext::MultiDirection ),
			ToolbarWidgetId::MultiDirectionWidget ),
		expert( props )
	{
		roles = astroconfig->directions.multiDirectionRoles;
		expert.setDirectionModes( roles );

		dateCtrl = getToolbarControl<DateInputCombiCtrl>( ToolbarCmd::DateSelectionCombiCtrl );
		assert( dateCtrl );

		for( auto *page : pages )
		{
			if ( page->getWidgetCategory() == WidgetCategory::PlanetListChartPair )
			{
				CombiWidget *textWidget = dynamic_cast<CombiWidget*>( page );
				assert( textWidget );
				textWidget->setDecorator( new MultitransitCombiWidgetDecorator( &expert ));
			}
		}

		dateCtrl->setIsLocaltime( gdiconfig->prefs.multitransitIsLocalTime );
		// must be done again after setIsLocaltime
		dateCtrl->setCurrentDate();
		OnDataChanged();

		Connect( (int)ToolbarCmd::ButtonConfigure, wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler( MultiDirectionWidget::OnConfiguration ));
	}

	/*****************************************************
	**
	**   MultiDirectionWidget   ---   Destructor
	**
	******************************************************/
	~MultiDirectionWidget()
	{
		gdiconfig->prefs.multitransitIsLocalTime = dateCtrl->getIsLocaltime();
	}

	/*****************************************************
	**
	**   MultiDirectionWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
#ifdef DEBUG_MULTIDIRECTIONWIDGET
		debuglog << "MultiDirectionWidget::OnDataChanged transit date "
			<< DateFormatter::get()->formatDatetime( dateCtrl->getJD(), dateCtrl->getIsLocaltime() );
#endif
		BasicWidget::OnDataChanged();

		props->isLocaltime = dateCtrl ? dateCtrl->getIsLocaltime() : false;

		// may have changed meanwhile
		if ( astroconfig->directions.multiDirectionRoles != roles )
		{
			roles = astroconfig->directions.multiDirectionRoles;
			expert.setDirectionModes( roles );
		}
		expert.calculateMultiDirections( dateCtrl->getJD());
		setDirty();
	}

protected:

	/*****************************************************
	**
	**   MultiDirectionWidget   ---   OnConfiguration
	**
	******************************************************/
	void OnConfiguration( wxCommandEvent& )
	{
		ConfigurationDialog( this, ConfletSet::Directions ).ShowModal();
  }

	DirectionExpert expert;

	std::list<HoroscopeRole> roles;

	DateInputCombiCtrl *dateCtrl = nullptr;
};


/**************************************************************
***
**   WidgetFactory   ---   createMultiDirectionWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createMultiDirectionWidget( wxWindow *parent )
{
	return new MultiDirectionWidget( parent, props );
}

