/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/BrushDialog.cpp
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

#include "BrushDialog.h"

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_PICKER_BGCOLOR = wxID_HIGHEST + 1, CD_SLIDER_ALPHA, CD_CHOICE_BGSTYLE, CD_CHOICE_BGIMAGE,
	CD_PREVIEW_PANEL, CD_SLIDER_HUE };

#include "ColorConfig.h"
#include "CommonControls.h"
#include "FileConfig.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "mathbase.h"
#include "Painter.h"

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/clrpicker.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/graphics.h>
#include <wx/image.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <iostream>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class BrushDialogPreviewWidget: public wxPanel
{
public:

	/*****************************************************
	**
	**   BrushDialogPreviewWidget   ---   Constructor
	**
	******************************************************/
	BrushDialogPreviewWidget( wxWindow* parent, wxWindowID id, MBrush &brush )
		: wxPanel( parent, id, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE ),
		brush( brush )
	{
		Connect( wxEVT_PAINT, wxPaintEventHandler( BrushDialogPreviewWidget::OnPaint ));
	}

private:

	/*****************************************************
	**
	**   BrushDialogPreviewWidget   ---   OnPaint
	**
	******************************************************/
	void OnPaint( wxPaintEvent& )
	{
		wxCoord h, w;
		GetSize( &w, &h );

		wxPaintDC dc( this );
		PrepareDC( dc );

    wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
    assert( gc );
    Painter *painter = PainterFactory().createGcPainter( gc );
    assert( painter );

		painter->setBrush( brush );
		painter->drawRectangle( MRect( 0, 0, w, h ));

		gc->Flush();
		delete painter;
    delete gc;
	}

	const MBrush &brush;

};

/*****************************************************
**
**   BrushDialog   ---   Constructor
**
******************************************************/
BrushDialog::BrushDialog( wxWindow* parent, const MBrush &b )
	: wxDialog(parent, -1, "--", wxDefaultPosition, wxDefaultSize, DEFAULT_DIALOG_STYLE),
	brush( b )
{
    // begin wxGlade: BrushDialog::BrushDialog
    SetTitle(_("Select Brush"));
    wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* sizer_bg = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Attributes")), wxVERTICAL);
    sizer_main->Add(sizer_bg, 1, wxALL|wxEXPAND, 3);
    wxFlexGridSizer* grid_bg = new wxFlexGridSizer(5, 2, 3, 3);
    sizer_bg->Add(grid_bg, 1, 0, 0);
    label_bgcolor = new wxStaticText(this, wxID_ANY, _("Color"));
    grid_bg->Add(label_bgcolor, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    button_bgcolor = new wxColourPickerCtrl(this, CD_PICKER_BGCOLOR);
    grid_bg->Add(button_bgcolor, 1, wxALL, 3);
    label_alpha = new wxStaticText(this, wxID_ANY, _("Alpha"));
    grid_bg->Add(label_alpha, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    slider_alpha = new wxSlider(this, CD_SLIDER_ALPHA, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS);
    grid_bg->Add(slider_alpha, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_bgstyle = new wxStaticText(this, wxID_ANY, _("Style"));
    grid_bg->Add(label_bgstyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    const wxString choice_bgstyle_choices[] = {
        _("dummy dummy"),
    };
    choice_bgstyle = new wxChoice(this, CD_CHOICE_BGSTYLE, wxDefaultPosition, wxDefaultSize, 1, choice_bgstyle_choices);
    choice_bgstyle->SetToolTip(_("Background images can be used with option 'Stipple'"));
    choice_bgstyle->SetSelection(0);
    grid_bg->Add(choice_bgstyle, 0, wxALL, 3);
    label_bgimage = new wxStaticText(this, wxID_ANY, _("Image"));
    grid_bg->Add(label_bgimage, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    choice_bgimage = new BackgroundImageChoice(this, CD_CHOICE_BGIMAGE);
    grid_bg->Add(choice_bgimage, 1, wxALL|wxEXPAND, 3);
    label_rotatehue = new wxStaticText(this, wxID_ANY, _("Rotate Hue"));
    grid_bg->Add(label_rotatehue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    slider_rotatehue = new wxSlider(this, CD_SLIDER_HUE, 0, 0, 360, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS);
    grid_bg->Add(slider_rotatehue, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_preview = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Brush Preview")), wxHORIZONTAL);
    sizer_bg->Add(sizer_preview, 1, wxALL|wxEXPAND, 3);
    panel_preview = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxTAB_TRAVERSAL);
    sizer_preview->Add(panel_preview, 1, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_previewpanel = new wxBoxSizer(wxHORIZONTAL);
    preview = new BrushDialogPreviewWidget(panel_preview, wxID_ANY, brush);
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
    grid_bg->AddGrowableCol(1);
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    Layout();
    // end wxGlade

	choice_bgstyle->Clear();
	{
		int i = 0;
		int sel = 0;
		for ( const auto &style : mapping_list_brushstyle )
		{
			//if ( style.first == wxPENSTYLE_INVALID ) continue;

			choice_bgstyle->Append( style.second );
			if ( style.first == brush.style ) sel = i;
			++i;
		}
		choice_bgstyle->SetSelection( sel );
	}

	slider_alpha->SetValue( brush.color.Alpha() );
	slider_rotatehue->SetValue( brush.rotateHue );
	choice_bgimage->SetStringSelection( brush.filename );

	updateUi();
	SetFocus();

	Connect( CD_PICKER_BGCOLOR, wxEVT_COMMAND_COLOURPICKER_CHANGED,
		wxColourPickerEventHandler( BrushDialog::OnColorPicker ));
	Connect( CD_SLIDER_ALPHA, wxEVT_SLIDER, wxCommandEventHandler( BrushDialog::OnSliderAlpha ));
	Connect( CD_CHOICE_BGSTYLE, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( BrushDialog::OnStyleChoice ));
	Connect( CD_CHOICE_BGIMAGE, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( BrushDialog::OnChoiceBgImage ));
	Connect( CD_SLIDER_HUE, wxEVT_SLIDER, wxCommandEventHandler( BrushDialog::OnSliderHue ));
	Connect( wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BrushDialog::OnOK ));
}

/*****************************************************
**
**   BrushDialog   ---   OnColorPicker
**
******************************************************/
void BrushDialog::OnColorPicker( wxColourPickerEvent &event )
{
	brush.color.Set(
		event.GetColour().Red(),
		event.GetColour().Green(),
		event.GetColour().Blue(),
		slider_alpha->GetValue()
	);
	updateUi();
}

/*****************************************************
**
**   BrushDialog   ---   OnSliderAlpha
**
******************************************************/
void BrushDialog::OnSliderAlpha( wxCommandEvent& )
{
	debuglog << "BrushDialog::OnSliderAlpha value " << slider_alpha->GetValue();
	brush.color.Set(
		brush.color.Red(),
		brush.color.Green(),
		brush.color.Blue(),
		slider_alpha->GetValue()
	);
	updateUi();
}

/*****************************************************
**
**   BrushDialog   ---   OnStyleChoice
**
******************************************************/
void BrushDialog::OnStyleChoice( wxCommandEvent& )
{
	debuglog << "BrushDialog::OnStyleChoice oldstyle ";
	const wxBrushStyle oldstyle = brush.style;

	brush.style = wxBRUSHSTYLE_INVALID;
	const wxString sel = choice_bgstyle->GetStringSelection();
	for ( const auto &m : mapping_list_brushstyle )
	{
		if ( m.second == sel )
		{
			brush.style = m.first;
			break;
		}
	}
	// set alpha to reasonable value
	if (( oldstyle == wxBRUSHSTYLE_STIPPLE || oldstyle == wxBRUSHSTYLE_TRANSPARENT )
		&& brush.color.Alpha() == 0 )
	{
		brush.color.Set(
			brush.color.Red(),
			brush.color.Green(),
			brush.color.Blue(),
			255
		);
	}
	// avoid emtpy selection for bgimage
	if ( brush.style == wxBRUSHSTYLE_STIPPLE && choice_bgimage->GetSelection() == -1 )
	{
		choice_bgimage->SetSelection( 0 );
	}
	if ( brush.style == wxBRUSHSTYLE_STIPPLE && choice_bgimage->GetSelection() != -1 )
	{
		brush.filename = choice_bgimage->GetStringSelection();
	}
	updateUi();
}

/*****************************************************
**
**   BrushDialog   ---   OnChoiceBgImage
**
******************************************************/
void BrushDialog::OnChoiceBgImage( wxCommandEvent& )
{
	brush.filename = choice_bgimage->GetStringSelection();
	updateUi();
}

/*****************************************************
**
**   BrushDialog   ---   OnSliderHue
**
******************************************************/
void BrushDialog::OnSliderHue( wxCommandEvent&  )
{
	brush.rotateHue = slider_rotatehue->GetValue();
	updateUi();
}

/*****************************************************
**
**   BrushDialog   ---   updateUi
**
******************************************************/
void BrushDialog::updateUi()
{
	const bool useColor = ( brush.style != wxBRUSHSTYLE_TRANSPARENT && brush.style != wxBRUSHSTYLE_STIPPLE );
	button_bgcolor->SetColour( useColor ? brush.color : wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ));

	label_bgcolor->Enable( useColor );
	button_bgcolor->Enable( useColor );
	label_alpha->Enable( useColor );
	slider_alpha->Enable( useColor );

	const bool isStipple = ( brush.style == wxBRUSHSTYLE_STIPPLE );
	label_bgimage->Enable( isStipple );
	choice_bgimage->Enable( isStipple );
	label_rotatehue->Enable( isStipple );
	slider_rotatehue->Enable( isStipple );

	preview->Refresh();
}

/*****************************************************
**
**   BrushDialog   ---   OnOK
**
******************************************************/
void BrushDialog::OnOK( wxCommandEvent& )
{
	if( brush.style == wxBRUSHSTYLE_STIPPLE )
	{
		brush.color = *wxWHITE;
	}
	else
	{
		brush.filename.clear();
		brush.rotateHue = 0;
	}
	EndModal( wxID_OK );
}

