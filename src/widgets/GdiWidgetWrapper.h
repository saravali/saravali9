/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/GdiWidgetWrapper.h
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

#ifndef _GDIWIDGETWRAPPER_H_
#define _GDIWIDGETWRAPPER_H_

#include "BasicWidget.h"

class GdiWidget;
class WidgetContext;

/*************************************************//**
*
*  Container for a single GdiWidget from gdi layer.
*
* A widget must be added with the methods addWidget or setHybridChart.
*
* There is a switch doPaintBackground which can be used to paint the background
* automatically here or by the widget itself.
*
******************************************************/
class GdiWidgetWrapper : public BasicWidget
{
public:

	GdiWidgetWrapper( wxWindow*, GdiWidget*);

	GdiWidgetWrapper( wxWindow*, ContextProps_ptr, const WidgetCategory& );

	~GdiWidgetWrapper();

	void setWidget( GdiWidget* );

	GdiWidget *getWidget() { return widget; }

	void invalidateLayout() { layoutValid = false; }

	virtual WidgetCategory getWidgetCategory() const override;

	void clearWidget();

	void setPaintBackground( const bool b = true ) { doPaintBackground = b; }

	virtual void OnDataChanged() override;

protected:

	void init();

	void OnClose( wxCloseEvent& );

	void OnSize( wxSizeEvent& );

	virtual void doPaint( Painter*, const MRect &refreshRect ) override;

	virtual WidgetResponse handleMouseLeftClick( const MComplex&, const int &keymod ) override;

	virtual WidgetResponse handleLeftDoubleClick( const MComplex&, const int&keymod ) override;

	virtual WidgetResponse handleResponsiveChartEvent( const MComplex&, const int &keymod ) override;

	GdiWidget *widget = nullptr;

	bool doPaintBackground;

	bool layoutValid;
};

#endif // _GRIDWRAPPER_H_

