/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/WindowDirectory.h
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

#ifndef _WINDOWDIRECTORY_H_
#define _WINDOWDIRECTORY_H_

#include "bootstrap.h"

#include <list>
#include <wx/string.h>

class ChildWindow;
class wxWindow;

/*************************************************//**
*
* Holds dynamic lists of current ChildWindow and Horoscope instances and distributes 
* status information among serveral GUI componenents.
*
* The class implements a simple publish/suscribe mechanism. Clients must suscribe and
* will then be notified via custom wxCommandEvent.
*
* wxCommandEvent can have the id of the relevant list item, access via GetExtraLong() method
*
* Subscribers (information consumers) are
* - ApplicationWindow (enable/disable toolbar items)
* - AppMenubar (enable/disable menu items)
* - WindowListCtrl (several: add/remove/enable entry, update labels)
* - PartnerWidget (update of Horoscope list)
*
******************************************************/
class WindowDirectory : public Singleton<WindowDirectory> //, public wxEvtHandler
{
  friend class Singleton<WindowDirectory>;

public:

	/**
	* Called for registration.
	*/
	void subscribe( wxWindow* );

	/**
	* Unregisters the given window
	*/
	void unsubscribe( wxWindow* );

	/**
	* Add a window. The included Horoscope will be added (uniquely) to the Horoscope list (if not null).
	*
	* Called by
	* - AppliationWindow addWindow
	*/
	void addWindow( ChildWindow* );

	/**
	* Window will be removed from the list. Associated Horoscope instance must be removed separately.
	*
	* Called by
	* - ChildWindow OnClose
	*/
	void releaseWindow( ChildWindow* );

	void closeSiblings( ChildWindow* );

	/**
	* Called by
	* - SelectableTextWidget on mode change (title changed)
	*/
	void windowTitleChanged( ChildWindow* );

	/*
	* Called by
	* - ChildWindow onActivate Event
	* - WindowListCtrl OnListItemSelected
	*/
	void setActiveWindow( ChildWindow* );

	void setActiveWindow( const int& );

	/**
	* Returns a list of all ChildWindow references.
	*/
	std::list<ChildWindow*> getWindows() const { return windows; }

	int getWindowCount() { return windows.size(); }

	ChildWindow *getWindow( const uint &i );

	ChildWindow *getActiveWindow();

	/**
	* Determines the list index of a given ChildWindow (or -1 if not found).
	*/
	int getIndexForWindow( ChildWindow* ) const;

	/**
	* Called by
	* - ChildWindow on window close if the Horoscope is not used anymore
	*/
	void forgetHoroscope( Horoscope_ptr );

	/**
	* Called by
	* - BirthDataDialog doApply
	*/
	void horoscopeDataChanged( Horoscope_ptr );

	/**
	* Called by
	* - ChildWindow on save/saveAs
	*/
	void horoscopeSaved( Horoscope_ptr );

	/**
	* Iterates through all Horoscope instances and counts the number of dirty/unsaved items.
	*/
	int getUnsavedHoroscopeCount() const;

	/**
	* Returns a list of ChildWindow references referrring to the given Horoscope.
	*/
	std::list<ChildWindow*> getWindowsForHoroscope( Horoscope_ptr ) const;

	int getHoroscopeCount() { return horoscopes.size(); }

	int getIndexForHoroscope( Horoscope_ptr ) const;

	std::list<Horoscope_ptr> getHoroscopes() const { return horoscopes; }

	/**
	* Returns a list of the names in the Horoscope list.
	*/
	std::list<wxString> getNames() const;

	Horoscope_ptr getHoroscope( const uint &i );

protected:

	WindowDirectory();

	void emit( const int &command, const int = -1, const wxString = "" );

	void horoscopeListChanged();

	std::list<Horoscope_ptr> horoscopes;

	std::list<ChildWindow*> windows;

	std::list<wxWindow*> subscribers;

	ChildWindow *activeWindow;
};


#endif // _WINDOWDIRECTORY_H_
