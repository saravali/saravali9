/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Formulary.h
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

#ifndef _FORMULARY_H_
#define _FORMULARY_H_

#include "Term.h"

#include <map>
#include <vector>

class MTerm;
struct UranianSection;

/*****************************************************
**
**   Repository for formulas in Uranian astrology.
**
******************************************************/
class Formulary : public Singleton<Formulary>
{
  friend class Singleton<Formulary>;

public:

	/**
	* Returns pairs of TermIds for all formulae and antiscia (if flag is true)
	*/
	std::vector<TermIdPair> getFormDefPairs( const std::list<int> &formulae, const bool includeAntiscia  = false,
		const bool includeLrPermutations = false );

	wxString getFormulaName( const int&, const TextFormat = TextFormat::Long ) const;

	std::vector<wxString> getFormulaNames( const UranianContext&, const TextFormat = TextFormat::Long ) const;

	/**
	* Name of a single term like "Midpoint"
	*/
	wxString getTermName( const TermId&, const TextFormat = TextFormat::Long ) const;

	/**
	*  Returns a list of available MTerm Ids (with or without antiterms).
	*/
	std::list<TermId> getTermIds( const bool includeAntiTerms ) const;

	/**
	*  Returns the mirror (antiscium) term.
	*/
	TermId getAntiTerm( const TermId& ) const;

protected:

	const std::vector<TermIdPair> defs;

	const std::map<TermId, wxString> termnames;

	Formulary();
};

#endif // _FORMULARY_H_



