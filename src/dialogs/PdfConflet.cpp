/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PdfConflet.cpp
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

#include "PdfConflet.h"

#include "gdiconfig.h"
#include "logger.h"
#include "TextValidator.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/filedlg.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>
#include <wx/valtext.h>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_LAUNCH_COMMAND_TEXT = wxID_HIGHEST + 1, CD_LAUNCH_VIEWER, CD_LAUNCH_COMMAND_BUTTON };

/*****************************************************
**
**   PdfConflet   ---   Constructor
**
******************************************************/
PdfConflet::PdfConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->pdf )
{
	// begin wxGlade: PdfConflet::PdfConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* sizer_right = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_right, 1, wxEXPAND, 0);
	wxBoxSizer* sizer_savelaunch = new wxBoxSizer(wxVERTICAL);
	sizer_right->Add(sizer_savelaunch, 1, wxEXPAND, 0);
	wxStaticBoxSizer* sizer_save_options = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Save Options")), wxVERTICAL);
	sizer_savelaunch->Add(sizer_save_options, 0, wxEXPAND, 0);
	check_overwrite = new wxCheckBox(sizer_save_options->GetStaticBox(), wxID_ANY, _("Ask on Overwrite"));
	sizer_save_options->Add(check_overwrite, 0, wxALL|wxEXPAND, 3);
	check_compress = new wxCheckBox(sizer_save_options->GetStaticBox(), wxID_ANY, _("Compress PDF"));
	sizer_save_options->Add(check_compress, 0, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_viewer = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Pdf Viewer")), wxVERTICAL);
	sizer_savelaunch->Add(sizer_viewer, 0, wxALL|wxEXPAND, 3);
	check_launchviewer = new wxCheckBox(sizer_viewer->GetStaticBox(), CD_LAUNCH_VIEWER, _("Launch Pdf Viewer Automatically"));
	sizer_viewer->Add(check_launchviewer, 0, wxALL|wxEXPAND, 3);
	button_launchcommand = new wxButton(sizer_viewer->GetStaticBox(), CD_LAUNCH_COMMAND_BUTTON, _("Command ..."));
	sizer_viewer->Add(button_launchcommand, 0, wxALL, 3);
	text_launchcommand = new wxTextCtrl(sizer_viewer->GetStaticBox(), CD_LAUNCH_COMMAND_TEXT, wxEmptyString);
	sizer_viewer->Add(text_launchcommand, 0, wxALL|wxEXPAND, 3);
	
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	debuglog << "PdfConflet cfg.askOverwrite " << cfg.askOverwrite;
	check_overwrite->SetValidator( wxGenericValidator( &cfg.askOverwrite ));
	check_compress->SetValidator( wxGenericValidator( &cfg.compressPdf ));

	check_launchviewer->SetValidator( wxGenericValidator( &cfg.launchPdfViewer ));

#ifdef __WXMSW__
	button_launchcommand->Enable( false );
	text_launchcommand->Enable( false );
#else

#ifndef __WXMAC__
	text_launchcommand->SetValidator( FilenameValidator( &cfg.pdfViewerCommand ));
#else
	text_launchcommand->SetValidator( StringValidator( &cfg.pdfViewerCommand ));
#endif
#endif

	Connect( CD_LAUNCH_COMMAND_BUTTON, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler( PdfConflet::OnChooseViewerCommand ));
}

/*****************************************************
**
**   PdfConflet   ---   updateUi
**
******************************************************/
void PdfConflet::updateUi()
{
	debuglog << "PdfConflet::updateUi";

#ifndef __WXMSW__
	button_launchcommand->Enable( cfg.launchPdfViewer );
	text_launchcommand->Enable( cfg.launchPdfViewer );
#endif
}

/*****************************************************
**
**   PdfConflet   ---   OnChooseViewerCommand
**
******************************************************/
void PdfConflet::OnChooseViewerCommand( wxCommandEvent& )
{
	wxFileDialog dialog( this, tr("Choose Pdf Viewer" ));

	if ( dialog.ShowModal() == wxID_OK )
	{
		cfg.pdfViewerCommand = dialog.GetPath();
		text_launchcommand->ChangeValue( cfg.pdfViewerCommand );
		guiHasChanged();
	}
}

