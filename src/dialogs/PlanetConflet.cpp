/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PlanetConflet.cpp
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

#include "PlanetConflet.h"

#include "ArabicPartBase.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Lang.h"
#include "logger.h"
#include "RearrangableListCtrl.h"
#include "SymbolProvider.h"
#include "Table.h"
#include "TextWidget.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choicdlg.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

enum { CD_ARABIC = wxID_HIGHEST + 1, CD_LIST };

// begin wxGlade: ::extracode
// end wxGlade

/*****************************************************
**
**   PlanetConflet   ---   Constructor
**
******************************************************/
PlanetConflet::PlanetConflet( wxWindow *parent, const Viewpoint &viewpoint  )
	: ConfletIncarnation( parent, astroconfig->getViewpointConfig( viewpoint ).planets ),
	viewpoint( viewpoint )
{
	props = ContextProps_ptr( new ContextProperties );
	props->setPreferredViewpoint( viewpoint );

	// begin wxGlade: PlanetConflet::PlanetConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBoxSizer* sizer_items = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Items")), wxHORIZONTAL);
	sizer_main->Add(sizer_items, 1, wxEXPAND, 0);
	listctrl = new RearrangableListCtrlPlanetId(sizer_items->GetStaticBox(), CD_LIST, cfg.items);
	sizer_items->Add(listctrl, 1, wxALL|wxEXPAND, 3);
	wxBoxSizer* sizer_textformats = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_textformats, 0, wxALL, 3);
	wxStaticBoxSizer* sizer_format = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Display Options")), wxVERTICAL);
	sizer_textformats->Add(sizer_format, 0, wxEXPAND, 0);
	wxFlexGridSizer* grid_planets = new wxFlexGridSizer(4, 2, 0, 0);
	sizer_format->Add(grid_planets, 1, wxEXPAND, 0);
	label_planet_textformat = new wxStaticText(sizer_format->GetStaticBox(), wxID_ANY, _("Text Format"));
	grid_planets->Add(label_planet_textformat, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_factor_textformat_choices[] = {
		_("Short Text"),
		_("Medium Text"),
		_("Long Text"),
		_("Symbol"),
	};
	choice_factor_textformat = new wxChoice(sizer_format->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, choice_factor_textformat_choices);
	choice_factor_textformat->SetSelection(0);
	grid_planets->Add(choice_factor_textformat, 0, wxALL, 3);
	label_node_names = new wxStaticText(sizer_format->GetStaticBox(), wxID_ANY, _("Node Names"));
	grid_planets->Add(label_node_names, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_node_names_choices[] = {
		_("Node"),
		_("Dragon Head/Tail"),
		_("Rahu/Ketu"),
	};
	choice_node_names = new wxChoice(sizer_format->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_node_names_choices);
	choice_node_names->SetSelection(0);
	grid_planets->Add(choice_node_names, 0, wxALL, 3);
	label_uranus = new wxStaticText(sizer_format->GetStaticBox(), wxID_ANY, _("Uranus"));
	label_uranus->SetMinSize(wxSize(80, -1));
	grid_planets->Add(label_uranus, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxBoxSizer* sizer_uranus2 = new wxBoxSizer(wxHORIZONTAL);
	grid_planets->Add(sizer_uranus2, 1, wxEXPAND, 0);
	const wxString choice_uranus_choices[] = {
		_("1"),
		_("2"),
	};
	choice_uranus = new wxChoice(sizer_format->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_uranus_choices);
	choice_uranus->SetSelection(0);
	sizer_uranus2->Add(choice_uranus, 0, wxALL|wxEXPAND, 3);
	preview_uranus = new wxStaticText(sizer_format->GetStaticBox(), wxID_ANY, _("h"));
	preview_uranus->SetMinSize(wxSize(30, -1));
	sizer_uranus2->Add(preview_uranus, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	label_pluto = new wxStaticText(sizer_format->GetStaticBox(), wxID_ANY, _("Pluto"));
	grid_planets->Add(label_pluto, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxBoxSizer* sizer_pluto2 = new wxBoxSizer(wxHORIZONTAL);
	grid_planets->Add(sizer_pluto2, 1, wxEXPAND, 0);
	const wxString choice_pluto_choices[] = {
		_("1"),
		_("2"),
		_("3"),
	};
	choice_pluto = new wxChoice(sizer_format->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_pluto_choices);
	choice_pluto->SetSelection(0);
	sizer_pluto2->Add(choice_pluto, 0, wxALL|wxEXPAND, 3);
	preview_pluto = new wxStaticText(sizer_format->GetStaticBox(), wxID_ANY, _("j"));
	preview_pluto->SetMinSize(wxSize(30, -1));
	sizer_pluto2->Add(preview_pluto, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	check_vedic_names = new wxCheckBox(sizer_format->GetStaticBox(), wxID_ANY, _("Vedic Names"));
	sizer_format->Add(check_vedic_names, 0, wxALL, 3);
	wxStaticBoxSizer* sizer_arabic = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Arabic Parts")), wxVERTICAL);
	sizer_textformats->Add(sizer_arabic, 0, wxEXPAND, 0);
	button_arabic = new wxButton(sizer_arabic->GetStaticBox(), CD_ARABIC, _("Select ..."));
	sizer_arabic->Add(button_arabic, 0, wxALL, 3);
	label_arabic_selection = new wxStaticText(sizer_arabic->GetStaticBox(), wxID_ANY, _("No parts selected"));
	sizer_arabic->Add(label_arabic_selection, 0, wxALL, 3);
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

	constexpr int fontsize = 14;
	const wxFont symbolFont( fontsize, wxFONTFAMILY_ROMAN,
		wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, gdiconfig->fonts.symbolFontFacename );

	preview_uranus->SetFont( symbolFont );
	preview_pluto->SetFont( symbolFont );

	choice_factor_textformat->SetValidator( wxGenericValidator( (int*)&cfg.textFormat ));
	choice_node_names->SetValidator( wxGenericValidator( &cfg.nodeNames ));

	check_vedic_names->SetValidator( wxGenericValidator( &cfg.useVedicNames ));

	choice_uranus->SetValidator( wxGenericValidator( &cfg.uranusSymbol ));
	choice_pluto->SetValidator( wxGenericValidator( &cfg.plutoSymbol ));

	Connect( CD_ARABIC, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlanetConflet::OnArabicParts ));
	Connect( wxID_ANY, wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( PlanetConflet::OnCheckBox ));
}

/*****************************************************
**
**   PlanetConflet   ---   updateUi
**
******************************************************/
void PlanetConflet::updateUi()
{
	TransferDataFromWindow();
	debuglog << "PlanetConflet::updateUi";
	debuglog << "PlanetConflet::updateUi cfg.useVedicNames" << cfg.useVedicNames;

	// Step 1: set labels of symbol choices
	SymbolProvider sp;
	preview_uranus->SetLabel( sp.getPlanetCode( PlanetId::Uranus, cfg.uranusSymbol, cfg.plutoSymbol ));
	preview_pluto->SetLabel( sp.getPlanetCode( PlanetId::Pluto, cfg.uranusSymbol, cfg.plutoSymbol ));

	// Step 2: enable/disable items
	const bool isSymbol = ( choice_factor_textformat->GetSelection() == 3 );
	label_uranus->Enable( isSymbol );
	choice_uranus->Enable( isSymbol );
	preview_uranus->Enable( isSymbol );
	label_pluto->Enable( isSymbol );
	choice_pluto->Enable( isSymbol );
	preview_pluto->Enable( isSymbol );
	label_node_names->Enable( ! isSymbol );
	choice_node_names->Enable( ! isSymbol );
	check_vedic_names->Enable( ! isSymbol );

	// Step 3: label for Arabic Parts
  if ( cfg.arabicParts.size() == 0 )
  {
    label_arabic_selection->SetLabel( tr( "No part selected" ));
  }
  else if ( cfg.arabicParts.size() == 1 )
  {
    label_arabic_selection->SetLabel( ArabicPartsConfig::getPlanetName(
      (PlanetId)(*cfg.arabicParts.begin() + (int)PlanetId::FirstArabic), TextFormat::Long ));
  }
  else
  {
    label_arabic_selection->SetLabel( std::format( "{} {}", cfg.arabicParts.size(), tr( "parts selected" )));
  }

	// Step 4: write preview document
	Lang lang;
	MDocument_ptr doc = preview->getEmptyDoc();
	if ( cfg.items.size() > 0 )
	{
		//doc->addHeader2( tr( "Preview" ));

		Table *table = new Table;
		table->addHeaderRow( { wxString( tr( "No." )), tr( "Name" ) } );
		int line = 1;

		Planets planets;
		for( const auto &item: cfg.items )
		{
			if ( item.second ) planets.push_back( item.first );
		}

		for( const PlanetId &ob : PlanetHelper::expandPlanets( planets, cfg.arabicParts ))
		{
				TableRow row;
				if ( cfg.textFormat == TextFormat::Symbol )
				{
					row.addValue( lang.getPlanetName( ob, viewpoint, TextFormat::Long ));
				}
				else
				{
					row.addInteger( line++ );
				}
				row.addValue( MToken( ob, cfg ));
				table->addRow( row );
		}
		doc->addItem( table );
	}
	else
	{
		doc->addParagraph( tr( "No planet groups selected" ));
	}
	preview->OnDataChanged();
}

/*************************************************//**
*
**   PlanetConflet   ---   OnCheckBox
*
******************************************************/
void PlanetConflet::OnCheckBox( wxCommandEvent& )
{
	//debuglog << "PlanetConflet::OnCheckBox object group size " << cfg.objectGroups.size();;
	TransferDataFromWindow();
	guiHasChanged();
}

/*****************************************************
**
**   PlanetConflet   ---   OnArabicParts
**
******************************************************/
void PlanetConflet::OnArabicParts( wxCommandEvent& )
{
	debuglog << "PlanetConflet::OnArabicParts";
	wxString parts[MAX_ARABIC_PARTS];
	for ( uint i = 0; i < MAX_ARABIC_PARTS; i++ )
	{
		parts[i] = ArabicPartsConfig::getPlanetName( (PlanetId)( (int)PlanetId::FirstArabic + i ), TextFormat::Long );
	}

	wxMultiChoiceDialog dialog( this,	_( "Choose Arabic Parts" ), GUI_APP_NAME, MAX_ARABIC_PARTS, parts );

	// convert std::list to wxArrayInt
	{
		wxArrayInt wxa;
		for( const auto &i : cfg.arabicParts ) wxa.Add( i );
		dialog.SetSelections( wxa );
	}

	if ( dialog.ShowModal() == wxID_OK )
	{
		cfg.arabicParts.clear();
		for( uint i = 0; i < dialog.GetSelections().Count(); i++ )
		{
			cfg.arabicParts.push_back( dialog.GetSelections().Item( i ));
		}

		// TODO AllArabic rein oder raus
		//const bool arabicFound = std::find( cfg.items.begin(), cfg.items.end(), 
		bool arabicFound = false;
		for ( const auto &item : cfg.items )
		{
			if ( item.first == PlanetId::ArabicParts )
			{
				arabicFound = true;
				break;
			}
		}
		if ( cfg.arabicParts.size() > 0 && ! arabicFound )
		{
			cfg.items.push_back( { PlanetId::ArabicParts, true } );
			TransferDataToWindow();
		}
		else if ( cfg.arabicParts.size() == 0 && arabicFound )
		{
			auto iter = cfg.items.begin();
			while( iter != cfg.items.end())
			{
				if ( iter->first == PlanetId::ArabicParts )
				{
					cfg.items.erase( iter );
					break;
				}
				++iter;
			}
			TransferDataToWindow();
		}
		guiHasChanged();
	}
}

/*****************************************************
**
**   PlanetConflet   ---   restoreDefaults
**
******************************************************/
void PlanetConflet::restoreDefaults()
{
	debuglog << "PlanetConflet::restoreDefaults viewpoint " << viewpoint;
  cfg = PlanetConfig( viewpoint );
	TransferDataToWindow();
}

