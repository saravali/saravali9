/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/PdfLauncher.cpp
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

#include "PdfLauncher.h"

#include "gdiconfig.h"
#include "FileConfig.h"
#include "guibase.h"
#include "Horoscope.h"
#include "PdfGenerator.h"
#include "Sheet.h"

#include <wx/choicdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#ifdef __WXMSW__
#include <wx/mimetype.h>
#endif

#include <format>

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   PdfLauncher   ---   Constructor
**
******************************************************/
PdfLauncher::PdfLauncher() : printoutCfg( gdiconfig->printout )
{
}

/*****************************************************
**
**   PdfLauncher   ---   Constructor
**
******************************************************/
PdfLauncher::PdfLauncher( const PrintoutConfig &gcfg ) : printoutCfg( gcfg )
{
}

/*****************************************************
**
**   PdfLauncher   ---   printTestpage
**
******************************************************/
void PdfLauncher::printTestpage()
{
	Horoscope_ptr h = Horoscope_ptr( new Horoscope );
	h->update();

	const wxString filename = getTestpageFilename();
	printHoroscope( h, filename );
}

/*****************************************************
**
**   PdfLauncher   ---   printTestpage
**
******************************************************/
void PdfLauncher::printTestpage( Horoscope_ptr horoscope, ContextProps_ptr )
{
	const wxString filename = getTestpageFilename();
	debuglog << "Tempfile name " << filename;

	PdfGenerator( horoscope, printoutCfg, gdiconfig->page ).generatePdf( filename );

	if ( gdiconfig->pdf.launchPdfViewer ) launchViewer( filename );
	else
	{
		const wxString s = tr( "Chart printed to file" ) + " " + filename;
		doMessageBox( nullptr, s, wxOK | wxCENTRE );
	}
}

/*****************************************************
**
**   PdfLauncher   ---   selectPrintoutId
**
******************************************************/
bool PdfLauncher::selectPrintoutId( int &type )
{
	const wxString configkey = "PrintoutSelectionDialog";
	SheetSpecLoader *loader = SheetSpecLoader::get();

	wxArrayString choices;
	for( const auto &name : loader->getNames()) choices.Add( name );

	wxSingleChoiceDialog dialog( 0, _( "Select Printout" ), GUI_APP_NAME, choices );
	setSavedWindowSize( &dialog, configkey );
	dialog.SetSelection( printoutCfg.sheetId );
	if ( dialog.ShowModal() == wxID_OK )
	{
		saveWindowSize( &dialog, configkey );
		type = dialog.GetSelection();
		return true;
	}
	return printoutCfg.sheetId;
}

/*****************************************************
**
**   PdfLauncher   ---   selectFilename
**
******************************************************/
bool PdfLauncher::selectFilename( wxString &filename, const wxString &basename )
{
	const static wxString filetypes = "Portable Document Format (*.pdf)|*.pdf|All files (*)| *.*";

	// override prompt is handled later
	wxFileDialog exportFileDialog( NULL,
		_("Save PDF Document" ),
		gdiconfig->prefs.defExportPath,
		basename,
		filetypes,
		wxFD_SAVE, // | wxFD_OVERWRITE_PROMPT,
		wxDefaultPosition );
	if ( exportFileDialog.ShowModal() == wxID_OK )
	{
		filename = exportFileDialog.GetDirectory() + wxFileName::GetPathSeparator() + exportFileDialog.GetFilename();
		gdiconfig->prefs.defExportPath = exportFileDialog.GetDirectory();
		return true;
	}
	return false;
}

/*****************************************************
**
**   PdfLauncher   ---   printHoroscope
**
******************************************************/
void PdfLauncher::printHoroscope( Horoscope_ptr horoscope, const bool doSelect )
{
	debuglog << "PdfLauncher::printHoroscope select " << doSelect;

	if ( doSelect )
	{
		if ( ! selectPrintoutId( printoutCfg.sheetId )) return;
	}

	// set name in horoscope if empty
	if ( horoscope->dl.name.empty())
	{
		horoscope->dl.name = tr( "unknown" );
	}

	wxString basename = horoscope->dl.name + ".pdf";
	wxString filename;
	if ( gdiconfig->pdf.askOverwrite )
	{
		if ( ! selectFilename( filename, basename )) return;
	}
	else
	{
		filename = gdiconfig->prefs.defExportPath;
		if ( ! gdiconfig->prefs.defExportPath.empty())
		{
			filename += wxFileName::GetPathSeparator();
		}
		filename += basename;
	}
	printHoroscope( horoscope, filename );
}

/*****************************************************
**
**   PdfLauncher   ---   printHoroscope
**
******************************************************/
void PdfLauncher::printHoroscope( Horoscope_ptr horoscope, wxString filename )
{
	try
	{
		PdfGenerator( horoscope, printoutCfg, gdiconfig->page ).generatePdf( filename );
		finishPrint( filename );
	}
	catch( const std::exception &e )
	{
		errorlog << "cannot print document " << e.what();
	}
}

/*****************************************************
**
**   PdfLauncher   ---   finishPrint
**
******************************************************/
bool PdfLauncher::finishPrint( wxString filename )
{
	if ( gdiconfig->pdf.launchPdfViewer ) return launchViewer( filename );

	else
	{
		wxString message =  tr( "Chart printed to file" ) + " " + filename;
		doMessageBox( nullptr, message, wxOK | wxCENTRE );
	}
	return false;
}

/*****************************************************
**
**   PdfLauncher   ---   getTestpageFilename
**
******************************************************/
wxString PdfLauncher::getTestpageFilename() const
{
	return wxFileName::CreateTempFileName( "testpage" ) + ".pdf";
}

/*****************************************************
**
**   PdfLauncher   ---   launchViewer
**
******************************************************/
bool PdfLauncher::launchViewer( wxString filename )
{
	wxString command;

#ifdef __WXMSW__
	wxFileType *filetype = wxMimeTypesManager().GetFileTypeFromExtension( "pdf" );
	if ( filetype != 0 )
	{
		command = filetype->GetOpenCommand( filename );
	}
	if ( filetype == 0 || command.IsEmpty())
	{
		doMessageBox( 0, _( "Cannot launch pdf viewer." ), wxOK | wxICON_ERROR | wxCENTRE );
		return false;
	}
#else

	if ( gdiconfig->pdf.pdfViewerCommand.empty())
	{
		gdiconfig->pdf.pdfViewerCommand = FileConfig::get()->getPdfViewerDefaultFilename();
	}

	if ( gdiconfig->pdf.pdfViewerCommand.empty())
	{
		doMessageBox( 0,
			_( "Pdf viewer command is empty. Please configure the command in the configuration dialog." ),
			wxOK | wxICON_ERROR | wxCENTRE );
		return false;
	}

	// wxmac has generic open command
#ifndef __WXMAC__
	else if ( ! wxFileName::FileExists( gdiconfig->pdf.pdfViewerCommand ))
	{
		doMessageBox( 0,
			wxString::Format( tr( "Pdf viewer file \"%s\" does not exist. Please configure the command in the configuration dialog." ),
				gdiconfig->pdf.pdfViewerCommand, wxOK | wxICON_ERROR | wxCENTRE ));
		return false;
	}
#endif

	command << gdiconfig->pdf.pdfViewerCommand << " \"" << filename << "\"";
#endif
	long r = wxExecute( command );
	if ( r == -1 )
	{
		doMessageBox( 0,
			wxString::Format( tr( "Can't launch pdf viewer, command is \"%s\" (error code %ld)" ), command, r ),
			wxOK | wxICON_ERROR | wxCENTRE );
		return false;
	}
	return true;
}


