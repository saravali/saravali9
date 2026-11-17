/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/MiscellaneousConflet.cpp
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

#include "MiscellaneousConflet.h"
#include "CommonControls.h"
#include "enumserializer.h"
#include "guibase.h"
#include "logger.h"
#include "SpinCtrl.h"

#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

extern Gdiconfig *gdiconfig;

enum { CD_ENABLE_UI_LOGGING = wxID_HIGHEST + 1 };

// begin wxGlade: ::extracode
// end wxGlade

/*****************************************************
**
**   MiscellaneousConflet   ---   Constructor
**
******************************************************/
MiscellaneousConflet::MiscellaneousConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->view )
{
	// begin wxGlade: MiscellaneousConflet::MiscellaneousConflet
	wxFlexGridSizer* sizer_main = new wxFlexGridSizer(1, 2, 0, 0);
	wxFlexGridSizer* sizer_left = new wxFlexGridSizer(2, 1, 0, 0);
	sizer_main->Add(sizer_left, 1, wxALL, 3);
	wxStaticBoxSizer* sizer_misc = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Miscellaneous")), wxVERTICAL);
	sizer_left->Add(sizer_misc, 0, wxEXPAND, 0);
	check_responsive_events = new wxCheckBox(sizer_misc->GetStaticBox(), wxID_ANY, _("Handle Responsive Events in Widgets"));
	sizer_misc->Add(check_responsive_events, 0, wxALL|wxEXPAND, 3);
	check_showstatusinfo = new wxCheckBox(sizer_misc->GetStaticBox(), wxID_ANY, _("Running Planets in Status Bar"));
	sizer_misc->Add(check_showstatusinfo, 0, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* grid_misc = new wxFlexGridSizer(3, 2, 3, 3);
	sizer_misc->Add(grid_misc, 0, wxALL, 3);
	label_file_backup = new wxStaticText(sizer_misc->GetStaticBox(), wxID_ANY, _("Create Backup Files"));
	grid_misc->Add(label_file_backup, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_file_backup_choices[] = {
		_("Never"),
		_("One"),
		_("Infinite"),
	};
	choice_file_backup = new wxChoice(sizer_misc->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, choice_file_backup_choices);
	choice_file_backup->SetSelection(0);
	grid_misc->Add(choice_file_backup, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	label_recent_files = new wxStaticText(sizer_misc->GetStaticBox(), wxID_ANY, _("Number of Recent Files"));
	grid_misc->Add(label_recent_files, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	spin_recent_files = new wxSpinCtrl(sizer_misc->GetStaticBox(), wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, 0, 50);
	grid_misc->Add(spin_recent_files, 0, wxALL|wxEXPAND, 3);
	label_frame_width = new wxStaticText(sizer_misc->GetStaticBox(), wxID_ANY, _("Child View Frame Width"));
	grid_misc->Add(label_frame_width, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	spin_frame_width = new wxSpinCtrl(sizer_misc->GetStaticBox(), wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, 0, 100);
	grid_misc->Add(spin_frame_width, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	check_textwidget_native = new wxCheckBox(sizer_misc->GetStaticBox(), wxID_ANY, _("Use Native Text Widget instead of wx"));
	sizer_misc->Add(check_textwidget_native, 0, wxALL|wxEXPAND, 3);
	check_validator_feedback = new wxCheckBox(sizer_misc->GetStaticBox(), wxID_ANY, _("Feedback on Validator Error in Dialogs"));
	sizer_misc->Add(check_validator_feedback, 0, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_export = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Image and Text Export")), wxVERTICAL);
	sizer_left->Add(sizer_export, 1, wxALL|wxEXPAND, 3);
	wxBoxSizer* grid_export = new wxBoxSizer(wxHORIZONTAL);
	sizer_export->Add(grid_export, 1, wxALL|wxEXPAND, 3);
	label_graphic_size_dialog = new wxStaticText(sizer_export->GetStaticBox(), wxID_ANY, _("Image Size"));
	grid_export->Add(label_graphic_size_dialog, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_graphic_size_dialog_choices[] = {
		_("Show size dialog"),
		_("Use view size"),
	};
	choice_graphic_size_dialog = new wxChoice(sizer_export->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, choice_graphic_size_dialog_choices);
	choice_graphic_size_dialog->SetSelection(0);
	grid_export->Add(choice_graphic_size_dialog, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	check_overwrite = new wxCheckBox(sizer_export->GetStaticBox(), wxID_ANY, _("Ask on overwrite"));
	sizer_export->Add(check_overwrite, 0, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* sizer_right = new wxFlexGridSizer(3, 1, 0, 0);
	sizer_main->Add(sizer_right, 1, wxALL, 3);
	wxStaticBoxSizer* sizer_ui_logging = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("User Interface Logging")), wxVERTICAL);
	sizer_right->Add(sizer_ui_logging, 1, wxEXPAND, 0);
	wxBoxSizer* sizer_ui_loglevel = new wxBoxSizer(wxHORIZONTAL);
	sizer_ui_logging->Add(sizer_ui_loglevel, 0, wxEXPAND, 0);
	label_ui_loglevel = new wxStaticText(sizer_ui_logging->GetStaticBox(), wxID_ANY, _("Log Level"));
	sizer_ui_loglevel->Add(label_ui_loglevel, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_ui_loglevel_choices[] = {
		_("dummy"),
	};
	choice_ui_loglevel = new wxChoice(sizer_ui_logging->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 1, choice_ui_loglevel_choices);
	sizer_ui_loglevel->Add(choice_ui_loglevel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxStaticBoxSizer* sizer_notification = new wxStaticBoxSizer(new wxStaticBox(sizer_ui_logging->GetStaticBox(), wxID_ANY, _("Notifications on Desktop")), wxVERTICAL);
	sizer_ui_logging->Add(sizer_notification, 1, wxEXPAND, 0);
	check_notification = new wxCheckBox(sizer_notification->GetStaticBox(), wxID_ANY, _("Enable Notifications"));
	sizer_notification->Add(check_notification, 0, wxALL, 3);
	wxFlexGridSizer* grid_notification = new wxFlexGridSizer(2, 2, 0, 0);
	sizer_notification->Add(grid_notification, 1, wxEXPAND, 0);
	label_notification_level = new wxStaticText(sizer_notification->GetStaticBox(), wxID_ANY, _("Notify From Level"));
	grid_notification->Add(label_notification_level, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_notification_level_choices[] = {
		_("dummy"),
	};
	choice_notification_level = new wxChoice(sizer_notification->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 1, choice_notification_level_choices);
	grid_notification->Add(choice_notification_level, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
	label_notification_timeout = new wxStaticText(sizer_notification->GetStaticBox(), wxID_ANY, _("Timeout (sec)"));
	grid_notification->Add(label_notification_timeout, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	spin_notification_timeout = new wxSpinCtrl(sizer_notification->GetStaticBox(), wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100);
	grid_notification->Add(spin_notification_timeout, 0, 0, 0);
	wxStaticBoxSizer* sizer_stdout_logging = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Logging to Stdout")), wxVERTICAL);
	sizer_right->Add(sizer_stdout_logging, 1, wxEXPAND, 0);
	check_stdout_logging = new wxCheckBox(sizer_stdout_logging->GetStaticBox(), wxID_ANY, _("Enable Stdout Logging"));
	sizer_stdout_logging->Add(check_stdout_logging, 0, wxALL, 3);
	wxBoxSizer* sizer_std_loglevel = new wxBoxSizer(wxHORIZONTAL);
	sizer_stdout_logging->Add(sizer_std_loglevel, 1, wxEXPAND, 0);
	label_stdout_loglevel = new wxStaticText(sizer_stdout_logging->GetStaticBox(), wxID_ANY, _("Log Level"));
	sizer_std_loglevel->Add(label_stdout_loglevel, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_stdout_loglevel_choices[] = {
		_("dummy"),
	};
	choice_stdout_loglevel = new wxChoice(sizer_stdout_logging->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 1, choice_stdout_loglevel_choices);
	sizer_std_loglevel->Add(choice_stdout_loglevel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxStaticBoxSizer* sizer_1 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("TEST")), wxVERTICAL);
	sizer_right->Add(sizer_1, 1, wxEXPAND, 0);
	spin_date = new DateSpin(sizer_1->GetStaticBox(), wxID_ANY, &cfg.jd);
	sizer_1->Add(spin_date, 1, wxALL|wxEXPAND, 3);
	spin_time = new DegreeSpin(sizer_1->GetStaticBox(), wxID_ANY, &cfg.time, 0, 24, wxDefaultPosition, wxDefaultSize);
	sizer_1->Add(spin_time, 0, wxALL|wxEXPAND, 3);
	degree_longitude = new CombinedDegreeSpin(sizer_1->GetStaticBox(), wxID_ANY, &cfg.longitude, DegreeType::Longitude);
	sizer_1->Add(degree_longitude, 1, wxALL|wxEXPAND, 3);
	
	sizer_right->AddGrowableCol(0);
	grid_misc->AddGrowableCol(1);
	sizer_left->AddGrowableCol(0);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	// Main Window
	check_responsive_events->SetValidator( wxGenericValidator( &cfg.doHandleResponsiveWidgetEvents ));
	check_showstatusinfo->SetValidator(  wxGenericValidator( &cfg.showStatusInfo ));

	// Left: miscellaneous
	choice_file_backup->SetValidator( wxGenericValidator( &cfg.backupFileMode ));
	spin_frame_width->SetValidator( wxGenericValidator( &cfg.childViewFrameWidth ));
	spin_recent_files->SetValidator( wxGenericValidator( &cfg.maxRecentFiles ));

	// text widget
	check_textwidget_native->SetValidator( wxGenericValidator( &cfg.textWidgetUseGdiNative ));

	check_validator_feedback->SetValidator( wxGenericValidator( &cfg.validatorFeedbackOnError ));

	// image and text export
	choice_graphic_size_dialog->SetValidator( wxGenericValidator( &cfg.graphicExportSizeMode ));
	check_overwrite->SetValidator( wxGenericValidator( &cfg.exportAskOnOverwrite ));

	// logging
	choice_ui_loglevel->Clear();
	for( const LogLevel l : Enum<LogLevel>())
	{
		choice_ui_loglevel->Append( enum_to_string( l ));
	}
	choice_ui_loglevel->SetValidator( wxGenericValidator( (int*)&cfg.uiLogLevel ));

	check_notification->SetValidator( wxGenericValidator( &cfg.enableUiNotification ));
	choice_notification_level->Clear();
	for( const LogLevel l : Enum<LogLevel>()) // LogLevel::Fatal, LogLevel::Info ))
	{
		choice_notification_level->Append( enum_to_string( l ));
	}
	choice_notification_level->SetValidator( wxGenericValidator( (int*)&cfg.uiNotificationLogLevel ));
	spin_notification_timeout->SetValidator( wxGenericValidator( &cfg.uiNotificationTimeout ));

// no stdout available on MSW
#ifdef __WXMSW__
	check_stdout_logging->Enable( false );
	label_stdout_loglevel->Enable( false );
	choice_stdout_loglevel->Enable( false );

#else
	check_stdout_logging->SetValidator( wxGenericValidator( &cfg.enableStdoutLogging ));
	choice_stdout_loglevel->Clear();
	for( const LogLevel l : Enum<LogLevel>())
	{
		choice_stdout_loglevel->Append( enum_to_string( l ));
	}
	choice_stdout_loglevel->SetValidator( wxGenericValidator( (int*)&cfg.stdoutLogLevel ));

#endif
}

/*****************************************************
**
**   MiscellaneousConflet   ---   model2config
**
******************************************************/
void MiscellaneousConflet::model2config()
{
	globalConfig = cfg;
	LogAdapter *adapter = LogAdapter::get();

	adapter->setWxLogLevel( std::max( cfg.uiLogLevel, cfg.uiNotificationLogLevel ));

	adapter->enableStdoutLogAppender( cfg.enableStdoutLogging );
	adapter->setStdoutLogLevel( cfg.stdoutLogLevel );
}

/*****************************************************
**
**   MiscellaneousConflet   ---   updateUi
**
******************************************************/
void MiscellaneousConflet::updateUi()
{
	debuglog << "MiscellaneousConflet::updateUi";
	const bool enable_notify = check_notification->IsChecked();

#ifndef __WXMSW__
	const bool enable_stdout = check_stdout_logging->IsChecked();
#else
	const bool enable_stdout = false;
#endif

	label_notification_level->Enable( enable_notify );
	choice_notification_level->Enable( enable_notify );
	label_notification_timeout->Enable( enable_notify );
	spin_notification_timeout->Enable( enable_notify );

	label_stdout_loglevel->Enable( enable_stdout );
	choice_stdout_loglevel->Enable( enable_stdout );
}


