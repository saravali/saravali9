/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/Sidebar.h
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

#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "constants.h"

#include <wx/panel.h>

enum { SBI_SIDEPANEL = wxID_HIGHEST + 3000 };

class DivisionChoice;
class FilterPanel;
class FormularyPanel;
class DegreeSpin;
class RestrictorPanel;
class SidebarPanel;
class TextOutputField;
class UranianHouseChoice;
struct UranianSection;
enum class SidebarItem;

class wxButton;
class wxChoice;
class wxCheckBox;
class wxSpinCtrl;
class wxSpinEvent;
class wxStaticText;

// eventfinder widget
enum
{
	SBE_BUTTON_UPDATE = wxID_HIGHEST + 1300,
};

/*************************************************//**
*
*  Creates itme for tht SidebarPanel according to the view context.
*
******************************************************/
class SidebarPanelFactory
{
public:

	SidebarPanel *createSidebarPanel( wxWindow* parent, ContextProps_ptr, const UranianContext&, UranianSection* );

protected:

	void addItemGroup( SidebarPanel*, const wxString &title, const std::list<SidebarItem>& );

	wxSizer *createItemGridSizer( SidebarPanel*, const std::list<SidebarItem>& );

	void addSingleItems( SidebarPanel*, wxSizer*, const std::list<SidebarItem>& );

	void addSeparator( SidebarPanel* );

	wxCheckBox *addCheckboxItem( wxSizer*, wxWindow *parent, const int &id, const wxString &label, bool *cfgitem, const bool updateNeeded = true );

	ContextProps_ptr props;

	UranianContext ucontext;

	UranianSection *usection;
};

/*************************************************//**
*
*  Additional panel for Uranian views. Shown on the left side of the widgets.
*
******************************************************/
class SidebarPanel : public wxPanel
{
	friend class SidebarPanelFactory;

public:

	SidebarPanel( wxWindow* parent, int id, ContextProps_ptr, const UranianContext&, UranianSection* );

	void setDegreeSpinValue( const double& );

	void updateUi();

	wxButton *getUpdateButton() { return buttonUpdate; }

protected:

	void OnCommand( wxCommandEvent& );

	void OnMatchFormulas( wxCommandEvent& );

	void OnChoice( wxCommandEvent& );

	void OnSortCommand( wxCommandEvent& );

	void OnOffsetSpin( wxSpinEvent& );

	void OnSpin( wxSpinEvent& );

	void OnRefreshCommand( wxCommandEvent& );

	void OnToggleOffset( wxCommandEvent& );

	void OnCheckOffsetToggle( wxCommandEvent& );

	void OnIdle( wxIdleEvent& );

	void emitChangeEvent();

	ContextProps_ptr props;

	const UranianContext ucontext;

	UranianSection *usection = nullptr;

	UranianHouseChoice *choiceHouse = nullptr;

	wxStaticText *labelHouseType = nullptr;

	DivisionChoice *choiceChartDivision = nullptr;

	DivisionChoice *choiceMatchDivision = nullptr;

	wxChoice *choiceProgressionMode = nullptr;

	wxStaticText *labelProgressionMode = nullptr;

	wxChoice *choicePartnerDataSource = nullptr;

	wxSpinCtrl *spinOrbis =  nullptr;

	DegreeSpin *spinOffset = nullptr;

	wxCheckBox *checkDueDateTime = nullptr;

	wxButton *buttonUpdate = nullptr;

	RestrictorPanel *restrictorPanel = nullptr;

	TextOutputField *matchCountField = nullptr;

	int myMatchCount = -1;

	FormularyPanel *formularyPanel = nullptr;

	FormularyPanel *matchFormularyPanel = nullptr;
};


#endif // SIDEBAR_H
