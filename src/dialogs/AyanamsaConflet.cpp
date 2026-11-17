/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AyanamsaConflet.cpp
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

#include "AyanamsaConflet.h"
#include "Calculator.h"
#include "ContextProperties.h"
#include "DateFormatter.h"
#include "Formatter.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "Table.h"
#include "TextValidator.h"
#include "TextWidget.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/choicdlg.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>

extern Astroconfig *astroconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_USE_BUILTIN = wxID_HIGHEST + 1, CD_PRECESSION_MODEL };

/*****************************************************
**
**   PrecessionModelChoice 
**
******************************************************/
class PrecessionModelChoice : public wxChoice
{
public:

	/*****************************************************
	**
	**   PrecessionModelChoice   ---   Constructor
	**
	******************************************************/
	PrecessionModelChoice( wxWindow *parent, const int id, int *cfg )
	: wxChoice( parent, id, wxDefaultPosition, wxDefaultSize ),
	cfg( cfg ),
	max_prec( CalculatorFactory().getCalculator()->getBuiltinPrecessionModelsSize())
	{
		Calculator *calculator = CalculatorFactory().getCalculator();
		for( const auto &a : calculator->getPrecessionModeNames())
		{
			Append( a );
		}
		Append( tr( "Unspecified" ));
		Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PrecessionModelChoice::OnCommand ));
	}

	virtual bool TransferDataToWindow() override
	{
		debuglog << "PrecessionModelChoice::TransferDataToWindow cfg " << *cfg;
		if ( *cfg >= 0 ) SetSelection( *cfg );
		else if ( *cfg == -1 ) SetSelection( max_prec  + 1 );
		else
		{
			warnlog << "PrecessionModelChoice::TransferDataToWindow has wrong value " << *cfg << " using default";
			SetSelection( 0 );
			return false;
		}
		return true;
	}


protected:

	/*****************************************************
	**
	**   PrecessionModelChoice   ---   OnCommand
	**
	******************************************************/
	void OnCommand( wxCommandEvent &event )
	{
		const int sel = GetSelection();
		debuglog << "PrecessionModelChoice::OnCommand sel " << sel;
		if ( sel != max_prec + 1 ) *cfg = sel;
		else *cfg = -1;
		event.Skip();
	}

	int *cfg;	

	const int max_prec;

};

/*****************************************************
**
**   AyanamsaConflet   ---   Constructor
**
******************************************************/
AyanamsaConflet::AyanamsaConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, astroconfig->customAyanamsa )
{
	props = ContextProps_ptr( new ContextProperties );
	lastBuiltinSelection = 0;

	// begin wxGlade: AyanamsaConflet::AyanamsaConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBoxSizer* sizer_aya = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Custom Ayanamsa")), wxVERTICAL);
	sizer_main->Add(sizer_aya, 1, wxALL, 3);
	wxFlexGridSizer* grid_params = new wxFlexGridSizer(4, 3, 3, 3);
	sizer_aya->Add(grid_params, 0, wxEXPAND, 0);
	label_custom_aya_t0 = new wxStaticText(sizer_aya->GetStaticBox(), wxID_ANY, _("Date t0"));
	grid_params->Add(label_custom_aya_t0, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	text_t0 = new wxTextCtrl(sizer_aya->GetStaticBox(), wxID_ANY, wxEmptyString);
	grid_params->Add(text_t0, 1, wxALL|wxEXPAND, 3);
	text_t0_formatted = new wxStaticText(sizer_aya->GetStaticBox(), wxID_ANY, _("0000-00-00 00:00:00"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	grid_params->Add(text_t0_formatted, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	label_custom_aya_ayat0 = new wxStaticText(sizer_aya->GetStaticBox(), wxID_ANY, _("Ayanamsa at t0"));
	grid_params->Add(label_custom_aya_ayat0, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	text_ayan_t0 = new wxTextCtrl(sizer_aya->GetStaticBox(), wxID_ANY, wxEmptyString);
	grid_params->Add(text_ayan_t0, 1, wxALL|wxEXPAND, 3);
	text_ayan_t0_formatted = new wxStaticText(sizer_aya->GetStaticBox(), wxID_ANY, _("00:00:00.000"));
	grid_params->Add(text_ayan_t0_formatted, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	grid_params->Add(20, 20, 0, 0, 0);
	check_to_ut = new wxCheckBox(sizer_aya->GetStaticBox(), wxID_ANY, _("t0 Is UT"));
	grid_params->Add(check_to_ut, 0, wxALL, 3);
	grid_params->Add(20, 20, 0, 0, 0);
	label_precession_model = new wxStaticText(sizer_aya->GetStaticBox(), wxID_ANY, _("Precession Model"));
	grid_params->Add(label_precession_model, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	choice_precession_model = new PrecessionModelChoice(sizer_aya->GetStaticBox(), wxID_ANY, &cfg.precessionModel);
	grid_params->Add(choice_precession_model, 1, wxALL|wxEXPAND, 3);
	grid_params->Add(20, 20, 0, 0, 0);
	wxStaticBoxSizer* sizer_bultin = new wxStaticBoxSizer(new wxStaticBox(sizer_aya->GetStaticBox(), wxID_ANY, _("Fill in Bultin Params")), wxHORIZONTAL);
	sizer_aya->Add(sizer_bultin, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
	button_use_builtin = new wxButton(sizer_bultin->GetStaticBox(), CD_USE_BUILTIN, _("Select ..."));
	sizer_bultin->Add(button_use_builtin, 0, wxALL, 3);
	wxStaticBoxSizer* sizer_preview = new wxStaticBoxSizer(new wxStaticBox(sizer_aya->GetStaticBox(), wxID_ANY, _("Sample Values")), wxVERTICAL);
	sizer_aya->Add(sizer_preview, 1, wxALL|wxEXPAND, 3);
	preview = new TextWidget(sizer_preview->GetStaticBox(), props, WidgetCategory::TextGeneric);
	preview->SetMinSize(wxSize(200, 220));
	sizer_preview->Add(preview, 1, wxALL|wxEXPAND, 3);
	
	grid_params->AddGrowableCol(1);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	text_t0->SetValidator( DoubleValidator( &cfg.t0, MIN_EPHEM_JD, MAX_EPHEM_JD ));
	text_ayan_t0->SetValidator( DegreeValidator( &cfg.ayan_t0, 0, 360 ));

	check_to_ut->SetValidator( wxGenericValidator( &cfg.t0_ut ));
	choice_precession_model->SetValidator( wxGenericValidator( &cfg.precessionModel ));

	Connect( CD_USE_BUILTIN, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler( AyanamsaConflet::OnSelectBuiltin ));
}

/*****************************************************
**
**   AyanamsaConflet   ---   updateUi
**
******************************************************/
void AyanamsaConflet::updateUi()
{
	debuglog << "AyanamsaConflet::updateUi";
	Formatter *formatter = Formatter::get();
	DateFormatter *df = DateFormatter::get();
	Calculator *calculator = CalculatorFactory().getCalculator();

	if ( text_t0 != 0 && ! text_t0->GetValue().IsEmpty())
	{
		double value;
		const bool isvalid = stringToDouble( text_t0->GetValue(), value );
		if ( isvalid )
		{
			wxString t0 = df->formatUTDatetime( value );
			if ( text_t0_formatted ) text_t0_formatted->SetLabel( t0 );
		}
	}
	if ( text_ayan_t0  != 0 && ! text_ayan_t0->GetValue().IsEmpty())
	{
		double value;
		const bool isvalid = stringToDouble( text_ayan_t0->GetValue(), value );
		if ( isvalid )
		{
			wxString ayan_t0 = formatter->formatDegrees( value, DegPrecision::More );
			if ( text_ayan_t0_formatted ) text_ayan_t0_formatted->SetLabel( ayan_t0 );
		}
	}

	Table *table = new Table;
	table->addHeaderRow(
	{
		tr( "Date" ),
		tr( "Ayanamsa" )
	});

	std::list<double> jds = { MDate::getCurrentJD() };
	for( const int year : { 1800, 1900, 2000, 2050, 2100, 2500 } )
	{
		jds.push_back( MDate( year, 1, 1, 0 ).jd );
	}
	jds.sort();
	for( const double jd : jds )
	{
		TableRow row;
		row.addValue( df->formatUTDatetime( jd ));
		row.addValue( formatter->formatDegrees( calculator->calcCustomAyanamsa( jd, cfg ), DegPrecision::More ));
		table->addRow( row );
	}
	preview->clearDoc();
	preview->getDoc()->addItem( table );
	preview->OnDataChanged();
}

/*****************************************************
**
**   AyanamsaConflet   ---   OnSelectBuiltin
**
******************************************************/
void AyanamsaConflet::OnSelectBuiltin( wxCommandEvent& )
{
	Calculator *calculator = CalculatorFactory().getCalculator();
	wxArrayString names;

	for( const auto &name : calculator->getBuiltinAyanamsaNames())
	{
		names.Add( name );
	}
	wxSingleChoiceDialog dialog( this,	tr( "Select Builtin Ayanamsa" ), GUI_APP_NAME, names );
	dialog.SetSelection( lastBuiltinSelection );
	if ( dialog.ShowModal() == wxID_OK )
	{
		cfg = calculator->getBuiltinAyanamsaParams( dialog.GetSelection());
		lastBuiltinSelection = dialog.GetSelection();
		TransferDataToWindow();
		guiHasChanged();
	}
}



