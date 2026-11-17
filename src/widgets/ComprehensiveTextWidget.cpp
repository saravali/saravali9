/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/ComprehensiveTextWidget.cpp
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

#include "CommonControls.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Lang.h"
#include "MDocument.h"
#include "TextHelper.h"
#include "TextWidget.h"
#include "ToolPanel.h"
#include "WidgetFactory.h"
#include "WindowDirectory.h"

#include <wx/choice.h>
#include <wx/sizer.h>

extern Gdiconfig *gdiconfig;

/*************************************************//**
*
*  view for various kinds of text output including toolbar controls
*
******************************************************/
class ComprehensiveTextWidget : public BasicWidget
{
public:
	ComprehensiveTextWidget( wxWindow *parent, ContextProps_ptr props )
		: BasicWidget( parent, props, WidgetCategory::ComprehensiveTextWidget, ToolbarWidgetId::ComprehensiveTextWidget )
	{
		category = gdiconfig->prefs.textWindowPage;

		twidget = new TextWidget( this, props, category );
		mainsizer->Add( twidget, 1, wxEXPAND );
		SetSizer( mainsizer );

		assert( toolbar );
		choice_mode = getToolbarControl<TextViewModeChoice>( ToolbarCmd::ChoiceTextMode );
		assert( choice_mode );
		choice_mode->setSelectedCategory( category );

		choice_varga = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceVarga );
		assert( choice_varga );
		choice_dasa = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceDasa );
		assert( choice_dasa );

		OnToolbarCommand();
		OnDataChanged();
	}

	/**************************************************************
	***
	**   ComprehensiveTextWidget   ---   Destructor
	***
	***************************************************************/
	~ComprehensiveTextWidget()
	{
		gdiconfig->prefs.textWindowPage = category;
	}

	/**************************************************************
	***
	**   ComprehensiveTextWidget   ---   getTitle
	***
	***************************************************************/
  virtual wxString getTitle( const bool /*shortname*/ = false ) override
	{
		debuglog << "ComprehensiveTextWidget::getTitle category " << category;
		return Lang().getWidgetCategoryName( category );
  }

	/*****************************************************
	**
	**   ComprehensiveTextWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		debuglog << "ComprehensiveTextWidget::OnDataChanged category " << category;
		MDocument_ptr doc = twidget->getDoc();
		assert( doc );

		doc->clear();
		TextHelper helper( props, doc );
		const int ret = helper.writeTextAnalysis( category, varga, dasa );

		static bool shown = false;
		if ( ret && ! shown )
		{
			shown = true;
			showMissingEphemDataMessage( this );
		}
		twidget->OnDataChanged();
		//BasicWidget::OnDataChanged();
	}

protected:

	/*****************************************************
	**
	**   ComprehensiveTextWidget   ---   OnToolbarCommand
	**
	******************************************************/
	void OnToolbarCommand() override
	{
		category = choice_mode->getSelectedCategory();
		twidget->setWidgetCategory( category );

		sendUpdateTitleEvent();

		varga = (Varga)choice_varga->GetSelection();
		choice_varga->Enable( category == WidgetCategory::TextAshtakavarga || category == WidgetCategory::TextJaimini );

		dasa = (DasaId)choice_dasa->GetSelection();
		choice_dasa->Enable( category == WidgetCategory::TextDasaShort
			|| category == WidgetCategory::TextDasaCompact
			|| category == WidgetCategory::TextDasaComprehensive
			|| category == WidgetCategory::TextKp );
		OnDataChanged();
	}

protected:

	TextWidget *twidget;

	TextViewModeChoice *choice_mode = nullptr;

	WidgetCategory category = WidgetCategory::None;

	DasaId dasa = DasaId::Vimsottari;

	Varga varga = Varga::Rasi;

	wxChoice *choice_varga = nullptr;

	wxChoice *choice_dasa = nullptr;
};

/**************************************************************
***
**   WidgetFactory   ---   createComprehensiveTextWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createComprehensiveTextWidget( wxWindow *parent )
{
	return new ComprehensiveTextWidget( parent, props );
}

