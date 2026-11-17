/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/PpWidget.cpp
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
#include "GridWrapper.h"
#include "GdiWidgetGrid.h"
#include "PlanetaryPicture.h"
#include "PpTree.h"
#include "Sidebar.h"
#include "UranianHelper.h"
#include "WidgetFactory.h"

#include <wx/sizer.h>

extern Astroconfig *astroconfig;

/*************************************************//**
*
* 
*
******************************************************/
class PpWidget : public BasicWidget
{
public:

	PpWidget( wxWindow *parent, ContextProps_ptr props )
		: BasicWidget( parent, props, WidgetCategory::PpWidget ),
		usetting( UranianHelper::get()->calculateUranianContext( props, category ))
	{
		usection = new UranianSection( astroconfig->getUranianSection( usetting ));

		leftpanel = SidebarPanelFactory().createSidebarPanel( this, props, usetting, usection );
		ppWrapper = new GridWrapper( this, props, WidgetCategory::PpTree, WidgetGridFormat::FloatFixedColWidth, 1 );

    wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
    sizer->Add( leftpanel, 0, wxEXPAND );
    sizer->Add( ppWrapper, 1, wxEXPAND );

		mainsizer->Add( sizer, 1, wxEXPAND, 3 );
		SetSizer( mainsizer );

		Connect( wxID_ANY, COMMAND_SIDEPANEL_CHANGED, wxCommandEventHandler( PpWidget::OnSidePanel ));
		Connect( wxID_ANY, COMMAND_SIDEPANEL_REFRESH, wxCommandEventHandler( PpWidget::OnSidePanel ));
		Connect( wxID_ANY, COMMAND_SIDEPANEL_SORT, wxCommandEventHandler( PpWidget::OnSidePanel ));

		OnDataChanged();
	}

	/*****************************************************
	**
	**   PpWidget   ---   Destructor
	**
	******************************************************/
	~PpWidget()
	{
		astroconfig->getUranianSection( usetting ) = *usection;
		delete usection;
	}

	/*****************************************************
	**
	**   PpWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		debuglog << "PpWidget::OnDataChanged";

		ppWrapper->clearWidgets();
		if ( props->getActiveRoles().size() ) // maybe nullptr in case of PartnerView
		{
			PlanetaryPictureExpert expert( props, usection );

			// expert will use h1 if h2 is null
			if ( ! expert.createPictures())
			{
				doNotShowAgainMessage( this, DoNotShowAgainTopic::UranianMatchesExceeded,
				std::format( "Maximum number of matches ({}) exceeded, results are incomplete. You can increase it in Configuration/Calculation/Uranian",
					astroconfig->uranian.maxMatchCount ), true );
			}

			for( const auto &c : expert.getPictures())
			{
				debuglog << "PpWidget addWidget";
				ppWrapper->addWidget( new PpTree( ppWrapper->getProps(), c, usection ));
			}
		}
		ppWrapper->OnDataChanged();
	}

protected:

	/*****************************************************
	**
	**   PpWidget   ---   OnSidePanel
	**
	******************************************************/
	void OnSidePanel( wxCommandEvent& )
	{
		debuglog << "PpWidget::OnSidePanel";
		OnDataChanged();
	}

	const UranianContext usetting;

	UranianSection *usection = nullptr;

	SidebarPanel *leftpanel = nullptr;

	GridWrapper *ppWrapper = nullptr;
};

/*************************************************//**
*
*    WidgetFactory   ---   createPpWidget
*
******************************************************/
BasicWidget *WidgetFactory::createPpWidget( wxWindow *parent )
{
	return new PpWidget( parent, props );
}

