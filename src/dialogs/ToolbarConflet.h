/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ToolbarConflet.h
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

#ifndef TOOLBARCONFLET_H
#define TOOLBARCONFLET_H

#include "Conflet.h"
#include "gdiconfig.h"

class wxBitmapButton;
class wxChoice;
class wxImageList;
class wxListCtrl;
class wxListEvent;
class wxStaticText;
enum class BitmapId;

// begin wxGlade: ::dependencies
#include <wx/listctrl.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class ToolbarConflet : public ConfletIncarnation<ToolbarConfig>
{
public:
	// begin wxGlade: ToolbarConflet::ids
	// end wxGlade

	ToolbarConflet( wxWindow* parent );

	~ToolbarConflet();

	virtual void model2config() override;

	virtual void restoreDefaults() override;

private:

	void OnLeft( wxCommandEvent& );
	void OnRight( wxCommandEvent& );
	void OnUp( wxCommandEvent& );
	void OnDown( wxCommandEvent& );

	void OnLeftSelected( wxListEvent& );
	void OnRightSelected( wxListEvent& );
	void OnChoice( wxCommandEvent& );

	void updateItemConfig();
	void initToolItems();
	void initRightControl();
	void insertRightItemById( const CommandId &commmandid, const int &pos, const bool doarrange = true );

	wxImageList *imageList;
	std::map< BitmapId, int > imageListIds;
	int left_sel, right_sel;


protected:
	// begin wxGlade: ToolbarConflet::attributes
	wxListCtrl* list1;
	wxBitmapButton* button_right;
	wxBitmapButton* button_left;
	wxBitmapButton* button_up;
	wxBitmapButton* button_down;
	wxListCtrl* list2;
	wxStaticText* label_toolstyle;
	wxChoice* choice_toolstyle;
	// end wxGlade
}; // wxGlade: end class


#endif // TOOLBARCONFLET_H
