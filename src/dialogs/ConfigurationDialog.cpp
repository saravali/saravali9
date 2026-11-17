/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ConfigurationDialog.cpp
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

#include "ConfigurationDialog.h"

#include "Commands.h"
#include "ConfletFactory.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "ImageProvider.h"
#include "logger.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/treebook.h>

extern Gdiconfig *gdiconfig;

enum { CONFIG_NOTEBOOK = wxID_HIGHEST + 3000, CD_RESTORE_DEFAULTS };

/*****************************************************
**
**   ConfigurationDialog   ---   Constructor
**
******************************************************/
ConfigurationDialog::ConfigurationDialog( wxWindow* parent, const ConfletSet &confletSet )
	: wxDialog(parent, -1, "", wxDefaultPosition, wxDefaultSize, DEFAULT_DIALOG_STYLE )
{
	configkey =  "ConfigurationDialog::" + enum_to_string( confletSet );

	ConfletFactory *factory = ConfletFactory::get();
	nodes = factory->calculateTreenodes( confletSet );
	debuglog << "ConfigurationDialog conflet set " << (int)confletSet
		<< " rootnode children size " << nodes.size();

	activePanelIndex = -1;
	notebook = nullptr;
	wxWindow *contentsWindow = nullptr;

	assert( nodes.size() > 0 );
	if ( nodes.size() > 1 )
	{
		notebook = new wxTreebook( this, CONFIG_NOTEBOOK );
		for( const auto &node : nodes )
		{
			// the page is lazy loaded, conflet will be initilaized when required 
			wxPanel *panel = new wxPanel( notebook, -1 );

			wxString title = node.title;
			if ( title.empty())
			{
				title = factory->getConfletTitle( node.id );
			}
			//notebook->AddPage( panel, factory->getPageTitle( node.id ));
			notebook->AddPage( panel, title );
			toc.push_back( ConfigdialogPage( node, panel ));

			for( const auto &childnode : node.children )
			{
				wxPanel *panel = new wxPanel( notebook, -1 );
				notebook->AddSubPage( panel, factory->getConfletTitle( childnode.id ) );
				toc.push_back( ConfigdialogPage( childnode.id, panel ));
			}
		}
		assert( notebook->GetPageCount() == toc.size());
		contentsWindow = notebook;
	}
	else
	{
		const auto node = nodes.front();
		wxPanel *panel = new wxPanel( this, -1 );
		toc.push_back( ConfigdialogPage( node, panel ));
		contentsWindow = panel;
	}
	assert( contentsWindow );
	assert( toc.size());

	ok_button =new wxButton( this, wxID_OK, tr( "OK" ));
	apply_button = new wxButton( this, wxID_APPLY, tr( "Apply" ));
	restore_button = new wxButton( this, CD_RESTORE_DEFAULTS, tr( "Restore Defaults" ));
	cancel_button = new wxButton( this, wxID_CANCEL, tr( "Cancel" ));

	wxBoxSizer *buttonsizer = new wxBoxSizer( wxHORIZONTAL );
	buttonsizer->Add( ok_button, 0, wxALL, 3 );
	buttonsizer->Add( apply_button, 0, wxALL, 3 );
	buttonsizer->Add( restore_button, 0, wxALL, 3 );
	buttonsizer->Add( cancel_button, 0, wxALL, 3 );

	wxBoxSizer* sizer_main = new wxBoxSizer( wxVERTICAL );
	sizer_main->Add( contentsWindow, 1, wxALL|wxEXPAND, 3 );
	sizer_main->Add( new wxStaticLine( this, -1 ), 0, wxEXPAND, 0 );
	sizer_main->Add( buttonsizer, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxFIXED_MINSIZE , 3 );
	SetMinSize( wxSize( 400, 300 ));
	SetAutoLayout( true );
	SetSizer( sizer_main );
	Layout();
	SetIcon( ImageProvider::get()->getIcon( BitmapId::Config ));

	setSavedWindowSize( this, configkey, wxSize( 800, 600 ));
	setSavedWindowPosition( this, configkey );

	Connect( CONFIG_NOTEBOOK, wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING,
		wxBookCtrlEventHandler( ConfigurationDialog::OnTreebookChanging ));
	Connect( CONFIG_NOTEBOOK, wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED,
		wxBookCtrlEventHandler( ConfigurationDialog::OnTreebookChanged ));
	Connect( wxID_ANY, CONFLET_CHANGED, wxCommandEventHandler( ConfigurationDialog::OnConfletChanged ));
	Connect( wxID_ANY, CONFIG_TOOLBAR_CHANGED, wxCommandEventHandler( ConfigurationDialog::OnToolbarConfigChanged ));

	Connect( wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConfigurationDialog::OnOK ));
	Connect( wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConfigurationDialog::OnCancel ));
	Connect( wxID_APPLY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConfigurationDialog::OnApply ));
	Connect( CD_RESTORE_DEFAULTS, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler( ConfigurationDialog::OnRestoreDefaults ));

	if ( notebook )
	{
		const int selpage = gdiconfig->getActiveNotebookTab( configkey );
		notebook->SetSelection( selpage );
		if ( ! selpage ) showPanel( 0 ); // there is no change event for default panel id 0
	}
	else
	{
		assert( toc.size() == 1 );
		showPanel( 0 );
	}
}

/*****************************************************
**
**   ConfigurationDialog   ---   Destructor
**
******************************************************/
ConfigurationDialog::~ConfigurationDialog()
{
	saveWindowSize( this, configkey );
	saveWindowPosition( this, configkey );
	if ( notebook )
	{
		gdiconfig->prefs.activeNotebookTab[configkey] = notebook->GetSelection();
	}
}

/*****************************************************
**
**   ConfigurationDialog   ---   OnConfletChanged
**
******************************************************/
void ConfigurationDialog::OnConfletChanged( wxCommandEvent& )
{
	debuglog << "ConfigurationDialog::OnConfletChanged";
	apply_button->Enable( true );
}

/*****************************************************
**
**   ConfigurationDialog   ---   OnToolbarConfigChanged
**
******************************************************/
void ConfigurationDialog::OnToolbarConfigChanged( wxCommandEvent& /*event*/ )
{
	debuglog << "ConfigurationDialog::OnToolbarConfigChanged";
	// does not work
	//event.ResumePropagation( wxEVENT_PROPAGATE_MAX );
	//event.Skip();
	wxCommandEvent e( CONFIG_TOOLBAR_CHANGED, GetId() );
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   ConfigurationDialog   ---   OnOK
**
******************************************************/
void ConfigurationDialog::OnOK( wxCommandEvent& )
{
	debuglog << "ConfigurationDialog::OnOK";
	saveData();
	EndModal( wxID_OK );
	wxCommandEvent e( CONFIG_CHANGED, GetId() );
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   ConfigurationDialog   ---   OnRestoreDefaults
**
******************************************************/
void ConfigurationDialog::OnRestoreDefaults( wxCommandEvent& )
{
	const int sel = notebook ? notebook->GetSelection() : 0;
	debuglog << "ConfigurationDialog::OnRestoreDefaults sel " << sel;
	if ( sel != wxNOT_FOUND )
	{
		assert( sel >= 0 && sel < (int)toc.size() );
		const ConfigdialogPage page = toc[sel];

		assert( page.conflet );
		page.conflet->restoreDefaults();
		page.conflet->setDirty();
		page.conflet->TransferDataToWindow();
		page.conflet->updateUi();
		page.conflet->Validate();
		page.conflet->setDirty();

		apply_button->Enable( true );
	}
}

/*****************************************************
**
**   ConfigurationDialog   ---   OnTreebookChanging
**
******************************************************/
void ConfigurationDialog::OnTreebookChanging( wxBookCtrlEvent &event )
{
	const int oldsel = event.GetOldSelection();

	debuglog << "ConfigurationDialog::OnTreebookChanging old sel " << oldsel
		<< " new sel " << event.GetSelection();
	if ( oldsel == -1 ) return;

	assert( oldsel >= 0 && oldsel < (int)toc.size() );
	const ConfigdialogPage oldentry = toc[oldsel];

	if ( oldentry.conflet )
	{
		bool dirty = oldentry.conflet->isDirty();
		if ( dirty )
		{
			const int a = doMessageBox( this,
				tr( "Configuration has changed. Do you want to save?" ),
				wxYES_NO | wxCANCEL | wxCENTRE | wxICON_QUESTION );

			switch( a )
			{
				case wxID_YES:
					doApply();
				break;
				case wxID_NO:
					oldentry.conflet->setDirty( false );
					oldentry.conflet->InitDialog();
					oldentry.conflet->Validate();
				break;
				case wxID_CANCEL:
					event.Veto();
				break;
				default:
					assert( false );
				break;
			}
		}
	}
}

/*****************************************************
**
**   ConfigurationDialog   ---   OnTreebookChanged
**
******************************************************/
void ConfigurationDialog::OnTreebookChanged( wxBookCtrlEvent &event )
{
	debuglog << "ConfigurationDialog activePanelIndex " << activePanelIndex
		<< " event sel " << event.GetSelection()
		<< " toc size " << toc.size();
	if ( activePanelIndex != -1 )
	{
		assert( activePanelIndex >= 0 && activePanelIndex < (int)toc.size());
		assert( toc[activePanelIndex].conflet );
		toc[activePanelIndex].conflet->onPassivate();
	}

	assert( event.GetSelection() >= 0 && event.GetSelection() < (int)toc.size() );
	showPanel( event.GetSelection() );
}

/*****************************************************
**
**   ConfigurationDialog   ---   showPanel
**
******************************************************/
Conflet *ConfigurationDialog::showPanel( const int &id )
{
	ConfletFactory *factory = ConfletFactory::get();
	
	int pageid = id;
	if ( id < 0 || id >= (int)toc.size())
	{
		errorlog << "ConfigurationDialog::showPanel has invalid id " << id;
		pageid = 0;
	}

	ConfigdialogPage &page = toc[pageid];
	debuglog << "ConfigurationDialog::showPanel id " << id << " toc size " << toc.size()
		<< "  page.confletset " << (bool)(page.conflet != nullptr) ;

	SetTitle( tr( "Configuration" ) + ": " + factory->getConfletTitle( page.node.id ));

	if ( ! page.conflet )
	{
		assert( page.wxpanel );
		page.wxpanel->Show( false );
		page.conflet = factory->createConflet( page.wxpanel, page.node );
		page.conflet->InitDialog();
		page.conflet->Validate();

    wxBoxSizer* sizer= new wxBoxSizer( wxVERTICAL );
		sizer->Add( page.conflet, 1, wxEXPAND, 0 );

		page.wxpanel->SetSizer( sizer );
		page.wxpanel->Layout();
		page.wxpanel->Show( true );
	}
	assert( page.conflet );
	//page.conflet->TransferDataToWindow();
	page.conflet->onActivate();
	page.conflet->updateUi();

	activePanelIndex = id;
	apply_button->Enable( page.conflet->isDirty());
	restore_button->Enable( page.node.id >= ConfletId::General && page.node.id <= ConfletId::Last );
	return page.conflet;
}

/*****************************************************
**
**   ConfigurationDialog   ---   doApply
**
******************************************************/
void ConfigurationDialog::doApply()
{
	saveData();
	for ( const auto &page : toc )
	{
		if ( page.conflet )
		{
			page.conflet->InitDialog();
			page.conflet->Validate();
		}
	}
	wxCommandEvent e( CONFIG_CHANGED, GetId() );
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   ConfigurationDialog   ---   saveData
**
******************************************************/
void ConfigurationDialog::saveData()
{
	debuglog << "ConfigurationDialog::savaData";
	for ( const auto &page : toc )
	{
		//if ( page.conflet && page.conflet->isDirty() )
		if ( page.conflet )
		{
			debuglog << "ConfigurationDialog::saveData save page " << (int)page.node.id;
			page.conflet->TransferDataFromWindow();
			page.conflet->model2config();
			page.conflet->setDirty( false );
		}
	}
	apply_button->Enable( false );
}

