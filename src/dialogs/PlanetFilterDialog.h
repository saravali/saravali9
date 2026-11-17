/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PlanetFilterDialog.h
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

#ifndef _PLANETFILTERDIALOG_H
#define _PLANETFILTERDIALOG_H

#include <wx/dialog.h>

// begin wxGlade: ::dependencies
#include <wx/statline.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

#include "constants.h"
#include "astroconfig.h"

class wxButton;
class wxCheckListBox;
class wxChoice;
class wxStaticBox;
class wxStaticLine;
class wxStaticText;
enum class UranianContext;

/*************************************************//**
*
*  Manages the filter for several Uranian views and is integrated in the Uranian side panel.
*
******************************************************/
class PlanetFilterDialog: public wxDialog
{
public:
    // begin wxGlade: PlanetFilterDialog::ids
    // end wxGlade

	/**
	* The isDoubleSided flag determines whether the "Matching Side" choice will be enabled. Period view does not
	* use it.
	*/
	PlanetFilterDialog( wxWindow* parent, ContextProps_ptr, const UranianContext&, const HoroscopeRole&, const MatchFilter& );

	~PlanetFilterDialog();

	MatchFilter getFilter() const { return filter; }

protected:

	void updateSelections();

	void OnItemChecked( wxCommandEvent& );

	void OnAll( wxCommandEvent& );
	void OnFastPlanets( wxCommandEvent& );
	void OnSlowPlanets( wxCommandEvent& );
	void OnNone( wxCommandEvent& );
	void OnOK( wxCommandEvent& );

	ContextProps_ptr props;

	MatchFilter filter;

	const wxString configkey;

	std::vector<std::pair<PlanetId, bool>> model;

    // begin wxGlade: PlanetFilterDialog::attributes
    wxCheckListBox* list;
    wxButton* button_none;
    wxButton* button_fast_planets;
    wxButton* button_slow_planets;
    wxButton* button_all;
    wxChoice* choice_side;
    wxChoice* choice_rule;
    wxStaticLine* static_line_2;
    wxButton* button_ok;
    wxButton* button_cancel;
    // end wxGlade
}; // wxGlade: end class


#endif // _PLANETFILTERDIALOG_H
