/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/GeneralConfigConflet.cpp
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

#include "GeneralConfigConflet.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "LanguageConfig.h"
#include "logger.h"
#include "mathbase.h"

#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include <wx/valgen.h>

enum { CD_LANGUAGE = wxID_HIGHEST + 1, CD_DATEFORMAT };

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

//#ifdef __WXMSW__
#if true
#define SHOW_LANGUAGES
#endif

/*****************************************************
**
**   GeneralConfigConflet   ---   Constructor
**
******************************************************/
GeneralConfigConflet::GeneralConfigConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, astroconfig->general )
{
	// begin wxGlade: GeneralConfigConflet::GeneralConfigConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer* sizer_general = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("General")), wxVERTICAL);
	sizer_main->Add(sizer_general, 0, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* grid_general = new wxFlexGridSizer(2, 2, 3, 3);
	sizer_general->Add(grid_general, 1, wxALL, 3);
	wxStaticText* label_default_viewpoint = new wxStaticText(sizer_general->GetStaticBox(), wxID_ANY, _("Default Viewpoint"));
	grid_general->Add(label_default_viewpoint, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_default_viewpoint_choices[] = {
		_("Vedic"),
		_("Western"),
		_("Uranian"),
		_("Astronomical"),
	};
	choice_default_viewpoint = new wxChoice(sizer_general->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, choice_default_viewpoint_choices);
	choice_default_viewpoint->SetSelection(0);
	grid_general->Add(choice_default_viewpoint, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
	wxStaticText* label_viewpoint_policy = new wxStaticText(sizer_general->GetStaticBox(), wxID_ANY, _("Viewpoint Change Policy"));
	grid_general->Add(label_viewpoint_policy, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_viewpoint_policy_choices[] = {
		_("Fixed"),
		_("Medium"),
		_("Flexible"),
	};
	choice_viewpoint_policy = new wxChoice(sizer_general->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_viewpoint_policy_choices);
	choice_viewpoint_policy->SetSelection(0);
	grid_general->Add(choice_viewpoint_policy, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_localization = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Localization")), wxVERTICAL);
	sizer_main->Add(sizer_localization, 0, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* grid_localization = new wxFlexGridSizer(2, 3, 3, 3);
	sizer_localization->Add(grid_localization, 1, wxALL|wxEXPAND, 3);
	wxStaticText* label_lang = new wxStaticText(sizer_localization->GetStaticBox(), wxID_ANY, _("Language"));
	grid_localization->Add(label_lang, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_lang_choices[] = {
		_("dummy"),
		_("dummy"),
		_("dummy"),
		_("dummy"),
	};
	choice_lang = new wxChoice(sizer_localization->GetStaticBox(), CD_LANGUAGE, wxDefaultPosition, wxDefaultSize, 4, choice_lang_choices);
	choice_lang->SetSelection(0);
	grid_localization->Add(choice_lang, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
	grid_localization->Add(20, 20, 0, 0, 0);
	wxStaticText* label_dateformat = new wxStaticText(sizer_localization->GetStaticBox(), wxID_ANY, _("Date Format"));
	grid_localization->Add(label_dateformat, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_dateformat_choices[] = {
		_("ISO (YYYY-MM-DD)"),
		_("Local Format (experimental)"),
	};
	choice_dateformat = new wxChoice(sizer_localization->GetStaticBox(), CD_DATEFORMAT, wxDefaultPosition, wxDefaultSize, 2, choice_dateformat_choices);
	choice_dateformat->SetSelection(0);
	grid_localization->Add(choice_dateformat, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
	label_datepreview = new wxStaticText(sizer_localization->GetStaticBox(), wxID_ANY, _("YYYY-MM-DD TODO"));
	grid_localization->Add(label_datepreview, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND, 0);
	wxStaticBoxSizer* sizer_statusbar = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Statusbar in Main Window")), wxVERTICAL);
	sizer_main->Add(sizer_statusbar, 0, wxEXPAND, 0);
	checkbox_logmessages = new wxCheckBox(sizer_statusbar->GetStaticBox(), wxID_ANY, _("Show Log Messages"));
	sizer_statusbar->Add(checkbox_logmessages, 0, wxALL, 3);
	checkbox_ascpos = new wxCheckBox(sizer_statusbar->GetStaticBox(), wxID_ANY, _("Show Current Ascendant"));
	sizer_statusbar->Add(checkbox_ascpos, 0, wxALL, 3);
	checkbox_moonpos = new wxCheckBox(sizer_statusbar->GetStaticBox(), wxID_ANY, _("Show Current Moon Position"));
	sizer_statusbar->Add(checkbox_moonpos, 0, wxALL, 3);
	wxBoxSizer* sizer_1 = new wxBoxSizer(wxHORIZONTAL);
	sizer_statusbar->Add(sizer_1, 1, wxEXPAND, 0);
	wxStaticText* label_timeout = new wxStaticText(sizer_statusbar->GetStaticBox(), wxID_ANY, _("Message Timeout (sec)"));
	sizer_1->Add(label_timeout, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	spin_statusmessage_timeout = new wxSpinCtrl(sizer_statusbar->GetStaticBox(), wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);
	sizer_1->Add(spin_statusmessage_timeout, 0, wxALL, 3);
	wxStaticBoxSizer* sizer_window_lifecycle = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Window Life Cycle")), wxVERTICAL);
	sizer_main->Add(sizer_window_lifecycle, 0, wxEXPAND, 0);
	check_opennewdoc = new wxCheckBox(sizer_window_lifecycle->GetStaticBox(), wxID_ANY, _("Open New Document on Startup"));
	sizer_window_lifecycle->Add(check_opennewdoc, 0, wxALL|wxEXPAND, 1);
	check_askonquit = new wxCheckBox(sizer_window_lifecycle->GetStaticBox(), wxID_ANY, _("Ask on Quit"));
	check_askonquit->SetMinSize(wxSize(150, 24));
	sizer_window_lifecycle->Add(check_askonquit, 0, wxALL|wxEXPAND, 1);
	
	grid_localization->AddGrowableCol(2);
	grid_general->AddGrowableCol(1);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	choice_default_viewpoint->SetValidator( wxGenericValidator( (int*)&cfg.defaultViewpoint ));
	choice_viewpoint_policy->SetValidator( wxGenericValidator( (int*)&cfg.viewpointChangePolicy ));

#ifdef SHOW_LANGUAGES
	languageChoices = LanguageConfig::get()->getLanguageChoiceItems();
	choice_lang->Clear();
	for( const auto &a : languageChoices )
	{
		choice_lang->Append( a.second );
	}
	setLanguageChoiceSelection();
#else
	choice_lang->Enable( false );
#endif

	choice_dateformat->Clear();
	for( const auto &m : Enum<DateFormat>())
	{
		choice_dateformat->Append( resolveCamelNotation( m ));
	}
	choice_dateformat->SetValidator( wxGenericValidator( (int*)&cfg.dateFormat ));

	checkbox_logmessages->SetValidator( wxGenericValidator( &cfg.statusbarShowLogMessages ));
	checkbox_ascpos->SetValidator( wxGenericValidator( &cfg.statusbarShowAscPosition ));
	checkbox_moonpos->SetValidator( wxGenericValidator( &cfg.statusbarShowMoonPosition ));
	spin_statusmessage_timeout->SetValidator( wxGenericValidator( &cfg.statusMessageTimeout ));

	check_opennewdoc->SetValidator( wxGenericValidator( &cfg.openNewDocOnStart ));
	check_askonquit->SetValidator( wxGenericValidator( &cfg.askOnQuit ));

	Connect( CD_LANGUAGE, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( GeneralConfigConflet::OnLanguageChoice ));
}

/*****************************************************
**
**   GeneralConfigConflet   ---   updateUi
**
******************************************************/
void GeneralConfigConflet::updateUi()
{
	TransferDataFromWindow();

	debuglog << "GeneralConfigConflet::updateUi dateFormat " << cfg.dateFormat;
	label_datepreview->SetLabel( DateFormatter::get()->formatUTDate( MDate::getCurrentJD(), cfg.dateFormat ));
}

/*****************************************************
**
**   GeneralConfigConflet   ---   setLanguageChoiceSelection
**
******************************************************/
void GeneralConfigConflet::setLanguageChoiceSelection()
{
	debuglog << "GeneralConfigConflet::setLanguageChoiceSelection cfg.languageCode " << cfg.languageCode;
	uint sel = 0;

	for( uint i = 1; i < languageChoices.size(); i++ )
	{
		if ( cfg.languageCode == languageChoices[i].first )
		{
			sel = i;
			break;
		}
	}
	choice_lang->SetSelection( sel );
	debuglog << "Finished GeneralConfigConflet::setLanguageChoiceSelection selection is now " << choice_lang->GetSelection();
}

/*****************************************************
**
**   GeneralConfigConflet   ---   OnLanguageChoice
**
******************************************************/
void GeneralConfigConflet::OnLanguageChoice( wxCommandEvent& )
{
	const uint sel = choice_lang->GetSelection();
	debuglog << "GeneralConfigConflet::OnLanguageChoice selection is " << sel << " cfg.languageCode " << cfg.languageCode;
	if ( sel == 0 )
	{
		cfg.languageCode = "";
	}
	else if ( sel > 0 && sel <= languageChoices.size() )
	{
		cfg.languageCode = languageChoices[sel].first;
	}
	else
	{
		warnlog << "GeneralConfigConflet::OnLanguageChoice invalidLanguageChoice value " << sel;
	}
	debuglog << "Finished GeneralConfigConflet::OnLanguageChoice cfg.languageCode is now " << cfg.languageCode;

	doNotShowAgainMessage( this, DoNotShowAgainTopic::LanguageChange,
		tr( "The language will be changed when you restart the program." ));
	guiHasChanged();
}

/*****************************************************
**
**   GeneralConfigConflet   ---   restoreDefaults
**
******************************************************/
void GeneralConfigConflet::restoreDefaults()
{
	debuglog << "GeneralConfigConflet::restoreDefaults";
	cfg = GeneralConfig();
	setLanguageChoiceSelection();
	TransferDataToWindow();
	guiHasChanged();
}




