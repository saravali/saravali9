/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ChartPreviewPanel.h
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

#ifndef CHARTPREVIEWPANEL_H_
#define CHARTPREVIEWPANEL_H_

#include "bootstrap.h"
#include "constants.h"

#include <wx/panel.h>

class DirectionExpert;
class DivisionChoice;
class GdiWidgetWrapper;
class HybridChart;
struct UranianSection;
struct ChartConfig;
enum class WidgetCategory;

class wxCheckBox;
class wxChoice;
class wxCommandEvent;
class wxMouseEvent;
class wxStaticText;

/*************************************************//**
*
*   Generic preview of chart on the right side of GenericCharttConflet
*
******************************************************/
class ChartPreviewPanel: public wxPanel
{
public:

	ChartPreviewPanel( wxWindow* parent, ContextProps_ptr props,
		ChartConfig* chartConfig, const WidgetCategory &category );

	~ChartPreviewPanel();

	void OnMouseMotion( wxMouseEvent& );

	void reset();

	void OnDataChanged();

	void setChartConfig( ChartConfig* );
	
	HybridChart *getChart() { return chart; }

protected:

	void OnCheckBox( wxCommandEvent& );

	void recreateChart();

	void OnChoiceDivision( wxCommandEvent& );

	void OnMouseWheelEvent( wxMouseEvent& );

	void OnWidgetChanged( wxCommandEvent &event );

	void emitChangeEvent();

	void do_layout();

	ContextProps_ptr props;

	const WidgetCategory category = WidgetCategory::None;

	ChartConfig* cfg = nullptr;

	Horoscope_ptr horoscope;

	Horoscope_ptr hdirection;

	UranianSection *usection = nullptr;

	DirectionExpert *directionExpert = nullptr;

	HybridChart *chart = nullptr;

	GdiWidgetWrapper *cw = nullptr;

	wxCheckBox* check_secondchart = nullptr;

	DivisionChoice *choice_division = nullptr;
};

#endif // CHARTPREVIEWPANEL_H_

