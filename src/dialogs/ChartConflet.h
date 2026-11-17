/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ChartConflet.h
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

#ifndef CHARTCONFLET_H
#define CHARTCONFLET_H

#include "Conflet.h"
#include "astroconfig.h"

struct ChartConfig;
class ChartPreviewPanel;
class wxButton;
class wxCheckBox;
class wxChoice;
class wxSpinCtrl;
class wxSlider;
class wxStaticText;
enum class ConfletId;

// begin wxGlade: ::dependencies
#include <wx/spinctrl.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
*   
*
******************************************************/
class ChartOptionsPanel : public wxPanel
{
public:
	ChartOptionsPanel( wxWindow *parent, ContextProps_ptr props, ChartConfig *cfg )
	: wxPanel( parent ),
		props( props ),
		cfg( cfg )
	{
	}

	virtual void updateUi() {}

protected:

	ContextProps_ptr props;

public:
	ChartConfig *cfg;
};

/*************************************************//**
*
* 
*
******************************************************/
class ChartConflet : public Conflet
{
public:
	// begin wxGlade: ChartConflet::ids
	// end wxGlade

	ChartConflet( wxWindow* parent, const ConfletId& );

	~ChartConflet();

	virtual void config2model() override;

	virtual void model2config() override;

	virtual void updateUi() override;

	virtual void restoreDefaults() override;

private:

protected:

	const ConfletId confletId;

	void setupGeomsAndSkins();

	void OnCommand( wxCommandEvent& );

	void OnChoiceGeometry( wxCommandEvent& );

	void OnMouseMotion( wxMouseEvent& );

	ContextProps_ptr props;

	WidgetCategory category;

	ChartConfig *cfg = nullptr;

	ChartOptionsPanel *optionsPanel = nullptr;

	std::vector<std::pair<uint,wxString>> geomModel;

	// begin wxGlade: ChartConflet::attributes
	wxChoice* choice_geometry;
	wxChoice* choice_skin;
	wxChoice* choice_planetstyle;
	wxSlider* slider_fontsize;
	wxChoice* choice_retrostyle;
	wxCheckBox* check_highlight;
	wxCheckBox* check_zoom;
	wxSpinCtrl* spin_zoomfactor;
	wxStaticText* label_percent;
	ChartPreviewPanel* preview;
	// end wxGlade
}; // wxGlade: end class


#endif // CHARTCONFLET_H
