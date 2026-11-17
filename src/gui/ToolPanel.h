/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/ToolPanel.h
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

#ifndef _TOOLPANEL_H_
#define _TOOLPANEL_H_

#include <vector>
#include <wx/toolbar.h>

#include "bootstrap.h"
#include "constants.h"

#define TOOLBAR_ID wxID_HIGHEST + 18756

enum class CommandId;

// Toolbar constants
enum class ToolbarCmd
{
	None = 0,
	ButtonNow = wxID_HIGHEST + 10000,
	ButtonContextHoroscope,
	ButtonConfigure,
	ButtonFileClose,
	ButtonFileNew,
	ButtonFileOpen,
	ButtonFileSave,
	ButtonFileSaveAs,
	ButtonGotoYear,
	ButtonLaunchViewer,
	ButtonSelectDasa,
	ButtonFilter,
	ButtonZoomIn,
	ButtonZoomOriginal,
	ButtonZoomOut,
	ChoiceAntardasa,
	ChoiceDasa,
	ChoiceEclipseType,
	ChoiceEphemCoordinateType,
	ChoiceDivision,
	ChoicePartner1,
	ChoicePartner2,
	ChoicePartnerOrigin,
	ChoicePeriodDuration,
	ChoiceSpDisplaymode,
	ChoiceSpObject,
	ChoiceSpTerm,
	ChoiceDirectionMode,
	ChoiceTextMode,
	ChoiceTimezone,
	ChoiceVarga,
	ChoiceYogaGroupFilter,
	ChoiceYogaSourceFilter,
	ChoiceYogaFirstHouse,
	InputDate,
	DateSelectionCombiCtrl,
	SpinMonth,
	SpinYear,
	SpinYearCount,
	ToolbarLabel
};

enum class ToolbarWidgetId
{
	None = -1,
	SpWidget = 0,
	YogaWidget,
	ShadbalaWidget,
	AshtakavargaWidget,
	DirectionWidget,
	MultiDirectionWidget,
	DasaChartWidget,
	SolarWidget,
	EventFinderWidget,
	EclipseWidget,
	EphemWidget,
	HoraWidget,
	PartnerWidget,
	ComprehensiveTextWidget,
	YogaEditorWidget,
};

/*************************************************//**
*
*  encapsulates creation of toolbar
*
******************************************************/
class ToolbarFactory : public Singleton<ToolbarFactory>
{
  friend class Singleton<ToolbarFactory>;

public:

	//void populateToolbar( wxToolBar*, const ViewId& );
	
	wxToolBar *createToolbar( wxWindow *parent, const ToolbarWidgetId& );

	void populateMainToolbar( wxToolBar* );

	int getToolbarStyle();

	std::vector<CommandId> getFullCommandIdList();
	std::vector<CommandId> getDefaultCommandIdList();

private:

	ToolbarFactory();

	void init();

	void addButton( wxToolBar*, const ToolbarCmd& );

	void addTool( wxToolBar*, const CommandId&,  const wxItemKind = wxITEM_NORMAL );
};

#endif


