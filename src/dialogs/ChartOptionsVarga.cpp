/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ChartOptionsVarga.cpp
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

#include "ChartOptionsVarga.h"

#include "ChartConfig.h"
#include "ChartGeometry.h"
#include "ContextProperties.h"
#include "logger.h"

#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

// begin wxGlade: ::extracode
// end wxGlade

/*****************************************************
**
**   ChartOptionsVarga   ---   Constructor
**
******************************************************/
ChartOptionsVarga::ChartOptionsVarga( wxWindow *parent, ContextProps_ptr props, ChartConfig *cfg )
	: ChartOptionsPanel( parent, props, cfg )
{
	assert( cfg );

	// begin wxGlade: ChartOptionsVarga::ChartOptionsVarga
	wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer* sizer_vargaoptions = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Varga Chart Options")), wxVERTICAL);
	sizer_main->Add(sizer_vargaoptions, 0, wxALL|wxEXPAND, 3);
	check_arudhas = new wxCheckBox(sizer_vargaoptions->GetStaticBox(), wxID_ANY, _("Show Arudhas"));
	sizer_vargaoptions->Add(check_arudhas, 0, wxALL, 3);
	check_ashtakavarga = new wxCheckBox(sizer_vargaoptions->GetStaticBox(), wxID_ANY, _("Show Ashtakavarga in Transit Chart"));
	sizer_vargaoptions->Add(check_ashtakavarga, 0, wxALL, 3);
	wxGridSizer* grid_options = new wxGridSizer(3, 2, 0, 0);
	sizer_vargaoptions->Add(grid_options, 1, wxEXPAND, 0);
	label_north_style = new wxStaticText(sizer_vargaoptions->GetStaticBox(), wxID_ANY, _("Signs in North Indian Chart"));
	grid_options->Add(label_north_style, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_north_chart_choices[] = {
		_("Ascendant Only"),
		_("Number"),
		_("Short String"),
		_("Symbol"),
	};
	choice_north_chart = new wxChoice(sizer_vargaoptions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, choice_north_chart_choices);
	choice_north_chart->SetSelection(0);
	grid_options->Add(choice_north_chart, 0, wxALL|wxEXPAND, 3);
	label_markup_ascendant = new wxStaticText(sizer_vargaoptions->GetStaticBox(), wxID_ANY, _("Ascendant in South Indian Chart"));
	grid_options->Add(label_markup_ascendant, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_markup_ascendant_choices[] = {
		_("Name"),
		_("Solid Line"),
		_("Dotted Line"),
	};
	choice_markup_ascendant = new wxChoice(sizer_vargaoptions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_markup_ascendant_choices);
	choice_markup_ascendant->SetSelection(0);
	grid_options->Add(choice_markup_ascendant, 0, wxALL|wxEXPAND, 3);
	label_chartcenter = new wxStaticText(sizer_vargaoptions->GetStaticBox(), wxID_ANY, _("Center of Chart"));
	grid_options->Add(label_chartcenter, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_chart_center_choices[] = {
		_("Nothing"),
		_("Division Name"),
		_("Name + Signification"),
		_("D-N"),
		_("D-N + Signification"),
	};
	choice_chart_center = new wxChoice(sizer_vargaoptions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 5, choice_chart_center_choices);
	choice_chart_center->SetSelection(0);
	grid_options->Add(choice_chart_center, 0, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_aspect_options = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Aspect Display")), wxVERTICAL);
	sizer_main->Add(sizer_aspect_options, 0, wxALL|wxEXPAND, 3);
	wxGridSizer* grid_aspects = new wxGridSizer(3, 2, 3, 3);
	sizer_aspect_options->Add(grid_aspects, 0, wxALL|wxEXPAND, 3);
	label_aspect_type = new wxStaticText(sizer_aspect_options->GetStaticBox(), wxID_ANY, _("Preferred Aspect Type"));
	grid_aspects->Add(label_aspect_type, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_aspect_type_choices[] = {
		_("Graha Drishti"),
		_("Rasi Drishti"),
	};
	choice_aspect_type = new wxChoice(sizer_aspect_options->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_aspect_type_choices);
	choice_aspect_type->SetSelection(0);
	grid_aspects->Add(choice_aspect_type, 0, wxALL|wxEXPAND, 3);
	label_minimum_strength = new wxStaticText(sizer_aspect_options->GetStaticBox(), wxID_ANY, _("Minimal Graha Drishti Strength"));
	grid_aspects->Add(label_minimum_strength, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_minimum_strength_choices[] = {
		_("1/4"),
		_("Half"),
		_("3/4"),
		_("Full"),
	};
	choice_minimum_strength = new wxChoice(sizer_aspect_options->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, choice_minimum_strength_choices);
	choice_minimum_strength->SetSelection(0);
	grid_aspects->Add(choice_minimum_strength, 0, wxALL|wxEXPAND, 3);
	label_benefic = new wxStaticText(sizer_aspect_options->GetStaticBox(), wxID_ANY, _("Benefic/Malefic Colors"));
	grid_aspects->Add(label_benefic, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_benefic_choices[] = {
		_("Specific Colors"),
		_("Foreground Color"),
		_("Grey"),
	};
	choice_benefic = new wxChoice(sizer_aspect_options->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_benefic_choices);
	choice_benefic->SetSelection(0);
	grid_aspects->Add(choice_benefic, 0, wxALL|wxEXPAND, 3);
	check_include_asmc = new wxCheckBox(sizer_aspect_options->GetStaticBox(), wxID_ANY, _("Include Angles in Aspects"));
	sizer_aspect_options->Add(check_include_asmc, 0, wxALL, 3);
	
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	check_arudhas->SetValidator( wxGenericValidator( &cfg->showArudhas ));
	check_ashtakavarga->SetValidator( wxGenericValidator( &cfg->showAshtakavarga ));

	choice_north_chart->SetValidator( wxGenericValidator( (int*)&cfg->northIndianSignDisplayType ));
	choice_markup_ascendant->SetValidator( wxGenericValidator( &cfg->southIndianAscendantMarkup ));
	choice_chart_center->SetValidator( wxGenericValidator( (int*)&cfg->centerInfoType ));

	choice_aspect_type->SetValidator( wxGenericValidator( (int*)&cfg->aspectType ));
	choice_minimum_strength->SetValidator( wxGenericValidator( (int*)&cfg->grahaDrishtiMinimumStrength ));
	choice_benefic->SetValidator( wxGenericValidator( &cfg->beneficMaleficDisplay ));
	check_include_asmc->SetValidator( wxGenericValidator( &cfg->includeAsMc ));
}

/*****************************************************
**
**   ChartOptionsVarga   ---   updateUi
**
******************************************************/
void ChartOptionsVarga::updateUi()
{
	debuglog << "ChartOptionsVarga::updateUi";

	const uint gid = props->getGeometry( WidgetCategory::VargaChart );
	const ChartGeometry &geom = ChartGeometryLoader::get()->getConfig( gid );

	const bool isNorthIndianChart = geom.containsElementType( CgElementType::DiamondGrid );
	const bool isSouthIndianChart = geom.containsElementType( CgElementType::SouthGrid );
	const bool isGrahaDrishti = choice_aspect_type->GetSelection() == 0;

	label_north_style->Enable( isNorthIndianChart );
	choice_north_chart->Enable( isNorthIndianChart );

	choice_markup_ascendant->Enable( isSouthIndianChart );
  label_markup_ascendant->Enable( isSouthIndianChart );

	label_minimum_strength->Enable( isGrahaDrishti );
  choice_minimum_strength->Enable( isGrahaDrishti );
}


