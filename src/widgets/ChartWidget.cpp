/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/ChartWidget.cpp
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

#include "Commands.h"
#include "enumserializer.h"
#include "GdiWidgetWrapper.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "Varga.h"
#include "WidgetFactory.h"

#include <wx/sizer.h>

/*************************************************//**
*
* 
*
******************************************************/
class SingleChartWidget : public GdiWidgetWrapper
{
public:

	SingleChartWidget( wxWindow *parent, ContextProps_ptr props, const WidgetCategory &cat, Varga varga = Varga::Rasi )
		: GdiWidgetWrapper( parent, props, cat ),
		varga( varga )
	{
		debuglog << "Constructor SingleChartWidget cat " << cat << " varga " << varga;
		HybridChart *chart = nullptr;

		switch( cat )
		{
			case WidgetCategory::UniversalChart:
			case WidgetCategory::WesternChart:
			case WidgetCategory::VargaChart:
				chart = new HybridChart( props, category, varga );
			break;

			case WidgetCategory::SarvatobhadraChart:
				chart = new HybridChart( props, WidgetCategory::SarvatobhadraChart );
			break;

			default:
				assert( false );
			break;
		}
		assert( chart );
		setWidget( chart );
		setChartActions();

		OnDataChanged();
	}

  virtual wxString getTitle( const bool shortname = false ) override
	{
		debuglog << "SingleChartWidget::getTitle category " << category << " varga " << varga;
    if ( category == WidgetCategory::VargaChart )
    {
      return VargaExpert::getVargaLabel( varga, TextFormat::Long );
    }
		else return BasicWidget::getTitle( shortname );
  }

	Varga getVarga() const { return varga; }

protected:

	const Varga varga;
};

/*****************************************************
**
**   WidgetFactory   ---   createSingleChart
**
******************************************************/
BasicWidget *WidgetFactory::createSingleChart( wxWindow *parent, const WidgetCategory &cat, const Varga &varga )
{
	return new SingleChartWidget( parent, props, cat, varga );
}

