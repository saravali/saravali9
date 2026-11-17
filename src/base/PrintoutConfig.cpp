/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/PrintoutConfig.cpp
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

#include "PrintoutConfig.h"
#include "FileConfig.h"

constexpr int defaultMargin = 36; // pt

/*****************************************************
**
**   PageConfig   ---   Constructor
**
******************************************************/
PageConfig::PageConfig()
{
	paperSize = PaperSize::A4;
	horizontalMargin = defaultMargin;
	verticalMargin = defaultMargin;

	header.enabled = true;
	header.text = "$name/$fulldate/$location";
	header.showSeparator = true;
	header.orientation = 0;
	header.showOnFirstPage = false;

	footer.enabled = true;
	footer.text = tr( "Saravali $version/$creationdate page $page/$pagecount" );
	footer.showSeparator = true;
	footer.orientation = 0;
	footer.showOnFirstPage = true;
}

/*****************************************************
**
**   PdfConfig   ---   Constructor
**
******************************************************/
PdfConfig::PdfConfig()
{
	pdfViewerCommand = FileConfig::get()->getPdfViewerDefaultFilename();
}

/*****************************************************
**
**   PrintoutConfig   ---   Constructor
**
******************************************************/
PrintoutConfig::PrintoutConfig()
{
	for( const WidgetCategory &cat : Enum<WidgetCategory>( WidgetCategory::WesternChart, WidgetCategory::WesternChart ))
	{
		geoms[cat] = 0;
		skins[cat] = 0;
	}
}

