/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/CommonControls.h
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
#ifndef _COMMONCONTROLS_H_
#define _COMMONCONTROLS_H_

#include "constants.h"
#include "gdibase.h"
#include "mstring.h"

#include <vector>
#include <wx/bmpbuttn.h>
#include <wx/choice.h>
#include <wx/log.h>
#include <wx/panel.h>
#include <wx/sizer.h>

enum class BitmapId;
enum class CommandId;
enum class Viewpoint;
struct YearLengthConfig;

class Painter;
class wxButton;
class wxStaticText;
class wxTextCtrl;

/*************************************************//**
*
* Choice for year length
*
* Used by: DasaConflet, TransitConflet
*
******************************************************/
class YearLengthPanel : public wxPanel
{
public:
	YearLengthPanel( wxWindow *parent, const int &id, YearLengthConfig& );

private:

	void OnChoice( wxCommandEvent& );

	void updateUi();

	YearLengthConfig &cfg;

	wxChoice* choice_type;

	wxStaticText *label_custom_value;

	wxTextCtrl* text_custom_value;
};

/*************************************************//**
*
* Choice with direction modes (Transit/Progressive/SolarArc etc)
*
* Initiated by: Toolbar
*
* Used by: DiretionView
*
******************************************************/
class DirectionModeChoice : public wxChoice
{
public:

	DirectionModeChoice( wxWindow *parent, const int &id );

	HoroscopeRole getDirectionMode() const;

	void setDirectionMode( const HoroscopeRole& );

private:
	inline static int basic_offset = (int)HoroscopeRole::Transit;	
};

/*************************************************//**
* 
* Choice box for text view modes.
*
* Initiated by: Toolbar
*
* Used by: TextView
*
******************************************************/
class TextViewModeChoice : public wxChoice
{
public:
	TextViewModeChoice( wxWindow *parent, const int &id, const int selected = 0 );

	void setSelectedCategory( const WidgetCategory& );

	WidgetCategory getSelectedCategory();

	WidgetCategory getCategoryForSelection( const int &selection );

	void recreateModes( const Viewpoint& );

private:
	std::vector<WidgetCategory> cats;
};

/*************************************************//**
*
* Choice with timezone optins
*
* Initiated by: Toolbar
*
* Used by: HoraView, TransitView, EphemView, EclipseView
*
******************************************************/
class TimezoneChoice : public wxChoice
{
public:

	TimezoneChoice( wxWindow *parent, const int &id );
};

/*************************************************//**
*
* Choice box for background image files
*
* Used by: PenDialog/BrushDialog
*
******************************************************/
class BackgroundImageChoice : public wxChoice
{
public:
	BackgroundImageChoice( wxWindow *parent, const int &id );
};

/*************************************************//**
*
* Simple text label for toolbar
*
* Used by: DasaChart
*
******************************************************/
class ToolbarLabelCtrl : public wxControl
{
public:

	ToolbarLabelCtrl( wxWindow *parent, wxWindowID id, wxPoint pos, wxSize size, wxString tooltip = wxEmptyString );

	void SetLabel( const wxString& );

private:
	wxStaticText *theText;
};

/*************************************************//**
*
* Context help button
*
******************************************************/
class HelpButton : public wxBitmapButton
{
public:
	HelpButton( wxWindow *parent, const wxString title = "", const wxString contents = "" );

	wxString title;

	wxString contents;

private:

	void OnButton( wxCommandEvent& );
};

/*************************************************//**
*
*  Simple preview for text items (MString based).
*
*  Backgound and text color can be set separately.
*
******************************************************/
class StringPreviewPanel : public wxPanel
{
public:

	wxColour textColor;

	MBrush brush;

protected:

	StringPreviewPanel( wxWindow* );

	virtual MString getStringContents() = 0;

	void OnPaint( wxPaintEvent& );
};

#endif // _COMMONCONTROLS_H_

