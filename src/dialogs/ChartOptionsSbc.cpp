/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ChartOptionsSbc.cpp
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

#include "ChartOptionsSbc.h"

#include "ChartConfig.h"
#include "logger.h"

#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/valgen.h>

// begin wxGlade: ::extracode
// end wxGlade

enum { CP_BENEFIC = wxID_HIGHEST + 3000, CP_DEVANAGARI };

/*****************************************************
**
**   ChartOptionsSbc   ---   Constructor
**
******************************************************/
ChartOptionsSbc::ChartOptionsSbc( wxWindow *parent, ContextProps_ptr props, ChartConfig *cfg )
	: ChartOptionsPanel( parent, props, cfg )
{
	assert( cfg );

	// begin wxGlade: ChartOptionsSbc::ChartOptionsSbc
	wxBoxSizer* sizer_chart = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer* sizer_options = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("SBC Options")), wxVERTICAL);
	sizer_chart->Add(sizer_options, 0, wxEXPAND, 0);
	check_devanagari_chars = new wxCheckBox(sizer_options->GetStaticBox(), CP_DEVANAGARI, _("Show Devanagari Characters"));
	sizer_options->Add(check_devanagari_chars, 0, wxALL|wxEXPAND, 3);
	check_nakshatra_qualities = new wxCheckBox(sizer_options->GetStaticBox(), CP_BENEFIC, _("Benefic/Malefic (B/M) in Nakshatras Names"));
	sizer_options->Add(check_nakshatra_qualities, 0, wxALL|wxEXPAND, 3);
	
	SetSizer(sizer_chart);
	sizer_chart->Fit(this);
	// end wxGlade

	debuglog << "Transfer DEVA " << CP_DEVANAGARI;

	check_devanagari_chars->SetValidator( wxGenericValidator( &cfg->showSbcDevanagariCharacters ));
	check_nakshatra_qualities->SetValidator( wxGenericValidator( &cfg->showSbcNakshatraQuality ));
}

