/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/YogaEditorPanel.h
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

#ifndef YOGAEDITORPANEL_H
#define YOGAEDITORPANEL_H

#include "ConfigListItems.h"

#include <wx/panel.h>
#include <vector>

// begin wxGlade: ::dependencies
#include <wx/listctrl.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

class wxBitmapButton;
class wxButton;
class wxChoice;
class wxListCtrl;
class wxListEvent;
class wxStaticBox;
class wxStaticText;
class wxTextCtrl;

class DragChart;
struct YogaConfig;
enum class PlanetId;

enum { YE_YOGA_PANEL = wxID_HIGHEST + 100, YE_YOGA_LIST, YE_OPENFILE,
	YE_PARSE_RULE, YE_FORMAT_RULE, YE_FORMAT_ALL_RULES, YE_NEW_SOURCE,
	YE_NEW_GROUP, YE_DESCRIPTION, YE_EVALUATE, YE_UP, YE_DOWN, YE_DELETE_YOGA,
	YE_NEW_YOGA, YE_COPY_YOGA, YE_CLEAR_CHART, YE_OBJECTS,
	YE_TEXT_EFFECT, YE_TEXT_RULE, YE_TEXT_DESCRIPTION,
	YE_CHOICE_GROUP, YE_CHOICE_SOURCE, YE_CHECK_VARGAS
};

/*************************************************//**
*
*  Main panel for YogaEditor
*
******************************************************/
class YogaEditorPanel : public wxPanel
{
public:
    // begin wxGlade: YogaEditorPanel::ids
    // end wxGlade

	YogaEditorPanel( wxWindow*, std::vector<YogaConfig>& );

	void addSource( wxString );

	void addGroup( wxString );

	void loadYogas();

	void clearGui();

	void clearChart();

	wxString getRuleText() const;

	std::map<PlanetId,Sign> getPlanetData() const;

	bool isDirty() const { return dirty; }
	void setDirty( const bool b = true ) { dirty = b; }

	void echoOnDragChart( wxString );

	void newItem();

private:

	void OnFieldChanged( wxCommandEvent& );
	void updateFieldStatus();
	void storeFields();

	void OnYogaListLeftSelected( wxListEvent& );

	void OnYogaListItemUp( wxCommandEvent& );
	void OnYogaListItemDown( wxCommandEvent& );
	void OnYogaListNewItem( wxCommandEvent& ) { newItem(); }
	void OnYogaListDeleteItem( wxCommandEvent& );
	void OnYogaListCopyItem( wxCommandEvent& );

	void propagateDirtyEvent();

	void updateLabels();

	std::vector<YogaConfig>::iterator getItemAt( const long &index );

	long getCurrentItemId();

	YogaConfig *getCurrentItem();

	YogaConfig &getYogaConfig( const long &item );

	bool propagateUserInput;

	bool dirty = false;

  std::vector<YogaConfig> &yogas;

protected:

	virtual void doPaint( bool = true, const wxRect* = 0 ) {}

    // begin wxGlade: YogaEditorPanel::attributes
    wxListCtrl* list_yogas;
    wxBitmapButton* button_up;
    wxBitmapButton* button_down;
    wxBitmapButton* button_new_yoga;
    wxBitmapButton* button_delete_yoga;
    wxBitmapButton* button_copy_yoga;
    wxStaticText* label_description;
    wxTextCtrl* text_description;
    wxStaticText* label_group;
    wxChoice* choice_group;
    wxBitmapButton* button_new_group;
    wxStaticText* label_source;
    wxChoice* choice_source;
    wxBitmapButton* button_new_source;
    wxTextCtrl* text_effect;
    wxTextCtrl* text_rule;
    DragChart* drag_chart;
    wxButton* button_parse_rule;
    wxButton* button_evaluate;
    wxButton* button_clear_chart;
    // end wxGlade
}; // wxGlade: end class


#endif // YOGAEDITORPANEL_H
