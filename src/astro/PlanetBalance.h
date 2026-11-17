/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/PlanetBalance.h
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

#ifndef _PLANETBALANCE_H_
#define _PLANETBALANCE_H_

#include "Expert.h"
#include "mstring.h"
#include "Viewpoint.h"

#include <map>
#include <variant>
#include <wx/string.h>

/*************************************************//**
*
* 
*
******************************************************/
class PlanetBalanceExpert : public Expert
{
public:

	PlanetBalanceExpert( ContextProps_ptr props, const PlanetConfig &cfg )
		: Expert( props, Viewpoint::None ),
		planetCfg( cfg )
	{
	}

	void clear();

	void update();

	std::pair<wxString, MString> getItemValue( const std::variant<Element,SignQuality,ChartSide>& );

	void write( MDocument_ptr, const bool show_header = false );

private:

	const PlanetConfig &planetCfg;

	MString createPlanetList( const Planets& );

	std::map<Element, Planets> element_map;

	std::map<SignQuality, Planets> quality_map;

	std::map<ChartSide, Planets> chartside_map;
};


#endif // _PLANETBALANCE_H_

