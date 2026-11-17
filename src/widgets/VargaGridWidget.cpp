/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/VargaGridWidget.cpp
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
#include "GridWrapper.h"
#include "HybridChart.h"
#include "logger.h"
#include "GdiWidgetGrid.h"
#include "Varga.h"
#include "WidgetFactory.h"

/*************************************************//**
*
* 
*
******************************************************/
class VargaGridWidget : public GridWrapper
{
public:

	VargaGridWidget( wxWindow *parent, ContextProps_ptr props, const WidgetCategory &category )
		:  GridWrapper( parent, props, category, WidgetGridFormat::FixedColsScaled, 3 )
	{
		debuglog << "VargaGridWidget create for category " << category;
		VargaGroup group;
		switch( category )
		{
			case WidgetCategory::Varga1Grid:
				group = VargaGroup::Shadvarga;
			break;
			case WidgetCategory::Varga2Grid:
        group = VargaGroup::Varga2;
			break;
			case WidgetCategory::Varga3Grid:
        group = VargaGroup::Varga3;
			break;
      break;
      default:
        assert( false );
      break;
		}

		for( const Varga varga : VargaExpert::getVargaIdList( group ))
		{
			addWidget( new HybridChart( props, WidgetCategory::VargaChart, varga ));
		}
		setChartActions();
		OnDataChanged();
	}

	VargaGridWidget( wxWindow *parent, ContextProps_ptr props, const WidgetCategory &category,
		const std::list<Varga> vargas, const int &cols )
		:  GridWrapper( parent, props, category, WidgetGridFormat::FixedColsScaled, cols )
	{
		debuglog << "VargaGridWidget create for category " << category;
		for( const Varga varga : vargas )
		{
			addWidget( new HybridChart( props, WidgetCategory::VargaChart, varga ));
		}
		setChartActions();
		OnDataChanged();
	}

};

/*****************************************************
**
**   WidgetFactory   ---   createVargaGrid
**
******************************************************/
BasicWidget *WidgetFactory::createVargaGrid( wxWindow *parent, const WidgetCategory &category )
{
	return new VargaGridWidget( parent, props, category );
}

/*****************************************************
**
**   WidgetFactory   ---   createRasiNavamsaGrid
**
******************************************************/
BasicWidget *WidgetFactory::createRasiNavamsaGrid( wxWindow *parent )
{
	return new VargaGridWidget( parent, props, WidgetCategory::RasiNavamsaGrid, { Varga::Rasi, Varga::Navamsa }, 1 );
}

