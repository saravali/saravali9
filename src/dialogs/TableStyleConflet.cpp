/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/TableStyleConflet.cpp
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

#include "TableStyleConflet.h"

#include "ConfigLoaderImpls.h"
#include "ContextProperties.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "logger.h"
#include "StyleManagerDialog.h"
#include "TextHelper.h"
#include "TextWidget.h"

#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/clrpicker.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum
{
	CD_SELECT_TABLESTYLE = wxID_HIGHEST + 1,
	CD_STYLE_MANAGER,

	// color pickers
	CD_HEADER_TEXTCOLOR,
	CD_HEADER_BGCOLOR,
	CD_CELL_TEXTCOLOR,
	CD_BGMODE,
	CD_ALL_CELL_BGCOLOR,
	CD_ODD_CELL_BGCOLOR,
	CD_EVEN_CELL_BGCOLOR
};

/*****************************************************
**
**   TableStyleConflet   ---   Constructor
**
******************************************************/
TableStyleConflet::TableStyleConflet( wxWindow *parent )
	: ConfletIncarnation( parent, gdiconfig->tableStyleId ),
	tableStyles( TableStyleLoader::get()->getConfigs())
{
	props = ContextProps_ptr( new ContextProperties );
	horoscope = Horoscope_ptr( new Horoscope );
	props->setRadixHoroscope( horoscope );

	if ( tableStyles.size() == 0 )
	{
		tableStyles = TableStyleLoader::get()->getConfigs();
	}

	// begin wxGlade: TableStyleConflet::TableStyleConflet
	wxFlexGridSizer* sizer_main = new wxFlexGridSizer(1, 1, 0, 0);
	wxBoxSizer* sizer_tablestyles = new wxBoxSizer(wxHORIZONTAL);
	sizer_main->Add(sizer_tablestyles, 1, wxEXPAND, 0);
	wxFlexGridSizer* grid_tablestyles = new wxFlexGridSizer(2, 1, 0, 0);
	sizer_tablestyles->Add(grid_tablestyles, 0, wxALL|wxEXPAND, 3);
	wxBoxSizer* sizer_select_style = new wxBoxSizer(wxHORIZONTAL);
	grid_tablestyles->Add(sizer_select_style, 1, wxALL|wxEXPAND, 3);
	label_tablestyle = new wxStaticText(this, wxID_ANY, _("Style"));
	sizer_select_style->Add(label_tablestyle, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_tablestyle_choices[] = {
		_("dummy"),
		_("dummy"),
		_("dummy"),
		_("dummy"),
	};
	choice_tablestyle = new wxChoice(this, CD_SELECT_TABLESTYLE, wxDefaultPosition, wxDefaultSize, 4, choice_tablestyle_choices);
	choice_tablestyle->SetSelection(0);
	sizer_select_style->Add(choice_tablestyle, 1, wxALL, 3);
	button_manage = new wxButton(this, CD_STYLE_MANAGER, _("Manage ..."));
	sizer_select_style->Add(button_manage, 0, wxALL, 3);
	wxFlexGridSizer* grid_pane_table = new wxFlexGridSizer(3, 1, 0, 0);
	grid_tablestyles->Add(grid_pane_table, 1, wxEXPAND, 0);
	wxStaticBoxSizer* sizer_table_headers = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Table Header")), wxVERTICAL);
	grid_pane_table->Add(sizer_table_headers, 1, wxEXPAND, 0);
	check_use_header_colors = new wxCheckBox(sizer_table_headers->GetStaticBox(), wxID_ANY, _("Use Header Colors"));
	sizer_table_headers->Add(check_use_header_colors, 0, wxALL, 3);
	wxFlexGridSizer* grid_properties = new wxFlexGridSizer(2, 2, 0, 0);
	sizer_table_headers->Add(grid_properties, 1, wxEXPAND, 0);
	label_header_textcolor = new wxStaticText(sizer_table_headers->GetStaticBox(), wxID_ANY, _("Text Color"));
	grid_properties->Add(label_header_textcolor, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_header_textcolor = new wxColourPickerCtrl(sizer_table_headers->GetStaticBox(), CD_HEADER_TEXTCOLOR);
	grid_properties->Add(button_header_textcolor, 1, wxALIGN_RIGHT|wxALL, 3);
	label_header_bgcolor = new wxStaticText(sizer_table_headers->GetStaticBox(), wxID_ANY, _("Background"));
	grid_properties->Add(label_header_bgcolor, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_header_bgcolor = new wxColourPickerCtrl(sizer_table_headers->GetStaticBox(), CD_HEADER_BGCOLOR);
	grid_properties->Add(button_header_bgcolor, 1, wxALIGN_RIGHT|wxALL, 3);
	wxStaticBoxSizer* sizer_table_cells = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Table Cells")), wxVERTICAL);
	grid_pane_table->Add(sizer_table_cells, 1, wxEXPAND, 0);
	check_use_cell_colors = new wxCheckBox(sizer_table_cells->GetStaticBox(), wxID_ANY, _("Use Cell Colors"));
	sizer_table_cells->Add(check_use_cell_colors, 0, wxALL, 3);
	wxFlexGridSizer* grid_table_cells = new wxFlexGridSizer(5, 2, 0, 0);
	sizer_table_cells->Add(grid_table_cells, 1, wxALL|wxEXPAND, 3);
	label_cell_textcolor = new wxStaticText(sizer_table_cells->GetStaticBox(), wxID_ANY, _("Text Color"));
	grid_table_cells->Add(label_cell_textcolor, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_cell_textcolor = new wxColourPickerCtrl(sizer_table_cells->GetStaticBox(), CD_CELL_TEXTCOLOR);
	grid_table_cells->Add(button_cell_textcolor, 1, wxALIGN_RIGHT|wxALL, 3);
	label_cell_bgmode = new wxStaticText(sizer_table_cells->GetStaticBox(), wxID_ANY, _("Cell Background Mode"));
	grid_table_cells->Add(label_cell_bgmode, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_cell_bgmode_choices[] = {
		_("Transparent"),
		_("Constant"),
		_("Alternating"),
	};
	choice_cell_bgmode = new wxChoice(sizer_table_cells->GetStaticBox(), CD_BGMODE, wxDefaultPosition, wxDefaultSize, 3, choice_cell_bgmode_choices);
	choice_cell_bgmode->SetSelection(0);
	grid_table_cells->Add(choice_cell_bgmode, 1, wxALIGN_RIGHT|wxALL, 3);
	label_all_row_bgcolor = new wxStaticText(sizer_table_cells->GetStaticBox(), wxID_ANY, _("All Rows"));
	grid_table_cells->Add(label_all_row_bgcolor, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_all_cell_bgcolor = new wxColourPickerCtrl(sizer_table_cells->GetStaticBox(), CD_ALL_CELL_BGCOLOR);
	grid_table_cells->Add(button_all_cell_bgcolor, 1, wxALIGN_RIGHT|wxALL, 3);
	label_even_row_bgcolor = new wxStaticText(sizer_table_cells->GetStaticBox(), wxID_ANY, _("Even Rows"));
	grid_table_cells->Add(label_even_row_bgcolor, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_even_cell_bgcolor = new wxColourPickerCtrl(sizer_table_cells->GetStaticBox(), CD_EVEN_CELL_BGCOLOR);
	grid_table_cells->Add(button_even_cell_bgcolor, 1, wxALIGN_RIGHT|wxALL, 3);
	label_odd_row_bgcolor = new wxStaticText(sizer_table_cells->GetStaticBox(), wxID_ANY, _("Odd Rows"));
	grid_table_cells->Add(label_odd_row_bgcolor, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_odd_cell_bgcolor = new wxColourPickerCtrl(sizer_table_cells->GetStaticBox(), CD_ODD_CELL_BGCOLOR);
	grid_table_cells->Add(button_odd_cell_bgcolor, 1, wxALIGN_RIGHT|wxALL, 3);
	wxStaticBoxSizer* sizer_table_border = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Borders")), wxVERTICAL);
	grid_pane_table->Add(sizer_table_border, 0, wxEXPAND, 0);
	check_show_grid = new wxCheckBox(sizer_table_border->GetStaticBox(), wxID_ANY, _("Show Grid"));
	sizer_table_border->Add(check_show_grid, 0, wxALL, 3);
	wxStaticBoxSizer* sizer_preview = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Preview")), wxVERTICAL);
	sizer_tablestyles->Add(sizer_preview, 1, wxALL|wxEXPAND, 3);
	panel_preview = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxTAB_TRAVERSAL);
	sizer_preview->Add(panel_preview, 1, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* grid_previewpanel = new wxFlexGridSizer(1, 1, 3, 3);
	preview = new TextWidget(panel_preview, props, WidgetCategory::TextGeneric);
	preview->SetMinSize(wxSize(200, 220));
	grid_previewpanel->Add(preview, 1, wxALL|wxEXPAND, 3);
	
	grid_previewpanel->AddGrowableRow(0);
	grid_previewpanel->AddGrowableCol(0);
	panel_preview->SetSizer(grid_previewpanel);
	grid_table_cells->AddGrowableCol(1);
	grid_properties->AddGrowableCol(1);
	grid_pane_table->AddGrowableCol(0);
	grid_tablestyles->AddGrowableCol(0);
	sizer_main->AddGrowableRow(0);
	sizer_main->AddGrowableCol(0);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	updateStyleChoice();
	setControlValues();
	updateUi();

	Connect( CD_STYLE_MANAGER, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TableStyleConflet::OnStyleManager ));
	Connect( CD_BGMODE, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TableStyleConflet::OnCommand ));
	Connect( wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( TableStyleConflet::OnCommand ));
	Connect( wxID_ANY, wxEVT_COMMAND_COLOURPICKER_CHANGED, wxCommandEventHandler( TableStyleConflet::OnCommand ));

	Connect( CD_SELECT_TABLESTYLE, wxEVT_COMMAND_CHOICE_SELECTED,
		wxCommandEventHandler( TableStyleConflet::OnSelectTableStyle ));
}


/*****************************************************
**
**   TableStyleConflet   ---   Destructor
**
******************************************************/
TableStyleConflet::~TableStyleConflet()
{
}

/*****************************************************
**
**   TableStyleConflet   ---   updateUi
**
******************************************************/
void TableStyleConflet::updateUi()
{
	debuglog << "TableStyleConflet::updateUi dirty " << isDirty();
	const bool activateHeaderItems = check_use_header_colors->GetValue();
	label_header_textcolor->Enable( activateHeaderItems );
	button_header_textcolor->Enable( activateHeaderItems );
	label_header_bgcolor->Enable( activateHeaderItems );
	button_header_bgcolor->Enable( activateHeaderItems );

	const bool activateCellItems = check_use_cell_colors->GetValue();
	label_cell_textcolor->Enable( activateCellItems );
	button_cell_textcolor->Enable( activateCellItems );
	label_cell_bgmode->Enable( activateCellItems );
	choice_cell_bgmode->Enable( activateCellItems );

	const bool enableAllCellBg = ( activateCellItems && choice_cell_bgmode->GetSelection() == 1 );
	label_all_row_bgcolor->Enable( enableAllCellBg );
	button_all_cell_bgcolor->Enable( enableAllCellBg );

	const bool enableOddEvenBg = ( activateCellItems && choice_cell_bgmode->GetSelection() == 2 );
	label_odd_row_bgcolor->Enable( enableOddEvenBg );
	button_odd_cell_bgcolor->Enable( enableOddEvenBg );
	label_even_row_bgcolor->Enable( enableOddEvenBg );
	button_even_cell_bgcolor->Enable( enableOddEvenBg );

	preview->clearDoc();
	assert( cfg >= 0 && cfg < (int)choice_tablestyle->GetCount());
	preview->setTableStyle( &tableStyles[ cfg ] );

	MDocument_ptr doc = preview->getDoc();
	horoscope->dl.date.setCurrentDate();
	horoscope->update();
	TextHelper( props, doc ).writePlanets( horoscope );
	preview->OnDataChanged();	
	preview->Refresh();	
}

/*****************************************************
**
**   TableStyleConflet   ---   OnCommand
**
******************************************************/
void TableStyleConflet::OnCommand( wxCommandEvent& )
{ 
	cfg = choice_tablestyle->GetSelection();
	assert( cfg >= 0 && cfg < (int)choice_tablestyle->GetCount());
	TableStyle &style = tableStyles[ cfg ];

	style.useHeaderColors = check_use_header_colors->GetValue();
	style.headerTextColor = button_header_textcolor->GetColour();
	style.headerBgColor = button_header_bgcolor->GetColour();

	style.useCellColors = check_use_cell_colors->GetValue();
	style.cellBgMode = choice_cell_bgmode->GetSelection();

	style.cellTextColor = button_cell_textcolor->GetColour();
	style.allCellBgColor = button_all_cell_bgcolor->GetColour();
	style.oddCellBgColor = button_odd_cell_bgcolor->GetColour();
	style.evenCellBgColor = button_even_cell_bgcolor->GetColour();

	style.showGrid = check_show_grid->GetValue();
  guiHasChanged();
} 

/*****************************************************
**
**   TableStyleConflet   ---   OnStyleManager
**
******************************************************/
void TableStyleConflet::OnStyleManager( wxCommandEvent& )
{
	debuglog << "TableStyleConflet::OnStyleManager";
	StyleManagerDialog dialog( this, tableStyles, cfg );
	dialog.ShowModal();

	updateStyleChoice();
  guiHasChanged();
}

/*****************************************************
**
**   TableStyleConflet   ---   updateStyleChoice
**
******************************************************/
void TableStyleConflet::updateStyleChoice()
{
	choice_tablestyle->Clear();
	for( const auto &a : tableStyles )
	{
		choice_tablestyle->Append( a.name );
	}
	if ( cfg > 0 && cfg < (int)tableStyles.size())
	{
		choice_tablestyle->SetSelection( cfg );
	}
	else
	{
		warnlog << "TableStyleConflet::updateStyleChoice tableStyleId " << cfg << " invalid";
		choice_tablestyle->SetSelection( 0 );
	}
}

/*****************************************************
**
**   TableStyleConflet   ---   setControlValues
**
******************************************************/
void TableStyleConflet::setControlValues()
{
	if ( cfg < 0 || cfg >= (int)choice_tablestyle->GetCount())
	{
		errorlog << "TableStyleConflet::setControlValues tableStyleId " << cfg << " invalid";
		cfg = 0;
	}
	const TableStyle &style = tableStyles[ cfg ];

	check_use_header_colors->SetValue( style.useHeaderColors );
	button_header_textcolor->SetColour( style.headerTextColor );
	button_header_bgcolor->SetColour( style.headerBgColor );

	check_use_cell_colors->SetValue( style.useCellColors );
	choice_cell_bgmode->SetSelection( style.cellBgMode );

	button_cell_textcolor->SetColour( style.cellTextColor );
	button_all_cell_bgcolor->SetColour( style.allCellBgColor );
	button_odd_cell_bgcolor->SetColour( style.oddCellBgColor );
	button_even_cell_bgcolor->SetColour( style.evenCellBgColor );

	check_show_grid->SetValue( style.showGrid );
}

/*****************************************************
**
**   TableStyleConflet   ---   OnSelectTableStyle
**
******************************************************/
void TableStyleConflet::OnSelectTableStyle( wxCommandEvent& )
{
	cfg = choice_tablestyle->GetSelection();
	debuglog << "TableStyleConflet::OnSelectTableStyle sel " << cfg;
	setControlValues();
	updateUi();
  guiHasChanged();
}

/*****************************************************
**
**   TableStyleConflet   ---   config2model
**
******************************************************/
void TableStyleConflet::config2model()
{
	cfg = globalConfig;
	tableStyles = TableStyleLoader::get()->getConfigs();
}

/*****************************************************
**
**   TableStyleConflet   ---   model2config
**
******************************************************/
void TableStyleConflet::model2config()
{
	debuglog << "TableStyleConflet::model2config cfg " << cfg;
	globalConfig = cfg;
	TableStyleLoader *loader = TableStyleLoader::get();
	loader->setConfigs( tableStyles );
	loader->saveConfigs();
}

/*****************************************************
**
**   TableStyleConflet   ---   restoreDefaults
**
******************************************************/
void TableStyleConflet::restoreDefaults()
{
	debuglog << "TableStyleConflet::restoreDefaults";

	cfg = 0;
	tableStyles = TableStyleLoader::get()->getConfigs();
	setControlValues();
	updateStyleChoice();
	guiHasChanged();
}
