/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/NotebookWidget.h
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

#ifndef _NOTEBOOKWIDGET_H_
#define _NOTEBOOKWIDGET_H_

#include "BasicWidget.h"
#include "constants.h"

#include <vector>

class wxBookCtrlEvent;
class wxIdleEvent;
class wxBookCtrlBase;

/*************************************************//**
*
*  This view is a container for one or more widgets.
*  - a single widget is directly inserted as child
*  - two or more widgets are managed by a notebook.
* 
*  This class is used for several composite (universal) Radix views,
*  for Transit and Partner View.
* 
*  Relevant context data is in the ContextProperties object:
* - DataContext, i.e. Radix, Transit, Partner, Solar, AV
* - 1st Horoscope: the Radix or 1st partner
* - 2nd Horoscope: auxiliary or 2nd partner
* - more are possible, e.g. in Multitransit
*
*  Special handling for Radix if a Uranian chart is included:
*  this view creates a clone of the 1st Horoscope and updates it
*  if necessary (this is used for manual offset use cases).
*
******************************************************/
class NotebookWidget : public BasicWidget
{
public:

	NotebookWidget( wxWindow *parent, ContextProps_ptr, const WidgetCategory&,
		const std::vector<WidgetCategory> &elements, const ToolbarWidgetId& );

	~NotebookWidget();

	virtual void postCreate() override;

	virtual void OnDataChanged() override;

protected:
	
	void OnNotebookEvent( wxBookCtrlEvent& );

	void OnIdle( wxIdleEvent& );

	void updatePage( const int& );

	void setDirty( const bool = true );

	const std::vector<WidgetCategory> elements;

	Horoscope_ptr h2;

	wxBookCtrlBase *notebook = nullptr;

	std::vector<BasicWidget*> pages;

	std::vector<bool> dirty;
};

#endif // _NOTEBOOKWIDGET_H_

