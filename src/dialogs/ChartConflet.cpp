/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ChartConflet.cpp
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

#include "ChartConflet.h"

#include "ChartGeometry.h"
#include "ChartOptionsSbc.h"
#include "ChartOptionsUranian.h"
#include "ChartOptionsVarga.h"
#include "ChartOptionsWestern.h"
#include "ChartPreviewPanel.h"
#include "ConfigLoaderImpls.h"
#include "ConfletFactory.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "logger.h"

#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_CHOICE_GEOMETRY = wxID_HIGHEST + 1,
	CD_CHOICE_SKIN,
	CD_CHOICE_PLANETSTYLE,
	CD_SLIDER_FONTSIZE,
	CD_CHOICE_RETROSTYLE,
	CD_ZOOMFACTOR
};


/*****************************************************
**
**   ChartConflet   ---   Constructor
**
******************************************************/
ChartConflet::ChartConflet( wxWindow *parent, const ConfletId &id )
	: Conflet( parent ),
	confletId( id )
{
	props = ContextProps_ptr( new ContextProperties );
	preview = nullptr;
	switch( confletId )
	{
		case ConfletId::Sbc:
			category = WidgetCategory::SarvatobhadraChart;
			config2model();
			props->setPreferredViewpoint( Viewpoint::Vedic );
			optionsPanel = new ChartOptionsSbc( this, props, cfg );
		break;
		case ConfletId::VargaChart:
			category = WidgetCategory::VargaChart;
			config2model();
			props->setPreferredViewpoint( Viewpoint::Vedic );
			optionsPanel = new ChartOptionsVarga( this, props, cfg );
		break;
		case ConfletId::UranianChart:
			category = WidgetCategory::UranianChart;
			config2model();
			props->setPreferredViewpoint( Viewpoint::Uranian );
			optionsPanel = new ChartOptionsUranian( this, props, cfg );
		break;
		case ConfletId::WesternChart:
			category = WidgetCategory::WesternChart;
			config2model();
			props->setPreferredViewpoint( Viewpoint::Western );
			optionsPanel = new ChartOptionsWestern( this, props, cfg );
		break;
		case ConfletId::UniversalChart:
			category = WidgetCategory::UniversalChart;
			config2model();
			// there is no options panel
		break;
		default:
			assert( false );
		break;
	}

	// begin wxGlade: ChartConflet::ChartConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* sizer_left = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_left, 0, wxALL, 3);
	wxStaticBoxSizer* sizer_generalconfig = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("General Configuration")), wxVERTICAL);
	sizer_left->Add(sizer_generalconfig, 0, wxEXPAND, 0);
	wxFlexGridSizer* grid_general = new wxFlexGridSizer(5, 2, 0, 0);
	sizer_generalconfig->Add(grid_general, 0, wxEXPAND, 0);
	wxStaticText* label_geometry = new wxStaticText(sizer_generalconfig->GetStaticBox(), wxID_ANY, _("Geometry"));
	grid_general->Add(label_geometry, 3, wxALIGN_CENTER_VERTICAL|wxALL, 0);
	const wxString choice_geometry_choices[] = {
		_("dummy"),
	};
	choice_geometry = new wxChoice(sizer_generalconfig->GetStaticBox(), CD_CHOICE_GEOMETRY, wxDefaultPosition, wxDefaultSize, 1, choice_geometry_choices);
	grid_general->Add(choice_geometry, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
	wxStaticText* label_skin = new wxStaticText(sizer_generalconfig->GetStaticBox(), wxID_ANY, _("Skin"));
	grid_general->Add(label_skin, 3, wxALIGN_CENTER_VERTICAL|wxALL, 0);
	const wxString choice_skin_choices[] = {
		_("dummy"),
	};
	choice_skin = new wxChoice(sizer_generalconfig->GetStaticBox(), CD_CHOICE_SKIN, wxDefaultPosition, wxDefaultSize, 1, choice_skin_choices);
	grid_general->Add(choice_skin, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
	wxStaticText* label_planetstyle = new wxStaticText(sizer_generalconfig->GetStaticBox(), wxID_ANY, _("Planet Style"));
	grid_general->Add(label_planetstyle, 3, wxALIGN_CENTER_VERTICAL|wxALL, 0);
	const wxString choice_planetstyle_choices[] = {
		_("Short Text"),
		_("Medium Text"),
		_("Long Text"),
		_("Symbol"),
	};
	choice_planetstyle = new wxChoice(sizer_generalconfig->GetStaticBox(), CD_CHOICE_PLANETSTYLE, wxDefaultPosition, wxDefaultSize, 4, choice_planetstyle_choices);
	choice_planetstyle->SetSelection(0);
	grid_general->Add(choice_planetstyle, 0, wxALL|wxEXPAND, 3);
	wxStaticText* label_fontsize = new wxStaticText(sizer_generalconfig->GetStaticBox(), wxID_ANY, _("Font Size"));
	grid_general->Add(label_fontsize, 3, wxALIGN_CENTER_VERTICAL|wxALL, 0);
	slider_fontsize = new wxSlider(sizer_generalconfig->GetStaticBox(), CD_SLIDER_FONTSIZE, 100, 30, 300, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_LABELS);
	grid_general->Add(slider_fontsize, 0, wxALL|wxEXPAND, 3);
	wxStaticText* label_retrotstyle = new wxStaticText(sizer_generalconfig->GetStaticBox(), wxID_ANY, _("Retrogression Style"));
	grid_general->Add(label_retrotstyle, 3, wxALIGN_CENTER_VERTICAL|wxALL, 0);
	const wxString choice_retrostyle_choices[] = {
		_("Nothing"),
		_("Symbol"),
		_("Planet+R"),
		_("(Planet)"),
	};
	choice_retrostyle = new wxChoice(sizer_generalconfig->GetStaticBox(), CD_CHOICE_RETROSTYLE, wxDefaultPosition, wxDefaultSize, 4, choice_retrostyle_choices);
	choice_retrostyle->SetSelection(0);
	grid_general->Add(choice_retrostyle, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_mouseover = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Behavior on Mouseover")), wxVERTICAL);
	sizer_left->Add(sizer_mouseover, 0, wxALL|wxEXPAND, 3);
	check_highlight = new wxCheckBox(sizer_mouseover->GetStaticBox(), wxID_ANY, _("Highlight"));
	sizer_mouseover->Add(check_highlight, 0, wxALL, 3);
	wxBoxSizer* sizer_zoom = new wxBoxSizer(wxHORIZONTAL);
	sizer_mouseover->Add(sizer_zoom, 0, 0, 0);
	check_zoom = new wxCheckBox(sizer_mouseover->GetStaticBox(), wxID_ANY, _("Zoom"));
	sizer_zoom->Add(check_zoom, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	spin_zoomfactor = new wxSpinCtrl(sizer_mouseover->GetStaticBox(), CD_ZOOMFACTOR, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 300);
	sizer_zoom->Add(spin_zoomfactor, 1, wxALL|wxEXPAND, 3);
	label_percent = new wxStaticText(sizer_mouseover->GetStaticBox(), wxID_ANY, _("%"));
	sizer_zoom->Add(label_percent, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxBoxSizer* sizer_right = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_right, 1, wxEXPAND, 0);
	preview = new ChartPreviewPanel(this, props, cfg, category);
	sizer_right->Add(preview, 1, wxEXPAND, 0);
	
	grid_general->AddGrowableCol(1);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	if ( optionsPanel )
	{
		sizer_left->Add( optionsPanel, 1, wxEXPAND, 0);
	}

	// create data model and setup geom choice
	geomModel = ChartGeometryLoader::get()->getAllIdsAndNames( category );

	choice_geometry->Clear();
	for( const auto &a : geomModel )
	{
		debuglog << "ChartConflet geom id " << a.first << " name " << a.second;
		choice_geometry->Append( a.second );
	}

	choice_skin->Clear();
	for ( const auto &name : WidgetSkinLoader::get()->getNames())
	{
		choice_skin->Append( name );
	}
	setupGeomsAndSkins();

	// validators
	choice_planetstyle->SetValidator( wxGenericValidator( (int*)&cfg->textFormat ));
	slider_fontsize->SetValidator( wxGenericValidator( &cfg->relativeFontSize ));
	choice_retrostyle->SetValidator( wxGenericValidator( (int*)&cfg->retroStyle ));

	check_zoom->SetValidator( wxGenericValidator( &cfg->zoomOnMouseOver ));
	spin_zoomfactor->SetValidator( wxGenericValidator( &cfg->zoomFactor ));
	check_highlight->SetValidator( wxGenericValidator( &cfg->highlightOnMouseOver ));

	Connect( CD_CHOICE_GEOMETRY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ChartConflet::OnChoiceGeometry ));
	Connect( wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ChartConflet::OnCommand ));

	Connect( wxID_ANY, wxEVT_SLIDER, wxCommandEventHandler( ChartConflet::OnCommand ));
	Connect( wxEVT_MOTION, wxMouseEventHandler( ChartConflet::OnMouseMotion ));
}

/*****************************************************
**
**   ChartConflet   ---   Destructor
**
******************************************************/
ChartConflet::~ChartConflet()
{
	delete cfg;
}

/*****************************************************
**
**   ChartConflet   ---   setupGeomsAndSkins
**
******************************************************/
void ChartConflet::setupGeomsAndSkins()
{
	debuglog << "ChartConflet::setupGeomsAndSkins category" << category;

	int sel = 0;
	const uint geom = props->getGeometry( category );

	for( uint i = 0; i < geomModel.size(); i++ )
	{
		if ( geomModel[i].first == geom ) sel = i;
	}
	choice_geometry->SetSelection( sel );

	choice_skin->SetSelection( props->getSkin( category ));
	debuglog << "ChartConflet::setupGeomsAndSkins geom " << geom << " sel " << sel;
}

/*****************************************************
**
**   ChartConflet   ---   OnCommand
**
******************************************************/
void ChartConflet::OnCommand( wxCommandEvent &event )
{
	debuglog << "ChartConflet::OnCommand ";

	const uint geom = choice_geometry->GetSelection();
	assert( geom < geomModel.size());
	props->setGeometry( category, geomModel[geom].first );

	const uint skin = choice_skin->GetSelection();
	props->setSkin( category, skin );

	cfg->relativeFontSize = slider_fontsize->GetValue();

	preview->OnDataChanged();
	Conflet::OnCommand( event );
}

/*****************************************************
**
**   ChartConflet   ---   OnChoiceGeometry
**
******************************************************/
void ChartConflet::OnChoiceGeometry( wxCommandEvent& )
{
	const uint geom = choice_geometry->GetSelection();
	debuglog << "ChartConflet::OnChoiceGeometry geom " << geom;
}

/*****************************************************
**
**   ChartConflet   ---   OnMouseMotion
**
******************************************************/
void ChartConflet::OnMouseMotion( wxMouseEvent& )
{
	debuglog << "ChartConflet::OnMouseMotion";
}

/*****************************************************
**
**   ChartConflet   ---   config2model
**
******************************************************/
void ChartConflet::config2model()
{
	debuglog << "ChartConflet::config2model";
	if ( cfg ) delete cfg;
	switch( category )
	{
		case WidgetCategory::WesternChart:
			cfg = new ChartConfig( gdiconfig->chartWestern );
		break;
		case WidgetCategory::UranianChart:
			cfg = new ChartConfig( gdiconfig->chartUranian );
		break;
		case WidgetCategory::VargaChart:
			cfg = new ChartConfig( gdiconfig->chartVarga );
		break;
		case WidgetCategory::SarvatobhadraChart:
			cfg = new ChartConfig( gdiconfig->chartSbc );
		break;
		case WidgetCategory::UniversalChart:
			cfg = new ChartConfig( gdiconfig->chartUniversal );
		break;
		default:
			assert( false );
		break;
	}
	if ( preview ) preview->setChartConfig( cfg );
}

/*****************************************************
**
**   ChartConflet   ---   model2config
**
******************************************************/
void ChartConflet::model2config()
{
	debuglog << "ChartConflet::model2config";
	gdiconfig->geoms[ category ] = props->getGeometry( category );
	gdiconfig->skins[ category ] = props->getSkin( category );

	switch( category )
	{
		case WidgetCategory::WesternChart:
			gdiconfig->chartWestern = *cfg;
		break;
		case WidgetCategory::UranianChart:
			gdiconfig->chartUranian = *cfg;
		break;
		case WidgetCategory::VargaChart:
			gdiconfig->chartVarga = *cfg;
		break;
		case WidgetCategory::SarvatobhadraChart:
			gdiconfig->chartSbc = *cfg;
		break;
		case WidgetCategory::UniversalChart:
			gdiconfig->chartUniversal = *cfg;
		break;
		default:
			assert( false );
		break;
	}
}

/*****************************************************
**
**   ChartConflet   ---   updateUi
**
******************************************************/
void ChartConflet::updateUi()
{
	TransferDataFromWindow();
	debuglog << "ChartConflet::updateUi";

	if ( optionsPanel ) optionsPanel->updateUi();
	preview->OnDataChanged();
}

/*****************************************************
**
**   ChartConflet   ---   restoreDefaults
**
******************************************************/
void ChartConflet::restoreDefaults()
{
	debuglog << "ChartConflet::restoreDefaults";

	props->setGeometry( category, 0 );
	props->setSkin( category, 0 );

	*cfg = ChartConfig( category );

	props->setGeometry( category, geomModel[0].first );
	setupGeomsAndSkins();
	TransferDataToWindow();
	updateUi();
}

