/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/PdfLauncher.h
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

#ifndef _PDFLAUNCHER_H_
#define _PDFLAUNCHER_H_

#include "bootstrap.h"
#include "PrintoutConfig.h"

/*************************************************//**
*
*  High level wrapper for printing. The class calls the corresponding logice in the gdi
*  layer an is able to launch the PDF viewer.
*
******************************************************/
class PdfLauncher
{
public: 

	PdfLauncher();

	PdfLauncher( const PrintoutConfig& );

	void printHoroscope( Horoscope_ptr, const bool doSelect = false );

	void printTestpage( Horoscope_ptr, ContextProps_ptr props );

	void printTestpage();

protected:

	wxString getTestpageFilename() const;

  PrintoutConfig printoutCfg;

	bool finishPrint( wxString filename );

	bool launchViewer( wxString filename );

	void printHoroscope( Horoscope_ptr, wxString filename );

	bool selectPrintoutId( int &type );

	/**
	* param filename contains the selected file
	*
	* return true if a file was selected.
	*/
	bool selectFilename( wxString &filename, const wxString &basename );
};


#endif


