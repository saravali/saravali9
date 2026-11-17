/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/CalculationConflet.cpp
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

#include "CalculationConflet.h"

#include "astroconfig.h"
#include "Calculator.h"
#include "enumserializer.h"
#include "logger.h"
#include "mathbase.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>

extern Astroconfig *astroconfig;

enum { CD_AYANAMSA_TYPE = wxID_HIGHEST + 1 };

// begin wxGlade: ::extracode
// end wxGlade

/*****************************************************
**
**   CalculationConflet   ---   Constructor
**
******************************************************/
CalculationConflet::CalculationConflet( wxWindow *parent, const Viewpoint &viewpoint )
	: ConfletIncarnation( parent, astroconfig->getViewpointConfig( viewpoint ).calculation )
{
	debuglog << "CalculationConflet Viewpoint " << viewpoint;

	// begin wxGlade: CalculationConflet::CalculationConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBoxSizer* sizer_calcoptions = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Calculation Options")), wxVERTICAL);
	sizer_main->Add(sizer_calcoptions, 0, wxALL, 3);
	wxFlexGridSizer* grid_calcoptions = new wxFlexGridSizer(4, 2, 0, 0);
	sizer_calcoptions->Add(grid_calcoptions, 1, wxALL, 3);
	label_calc_node = new wxStaticText(sizer_calcoptions->GetStaticBox(), wxID_ANY, _("Lunar Node"));
	grid_calcoptions->Add(label_calc_node, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_node_choices[] = {
		_("True"),
		_("Mean"),
	};
	choice_node = new wxChoice(sizer_calcoptions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_node_choices);
	choice_node->SetSelection(0);
	grid_calcoptions->Add(choice_node, 0, wxALIGN_RIGHT|wxALL, 3);
	label_calc_house = new wxStaticText(sizer_calcoptions->GetStaticBox(), wxID_ANY, _("House System"));
	grid_calcoptions->Add(label_calc_house, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_house_choices[] = {
		_("dummy"),
		_("dummy"),
	};
	choice_house = new wxChoice(sizer_calcoptions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_house_choices);
	choice_house->SetSelection(0);
	grid_calcoptions->Add(choice_house, 0, wxALIGN_RIGHT|wxALL, 3);
	label_house_start = new wxStaticText(sizer_calcoptions->GetStaticBox(), wxID_ANY, _("Start of Houses"));
	grid_calcoptions->Add(label_house_start, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_house_start_choices[] = {
		_("Cusp"),
		_("Middle (Sandhi)"),
	};
	choice_house_start = new wxChoice(sizer_calcoptions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_house_start_choices);
	choice_house_start->SetSelection(0);
	grid_calcoptions->Add(choice_house_start, 0, wxALIGN_RIGHT|wxALL, 3);
	label_ayanamsa = new wxStaticText(sizer_calcoptions->GetStaticBox(), CD_AYANAMSA_TYPE, _("Ayanamsa"));
	grid_calcoptions->Add(label_ayanamsa, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_ayanamsa_choices[] = {
		_("dummy"),
	};
	choice_ayanamsa = new wxChoice(sizer_calcoptions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 1, choice_ayanamsa_choices);
	choice_ayanamsa->SetSelection(0);
	grid_calcoptions->Add(choice_ayanamsa, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
	
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	choice_node->SetValidator( wxGenericValidator( (int*)&cfg.lunarNodeMode ));
 
	choice_house->Clear();
	for ( const HouseSystem &hs : Enum<HouseSystem>() )
	{
		choice_house->Append( resolveCamelNotation( hs ));
	}

	choice_house->SetValidator( wxGenericValidator( (int*)&cfg.houseSystem ));
	choice_house_start->SetValidator( wxGenericValidator( (int*)&cfg.houseStart ));

	choice_ayanamsa->Clear();
	choice_ayanamsa->Append( tr( "Tropical (None)" ));
	choice_ayanamsa->Append( tr( "Custom" ));
	for( const auto &s : CalculatorFactory().getCalculator()->getBuiltinAyanamsaNames())
	{
		choice_ayanamsa->Append( s );
	}
	choice_ayanamsa->SetValidator( wxGenericValidator( (int*)&cfg.ayanamsa ));
}

/*****************************************************
**
**   CalculationConflet   ---   restoreDefaults
**
******************************************************/
void CalculationConflet::restoreDefaults()
{
	debuglog << "CalculationConflet::restoreDefaults viewpoint " << cfg.getViewpoint();
  cfg = CalculationConfig( cfg.getViewpoint() );
}

