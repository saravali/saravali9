/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/MultipleView.cpp
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

#include "MultipleView.h"

#include "constants.h"

/*****************************************************
**
**   MultipleViewHandler   ---   Constructor
**
******************************************************/
MultipleViewHandler::MultipleViewHandler()
	: builtinDefs(
	{
		{
			"Western Simple",
			"Western planets and chart",
			Viewpoint::Western,
			{
				WidgetCategory::PlanetListChartPair
			}
		},
		{
			"Vedic Simple",
			"Vedic planets and Rasi chart",
			Viewpoint::Vedic,
			{
				WidgetCategory::PlanetListChartPair
			}
		},
		{
			"Vedic with Dasas",
			"Vedic planets, Rasi and Navamsa chart and tree view of Dasas",
			Viewpoint::Vedic,
			{
				WidgetCategory::VedicTriple
			}
		},
		{
			"Vedic Comprehensive",
			"Vedic planets, Rasi and Navamsa chart and tree view of Dasas. Extra pages with Vargas and Sbc",
			Viewpoint::Vedic,
			{
				WidgetCategory::VedicTriple,
				WidgetCategory::Varga1Grid,
				WidgetCategory::Varga2Grid,
				WidgetCategory::Varga3Grid,
				WidgetCategory::SarvatobhadraChart,
				WidgetCategory::AvWidget,
				WidgetCategory::SolarWidget,
				WidgetCategory::YogaWidget,
				WidgetCategory::ShadbalaWidget,
			}
		},
		{
			"Uranian Default",
			"Uranian planet and western chart. Matches. Planetay pictures and Uranian chart",
			Viewpoint::Uranian,
			{
				WidgetCategory::PlanetListChartPair,
				WidgetCategory::MatchesWidget,
				WidgetCategory::SpWidget,
				WidgetCategory::PpWidget,
				WidgetCategory::UranianChartWidget
			}
		},
	} )
{
	// empty
}

/*****************************************************
**
**   MultipleViewHandler   ---   getBuiltinDef
**
******************************************************/
const CompositeViewDefinition &MultipleViewHandler::getBuiltinDef( const uint &i ) const
{
	assert( i < builtinDefs.size());
	return builtinDefs[i];
}

/*****************************************************
**
**   MultipleViewHandler   ---   getDefaultMultipleViewItems
**
******************************************************/
std::list<WidgetCategory> MultipleViewHandler::getDefaultMultipleViewItems( const DataContext &ctx )
{
	// TOD?
	return getAvailableMultipleViewItems( ctx );
}

/*****************************************************
**
**   MultipleViewHandler   ---   createPairs
**
******************************************************/
std::vector<std::pair<WidgetCategory, bool>> MultipleViewHandler::createPairs( const DataContext &ctx )
{
	// TODO works only if all are active. use getDefaultItems... 
	std::vector<std::pair<WidgetCategory, bool>> v;
	for( const WidgetCategory &cat : getAvailableMultipleViewItems( ctx ))
	{
		v.push_back( std::make_pair( cat, true ));
	}
	return v;
}

/*****************************************************
**
**   MultipleViewHandler   ---   getAvailableMultipleViewItems
**
******************************************************/
std::list<WidgetCategory> MultipleViewHandler::getAvailableMultipleViewItems( const DataContext &type )
{
	switch( type )
	{
		case DataContext::Partner:
			return
			{
				WidgetCategory::SynastryPair,
				WidgetCategory::CompositePair,
				WidgetCategory::UranianChartWidget,
				WidgetCategory::Varga1Grid,
				WidgetCategory::Varga2Grid,
				WidgetCategory::Varga3Grid,
				WidgetCategory::SarvatobhadraChart,
				WidgetCategory::TextAshtakoota,
				WidgetCategory::MatchesWidget,
				WidgetCategory::PpWidget,
			};
		break;
		case DataContext::Radix:
			return
			{
				WidgetCategory::PlanetListChartPair,
				WidgetCategory::UranianChartWidget,
				WidgetCategory::VedicTriple,
				WidgetCategory::Varga1Grid,
				WidgetCategory::Varga2Grid,
				WidgetCategory::Varga3Grid,
				WidgetCategory::SarvatobhadraChart,
				WidgetCategory::ComprehensiveTextWidget,
				WidgetCategory::DasaTree,
				WidgetCategory::DasaChart,
				WidgetCategory::AvWidget,
				WidgetCategory::YogaWidget,
				WidgetCategory::ShadbalaWidget,
				WidgetCategory::MatchesWidget,
				WidgetCategory::SpWidget,
				WidgetCategory::PpWidget,
			};
		break;

		case DataContext::MultiDirection:
			return
			{
				WidgetCategory::PlanetListChartPair,
				WidgetCategory::UranianChartWidget,
				WidgetCategory::MatchesWidget,
			};
		break;

		case DataContext::Solar:
		case DataContext::Direction:
		default:
			return
			{
				WidgetCategory::PlanetListChartPair,
				WidgetCategory::UranianChartWidget,
				WidgetCategory::Varga1Grid,
				WidgetCategory::Varga2Grid,
				WidgetCategory::Varga3Grid,
				WidgetCategory::SarvatobhadraChart,
				WidgetCategory::MatchesWidget,
				WidgetCategory::PpWidget,
				WidgetCategory::SpWidget,
			};
		break;
	}
}


