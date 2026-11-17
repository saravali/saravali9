/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/PrintoutConflet.h
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

#ifndef PRINTOUTCONFLET_H_
#define PRINTOUTCONFLET_H_

#include "Conflet.h"
#include "PrintoutConfig.h"

#include <vector>

class wxButton;
class wxChoice;
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
class PrintoutConflet : public ConfletIncarnation<PrintoutConfig>
{
public:
	// begin wxGlade: PrintoutConflet::ids
	// end wxGlade

	PrintoutConflet( wxWindow* parent );

	~PrintoutConflet();

	virtual void updateUi() override;

private:

	void updateCategoryDepChoices();

	void OnChoiceCategory( wxCommandEvent& ) { updateCategoryDepChoices(); }

	void OnChoiceGeom( wxCommandEvent& );

	void OnChoiceSkin( wxCommandEvent& );

	void OnPrintTestPage( wxCommandEvent& );

	ContextProps_ptr props;

	// maps choice box selection to geom id
	std::vector<int> geom_mapping;

protected:

	virtual void restoreDefaults() override;

	// begin wxGlade: PrintoutConflet::attributes
	wxChoice* choice_printout_type;
	wxStaticText* label_description;
	wxStaticText* label_viewpoint;
	wxStaticText* label_itemcount;
	wxStaticText* label_planets;
	wxStaticText* label_columns;
	wxStaticText* label_charts;
	wxStaticText* label_basedata;
	wxChoice* choice_category;
	wxChoice* choice_geom;
	wxChoice* choice_skin;
	wxChoice* choice_tablestyle;
	wxButton* button_test_page;
	// end wxGlade
}; // wxGlade: end class


#endif // PRINTOUTCONFLET_H_
