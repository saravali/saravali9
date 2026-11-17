/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/WindowDirectory.cpp
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

#include "WindowDirectory.h"

#include "ChildWindow.h"
#include "guibase.h"
#include "Horoscope.h"
#include "logger.h"

/*
Events

PartnerView
Horosocpe list changed

AppView
active window changed (toolbar)
selected window changed (toolbar)

Menubar
active window changed
selected window changed
Horoscope data changed (recent file) -> Horoscope (falsch: saved, oder?)

Windowlist
active window changed
selected window changed

Horoscope label changed -> Horoscope

HOROSCOPE_LIST_CHANGED
HOROSCOPE_DATA_CHANGED
SELECTED_VIEW_CHANGED
SELECTED_VIEW_CHANGED
VIEW_ADDED
VIEW_REMOVED

*/

/*****************************************************
**
**   WindowDirectory   ---   Constructor
**
******************************************************/
WindowDirectory::WindowDirectory()
{
	activeWindow = nullptr;
}

/*****************************************************
**
**   WindowDirectory   ---   subscribe
**
******************************************************/
void WindowDirectory::subscribe( wxWindow *w )
{
	subscribers.push_back( w );
}

/*****************************************************
**
**   WindowDirectory   ---   unsubscribe
**
******************************************************/
void WindowDirectory::unsubscribe( wxWindow *w )
{
	const auto subscriber = std::find( subscribers.begin(), subscribers.end(), w );
	if ( subscriber == subscribers.end())
	{
		errorlog << "WindowDirectory::unsubscribe: invalid subscriber, cannot unsubscribe";
	}
	else
	{
		subscribers.erase( subscriber );
	}
}

/*****************************************************
**
**   WindowDirectory   ---   addWindow
**
******************************************************/
void WindowDirectory::addWindow( ChildWindow *window )
{
	debuglog << "WindowDirectory::addWindow";
#if 0
	for( auto *v : windows )
	{
		assert( v );
		debuglog << "WindowDirectory window " << v->GetTitle();
	}
#endif

	assert( window );
	windows.push_back( window );
	emit( WD_WINDOW_ADDED, windows.size() - 1 );

	// insert horoscope if required
	Horoscope_ptr horoscope = window->getHoroscope();
	if ( horoscope )
	{
		const auto founddoc = std::find( horoscopes.begin(), horoscopes.end(), horoscope );
		if ( founddoc == horoscopes.end())
		{
			horoscopes.push_back( horoscope );
			horoscopeListChanged();
		}
	}
}

/*****************************************************
**
**   WindowDirectory   ---   releaseWindow
**
******************************************************/
void WindowDirectory::releaseWindow( ChildWindow *window )
{
	assert( window );
	static int count = 0;
	debuglog << "WindowDirectory::releasewindow " << count;

#if 0
	for( auto *v : windows )
	{
		assert( v );
		debuglog << "WindowDirectory window " << v->GetTitle();
	}
#endif

	if ( windows.size() == 0 )
	{
		errorlog << "cannot release window: list is empty";
		return;
	}

	const auto foundchild = std::find( windows.begin(), windows.end(), window );
  if ( foundchild == windows.end())
  {
		errorlog << "WindowDirectory::releaseWindow: cannot release window " << window->GetTitle() << " (not found)";
  }
	else
	{
		emit( WD_WINDOW_REMOVED, getIndexForWindow( window ));
		windows.erase( foundchild );
	}
}

/*****************************************************
**
**   WindowDirectory   ---   setActiveWindow
**
******************************************************/
void WindowDirectory::setActiveWindow( ChildWindow *window )
{
	const int index = getIndexForWindow( window );
	if ( index >= 0 )
	{
		activeWindow = window;
		emit( WD_WINDOW_ACTIVATED, index );
	}
	else
	{
		activeWindow = nullptr;
		errorlog << "WindowDirectory::setActiveWindow: Cannot set active window " << window << " windows size " << windows.size();
	}
}

/*****************************************************
**
**   WindowDirectory   ---   closeSiblings
**
******************************************************/
void WindowDirectory::closeSiblings( ChildWindow *window )
{
	assert( window );
	debuglog << "WindowDirectory::closeSiblings window " << window->GetTitle();

	Horoscope_ptr horoscope = window->getHoroscope();
	if ( horoscope )
	{
		for( ChildWindow *v : getWindowsForHoroscope( horoscope ))
		{
			if ( v != window )
			{
				assert( v );
				debuglog << "WindowDirectory::closeSiblings dependent window " << v->GetTitle();
				v->Close();
			}
		}
	}
}

/*****************************************************
**
**   WindowDirectory   ---   setActiveWindow
**
******************************************************/
void WindowDirectory::setActiveWindow( const int &pos )
{
	if ( pos < 0 || pos >= (int)windows.size())
	{
		errorlog << "WindowDirectory::setActiveWindow: cannot set selection "
			<< pos << ", size of windows " << windows.size();
	}
	else
	{
		activeWindow = getWindow( pos );
		emit( WD_WINDOW_ACTIVATED, pos );
	}
}

/*****************************************************
**
**   WindowDirectory   ---   getActiveWindow
**
******************************************************/
ChildWindow *WindowDirectory::getActiveWindow()
{
	if ( activeWindow
		&& std::find( windows.begin(), windows.end(), activeWindow ) == windows.end())
	{
		errorlog << "WindowDirectory::getActiveWindow: item is not in window list";
		activeWindow = nullptr;
	}
	debuglog <<  "WindowDirectory::getActiveWindow activeWindow " << activeWindow << " window count " << windows.size();
	return activeWindow;
}

/*****************************************************
**
**   WindowDirectory   ---   getWindowsForHoroscope
**
******************************************************/
std::list<ChildWindow*> WindowDirectory::getWindowsForHoroscope( Horoscope_ptr horoscope ) const
{
	std::list<ChildWindow*> l;
	if ( horoscope )
	{
		for( auto *v : windows )
		{
			if ( v->getHoroscope() && v->getHoroscope() == horoscope )
			{
				l.push_back( v );
			}
		}
	}
	return l;
}

/*****************************************************
**
**   WindowDirectory   ---   getIndexForWindow
**
******************************************************/
int WindowDirectory::getIndexForWindow( ChildWindow *cw ) const
{
	int i = 0;
	for( auto *v : windows )
	{
		if ( cw == v ) return i;
		++i;
	}
	errorlog << "WindowDirectory::getIndexForWindow: window not found " << cw << ", size " << windows.size();
	return -1;
}

/*****************************************************
**
**   WindowDirectory   ---   windowTitleChanged
**
******************************************************/
void WindowDirectory::windowTitleChanged( ChildWindow *window )
{
	const int index = getIndexForWindow( window );
	debuglog << "WindowDirectory::windowTitleChanged index " << index;
	if ( index != -1 )
	{
		emit( WD_WINDOW_TITLE_CHANGED, index );
	}
	else
	{
		errorlog << "WindowDirectory::windowTitleChanged: cannot find window, size is " << windows.size();
	}
}

/*****************************************************
**
**   WindowDirectory   ---   getUnsavedHoroscopeCount
**
******************************************************/
int WindowDirectory::getUnsavedHoroscopeCount() const
{
	int i = 0;
	for( Horoscope_ptr horoscope : horoscopes )
	{
		assert( horoscope );
		if ( horoscope->dl.dirty ) ++i;
	}
	return i;
}

/*****************************************************
**
**   WindowDirectory   ---   forgetHoroscope
**
******************************************************/
void WindowDirectory::forgetHoroscope( Horoscope_ptr horoscope )
{
	const auto a = std::find( horoscopes.begin(), horoscopes.end(), horoscope );
	if ( a != horoscopes.end())
	{
		horoscopes.erase( a );
		horoscopeListChanged();
	}
	else
	{
		warnlog << "WindowDirectory::forgetHoroscope cannot erase horoscope from list, not found";
	}
}

/*****************************************************
**
**   WindowDirectory   ---   horoscopeSaved
**
******************************************************/
void WindowDirectory::horoscopeSaved( Horoscope_ptr horoscope )
{
	const int index = getIndexForHoroscope( horoscope );
	debuglog << "WindowDirectory::horoscopeSaved index " << index;
	if ( index != -1 )
	{
		emit( WD_HOROSCOPE_SAVED, index, horoscope->dl.filename );
	}
	else
	{
		errorlog << "WindowDirectory::horoscopeSaved cannot find Horoscope, size is " << horoscopes.size();
	}
}

/*****************************************************
**
**   WindowDirectory   ---   horoscopeDataChanged
**
******************************************************/
void WindowDirectory::horoscopeDataChanged( Horoscope_ptr horoscope )
{
	const int index = getIndexForHoroscope( horoscope );
	debuglog << "WindowDirectory::horoscopeDataChanged index " << index;
	if ( index != -1 )
	{
		emit( WD_HOROSCOPE_DATA_CHANGED, index );
	}
	else
	{
		errorlog << "WindowDirectory::horoscopeDataChanged cannot find Horoscope, size is " << horoscopes.size();
	}
}

/*****************************************************
**
**   WindowDirectory   ---   horoscopeListChanged
**
******************************************************/
void WindowDirectory::horoscopeListChanged()
{
	debuglog << "WindowDirectory::horoscopeListChanged";
	emit( WD_HOROSCOPE_LIST_CHANGED );
}

/**************************************************************
***
**   WindowDirectory   ---   getHoroscope
***
***************************************************************/
Horoscope_ptr WindowDirectory::getHoroscope( const uint &i )
{
	if ( i < horoscopes.size())
	{
		auto iter = horoscopes.begin();
		std::advance( iter, i );
		return *iter;
	}
	errorlog << "WindowDirectory::getHoroscope has invalid index " << i << ", horoscopes size is " << horoscopes.size();
	return nullptr;
}

/**************************************************************
***
**   WindowDirectory   ---   getIndexForHoroscope
***
***************************************************************/
int WindowDirectory::getIndexForHoroscope( Horoscope_ptr horoscope ) const
{
	int i = 0;
	for( auto h : horoscopes )
	{
		if ( h == horoscope ) return i;
		++i;
	}
	errorlog << "WindowDirectory::getIndexForHoroscope: not found " << horoscope.get() << ", size " << horoscopes.size();
	return -1;
}

/**************************************************************
***
**   WindowDirectory   ---   getWindow
***
***************************************************************/
ChildWindow *WindowDirectory::getWindow( const uint &i )
{
	//assert( i < windows.size());
	if ( i < windows.size())
	{
		auto iter = windows.begin();
		std::advance( iter, i );
		return *iter;
	}
	else
	{
		errorlog << "WindowDirectory::getWindow with invalid index " << i << ", size is " << windows.size();
	}
	return nullptr;
}

/**************************************************************
***
**   WindowDirectory   ---   getNamesArray
***
***************************************************************/
std::list<wxString> WindowDirectory::getNames() const
{
	std::list<wxString> names;
	for( Horoscope_ptr horoscope : horoscopes )
	{
		names.push_back( horoscope->dl.name );
	}
	return names;
}

/**************************************************************
***
**   WindowDirectory   ---   emit
***
***************************************************************/
void WindowDirectory::emit( const int &command, const int id, const wxString infoString )
{
	debuglog << "WindowDirectory::emit INT " << command << " size subscribers " << subscribers.size();
	for( wxWindow *w : subscribers )
	{
		wxCommandEvent e( command );
		e.SetExtraLong( id );
		if ( ! infoString.empty()) e.SetString( infoString );
		wxPostEvent( w, e );
	}
}

