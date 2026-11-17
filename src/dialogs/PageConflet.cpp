/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PageConflet.cpp
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

#include "PageConflet.h"
#include "CommonControls.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "logger.h"
#include "TextValidator.h"

#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>
#include <wx/valtext.h>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

/*****************************************************
**
**   PageConflet   ---   Constructor
**
******************************************************/
PageConflet::PageConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->page )
{
	// begin wxGlade: PageConflet::PageConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBoxSizer* sizer_left = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Page Setup")), wxVERTICAL);
	sizer_main->Add(sizer_left, 0, wxALL|wxEXPAND, 3);
	wxBoxSizer* sizer_paperformat = new wxBoxSizer(wxHORIZONTAL);
	sizer_left->Add(sizer_paperformat, 0, wxALL|wxEXPAND, 3);
	label_papersize = new wxStaticText(sizer_left->GetStaticBox(), wxID_ANY, _("Paper Size"));
	sizer_paperformat->Add(label_papersize, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	const wxString choice_papersize_choices[] = {
		_("dummy"),
	};
	choice_papersize = new wxChoice(sizer_left->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 1, choice_papersize_choices);
	sizer_paperformat->Add(choice_papersize, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_margins = new wxStaticBoxSizer(new wxStaticBox(sizer_left->GetStaticBox(), wxID_ANY, _("Page Margins")), wxHORIZONTAL);
	sizer_left->Add(sizer_margins, 0, wxEXPAND, 0);
	wxFlexGridSizer* grid_margins = new wxFlexGridSizer(2, 3, 0, 0);
	sizer_margins->Add(grid_margins, 1, wxALL|wxEXPAND, 3);
	wxStaticText* label_margin_horizontal = new wxStaticText(sizer_margins->GetStaticBox(), wxID_ANY, _("Left/Right"));
	grid_margins->Add(label_margin_horizontal, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	spin_margin_horizontal = new wxSpinCtrl(sizer_margins->GetStaticBox(), wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);
	grid_margins->Add(spin_margin_horizontal, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxStaticText* label_margin_horizontal_pt = new wxStaticText(sizer_margins->GetStaticBox(), wxID_ANY, _("pt"));
	grid_margins->Add(label_margin_horizontal_pt, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxStaticText* label_margin_vertical = new wxStaticText(sizer_margins->GetStaticBox(), wxID_ANY, _("Top/Bottom"));
	grid_margins->Add(label_margin_vertical, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	spin_margin_vertical = new wxSpinCtrl(sizer_margins->GetStaticBox(), wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);
	grid_margins->Add(spin_margin_vertical, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxStaticText* label_margin_vertical_pt = new wxStaticText(sizer_margins->GetStaticBox(), wxID_ANY, _("pt"));
	grid_margins->Add(label_margin_vertical_pt, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	sizer_fontoptions = new wxStaticBoxSizer(new wxStaticBox(sizer_left->GetStaticBox(), wxID_ANY, _("Font Options")), wxVERTICAL);
	sizer_left->Add(sizer_fontoptions, 0, wxEXPAND, 0);
	wxBoxSizer* sizer_font2 = new wxBoxSizer(wxHORIZONTAL);
	sizer_fontoptions->Add(sizer_font2, 1, 0, 0);
	wxStaticText* label_fontsize = new wxStaticText(sizer_fontoptions->GetStaticBox(), wxID_ANY, _("Basic Font Size"));
	sizer_font2->Add(label_fontsize, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	spin_fontsize = new wxSpinCtrl(sizer_fontoptions->GetStaticBox(), wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 5, 20);
	sizer_font2->Add(spin_fontsize, 0, wxALL, 3);
	wxStaticText* label_fontpt = new wxStaticText(sizer_fontoptions->GetStaticBox(), wxID_ANY, _("pt"));
	sizer_font2->Add(label_fontpt, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxStaticBoxSizer* sizer_right = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxEmptyString), wxVERTICAL);
	sizer_main->Add(sizer_right, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_custom_header = new wxStaticBoxSizer(new wxStaticBox(sizer_right->GetStaticBox(), wxID_ANY, _("Custom Header")), wxVERTICAL);
	sizer_right->Add(sizer_custom_header, 0, wxALL|wxEXPAND, 3);
	wxBoxSizer* sizer_header_with_button = new wxBoxSizer(wxHORIZONTAL);
	sizer_custom_header->Add(sizer_header_with_button, 0, wxEXPAND, 0);
	check_header_showtext = new wxCheckBox(sizer_custom_header->GetStaticBox(), wxID_ANY, _("Show Custom Header Text"));
	sizer_header_with_button->Add(check_header_showtext, 1, wxALL|wxEXPAND, 3);
	button_help = new HelpButton(sizer_custom_header->GetStaticBox(), "Agenda of Variables");
	sizer_header_with_button->Add(button_help, 0, wxALL|wxEXPAND, 3);
	text_header = new wxTextCtrl(sizer_custom_header->GetStaticBox(), wxID_ANY, wxEmptyString);
	sizer_custom_header->Add(text_header, 0, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* sizer_header_orientation = new wxFlexGridSizer(1, 2, 0, 0);
	sizer_custom_header->Add(sizer_header_orientation, 0, wxALL|wxEXPAND, 3);
	label_header_orientation = new wxStaticText(sizer_custom_header->GetStaticBox(), wxID_ANY, _("Text Orientation"));
	sizer_header_orientation->Add(label_header_orientation, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_header_orientation_choices[] = {
		_("Left"),
		_("Center"),
		_("Right"),
	};
	choice_header_orientation = new wxChoice(sizer_custom_header->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_header_orientation_choices);
	choice_header_orientation->SetSelection(0);
	sizer_header_orientation->Add(choice_header_orientation, 1, wxALIGN_RIGHT|wxALL, 3);
	check_header_line = new wxCheckBox(sizer_custom_header->GetStaticBox(), wxID_ANY, _("Draw Line"));
	sizer_custom_header->Add(check_header_line, 0, wxALL, 3);
	check_header_firstpage = new wxCheckBox(sizer_custom_header->GetStaticBox(), wxID_ANY, _("Include First Page"));
	sizer_custom_header->Add(check_header_firstpage, 0, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_curstom_footer = new wxStaticBoxSizer(new wxStaticBox(sizer_right->GetStaticBox(), wxID_ANY, _("Custom Footer")), wxVERTICAL);
	sizer_right->Add(sizer_curstom_footer, 0, wxALL|wxEXPAND, 3);
	check_footer_showtext = new wxCheckBox(sizer_curstom_footer->GetStaticBox(), wxID_ANY, _("Show Custom Footer Text"));
	sizer_curstom_footer->Add(check_footer_showtext, 1, wxALL|wxEXPAND, 3);
	text_footer = new wxTextCtrl(sizer_curstom_footer->GetStaticBox(), wxID_ANY, wxEmptyString);
	sizer_curstom_footer->Add(text_footer, 0, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* sizer_footer_orientation = new wxFlexGridSizer(1, 2, 0, 0);
	sizer_curstom_footer->Add(sizer_footer_orientation, 0, wxALL|wxEXPAND, 3);
	label_footer_orientation = new wxStaticText(sizer_curstom_footer->GetStaticBox(), wxID_ANY, _("Text Orientation"));
	sizer_footer_orientation->Add(label_footer_orientation, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_footer_orientation_choices[] = {
		_("Left"),
		_("Center"),
		_("Right"),
	};
	choice_footer_orientation = new wxChoice(sizer_curstom_footer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_footer_orientation_choices);
	choice_footer_orientation->SetSelection(0);
	sizer_footer_orientation->Add(choice_footer_orientation, 1, wxALIGN_RIGHT|wxALL, 3);
	check_footer_line = new wxCheckBox(sizer_curstom_footer->GetStaticBox(), wxID_ANY, _("Draw Line"));
	sizer_curstom_footer->Add(check_footer_line, 0, wxALL, 3);
	check_footer_firstpage = new wxCheckBox(sizer_curstom_footer->GetStaticBox(), wxID_ANY, _("Include First Page"));
	sizer_curstom_footer->Add(check_footer_firstpage, 0, wxALL|wxEXPAND, 3);
	
	sizer_footer_orientation->AddGrowableCol(1);
	sizer_header_orientation->AddGrowableCol(1);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	// left side
	spin_margin_horizontal->SetValidator( wxGenericValidator( &cfg.horizontalMargin ));
	spin_margin_vertical->SetValidator( wxGenericValidator( &cfg.verticalMargin ));
	spin_fontsize->SetValidator( wxGenericValidator( &cfg.basicFontSize ));

	// right side
	choice_papersize->Clear();
	for( const PaperSize ps : Enum<PaperSize>())
	{
		choice_papersize->Append( enum_to_string( ps ));
	}
	choice_papersize->SetValidator( wxGenericValidator( (int*)&cfg.paperSize ));

	// header and footer
	check_header_line->SetValidator( wxGenericValidator( &cfg.header.showSeparator ));
	check_header_showtext->SetValidator( wxGenericValidator( &cfg.header.enabled ));
	text_header->SetValidator( StringValidator( &cfg.header.text ));
	choice_header_orientation->SetValidator( wxGenericValidator( &cfg.header.orientation ));
	check_header_firstpage->SetValidator( wxGenericValidator( &cfg.header.showOnFirstPage ));

	check_footer_line->SetValidator( wxGenericValidator( &cfg.footer.showSeparator ));
	check_footer_showtext->SetValidator( wxGenericValidator( &cfg.footer.enabled ));
	text_footer->SetValidator( StringValidator( &cfg.footer.text ));
	choice_footer_orientation->SetValidator( wxGenericValidator( &cfg.footer.orientation ));
	check_footer_firstpage->SetValidator( wxGenericValidator( &cfg.footer.showOnFirstPage ));

	button_help->contents = "$name: name of the native\n"
		 "$date: birth date of the native\n"
		 "$fulldate: birth date and time\n"
		 "$location: location of birth\n"
		 "$page: current page\n"
		 "$pagecount: total number of pages\n"
		 "$version: version of the software\n"
		 "$creationdate: date of document creation";
}

/*****************************************************
**
**   PageConflet   ---   updateUi
**
******************************************************/
void PageConflet::updateUi()
{
	debuglog << "PageConflet::updateUi";
	const bool headerChecked = check_header_showtext->IsChecked();
	check_header_line->Enable( headerChecked );
	text_header->Enable( headerChecked );
	label_header_orientation->Enable( headerChecked );
	choice_header_orientation->Enable( headerChecked );
	check_header_firstpage->Enable( headerChecked );

	const bool footerChecked = check_footer_showtext->IsChecked();
	check_footer_line->Enable( footerChecked );
	text_footer->Enable( footerChecked );
	label_footer_orientation->Enable( footerChecked );
	choice_footer_orientation->Enable( footerChecked );
	check_footer_firstpage->Enable( footerChecked );
}

