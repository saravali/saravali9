/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/DasaConflet.h
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

#ifndef _DASACONFLET_H_
#define _DASACONFLET_H_

#include "Conflet.h"
#include "astroconfig.h"

class wxChoice;
class wxStaticText;
class YearLengthPanel;

// begin wxGlade: ::dependencies
#include <wx/spinctrl.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class DasaConflet : public ConfletIncarnation<DasaCalculationConfig>
{
public:
	// begin wxGlade: DasaConflet::ids
	// end wxGlade

	DasaConflet( wxWindow* parent );

private:

protected:
	// begin wxGlade: DasaConflet::attributes
	wxStaticText* label_nakdasa;
	wxChoice* choice_nakdasa;
	wxStaticText* label_dasa_enddate;
	wxChoice* choice_dasa_show_date_mode;
	wxStaticText* label_last_mahadasa;
	wxSpinCtrl* spin_last_mahadasa;
	wxStaticText* label_last_mahadasa2;
	YearLengthPanel* yearLengthPanel;
	wxStaticText* label_kalachakra;
	wxChoice* choice_kala;
	// end wxGlade
}; // wxGlade: end class


#endif // _DASACONFLET_H_
