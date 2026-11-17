/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/VargaDiagrams.cpp
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

#include "VargaDiagrams.h"

#include "ContextProperties.h"
#include "Dignity.h"
#include "Horoscope.h"

#include <variant>

/*****************************************************
**
**   VargaDiagram   ---   Constructor
**
******************************************************/
VargaDiagram::VargaDiagram( ContextProps_ptr props,
    const VargaWidgetType &type,
    const BarDiagramConfig *barconfig )
  : BarDiagram( props, "", barconfig ),
	type( type )
{
	switch( type )
	{
		case VargaWidgetType::ShadVarga:
			title = tr( "Shad Varga" );
		break;
		case VargaWidgetType::SaptaVarga:
			title = tr( "Sapta Varga" );
		break;
		case VargaWidgetType::DasaVarga:
			title = tr( "Dasa Varga" );
		break;
		case VargaWidgetType::ShodasaVarga:
			title = tr( "Shodasa Varga" );
		break;
		case VargaWidgetType::Average:
			title = tr( "Average" );
		break;
		default:
			fatallog << "invalid index for VargaDiagram " << (int)type;
			assert( false );
		break;
  }

	for( const PlanetId &p : PlanetHelper::createVedicOnlyPlanets())
  {
		addItem( p );
	}
}

/*****************************************************
**
**   VargaDiagram   ---   OnDataChanged
**
******************************************************/
void VargaDiagram::OnDataChanged()
{
	for( auto &item : items )
	{
		assert( std::holds_alternative<PlanetId>( item.object ));
		const VedicPlanetData &pdata
			= props->getRadixHoroscope()->getVedicPlanetData( std::get<PlanetId>( item.object ), getViewpoint());

		switch( type )
		{
			case VargaWidgetType::ShadVarga:
				item.value = pdata.vimsopakaBala[0];
			break;
			case VargaWidgetType::SaptaVarga:
				item.value = pdata.vimsopakaBala[1];
			break;
			case VargaWidgetType::DasaVarga:
				item.value = pdata.vimsopakaBala[2];
			break;
			case VargaWidgetType::ShodasaVarga:
				item.value = pdata.vimsopakaBala[3];
			break;
			case VargaWidgetType::Average:
				item.value = pdata.averageVimsopakaBala;
			break;
			default:
				fatallog << "invalid index for VargaDiagram " << (int)type;
				assert( false );
			break;
		}
	}
}

