/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/WidgetFactory.h
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

#ifndef _WIDGETFACTORY_H_
#define _WIDGETFACTORY_H_

#include "bootstrap.h"
#include "logger.h"
#include "constants.h"

class BasicWidget;
enum class ToolbarWidgetId;

class wxWindow;

/*************************************************//**
*
* 
*
******************************************************/
class WidgetFactory
{
public:

	WidgetFactory( ContextProps_ptr props ) : props( props ) {}

	BasicWidget *createWidget( wxWindow*, const WidgetCategory&, const Varga = Varga::None );

	BasicWidget *createNotebookWidget( wxWindow*, const WidgetCategory&,
		const std::vector<WidgetCategory> &elements, const ToolbarWidgetId& );

protected:

	BasicWidget *createSingleChart( wxWindow*, const WidgetCategory&, const Varga& );

	BasicWidget *createRasiNavamsaGrid( wxWindow* );

	BasicWidget *createVargaGrid( wxWindow*, const WidgetCategory& );

	BasicWidget *createDasaTreeWidget( wxWindow*, const WidgetCategory& );

	BasicWidget *createUranianChartWidget( wxWindow* );

	BasicWidget *createComprehensiveTextWidget( wxWindow* );

	BasicWidget *createGenericTextWidget( wxWindow*, const WidgetCategory& );

	BasicWidget *createVimsopakabalaWidget( wxWindow* );

	BasicWidget *createMatchesWidget( wxWindow* );

	BasicWidget *createSpWidget( wxWindow* );

	BasicWidget *createPpWidget( wxWindow* );

	BasicWidget *createPartnerWidget( wxWindow* );

	BasicWidget *createEventFinderWidget( wxWindow* );

	BasicWidget *createDirectionWidget( wxWindow* );

	BasicWidget *createMultiDirectionWidget( wxWindow* );

	BasicWidget *createSolarWidget( wxWindow* );

	BasicWidget *createShadbalaWidget( wxWindow* );

	BasicWidget *createAshtakavargaWidget( wxWindow* );

	BasicWidget *createDasaChartWidget( wxWindow* );

	BasicWidget *createYogaWidget( wxWindow* );

	BasicWidget *createAshtakootaWidget( wxWindow* );

	BasicWidget *createCombiWidget( wxWindow*, const WidgetCategory& );

	BasicWidget *createMixedWidgetGrid( wxWindow*, const WidgetCategory&, const std::list<WidgetCategory> &subCategories );

	BasicWidget *createYogaEditorWidget( wxWindow* );

	BasicWidget *createEclipseWidget( wxWindow* );

	BasicWidget *createHoraWidget( wxWindow* );

	BasicWidget *createEphemWidget( wxWindow* );

	BasicWidget *createFileAndDirectoryReportWidget( wxWindow* );

	BasicWidget *createLicenseWidget( wxWindow* );

	BasicWidget *createEmptyWidget( wxWindow*, const wxString message = "", const LogLevel = LogLevel::Info );

	ContextProps_ptr props;
};


#endif // _WIDGETFACTORY_H_

