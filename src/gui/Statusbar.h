/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/Statusbar.h
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

#ifndef _STATUSBAR_H_
#define _STATUSBAR_H_

#include "bootstrap.h"

#include <wx/statusbr.h>

class wxTimer;
class wxTimerEvent;

/*************************************************//**
*
*  status bar with planet view feature
*
******************************************************/
class MainWindowStatusbar : public wxStatusBar
{
public:

	MainWindowStatusbar( wxWindow *parent );

	~MainWindowStatusbar();

	void echo( const wxString& );

private:

	void OnShow( wxShowEvent& );

	void StartPlanetTimer( const bool& );

	void OnRightDown( wxMouseEvent& );

	void OnMessageTimer( wxTimerEvent& );

	void OnPlanetTimer( wxTimerEvent& );

	Horoscope_ptr horoscope;

	wxTimer *msgtimer = nullptr;

	wxTimer *planettimer = nullptr;

	const bool showAsc = false;

	const bool showMoon = false;

  enum
	{
		Field_Text,
		Field_AscPos,
		Field_MoonPos,
		Field_Max
	};

};

#endif

