/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/TextWrapperWidgets.cpp
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

#include "TextWidget.h"

#include "FileConfig.h"
#include "MDocument.h"
#include "WidgetFactory.h"

#include <wx/textfile.h>

/*************************************************//**
*
*  Wrapper for FileConfig::writeFileAndDirectoryReport
*
******************************************************/
class FileAndDirectoryReportWidget : public TextWidget
{
public:

	/*****************************************************
	**
	**   FileAndDirectoryReportWidget   ---   Constructor
	**
	******************************************************/
	FileAndDirectoryReportWidget( wxWindow *parent, ContextProps_ptr props )
		: TextWidget( parent, props, WidgetCategory::TextFileAndDirectoryReport )
	{
		OnDataChanged();
	}

	/*****************************************************
	**
	**   FileAndDirectoryReportWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		debuglog << "FileAndDirectoryReportWidget::OnDataChanged";
		clearDoc();
		FileConfig::get()->writeFileAndDirectoryReport( getDoc());

		TextWidget::OnDataChanged();
	}
};

/*************************************************//**
*
*  Viewer for License file
*
******************************************************/
class LicenseWidget : public TextWidget
{
public:

	/*****************************************************
	**
	**   FileAndDirectoryReportWidget   ---   Constructor
	**
	******************************************************/
	LicenseWidget( wxWindow *parent, ContextProps_ptr props )
		: TextWidget( parent, props, WidgetCategory::TextLicense )
	{
		OnDataChanged();
	}

	/*****************************************************
	**
	**   FileAndDirectoryReportWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		debuglog << "LicenseWidget::OnDataChanged";
		clearDoc();
		MDocument_ptr doc = getDoc();

		wxString filename = FileConfig::get()->getLicenseFilename();

		wxTextFile licensefile( filename );
		if ( licensefile.Open())
		{
			for ( wxString s = licensefile.GetFirstLine(); ! licensefile.Eof(); s = licensefile.GetNextLine() )
			{
				doc->addLine( s );
			}
		}
		else
		{
			errorlog << "ERROR: license file " << filename << " not found";
		}

		TextWidget::OnDataChanged();
	}
};


/**************************************************************
***
**   WidgetFactory   ---   createLicenseWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createLicenseWidget( wxWindow *parent )
{
	return new LicenseWidget( parent, props );
}

/**************************************************************
***
**   WidgetFactory   ---   createFileAndDirectoryReportWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createFileAndDirectoryReportWidget( wxWindow *parent )
{
	return new FileAndDirectoryReportWidget( parent, props );
}


