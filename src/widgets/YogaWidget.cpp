/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/YogaWidget.cpp
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
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Jaimini.h"
#include "constants.h"
#include "TextWidget.h"
#include "ToolPanel.h"
#include "WidgetFactory.h"
#include "Varga.h"
#include "Yoga.h"

#include <wx/choice.h>

extern Gdiconfig *gdiconfig;

/*************************************************//**
*
*  view for Yogas including text view and toolbar controls
*
******************************************************/
class YogaWidget : public TextWidget
{
public:

	/*****************************************************
	**
	**   YogaWidget   ---   Constructor
	**
	******************************************************/
	YogaWidget( wxWindow *parent, ContextProps_ptr props )
		: TextWidget( parent, props, WidgetCategory::TextYoga, ToolbarWidgetId::YogaWidget )
	{
		assert( props->getRadixHoroscope());
		YogaExpert *expert = YogaExpert::get();
		expert->init();

		choiceVarga = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceVarga );
		assert( choiceVarga );
		choiceVarga->SetSelection( 0 );

		choiceGroupFilter = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceYogaGroupFilter );
		assert( choiceGroupFilter );
		choiceGroupFilter->Clear();
		choiceGroupFilter->Append( tr( "All Groups" ));
		for ( const auto &group : expert->getGroups()) choiceGroupFilter->Append( group );
		choiceGroupFilter->SetSelection( gdiconfig->prefs.yogaGroupFilter );

		choiceSourceFilter = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceYogaSourceFilter );
		assert( choiceSourceFilter );
		choiceSourceFilter->Clear();
		choiceSourceFilter->Append( tr( "All Sources" ));
		for ( const auto &source : expert->getSources()) choiceSourceFilter->Append( source );
		choiceSourceFilter->SetSelection( gdiconfig->prefs.yogaSourceFilter );

		choiceFirstHouse = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceYogaFirstHouse );
		assert( choiceFirstHouse );
		choiceFirstHouse->SetSelection( 0 );

		OnDataChanged();
	}

	/*****************************************************
	**
	**   YogaWidget   ---   Destructor
	**
	******************************************************/
	~YogaWidget()
	{
		gdiconfig->prefs.yogaGroupFilter = choiceGroupFilter->GetSelection();
		gdiconfig->prefs.yogaSourceFilter = choiceSourceFilter->GetSelection();
	}

	/*****************************************************
	**
	**   YogaWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		Sign lagna;
		wxString sourceFilter, groupFilter;
		const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::Vedic );
		Horoscope_ptr horoscope = props->getRadixHoroscope();

		if ( choiceGroupFilter->GetSelection() ) groupFilter = choiceGroupFilter->GetStringSelection();
		if ( choiceSourceFilter->GetSelection() ) sourceFilter = choiceSourceFilter->GetStringSelection();

		const Varga varga = (Varga)choiceVarga->GetSelection();

		const int firsthouse = choiceFirstHouse->GetSelection();

		switch ( firsthouse )
		{
			case 0:
				lagna = Sign::None;
			break;
			case 1:
			{
				const double longitude = horoscope->getLongitude( PlanetId::Moon, viewpoint );
				lagna = VargaExpert( horoscope.get(), viewpoint ).calcVarga( longitude, varga );
			}
			break;
			case 2:
				lagna = (Sign)JaiminiExpert( props, varga ).calcPada( 0 );
			break;
			default:
				lagna = (Sign)(firsthouse - 3);
				assert_valid_enum<Sign>( lagna );
			break;
		}
		debuglog << "YogaWidget::OnDataChanged varga " << varga << " first house " << firsthouse
			<< " lagna " << lagna << " group filter " << groupFilter << " source filter " << sourceFilter;

		clearDoc();
		YogaExpert::get()->updateAndWrite( getDoc(), props, varga, lagna, groupFilter, sourceFilter );
		TextWidget::OnDataChanged();
	}

protected:

	wxChoice *choiceVarga = nullptr;

	wxChoice *choiceGroupFilter = nullptr;

	wxChoice *choiceSourceFilter = nullptr;

	wxChoice *choiceFirstHouse = nullptr;
};

/*************************************************//**
*
*    WidgetFactory   ---   createYogaWidget
*
******************************************************/
BasicWidget *WidgetFactory::createYogaWidget( wxWindow *parent )
{
	return new YogaWidget( parent, props );
}

