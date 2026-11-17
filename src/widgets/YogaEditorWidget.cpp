/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/YogaEditorWidget.cpp
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

#include "YogaEditorWidget.h"

#include "Commands.h"
#include "ConfigLoaderImpls.h"
#include "enumserializer.h"
#include "FileConfig.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "Statusbar.h"
#include "ToolPanel.h"
#include "WidgetFactory.h"
#include "WindowDirectory.h"
#include "YdlInterpreter.h"
#include "Yoga.h"
#include "YogaEditorPanel.h"

#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textdlg.h>

extern Gdiconfig *gdiconfig;

#define STATUS_MESSAGE_TIMEOUT 3000

#define USE_YOGAEDITOR_MENU
/*****************************************************
**
**   YogaEditorWidget   ---   Constructor
**
******************************************************/
YogaEditorWidget::YogaEditorWidget( wxWindow *parent, ContextProps_ptr props )
	: BasicWidget( parent, props, WidgetCategory::YogaEditorWidget, ToolbarWidgetId::YogaEditorWidget )
{
	expert = YogaExpert::get();
	panel = new YogaEditorPanel( this, yogas );
	mainsizer->Add( panel, 1, wxEXPAND, 3 );
	SetSizer( mainsizer );

	if ( ! gdiconfig->prefs.lastYogaConfigFile.empty() ) loadFile( gdiconfig->prefs.lastYogaConfigFile );

	// Toolbutton commands
	Connect( (int)CommandId::AppNew, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorWidget::OnNewFile ));
	Connect( (int)CommandId::AppOpen, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorWidget::OnOpenFile ));
	Connect( (int)CommandId::Save, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorWidget::OnSaveFile ));
	Connect( (int)CommandId::SaveAs, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorWidget::OnSaveFileAs ));

	// Menus (not available on Mac OS X)
#ifdef USE_YOGAEDITOR_MENU
	Connect( (int)CommandId::AppNew, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( YogaEditorWidget::OnNewFile ));
	Connect( (int)CommandId::AppOpen, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( YogaEditorWidget::OnOpenFile ));
	Connect( (int)CommandId::Save, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( YogaEditorWidget::OnSaveFile ));
	Connect( (int)CommandId::SaveAs, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( YogaEditorWidget::OnSaveFileAs ));
	Connect( (int)CommandId::Close, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( YogaEditorWidget::OnCloseCommand ));
#endif

	// new sources and groups
	Connect( YE_NEW_SOURCE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorWidget::OnNewSource ));
	Connect( YE_NEW_GROUP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorWidget::OnNewGroup ));

	//Connect( wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( YogaEditorWidget::OnChoice ));

	Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( YogaEditorWidget::OnClose ));
	Connect( wxEVT_IDLE, wxIdleEventHandler( YogaEditorWidget::OnIdle ));

	// rule buttons
	Connect( YE_PARSE_RULE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorWidget::OnParseRule ));
	Connect( YE_CLEAR_CHART, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorWidget::OnClearChart ));
	Connect( YE_EVALUATE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorWidget::OnEvaluateRule ));
	Connect( YE_OBJECTS, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorWidget::OnSelectObjects ));
}

/*****************************************************
**
**   YogaEditorWidget   ---   Destructor
**
******************************************************/
YogaEditorWidget::~YogaEditorWidget()
{
	gdiconfig->prefs.lastYogaConfigFile = filename;
}

/*****************************************************
**
**   YogaEditorWidget   ---   isDirty
**
******************************************************/
bool YogaEditorWidget::isDirty()
{
	assert( panel );
	return panel->isDirty();
}

/*****************************************************
**
**   YogaEditorWidget   ---   queryClose
**
******************************************************/
bool YogaEditorWidget::queryClose()
{
	debuglog << "YogaEditorWidget::queryClose";
	if ( panel->isDirty() )
	{
		wxString s = tr( "Do you want to save the Yoga definition file" ) + " " + filename + "?";
		const int ret = doMessageBox( this, s, wxYES_NO  | wxCANCEL | wxCENTRE | wxICON_QUESTION ); 
		switch ( ret )
		{
		case wxID_YES:
			return saveFile();
			break;
		case wxID_CANCEL:
			return false;
			break;
		default:
			return true;
			break;
		}
	}
	return true;
}

/*****************************************************
**
**   YogaEditorWidget   ---   closeFile
**
******************************************************/
bool YogaEditorWidget::closeFile()
{
	if ( ! queryClose()) return false;

	yogas.clear();
	panel->clearGui();
	filename.Clear();
	panel->setDirty( false );
	echo( tr( "File closed" ));
	updateUiItems();
	return true;
}

/*****************************************************
**
**   YogaEditorWidget   ---   loadFile
**
******************************************************/
void YogaEditorWidget::loadFile( wxString fn )
{
	panel->setDirty( false );
	yogas.clear();
	panel->clearGui();

	try
	{
		YogaConfigLoader::get()->loadConfigs( fn, yogas );
		filename = fn;
		echo( std::format( "File {} loaded, {} Yogas found.", fn, yogas.size()));
		panel->loadYogas();
	}
	catch( const std::exception& )
	{
		// TODO reason
		filename.Clear();
		echo( std::format( "Error: cannot load file {}", fn ));
	}
	updateUiItems();
}

/*****************************************************
**
**   YogaEditorWidget   ---   OnNewFile
**
******************************************************/
void YogaEditorWidget::OnNewFile( wxCommandEvent& )
{
	closeFile();
	panel->newItem();
	panel->setDirty( false );
	echo( tr( "New File created" ));
}

/*****************************************************
**
**   YogaEditorWidget   ---   OnOpenFile
**
******************************************************/
void YogaEditorWidget::OnOpenFile( wxCommandEvent& )
{
	//wxString filetypes = "JSON (*.json)|*.json| Xml (*.xml)|*.xml| All files (*.*)|*.*";
	const wxString filetypes = "JSON (*.json)|*.json| All files (*.*)|*.*";

	if ( ! queryClose() ) return;
	wxString path = gdiconfig->prefs.yogaEditorOpenPath;
	if ( path.IsEmpty()) path = FileConfig::get()->getResourceDir( ConfigResource::Yoga );

	wxFileDialog openFileDialog( this, tr( "Open Yoga Definition File"), path,
		wxEmptyString, filetypes, wxFD_OPEN, wxDefaultPosition );

	if ( openFileDialog.ShowModal() == wxID_OK )
	{
		loadFile( openFileDialog.GetPath() );
		gdiconfig->prefs.yogaEditorOpenPath = openFileDialog.GetDirectory();
	}
	updateUiItems();
}

/*****************************************************
**
**   YogaEditorWidget   ---   saveFile
**
******************************************************/
bool YogaEditorWidget::saveFile()
{
	if ( filename.IsEmpty() && ! saveFileAs()) return false;

	FileConfig::get()->backupFile( filename );

	try
	{
		YogaConfigLoader::get()->saveConfigs( filename, yogas );
		panel->setDirty( false );
		echo( "File " + filename + " sucessfully saved" );
	}
	catch( std::exception &e )
	{
		errorlog << "Error while saving yogas in " << filename << ". Reason was: " << e.what();
		echo( "Cannot save file " + filename );
		return false;
	}

	updateUiItems();
	return true;
}

/*****************************************************
**
**   YogaEditorWidget   ---   saveFileAs
**
******************************************************/
bool YogaEditorWidget::saveFileAs()
{
	const wxString filetypes = "JSON (*.json)|*.json| All files (*.*)|*.*";

	wxString path = gdiconfig->prefs.yogaEditorSavePath;
	if ( path.IsEmpty()) path = FileConfig::get()->getResourceDir( ConfigResource::Yoga );

	wxFileName fn( filename );

	wxFileDialog saveFileDialog( this, tr("Save file as" ), path, fn.GetFullName(), filetypes,
		wxFD_SAVE, wxDefaultPosition );

	if ( saveFileDialog.ShowModal() == wxID_OK )
	{
		filename = saveFileDialog.GetPath();
		gdiconfig->prefs.yogaEditorSavePath = saveFileDialog.GetDirectory();
		return saveFile();
	}

	return false;
}

/*****************************************************
**
**   YogaEditorWidget   ---   OnClose
**
******************************************************/
void YogaEditorWidget::OnClose( wxCloseEvent &event )
{
	debuglog << "YogaEditorWidget::OnClose";
	if ( ! queryClose()) 
	{
		event.Veto();
	}
	else
	{
		//WindowDirectory::get()->releaseView( this );
		Destroy();
	}
}
 
/*****************************************************
**
**   YogaEditorWidget   ---   OnCloseCommand
**
******************************************************/
void YogaEditorWidget::OnCloseCommand( wxCommandEvent& )
{
	debuglog << "YogaEditorWidget::OnCloseCommand";
	Close();
}

/*****************************************************
**
**   YogaEditorWidget   ---   parseRule
**
******************************************************/
void YogaEditorWidget::parseRule()
{
	Rule *rule = nullptr;
	extern ExpressionErrorStatus parserErrorStatus;

	assert( panel );
	wxString rulestr =  panel->getRuleText();

	if ( rulestr.IsEmpty())
	{
		panel->echoOnDragChart( tr( "No rule specified" ));
		return;
	}

	rule = expert->parseRule( rulestr );
	panel->echoOnDragChart( enum_to_string( parserErrorStatus ));
	if ( rule ) delete rule;
}

/*****************************************************
**
**   YogaEditorWidget   ---   OnEvaluateRule
**
******************************************************/
void YogaEditorWidget::OnEvaluateRule( wxCommandEvent& )
{
	assert( panel );
	const std::map<PlanetId, Sign> fake_data = panel->getPlanetData();
	extern ExpressionErrorStatus parserErrorStatus;
	
	for( const auto &a : fake_data )
	{
		debuglog << "Planet data " << a.first << " sign " << a.second;
	}
	//YdlInterpreter interpreter( fake_data );

	Rule *rule = expert->parseRule( panel->getRuleText() );
	if ( rule )
	{
		const double r = YdlInterpreter( fake_data ).evaluateRule( rule );
		debuglog << "YogaEditorWidget::OnEvaluateRule value " << r;

		const ExpressionErrorStatus status = rule->getNestedErrorStatus();
		if ( status == ExpressionErrorStatus::Ok )
		{
			wxString s = expert->resultToString( r, rule->getOutputType());
			echo( s );
			panel->echoOnDragChart( s );
			debuglog << "YogaEditorWidget::OnEvaluateRule result " << s;
		}
		else
		{
			panel->echoOnDragChart( enum_to_string( status ));
			debuglog << "YogaEditorWidget::OnEvaluateRule errorstatus " << status;
		}

		delete rule;
	}
	else
	{
		panel->echoOnDragChart( enum_to_string( parserErrorStatus ));
	}
}

/*****************************************************
**
**   YogaEditorWidget   ---   OnChoice
**
******************************************************/
void YogaEditorWidget::OnChoice( wxCommandEvent& )
{
	debuglog << "YogaEditorWidget::OnChoice";
}

/*****************************************************
**
**   YogaEditorWidget   ---   OnSelectObjects
**
******************************************************/
void YogaEditorWidget::OnSelectObjects( wxCommandEvent& )
{
	doMessageBox( this, "not implemented" );
}

/*****************************************************
**
**   YogaEditorWidget   ---   OnClearChart
**
******************************************************/
void YogaEditorWidget::OnClearChart( wxCommandEvent& )
{
	panel->clearChart();
}

/*****************************************************
**
**   YogaEditorWidget   ---   OnNewSource
**
******************************************************/
void YogaEditorWidget::OnNewSource( wxCommandEvent& )
{
	wxTextEntryDialog dialog( this, tr( "New Source" ), tr( "Yoga Editor"), tr( "New Source" ));
	if ( dialog.ShowModal()  == wxID_OK )
	{
		panel->addSource( dialog.GetValue() );
	}
}

/*****************************************************
**
**   YogaEditorWidget   ---   OnNewGroup
**
******************************************************/
void YogaEditorWidget::OnNewGroup( wxCommandEvent& )
{
	wxTextEntryDialog dialog( this, tr( "New Group" ), tr( "Yoga Editor"), tr( "New Group" ));
	if ( dialog.ShowModal()  == wxID_OK )
	{
		panel->addGroup( dialog.GetValue() );
	}
}

/*****************************************************
**
**   YogaEditorWidget   ---   OnIdle
**
******************************************************/
void YogaEditorWidget::OnIdle( wxIdleEvent& )
{
	//updateUiItems();
	//updateTitle();
}

/*****************************************************
**
**   YogaEditorWidget   ---   updateUiItems
**
******************************************************/
void YogaEditorWidget::updateUiItems()
{
	// status bar messages
	if ( filename.IsEmpty())
	{
		echo( tr( "No file opened" ), 1 );
	}
	else
	{
		echo( filename, 1 );
		if ( panel->isDirty() ) echo( tr( "changed" ), 2 );
		else echo( tr( "unchanged" ), 2 );
	}
	wxCommandEvent e( WIDGET_UPDATE_TITLE_QUERY, GetId());
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   YogaEditorWidget   ---   getTitle
**
******************************************************/
wxString YogaEditorWidget::getTitle( const bool /*shortname*/ )
{
	debuglog << "YogaEditorWidget::getTitle";
	wxString s = tr( "Yoga Editor" );
	if ( ! filename.IsEmpty())
	{
		s += ": " + wxFileName( filename ).GetFullName();
	}
	return s;
}

/**************************************************************
***
**   YogaEditorWidget   ---   echo
***
***************************************************************/
#if 0
void YogaEditorWidget::echo( const wxString message, const int field )
{
	debuglog << "YogaEditorWidget::echo field " << field << " message " << message;
	wxFrame *frame = dynamic_cast<wxFrame*>( GetParent());
	if ( ! frame )
	{
		errorlog << "YogaEditorWidget has no wxFrame parent";
		return;
	}
	wxStatusBar *statbar = frame->GetStatusBar();
	if ( ! statbar )
	{
		errorlog << "cannot access statusbar of YogaEditorWidget";
		return;
	}
	statbar->SetStatusText( message, field );
}
#else
void YogaEditorWidget::echo( const wxString, const int ) {}
#endif

/*****************************************************
**
**   WindowFactory   ---   createYogaEditorWidget
**
******************************************************/
BasicWidget *WidgetFactory::createYogaEditorWidget( wxWindow *parent )
{
	return new YogaEditorWidget( parent, props );
}

