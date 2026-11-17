/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/HoraWidget.cpp
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
#include "Commands.h"
#include "ContextProperties.h"
#include "DateFormatter.h"
#include "datelocation.h"
#include "gdiconfig.h"
#include "Hora.h"
#include "TextWidget.h"
#include "ToolPanel.h"
#include "SpinCtrl.h"
#include "WidgetFactory.h"

#include <wx/choice.h>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define DEBUG_HORAWIDGET

/*************************************************//**
*
*  widget containing text output for Hora
*
******************************************************/
class HoraWidget : public TextWidget
{
public:

	/*****************************************************
	**
	**   HoraWidget   ---   Constructor
	**
	******************************************************/
	HoraWidget( wxWindow *parent, ContextProps_ptr props )
		: TextWidget( parent, props, WidgetCategory::TextHora, ToolbarWidgetId::HoraWidget )
	{
		expert = new HoraExpert();

		spin_date = getToolbarControl<DateSpin>( ToolbarCmd::InputDate );
		assert( spin_date );
		spin_date->changeValuePointer( &jd );

		props->isLocaltime = gdiconfig->prefs.horaTimezone;
#ifdef DEBUG_HORAWIDGET
		debuglog << "HoraWidget::Constructor isLocaltime " << props->isLocaltime;
#endif

		choice_tz = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceTimezone );
		assert( choice_tz );
		choice_tz->SetSelection( (int)props->isLocaltime );

		setCurrentDate();

		Connect( (int)CommandId::Now, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( HoraWidget::OnNow ));
	}

	/*****************************************************
	**
	**   HoraWidget   ---   Destructor
	**
	******************************************************/
	~HoraWidget()
	{
#ifdef DEBUG_HORAWIDGET
		debuglog << "HoraWidget::Destructor isLocaltime " << props->isLocaltime;
#endif
		gdiconfig->prefs.horaTimezone = props->isLocaltime;
		delete expert;
	}

	/*****************************************************
	**
	**   HoraWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
#ifdef DEBUG_HORAWIDGET
		debuglog << "HoraWidget::OnDataChanged jd " << DateFormatter::get()->formatUTDatetime( jd )
			<< " localtime " << props->isLocaltime;
#endif

		expert->update( jd + 1 );
		clearDoc();
		expert->write( getDoc(), props->isLocaltime );

		TextWidget::OnDataChanged();
	}

protected:

	/*****************************************************
	**
	**   HoraWidget   ---   OnToolbarCommand
	**
	******************************************************/
	virtual void OnToolbarCommand() override
	{
#ifdef DEBUG_HORAWIDGET
		debuglog << "HoraWidget::OnToolbarCommand isLocaltime " << props->isLocaltime;
#endif
		if ( choice_tz )
		{
			props->isLocaltime = choice_tz->GetSelection();
		}
		TransferDataFromWindow();
		OnDataChanged();
	}

	/*****************************************************
	**
	**   HoraWidget   ---   OnNow
	**
	******************************************************/
	void setCurrentDate()
	{
		double currentJD = MDate::getCurrentJD();
		if ( props->isLocaltime )
		{
			currentJD += ( astroconfig->standardLocation.tz + astroconfig->standardLocation.dst ) / 24.0;
		}

		jd = floor( currentJD - .5 ) + .5;

#ifdef DEBUG_HORAWIDGET
		debuglog << "HoraWidget::OnNow current jd "
			<< DateFormatter::get()->formatUTDatetime( currentJD )
			<< " JD for expert: " << DateFormatter::get()->formatUTDatetime( jd )
			<< " localtime " << props->isLocaltime;
#endif
		if ( toolbar ) toolbar->TransferDataToWindow();
		OnDataChanged();
	}

	void OnNow( wxCommandEvent& ) { setCurrentDate(); }

	double jd = 0.0;

	wxChoice *choice_tz = nullptr;

	DateSpin *spin_date = nullptr;

	HoraExpert *expert = nullptr;
};

/**************************************************************
***
**   WidgetFactory   ---   createHoraWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createHoraWidget( wxWindow *parent )
{
	return new HoraWidget( parent, props );
}

