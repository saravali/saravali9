/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/GridWrapper.cpp
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

#include "GridWrapper.h"

#include "ContextProperties.h"
#include "logger.h"
#include "mathbase.h"
#include "Painter.h"
#include "GdiWidgetGrid.h"

/*****************************************************
**
**   GridWrapper   ---   Constructor
**
******************************************************/
GridWrapper::GridWrapper( wxWindow *parent,
		ContextProps_ptr props,
		const WidgetCategory &category,
		const WidgetGridFormat &format,
    const int columnCount )
	: GdiWidgetWrapper( parent, props, category )
{
	GdiWidgetGrid *grid = new GdiWidgetGrid( props, category, format, columnCount );
	setWidget( grid );
}

/**************************************************************
***
**   GridWrapper   ---   addWidget
***
***************************************************************/
void GridWrapper::addWidget( GdiWidget *w )
{
	GdiWidgetGrid *grid = dynamic_cast<GdiWidgetGrid*>( widget );
	assert( grid );
	grid->addWidget( w );
}

/**************************************************************
***
**   GridWrapper   ---   clearWidgets
***
***************************************************************/
void GridWrapper::clearWidgets()
{
	GdiWidgetGrid *grid = dynamic_cast<GdiWidgetGrid*>( widget );
	assert( grid );
	grid->clearWidgets();
}

