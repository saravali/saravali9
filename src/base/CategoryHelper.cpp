/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/CategoryHelper.cpp
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

#include "CategoryHelper.h"

#include "ConfigListItems.h"
#include "logger.h"
#include "constants.h"
#include "enumserializer.h"
#include "gdiconfig.h"

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   WidgetSkin   ---   Constructor
**
******************************************************/
WidgetSkin::WidgetSkin()
{
	//pen = wxPen( gdiconfig->colors.fgColor, 1, wxPENSTYLE_SOLID );
	brush = MBrush( wxColour(), wxBRUSHSTYLE_TRANSPARENT );
	//brush = MBrush( gdiconfig->colors.bgColor, wxBRUSHSTYLE_SOLID );
}

WidgetCategory CategoryHelper::getGeomCategory( const WidgetCategory &cat )
{
	return getSuperiorCategory( cat );
}

WidgetCategory CategoryHelper::getSkinCategory( const WidgetCategory &cat )
{
	return getSuperiorCategory( cat );
}

/*****************************************************
**
**   CategoryHelper   ---   getSuperiorCategory
**
******************************************************/
WidgetCategory CategoryHelper::getSuperiorCategory( const WidgetCategory &cat )
{
	if ( IS_TEXT_WIDGET( cat ))
	{
		return WidgetCategory::TextGeneric;
	}
	else if ( IS_BARDIAGRAM_WIDGET( cat ))
	{
		return WidgetCategory::BardiagramGeneric;
	}
	else if ( cat >= WidgetCategory::VargaChart && cat <= WidgetCategory::LastChart )
	{
		return WidgetCategory::VargaChart;
	}
	return cat;
}

