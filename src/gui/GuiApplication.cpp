/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/GuiApplication.cpp
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

#include "astroconfig.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "FileConfig.h"
#include "LanguageConfig.h"

#include <wx/app.h>
#include <wx/filename.h>

Astroconfig *astroconfig = nullptr;
Gdiconfig *gdiconfig = nullptr;

// some builds have debug asserts enabled even in release mode
//#define SHOW_APP_DEBUG_ASSERTS

#define DEBUG_GUIAPPLICATION

// try to retrieve logging info before astroconfig etc is initialized
//#define EARLY_LOGGING

/*************************************************//**
*
*  application class managing life cycle of ApplicationWindow
*
******************************************************/
class GuiApplication : public wxApp
{
public:

	/*****************************************************
	**
	**   GuiApplication   ---   Constructor
	**
	******************************************************/
	GuiApplication() : wxApp() {}

protected:


	/*****************************************************
	**
	**   GuiApplication   ---   OnInit
	**
	******************************************************/
	virtual bool OnInit() override
	{
		SetAppName( APP_NAME );
		SetVendorName( VENDOR_NAME );
		SetAppDisplayName( APP_NAME );

		try
		{
#ifdef EARLY_LOGGING
			LogAdapter::get()->enableStdoutLogAppender( true );
#endif

			FileConfig *fileconfig = FileConfig::get();
			fileconfig->initHomedir();

			astroconfig = new Astroconfig;
			astroconfig->load();
			gdiconfig = new Gdiconfig;
			gdiconfig->load();

#ifndef __WXMSW__
			LogAdapter *logadapter = LogAdapter::get();
			if ( gdiconfig->view.enableStdoutLogging )
			{
				logadapter->enableStdoutLogAppender( true );
				logadapter->setStdoutLogLevel( gdiconfig->view.stdoutLogLevel );
			}
#endif

#ifdef DEBUG_GUIAPPLICATION
			debuglog << "Bootstrap gui app: program name is " << argv[0];
#endif	

		  wxInitAllImageHandlers();
			fileconfig->initDatadir( argv[0] );

			LanguageConfig::get()->setLanguage();

			const wxString configkey = "ApplicationWindow";
			wxPoint point( 50, 50 );
			if ( const auto &a = gdiconfig->prefs.widgetPosition.find( configkey );
				a != gdiconfig->prefs.widgetPosition.end())
			{
				point = wxPoint( a->second.x, a->second.y );
			}
			wxSize size = gdiconfig->prefs.getWidgetSize( ViewId::ApplicationWindow );
			frame = new ApplicationWindow( GUI_APP_NAME, point, size );

			frame->Show( true );
			//SetTopWindow(frame);

			if ( argc > 1 )
			{
				for( int i = 1; i < argc; i++ )
				{
					wxString filename = argv[i];
#ifdef DEBUG_GUIAPPLICATION
					debuglog << "Found Param: " << filename;
#endif	
					if ( ! wxFileName::Exists( filename ))
					{
						errorlog << "File " << filename << " not found ";
					}
					else
					{
						frame->OpenFile( filename );
					}
				}
			}
			else
			{
				if ( astroconfig->general.openNewDocOnStart )
				{
					frame->NewFile( gdiconfig->multipleView.useMultipleViews );
				}
			}
			// set size again. macos increases y-size per start
			frame->SetSize( size );
		}
		catch( const std::exception &e )
		{
			errorlog << "General exception during startup: " << e.what();
			//assert( false );
		}
		return true;
	}

	/*****************************************************
	**
	**   GuiApplication   ---   OnAssertFailure
	**
	******************************************************/
	// avoid compiler warnings about unsed params
#ifdef SHOW_APP_DEBUG_ASSERTS
	virtual void OnAssertFailure( const wxChar *file, int	line, const wxChar *func, const wxChar *cond, const wxChar *msg ) override
	{
		wxApp::OnAssertFailure( file, line, func, cond, msg );
#else
	virtual void OnAssertFailure( const wxChar*, int, const wxChar*, const wxChar*, const wxChar* ) override
	{
#endif

		nbDebugAsserts++;
	}

	/*****************************************************
	**
	**   GuiApplication   ---   OnExit
	**
	******************************************************/
	virtual int OnExit() override
	{
		if ( nbDebugAsserts > 0 ) debuglog << nbDebugAsserts <<  " debug asserts";
		astroconfig->save();
		gdiconfig->save();
		delete astroconfig;
		delete gdiconfig;
		return 0;
	}

	ApplicationWindow *frame = nullptr;

	int nbDebugAsserts = 0;
};

#ifdef __MINGW64__
IMPLEMENT_APP_CONSOLE(GuiApplication);
#else
IMPLEMENT_APP(GuiApplication);
#endif


