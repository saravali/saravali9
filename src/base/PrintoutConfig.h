/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/PrintoutConfig.h
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
#ifndef _PRINTOUTCONFIG_H_
#define _PRINTOUTCONFIG_H_

#include "bootstrap.h"
#include "constants.h"

#include <map>

#include <wx/defs.h>
#include <wx/string.h>

/*************************************************//**
*
*
*
******************************************************/
enum class PageBreakMode
{
  None = -1,
  OnePage, // no break = normal GDI widget
  Float, // breaking but y coordinates continue = printout preview showing pages
  NewCoordinates // break and start page with new coordinates = PDF printout
};

enum class PaperSize
{
	// json:enum
	None = -1,
	A4 = 0,
	Legal,
	Letter,

	First = A4,
	Last = Letter
};

/*************************************************//**
*
* This class holds the configuration for page header and footer.
*
******************************************************/
struct PageMarginal : public Jsonable
{
	PageMarginal()
	{
		enabled = false;
		showSeparator = false;
		orientation = 0;
		showOnFirstPage = false;
	}

	// json:start
	bool enabled;
  wxString text;
	bool showSeparator;
	int orientation;
	bool showOnFirstPage;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct PageConfig : public Jsonable
{
	PageConfig();

	// json:start
	PaperSize paperSize;
	int horizontalMargin;
	int verticalMargin;

	int basicFontSize = 10;

	// header and footer
	PageMarginal header;
	PageMarginal footer;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct PdfConfig : public Jsonable
{
	PdfConfig();

	// json:start
	bool askOverwrite = true;
	bool compressPdf = true;
	bool launchPdfViewer = true;
	wxString pdfViewerCommand;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* This class contains the printout configuration.
*
******************************************************/
struct PrintoutConfig : public Jsonable
{
	PrintoutConfig();

	// json:start
	int sheetId = 0;
	std::map<WidgetCategory,uint> skins;
  std::map<WidgetCategory,uint> geoms;

	int tableStyleId = 0;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

#endif

