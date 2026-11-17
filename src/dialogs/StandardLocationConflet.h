/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/StandardLocationConflet.h
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

#ifndef _STANDARDLOCATIONCONFLET_H_
#define _STANDARDLOCATIONCONFLET_H_

#include "Conflet.h"
#include "astroconfig.h"

class wxButton;
class wxChoice;
class wxRadioBox;
class wxStaticBox;
class wxStaticText;
class wxTextCtrl;
class wxTimer;
class wxTimerEvent;
class CombinedDegreeSpin;
class ComboDegreeCtrl;

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class StandardLocationConflet : public ConfletIncarnation<Location>
{
public:
	// begin wxGlade: StandardLocationConflet::ids
	// end wxGlade

	StandardLocationConflet( wxWindow* parent );

	~StandardLocationConflet();

	virtual void updateUi() override;

private:

	void OnTextEnter( wxCommandEvent& );
	void OnSearchLocation( wxCommandEvent& );
	void searchLocation();

	void OnTimer( wxTimerEvent& );
	virtual void onActivate() override;
	virtual void onPassivate() override;

	virtual bool TransferDataToWindow() override;
	virtual bool TransferDataFromWindow() override;

	wxTimer *timer;


protected:
	// begin wxGlade: StandardLocationConflet::attributes
	wxTextCtrl* text_name;
	wxButton* button_search;
	wxStaticText* label_longitude;
	CombinedDegreeSpin* degree_longitude;
	wxStaticText* label_latitude;
	CombinedDegreeSpin* degree_latitude;
	wxStaticText* label_timezone;
	CombinedDegreeSpin* degree_timezone;
	wxRadioBox* radio_tzmode;
	wxPanel* panel_currenttime;
	wxStaticText* label_localtime;
	wxStaticText* text_localtime;
	wxStaticText* label_uttime;
	wxStaticText* text_uttime;
	wxStaticText* label_tzoffset;
	wxStaticText* text_tzoffset;
	wxStaticText* label_dstoffset;
	wxStaticText* text_dstoffset;
	wxStaticText* label_viewpoint;
	wxStaticText* text_viewpoint;
	wxStaticText* label_ascendant;
	wxStaticText* text_ascendant;
	wxStaticText* label_meridian;
	wxStaticText* text_meridian;
	// end wxGlade
}; // wxGlade: end class


#endif // _STANDARDLOCATIONCONFLET_H_
