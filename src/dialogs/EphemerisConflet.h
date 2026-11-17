/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/EphemerisConflet.h
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

#ifndef EPHEMERISCONFLET_H
#define EPHEMERISCONFLET_H

#include "Conflet.h"
#include "astroconfig.h"

class wxButton;
class wxCheckBox;
class wxChoice;
class wxStaticBox;
class wxStaticText;
class wxTextCtrl;

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class EphemerisConflet : public ConfletIncarnation<EphemConfig>
{
public:
	// begin wxGlade: EphemerisConflet::ids
	// end wxGlade

	EphemerisConflet( wxWindow* parent );

	virtual void updateUi() override;

private:

	void OnCommand( wxCommandEvent& );
	void OnSelectSEPath( wxCommandEvent& );


protected:
	// begin wxGlade: EphemerisConflet::attributes
	wxStaticText* label_timemodel;
	wxChoice* choice_timemodel;
	wxStaticText* label_coord;
	wxChoice* choice_coord;
	wxStaticText* label_pos;
	wxChoice* choice_pos;
	wxStaticText* label_sunrise_disk;
	wxChoice* choice_sunrise_disk;
	wxCheckBox* check_sunrise_refrac;
	wxButton* button_sepath;
	wxTextCtrl* text_sepath;
	// end wxGlade
}; // wxGlade: end class


#endif // EPHEMERISCONFLET_H
