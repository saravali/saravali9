/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/MatchesWidget.cpp
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
#include "Direction.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "MatchWriter.h"
#include "Sidebar.h"
#include "TextWidget.h"
#include "Uranian.h"
#include "UranianHelper.h"
#include "WidgetFactory.h"

#include <wx/sizer.h>

extern Astroconfig *astroconfig;

#define DEBUG_URANIAN

/*************************************************//**
*
* 
*
******************************************************/
class MatchesWidget : public BasicWidget
{
public:

	MatchesWidget( wxWindow *parent, ContextProps_ptr props )
		: BasicWidget( parent, props, WidgetCategory::MatchesWidget ),
		usetting( UranianHelper::get()->calculateUranianContext( props, category ))
	{
		usection = new UranianSection( astroconfig->getUranianSection( usetting ));
		expert = new UranianExpert( props, usection );

		if ( props->getDataContext() == DataContext::MultiDirection && usection->roleRestrictors.size() == 0 )
		{
			usection->roleRestrictors.push_back( RoleRestrictor( { HoroscopeRole::Transit, false } ));
			usection->roleRestrictors.push_back( RoleRestrictor( HoroscopeRole::Progressive, false ));
			usection->roleRestrictors.push_back( RoleRestrictor( HoroscopeRole::SolarArc, true ));
		}

    wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
		sidebar = SidebarPanelFactory().createSidebarPanel( this, props, usetting, usection );
    sizer->Add( sidebar, 0, wxEXPAND );

		text = new TextWidget( this, props, category );
    sizer->Add( text, 1, wxEXPAND );

		mainsizer->Add( sizer, 1, wxEXPAND, 3 );
		SetSizer( mainsizer );

		Connect( wxID_ANY, COMMAND_SIDEPANEL_CHANGED, wxCommandEventHandler( MatchesWidget::OnSidePanelChanged ));
		Connect( wxID_ANY, COMMAND_SIDEPANEL_REFRESH, wxCommandEventHandler( MatchesWidget::OnSidePanelRefresh ));
		Connect( wxID_ANY, COMMAND_SIDEPANEL_SORT, wxCommandEventHandler( MatchesWidget::OnSidePanelSort ));

		OnDataChanged();
	}

	/*****************************************************
	**
	**   MatchesWidget   ---   Destructor
	**
	******************************************************/
	~MatchesWidget()
	{
		astroconfig->getUranianSection( usetting ) = *usection;
		delete expert;
		delete usection;
	}

	/*****************************************************
	**
	**   MatchesWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
#ifdef DEBUG_URANIAN
		debuglog << "MatchesWidget::OnDataChanged DataContext " << props->getDataContext();
#endif

		assert( expert );
    expert->clear();

		switch( props->getDataContext())
		{
			case DataContext::Radix:
				expert->calculateRadix();
			break;
			case DataContext::Direction:
			{
				const auto roles = props->getActiveRoles();
				assert( roles.size() > 0 );
				if ( roles.size() > 1 )
				{
					assert( DirectionCalculator::get()->isDirectionRole( roles[1] ));
					expert->calculateDirections( props->getHoroscope( roles[1] )->getJD() );
				}
			}
			break;
			case DataContext::MultiDirection:
			{
				const auto roles = props->getActiveRoles();
				assert( roles.size() > 0 );
				if ( roles.size() > 1 )
				{
					for ( const auto &role : roles )
					{
						if ( role == HoroscopeRole::Radix ) continue;
						assert( roles[1] >= HoroscopeRole::FirstDirection && roles[1] <= HoroscopeRole::LastDirection );
					}
					expert->calculateMultiDirections( props->getHoroscope( roles[1] )->getJD(),
						usection->roleRestrictors );
				}
			}
			break;
			case DataContext::Solar:
				expert->calculateSolar();
			break;
			case DataContext::Partner:
				expert->calculatePartner();
			break;
			default:
				assert( false );
			break;
		}

		updateTextWidget();
	}

protected:

	/*****************************************************
	**
	**   MatchesWidget   ---   updateTextWidget
	**
	******************************************************/
	 void updateTextWidget()
	{
#ifdef DEBUG_URANIAN
		debuglog << "MatchesWidget::updateTextWidget";
#endif
		assert( text );

		text->clearDoc();
		MatchWriter writer( props, usection, Viewpoint::Uranian );
		writer.writeMatches( text->getDoc(), expert->getMatches() );
		text->OnDataChanged();
		Refresh();

		assert( sidebar );
		sidebar->updateUi();
	}

	/*****************************************************
	**
	**   MatchesWidget   ---   OnSidePanelChanged
	**
	******************************************************/
	void OnSidePanelChanged( wxCommandEvent& )
	{
#ifdef DEBUG_URANIAN
		debuglog << "MatchesWidget::OnSidePanelChanged";
#endif
		OnDataChanged();
	}

	/*****************************************************
	**
	**   MatchesWidget   ---   OnSidePanelRefresh
	**
	******************************************************/
	void OnSidePanelRefresh( wxCommandEvent& )
	{
#ifdef DEBUG_URANIAN
		debuglog << "MatchesWidget::OnSidePanelRefresh";
#endif
		updateTextWidget();
	}

	/*****************************************************
	**
	**   MatchesWidget   ---   OnSidePanelSort
	**
	******************************************************/
	void OnSidePanelSort( wxCommandEvent& )
	{
#ifdef DEBUG_URANIAN
		debuglog << "MatchesWidget::OnSidePanelSort";
#endif
		assert( expert );
		expert->sort();
		
		updateTextWidget();
	}

	SidebarPanel *sidebar = nullptr;

	const UranianContext usetting;

	UranianExpert *expert;

	UranianSection *usection;

	TextWidget *text;
};

/*************************************************//**
*
* 
*
******************************************************/
BasicWidget *WidgetFactory::createMatchesWidget( wxWindow *parent )
{
	return new MatchesWidget( parent, props );
}

