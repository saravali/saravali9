/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/UranianControls.h
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

#ifndef URANIANCONTROLS_H_
#define URANIANCONTROLS_H_

#include "constants.h"

#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/spinctrl.h>
#include <list>
#include <map>
#include <vector>

struct MatchFilter;
struct RoleRestrictor;
struct UranianSection;
enum class PlanetId;

class wxButton;
class wxCheckBox;
class wxStaticBox;
class wxStaticText;

/*************************************************//**
*
* Configurable choice for sort orders.
*
* Used by: Sidebar
*
******************************************************/
class SortOrderChoice : public wxChoice
{
public:
	SortOrderChoice( wxWindow*, const int &id, const UranianContext&, EventSortOrder* );

protected:

	void OnSelection( wxCommandEvent& );

	EventSortOrder *order;

	std::vector<EventSortOrder> model;
};

/*************************************************//**
* 
* Text output control: shows single line text (read only) on a separat panel
*
* Used by: FilterPanel
*
******************************************************/
class TextOutputField : public wxPanel
{
public:
	TextOutputField( wxWindow *parent, const int &id, const wxString& );

	void setLabel( const wxString& );

protected:
	wxStaticText *label;
};

/*************************************************//**
* 
* Filter for term elements
*
* Used by: Sidebar
*
******************************************************/
class FilterPanel : public wxPanel
{
public:

	FilterPanel( wxWindow *parent, const int &id, ContextProps_ptr, const UranianContext&, MatchFilter&,
		const HoroscopeRole = HoroscopeRole::None, const bool enabled = true );

	void enableFilterElements( const bool = true );

	bool isEnabled() const;
	
protected:
	void updateFilterLabel();

	void OnCheckEnabled( wxCommandEvent& );

	void OnFilter( wxCommandEvent& );

	void OnClearFilter( wxCommandEvent& );

	void emitChangeEvent();

	wxCheckBox *check_enabled = nullptr;

	wxButton *button_filter = nullptr;

	wxButton *button_clear_filter = nullptr;

	TextOutputField *label_filter = nullptr;

	ContextProps_ptr props;

	const UranianContext ctx;

	MatchFilter &filterCfg;

	const HoroscopeRole role;
};

/*************************************************//**
* 
* Filter group for multiple filters including enabling of roles.
*
* Used by: Sidebar
*
******************************************************/
class RestrictorPanel : public wxPanel
{
public:
	RestrictorPanel( wxWindow *parent, ContextProps_ptr, const UranianContext& );

	bool hasActiveProgressions() const;

protected:

	void OnCheckEnabled( wxCommandEvent& );

	ContextProps_ptr props;

	const UranianContext ctx;

	std::map<HoroscopeRole,FilterPanel*> panels;

	std::list<RoleRestrictor>* restrictors = nullptr;
};

/*************************************************//**
*
* Selects a division in Uranian astrology
*
* Used by: Sidebar, EphemView, SpView
*
******************************************************/
class DivisionChoice : public wxChoice
{
public:
	DivisionChoice( wxWindow *parent, const int &id );

	int getDivision() const;

	void setDivision( const int& );

protected:

	const std::vector<int> divisions;
};

/*************************************************//**
*
* Used by: Sidebar
*
******************************************************/
class UranianHouseChoice : public wxChoice
{
public:
	UranianHouseChoice( wxWindow*, const int &id );

	void setSelectedPlanet( const PlanetId& );

	PlanetId getSelectedPlanet() const;

private:
	std::vector<PlanetId> planets;
};

/*************************************************//**
*
* Multichoice for TermIds.
* 
* Used by: Sidebar
*
******************************************************/
class FormularyPanel : public wxPanel
{
public:
	FormularyPanel( wxWindow *parent, const int &id, const UranianContext&, std::list<int>& );

protected:

	std::list<int>& formulae;

	std::list<wxCheckBox*> boxes;

	void OnCheckBox( wxCommandEvent& );

	inline static constexpr int id_check = wxID_HIGHEST + 1700;
};

/*************************************************//**
*
* Select the total time span for period matches calculation
*
* Initiated by: Toolbar
*
* Used by: PeriodView
*
******************************************************/
class PeriodDurationChoice : public wxChoice
{
public:
	PeriodDurationChoice( wxWindow *parent, int id );

	double getDuration() const;
};

/*************************************************//**
*
* Select the term in SpView
*
* Used by: SpView
*
******************************************************/
class SpTermChoice : public wxChoice
{
public:
	SpTermChoice( wxWindow *parent, const int = -1 );

	TermId getSelectedTermId() const;

	void setSelectedTerm( const TermId& );

public:
	const std::vector<TermId> termIds;
};

/*************************************************//**
*
* Select third object in Uranian triples
*
* Used by: SpView
*
******************************************************/
class SpObjectSelectionChoice : public wxChoice
{
public:
	SpObjectSelectionChoice( wxWindow *parent, const int id = -1 );

	void initPlanets( const Viewpoint& );

	void selectObject( const PlanetId& );

	PlanetId getSelectedPlanetId() const;

protected:

	std::vector<PlanetId> obs;
};

/*************************************************//**
*
* Used by: Sidebar/PartnerWidget
*
******************************************************/
class PartnerShiftPanel : public wxPanel
{
public:
	PartnerShiftPanel( wxWindow *parent, const int &id, UranianSection* );

protected:

	void OnCheckBox( wxCommandEvent& );

	int planetIdToSelection( const PlanetId& );

	void OnChoice( wxCommandEvent& );

	void updateUi();

	void emitChangeEvent();

	UranianSection *usection = nullptr;

	wxCheckBox *check_active = nullptr;

	wxStaticBox *label1 = nullptr;

	wxChoice *choice_partner1 = nullptr;

	wxStaticBox *label2 = nullptr;

	wxChoice *choice_partner2 = nullptr;

	const std::vector<PlanetId> planets;
};

#endif // URANIANCONTROLS_H_

