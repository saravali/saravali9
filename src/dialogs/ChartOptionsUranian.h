/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ChartOptionsUranian.h
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

#ifndef CHARTOPTIONSURANIAN_H_
#define CHARTOPTIONSURANIAN_H_

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
class ChartOptionsUranian : public ChartOptionsPanel
{
public:
	// begin wxGlade: ChartOptionsUranian::ids
	// end wxGlade

	ChartOptionsUranian( wxWindow* parent, ContextProps_ptr, ChartConfig* );

private:

protected:
	// begin wxGlade: ChartOptionsUranian::attributes
	wxCheckBox* check_coloredplanets;
	wxCheckBox* check_use_sign_colors;
	wxStaticText* label_chartcenter;
	wxChoice* choice_chart_center;
	wxCheckBox* check_showmatches;
	wxCheckBox* check_show_eha;
	// end wxGlade
}; // wxGlade: end class


#endif // CHARTOPTIONSURANIAN_H_
