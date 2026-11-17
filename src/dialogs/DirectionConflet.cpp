/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/DirectionConflet.cpp
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

#include "DirectionConflet.h"

#include "CommonControls.h"
#include "enumserializer.h"
#include "logger.h"

#include <wx/checklst.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>

extern Astroconfig *astroconfig;

enum { CD_MULTIDIRECTION_ROLES = wxID_HIGHEST + 1, CD_EVENTFINDER_ROLES };

// begin wxGlade: ::extracode
// end wxGlade

//#define DEBUG_DIRECTIONCONFLET

/*************************************************//**
*
* 
*
******************************************************/
class RoleListValidator : public wxValidator
{
public:
	RoleListValidator( std::list<HoroscopeRole> *roles )
		: roles( roles ) {}

	virtual wxObject *Clone() const override
	{
		return new RoleListValidator( *this );
	}

	virtual bool TransferFromWindow() override
	{
#ifdef DEBUG_DIRECTIONCONFLET
		debuglog << "RoleListValidator::TransferFromWindow";
#endif
		wxCheckListBox *cb = getListCtrl();

		int i = 0;
		roles->clear();
		for( const auto &role : Enum<HoroscopeRole>( HoroscopeRole::FirstDirection, HoroscopeRole::LastDirection ))
		{
			if ( cb->IsChecked( i ))
			{
				roles->push_back( role );
			}
			i++;
		}
		return true;
	}

	virtual bool TransferToWindow() override
	{
#ifdef DEBUG_DIRECTIONCONFLET
		debuglog << "RoleListValidator::TransferToWindow";
#endif
		wxCheckListBox *cb = getListCtrl();
		cb->Clear();

		int i = 0;
		for( const auto &role : Enum<HoroscopeRole>( HoroscopeRole::FirstDirection, HoroscopeRole::LastDirection ))
		{
			cb->Append( enum_to_string( role ));
			auto a = std::find_if( roles->begin(), roles->end(),
				[role]( const HoroscopeRole &r ) { return r == role; } );
			cb->Check( i, a != roles->end() );
			i++;
		}
		return true;
	}

protected:

		wxCheckListBox *getListCtrl()
		{
			wxCheckListBox *ctrl = dynamic_cast<wxCheckListBox*>( GetWindow() );
			assert( ctrl );
			return ctrl;
		}

	std::list<HoroscopeRole> *roles;
};

/*****************************************************
**
**   DirectionConflet   ---   Constructor
**
******************************************************/
DirectionConflet::DirectionConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, astroconfig->directions )
{
	for ( const auto &a : cfg.eventFinderItems )
	{
		eventFinderRoles.push_back( a.role );
	}
	// begin wxGlade: DirectionConflet::DirectionConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* sizer_top = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_top, 0, wxALL, 3);
	wxBoxSizer* sizer_yl = new wxBoxSizer(wxHORIZONTAL);
	sizer_top->Add(sizer_yl, 1, wxALL|wxEXPAND, 3);
	yearLengthPanel = new YearLengthPanel(this, wxID_ANY, cfg.yearLength);
	sizer_yl->Add(yearLengthPanel, 1, wxALL|wxEXPAND, 3);
	wxBoxSizer* sizer_bottom = new wxBoxSizer(wxHORIZONTAL);
	sizer_main->Add(sizer_bottom, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_multidirection = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Multi Direction Roles")), wxVERTICAL);
	sizer_bottom->Add(sizer_multidirection, 1, wxALL|wxEXPAND, 3);
	check_multidirection = new wxCheckListBox(sizer_multidirection->GetStaticBox(), CD_MULTIDIRECTION_ROLES);
	sizer_multidirection->Add(check_multidirection, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_eventfinder = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Event Finder Roles")), wxVERTICAL);
	sizer_bottom->Add(sizer_eventfinder, 1, wxALL|wxEXPAND, 3);
	check_eventfinder = new wxCheckListBox(sizer_eventfinder->GetStaticBox(), CD_EVENTFINDER_ROLES);
	sizer_eventfinder->Add(check_eventfinder, 1, wxALL|wxEXPAND, 3);
	
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	check_multidirection->SetValidator( RoleListValidator( &cfg.multiDirectionRoles ));
	check_eventfinder->SetValidator( RoleListValidator( &eventFinderRoles ));

	Connect( wxID_ANY, wxEVT_CHECKLISTBOX, wxCommandEventHandler( DirectionConflet::OnListbox ));
	Connect( wxID_ANY, wxEVT_LISTBOX, wxCommandEventHandler( DirectionConflet::OnListbox ));
}

/*****************************************************
**
**   DirectionConflet   ---   updateUi
**
******************************************************/
void DirectionConflet::updateUi()
{
#ifdef DEBUG_DIRECTIONCONFLET
	debuglog << "DirectionConflet::updateUi";
#endif
}

/*****************************************************
**
**   DirectionConflet   ---   OnListbox
**
******************************************************/
void DirectionConflet::OnListbox( wxCommandEvent& )
{
#ifdef DEBUG_DIRECTIONCONFLET
	debuglog << "DirectionConflet::OnListbox";
#endif
	guiHasChanged();
}

/*****************************************************
**
**   DirectionConflet   ---   config2model
**
******************************************************/
void DirectionConflet::config2model()
{
#ifdef DEBUG_DIRECTIONCONFLET
	debuglog << "DirectionConflet::config2model";
#endif
	cfg = astroconfig->directions;

	eventFinderRoles.clear();
	for ( const auto &a : cfg.eventFinderItems )
	{
		eventFinderRoles.push_back( a.role );
	}
}

/*****************************************************
**
**   DirectionConflet   ---   updateUranianSection
**
******************************************************/
void DirectionConflet::updateUranianSection( const UranianContext &context )
{
#ifdef DEBUG_DIRECTIONCONFLET
	debuglog << "DirectionConflet::updateUranianSection context " << context;
#endif

	UranianSection &usection = astroconfig->getUranianSection( context );

	for( const auto &role : Enum<HoroscopeRole>( HoroscopeRole::FirstDirection, HoroscopeRole::LastDirection ))
	{
		const bool cfgFound = ( std::find( cfg.multiDirectionRoles.begin(), cfg.multiDirectionRoles.end(), role )
			!= cfg.multiDirectionRoles.end());
		const auto a = std::find_if( usection.roleRestrictors.begin(), usection.roleRestrictors.end(), 
			[role] ( const RoleRestrictor &restrictor ) { return restrictor.role == role; } );
		const bool secFound = ( a != usection.roleRestrictors.end());

		if ( cfgFound )
		{
			if ( ! secFound )
			{
				usection.roleRestrictors.push_back( RoleRestrictor( role, false ));
			}
		}
		else
		{
			if ( secFound )
			{
				usection.roleRestrictors.erase( a );
			}
		}
	}

	usection.roleRestrictors.sort( []( const RoleRestrictor &r1, const RoleRestrictor &r2 ) { return r1.role < r2.role; } );

	for ( auto &a : usection.roleRestrictors )
	{
#ifdef DEBUG_DIRECTIONCONFLET
		debuglog << "DirectionConflet::updateUranianSection role " << a.role;
#endif
	}
}

/*****************************************************
**
**   DirectionConflet   ---   restoreDefaults
**
******************************************************/
void DirectionConflet::restoreDefaults()
{
#ifdef DEBUG_DIRECTIONCONFLET
	debuglog << "DirectionConflet::restoreDefaults";
#endif

	cfg = DirectionConfig();

	eventFinderRoles.clear();
	for ( const auto &a : cfg.eventFinderItems )
	{
		eventFinderRoles.push_back( a.role );
	}
}

/*****************************************************
**
**   DirectionConflet   ---   model2config
**
******************************************************/
void DirectionConflet::model2config()
{
#ifdef DEBUG_DIRECTIONCONFLET
	debuglog << "DirectionConflet::model2config";
#endif

	// avoid empty lists
	if ( cfg.multiDirectionRoles.size() == 0 )
	{
			warnlog << "DirectionConflet::model2config has empty multidirection role list";
			cfg.multiDirectionRoles.push_back( HoroscopeRole::Transit );
	}


	updateUranianSection( UranianContext::MultiDirectionChart );
	updateUranianSection( UranianContext::MultiDirectionMatches );

	// event finder stuff
	for( const auto &role : Enum<HoroscopeRole>( HoroscopeRole::FirstDirection, HoroscopeRole::LastDirection ))
	{
		const bool cfgFound = ( std::find( eventFinderRoles.begin(), eventFinderRoles.end(), role )
			!= eventFinderRoles.end());

		const auto a = std::find_if( cfg.eventFinderItems.begin(), cfg.eventFinderItems.end(), 
			[role] ( const RoleRestrictor &restrictor ) { return restrictor.role == role; } );
		const bool secFound = ( a != cfg.eventFinderItems.end());

		if ( cfgFound )
		{
			if ( ! secFound )
			{
				cfg.eventFinderItems.push_back( RoleRestrictor( role, false ));
			}
		}
		else
		{
			if ( secFound )
			{
				cfg.eventFinderItems.erase( a );
			}
		}
	}

	// avoid empty lists
	if ( cfg.eventFinderItems.size() == 0 )
	{
			warnlog << "DirectionConflet::model2config event finder items empty, add transit";
			cfg.eventFinderItems.push_back( RoleRestrictor( HoroscopeRole::SolarArc, false ));
	}
	astroconfig->directions = cfg;

#ifdef DEBUG_DIRECTIONCONFLET
	for ( auto &role : eventFinderRoles )
	{
		debuglog << "DirectionConflet::model2config event finder role " << role;
	}
	for ( auto &a : cfg.eventFinderItems )
	{
		debuglog << "DirectionConflet::model2config event finder item role " << a.role
			<< " enabled " << std::boolalpha << a.enabled;
	}
#endif
}

