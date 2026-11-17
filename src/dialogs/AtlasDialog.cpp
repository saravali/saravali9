/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AtlasDialog.cpp
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

#include "AtlasDialog.h"

#include "AtlasEditDialog.h"
#include "AtlasImportDialog.h"
#include "AtlasLogic.h"
#include "Formatter.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "constants.h"
#include "mathbase.h"

#include <math.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/grid.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/tokenzr.h>

extern Gdiconfig *gdiconfig;

enum { ATLAS_FILTER = wxID_HIGHEST + 1, ATLAS_COUNTRY, ATLAS_MODE, ATLAS_EDIT, ATLAS_DELETE, ATLAS_ADD, ATLAS_GRID, ATLAS_TIMER };

// Interval for lookup of new location data
#define TIMER_INTERVAL_MILLISEC 50

// begin wxGlade: ::extracode
// end wxGlade

#define ATLAS_GRID_MAXCOLS 5

/*****************************************************
**
**   AtlasDialogWrapper   ---   Constructor
**
******************************************************/
AtlasDialogWrapper::AtlasDialogWrapper( wxWindow *parent, const int &callerId, wxString s  )
	: callerId( callerId ),
	searchString( s ),
	parent( parent )
{
	dialog = 0;
}

/*****************************************************
**
**   AtlasDialogWrapper   ---   Destructor
**
******************************************************/
AtlasDialogWrapper::~AtlasDialogWrapper()
{
	if ( dialog ) delete dialog;
}

/*****************************************************
**
**   AtlasDialogWrapper   ---   run
**
******************************************************/
int AtlasDialogWrapper::run()
{
	bool startdialog = false;

	AtlasDao dao;
	bool dbok = dao.isDbStatusOkay();

	// check if the database is empty - query timezones as example
	if ( dbok )
	{
		const auto timezones = dao.getAllTimezones();
		if ( ! timezones.size() ) dbok = false;
	}
	
	if ( ! dbok )
	{
		AtlasImportDialog importDialog( parent, _( "First Time Import of Atlas Data" ));
		importDialog.setDefaultFiles();
		importDialog.run();

		int ret = importDialog.ShowModal();
		if ( ret == wxID_OK )
		{
			startdialog = true;
		}
	}
	else
	{
		startdialog = true;
	}
	if ( startdialog )
	{
		dialog = new AtlasDialog( parent, callerId, searchString );
		return dialog->ShowModal();
	}
	return wxID_CANCEL;
}

/*****************************************************
**
**   AtlasDialogWrapper   ---   getSelectedEntry
**
******************************************************/
AtlasEntry *AtlasDialogWrapper::getSelectedEntry()
{
	if ( dialog ) return &dialog->getSelectedEntry();
	else return (AtlasEntry*)NULL;
}

/*****************************************************
**
**   AtlasGridTable
**
******************************************************/
class AtlasGridTable : public wxGridTableBase
{
public:
	AtlasGridTable( wxGrid *view, AtlasLogic *logic )
	{
		SetView( view );
		this->logic = logic;
	}

	virtual wxString GetValue(int row, int col)
	{
		Formatter *formatter = Formatter::get();
		AtlasEntry *entry = logic->getEntryByRowId( row );
		if ( entry )
		{
			switch ( col )
			{
			case 0:
				return entry->name;
				break;
			case 1:
				return entry->country;
				break;
			case 2:
				return entry->admin;
				break;
			case 3:
				return formatter->formatLatitude( entry->latitude );
				break;
			case 4:
				return formatter->formatLongitude( entry->longitude );
				break;
			default:
				return "?";
				break;
			}
		}
		else
		{
			return col == 0 ? "..." : "";
		}
	}

	virtual bool AppendRows(size_t numRows = 1)
	{
		if ( GetView() )
		{
			wxGridTableMessage msg( this, wxGRIDTABLE_NOTIFY_ROWS_APPENDED, numRows );
			GetView()->ProcessTableMessage( msg );
		}
		return true;
	}

	virtual bool DeleteRows(size_t pos = 0, size_t numRows = 1)
	{
		if ( GetView() )
		{
			wxGridTableMessage msg( this, wxGRIDTABLE_NOTIFY_ROWS_DELETED, pos, numRows );
			GetView()->ProcessTableMessage( msg );
		}

		return true;
	}

	virtual int GetNumberRows() {
		return ATLAS_MAX_GRID_ELEMENTS;
	}
	virtual int GetNumberCols() {
		return ATLAS_GRID_MAXCOLS;
	}

	virtual bool IsEmptyCell(int, int) {
		return false;
	}
	virtual void SetValue(int, int, const wxString&) {}
 
	virtual wxString GetColLabelValue( int col )
	{
		switch ( col )
		{
		case 0:
			return _( "Name" );
			break;
		case 1:
			return _( "Country" );
			break;
		case 2:
			return _( "Admin Division" );
			break;
		case 3:
			return _( "Latitude" );
			break;
		case 4:
			return _( "Longitude" );
			break;
		default:
			return "Unknown";
			break;
		}
	}

private:
	AtlasLogic *logic;
};

/*****************************************************
**
**   AtlasDialog   ---   Constructor
**
******************************************************/
AtlasDialog::AtlasDialog(wxWindow* parent, const int &callerId, wxString searchString )
		:  wxDialog(parent, -1, "Atlas",  wxDefaultPosition, wxDefaultSize, DEFAULT_DIALOG_STYLE ),
		callerId( callerId ),
		configkey( "AtlasDialog" )
{
	logic = new AtlasLogic( true );

    // begin wxGlade: AtlasDialog::AtlasDialog
    SetTitle(_("Atlas"));
    wxFlexGridSizer* sizer_main = new wxFlexGridSizer(7, 1, 0, 0);
    wxFlexGridSizer* sizer_top = new wxFlexGridSizer(1, 3, 0, 0);
    sizer_main->Add(sizer_top, 1, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 6);
    wxStaticBoxSizer* sizer_filter = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Filter")), wxVERTICAL);
    sizer_top->Add(sizer_filter, 1, wxALL|wxEXPAND, 5);
    wxFlexGridSizer* grid_filter = new wxFlexGridSizer(1, 6, 3, 3);
    sizer_filter->Add(grid_filter, 1, wxALL|wxEXPAND, 3);
    label_filtername = new wxStaticText(this, wxID_ANY, _("Name"));
    grid_filter->Add(label_filtername, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    filter_edit = new wxTextCtrl(this, ATLAS_FILTER, wxEmptyString);
    filter_edit->SetMinSize(wxSize(200, -1));
    filter_edit->SetToolTip(_("\"*\" or \"%\" can be used for multiple character wildcards, \"?\" or \"_\" for single character wildcards"));
    grid_filter->Add(filter_edit, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);
    label_filtercountry = new wxStaticText(this, wxID_ANY, _("Country"));
    grid_filter->Add(label_filtercountry, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    const wxString country_choice_choices[] = {
        _("dummy"),
        _("dummy"),
        _("dummy"),
        _("dummy"),
    };
    country_choice = new wxChoice(this, ATLAS_COUNTRY, wxDefaultPosition, wxDefaultSize, 4, country_choice_choices);
    country_choice->SetSelection(0);
    grid_filter->Add(country_choice, 0, wxALL, 3);
    label_mode = new wxStaticText(this, wxID_ANY, _("Key Field"));
    grid_filter->Add(label_mode, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    const wxString choice_mode_choices[] = {
        _("Name"),
        _("ASCII Name"),
        _("Alias Name"),
    };
    choice_mode = new wxChoice(this, ATLAS_MODE, wxDefaultPosition, wxDefaultSize, 3, choice_mode_choices);
    choice_mode->SetSelection(0);
    grid_filter->Add(choice_mode, 0, wxALL, 3);
    locgrid = new wxGrid(this, wxID_ANY);
    locgrid->SetMinSize(wxSize(-1,200));
    sizer_main->Add(locgrid, 1, wxALL|wxEXPAND, 3);
    match_label = new wxStaticText(this, wxID_ANY, _("dummy"));
    sizer_main->Add(match_label, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 6);
    static_line_match = new wxStaticLine(this, wxID_ANY);
    static_line_match->SetMinSize(wxSize(640, 2));
    sizer_main->Add(static_line_match, 0, wxBOTTOM|wxTOP, 5);
    wxBoxSizer* sizer_4 = new wxBoxSizer(wxHORIZONTAL);
    sizer_main->Add(sizer_4, 1, wxEXPAND, 0);
    wxStaticBoxSizer* sizer_details = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Location")), wxHORIZONTAL);
    sizer_4->Add(sizer_details, 1, wxEXPAND, 0);
    wxBoxSizer* sizer_details2 = new wxBoxSizer(wxVERTICAL);
    sizer_details->Add(sizer_details2, 1, wxEXPAND, 0);
    wxFlexGridSizer* grid_details = new wxFlexGridSizer(5, 4, 0, 3);
    sizer_details2->Add(grid_details, 1, wxEXPAND, 0);
    label_name = new wxStaticText(this, wxID_ANY, _("Name"));
    label_name->SetMinSize(wxSize(100, 16));
    grid_details->Add(label_name, 0, wxALL, 3);
    text_name = new wxStaticText(this, wxID_ANY, _("dummy"));
    text_name->SetMinSize(wxSize(150, 16));
    grid_details->Add(text_name, 0, wxALL, 3);
    label_longitude = new wxStaticText(this, wxID_ANY, _("Longitude"));
    label_longitude->SetMinSize(wxSize(100, 16));
    grid_details->Add(label_longitude, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    text_longitude = new wxStaticText(this, wxID_ANY, _("dummy"));
    text_longitude->SetMinSize(wxSize(150, 16));
    grid_details->Add(text_longitude, 0, wxALL, 3);
    label_asciiname = new wxStaticText(this, wxID_ANY, _("ASCII Name"));
    label_asciiname->SetMinSize(wxSize(100, 17));
    grid_details->Add(label_asciiname, 0, wxALL, 3);
    text_asciiname = new wxStaticText(this, wxID_ANY, _("dummy"));
    text_asciiname->SetMinSize(wxSize(150, 17));
    grid_details->Add(text_asciiname, 0, wxALL, 3);
    label_latitude = new wxStaticText(this, wxID_ANY, _("Latitude"));
    label_latitude->SetMinSize(wxSize(100, 17));
    grid_details->Add(label_latitude, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    text_latitude = new wxStaticText(this, wxID_ANY, _("dummy"));
    text_latitude->SetMinSize(wxSize(150, 16));
    grid_details->Add(text_latitude, 0, wxALL, 3);
    label_country = new wxStaticText(this, wxID_ANY, _("Country"));
    label_country->SetMinSize(wxSize(100, 16));
    grid_details->Add(label_country, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    text_country = new wxStaticText(this, wxID_ANY, _("dummy"));
    text_country->SetMinSize(wxSize(150, 16));
    grid_details->Add(text_country, 0, wxALL, 3);
    label_timezone = new wxStaticText(this, wxID_ANY, _("Time Zone"));
    label_timezone->SetMinSize(wxSize(100, 16));
    grid_details->Add(label_timezone, 0, wxALL, 3);
    text_timezone = new wxStaticText(this, wxID_ANY, _("dummy"));
    text_timezone->SetMinSize(wxSize(150, 16));
    grid_details->Add(text_timezone, 0, wxALL, 3);
    label_admincode = new wxStaticText(this, wxID_ANY, _("Admin Code"));
    label_admincode->SetMinSize(wxSize(100, 16));
    grid_details->Add(label_admincode, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    text_admincode = new wxStaticText(this, wxID_ANY, _("dummy"));
    text_admincode->SetMinSize(wxSize(150, 16));
    grid_details->Add(text_admincode, 0, wxALL, 3);
    label_tzoffset = new wxStaticText(this, wxID_ANY, _("TZ Hours"));
    label_tzoffset->SetMinSize(wxSize(100, 16));
    grid_details->Add(label_tzoffset, 0, wxALL, 3);
    text_tzoffset = new wxStaticText(this, wxID_ANY, _("dummy"));
    text_tzoffset->SetMinSize(wxSize(150, 16));
    grid_details->Add(text_tzoffset, 0, wxALL, 3);
    label_population = new wxStaticText(this, wxID_ANY, _("Population"));
    label_population->SetMinSize(wxSize(100, 16));
    grid_details->Add(label_population, 0, wxALL, 3);
    text_population = new wxStaticText(this, wxID_ANY, _("dummy"));
    text_population->SetMinSize(wxSize(150, 16));
    grid_details->Add(text_population, 0, wxALL, 3);
    label_id = new wxStaticText(this, wxID_ANY, _("Internal ID"));
    label_id->SetMinSize(wxSize(100, 16));
    grid_details->Add(label_id, 0, wxALL, 3);
    text_id = new wxStaticText(this, wxID_ANY, _("dummy"));
    text_id->SetMinSize(wxSize(150, 16));
    grid_details->Add(text_id, 0, wxALL, 3);
    label_alias = new wxStaticText(this, wxID_ANY, _("Alias Names"));
    label_alias->SetMinSize(wxSize(100, 16));
    sizer_details2->Add(label_alias, 0, wxALL, 3);
    text_aliases = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
    text_aliases->SetMinSize(wxSize(-1, 100));
    sizer_details2->Add(text_aliases, 0, wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_actions = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Actions")), wxVERTICAL);
    sizer_4->Add(sizer_actions, 0, wxEXPAND, 0);
    button_add = new wxButton(this, ATLAS_ADD, _("New Entry"));
    sizer_actions->Add(button_add, 0, wxALL|wxEXPAND, 3);
    button_edit = new wxButton(this, ATLAS_EDIT, _("Edit Entry"));
    sizer_actions->Add(button_edit, 0, wxALL|wxEXPAND, 3);
    button_delete = new wxButton(this, ATLAS_DELETE, _("Delete Entry"));
    sizer_actions->Add(button_delete, 0, wxALL|wxEXPAND, 3);
    static_line_button = new wxStaticLine(this, wxID_ANY);
    sizer_main->Add(static_line_button, 0, wxALL|wxEXPAND, 5);
    wxFlexGridSizer* sizer_button = new wxFlexGridSizer(1, 5, 0, 0);
    sizer_main->Add(sizer_button, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
    button_ok = new wxButton(this, wxID_OK, _("OK"));
    sizer_button->Add(button_ok, 0, wxALL, 3);
    button_cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
    sizer_button->Add(button_cancel, 0, wxALL, 3);
    
    grid_details->AddGrowableCol(1);
    grid_details->AddGrowableCol(3);
    grid_filter->AddGrowableCol(1);
    grid_filter->AddGrowableCol(3);
    grid_filter->AddGrowableCol(5);
    sizer_top->AddGrowableCol(0);
    sizer_main->AddGrowableRow(1);
    sizer_main->AddGrowableCol(0);
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    Layout();
    // end wxGlade

	gridTable = new AtlasGridTable( locgrid, logic );
	locgrid->SetTable( gridTable, true );

	if ( gdiconfig->atlas.gridColumnSizes.size() == ATLAS_GRID_MAXCOLS + 1 )
	{
		locgrid->SetRowLabelSize( gdiconfig->atlas.gridColumnSizes[0] );
		for( uint i = 0; i < ATLAS_GRID_MAXCOLS; i++ )
		{
			for ( i = 0; i < ATLAS_GRID_MAXCOLS; i++ )
			{
				locgrid->SetColSize( i, gdiconfig->atlas.gridColumnSizes[i+1] );
			}
		}
	}

	locgrid->SetSelectionMode( wxGrid::wxGridSelectRows );
	locgrid->EnableEditing( false );
	locgrid->SetRowLabelSize( 50 );

	selectedRow = -1;

	if ( callerId == ATLASDIALOG_CALLER_APPLICATIONWINDOW )
	{
		button_cancel->Show( false );
	}
	updateCountryChoice();
	choice_mode->SetSelection( gdiconfig->atlas.filterMode );

	Connect( ATLAS_FILTER, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AtlasDialog::OnFilterChanged ));
	Connect( ATLAS_COUNTRY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AtlasDialog::OnFilterChanged ));
	Connect( ATLAS_MODE, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AtlasDialog::OnFilterChanged ));

	Connect( wxEVT_GRID_SELECT_CELL, wxGridEventHandler( AtlasDialog::OnCellChange ));
	Connect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( AtlasDialog::OnCellDClick ));

  Connect( ATLAS_TIMER, wxEVT_TIMER, wxTimerEventHandler( AtlasDialog::OnTimer ));

	Connect( ATLAS_ADD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasDialog::OnAddEntry ));
	Connect( ATLAS_EDIT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasDialog::OnEditEntry ));
	Connect( ATLAS_DELETE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AtlasDialog::OnDeleteEntry ));
	Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( AtlasDialog::OnKeyDown ));

	timer = new wxTimer( this, ATLAS_TIMER );
	timer->Start( TIMER_INTERVAL_MILLISEC );

	if ( ! searchString.IsEmpty())
	{
		// reset country and avoid empty list view
		country_choice->SetSelection( 0 );
		filter_edit->SetValue( searchString );
		filter_edit->SetFocus();
	}

	setSavedWindowSize( this, configkey );

	logic->setFilterConditions(
		filter_edit->GetValue(),
		getSelectedCountryISO(),
		choice_mode->GetSelection());
	updateEntryDetails();
}

/*****************************************************
**
**   AtlasDialog   ---   Destructor
**
******************************************************/
AtlasDialog::~AtlasDialog()
{
	if ( timer->IsRunning()) timer->Stop();
	delete timer;

	// Save grid column width
	gdiconfig->atlas.gridColumnSizes.clear();
	gdiconfig->atlas.gridColumnSizes.push_back( locgrid->GetRowLabelSize());

	for ( int i = 0; i < ATLAS_GRID_MAXCOLS; i++ )
	{
		gdiconfig->atlas.gridColumnSizes.push_back( locgrid->GetColSize( i ));
	}

	gdiconfig->atlas.lastSelectedCountry = getSelectedCountryISO();
	gdiconfig->atlas.filterMode = choice_mode->GetSelection();

	saveWindowSize( this, configkey );
	delete logic;
}

/*****************************************************
**
**   AtlasDialog   ---   updateCountryChoice
**
******************************************************/
void AtlasDialog::updateCountryChoice()
{
	country_choice->Clear();
	country_choice->Append( _( "All Countries" ));

	int selCountry = 0;
	int i = 1;
	for( const auto &a : logic->getFavouriteCountries( {} ))
	{
		wxStringClientData *clientData = new wxStringClientData( a.iso );
		country_choice->Append( a.name, clientData );
		if ( gdiconfig->atlas.lastSelectedCountry == a.iso)
		{
			selCountry = i;
		}
		i++;
	}
	country_choice->SetSelection( selCountry );
}

/*****************************************************
**
**   AtlasDialog   ---   OnFilterChanged
**
******************************************************/
void AtlasDialog::OnFilterChanged( wxCommandEvent& )
{
	logic->setFilterConditions(
		filter_edit->GetValue(),
		getSelectedCountryISO(),
		choice_mode->GetSelection());

	wxString filter = filter_edit->GetValue();
	selectedRow = -1;
	updateEntryDetails();

	updateFilterMatchLabel( -1 );
	locgrid->Scroll( 0, 0 );
}

/*****************************************************
**
**   AtlasDialog   ---   OnCellChange
**
******************************************************/
void AtlasDialog::OnCellChange( wxGridEvent& event )
{
	selectedRow = event.GetRow();
	updateEntryDetails();
	event.Skip();
}

/*****************************************************
**
**   AtlasDialog   ---   OnCellDClick
**
******************************************************/
void AtlasDialog::OnCellDClick( wxGridEvent& event )
{
	selectedRow = event.GetRow();
	if ( callerId == ATLASDIALOG_CALLER_DATADIALOG || callerId == ATLASDIALOG_CALLER_DEFAULTLOCATIONPANEL )
	{
		EndModal( wxID_OK );
	}
	else editEntry();
}

/*****************************************************
**
**   AtlasDialog   ---   OnSelectLabel
**
******************************************************/
void AtlasDialog::OnSelectLabel( wxGridEvent& event )
{
	// TODO
	selectedRow = event.GetRow();
}

/*****************************************************
**
**   AtlasDialog   ---   getSelectedCountryISO
**
******************************************************/
wxString AtlasDialog::getSelectedCountryISO()
{
	wxString iso;
	int sel = country_choice->GetSelection();
	if ( sel != -1 )
	{
		wxString name = country_choice->GetStringSelection();
		wxClientData *clientData = country_choice->GetClientObject( sel );
		if ( clientData != 0 )
		{
			iso = ((wxStringClientData*)clientData)->GetData();
		}
	}
	return iso;
}

/*****************************************************
**
**   AtlasDialog   ---   updateFilterMatchLabel
**
******************************************************/
void AtlasDialog::updateFilterMatchLabel( const int &count )
{
	wxString s;

	if ( count > 0 ) s.Printf( _( "%d entries" ), count );
	else if ( count == 0 ) s = _( "No match" );

	match_label->SetLabel( s );
}

/*****************************************************
**
**   AtlasDialog   ---   updateEntryDetails
**
******************************************************/
void AtlasDialog::updateEntryDetails()
{
	Formatter *formatter = Formatter::get();

	AtlasEntry *e = logic->getEntryByRowId( selectedRow );

	button_edit->Enable( e != 0 );
	button_delete->Enable( e != 0 );

	if ( e )
	{
		selectedEntry = logic->getFullEntry( e->id );
		text_name->SetLabel( selectedEntry.name );
		text_asciiname->SetLabel( selectedEntry.asciiname );
		text_country->SetLabel( selectedEntry.country );
		text_admincode->SetLabel( selectedEntry.admin );
		text_id->SetLabel( getIntOrEmtpy( selectedEntry.id ));
		text_population->SetLabel( getIntOrEmtpy( selectedEntry.population ));

		if ( selectedEntry.latitude != 0 )
			text_latitude->SetLabel( formatter->formatLatitude( selectedEntry.latitude ));
		else text_latitude->SetLabel( wxEmptyString );
		if ( selectedEntry.longitude != 0 )
			text_longitude->SetLabel( formatter->formatLongitude( selectedEntry.longitude ));
		else text_longitude->SetLabel( wxEmptyString );

		text_timezone->SetLabel( selectedEntry.timezone );
		text_tzoffset->SetLabel( wxString::Format( "%.1f", selectedEntry.tzoffset ));
		text_aliases->SetValue( selectedEntry.aliases );
	}
	else
	{
		text_name->SetLabel( wxEmptyString );
		text_asciiname->SetLabel( wxEmptyString );
		text_country->SetLabel( wxEmptyString );
		text_admincode->SetLabel( wxEmptyString );
		text_id->SetLabel( wxEmptyString );
		text_population->SetLabel( wxEmptyString );
		text_latitude->SetLabel( wxEmptyString );
		text_longitude->SetLabel( wxEmptyString );
		text_timezone->SetLabel( wxEmptyString );
		text_tzoffset->SetLabel( wxEmptyString );
		text_aliases->SetValue( wxEmptyString );
	}
}

/*****************************************************
**
**   AtlasDialog   ---   OnIdle
**
******************************************************/
void AtlasDialog::OnIdle( wxIdleEvent& )
{
	checkNews();
}

/*****************************************************
**
**   AtlasDialog   ---   OnTimer
**
******************************************************/
void AtlasDialog::OnTimer( wxTimerEvent& )
{
	checkNews();
}

/*****************************************************
**
**   AtlasDialog   ---   checkNews
**
******************************************************/
void AtlasDialog::checkNews()
{
	if ( logic->fetchHasNews())
	{
		logic->aknowledgeFetchHasNews();
		locgrid->Refresh();
		locgrid->SelectRow( 0 );
		selectedRow = 0;
		updateEntryDetails();
	}

	if ( logic->countHasNews())
	{
		logic->aknowledgeCountHasNews();
		int oldrows = locgrid->GetNumberRows();
		int newrows = logic->getCount();
		if ( newrows > oldrows )
		{
			locgrid->AppendRows( newrows - oldrows );
		}
		else if ( newrows < oldrows )
		{
			locgrid->DeleteRows( 0, oldrows - newrows );
		}
		updateFilterMatchLabel( logic->getCount());
	}
}

/*****************************************************
**
**   AtlasDialog   ---   OnKeyDown
**
******************************************************/
void AtlasDialog::OnKeyDown( wxKeyEvent &event )
{
	switch ( event.GetKeyCode())
	{
		case WXK_ESCAPE:
			Close();
		break;
		default:
			event.Skip();
		break;
	}
}

/*****************************************************
**
**   AtlasDialog   ---   OnAddEntry
**
******************************************************/
void AtlasDialog::OnAddEntry( wxCommandEvent& )
{
	AtlasEntry entry = selectedEntry;
	entry.id = 0;
	{
		entry.name = tr( "Copy of" ) + " " + selectedEntry.name;
	}
	{
		entry.asciiname = tr( "Copy of" ) + " " + selectedEntry.asciiname;
	}
	AtlasEditDialog dialog( this,  logic, entry );
	if ( dialog.ShowModal() == wxID_OK )
	{
		entry = dialog.getEntry();
		logic->saveEntry( entry );
		logic->updateFilter();
		locgrid->Refresh();
	}
}

/*****************************************************
**
**   AtlasDialog   ---   OnEditEntry
**
******************************************************/
void AtlasDialog::OnEditEntry( wxCommandEvent& )
{
	editEntry();
}

/*****************************************************
**
**   AtlasDialog   ---   editEntry
**
******************************************************/
void AtlasDialog::editEntry()
{
	AtlasEditDialog dialog( this, logic, selectedEntry );
	if ( dialog.ShowModal() == wxID_OK )
	{
		selectedEntry = dialog.getEntry();
		logic->saveEntry( selectedEntry );
		logic->updateFilter();
		locgrid->Refresh();
	}
}

/*****************************************************
**
**   AtlasDialog   ---   OnDeleteEntry
**
******************************************************/
void AtlasDialog::OnDeleteEntry( wxCommandEvent& )
{
	if ( doMessageBox( this, wxString::Format( tr( "Delete %s?" ), selectedEntry.name ), wxYES_NO | wxICON_QUESTION ))
	{
		logic->deleteEntry( selectedEntry.id );
		logic->updateFilter();
		selectedRow = -1;
		updateEntryDetails();
		locgrid->Scroll( 0, 0 );
	}
}

