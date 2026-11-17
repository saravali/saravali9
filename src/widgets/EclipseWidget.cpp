/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/EclipseWidget.cpp
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
#include "datelocation.h"
#include "Eclipse.h"
#include "gdiconfig.h"
#include "mathbase.h"
#include "TextWidget.h"
#include "ToolPanel.h"
#include "WidgetFactory.h"

#include <wx/choice.h>
#include <wx/spinctrl.h>

extern Gdiconfig *gdiconfig;

/*************************************************//**
*
*  view containing text output for eclipses
*
******************************************************/
class EclipseWidget : public TextWidget
{
public:

	/*****************************************************
	**
	**   EclipseWidget   ---   Constructor
	**
	******************************************************/
	EclipseWidget( wxWindow *parent, ContextProps_ptr props )
		: TextWidget( parent, props, WidgetCategory::EclipseWidget, ToolbarWidgetId::EclipseWidget )
	{
		spinYear = getToolbarControl<wxSpinCtrl>( ToolbarCmd::SpinYear );
		assert( spinYear );
		spinYear->SetValue( getCurrentYear() );

		spinYearCount = getToolbarControl<wxSpinCtrl>( ToolbarCmd::SpinYearCount );
		assert( spinYearCount );
		spinYearCount->SetValue( 3 );

		choiceType = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceEclipseType );
		assert( choiceType );
		choiceType->SetSelection( 0 );

		choiceTimezone = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceTimezone );
		assert( choiceTimezone );
		choiceTimezone->SetSelection( gdiconfig->prefs.eclipseTimezone );

		OnDataChanged();
	}

	/*****************************************************
	**
	**   EclipseWidget   ---   Destructor
	**
	******************************************************/
	~EclipseWidget()
	{
		gdiconfig->prefs.eclipseTimezone = choiceTimezone->GetSelection();
	}

	/*****************************************************
	**
	**   EclipseWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		MDate d;
		d.set( spinYear->GetValue(), 1, 1, 0 );
		const double startjd = d.jd;
		d.set( spinYear->GetValue() + spinYearCount->GetValue(), 1, 1, 0 );
		const double endjd = d.jd;

		if ( choiceTimezone )
		{
			props->isLocaltime = (bool)choiceTimezone->GetSelection();
		}

		clearDoc();

		{
			//const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::None );
			EclipseExpert expert( props );

			expert.update( startjd, endjd, choiceType->GetSelection(), 0 );
			expert.write( getDoc() );
		}

		TextWidget::OnDataChanged();
	}

protected:

	wxSpinCtrl *spinYear = nullptr;

	wxSpinCtrl *spinYearCount = nullptr;

	wxChoice *choiceType = nullptr;

	wxChoice *choiceTimezone = nullptr;
};

/**************************************************************
***
**   WidgetFactory   ---   createEclipseWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createEclipseWidget( wxWindow *parent )
{
	return new EclipseWidget( parent, props );
}

