/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/SpinCtrl.h
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

#ifndef _SPINCTRL_H_
#define _SPINCTRL_H_

#include "constants.h"
#include "Validator.h"

#include <wx/control.h>

enum class BitmapId;
class TimezoneChoice;

class wxButton;
class wxChoice;
class wxSpinButton;
class wxStaticText;
class wxTextCtrl;

/*************************************************//**
*
* 
*
******************************************************/
class BaseSpin : public wxControl
{
public:
	BaseSpin( wxWindow*, const int& id, double *value,
		const wxPoint &pos, const wxSize &size, const double &minvalue, const double &maxvalue, const bool &showLabel );

protected:

	void OnKillFocus( wxFocusEvent& );

	void OnTextChangeEvent( wxCommandEvent& );

	void propagateChangeEvent();

	double *value;

	const bool showLabel = false;

	const int charCount = 10;

	wxTextCtrl *text = nullptr;

	wxSpinButton *spin = nullptr;

	wxStaticText *label = nullptr;

	wxColour textBgColor;

	const double minvalue = 0.0;

	const double maxvalue = 0.0;

	void OnSpinUp( wxSpinEvent& );

	void OnSpinDown( wxSpinEvent& );

};

/*************************************************//**
*
* 
*
******************************************************/
class DegreeSpin : public BaseSpin
{
	friend class CombinedDegreeSpinValidator;
	friend class CombinedDegreeSpin;

public:
	DegreeSpin( wxWindow*, const int& id, double *value,
		const double minvalue = 0.0, const double maxvalue = 360.0,
		const wxPoint pos = wxDefaultPosition, const wxSize size = wxDefaultSize );

protected:

};

/*************************************************//**
*
* Combi control for "degree-like" input fields with min/max support and a choice box for east/west etc. config.
*
* Used by: StandardLocationConflet, AtlasEditDialog, BirthDataDialog
*
******************************************************/
class CombinedDegreeSpin : public wxControl
{
	friend class CombinedDegreeSpinValidator;

public:
	CombinedDegreeSpin( wxWindow *parent, const int &id, double *value, const DegreeType&,
		const wxPoint pos = wxDefaultPosition, const wxSize size = wxDefaultSize );

	void setValue( const double& );

protected:

	void propagateChangeEvent();

	void OnChoice( wxCommandEvent& );

	double *value = nullptr;

	const DegreeType degreeType = DegreeType::None;

	double degreeValue = 0;

	int choiceValue = 0;

	DegreeSpin *degreeCtrl = nullptr;

	wxChoice *choice = nullptr;

};

/*************************************************//**
*
* 
*
******************************************************/
class DateSpin : public BaseSpin
{

public:
	DateSpin( wxWindow*, const int& id, double *value,
		const wxPoint pos = wxDefaultPosition, const wxSize size = wxDefaultSize );

	// used by HoraWidget and EventFinderWidget because DateSpin is part of toolbar and cannot be set a priori
	void changeValuePointer( double* );

protected:

	virtual bool TransferDataToWindow() override;
};

/*************************************************//**
*
* This is a combined date/time input control containing date/time input, timezone choice, progress bars and "now" button.
*
*	The control is used as part of the toolbar in transit/multitransit widget.
*
******************************************************/
class DateInputCombiCtrl : public wxControl
{
public:

	DateInputCombiCtrl( wxWindow*, const int id = -1 );

	double getJD() const { return jd; }

	bool getIsLocaltime() const;

	void setIsLocaltime( const bool = true );

	void setCurrentDate();

protected:

	void dateControlsToJD();

	void jdToDateControls();

	void OnControl( wxCommandEvent& );

	void OnDateNavigation( wxCommandEvent& );

	void OnNow( wxCommandEvent& ) { setCurrentDate(); }

	void OnWrap( wxCommandEvent& );

	wxButton *addButton( const int& cmdid, const BitmapId &bmpid, const wxString &hint );

	void propagateChangeEvent();

	DateSpin *dateCtrl = nullptr;

	DegreeSpin *timeCtrl = nullptr;

	TimezoneChoice *tzChoice = nullptr;

	double jd = 0;

	double thedate = 0.0;

	double thetime = 0.0;
};


#endif // _SPINCTRL_H_

