/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PenDialog.cpp
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

#include "PenDialog.h"

#include "ColorConfig.h"
#include "CommonControls.h"
#include "FileConfig.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "mathbase.h"
#include "Painter.h"

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/colour.h>
#include <wx/clrpicker.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/graphics.h>
#include <wx/panel.h>
#include <wx/pen.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/stattext.h>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_PICKER_COLOR = wxID_HIGHEST + 1, CD_SLIDER_ALPHA, CD_SPIN_WIDTH, CD_CHOICE_STYLE,
	CD_CHOICE_BGIMAGE, CD_CHOICE_CAP,
	CD_CHOICE_JOIN, CD_PREVIEW_PANEL
};

/*****************************************************
**
**   PenPreviewWidget   ---   Constructor
**
******************************************************/
PenPreviewWidget::PenPreviewWidget( wxWindow* parent, wxWindowID id, wxPen *pen, const MBrush &b )
	: wxPanel( parent, id, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE ),
	pen( pen ),
	brush( b )
{
	Connect( wxEVT_PAINT, wxPaintEventHandler( PenPreviewWidget::OnPaint ));
}

/*****************************************************
**
**   PenPreviewWidget   ---   OnPaint
**
******************************************************/
void PenPreviewWidget::OnPaint( wxPaintEvent& )
{
	wxCoord h, w;
	wxPaintDC dc( this );
	PrepareDC( dc );
	const int border = 10;

	GetSize( &w, &h );
	wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	assert( gc );
	Painter *painter = PainterFactory().createGcPainter( gc );
	assert( painter );

	// background
	painter->setPenColor( gdiconfig->colors.bgColor );
	painter->setBrush( brush );
	painter->drawRectangle( 0, 0, w, h );

	painter->setPen( *pen );
	painter->drawRectangle( MRect( border, border, w - 2 * border, h - 2 * border ));
	painter->drawArrow( MComplex( border, h/2 ), MComplex( w - border, h/2 ));

	gc->Flush();
	delete painter;
	delete gc;
}

/*****************************************************
**
**   PenDialog   ---   Constructor
**
******************************************************/
PenDialog::PenDialog( wxWindow* parent, const wxPen &p, const MBrush &brush )
	: wxDialog(parent, -1, "--", wxDefaultPosition, wxDefaultSize, DEFAULT_DIALOG_STYLE),
	pen( p ),
	bgbrush( brush )
{
    // begin wxGlade: PenDialog::PenDialog
    SetTitle(_("Select Pen"));
    wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* sizer_attributes = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Attributes")), wxVERTICAL);
    sizer_main->Add(sizer_attributes, 1, wxALL|wxEXPAND, 3);
    wxFlexGridSizer* grid_attributes = new wxFlexGridSizer(4, 2, 3, 3);
    sizer_attributes->Add(grid_attributes, 0, 0, 0);
    label_color = new wxStaticText(this, wxID_ANY, _("Color"));
    grid_attributes->Add(label_color, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    button_color = new wxColourPickerCtrl(this, CD_PICKER_COLOR);
    grid_attributes->Add(button_color, 1, wxALL|wxEXPAND, 3);
    label_alpha = new wxStaticText(this, wxID_ANY, _("Alpha"));
    grid_attributes->Add(label_alpha, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    slider_alpha = new wxSlider(this, CD_SLIDER_ALPHA, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS);
    grid_attributes->Add(slider_alpha, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_width = new wxStaticText(this, wxID_ANY, _("Width"));
    grid_attributes->Add(label_width, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    spin_width = new wxSpinCtrl(this, CD_SPIN_WIDTH, wxT("1"), wxDefaultPosition, wxDefaultSize, 0, 1, 30);
    grid_attributes->Add(spin_width, 0, wxALL|wxEXPAND, 3);
    label_bgstyle = new wxStaticText(this, wxID_ANY, _("Style"));
    grid_attributes->Add(label_bgstyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    const wxString choice_style_choices[] = {
        _("dummy dummy"),
    };
    choice_style = new wxChoice(this, CD_CHOICE_STYLE, wxDefaultPosition, wxDefaultSize, 1, choice_style_choices);
    choice_style->SetToolTip(_("Background images can be used with option 'Stipple'"));
    choice_style->SetSelection(0);
    grid_attributes->Add(choice_style, 0, wxALL, 3);
    wxStaticBoxSizer* sizer_preview = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Pen Preview")), wxHORIZONTAL);
    sizer_attributes->Add(sizer_preview, 1, wxALL|wxEXPAND, 3);
    panel_preview = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxTAB_TRAVERSAL);
    sizer_preview->Add(panel_preview, 1, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_previewpanel = new wxBoxSizer(wxHORIZONTAL);
    preview = new PenPreviewWidget(panel_preview, wxID_ANY, &pen, bgbrush);
    preview->SetMinSize(wxSize(-1, 100));
    sizer_previewpanel->Add(preview, 1, wxALL|wxEXPAND, 3);
    static_line = new wxStaticLine(this, wxID_ANY);
    sizer_main->Add(static_line, 0, wxEXPAND, 0);
    wxBoxSizer* sizer_bottom = new wxBoxSizer(wxHORIZONTAL);
    sizer_main->Add(sizer_bottom, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
    button_ok = new wxButton(this, wxID_OK, _("OK"));
    button_ok->SetDefault();
    sizer_bottom->Add(button_ok, 0, wxALL, 3);
    button_cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
    sizer_bottom->Add(button_cancel, 0, wxALL, 3);
    
    panel_preview->SetSizer(sizer_previewpanel);
    grid_attributes->AddGrowableCol(1);
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    Layout();
    // end wxGlade

	button_color->SetColour( pen.GetColour() );
	slider_alpha->SetValue( pen.GetColour().Alpha() );
	spin_width->SetValue( pen.GetWidth() );

	choice_style->Clear();
	int i = 0;
	int sel = 0;
	for ( const auto &style : mapping_list_penstyle )
	{
		choice_style->Append( style.second );
		if ( style.first == pen.GetStyle()) sel = i;
		++i;
	}
	choice_style->SetSelection( sel );

	//choice_bgimage->SetStringSelection( pen.GetStipple() );
	updateUi();

	Connect( CD_PICKER_COLOR, wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( PenDialog::OnColorPicker ));
	Connect( CD_SLIDER_ALPHA, wxEVT_SCROLL_THUMBTRACK, wxCommandEventHandler( PenDialog::OnChoice ));
	Connect( CD_SPIN_WIDTH, wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( PenDialog::OnSpin ));
	Connect( CD_CHOICE_BGIMAGE, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PenDialog::OnChoice ));
	Connect( wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PenDialog::OnChoice ));
}

/*****************************************************
**
**   PenDialog   ---   getSelectedPenStyle
**
******************************************************/
wxPenStyle PenDialog::getSelectedPenStyle() const
{
	const wxString sel = choice_style->GetStringSelection();
	for ( const auto &m : mapping_list_penstyle )
	{
		if ( m.second == sel )
		{
			return m.first;
		}
	}
	return wxPENSTYLE_INVALID;
}

/*****************************************************
**
**   PenDialog   ---   updateUi
**
******************************************************/
void PenDialog::updateUi()
{
	debuglog << "PenDialog::updateUi slider " << slider_alpha->GetValue();

	wxColour color = button_color->GetColour();
	pen.SetColour( wxColour( color.Red(), color.Green(), color.Blue(), slider_alpha->GetValue()));

	button_color->SetColour( pen.GetColour() );
	pen.SetStyle( getSelectedPenStyle());
	pen.SetWidth( spin_width->GetValue());

	preview->Refresh();
}

