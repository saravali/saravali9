/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AtlasImportDialog.cpp
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

#include "AtlasImportDialog.h"

#include "AtlasImporter.h"
#include "FileConfig.h"
#include "gdibase.h"
#include "guibase.h"

#include <wx/button.h>
#include <wx/filename.h>
#include <wx/gauge.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/timer.h>

// begin wxGlade: ::extracode
// end wxGlade

enum { AI_SQLFILE = wxID_HIGHEST + 1, AI_DBFILE, AI_RUN, AI_ABORT, AI_TIMER };

#define TIMER_PERIOD_MILLISEC  100

/*************************************************//**
*
*  AtlasImportDialog   ---   Constructor
*
******************************************************/
AtlasImportDialog::AtlasImportDialog(wxWindow* parent, wxString title )
 : wxDialog( parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, DEFAULT_DIALOG_STYLE )
{
    // begin wxGlade: AtlasImportDialog::AtlasImportDialog
    SetTitle(_("Atlas Data Import"));
    wxBoxSizer* sizer_main = new wxBoxSizer(wxVERTICAL);
    panel_gauge = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED|wxTAB_TRAVERSAL);
    sizer_main->Add(panel_gauge, 1, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_gauge = new wxBoxSizer(wxVERTICAL);
    gauge = new wxGauge(panel_gauge, wxID_ANY, 100);
    gauge->SetMinSize(wxSize(300, -1));
    sizer_gauge->Add(gauge, 0, wxALL|wxEXPAND, 3);
    text_status = new wxStaticText(panel_gauge, wxID_ANY, _("dummy"));
    sizer_gauge->Add(text_status, 0, 0, 3);
    button_cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
    sizer_main->Add(button_cancel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
    
    panel_gauge->SetSizer(sizer_gauge);
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    Layout();
    // end wxGlade

	close_called = false;
	timer = new wxTimer( this, AI_TIMER );
	timer->Start( TIMER_PERIOD_MILLISEC );

	importer = new AtlasImporter;

	if ( ! title.IsEmpty() ) SetTitle( title );
	SetSize( 600, -1 );

	Connect( wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasImportDialog::OnCancel ));
	Connect( AI_TIMER, wxEVT_TIMER, wxTimerEventHandler( AtlasImportDialog::OnTimer ));
}

/*************************************************//**
*
*  AtlasImportDialog   ---   Destructor
*
******************************************************/
AtlasImportDialog::~AtlasImportDialog()
{
	if ( timer->IsRunning()) timer->Stop();
	delete importer;
}

/*****************************************************
**
**   AtlasImportDialog   ---   run
**
******************************************************/
bool AtlasImportDialog::run()
{
	// testen, ob Dateien ok
	if ( ! checkFiles()) return false;

	importer->setImportFile( sqlfile );
	importer->setDatabaseFile( dbfile );
	importer->run();
	return true;
}

/*****************************************************
**
**   AtlasImportDialog   ---   getErrorMessage
**
******************************************************/
wxString AtlasImportDialog::getErrorMessage()
{
	return importer->writeErrorStatus();
}

/*****************************************************
**
**   AtlasImportDialog   ---   checkFiles
**
******************************************************/
bool AtlasImportDialog::checkFiles()
{
	return true;
}

/*****************************************************
**
**   AtlasImportDialog   ---   setDefaultFiles
**
******************************************************/
bool AtlasImportDialog::setDefaultFiles()
{
	FileConfig *fc = FileConfig::get();
	return setData( fc->getDefaultAtlasSqlFile(), fc->getDefaultAtlasDbFile());
}

/*****************************************************
**
**   AtlasImportDialog   ---   setData
**
******************************************************/
bool AtlasImportDialog::setData( wxString sqlfile, wxString dbfile )
{
	this->sqlfile = sqlfile;
	this->dbfile = dbfile;
	return checkFiles();
	return true;
}

/*****************************************************
**
**   AtlasImportDialog   ---   OnTimer
**
******************************************************/
void AtlasImportDialog::OnTimer( wxTimerEvent& )
{
	gauge->Pulse();

	if ( importer->hasErrors())
	{
		if ( ! close_called ) EndModal( wxID_ABORT );
		close_called = true;
	}
	else if ( importer->isCanceled())
	{
		if ( ! close_called ) EndModal( wxID_CANCEL );
		close_called = true;
	}
	else if ( importer->isFinished())
	{
		if ( ! close_called ) EndModal( wxID_OK );
		close_called = true;
	}
	else
	{
		text_status->SetLabel( importer->writeRunStatus());
	}
}

/*****************************************************
**
**   AtlasImportDialog   ---   OnRun
**
******************************************************/
void AtlasImportDialog::OnRun( wxCommandEvent& )
{
	importer->run();
}

/*****************************************************
**
**   AtlasImportDialog   ---   OnCancel
**
******************************************************/
void AtlasImportDialog::OnCancel( wxCommandEvent& )
{
	importer->abort();
	if ( ! close_called ) EndModal( wxID_CANCEL );
	close_called = true;
}

