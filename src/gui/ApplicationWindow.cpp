/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/ApplicationWindow.cpp
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

#include "ApplicationWindow.h"

#include "AboutDialog.h"
#include "astroconfig.h"
#include "AtlasDialog.h"
#include "ChildWindow.h"
#include "constants.h"
#include "ContextProperties.h"
#include "Commands.h"
#include "ConfigurationDialog.h"
#include "enumserializer.h"
#include "FileConfig.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "Horoscope.h"
#include "ImageProvider.h"
#include "MenuProvider.h"
#include "MultipleView.h"
#include "SolarHoroscope.h"
#include "Statusbar.h"
#include "ToolPanel.h"
#include "WindowFactory.h"
#include "WindowDirectory.h"
#include "WindowListCtrl.h"
#include "YearlyHoroscopeDialog.h"

#include <wx/choicdlg.h>
#include <wx/cmdline.h>
#include <wx/dnd.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/listctrl.h>
#include <wx/log.h>
#include <wx/notifmsg.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/sysopt.h>
#include <wx/textctrl.h>
#include <wx/timer.h>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

enum { APPWINDOW_NOTIFICATION_TIMER = wxID_HIGHEST + 2500, APPWINDOW_INFOBAR_LOGWINDOW_BUTTON };

/*************************************************//**
*
* Handles file drop events
*
******************************************************/
class AppWindowDropTarget : public wxFileDropTarget
{
public:
    AppWindowDropTarget( ApplicationWindow *appwnd ) { appwindow = appwnd; }

    virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames )
		{
			debuglog << "AppWindowDropTarget::OnDropFiles " << x << "/" << y;
			for( const auto &fn : filenames )
			{
				debuglog << "drop filename " << fn;
				appwindow->OpenFile( fn );
			}
			return true;
		}

private:
    ApplicationWindow *appwindow;
};

/*****************************************************
**
**   ApplicationWindow   ---   Constructor
**
******************************************************/
ApplicationWindow::ApplicationWindow( const wxString &title, const wxPoint &pos, const wxSize &size )
	: wxFrame( nullptr, -1, title, pos, size )
{
	wxString rfiles, s;

	LogAdapter *logadapter = LogAdapter::get();
	logadapter->enableWxLogAppender( true );
	logadapter->setWxLogLevel( std::max( gdiconfig->view.uiLogLevel, gdiconfig->view.uiNotificationLogLevel ));
	showLogWindow( gdiconfig->prefs.showLogWindow );

	listctrl = new WindowListCtrl( this );

	SetIcon( ImageProvider::get()->getIcon( BitmapId::Application  ));
	droptarget = new AppWindowDropTarget( this );
	SetDropTarget( droptarget );

	menubar = new AppMenuBar( wxMB_DOCKABLE );
	SetMenuBar( menubar );
	SetAcceleratorTable( CommandHandler::get()->getAcceleratorTable() );

	if ( gdiconfig->prefs.showMainToolbar ) recreateToolbar();

	showStatusBar();

	docommands = true;
	WindowDirectory::get()->subscribe( this );


	// Child window command are delegated to the avtive child
	Connect( (int)CommandId::ChildFirst, (int)CommandId::ChildLast, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler( ApplicationWindow::OnChildCommand ));

	Connect( (int)CommandId::AppNew, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnFileNew ));
	Connect( (int)CommandId::AppNewSingle, (int)CommandId::AppNewMultipleLast, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler( ApplicationWindow::OnFileNew ));
	Connect( (int)CommandId::AppOpen, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnFileOpen ));
	Connect( (int)CommandId::AppFileRecent, (int)CommandId::AppFileRecentLast, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler( ApplicationWindow::OnOpenRecent ));

	// required by macos
	Connect( wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnFileQuit ));
	Connect( (int)CommandId::AppExit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnFileQuit ));

	Connect( (int)CommandId::AppToggleStatusbar, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnToggleStatusBar ));
	Connect( (int)CommandId::AppToggleMainToolbar, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnToggleToolBars ));
	Connect( (int)CommandId::AppToggleLogWindow, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnToggleLogWindow ));

	Connect( (int)CommandId::AppSaveConfig, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnSaveConfig ));
	Connect( (int)CommandId::AppConfigure, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnConfigurationDialog ));

	// only AppConfigGeneral is used
	Connect( (int)CommandId::AppConfigGeneral, (int)CommandId::AppConfigPrintout, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler( ApplicationWindow::OnConfigurationDialog ));

	Connect( (int)CommandId::AppAtlasdialog, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler( ApplicationWindow::OnLocationDialog ));
	Connect( wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnAppAbout ));
	Connect( (int)CommandId::AppAbout, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnAppAbout ));
	Connect( (int)CommandId::AppHelp, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnAppHelp ));

	Connect( (int)CommandId::AppListLargeIcons, (int)CommandId::AppListTable, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler( ApplicationWindow::OnToggleIcons ));
	Connect( (int)CommandId::AppHora, (int)CommandId::AppLicense, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler( ApplicationWindow::OnNewWindow ));
	Connect( (int)CommandId::AppYearlyHoroscope, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ApplicationWindow::OnYearlyHoroscope ));

	Connect( wxID_ANY, CONFIG_CHANGED, wxCommandEventHandler( ApplicationWindow::OnConfigChanged ));
	Connect( wxID_ANY, CONFIG_TOOLBAR_CHANGED, wxCommandEventHandler( ApplicationWindow::OnToolbarConfigChanged ));
	Connect( wxID_ANY, APPWINDOW_CREATEVIEW, wxCommandEventHandler( ApplicationWindow::OnCreateWindowFromExternal ));

	Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ApplicationWindow::OnClose ));
	Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( ApplicationWindow::OnDropFilesEvent ));
	Connect( wxEVT_IDLE, wxIdleEventHandler( ApplicationWindow::OnIdle ));

	Connect( wxID_ANY, WD_WINDOW_ACTIVATED, wxCommandEventHandler( ApplicationWindow::OnWindowActivated ));
	Connect( wxID_ANY, WD_HOROSCOPE_SAVED, wxCommandEventHandler( ApplicationWindow::OnHoroscopeSaved ));

	// methods for lazy filling of the "new" menu and the "new" dropdown in the toolbar (wxgtk and windows only)
	Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( ApplicationWindow::OnAppMenuOpen ));
	Connect( wxEVT_TOOL_DROPDOWN, wxMenuEventHandler( ApplicationWindow::onDropdown ));
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   Destructor
**
******************************************************/
ApplicationWindow::~ApplicationWindow()
{
	WindowDirectory::get()->unsubscribe( this );
}

/*****************************************************
**
**   ApplicationWindow   ---   OnOpenRecent
**
******************************************************/
void ApplicationWindow::OnOpenRecent( wxCommandEvent &event )
{
	debuglog << "ApplicationWindow::OnOpenRecent eventid " << event.GetId();
	OpenFile( menubar->getRecentFilesEntry( event.GetId()) );
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnIdle
**
******************************************************/
void ApplicationWindow::OnIdle( wxIdleEvent& )
{
	if ( ! updateTools ) return;

	docommands = false;
	ChildWindow *window = WindowDirectory::get()->getActiveWindow();

	const bool isHoroscope = ( window && window->getProps()->hasRadixHoroscope() );
	wxToolBarBase *toolbar = GetToolBar();
	if ( toolbar )
	{
		for ( int i = (int)CommandId::ChildFirst + 1; i < (int)CommandId::ChildLast; i++ ) toolbar->EnableTool( i, isHoroscope );
	}

	docommands = true;
	updateTools = false;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnFileNew
**
******************************************************/
void ApplicationWindow::OnFileNew( wxCommandEvent &event )
{
	bool useMultipleView = gdiconfig->multipleView.useMultipleViews;
	int viewId = gdiconfig->multipleView.defaultView;
	debuglog << "File NEW event id " << event.GetId() << " view id " << viewId
		<< " useMultipleView " << std::boolalpha << useMultipleView << " /" << CommandId::AppNewMultiple;

	switch ( (CommandId)event.GetId() )
	{
		case CommandId::AppNew: // use the standard params set above
		break;
		case CommandId::AppNewSingle:
			useMultipleView = false;
		break;
		case CommandId::AppNewMultipleChoice: // user input for multiple view selection
		{
			wxArrayString choices;
			for( const auto &def : MultipleViewHandler::get()->getBuiltinDefs()) { choices.Add( def.name ); }
			wxSingleChoiceDialog dialog( this, _( "Select View" ), GUI_APP_NAME, choices );
			dialog.SetSize( 300, 600 );
			if ( dialog.ShowModal() != wxID_OK ) return;
			viewId = dialog.GetSelection();
			useMultipleView = true;
		}
		break;
		case CommandId::AppNewMultiple: // default multiple view
			debuglog << "ApplicationWindow::OnFileNew NEW Multiple " << viewId;
			useMultipleView = true;
			if( viewId < 0 || viewId >= (int)MultipleViewHandler::get()->getBuiltinDefs().size())
			{
				errorlog << "ApplicationWindow::OnFileNew wrong multipleView.defaultView " << viewId << ", replacing by zero";
				viewId = 0;
			}
		break;
		default: // event id contains implicitely the mview id
			debuglog << "ApplicationWindow::OnFileNew NEW Default";
			useMultipleView = true;
			viewId = event.GetId() - (int)CommandId::AppNewMultiple;
			assert( viewId >= 0 && viewId < (int)MultipleViewHandler::get()->getBuiltinDefs().size());
		break;
	}
	NewFile( useMultipleView, viewId );
}

/*****************************************************
**
**   ApplicationWindow   ---   NewFile
**
******************************************************/
void ApplicationWindow::NewFile( const bool &useMultipleView, const int mViewId )
{
	debuglog << "ApplicationWindow::NewFile useMultipleView " << useMultipleView << " mViewId " << mViewId;

	Horoscope_ptr horoscope( new Horoscope());
	horoscope->update();

	doCreateNew( horoscope, useMultipleView, mViewId );
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   CreateNewWindow
**
******************************************************/
void ApplicationWindow::CreateNewWindow( Horoscope_ptr horoscope )
{
	horoscope->update();
	doCreateNew( horoscope, gdiconfig->multipleView.useMultipleViews, gdiconfig->multipleView.defaultView );
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   doCreateNew
**
******************************************************/
void ApplicationWindow::doCreateNew( Horoscope_ptr horoscope, const bool &useMultipleView, const int mViewId  )
{
	ChildWindow *window = nullptr;

	WindowFactory factory;
	if ( useMultipleView )
	{
		assert( mViewId >= 0 && mViewId < (int)MultipleViewHandler::get()->getBuiltinDefs().size());
		window = factory.createMultipleMainWindow( this, horoscope, mViewId );
	}
	else
	{
		//window = factory.createSingleMainWindow( this, horoscope );
		window = factory.createWindow( this, horoscope, CommandId::AppNewSingle );
	}
	assert( window );
	addWindow( window );
}

/*****************************************************
**
**   ApplicationWindow   ---   OnFileOpen
**
******************************************************/
void ApplicationWindow::OnFileOpen( wxCommandEvent& )
{
	wxString filetypes = "Maitreya (*.mtx)|*.mtx| Maitreya 2.x (*.atx)|*.atx| All files (*.*)|*.*";
	wxFileDialog openFileDialog( this, tr( "Open file"), gdiconfig->prefs.defOpenPath, "",
		filetypes, wxFD_OPEN, wxDefaultPosition );

	openFileDialog.SetFilterIndex( gdiconfig->view.defOpenFiletype );
	if (openFileDialog.ShowModal() == wxID_OK )
	{
		// Bugfix 7.0.3 GetDirectory returns home directory instead of selected dir
		//if ( OpenFile( openFileDialog.GetDirectory() + wxFileName::GetPathSeparator()  + openFileDialog.GetFilename() ))
		if ( OpenFile( openFileDialog.GetPath() ))
		{
			gdiconfig->prefs.defOpenPath = openFileDialog.GetDirectory();
			gdiconfig->view.defOpenFiletype = openFileDialog.GetFilterIndex();
		}
	}
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OpenFile
**
******************************************************/
bool ApplicationWindow::OpenFile( wxString filename )
{
	debuglog << "ApplicationWindow::OpenFile " << filename;

	Horoscope_ptr horoscope( new Horoscope());
	try
	{
		horoscope->openFile( filename );
	}
	catch( std::exception &e )
	{
		const wxString s = tr( "Cannot open file" ) + " " + filename + ", " + e.what();
		doMessageBox( this, s, wxOK | wxICON_ERROR );

		return false;
	}
	menubar->addToRecentFiles( filename );
	CreateNewWindow( horoscope );
	updateTools = true;
	return true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnFileQuit
**
******************************************************/
void ApplicationWindow::OnFileQuit( wxCommandEvent& )
{
	debuglog << "ApplicationWindow::OnFileQuit";
	//Close( true );
	Close( false );
}

/*****************************************************
**
**   ApplicationWindow   ---   addWindow
**
******************************************************/
void ApplicationWindow::addWindow( ChildWindow *window )
{
	if ( ! window )
	{
		errorlog << "Error: child is null!";
		return;
	}
	debuglog << "ApplicationWindow::addWindow title " << window->getTitle();

	WindowDirectory::get()->addWindow( window );

	window->Show( true );
	window->postCreate();
	window->updateTitle();
	updateTools = true;
}


/*****************************************************
**
**   ApplicationWindow   ---   OnCreateWindowFromExternal
**
******************************************************/
void ApplicationWindow::OnCreateWindowFromExternal( wxCommandEvent &event )
{
	debuglog << "ApplicationWindow::OnCreateWindowFromExternal id " << event.GetId();
	wxObject *ob = event.GetEventObject();
	assert( ob );
	HoroscopeWrapper *wrapper = dynamic_cast<HoroscopeWrapper*>( ob );
	assert( wrapper );
	Horoscope_ptr h = wrapper->horoscope;
	assert( h );
	debuglog << "ApplicationWindow::OnCreateWindowFromExternal name " << h->dl.name;
	CreateNewWindow( h );
}

/*****************************************************
**
**   ApplicationWindow   ---   OnYearlyHoroscope
**
******************************************************/
void ApplicationWindow::OnYearlyHoroscope( wxCommandEvent& )
{
	YearlyHoroscopeDialog( this ).ShowModal();
}

/*****************************************************
**
**   ApplicationWindow   ---   OnNewWindow
**
******************************************************/
void ApplicationWindow::OnNewWindow( wxCommandEvent &event )
{
	createNewWindow( (CommandId)event.GetId() );
}

/*****************************************************
**
**   ApplicationWindow   ---   createNewWindow
**
******************************************************/
void ApplicationWindow::createNewWindow( const CommandId &id )
{
	debuglog << "ApplicationWindow::createNewWindow type " << id;
	ChildWindow *view = nullptr;

	view = WindowFactory().createWindow( this, nullptr, id );
	if ( view ) addWindow( view );
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnChildCommand
**
******************************************************/
void ApplicationWindow::OnChildCommand( wxCommandEvent &event )
{
	if ( ! docommands ) return;
	updateTools = true;

	ChildWindow *window = WindowDirectory::get()->getActiveWindow();
	if ( ! window )
	{
		warnlog << "ApplicationWindow::OnChildCommand No active window";
		return;
	}
	ContextProps_ptr props = window->getProps();

	const CommandId id = (CommandId)event.GetId();
	debuglog << "ApplicationWindow::OnChildCommand command id " << id;

	if ( id >= CommandId::NewText && id <= CommandId::ChartGeom )
	{
		// avoid that document dependent view is opened based upon non doc view, ugly but works
		if ( ! props->hasRadixHoroscope() && (int)id >= (int)CommandId::NewText ) return;

		addWindow( WindowFactory().createWindow( this, window->getProps()->getRadixHoroscope(), id ));
	}
	else
	{
		window->dispatchApplicationWindowCommand( id );
	}
}

/*****************************************************
**
**   ApplicationWindow   ---   prepareSaveConfig
**
******************************************************/
void ApplicationWindow::prepareSaveConfig()
{
	debuglog << "ApplicationWindow::prepareSaveConfig";

	const wxString configkey = "ApplicationWindow";
	saveWindowSize( this, configkey );
  saveWindowPosition( this, configkey );

	menubar->saveRecentFiles();
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnClose
**
******************************************************/
void ApplicationWindow::OnClose( wxCloseEvent& )
{
	debuglog << "Start ApplicationWindow::OnClose";

	updateTools = true;
	if ( astroconfig->general.askOnQuit )
	{
		if ( doMessageBox( this, _( "Do you want to quit?" ), wxYES_NO | wxCENTRE | wxICON_QUESTION ) == wxID_NO ) return;
	}
	debuglog << "ApplicationWindow::OnClose -- Closing";

	// achtung: die liste ist evtl nicht mehr aktuell weil Fenster geschlossen werden
	for( auto *window : WindowDirectory::get()->getWindows())
	{
		debuglog << "ApplicationWindow::OnClose window " << window->getViewId() << " is main Window "
			<< std::boolalpha << window->isMainWindow();
		assert( window );
		if ( window->isMainWindow() || window->getViewId() == ViewId::YogaEditor )
		{
#if 0
			Horoscope_ptr h = window->getHoroscope();
			if ( h )
			{
				debuglog << "ApplicationWindow::OnClose window " << window->getTitle() << " -- Horoscope " << h
					<< " use count " << h.use_count();
			}
#endif
			if ( ! window->queryClose()) { return; }
		}
	}
	infolog << "Bye ...";
	prepareSaveConfig();
	Destroy();
}

/*****************************************************
**
**   ApplicationWindow   ---   OnSaveConfig
**
******************************************************/
void ApplicationWindow::OnSaveConfig( wxCommandEvent& )
{
	prepareSaveConfig();
	wxString dirname = FileConfig::get()->getConfigDir();

	astroconfig->save();
	gdiconfig->save();

	wxString message = tr( "Configurations saved to directory" ) + " " +  dirname;
	doMessageBox( this, message );
	infolog << message;

	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnToggleIcons
**
******************************************************/
void ApplicationWindow::OnToggleIcons( wxCommandEvent &event )
{
	gdiconfig->prefs.listStyle = event.GetId() - (int)CommandId::AppListLargeIcons;
	listctrl->recreate();

	// required on macOS
	Layout();
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnConfigurationDialog
**
******************************************************/
void ApplicationWindow::OnConfigurationDialog( wxCommandEvent &event )
{
	debuglog << "ApplicationWindow::OnConfigurationDialog " << event.GetId();
	ConfletSet cs;
	switch( (CommandId)event.GetId())
	{
		case CommandId::AppConfigure:
		case CommandId::AppConfigGeneral:
			cs = ConfletSet::General;
		break;
		case CommandId::AppConfigCalculation:
			cs = ConfletSet::Calculation;
		break;
		case CommandId::AppConfigGui:
			cs = ConfletSet::Gui;
		break;
		case CommandId::AppConfigPrintout:
			cs = ConfletSet::Printout;
		break;
		default:
			assert( false );
			//cs = ConfletSet::General;
		break;
	}
	const int ret = ConfigurationDialog( this, cs ).ShowModal();
	debuglog << "ApplicationWindow::OnConfigurationDialog return " << ret;
	//CalculationConfigDialog( this ).ShowModal();
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnLocationDialog
**
******************************************************/
void ApplicationWindow::OnLocationDialog( wxCommandEvent& )
{
	AtlasDialogWrapper wrapper( this, ATLASDIALOG_CALLER_APPLICATIONWINDOW );
	wrapper.run();
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnAppHelp
**
******************************************************/
void ApplicationWindow::OnAppHelp( wxCommandEvent& )
{
	wxString url = std::format( "{}documentation.html", SARAVALI_WEBSITE_URL );

	if ( ! wxLaunchDefaultBrowser( url )) doMessageBox( this, _( "Cannot launch Browser." ) );
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnAppAbout
**
******************************************************/
void ApplicationWindow::OnAppAbout( wxCommandEvent& )
{
	AboutDialog( this ).ShowModal();
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   showStatusBar
**
******************************************************/
void ApplicationWindow::showStatusBar()
{
	//statusbar->StartPlanetTimer( gdiconfig->prefs.showStatusBar && gdiconfig->view.showStatusInfo );
	if ( statusbar )
	{
		SetStatusBar( nullptr );
		delete statusbar;
	}
	statusbar = (MainWindowStatusbar*)CreateStatusBar();
	statusbar->Show( gdiconfig->prefs.showStatusBar );
	SendSizeEvent();
	updateTools = true;
}

/**************************************************************
***
**  ApplicationWindow   ---   OnCreateStatusBar
***
***************************************************************/
// this method is internally used by wxWidgets
wxStatusBar *ApplicationWindow::OnCreateStatusBar( int, long, wxWindowID, const wxString& )
{
	return new MainWindowStatusbar( this );
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnToggleStatusBar
**
******************************************************/
void ApplicationWindow::OnToggleStatusBar( wxCommandEvent &event )
{
	gdiconfig->prefs.showStatusBar = event.IsChecked();
	showStatusBar();
}

/*****************************************************
**
**   ApplicationWindow   ---   OnToggleToolBars
**
******************************************************/
void ApplicationWindow::OnToggleToolBars( wxCommandEvent &event )
{
	gdiconfig->prefs.showMainToolbar = event.IsChecked();
	if ( gdiconfig->prefs.showMainToolbar )
	{
		recreateToolbar();
	}
	else
	{
		wxToolBar *toolbar = GetToolBar();
		delete toolbar;
		SetToolBar( NULL );
	}
}

/*****************************************************
**
**   ApplicationWindow   ---   recreateToolbar
**
******************************************************/
void ApplicationWindow::recreateToolbar()
{
	ToolbarFactory *factory = ToolbarFactory::get();

	wxToolBar *toolbar = GetToolBar();
	delete toolbar;
	SetToolBar( nullptr );

	toolbar = CreateToolBar( factory->getToolbarStyle());
	factory->populateMainToolbar( toolbar );
	toolbar->Realize();
	//Layout();
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnToolbarConfigChanged
**
******************************************************/
void ApplicationWindow::OnToolbarConfigChanged( wxCommandEvent& )
{
	debuglog << "ApplicationWindow::OnToolbarConfigChanged";
	if ( gdiconfig->prefs.showMainToolbar )
	{
		recreateToolbar();
	}
}

/*****************************************************
**
**   ApplicationWindow   ---   OnDropFilesEvent
**
******************************************************/
void ApplicationWindow::OnDropFilesEvent( wxDropFilesEvent &event )
{
	debuglog << "ApplicationWindow::OnDropFilesEvent";
	for ( int i = 0; i < event.GetNumberOfFiles(); i++ )
	{
		debuglog << "ApplicationWindow::OnDropFilesEvent open " << (event.GetFiles())[i];
		OpenFile( (event.GetFiles())[i] );
	}
}

/*****************************************************
**
**   ApplicationWindow   ---   OnConfigChanged
**
******************************************************/
void ApplicationWindow::OnConfigChanged( wxCommandEvent& )
{
	debuglog << "ApplicationWindow::OnConfigChanged";
	WindowDirectory *wd = WindowDirectory::get();

	for( auto horoscope : wd->getHoroscopes())
	{
		horoscope->update();
	}
	for ( auto *window : wd->getWindows())
	{
		window->OnDataChanged();
	}

	listctrl->onConfigChanged();

	showStatusBar();
	// TODO braucht man das
	//recreateToolbar();

	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnWindowActivated
**
******************************************************/
void ApplicationWindow::OnWindowActivated( wxCommandEvent& )
{
	debuglog << "ApplicationWindow::OnWindowActivated";
	updateTools = true;
}

/*****************************************************
**
**   ApplicationWindow   ---   OnHoroscopeSaved
**
******************************************************/
void ApplicationWindow::OnHoroscopeSaved( wxCommandEvent &event )
{
	debuglog << "ApplicationWindow::OnHoroscopeSaved " << event.GetString();
	if ( ! event.GetString().IsEmpty()) menubar->addToRecentFiles( event.GetString() );
}

/*****************************************************
**
**   ApplicationWindow   ---   onDropdown
**
******************************************************/
void ApplicationWindow::onDropdown( wxMenuEvent &event )
{
	//debuglog << "ApplicationWindow::onDropdown";
	wxToolBarBase *toolbar = GetToolBar();
	if ( toolbar )
	{
		wxToolBarToolBase *tb = toolbar->FindById( (int)CommandId::AppNew );
		if ( tb )
		{
			wxMenu *menu = tb->GetDropdownMenu();
			if ( menu && ! menu->GetMenuItemCount() )
			{
				debuglog << "ApplicationWindow::onDropdown populate NEW dropdown";
				CommandHandler *ch = CommandHandler::get();
				MCommand *newcommand = ch->getCommand( CommandId::AppNewSingle );
				menu->Append( (int)CommandId::AppNewSingle, newcommand->text );
				menu->AppendSeparator();

				for( int i = 0; const auto &def : MultipleViewHandler::get()->getBuiltinDefs())
				{
					menu->Append( (int)CommandId::AppNewMultiple + i, def.name );
					++i;
				}
			}
		}
	}
	event.Skip();
}

/*****************************************************
**
**   ApplicationWindow   ---   OnAppMenuOpen
**
******************************************************/
void ApplicationWindow::OnAppMenuOpen( wxMenuEvent &event )
{
	//debuglog << "ApplicationWindow::OnAppMenuOpen";
	wxMenu *menu = event.GetMenu();
	if ( menu )
	{
		debuglog << "ApplicationWindow::OnAppMenuOpen id " << event.GetMenuId();
		menubar->OnOpenMenu( menu );
	}
}

/*****************************************************
**
**   ApplicationWindow   ---   showLogWindow
**
******************************************************/
void ApplicationWindow::showLogWindow( const bool &doShow )
{
	if ( ! logWindow )
	{
		class MyLogWindow : public wxLogWindow
		{
			public:
				MyLogWindow( wxWindow *parent ) : wxLogWindow( parent, tr( "Log Window" ), true, false ) {}
#if 1
				virtual void DoLogRecord( wxLogLevel level, const wxString &msg, const wxLogRecordInfo& )
				{
					if ( level <= LogAdapter::get()->convertToWxLogLevel( gdiconfig->view.uiLogLevel ))
					{
						DoLogTextAtLevel( level, msg );
					}
				}

#endif
				virtual bool OnFrameClose( wxFrame *frame ) override
				{
					const wxString logWindowKey = "LogWindow";

					gdiconfig->prefs.showLogWindow = false;
					saveWindowSize( frame, logWindowKey );
					saveWindowPosition( frame, logWindowKey );
					return wxLogWindow::OnFrameClose( frame );
				}
		};
		logWindow = new MyLogWindow( this );
		wxFrame *frame = logWindow->GetFrame();
		assert( frame );

		frame->SetSize( gdiconfig->prefs.getWidgetSize( ViewId::LogWindow ));
		if ( const auto &a = gdiconfig->prefs.widgetPosition.find( enum_to_string( ViewId::LogWindow ));
			a != gdiconfig->prefs.widgetPosition.end())
		{
			frame->SetPosition(  wxPoint( a->second.x, a->second.y ));
		}

		// TODO does not work
		//frame->SetAcceleratorTable( CommandHandler::get()->getAcceleratorTable() );
	}
	assert( logWindow );
	logWindow->Show( doShow );
}

/*****************************************************
**
**   ApplicationWindow   ---   OnToggleLogWindow
**
******************************************************/
void ApplicationWindow::OnToggleLogWindow( wxCommandEvent &event )
{
	const bool doShow = event.IsChecked();
	debuglog << "ApplicationWindow::OnToggleLogWindow " << std::boolalpha << doShow;

	gdiconfig->prefs.showLogWindow = doShow;
	showLogWindow( doShow );
}

