/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AtlasConflet.cpp
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

#include "AtlasConflet.h"

#include "AtlasDao.h"
#include "AtlasImportDialog.h"
#include "AtlasLogic.h"
#include "guibase.h"
#include "logger.h"
#include "TextValidator.h"

#include <wx/button.h>
#include <wx/choicdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum { AD_TEXTDBFILE = wxID_HIGHEST + 1, AD_COUNTRIES, AD_CHOOSEDBFILE, AD_TEXTSQLFILE, AD_CHOOSESQLFILE, AD_RUNIMPORT };

/*****************************************************
**
**   AtlasConflet   ---   Constructor
**
******************************************************/
AtlasConflet::AtlasConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->atlas )
{
	// begin wxGlade: AtlasConflet::AtlasConflet
	wxFlexGridSizer* sizer_main = new wxFlexGridSizer(1, 2, 0, 0);
	wxFlexGridSizer* sizer_left = new wxFlexGridSizer(2, 1, 0, 0);
	sizer_main->Add(sizer_left, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_db = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Database File")), wxVERTICAL);
	sizer_left->Add(sizer_db, 1, wxALL|wxEXPAND, 3);
	button_dbfile = new wxButton(sizer_db->GetStaticBox(), AD_CHOOSEDBFILE, _("Choose ..."));
	sizer_db->Add(button_dbfile, 0, wxALL, 3);
	text_dbfile = new wxTextCtrl(sizer_db->GetStaticBox(), AD_TEXTDBFILE, wxEmptyString);
	sizer_db->Add(text_dbfile, 0, wxALL|wxEXPAND, 3);
	text_dbstatus = new wxStaticText(sizer_db->GetStaticBox(), wxID_ANY, _("Status"));
	text_dbstatus->SetMinSize(wxSize(-1,-1));
	sizer_db->Add(text_dbstatus, 0, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_import = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Database  Import")), wxVERTICAL);
	sizer_left->Add(sizer_import, 1, wxEXPAND, 0);
	button_sqlfile = new wxButton(sizer_import->GetStaticBox(), AD_CHOOSESQLFILE, _("Choose ..."));
	sizer_import->Add(button_sqlfile, 0, wxALL, 3);
	text_sqlfile = new wxTextCtrl(sizer_import->GetStaticBox(), AD_TEXTSQLFILE, wxEmptyString);
	sizer_import->Add(text_sqlfile, 0, wxALL|wxEXPAND, 3);
	text_sqlfilestatus = new wxStaticText(sizer_import->GetStaticBox(), wxID_ANY, _("Status"));
	text_sqlfilestatus->SetMinSize(wxSize(-1,-1));
	sizer_import->Add(text_sqlfilestatus, 1, wxALL, 3);
	button_startimport = new wxButton(sizer_import->GetStaticBox(), AD_RUNIMPORT, _("Start Import"));
	sizer_import->Add(button_startimport, 0, wxALL, 3);
	wxFlexGridSizer* sizer_right = new wxFlexGridSizer(1, 1, 0, 0);
	sizer_main->Add(sizer_right, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_favcountries = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Manage Favourite Countries")), wxVERTICAL);
	sizer_right->Add(sizer_favcountries, 1, wxEXPAND, 0);
	const wxString list_countries_choices[] = {
		_("dummy"),
	};
	list_countries = new wxListBox(sizer_favcountries->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 1, list_countries_choices, 0);
	list_countries->SetMinSize(wxSize(-1, 200));
	sizer_favcountries->Add(list_countries, 1, wxALL|wxEXPAND, 3);
	button_countries = new wxButton(sizer_favcountries->GetStaticBox(), AD_COUNTRIES, _("Change Countries ..."));
	sizer_favcountries->Add(button_countries, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
	
	sizer_right->AddGrowableRow(0);
	sizer_right->AddGrowableCol(0);
	sizer_left->AddGrowableCol(0);
	sizer_main->AddGrowableRow(0);
	sizer_main->AddGrowableCol(0);
	sizer_main->AddGrowableCol(1);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	text_dbfile->SetValidator( FilenameValidator( &cfg.databaseFile ));
	text_sqlfile->SetValidator( FilenameValidator( &cfg.sqlFile ));

	Connect( AD_CHOOSEDBFILE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasConflet::OnChooseDbFile ));

	Connect( AD_CHOOSESQLFILE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasConflet::OnChooseSqlFile ));
	Connect( AD_RUNIMPORT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasConflet::OnStartImport ));

	Connect( AD_COUNTRIES, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasConflet::OnCountries ));

	updateCountryList();
}

/*****************************************************
**
**   AtlasConflet   ---   updateUi
**
******************************************************/
void AtlasConflet::updateUi()
{
	debuglog << "AtlasConflet::OnCommand";
	wxString message;

	wxString sqlfile = text_sqlfile->GetValue();
	sqlfile.Trim( true ).Trim( false );
	bool okstatus = getFileStatus( sqlfile, message );
	//debuglog << "sql file " << sqlfile << " okstatus " << okstatus << " message " << message;
	text_sqlfilestatus->SetLabel( message );

	wxString dbfile = text_dbfile->GetValue();
	dbfile.Trim( true ).Trim( false );
	okstatus = getFileStatus( dbfile, message );
	//debuglog << "db file " << dbfile << " okstatus " << okstatus << " message " << message;

	if ( okstatus )
	{
		AtlasDao dao( dbfile );
		okstatus = dao.isDbStatusOkay();
		if ( ! okstatus ) message = tr( "Database error" );
	}
	text_dbstatus->SetLabel( message );
}

/*****************************************************
**
**   AtlasConflet   ---   OnChooseDbFile
**
******************************************************/
void AtlasConflet::OnChooseDbFile( wxCommandEvent& )
{
	static const wxString filetypes = "SQLite database (*.db)|*.db| All files (*.*)|*.*";
	wxFileName fn( text_dbfile->GetValue() );
	wxFileDialog dialog( this, _( "Select database file"), fn.GetPath(), fn.GetFullName(), filetypes,
		wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition );

	if (dialog.ShowModal() == wxID_OK )
	{
		text_dbfile->SetValue( dialog.GetPath());
		guiHasChanged();
	}
}

/*****************************************************
**
**   AtlasConflet   ---   restoreDefaults
**
******************************************************/
void AtlasConflet::restoreDefaults()
{
	cfg = AtlasConfig();
	updateCountryList();
	guiHasChanged();
}

/*****************************************************
**
**   AtlasConflet   ---   getFileStatus
**
******************************************************/
bool AtlasConflet::getFileStatus( wxString filename, wxString &message )
{
	debuglog << "AtlasConflet::getFileStatus filename " << filename;
	message = tr( "Status" ) + ": ";

	if ( ! wxFileName::FileExists( filename ))
	{
		message += tr( "File not found" );
		return false;
	}
	else if ( ! wxFileName::IsFileReadable( filename ))
	{
		message += tr( "File not readable" );
		return false;
	}
	else if ( wxFileName::GetSize( filename ) == 0 )
	{
		message += tr( "File size zero" );
		return false;
	}
	message += tr( "Ok" );
	return true;
}

/*****************************************************
**
**   AtlasConflet   ---   OnChooseSqlFile
**
******************************************************/
void AtlasConflet::OnChooseSqlFile( wxCommandEvent& )
{
	static const wxString filetypes = "SQL file (*.sql)|*.sql| All files (*.*)|*.*";

	wxFileName fn( text_sqlfile->GetValue());
	wxFileDialog dialog( this, _( "Select import file"), fn.GetPath(),  fn.GetFullName(),
		filetypes, wxFD_OPEN, wxDefaultPosition );

	if (dialog.ShowModal() == wxID_OK )
	{
		text_sqlfile->SetValue( dialog.GetPath());
		guiHasChanged();
	}
}

/*****************************************************
**
**   AtlasConflet   ---   OnStartImport
**
******************************************************/
void AtlasConflet::OnStartImport( wxCommandEvent& )
{
	AtlasImportDialog importDialog( this );
	bool ok = importDialog.setData( text_sqlfile->GetValue(), text_dbfile->GetValue());
	if ( ok )
	{
		importDialog.run();
		int result = importDialog.ShowModal();
		if ( result == wxID_OK )
		{
			doMessageBox( this, _( "Import was successfull" ));
		}
		else if ( result == wxID_CANCEL )
		{
			doMessageBox( this, _( "Import was canceled" ), wxOK | wxICON_HAND );
		}
		else if ( result == wxID_ABORT )
		{
			wxString message;
			message << _( "Import had errors" ) << "\n"
				<< importDialog.getErrorMessage();
			doMessageBox( this, message,  wxOK | wxICON_ERROR );
		}
	}
	else
	{
		errorlog << "file status not okay";
	}
	updateCountryList();
}

/*****************************************************
**
**   AtlasConflet   ---   updateSqlFileStatus
**
******************************************************/
void AtlasConflet::updateSqlFileStatus()
{
	wxString message;
	wxString sqlfile = text_sqlfile->GetValue();
	bool okstatus = getFileStatus( sqlfile, message );
	text_sqlfilestatus->SetLabel( message );
	button_startimport->Enable( okstatus );
}

/*****************************************************
**
**   AtlasConflet   ---   updateCountryList
**
******************************************************/
void AtlasConflet::updateCountryList()
{
	list_countries->Clear();

	AtlasLogic logic;
	std::vector<wxString> vv;
	for( auto country : cfg.favouriteCountries )
	{
		vv.push_back( country );
	}
	const auto l = logic.getFavouriteCountries( vv );
	for( const auto &a :l )
	{
		list_countries->Append( a.name );
	}
	list_countries->Refresh();
	button_countries->Enable( l.size() > 0 );
}

/*****************************************************
**
**   AtlasConflet   ---   OnCountries
**
******************************************************/
void AtlasConflet::OnCountries( wxCommandEvent& )
{
	AtlasLogic logic;
	wxArrayInt fav;
	const auto l = logic.getAllCountries();

	wxArrayString countrylabels;
	countrylabels.Add( wxEmptyString, l.size());

	int i = 0;
	for( const auto &a : l )
	{
		countrylabels[i++] = a.name;
	}
	wxMultiChoiceDialog dialog( this,	_( "Choose Favourite Countries" ), GUI_APP_NAME, countrylabels );

	for( uint i = 0; i < cfg.favouriteCountries.size(); i++ )
	{
		wxString iso = cfg.favouriteCountries[i];
		int count = 0;
		for( const auto &a : l )
		{
			if ( a.iso == iso )
			{
				fav.Add( count );
				break;
			}
			count++;
		}

	}
	dialog.SetSelections( fav );

	if ( dialog.ShowModal() == wxID_OK )
	{
		wxArrayInt a = dialog.GetSelections();
		cfg.favouriteCountries.clear();
		for ( uint i = 0; i < a.Count(); i++ )
		{
			int count = 0;
			for( const auto &l1 : l )
			{
				if ( count++ == a[i] )
				{
					cfg.favouriteCountries.push_back( l1.iso );
				}
			}
		}
		guiHasChanged();
	}
	updateCountryList();
}


