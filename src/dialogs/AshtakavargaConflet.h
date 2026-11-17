/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/AshtakavargaConflet.h
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

#ifndef ASHTAKAVARGACONFLET_H
#define ASHTAKAVARGACONFLET_H

#include "Conflet.h"
#include "astroconfig.h"

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
class AshtakavargaConflet : public ConfletIncarnation<AshtakavargaConfig>
{
public:
	// begin wxGlade: AshtakavargaConflet::ids
	// end wxGlade

	AshtakavargaConflet( wxWindow* parent );

private:

protected:
	// begin wxGlade: AshtakavargaConflet::attributes
	wxStaticText* label_av_calcrules;
	wxChoice* choice_av_calcrules;
	wxStaticText* label_av_benefics;
	wxChoice* choice_av_benefics;
	wxStaticText* label_av_sarva;
	wxChoice* choice_av_sarva;
	wxStaticText* label_av_sodhya;
	wxChoice* choice_av_sodhya;
	// end wxGlade
}; // wxGlade: end class


#endif // ASHTAKAVARGACONFLET_H
