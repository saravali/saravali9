/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/DasaTool.h
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

#ifndef _DASATOOL_H_
#define _DASATOOL_H_

#include "bootstrap.h"
#include "Dasa.h"
#include "constants.h"

#include <map>
#include <set>

class DasaExpert;
struct DasaConfig;
enum class PlanetId;

/*************************************************//**
*
* 
*
******************************************************/
class DasaTool : public Singleton<DasaTool>
{
  friend class Singleton<DasaTool>;

public:

	wxString getDasaName( const DasaId& );

	wxString getDasaLevelName( const DasaLevel& );
	void calculateMahaAntarLords( Dasa_ptr, DasaLord &lord, DasaLord &sublord );
	void writeDasaEffect( MDocument_ptr, Dasa_ptr, const wxString sourcefilter = ""  );

	wxString getDasaLordChain( Dasa_ptr, const bool addStartAndEndDate = false );

	void writeDasaDocumentation( MDocument_ptr, Horoscope_ptr, Dasa_ptr, const wxString sourcefilter  = "" );

	bool dasaConfigMatchesDasa( const DasaConfig &cfg, Dasa *dasa ) const;

	void writeReport( MDocument_ptr, const DasaReportType&, const DasaId&,  ContextProps_ptr, const bool show_header = false,
		const TextFormat maxTextFormat = TextFormat::Long );

	void writeShortReport( MDocument_ptr, const DasaId&,  ContextProps_ptr, const bool show_header = false,
		const TextFormat maxTextFormat = TextFormat::Long );
	void writeCompactReport( MDocument_ptr, const DasaId&, ContextProps_ptr, const bool show_header = false,
		const TextFormat maxTextFormat = TextFormat::Long );
	void writeComprehensiveReport( MDocument_ptr, const DasaId&, ContextProps_ptr, const bool show_header = false,
		const TextFormat maxTextFormat = TextFormat::Long );

	std::pair<double, Nakshatra27> getNakshatraPortion( Horoscope_ptr, const PlanetId&, const Viewpoint& );

	double getPadaPortion( Horoscope_ptr, const PlanetId&, const Viewpoint& );

	/**
	*  Duration between birth and end of first Mahadasa (in days).
	*/
	double calculateDasaInfluenceTime( ContextProps_ptr, const DasaId& );

	/**
	*  Result of method above formatted to y/m/d
	*/
	wxString getDasaInfluenceTimeAsString( ContextProps_ptr, const DasaId& );

private:
	DasaTool() {}

};

#endif

