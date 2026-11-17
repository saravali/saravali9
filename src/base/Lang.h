/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Lang.h
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

#ifndef _LANG_H_
#define _LANG_H_

#include "bootstrap.h"
#include "constants.h"
#include "Planet.h"
#include "Viewpoint.h"

#include <wx/string.h>

struct PlanetPropertyConfig;
struct TextConfig;

/*************************************************//**
*
*  encapsulation of language specific names for astrological entities
*
******************************************************/
class Lang
{
public:

	wxString getWidgetCategoryName( const WidgetCategory&, const TextFormat = TextFormat::Long ) const;

	// get weekday name (0..6, sunday is 0)
	wxString getWeekdayName( const int& ) const;
	wxString getMonthName( const int &, const TextFormat = TextFormat::Long ) const;

	wxString getVargaName( const Varga& ) const;
	wxString getVargaSignification( const Varga& ) const;

	wxString getGanaName( const Gana& ) const;
	wxString getNadiName( const Nadi& ) const;
	wxString getVarnaName( const Varna& ) const;
	wxString getTaraName( const int& ) const;
	wxString getTithiName( const int& ) const;
	wxString getVashyaName( const Vashya& ) const;

	wxString getRajjuName( const int &aroha, const int &type ) const;

	wxString getBhavaName( const House& ) const;
	wxString getFriendshipName( const PlanetaryFriendship&, const TextFormat&, const bool &useVedicNames ) const;
	wxString getDignityName( const Dignity&, const TextFormat&, const bool &useVedicNames ) const;

	wxString getVimsopakaDignityName( const int &index, const int &dignity ) const;

	wxString getNakshatra27Name( const Nakshatra27&, const TextFormat = TextFormat::Long ) const;
	wxString getNakshatra28Name( const Nakshatra28&, const TextFormat = TextFormat::Long ) const;

	wxString getKarakaName( const CharaKaraka&, const TextFormat format = TextFormat::Long ) const;
	wxString getShastiamsaName( const int &i ) const;

	wxString getPlanetName( const PlanetId&, const Viewpoint&, const TextFormat = TextFormat::Long ) const;

	wxString getPlanetName( const PlanetId&, const PlanetConfig& ) const;

	wxString getObjectSignification( const PlanetId&, const TextFormat&, const bool vedic = false );

	wxString getSignName( const Sign&, const PlanetPropertyConfig& ) const;

	wxString getSignName( const Sign&, const Viewpoint&, const TextFormat = TextFormat::Long ) const;

	wxString getMovingDirectionName( const MovingDirection& ) const;

	wxString getElementName( const Element& ) const;
	wxString getSignQualityName( const SignQuality& ) const;
	wxString getChartSideName( const ChartSide& ) const;

	wxString getRoleContextSubscriptum( const HoroscopeRole& ) const;

	wxString getHoroscopeRoleName( const HoroscopeRole& );

	wxString getAyanamsaName( const int &ayanamsa );

private:
	wxString getNakshatraName( const int &nak, const bool &use28, const TextFormat = TextFormat::Long ) const;

	inline const static wxString c_none = "[none]";

	inline const static wxString c_error = "[error]";
};

#endif // _LANG_H_

