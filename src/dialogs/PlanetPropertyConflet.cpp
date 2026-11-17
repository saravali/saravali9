/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PlanetPropertyConflet.cpp
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

#include "PlanetPropertyConflet.h"

#include "ArabicPartBase.h"
#include "ContextProperties.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "logger.h"
#include "MDocument.h"
#include "RearrangableListCtrl.h"
#include "SymbolProvider.h"
#include "Table.h"
#include "TableWriter.h"
#include "TextWidget.h"

#include <wx/checkbox.h>
#include <wx/checklst.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

enum { CD_LIST = wxID_HIGHEST + 1 };

// begin wxGlade: ::extracode
// end wxGlade

/*****************************************************
**
**   PlanetPropertyConflet   ---   Constructor
**
******************************************************/
PlanetPropertyConflet::PlanetPropertyConflet( wxWindow *parent, const Viewpoint &viewpoint  )
	: ConfletIncarnation( parent, astroconfig->getViewpointConfig( viewpoint ).planetProps ),
	viewpoint( viewpoint )
{
	props = ContextProps_ptr( new ContextProperties );
	props->setPreferredViewpoint( viewpoint );
	horoscope = Horoscope_ptr( new Horoscope );
	props->setRadixHoroscope( horoscope );

	// begin wxGlade: PlanetPropertyConflet::PlanetPropertyConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBoxSizer* sizer_columns = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Columns")), wxHORIZONTAL);
	sizer_main->Add(sizer_columns, 1, wxEXPAND, 0);
	listctrl = new RearrangableListCtrlColumnGroup(sizer_columns->GetStaticBox(), CD_LIST, cfg.columnGroups);
	sizer_columns->Add(listctrl, 1, wxEXPAND, 0);
	wxBoxSizer* sizer_format = new wxBoxSizer(wxVERTICAL);
	sizer_columns->Add(sizer_format, 1, wxEXPAND, 0);
	wxStaticBoxSizer* sizer_signs = new wxStaticBoxSizer(new wxStaticBox(sizer_columns->GetStaticBox(), wxID_ANY, _("Sign Format")), wxVERTICAL);
	sizer_format->Add(sizer_signs, 0, wxEXPAND, 0);
	wxGridSizer* grid_format = new wxGridSizer(2, 2, 0, 0);
	sizer_signs->Add(grid_format, 0, wxEXPAND, 0);
	label_sign_textformat = new wxStaticText(sizer_signs->GetStaticBox(), wxID_ANY, _("Text Format"));
	grid_format->Add(label_sign_textformat, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_textformat_choices[] = {
		_("Short Text"),
		_("Medium Text"),
		_("Long Text"),
		_("Symbol"),
	};
	choice_textformat = new wxChoice(sizer_signs->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, choice_textformat_choices);
	choice_textformat->SetSelection(0);
	grid_format->Add(choice_textformat, 0, wxALL, 3);
	label_capricorn = new wxStaticText(sizer_signs->GetStaticBox(), wxID_ANY, _("Capricorn Symbol"));
	grid_format->Add(label_capricorn, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxBoxSizer* sizer_capricorn2 = new wxBoxSizer(wxHORIZONTAL);
	grid_format->Add(sizer_capricorn2, 1, wxEXPAND, 0);
	const wxString choice_capricorn_choices[] = {
		_("1"),
		_("2"),
	};
	choice_capricorn = new wxChoice(sizer_signs->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_capricorn_choices);
	choice_capricorn->SetSelection(0);
	sizer_capricorn2->Add(choice_capricorn, 0, wxALL|wxEXPAND, 3);
	preview_capricorn = new wxStaticText(sizer_signs->GetStaticBox(), wxID_ANY, _("J"));
	preview_capricorn->SetMinSize(wxSize(30, -1));
	sizer_capricorn2->Add(preview_capricorn, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	check_translate_sign_names = new wxCheckBox(sizer_signs->GetStaticBox(), wxID_ANY, _("Translate Sign Names"));
	sizer_signs->Add(check_translate_sign_names, 0, wxALL, 3);
	check_vedic_sign_names = new wxCheckBox(sizer_signs->GetStaticBox(), wxID_ANY, _("Vedic Sign Names"));
	sizer_signs->Add(check_vedic_sign_names, 0, wxALL, 3);
	wxStaticBoxSizer* sizer_positions = new wxStaticBoxSizer(new wxStaticBox(sizer_columns->GetStaticBox(), wxID_ANY, _("Planet Positions")), wxVERTICAL);
	sizer_format->Add(sizer_positions, 0, wxEXPAND, 0);
	wxGridSizer* grid_positions = new wxGridSizer(3, 2, 0, 0);
	sizer_positions->Add(grid_positions, 0, wxALL, 3);
	label_position_style = new wxStaticText(sizer_positions->GetStaticBox(), wxID_ANY, _("Style"));
	grid_positions->Add(label_position_style, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_position_style_choices[] = {
		_("Normal"),
		_("Compact"),
		_("Colons"),
		_("Vedic"),
	};
	choice_position_style = new wxChoice(sizer_positions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, choice_position_style_choices);
	grid_positions->Add(choice_position_style, 0, wxALL|wxEXPAND, 3);
	label_precision = new wxStaticText(sizer_positions->GetStaticBox(), wxID_ANY, _("Precision"));
	grid_positions->Add(label_precision, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_precision_choices[] = {
		_("Minute"),
		_("Second"),
		_("More"),
	};
	choice_precision = new wxChoice(sizer_positions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_precision_choices);
	grid_positions->Add(choice_precision, 0, wxALL|wxEXPAND, 3);
	label_retrogression = new wxStaticText(sizer_positions->GetStaticBox(), wxID_ANY, _("Retrogression"));
	grid_positions->Add(label_retrogression, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_retrogression_choices[] = {
		_("Symbol"),
		_("Text"),
		_("Off"),
	};
	choice_retrogression = new wxChoice(sizer_positions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_retrogression_choices);
	grid_positions->Add(choice_retrogression, 0, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_vedic = new wxStaticBoxSizer(new wxStaticBox(sizer_columns->GetStaticBox(), wxID_ANY, _("Special Vedic Formats")), wxVERTICAL);
	sizer_format->Add(sizer_vedic, 0, wxEXPAND, 0);
	wxBoxSizer* sizer_nakshatra = new wxBoxSizer(wxHORIZONTAL);
	sizer_vedic->Add(sizer_nakshatra, 0, wxEXPAND, 0);
	label_nakshatra_textformat = new wxStaticText(sizer_vedic->GetStaticBox(), wxID_ANY, _("Nakshatra Format"));
	sizer_nakshatra->Add(label_nakshatra_textformat, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_nakshatra_textformat_choices[] = {
		_("Short Text"),
		_("Medium Text"),
		_("Long Text"),
	};
	choice_nakshatra_textformat = new wxChoice(sizer_vedic->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_nakshatra_textformat_choices);
	choice_nakshatra_textformat->SetSelection(0);
	sizer_nakshatra->Add(choice_nakshatra_textformat, 0, wxALL, 3);
	wxStaticBoxSizer* sizer_preview = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Preview")), wxVERTICAL);
	sizer_main->Add(sizer_preview, 1, wxALL|wxEXPAND, 3);
	panel_preview = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxTAB_TRAVERSAL);
	sizer_preview->Add(panel_preview, 1, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* grid_previewpanel = new wxFlexGridSizer(1, 1, 3, 3);
	preview = new TextWidget(panel_preview, props, WidgetCategory::TextGeneric);
	preview->SetMinSize(wxSize(200, 220));
	grid_previewpanel->Add(preview, 1, wxALL|wxEXPAND, 3);
	
	grid_previewpanel->AddGrowableRow(0);
	grid_previewpanel->AddGrowableCol(0);
	panel_preview->SetSizer(grid_previewpanel);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	sizer_vedic->Show( viewpoint == Viewpoint::Vedic );

	constexpr int fontsize = 14;
	const wxFont symbolFont( fontsize, wxFONTFAMILY_ROMAN,
		wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, gdiconfig->fonts.symbolFontFacename );

	preview_capricorn->SetFont( symbolFont );

	choice_textformat->SetValidator( wxGenericValidator( (int*)&cfg.textFormat ));
	choice_position_style->SetValidator( wxGenericValidator( (int*)&cfg.positionStyle ));
	choice_precision->SetValidator( wxGenericValidator( (int*)&cfg.precision ));

	check_translate_sign_names->SetValidator( wxGenericValidator( &cfg.translateSignNames ));
	check_vedic_sign_names->SetValidator( wxGenericValidator( &cfg.vedicSignNames ));

	choice_retrogression->SetValidator( wxGenericValidator( &cfg.retroFormat ));

	choice_nakshatra_textformat->SetValidator( wxGenericValidator( (int*)&cfg.nakshatraTextFormat ));
	choice_capricorn->SetValidator( wxGenericValidator( &cfg.capricornSymbol ));

	Connect( wxID_ANY, wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( PlanetPropertyConflet::OnCheckBox ));
}

/*****************************************************
**
**   PlanetPropertyConflet   ---   updateUi
**
******************************************************/
void PlanetPropertyConflet::updateUi()
{
	TransferDataFromWindow();
	debuglog << "PlanetPropertyConflet::updateUi";

	// Step 1: set labels of symbol choices
	SymbolProvider sp;
	preview_capricorn->SetLabel( sp.getSignCode( Sign::Capricorn, cfg.capricornSymbol ));

	// Step 2: enable/disable items
	const bool isSymbol = ( choice_textformat->GetSelection() == 3 );
	label_capricorn->Enable( isSymbol );
	choice_capricorn->Enable( isSymbol );
	preview_capricorn->Enable( isSymbol );
	check_vedic_sign_names->Enable( ! isSymbol );;

	// Step 3: write preview document
	MDocument_ptr doc = preview->getEmptyDoc();
	if ( cfg.columnGroups.size() > 0 )
	{
		doc->addHeader2( tr( "Preview of Column Groups" ));

		horoscope->dl.date.setCurrentDate();
		horoscope->update();

		TableWriter writer( props, horoscope );
		doc->addItem( writer.createPlanetListTable( props->getPlanets( viewpoint ),
			astroconfig->getViewpointConfig( viewpoint ).planets, cfg ));
	}
	else
	{
		doc->addParagraph( tr( "No column groups selected" ));
	}
	preview->OnDataChanged();
}

/*************************************************//**
*
**   PlanetPropertyConflet   ---   OnCheckBox
*
******************************************************/
void PlanetPropertyConflet::OnCheckBox( wxCommandEvent& )
{
	debuglog << "PlanetPropertyConflet::OnCheckBox";
	TransferDataFromWindow();
	guiHasChanged();
}

/*****************************************************
**
**   PlanetPropertyConflet   ---   restoreDefaults
**
******************************************************/
void PlanetPropertyConflet::restoreDefaults()
{
	debuglog << "PlanetPropertyConflet::restoreDefaults";
  cfg = PlanetPropertyConfig( viewpoint );
	TransferDataToWindow();
}

