/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/TableStyleConflet.h
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

#ifndef TABLESTYLECONFLET_H_
#define TABLESTYLECONFLET_H_

#include "ConfigListItems.h"
#include "Conflet.h"

#include <vector>

class wxButton;
class wxCheckBox;
class wxChoice;
class wxColourPickerCtrl;
class wxStaticText;
class wxTextCtrl;
class TextWidget;

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class TableStyleConflet : public ConfletIncarnation<int>
{
public:
	// begin wxGlade: TableStyleConflet::ids
	// end wxGlade

	TableStyleConflet( wxWindow* parent );

	~TableStyleConflet();

	virtual void updateUi() override;

	virtual void config2model() override;

	virtual void model2config() override;

	virtual void restoreDefaults() override;

private:

	void OnCommand( wxCommandEvent& );

	void OnStyleManager( wxCommandEvent& );

	void updateStyleChoice();

	void OnSelectTableStyle( wxCommandEvent& );

	void setControlValues();

	std::vector<TableStyle> tableStyles;

	ContextProps_ptr props;

	Horoscope_ptr horoscope;

protected:
	// begin wxGlade: TableStyleConflet::attributes
	wxStaticText* label_tablestyle;
	wxChoice* choice_tablestyle;
	wxButton* button_manage;
	wxCheckBox* check_use_header_colors;
	wxStaticText* label_header_textcolor;
	wxColourPickerCtrl* button_header_textcolor;
	wxStaticText* label_header_bgcolor;
	wxColourPickerCtrl* button_header_bgcolor;
	wxCheckBox* check_use_cell_colors;
	wxStaticText* label_cell_textcolor;
	wxColourPickerCtrl* button_cell_textcolor;
	wxStaticText* label_cell_bgmode;
	wxChoice* choice_cell_bgmode;
	wxStaticText* label_all_row_bgcolor;
	wxColourPickerCtrl* button_all_cell_bgcolor;
	wxStaticText* label_even_row_bgcolor;
	wxColourPickerCtrl* button_even_cell_bgcolor;
	wxStaticText* label_odd_row_bgcolor;
	wxColourPickerCtrl* button_odd_cell_bgcolor;
	wxCheckBox* check_show_grid;
	wxPanel* panel_preview;
	TextWidget* preview;
	// end wxGlade
}; // wxGlade: end class


#endif // TABLESTYLECONFLET_H_
