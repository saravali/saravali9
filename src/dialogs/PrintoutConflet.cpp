/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PrintoutConflet.cpp
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

#include "PrintoutConflet.h"

#include "astroconfig.h"
#include "ChartGeometry.h"
#include "CommonControls.h"
#include "ConfigLoaderImpls.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "logger.h"
#include "PdfLauncher.h"
#include "Sheet.h"
#include "Viewpoint.h"

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>

extern Gdiconfig *gdiconfig;
extern Astroconfig *astroconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_CHOICE_PRINTOUT_TYPE = wxID_HIGHEST + 1, CD_CHOICE_CATEGORY, CD_CHOICE_GEOM,
	CD_CHOICE_SKIN, CD_CHOICE_TABLESTYLE, CD_PRINT_TEST_PAGE };

/*****************************************************
**
**   PrintoutConflet   ---   Constructor
**
******************************************************/
PrintoutConflet::PrintoutConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->printout )
{
	props = ContextProps_ptr( new ContextProperties );

	// begin wxGlade: PrintoutConflet::PrintoutConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* sizer_left = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_left, 1, wxEXPAND, 0);
	wxStaticBoxSizer* sizer_printout = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Printout Type")), wxVERTICAL);
	sizer_left->Add(sizer_printout, 0, wxALL|wxEXPAND, 3);
	const wxString *choice_printout_type_choices = NULL;
	choice_printout_type = new wxChoice(sizer_printout->GetStaticBox(), CD_CHOICE_PRINTOUT_TYPE, wxDefaultPosition, wxDefaultSize, 0, choice_printout_type_choices);
	sizer_printout->Add(choice_printout_type, 0, wxALL, 3);
	wxBoxSizer* sizer_details = new wxBoxSizer(wxVERTICAL);
	sizer_printout->Add(sizer_details, 1, wxEXPAND, 0);
	wxFlexGridSizer* grid_details = new wxFlexGridSizer(7, 2, 0, 0);
	sizer_details->Add(grid_details, 1, wxEXPAND, 0);
	wxStaticText* title_description = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("Description"));
	grid_details->Add(title_description, 0, wxALL, 3);
	label_description = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("xxx"));
	grid_details->Add(label_description, 0, wxALL, 3);
	wxStaticText* title_viewpoint = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("Viewpoint"));
	grid_details->Add(title_viewpoint, 0, wxALL, 3);
	label_viewpoint = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("xxx"));
	grid_details->Add(label_viewpoint, 0, wxALL, 3);
	wxStaticText* title_itemcount = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("Number of Items"));
	grid_details->Add(title_itemcount, 0, wxALL, 3);
	label_itemcount = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("xxx"));
	grid_details->Add(label_itemcount, 0, wxALL, 3);
	wxStaticText* title_planets = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("Planets and Houses"));
	grid_details->Add(title_planets, 0, wxALL, 3);
	label_planets = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("xxx"));
	grid_details->Add(label_planets, 0, wxALL, 3);
	wxStaticText* title_columns = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("Property Columns"));
	grid_details->Add(title_columns, 0, wxALL, 3);
	label_columns = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("xxx"));
	grid_details->Add(label_columns, 0, wxALL, 3);
	wxStaticText* title_charts = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("Charts"));
	grid_details->Add(title_charts, 0, wxALL, 3);
	label_charts = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("xxx"));
	grid_details->Add(label_charts, 0, wxALL, 3);
	wxStaticText* title_basedata = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("Basedata Items"));
	grid_details->Add(title_basedata, 0, wxALL, 3);
	label_basedata = new wxStaticText(sizer_printout->GetStaticBox(), wxID_ANY, _("xxx"));
	grid_details->Add(label_basedata, 0, wxALL, 3);
	wxBoxSizer* sizer_bottom = new wxBoxSizer(wxVERTICAL);
	sizer_left->Add(sizer_bottom, 1, 0, 0);
	wxStaticBoxSizer* sizer_chartstyles = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Chart Styles")), wxHORIZONTAL);
	sizer_bottom->Add(sizer_chartstyles, 0, wxEXPAND, 0);
	wxFlexGridSizer* grid_chartstyles = new wxFlexGridSizer(4, 2, 0, 0);
	sizer_chartstyles->Add(grid_chartstyles, 1, wxALL|wxEXPAND, 3);
	wxStaticText* label_category = new wxStaticText(sizer_chartstyles->GetStaticBox(), wxID_ANY, _("Category"));
	grid_chartstyles->Add(label_category, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_category_choices[] = {
		_("Western Chart"),
		_("Uranian Chart"),
		_("Sarvatobhadra Chart"),
		_("Varga Chart"),
	};
	choice_category = new wxChoice(sizer_chartstyles->GetStaticBox(), CD_CHOICE_CATEGORY, wxDefaultPosition, wxDefaultSize, 4, choice_category_choices);
	grid_chartstyles->Add(choice_category, 0, wxALL|wxEXPAND, 3);
	wxStaticText* label_geom = new wxStaticText(sizer_chartstyles->GetStaticBox(), wxID_ANY, _("Geometry"));
	grid_chartstyles->Add(label_geom, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_geom_choices[] = {
		_("dummy"),
	};
	choice_geom = new wxChoice(sizer_chartstyles->GetStaticBox(), CD_CHOICE_GEOM, wxDefaultPosition, wxDefaultSize, 1, choice_geom_choices);
	choice_geom->SetSelection(0);
	grid_chartstyles->Add(choice_geom, 0, wxALL|wxEXPAND, 3);
	wxStaticText* label_skin = new wxStaticText(sizer_chartstyles->GetStaticBox(), wxID_ANY, _("Skin"));
	grid_chartstyles->Add(label_skin, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_skin_choices[] = {
		_("dummy"),
	};
	choice_skin = new wxChoice(sizer_chartstyles->GetStaticBox(), CD_CHOICE_SKIN, wxDefaultPosition, wxDefaultSize, 1, choice_skin_choices);
	choice_skin->SetSelection(0);
	grid_chartstyles->Add(choice_skin, 0, wxALL|wxEXPAND, 3);
	wxStaticText* label_tablestyle = new wxStaticText(sizer_chartstyles->GetStaticBox(), wxID_ANY, _("Table Style"));
	grid_chartstyles->Add(label_tablestyle, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_tablestyle_choices[] = {
		_("dummy"),
	};
	choice_tablestyle = new wxChoice(sizer_chartstyles->GetStaticBox(), CD_CHOICE_TABLESTYLE, wxDefaultPosition, wxDefaultSize, 1, choice_tablestyle_choices);
	choice_tablestyle->SetSelection(0);
	grid_chartstyles->Add(choice_tablestyle, 2, wxALL|wxEXPAND, 3);
	button_test_page = new wxButton(this, CD_PRINT_TEST_PAGE, _("Print Test Page"));
	sizer_bottom->Add(button_test_page, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
	
	grid_details->AddGrowableCol(1);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	choice_printout_type->Clear();
	for( auto name : SheetSpecLoader::get()->getNames())
	{
		choice_printout_type->Append( name );
	}
	choice_printout_type->SetValidator( wxGenericValidator( &cfg.sheetId ));

	choice_category->SetSelection( gdiconfig->prefs.printoutConfletSelectedCategory );

	choice_skin->Clear();
	for( const auto &name : WidgetSkinLoader::get()->getNames())
	{
		choice_skin->Append( name );
	}
	choice_skin->SetSelection( 0 );

	choice_tablestyle->Clear();
	for( const auto &name : TableStyleLoader::get()->getNames())
	{
		choice_tablestyle->Append( name );
	}

	choice_tablestyle->SetValidator( wxGenericValidator( &cfg.tableStyleId ));
	updateCategoryDepChoices();

	Connect( CD_PRINT_TEST_PAGE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PrintoutConflet::OnPrintTestPage ));
	Connect( CD_CHOICE_CATEGORY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PrintoutConflet::OnChoiceCategory ));
	Connect( CD_CHOICE_GEOM, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PrintoutConflet::OnChoiceGeom ));
	Connect( CD_CHOICE_SKIN, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PrintoutConflet::OnChoiceSkin ));
}

/*****************************************************
**
**   PrintoutConflet   ---   Destructor
**
******************************************************/
PrintoutConflet::~PrintoutConflet()
{
	gdiconfig->prefs.printoutConfletSelectedCategory = choice_category->GetSelection();
}

/*****************************************************
**
**   PrintoutConflet   ---   updateUi
**
******************************************************/
void PrintoutConflet::updateUi()
{
	debuglog << "PrintoutConflet::updateUi";
	if ( choice_printout_type->GetSelection() != -1 )
	{
		const SheetSpec &spec = SheetSpecLoader::get()->getConfig( choice_printout_type->GetSelection() );
		const SheetSpecSummary sps = spec.getSummary();
		
		label_description->SetLabel( std::format( "{}", spec.description ));
		label_viewpoint->SetLabel( ViewpointHelper::get()->getViewpointName( spec.viewpoint ));
		label_itemcount->SetLabel( std::format( "{}", sps.itemCount ));
		label_planets->SetLabel( std::format( "{}", sps.planets.size() ));
		label_columns->SetLabel( std::format( "{}", sps.columnCount ));
		label_charts->SetLabel( std::format( "{}", sps.chartCount ));
		label_basedata->SetLabel( std::format( "{}", sps.basedataCount ));
	}
}

/*****************************************************
**
**   PrintoutConflet   ---   updateCategoryDepChoices
**
******************************************************/
void PrintoutConflet::updateCategoryDepChoices()
{
	const WidgetCategory category = (WidgetCategory)( choice_category->GetSelection() + 1 );
	debuglog << "PrintoutConflet::updateCategoryDepChoices " << category;
	assert( category >= WidgetCategory::WesternChart && category <= WidgetCategory::VargaChart );

	// setup geom choice with new contents and set selection if available
	{
		choice_geom->Clear();
		geom_mapping.clear();

		int selgeom = 0;
		const int geomId = cfg.geoms.find( category ) != cfg.geoms.end() ? cfg.geoms[ category ] : -1;

		const auto geomIds = ChartGeometryLoader::get()->getAllIdsAndNames( category );
		for( int i = 0;  i < (int)geomIds.size(); i++ )
		{
			choice_geom->Append( geomIds[i].second );
			geom_mapping.push_back( geomIds[i].first );

			if ( (int)geomIds[i].first == geomId ) selgeom = i;
		}
		choice_geom->SetSelection( selgeom );
	}

	// set new selection on skin choice
	if ( cfg.skins.find( category ) != cfg.skins.end())
	{
		choice_skin->SetSelection( cfg.skins.at( category ));
	}
	else choice_skin->SetSelection( 0 );
}

/*****************************************************
**
**   PrintoutConflet   ---   OnChoiceGeom
**
******************************************************/
void PrintoutConflet:: OnChoiceGeom( wxCommandEvent& )
{
	const WidgetCategory category = (WidgetCategory)( choice_category->GetSelection() + 1 );
	const int sel = choice_geom->GetSelection();

	debuglog << "PrintoutConflet::OnChoiceGeom category " << category << " selection " << sel;
	assert( category >= WidgetCategory::WesternChart && category <= WidgetCategory::VargaChart );
	assert( geom_mapping.size() == choice_geom->GetCount());

	cfg.geoms[ category ] = geom_mapping[ sel ];
	guiHasChanged();
}

/*****************************************************
**
**   PrintoutConflet   ---   OnChoiceSkin
**
******************************************************/
void PrintoutConflet::OnChoiceSkin( wxCommandEvent& )
{
	const WidgetCategory category = (WidgetCategory)( choice_category->GetSelection() + 1 );
	const int sel = choice_skin->GetSelection();

	debuglog << "PrintoutConflet::OnChoiceSkin category " << category << " selection " << sel;
	assert( category >= WidgetCategory::WesternChart && category <= WidgetCategory::VargaChart );

	cfg.skins[ category ] = sel;
	guiHasChanged();
}

/*****************************************************
**
**   PrintoutConflet   ---   OnPrintTestPage
**
******************************************************/
void PrintoutConflet::OnPrintTestPage( wxCommandEvent& )
{
	TransferDataFromWindow();
	PrintoutConfig p( cfg );

	Horoscope_ptr h( new Horoscope );
	h->update();

	PdfLauncher launcher( p );
	launcher.printTestpage( h, props );
}

/*****************************************************
**
**   PrintoutConflet   ---   restoreDefaults
**
******************************************************/
void PrintoutConflet::restoreDefaults()
{
	cfg = PrintoutConfig();
	updateCategoryDepChoices();
}

