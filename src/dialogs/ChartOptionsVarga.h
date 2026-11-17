/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ChartOptionsVarga.h
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

#ifndef CHARTOPTIONSVARGA_H_
#define CHARTOPTIONSVARGA_H_

#include "ChartConflet.h"

struct ChartConfig;
class wxCheckBox;
class wxChoice;
class wxStaticText;

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class ChartOptionsVarga : public ChartOptionsPanel
{
public:
	// begin wxGlade: ChartOptionsVarga::ids
	// end wxGlade

	ChartOptionsVarga( wxWindow* parent, ContextProps_ptr, ChartConfig* );

	virtual void updateUi() override;

private:

protected:

	// begin wxGlade: ChartOptionsVarga::attributes
	wxCheckBox* check_arudhas;
	wxCheckBox* check_ashtakavarga;
	wxStaticText* label_north_style;
	wxChoice* choice_north_chart;
	wxStaticText* label_markup_ascendant;
	wxChoice* choice_markup_ascendant;
	wxStaticText* label_chartcenter;
	wxChoice* choice_chart_center;
	wxStaticText* label_aspect_type;
	wxChoice* choice_aspect_type;
	wxStaticText* label_minimum_strength;
	wxChoice* choice_minimum_strength;
	wxStaticText* label_benefic;
	wxChoice* choice_benefic;
	wxCheckBox* check_include_asmc;
	// end wxGlade
}; // wxGlade: end class


#endif // CHARTOPTIONSVARGA_H_
