/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/BasicColorConflet.cpp
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

#include "BasicColorConflet.h"
#include "logger.h"
#include "PenDialog.h"
#include "Validator.h"

#include <wx/clrpicker.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum
{
	CD_DEFAULT_PEN = wxID_HIGHEST + 1
};

/*****************************************************
**
**   BasicColorConflet   ---   Constructor
**
******************************************************/
BasicColorConflet::BasicColorConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->colors )
{
	previewBgBrush = MBrush( cfg.bgColor );

	// begin wxGlade: BasicColorConflet::BasicColorConflet
	wxFlexGridSizer* grid_general = new wxFlexGridSizer(1, 2, 0, 0);
	wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
	grid_general->Add(sizer_main, 1, wxALL, 3);
	wxStaticBoxSizer* sizer_foreground = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Foreground")), wxVERTICAL);
	sizer_main->Add(sizer_foreground, 0, wxALL|wxEXPAND, 3);
	wxGridSizer* grid_fgcolors = new wxGridSizer(5, 2, 3, 3);
	sizer_foreground->Add(grid_fgcolors, 1, 0, 3);
	button_default_pen = new wxButton(sizer_foreground->GetStaticBox(), CD_DEFAULT_PEN, _("Pen ..."));
	grid_fgcolors->Add(button_default_pen, 0, wxALL, 3);
	pen_preview = new PenPreviewWidget(sizer_foreground->GetStaticBox(), wxID_ANY, &cfg.defaultPen, previewBgBrush);
	grid_fgcolors->Add(pen_preview, 1, wxALL|wxEXPAND, 3);
	label_text_fg = new wxStaticText(sizer_foreground->GetStaticBox(), wxID_ANY, _("Text Color"));
	grid_fgcolors->Add(label_text_fg, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_text_fg = new wxColourPickerCtrl(sizer_foreground->GetStaticBox(), wxID_ANY);
	grid_fgcolors->Add(button_text_fg, 1, wxALL|wxEXPAND, 3);
	label_benefic_fg = new wxStaticText(sizer_foreground->GetStaticBox(), wxID_ANY, _("Benefics"));
	grid_fgcolors->Add(label_benefic_fg, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_benefic_fg = new wxColourPickerCtrl(sizer_foreground->GetStaticBox(), wxID_ANY);
	grid_fgcolors->Add(button_benefic_fg, 1, wxALL|wxEXPAND, 3);
	label_malefic_fg = new wxStaticText(sizer_foreground->GetStaticBox(), wxID_ANY, _("Malefics"));
	grid_fgcolors->Add(label_malefic_fg, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_malefic_fg = new wxColourPickerCtrl(sizer_foreground->GetStaticBox(), wxID_ANY);
	grid_fgcolors->Add(button_malefic_fg, 1, wxALL|wxEXPAND, 3);
	label_transit_fg = new wxStaticText(sizer_foreground->GetStaticBox(), wxID_ANY, _("Transit Planets"));
	grid_fgcolors->Add(label_transit_fg, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_transit_fg = new wxColourPickerCtrl(sizer_foreground->GetStaticBox(), wxID_ANY);
	grid_fgcolors->Add(button_transit_fg, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_bgcolors = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Background Colors")), wxVERTICAL);
	grid_general->Add(sizer_bgcolors, 0, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* grid_bgcolors = new wxFlexGridSizer(4, 2, 3, 3);
	sizer_bgcolors->Add(grid_bgcolors, 1, wxALL, 3);
	label_bg = new wxStaticText(sizer_bgcolors->GetStaticBox(), wxID_ANY, _("Default Background"));
	grid_bgcolors->Add(label_bg, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_bg = new wxColourPickerCtrl(sizer_bgcolors->GetStaticBox(), wxID_ANY);
	grid_bgcolors->Add(button_bg, 1, wxALL|wxEXPAND, 3);
	label_marked_text_bg = new wxStaticText(sizer_bgcolors->GetStaticBox(), wxID_ANY, _("Marked Text Background"));
	grid_bgcolors->Add(label_marked_text_bg, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_marked_text_bg = new wxColourPickerCtrl(sizer_bgcolors->GetStaticBox(), wxID_ANY);
	grid_bgcolors->Add(button_marked_text_bg, 1, wxALL|wxEXPAND, 3);
	label_chart_markup_bg = new wxStaticText(sizer_bgcolors->GetStaticBox(), wxID_ANY, _("Marked Fields in Charts"));
	grid_bgcolors->Add(label_chart_markup_bg, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_chart_markup_bg = new wxColourPickerCtrl(sizer_bgcolors->GetStaticBox(), wxID_ANY);
	grid_bgcolors->Add(button_chart_markup_bg, 1, wxALL|wxEXPAND, 3);
	label_error_bg = new wxStaticText(sizer_bgcolors->GetStaticBox(), wxID_ANY, _("Errors"));
	grid_bgcolors->Add(label_error_bg, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_error_bg = new wxColourPickerCtrl(sizer_bgcolors->GetStaticBox(), wxID_ANY);
	grid_bgcolors->Add(button_error_bg, 1, wxALL|wxEXPAND, 3);
	
	grid_bgcolors->AddGrowableCol(1);
	SetSizer(grid_general);
	grid_general->Fit(this);
	// end wxGlade

	button_bg->SetValidator( ColorPickerValidator( &cfg.bgColor ));
	button_text_fg->SetValidator( ColorPickerValidator( &cfg.textFgColor ));
	button_marked_text_bg->SetValidator( ColorPickerValidator( &cfg.markedFieldsBgColor ));
	button_benefic_fg->SetValidator( ColorPickerValidator( &cfg.beneficFgColor ));
	button_malefic_fg->SetValidator( ColorPickerValidator( &cfg.maleficFgColor ));
	button_transit_fg->SetValidator( ColorPickerValidator( &cfg.transitFgColor ));
	button_chart_markup_bg->SetValidator( ColorPickerValidator( &cfg.chartMarkupBgColor ));
	button_error_bg->SetValidator( ColorPickerValidator( &cfg.errorBgColor ));

	Connect( CD_DEFAULT_PEN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BasicColorConflet::OnDefaultPen ));
}

/*****************************************************
**
**   BasicColorConflet   ---   updateUi
**
******************************************************/
void BasicColorConflet::updateUi()
{
	debuglog << "BasicColorConflet::updateUi";
	pen_preview->Refresh();
}

/*****************************************************
**
**   BasicColorConflet   ---   OnDefaultPen
***
******************************************************/
void BasicColorConflet::OnDefaultPen( wxCommandEvent& )
{
	debuglog << "OnDefaultPen";
	PenDialog dialog( this, cfg.defaultPen, previewBgBrush );
	if ( dialog.ShowModal() == wxID_OK )
	{
		cfg.defaultPen = dialog.getPen();
		guiHasChanged();
	}
	updateUi();
}

