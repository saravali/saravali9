/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/PdfGenerator.h
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

#ifndef _PDFGENERATOR_H_
#define _PDFGENERATOR_H_


#include "constants.h"

class DocItemChart;
class DocItemText;
class DocItemVargaChartGrid;
struct PageConfig;
class Painter;
struct PrintoutConfig;
struct SheetSpec;
class Table;
enum class TextItemSubType;
class wxPdfDocument;

struct PageCoordinates
{
	double xMax = 0.0;

	double yMax = 0.0;
	
	double xMargin = 0.0;

	double yMargin = 0.0;
};

/*************************************************//**
*
* 
*
******************************************************/
class PdfGenerator
{
public:

	PdfGenerator( Horoscope_ptr, const PrintoutConfig&, const PageConfig& );
	
	~PdfGenerator();
	
	/**
	* Generating PDF has several steps:
	* - Generate MDocument from Sheet spec (done by PrintoutTextHelper)
	* - Format items, calculate their width and height
	* - Position items including page breaks
	* - print items to PDF
	*
	*  return: true on success, false on file save error
	*/
	bool generatePdf( const wxString &filename );

private:

	//PageCoordinates getPageCoordinates( const uint &pageno );

	PageCoordinates getPageCoordinates( const PageOrientation = PageOrientation::Portrait );

	void formatItems( Painter* );

	void formatTable( Painter*, Table*, const PageOrientation& );

	void layoutDocument();

	void layoutPage( const PageOrientation&, const int &currentPage );

	void printItems( Painter* );

	void printVargaChartGrid( Painter*, DocItemVargaChartGrid* );

	void printChart( Painter*, DocItemChart* );

	void printTable( Painter*, Table* );
	
	void newPage( Painter*, const PageOrientation &orientation, const uint &pageno );

	void printHeaderFooter( Painter*, const bool &isHeader, const PageOrientation&, const uint &pageno );

	double getChartXyRatio( const WidgetCategory& );

	double getTextItemYScalingFactor( const TextItemSubType& );

	void setFont( Painter*, const TextItemSubType& );

	void printPageGrid( Painter*, const double &stepWidth, const PageOrientation& );

	void printCentimeterGrid( Painter*, const PageOrientation& );

	MDocument_ptr doc;

	Horoscope_ptr horoscope;

	ContextProps_ptr props;

	/**
	* Parameter given in contructor
	*/
	const PrintoutConfig& printoutCfg;

	/**
	* Parameter given in contructor
	*/
	const PageConfig& pageCfg;

	/**
	* Contents description for printout. Is calculated in constructor.
	*/
	const SheetSpec &sheetSpec;

	/**
	* Original page coordinates in Portrait mode. No margins.
	*/
	MComplex originalPageCoordinates;

	/**
	* Total number of pages.
	*/
	uint pagecount = 0;

	wxPdfDocument *pdfdoc = nullptr;

};

#endif // _PDFGENERATOR_H_

