/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/UranianControls.cpp
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

#include "UranianControls.h"

#include "astroconfig.h"
#include "Commands.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "Formulary.h"
#include "guibase.h"
#include "ImageProvider.h"
#include "Lang.h"
#include "logger.h"
#include "Planet.h"
#include "PlanetFilterDialog.h"
#include "UranianHelper.h"

#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>

extern Astroconfig *astroconfig;

// Transit Mode Panel: caution TMPANEL_ENABLED + n roles are already occupied
enum
{
	TMPANEL_ENABLED = wxID_HIGHEST + 1700
};

#define SIDEBAR_FILTER_EXCLUDE_LIST  { FilterGroup::Angles, FilterGroup::Houses }

/*****************************************************
**
**   SortOrderChoice   ---   Constructor
**
******************************************************/
SortOrderChoice::SortOrderChoice( wxWindow *parent, const int &id, const UranianContext &ctx,
	EventSortOrder *sortOrder )
	: wxChoice( parent, id, wxDefaultPosition, wxSize( 110, -1 )),
	order( sortOrder )
{
	switch( ctx )
	{
		case UranianContext::EventFinder:
			model =
			{
				EventSortOrder::Date,
				EventSortOrder::DateReverse,
				EventSortOrder::LeftPlanet,
				EventSortOrder::RightPlanet,
				EventSortOrder::Type,
				EventSortOrder::TypeReverse,
				EventSortOrder::Speed,
				EventSortOrder::SpeedReverse
			};
		break;
		case UranianContext::RadixPp:
		case UranianContext::DirectionPp:
		case UranianContext::PartnerPp:
			model =
			{
				EventSortOrder::Longitude,
				EventSortOrder::Term,
				EventSortOrder::Score
			};
		break;
		case UranianContext::DirectionMatches:
		case UranianContext::MultiDirectionMatches:
			model =
			{
				EventSortOrder::Date,
				EventSortOrder::DateReverse,
				EventSortOrder::LeftPlanet,
				EventSortOrder::RightPlanet,
				EventSortOrder::Orbis,
				EventSortOrder::OrbisAbsolute,
				EventSortOrder::OrbisReverse,
				EventSortOrder::Type,
				EventSortOrder::TypeReverse,
				EventSortOrder::Speed,
				EventSortOrder::SpeedReverse,
			};
		break;
		default:
			model =
			{
				EventSortOrder::LeftPlanet,
				EventSortOrder::RightPlanet,
				EventSortOrder::Orbis,
				EventSortOrder::OrbisAbsolute,
				EventSortOrder::OrbisReverse,
				EventSortOrder::Type,
				EventSortOrder::TypeReverse,
				EventSortOrder::Speed,
				EventSortOrder::SpeedReverse,
				EventSortOrder::Date,
				EventSortOrder::DateReverse,
			};
		break;
	}
	SetToolTip( _( "Sort Order" ));
	uint sel = 0;
	for( uint i = 0; i < model.size(); i++ )
	{
		switch( model[i] )
		{
			case EventSortOrder::RightPlanet:
				Append( tr( "Right Side" ));
			break;
			case EventSortOrder::LeftPlanet:
				Append( tr( "Left Side" ));
			break;
			case EventSortOrder::Orbis:
				Append( tr( "Orbis" ));
			break;
			case EventSortOrder::OrbisAbsolute:
				Append( tr( "Orbis Absolute" ));
			break;
			case EventSortOrder::OrbisReverse:
				Append( tr( "Orbis Reverse" ));
			break;
			case EventSortOrder::Type:
				Append( tr( "Division" ));
			break;
			case EventSortOrder::TypeReverse:
				Append( tr( "Division Reverse" ));
			break;
			case EventSortOrder::Speed:
				Append( tr( "Speed" ));
			break;
			case EventSortOrder::SpeedReverse:
				Append( tr( "Speed Reverse" ));
			break;
			case EventSortOrder::Date:
				Append( tr( "Due Date" ));
			break;
			case EventSortOrder::DateReverse:
				Append( tr( "Due Date Reverse" ));
			break;
			case EventSortOrder::Longitude:
				Append( tr( "Longitude" ));
			break;
			case EventSortOrder::Term:
				Append( tr( "Term" ));
			break;
			case EventSortOrder::Score:
				Append( tr( "Score" ));
			break;
			default:
				assert( false );
			break;
		}
		if ( order && model[i] == *order ) sel = i;
	}
	SetSelection( sel );
	Connect( wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SortOrderChoice::OnSelection ));
}

/*****************************************************
**
**   SortOrderChoice   ---   OnSelection
**
******************************************************/
void SortOrderChoice::OnSelection( wxCommandEvent &event )
{
	const int sel = GetSelection();
	debuglog << "SortOrderChoice::OnSelection: selected " << sel << " model size " << model.size();
	if ( order )
	{
		if ( sel >= 0 && sel < (int)model.size()) *order = model[sel];
		else *order = EventSortOrder::None;
	}
	event.Skip();
}

/*****************************************************
**
**   TextOutputField   ---   Constructor
**
******************************************************/
TextOutputField::TextOutputField( wxWindow *parent, const int &id, const wxString &s )
	: wxPanel( parent, id, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL)
{
	wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );

	label = new wxStaticText( this, wxID_ANY, s );
	setLabel( "dummy" );
	sizer->Add( label, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL, 3 );
	SetSizer( sizer );
}

/*****************************************************
**
**   TextOutputField   ---   setLabel
**
******************************************************/
void TextOutputField::setLabel( const wxString&s )
{
	label->SetLabel( s );
}

/*****************************************************
**
**   FilterPanel   ---   Constructor
**
******************************************************/
FilterPanel::FilterPanel( wxWindow *parent, const int &id, ContextProps_ptr props, const UranianContext &ctx, MatchFilter &cfg,
	const HoroscopeRole role, const bool enabled )
	: wxPanel( parent, id ),
	props( props ),
	ctx( ctx ),
	filterCfg( cfg ),
	role( role )
{
	ImageProvider *ip = ImageProvider::get();
	debuglog << "FilterPanel: id " << id << " enabled " << enabled << " planet size " << cfg.selectedPlanets.size();

	wxBoxSizer* sizer_filter = new wxBoxSizer( wxVERTICAL);
	wxBoxSizer* sizer_buttons = new wxBoxSizer( wxHORIZONTAL );

	if ( role != HoroscopeRole::None )
	{
		check_enabled = new wxCheckBox( this, TMPANEL_ENABLED, enum_to_string( role ));
		check_enabled->SetValue( enabled );
		sizer_buttons->Add( check_enabled, 1, wxALL | wxALIGN_CENTER_VERTICAL, 3);
	}
	else
		sizer_buttons->Add( new wxStaticText( this, wxID_ANY, tr( "Set filter" )), 1, wxALL | wxALIGN_CENTER_VERTICAL, 3);

	button_filter = new wxBitmapButton( this, (int)CommandId::Filter, wxNullBitmap);
	button_filter->SetMinSize(wxSize(32, 32));
	button_filter->SetBitmapLabel( ip->getBitmap( BitmapId::Filter, IconSize::Medium ));
	button_filter->SetToolTip( tr( "Configure Filter" ));
	sizer_buttons->Add( button_filter, 0, wxALL | wxALIGN_RIGHT, 3);

	button_clear_filter = new wxBitmapButton( this, (int)CommandId::ClearFilter, wxNullBitmap);
	button_clear_filter->SetMinSize(wxSize(32, 32));
	button_clear_filter->SetBitmapLabel( ip->getBitmap( BitmapId::Delete, IconSize::Medium ));
	button_clear_filter->SetToolTip( tr( "Clear Filter" ));
	sizer_buttons->Add( button_clear_filter, 0, wxALL | wxALIGN_RIGHT, 3);

	sizer_filter->Add( sizer_buttons, 1, wxEXPAND, 0 );

	label_filter = new TextOutputField( this, wxID_ANY, tr( "Off" ));
	sizer_filter->Add( label_filter, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL, 3 );
	SetSizer( sizer_filter );
	enableFilterElements( enabled );

	Connect( TMPANEL_ENABLED, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FilterPanel::OnCheckEnabled ));
	Connect( (int)CommandId::Filter, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterPanel::OnFilter ));
	Connect( (int)CommandId::ClearFilter, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterPanel::OnClearFilter ));

	updateFilterLabel();
}

/*****************************************************
**
**   FilterPanel   ---   isEnabled
**
******************************************************/
bool FilterPanel::isEnabled() const
{
	debuglog << "FilterPanel::isEnabled " << check_enabled->IsChecked();
	assert( check_enabled );
	return check_enabled->IsChecked();
}

/*****************************************************
**
**   FilterPanel   ---   OnCheckEnabled
**
******************************************************/
void FilterPanel::OnCheckEnabled( wxCommandEvent &event )
{
	debuglog << "FilterPanel::OnCheckBox id " << event.GetId() << " Enabled " << event.IsChecked();
	//event.SetInt( (int)role );
	event.SetExtraLong( (long)role );
	event.Skip();
}

/*****************************************************
**
**   FilterPanel   ---   enableFilterElements
**
******************************************************/
void FilterPanel::enableFilterElements( const bool b )
{
	debuglog << "FilterPanel::enableFilterElements " << std::boolalpha << b;
	button_filter->Enable( b );
	button_clear_filter->Enable( b );

	if ( ! label_filter ) return;
	label_filter->Enable( b );
}

/*****************************************************
**
**   FilterPanel   ---   OnFilter
**
******************************************************/
void FilterPanel::OnFilter( wxCommandEvent& )
{
	debuglog << "FilterPanel::OnFilter ctx " << ctx << " role " << role;

	PlanetFilterDialog dialog( this, props, ctx, role, filterCfg );
	if ( dialog.ShowModal() == wxID_OK )
	{
		filterCfg = dialog.getFilter();
		updateFilterLabel();
		emitChangeEvent();
	}
}

/*****************************************************
**
**   FilterPanel   ---   OnClearFilter
**
******************************************************/
void FilterPanel::OnClearFilter( wxCommandEvent& )
{
	filterCfg.selectedPlanets.clear();
	updateFilterLabel();
	emitChangeEvent();
}

/*****************************************************
**
**   FilterPanel   ---   emitChangeEvent
**
******************************************************/
void FilterPanel::emitChangeEvent()
{
	wxCommandEvent e( COMMAND_FILTER_CHANGED, GetId());
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   FilterPanel   ---   updateFilterLabel
**
******************************************************/
void FilterPanel::updateFilterLabel()
{
	UranianHelper *uh = UranianHelper::get();
	label_filter->setLabel( uh->getFilterStringRepresentation( filterCfg ));
}

enum { RP_FIRSTPANEL = wxID_HIGHEST + 1000 };

/*****************************************************
**
**   RestrictorPanel   ---   Constructor
**
******************************************************/
RestrictorPanel::RestrictorPanel( wxWindow *parent, ContextProps_ptr props, const UranianContext &ctx )
	: wxPanel( parent, -1 ),
	props( props ),
	ctx( ctx )
{
	switch( ctx )
	{
		case UranianContext::EventFinder:
			restrictors = &astroconfig->directions.eventFinderItems;
		break;
		case UranianContext::MultiDirectionMatches:
		case UranianContext::MultiDirectionChart:
			restrictors = &astroconfig->getUranianSection( ctx ).roleRestrictors;
		break;
		default:
		break;
	}
	assert( restrictors );

	wxBoxSizer* sizer_modes = new wxBoxSizer(wxVERTICAL);
	if ( restrictors->size() == 0 )
	{
		sizer_modes->Add( new wxStaticText( this, wxID_ANY, "Error: no restrictors" ), 1, wxEXPAND );
	}
	
	int id = RP_FIRSTPANEL;
	for( auto &r : *restrictors )
	{
		debuglog << "RestrictorPanel::RestrictorPanel add MultiFilter item " << r.role;
		FilterPanel *panel = new FilterPanel( this, id++, props, ctx, r.filter, r.role, r.enabled );
		panels[ r.role] = panel;
		sizer_modes->Add( panel, 1, wxEXPAND );
	}

	SetSizer( sizer_modes );
	Connect( TMPANEL_ENABLED, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RestrictorPanel::OnCheckEnabled ));
}

/*****************************************************
**
**   RestrictorPanel   ---   hasActiveProgressions
**
******************************************************/
bool RestrictorPanel::hasActiveProgressions() const
{
	debuglog << "RestrictorPanel::hasActiveProgressions";
	auto r = panels.find( HoroscopeRole::Progressive );
	if ( r != panels.end())
	{
		return r->second->isEnabled();
	}
	return false;
}

/*****************************************************
**
**   RestrictorPanel   ---   OnCheckEnabled
**
******************************************************/
void RestrictorPanel::OnCheckEnabled( wxCommandEvent &event )
{
	const bool checked = event.IsChecked();
	const HoroscopeRole role = (HoroscopeRole)( event.GetExtraLong() );
	debuglog << "RestrictorPanel::OnCheckEnabled role " << role << " id " << event.GetId() <<  " checked " << checked;

	auto r = std::find_if( restrictors->begin(), restrictors->end(),
		[role]( RoleRestrictor &restrictor ) { return restrictor.role == role; } );
	if ( r != restrictors->end())
	{
		r->enabled = checked;
	}
	panels[ role ]->enableFilterElements( checked );
	wxCommandEvent e( COMMAND_FILTER_CHANGED, GetId());
	wxPostEvent( GetParent(), e );
	//event.Skip();
}

/*****************************************************
**
**   DivisionChoice   ---   Constructor
**
******************************************************/
DivisionChoice::DivisionChoice( wxWindow *parent, const int &id )
	: wxChoice( parent, id, wxDefaultPosition, wxDefaultSize ),
	divisions( UranianHelper::get()->getAllDivisions())
{
	SetToolTip( tr( "Division" ));
	UranianHelper *uh = UranianHelper::get();

	int sel = 0;
	for( int i = 0; i < (int)divisions.size(); ++i )
	{
		Append( uh->divisionFullString( divisions[i] ));
	}
	SetSelection( sel );
}

/*****************************************************
**
**   DivisionChoice   ---   setDivision
**
******************************************************/
void DivisionChoice::setDivision( const int &d )
{
	for( int i = 0; i < (int)divisions.size(); ++i )
	{
		if ( divisions[i] == d )
		{
			SetSelection( i );
			return;
		}
	}
	warnlog << "DivisionChoice::setDivision has invalid value " << d;
	SetSelection( 0 );
}

/*****************************************************
**
**   DivisionChoice   ---   getDivision
**
******************************************************/
int DivisionChoice::getDivision() const
{
	const int sel = GetSelection();
	assert( sel >= 0 && sel < (int)divisions.size());
	return divisions[ sel ];
}

/*****************************************************
**
**   UranianHouseChoice   ---   Constructor
**
******************************************************/
UranianHouseChoice::UranianHouseChoice( wxWindow *parent, const int &id )
	: wxChoice( parent, id, wxDefaultPosition, wxDefaultSize )
{
	Lang lang;
	planets.clear();
	planets.push_back( PlanetId::Earth );

	for( const auto &p : PlanetHelper::expandPlanets( Viewpoint::Uranian ))
	{
		if ( PlanetHelper::isCardinalPoint( p )) continue;
		if ( PlanetHelper::isHouse( p )) continue;

		planets.push_back( p );
	}
	for( const auto p: planets )
  {
    Append( lang.getPlanetName( p, Viewpoint::Uranian, TextFormat::Long ));
  }
}

/*****************************************************
**
**   UranianHouseChoice   ---   getSelectedPlanet
**
******************************************************/
PlanetId UranianHouseChoice::getSelectedPlanet() const
{
	const int sel = GetSelection();
	assert( sel >= 0 && sel < (int)planets.size());
	return planets[sel];
}

/*****************************************************
**
**   UranianHouseChoice   ---   setSelectedPlanet
**
******************************************************/
void UranianHouseChoice::setSelectedPlanet( const PlanetId &ob )
{
	assert( planets.size() == GetCount());

	for( uint i = 0; i < planets.size(); ++i )
	{
		if ( planets[i] == ob )
		{
			SetSelection( i );
			return;
		}
	}
	warnlog << "UranianHouseChoice::setSelectedPlanet cannot select planet " << ob;
}

/*************************************************//**
*
**   FormularyPanel   ---   Constructor
*
******************************************************/
FormularyPanel::FormularyPanel( wxWindow *parent, const int &id, const UranianContext &ctx, std::list<int> &formulae )
	: wxPanel( parent, id, wxDefaultPosition, wxDefaultSize ),
	formulae( formulae )
 {
	wxStaticBoxSizer* sizer_main = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Formulae")), wxVERTICAL);
	//wxBoxSizer* sizer_main = new wxBoxSizer( wxVERTICAL);
	const std::vector<wxString> &names =  Formulary::get()->getFormulaNames( ctx, TextFormat::Long );

	for( uint id = 0; id < names.size(); id++ )
	{
		wxCheckBox *box = new wxCheckBox( this, id_check + id, names[id] );
		box->SetValue( std::find( formulae.begin(), formulae.end(), id ) != formulae.end());
		boxes.push_back( box );
		sizer_main->Add( box, 0, wxEXPAND );
	}

	SetSizer(sizer_main);
	sizer_main->Fit(this);

	Connect( wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FormularyPanel::OnCheckBox ));
}

/*************************************************//**
*
**   FormularyPanel   ---   OnCheckBox
*
******************************************************/
void FormularyPanel::OnCheckBox( wxCommandEvent &event )
{
	debuglog << "FormularyPanel::OnCheckBox id " << event.GetId() << " int " << event.GetInt();
	const int id = event.GetId() - id_check;
	const auto a = std::find( formulae.begin(), formulae.end(), id );
	if ( a != formulae.end() && event.GetInt() == 0 )
	{
		formulae.erase( a );
	}
	else if ( event.GetInt() == 1 )
	{
		formulae.push_back( id );
	}
	wxCommandEvent e( COMMAND_FORMULA_CHANGED, GetId());
	wxPostEvent( GetParent(), e );
	event.Skip();
}

/*****************************************************
**
**   PeriodDurationChoice   ---   Constructor
**
******************************************************/
PeriodDurationChoice::PeriodDurationChoice( wxWindow *parent, int id )
	: wxChoice( parent, id )
{
	SetToolTip( tr( "Duration" ));
	Append( "1 day" );
	Append( "1 week" );
	Append( "1 month" );
	Append( "3 months" );
	Append( "1 year" );
	Append( "10 years" );
}

/*****************************************************
**
**   PeriodDurationChoice   ---   getDuration
**
******************************************************/
double PeriodDurationChoice::getDuration() const
{
	switch( GetSelection())
	{
		case 0: return 1; break;
		case 1: return 7; break;
		case 2: return 31; break;
		case 3: return 92; break;
		case 5: return 3650; break;
		case 4:
		default:
			return 365;
		break;
	}
}

/*****************************************************
**
**   SpTermChoice   ---   Constructor
**
******************************************************/
SpTermChoice::SpTermChoice( wxWindow *parent, const int id )
	: wxChoice( parent, id ),
	termIds(
    {
      TermId::Single,
      TermId::AntiSingle,
      TermId::Midpoint,
      TermId::Sum,
      TermId::Difference,
      TermId::AntiMidpoint,
      TermId::ReflectionPoint,
      TermId::Triple,
      TermId::TripleSum
    })
{
	Formulary *formulary = Formulary::get();
	for( const auto id : termIds )
	{
		Append( formulary->getTermName( id ));
	}
	SetSelection( 0 );
}

/*****************************************************
**
**   SpTermChoice   ---   setSelectedTerm
**
******************************************************/
void SpTermChoice::setSelectedTerm( const TermId &termId )
{
	for( uint i = 0; i < termIds.size(); i++ )
	{
		debuglog << "SpTermChoice::setSelectedTerm i " << i << " termIds[i] " << (int)termIds[i] 
			<< " termid " << (int)termId;
		if ( termIds[i] == termId )
		{
			SetSelection( i );
			return;
		}
	}
	SetSelection( 0 );
}

/*****************************************************
**
**   SpTermChoice   ---   getSelectedTermId
**
******************************************************/
TermId SpTermChoice::getSelectedTermId() const
{
	const int sel = GetSelection();
	assert( sel >= 0 && sel < (int)termIds.size());
	return termIds[sel];
}

/*****************************************************
**
**   SpObjectSelectionChoice   ---   Constructor
**
******************************************************/
SpObjectSelectionChoice::SpObjectSelectionChoice( wxWindow *parent, int id )
	: wxChoice( parent, id, wxDefaultPosition )
{
}

/*****************************************************
**
**   SpObjectSelectionChoice   ---   initPlanets
**
******************************************************/
void SpObjectSelectionChoice::initPlanets( const Viewpoint &viewpoint )
{
	Lang lang;
	obs.clear();

	for( const auto &a : astroconfig->getViewpointConfig( Viewpoint::Uranian ).planets.items )
	{
		if ( a.second ) 
		{
			Append( lang.getPlanetName( a.first, viewpoint, TextFormat::Long ));
			obs.push_back( a.first );
		}
	}
	SetSelection( 0 );
	debuglog << "SpObjectSelectionChoice::initPlanets viewpoint " << viewpoint << " size " << obs.size();
}

/*****************************************************
**
**   SpObjectSelectionChoice   ---   selectObject
**
******************************************************/
void SpObjectSelectionChoice::selectObject( const PlanetId &ob )
{
	for( uint i = 0; i < obs.size(); i++ )
	{
		if ( obs[i] == ob ) SetSelection( i );
		return;
	}
	SetSelection( 0 );
}

/*****************************************************
**
**   SpObjectSelectionChoice   ---   getSelectedPlanetId
**
******************************************************/
PlanetId SpObjectSelectionChoice::getSelectedPlanetId() const
{
	const int sel = GetSelection();
	if( sel >= 0 && sel < (int)obs.size())
	{
		return obs[sel];
	}
	warnlog << "SpObjectSelectionChoice::getSelectedPlanetId has no match, ob size " << obs.size();
	return PlanetId::None;
}

/*****************************************************
**
**   PartnerShiftPanel   ---   Constructor
**
******************************************************/
PartnerShiftPanel::PartnerShiftPanel( wxWindow *parent, const int &id, UranianSection *section )
	: wxPanel( parent, id ),
	usection( section ),
	planets( {
		PlanetId::Aries,
		PlanetId::Meridian,
		PlanetId::Sun,
		PlanetId::Ascendant,
		PlanetId::Moon,
		PlanetId::AscNode,
		PlanetId::Mercury,
		PlanetId::Venus,
		PlanetId::Mars,
		PlanetId::Jupiter,
		PlanetId::Saturn
		} )
{
	assert( usection );
	const Viewpoint viewpoint = astroconfig->general.defaultViewpoint;
	Lang lang;

	wxStaticBoxSizer* sizer = new wxStaticBoxSizer( wxVERTICAL, this, tr( "Shift  Planet to Origin" ));

	check_active = new wxCheckBox( this, wxID_ANY, tr( "Enable" ));
	check_active->SetValue( usection->enablePlanetShift );
	sizer->Add( check_active, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL, 3 );

	wxFlexGridSizer* grid = new wxFlexGridSizer( 2, 2, 0, 0);

	label1 = new wxStaticBox( this, wxID_ANY, tr( "Partner 1" ));
	grid->Add( label1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3 );
	choice_partner1 = new wxChoice( this, -1 );
	grid->Add( choice_partner1, 0, wxALL, 3 );

	label2 = new wxStaticBox( this, wxID_ANY, tr( "Partner 2" ));
	grid->Add( label2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3 );
	choice_partner2 = new wxChoice( this, -1 );
	grid->Add( choice_partner2, 0, wxALL, 3 );

	sizer->Add( grid, 0, wxALL, 3 );

	for( const auto p : planets )
	{
		choice_partner1->Append( lang.getPlanetName( p, viewpoint ));
		choice_partner2->Append( lang.getPlanetName( p, viewpoint ));
	}
	SetToolTip( tr( "Origin" ));

	//check_active->SetValidator( wxGenericValidator( &usection->enablePartnerShift ));
	check_active->SetValidator( wxGenericValidator( &usection->enablePlanetShift ));

	choice_partner1->SetSelection( 0 );
	choice_partner2->SetSelection( 0 );
	SetSizer( sizer );

	Connect( wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PartnerShiftPanel::OnCheckBox ));
	Connect( wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PartnerShiftPanel::OnChoice ));
	updateUi();

}

/*****************************************************
**
**   PartnerShiftPanel   ---   planetIdToSelection
**
******************************************************/
int PartnerShiftPanel::planetIdToSelection( const PlanetId &p )
{
	for( uint i = 0; i < planets.size(); i++ )
	{
		if ( planets[i] == p )
		{
			return i;
		}
	}
	return 0;
}

/*****************************************************
**
**   PartnerShiftPanel   ---   OnCheckBox
**
******************************************************/
void PartnerShiftPanel::OnCheckBox( wxCommandEvent& )
{
	updateUi();
	emitChangeEvent();
}

/*****************************************************
**
**   PartnerShiftPanel   ---   OnChoice
**
******************************************************/
void PartnerShiftPanel::OnChoice( wxCommandEvent& )
{
	assert( usection );
	const uint sel1 = choice_partner1->GetSelection();
	if ( sel1 < planets.size())
	{
		usection->shiftPlanetIds[ HoroscopeRole::Partner1 ] = planets[sel1];
	}
	const uint sel2 = choice_partner2->GetSelection();
	if ( sel2 < planets.size())
	{
		usection->shiftPlanetIds[ HoroscopeRole::Partner2 ] = planets[sel2];
	}
	emitChangeEvent();
}

/*****************************************************
**
**   PartnerShiftPanel   ---   emitChangeEvent
**
******************************************************/
void PartnerShiftPanel::emitChangeEvent()
{
	wxCommandEvent e( COMMAND_SIDEPANEL_CHANGED, GetId());
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   PartnerShiftPanel   ---   updateUi
**
******************************************************/
void PartnerShiftPanel::updateUi()
{
	assert( check_active );
	const bool b = check_active->IsChecked();
	usection->enablePlanetShift = b;
	debuglog << "PartnerShiftPanel::updateUi checked " << b;

	label1->Enable( b );
	choice_partner1->Enable( b );
	label2->Enable( b );
	choice_partner2->Enable( b );
}




