/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/MultipleView.h
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
#ifndef _MULTIPLEVIEW_H_
#define _MULTIPLEVIEW_H_

#include "bootstrap.h"
#include "constants.h"
#include "gdiconfig.h"
#include "Viewpoint.h"

#include <list>
#include <vector>
#include <wx/string.h>

/*************************************************//**
*
*
*
******************************************************/
struct CompositeViewDefinition
{
	CompositeViewDefinition()
	{
		viewpoint = Viewpoint::None;
	}

	CompositeViewDefinition(
		const wxString name,
		const wxString description,
		const Viewpoint &vp,
		const std::vector<WidgetCategory> elems )
		: name( name ),
		description( description ),
		viewpoint( vp ),
		elements( elems )
	{
	}

	wxString name;

	wxString description;

	Viewpoint viewpoint;

	std::vector<WidgetCategory> elements;
};

/*************************************************//**
*
*
*
******************************************************/
class MultipleViewHandler : public Singleton<MultipleViewHandler>
{
  friend class Singleton<MultipleViewHandler>;

public:

	std::list<WidgetCategory> getDefaultMultipleViewItems( const DataContext& );

	std::list<WidgetCategory> getAvailableMultipleViewItems( const DataContext& );

	std::vector<std::pair<WidgetCategory, bool>> createPairs( const DataContext& );
	
	const std::vector<CompositeViewDefinition> &getBuiltinDefs() { return builtinDefs; }

	const CompositeViewDefinition &getBuiltinDef( const uint& ) const;

private:

	MultipleViewHandler();

	std::vector<CompositeViewDefinition> builtinDefs;

};

#endif // _MULTIPLEVIEW_H_


