/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ChartConfig.h
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

#ifndef _CHARTCONFIG_H_
#define _CHARTCONFIG_H_

#include "constants.h"

/*************************************************//**
*
* 
*
******************************************************/
struct ChartConfig : public Jsonable
{
	// impl in ChartGeometry.cpp 
	ChartConfig( const WidgetCategory = WidgetCategory::None );

	// json:start
	TextFormat textFormat = TextFormat::Long;
	RetroDisplayStyle retroStyle = RetroDisplayStyle::Symbol;
	int relativeFontSize = 100;

	bool highlightOnMouseOver = false;
	bool zoomOnMouseOver = true;
	int zoomFactor = 102;

	bool showPlanetColors = false;

	// Western
	bool showHouses = true;
	bool showSignColors = true;
	bool showHouseColors = true;
	WesternChartOrientation chartOrientation = WesternChartOrientation::Ascendant;
	HouseNumberFormat houseNumberStyle = HouseNumberFormat::Arabic;

	// Uranian
	UranianChartOrigin uranianChartOrigin = UranianChartOrigin::Right;

	bool showMatchesOnMouseOver = true;

	int division = 1;

	int chartCenterType = 3;

	// varga
	bool showArudhas = false;
	bool showAshtakavarga = false;

	NorthIndianChartType northIndianSignDisplayType = NorthIndianChartType::Number;
	int southIndianAscendantMarkup = 0;
	VedicChartCenterType centerInfoType = VedicChartCenterType::Signification;

	// Graha Drishti
	VedicAspectId aspectType = VedicAspectId::GrahaDrishti;
	VedicAspectStrength grahaDrishtiMinimumStrength = VedicAspectStrength::Full;
	int beneficMaleficDisplay = 0;
	bool includeAsMc = false;

	// sbc
	bool showSbcDevanagariCharacters = true;
	bool showSbcNakshatraQuality = true;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

#endif // _CHARTCONFIG_H_

