/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/BardiagramConflet.h
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

#ifndef BARDIAGRAMCONFLET_H
#define BARDIAGRAMCONFLET_H

#include "Conflet.h"
#include "gdiconfig.h"

class wxButton;
class wxCheckBox;
class wxChoice;
class wxSpinCtrl;

class AshtakavargaExpert;
class DasaBarPreview;
class GdiWidgetWrapper;

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
class BardiagramConflet : public ConfletIncarnation<BarDiagramConfig>
{
public:
	// begin wxGlade: BardiagramConflet::ids
	// end wxGlade

	BardiagramConflet( wxWindow* parent );

	~BardiagramConflet();

	virtual void updateUi() override;

private:

	void OnBarSpin( wxSpinEvent& ) { setDirty(); updateUi(); }
	void OnBarCheckBox( wxCommandEvent& ) { setDirty(); updateUi(); }
	void OnBarObjectChoice( wxCommandEvent& ) { updateUi(); }

	void OnButtonBarPen( wxCommandEvent& );
	void OnButtonBarBrush( wxCommandEvent& );
	void OnSize( wxSizeEvent& );
	void OnMouseMotion( wxMouseEvent& );

	const std::list<WidgetCategory> categories;
	ContextProps_ptr props;
	DasaBarPreview *dasaWidget;

	Horoscope_ptr horoscope;
  AshtakavargaExpert *expert;

protected:
	// begin wxGlade: BardiagramConflet::attributes
	wxChoice* choice_object;
	wxButton* button_pen;
	wxButton* button_brush;
	wxCheckBox* check_zoom;
	wxSpinCtrl* spin_zoom;
	wxSpinCtrl* spin_cornerradius;
	GdiWidgetWrapper* barwrapper;
	GdiWidgetWrapper* diagramwrapper;
	// end wxGlade
}; // wxGlade: end class


#endif // BARDIAGRAMCONFLET_H
