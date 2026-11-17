/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Yoga.h
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

#ifndef _YOGA_H_
#define _YOGA_H_

#include <set>

#include "ConfigListItems.h"
#include "constants.h"
#include "YdlExpression.h"

class Rule;
struct YogaConfig;

/*************************************************//**
*
*  \brief Singleton encapsulating calculation and text report of Yogas
*
******************************************************/
class YogaExpert : public Singleton<YogaExpert>
{
  friend class Singleton<YogaExpert>;

public:

	/**
	 * setup sources, groups and parse rules
	 */
	void init();

	/**
	 * 
	 */
	void updateAndWrite( MDocument_ptr, ContextProps_ptr, const Varga = Varga::Rasi,
		const Sign artificialLagna = Sign::None, const wxString groupFilter = "", const wxString sourceFilter = "" );

	/**
	 * parse a rule with a given string representation
	 */
	Rule *parseRule( wxString s );

	/**
	 * write evaluation result correctly to string, e.g. 0 -> House1
	 */
	wxString resultToString( const double&, const RuntimeTypeId& );

	/**
	 * get list of possible sources (for choice in YogaView)
	 */
	std::set<wxString> getSources() const { return sources; }

	/**
	 * get list of possible groups (for choice in YogaView)
	 */
	std::set<wxString> getGroups() const { return groups; }

	/**
	 *  cleanup groups, soources and Yoga definitions
	 */
	void clear();

private:

	YogaExpert() {}

	std::set<wxString> sources;
	std::set<wxString> groups;
	wxString groupFilter;

	int artificialLagna = -1;

	// TODO raus (unklar)
	bool initialized;

};

#endif


