/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/CombiWidget.h
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

#ifndef _COMBIWIDGET_H_
#define _COMBIWIDGET_H_

#include "BasicWidget.h"

class GdiWidgetWrapper;
class TextWidget;

class wxSplitterWindow;

/*************************************************//**
*
*  Decorator responsible for writing text contents in multiple contexts (e.g. radix, transit, solar).
*
******************************************************/
class CombiWidgetDecorator
{
public:

	virtual ~CombiWidgetDecorator() {}

	virtual int writePlanetListDoc( MDocument_ptr ) = 0;
};

/*************************************************//**
*
*  This is a container with a text view and a widget wrapper (generally used as graphical chart)
*
*  The text view is used for objects lists in several contexts.
*  It needs a CombiWidgetDecorator which is responsible to write the text contents.
*
* Actual implementations are:
* - Solar view with chart and solar decorator
* - Transit view with chart and transit decorator
* - Partner view with chart and synastry decorator
* - Partner view with chart and composite decorator
* - Western radix with chart
* - Vedic radix with double chart (D-1 and D-9) and Dasa tree (triple)
*
******************************************************/
class CombiWidget : public BasicWidget
{
public:

	CombiWidget( wxWindow *parent, ContextProps_ptr props, const WidgetCategory &category );

	~CombiWidget();

	virtual void postCreate() override;

	void setDecorator( CombiWidgetDecorator* );

	virtual void OnDataChanged() override;

protected:

	void recreateChart();

	const wxString configkey;

	Viewpoint oldvp;

	CombiWidgetDecorator *decorator = nullptr;

	TextWidget *twidget;

	GdiWidgetWrapper *wrapper = nullptr;

	BasicWidget *dwidget = nullptr;

	wxSplitterWindow *splitter = nullptr;
};

#endif // _COMBIWIDGET_H_

