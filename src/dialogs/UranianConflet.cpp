/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/UranianConflet.cpp
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

#include "UranianConflet.h"

#include "CommonControls.h"
#include "ContextProperties.h"
#include "DocHelper.h"
#include "enumserializer.h"
#include "Lang.h"
#include "logger.h"
#include "MatchWriter.h"
#include "Painter.h"
#include "Table.h"
#include "Term.h"
#include "TextWidget.h"
#include "Uranian.h"
#include "UranianHelper.h"

#include <wx/checklst.h>
#include <wx/choice.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

#include <format>

extern Astroconfig *astroconfig;

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class DivisorCheckboxValidator : public wxValidator
{
public:
	DivisorCheckboxValidator( std::set<int> *p ) : primes( p ) {}

	virtual wxObject *Clone() const override
	{
		return new DivisorCheckboxValidator( *this );
	}

	virtual bool TransferFromWindow() override
	{
		wxCheckListBox *cb = wxDynamicCast( GetWindow(), wxCheckListBox );
		assert( cb );
		primes->clear();

		int i = 0;
		for( const auto p : UranianHelper::get()->getAllDivisionPrimes())
		{
			if ( cb->IsChecked( i ))
			{
				primes->insert( p );
				debuglog << "PRIME: " << p;
			}
			//cb->Check( i, primes->find( p ) != primes->end());
			++i;
		}
		return true;
	}

	virtual bool TransferToWindow() override
	{
		wxCheckListBox *cb = wxDynamicCast( GetWindow(), wxCheckListBox );
		assert( cb );

		int i = 0;
		for( const auto p : UranianHelper::get()->getAllDivisionPrimes())
		{
			cb->Check( i, primes->find( p ) != primes->end());
			++i;
		}
		return true;
	}

protected:

	std::set<int> *primes;
};

/*************************************************//**
*
* 
*
******************************************************/
class AntisciaPreviewPanel : public StringPreviewPanel
{
public:
	AntisciaPreviewPanel( wxWindow* parent, int &antisciaCfg ) : StringPreviewPanel( parent ), antisciaCfg( antisciaCfg ) {}

protected:

	virtual MString getStringContents() override
	{
		MTerm term( TermId::AntiSingle );
		term.incarnate( PlanetId::Jupiter );

		return term.firstMString( Viewpoint::Uranian, &antisciaCfg );
	}

	int &antisciaCfg;
};

/*************************************************//**
*
* 
*
******************************************************/
class MatchFormatPreviewPanel : public StringPreviewPanel
{
public:
	MatchFormatPreviewPanel( wxWindow* parent, UranianMatchFormat &formatCfg ) : StringPreviewPanel( parent ), formatCfg( formatCfg ) {}

protected:
	virtual MString getStringContents() override
	{
		Match match( 4, .5, 90 );
		MTerm *term1 = new MTerm( TermId::Single );
		term1->incarnate( PlanetId::Moon );
		match.e1 = new MatchElement( HoroscopeRole::Radix, term1, 2 );

		MTerm *term2 = new MTerm( TermId::Single );
		term2->incarnate( PlanetId::Jupiter );
		match.e2 = new MatchElement( HoroscopeRole::Radix, term2, 90 );

		return MatchFormatter::get()->formatMatch( match, Viewpoint::Uranian, formatCfg );
	}

	UranianMatchFormat &formatCfg;
};

/*****************************************************
**
**   UranianConflet   ---   Constructor
**
******************************************************/
UranianConflet::UranianConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, astroconfig->uranian )
{
	props = ContextProps_ptr( new ContextProperties );

	// begin wxGlade: UranianConflet::UranianConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* sizer_left = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_left, 0, 0, 0);
	wxStaticBoxSizer* sizer_generaloptions = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("General Options")), wxVERTICAL);
	sizer_left->Add(sizer_generaloptions, 0, wxALL|wxEXPAND, 3);
	wxBoxSizer* sizer_antiscia = new wxBoxSizer(wxHORIZONTAL);
	sizer_generaloptions->Add(sizer_antiscia, 1, wxEXPAND, 0);
	wxStaticText* label_antiscia = new wxStaticText(sizer_generaloptions->GetStaticBox(), wxID_ANY, _("Antiscia Prefix"));
	sizer_antiscia->Add(label_antiscia, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_antiscia_choices[] = {
		_("*"),
		_("%"),
		_("@"),
		_("-"),
	};
	choice_antiscia = new wxChoice(sizer_generaloptions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, choice_antiscia_choices);
	choice_antiscia->SetSelection(0);
	sizer_antiscia->Add(choice_antiscia, 1, wxALL|wxEXPAND, 3);
	antisciaPreview = new AntisciaPreviewPanel(sizer_generaloptions->GetStaticBox(), cfg.antisciaDisplayStyle);
	sizer_generaloptions->Add(antisciaPreview, 1, wxEXPAND, 0);
	wxBoxSizer* sizer_matchformat = new wxBoxSizer(wxHORIZONTAL);
	sizer_generaloptions->Add(sizer_matchformat, 1, wxEXPAND, 0);
	wxStaticText* label_match_format = new wxStaticText(sizer_generaloptions->GetStaticBox(), wxID_ANY, _("Match Format"));
	sizer_matchformat->Add(label_match_format, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString *choice_match_format_choices = NULL;
	choice_match_format = new wxChoice(sizer_generaloptions->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, choice_match_format_choices);
	sizer_matchformat->Add(choice_match_format, 1, wxALL, 3);
	matchFormatPreview = new MatchFormatPreviewPanel(sizer_generaloptions->GetStaticBox(), cfg.matchFormat);
	sizer_generaloptions->Add(matchFormatPreview, 1, wxEXPAND, 0);
	wxBoxSizer* sizer_maxmatches = new wxBoxSizer(wxHORIZONTAL);
	sizer_left->Add(sizer_maxmatches, 1, wxEXPAND, 0);
	wxStaticText* label_maxmatches = new wxStaticText(this, wxID_ANY, _("Maximum Number of Matches"));
	sizer_maxmatches->Add(label_maxmatches, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	spin_maxmatches = new wxSpinCtrl(this, wxID_ANY, wxT("100"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10000);
	sizer_maxmatches->Add(spin_maxmatches, 0, wxALL, 3);
	wxStaticBoxSizer* sizer_right = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Division Primes")), wxHORIZONTAL);
	sizer_main->Add(sizer_right, 2, wxEXPAND, 0);
	wxBoxSizer* sizer_primes = new wxBoxSizer(wxVERTICAL);
	sizer_right->Add(sizer_primes, 1, wxEXPAND, 0);
	const wxString check_primes_choices[] = {
		_("dummy"),
	};
	check_primes = new wxCheckListBox(sizer_right->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 1, check_primes_choices);
	check_primes->SetMinSize(wxSize(-1, 200));
	sizer_primes->Add(check_primes, 1, wxALL|wxEXPAND, 3);
	wxBoxSizer* sizer_max = new wxBoxSizer(wxHORIZONTAL);
	sizer_primes->Add(sizer_max, 0, wxALL, 3);
	wxStaticText* label_max = new wxStaticText(sizer_right->GetStaticBox(), wxID_ANY, _("Maximum Division"));
	sizer_max->Add(label_max, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	spin_maximum = new wxSpinCtrl(sizer_right->GetStaticBox(), wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 300);
	sizer_max->Add(spin_maximum, 0, wxALL, 3);
	preview = new TextWidget(sizer_right->GetStaticBox(), props, WidgetCategory::TextGeneric);
	preview->SetMinSize(wxSize(200, 220));
	sizer_right->Add(preview, 1, wxALL|wxEXPAND, 3);
	
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	choice_antiscia->SetValidator( wxGenericValidator( &cfg.antisciaDisplayStyle ));

	choice_match_format->Clear();
	for ( const UranianMatchFormat &f : Enum<UranianMatchFormat>() )
	{
		choice_match_format->Append( enum_to_string( f ));
	}
	choice_match_format->SetValidator( wxGenericValidator( (int*)&cfg.matchFormat ));

	check_primes->Clear();
	for( const auto p : UranianHelper::get()->getAllDivisionPrimes())
	{
		check_primes->Append( std::format( "{}", p ));
	}

	spin_maximum->SetValidator( wxGenericValidator( &cfg.maximumDivision ));
	check_primes->SetValidator( DivisorCheckboxValidator( &cfg.divisionPrimes ));

	spin_maxmatches->SetValidator( wxGenericValidator( (int*)&cfg.maxMatchCount ));

	Connect( wxID_ANY, wxEVT_CHECKLISTBOX, wxCommandEventHandler( UranianConflet::OnCommand ));
	Connect( wxID_ANY, wxEVT_CHECKLISTBOX, wxCommandEventHandler( UranianConflet::OnCommand ));
}

/*****************************************************
**
**   UranianConflet   ---   OnCommand
**
******************************************************/
void UranianConflet::OnCommand( wxCommandEvent &event )
{
	debuglog << "UranianConflet::OnCommand";
	TransferDataFromWindow();
	guiHasChanged();
	event.Skip();
}

/*****************************************************
**
**   UranianConflet   ---   updateUi
**
******************************************************/
void UranianConflet::updateUi()
{
	debuglog << "UranianConflet::updateUi maximumDivision " << cfg.maximumDivision;
	UranianHelper *uh = UranianHelper::get();

	TransferDataFromWindow();

	antisciaPreview->Refresh();
	matchFormatPreview->Refresh();

	preview->clearDoc();
	MDocument_ptr doc = preview->getDoc();

	Table *table = new Table( 2, 1, tr( "Available Divisions" ));
	table->setHeader( 0, tr( "Division" ));
	table->setHeader( 1, tr( "Degrees" ));

	for( const auto d : uh->calculateDivisionsForPrimes( cfg.divisionPrimes, cfg.maximumDivision ))
	{
		debuglog << "DivisorConflet::updateUi division " << d;
		TableRow row;
		row.addValue( uh->divisionDString( d ));
		row.addValue( uh->divisionDegreeString( d ));
		table->addRow( row );
	}
	doc->addItem( table );
	preview->OnDataChanged();
}

