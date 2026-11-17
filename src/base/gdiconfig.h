/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/gdiconfig.h
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

#ifndef _GDICONFIG_H_
#define _GDICONFIG_H_

#include "AspectConfig.h"
#include "ChartConfig.h"
#include "ColorConfig.h"
#include "ConfigListItems.h"
#include "gdibase.h"
#include "logger.h"
#include "constants.h"
#include "mstring.h"
#include "PrintoutConfig.h"

#include <array>
#include <vector>

#include <wx/string.h>

/*************************************************//**
*
* 
*
******************************************************/
struct ToolbarConfig : public Jsonable
{
	// json:start
	std::vector<CommandId> mainToolbarItems;

	// 1 = text and Icons
	int toolbarStyle = 1;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct ViewPreferencesConfig : public Jsonable
{
	ViewPreferencesConfig();

	wxSize getWidgetSize( const ViewId& );
	void setWidgetSize( const ViewId&, const wxSize& );

	// json:start
	std::map<wxString,wxSize> widgetSize;
	std::map<wxString,wxSize> widgetPosition;
	std::map<wxString,int> activeNotebookTab;
	std::map<wxString,int> splitterSashPos;

	bool doSplitDasaTree = true;
	int dasaTreeTextViewWidth = 300;

	WidgetCategory textWindowPage = WidgetCategory::TextBaseData;

	// show warning dialogs: topic is added to list if display is not desired
	std::set<DoNotShowAgainTopic> doNotShowAgainTopics;

	// Main window options
	int listStyle = 0;
	bool showMainToolbar = true;
	bool showStatusBar = true;
	bool showLogWindow = false;

	// dasa chart
	int dasaChartAntarDasaLevel = 1;
	std::list<DasaId> dasaChartSelectedDasas;

	// YogaWidget
	int yogaSourceFilter = 0;
	int yogaGroupFilter = 0;
	int yogaCustomFilter = 0;

	// directions
	HoroscopeRole directionMode = HoroscopeRole::Transit;
	bool transitIsLocalTime = false;

	// multi directions
	bool multitransitIsLocalTime = false;

	// SpWidget
	int divisionSpWidget = 1;
	int termSpWidget = 0;
	int modeSpWidget = 0;

	// EventFinderWidget, 4 = year
	int periodVWidgetDurationChoice = 4;

	// Ephem widget
	int ephemTimezone = 0;
	int coordinateModeEphem = 0;
	int divisionEphem = 1;

	// Eclipse widget
	int eclipseTimezone = 0;

	// Hora widget
	int horaTimezone = 0;

	// printout conflet
	int printoutConfletSelectedCategory = 0;

	// chart preview Conflet
	bool chartPreviewShowTransits = false;
	int chartPreviewDivision = 1;

	// remember paths
	std::vector<wxString> recentFiles;
	wxString defOpenPath;
	wxString defSavePath;
	wxString defExportPath;
	wxString defPdfSavePath;
	wxString lastYogaConfigFile;
	wxString yogaEditorOpenPath;
	wxString yogaEditorSavePath;
	// json:end

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*   Contains the view params that can be configured in the Miscellaneous panel.
*
*  TODO skin, geom
*
******************************************************/
struct ViewConfig : public Jsonable
{
	ViewConfig()
	{
		#ifdef __WXMSW__
		enableStdoutLogging = false;
		#endif
	}

	// json:start
	bool doHandleResponsiveWidgetEvents = true;
	bool showStatusInfo = true;
	int childViewFrameWidth = 0;
	bool showTextViewHeaders = false;

	bool textWidgetUseGdiNative = false;

	int defOpenFiletype = 0;
	bool exportAskOnOverwrite = true;
	int graphicExportSizeMode = 0;

	bool validatorFeedbackOnError = true;

	int backupFileMode = 1;

	int maxRecentFiles = 10;

	// logging section
	LogLevel uiLogLevel = LogLevel::Info;

	// Raus
	bool enableUiNotification = true;

	int uiNotificationTimeout = 5; // seconds

	LogLevel uiNotificationLogLevel = LogLevel::Warning;

	bool enableStdoutLogging = true;

	LogLevel stdoutLogLevel = LogLevel::Info;

	// TEST
	double jd = 2439387;
	double time = 7.5;
	double longitude = -15.5;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct FontConfig : public Jsonable
{
	// json:start
	wxString standardFontFacename;
	wxString fixedFontFacename;
	int textFontSize = 10;

	wxString graphicFontFacename;
	int graphicFontSize = 12;

	wxString symbolFontFacename = SYMBOL_FONT_NAME;
	int symbolFontSize = 10;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct AtlasConfig  : public Jsonable
{
	AtlasConfig();

	// json:start
	wxString databaseFile;
	wxString sqlFile;
	std::vector<wxString> favouriteCountries;
	int filterMode;
	wxString lastSelectedCountry;
	std::vector<int> gridColumnSizes;
	bool filterCaseSensitive;
	int cacheSize;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct MultipleViewConfig : public Jsonable
{
	MultipleViewConfig();

	/**
	* return a vector of active categories for a given DataContext (disabled categories removed)
	*/
	std::vector<WidgetCategory> extractItems( const DataContext& );

	/**
	* return a vector of active categories from MultipleViewItem list (i.e. with enabled/disabled items)
	*/
	//std::vector<WidgetCategory> extractItems( const std::vector<MultipleViewItem>& );

	// json:start
	bool useMultipleViews = false;

	int defaultView = 0;

	int notebookStyle = 0;

	int notebookOrientation = 0;

	std::vector<std::pair<WidgetCategory,bool>> radixItems;
	std::vector<std::pair<WidgetCategory,bool>> directionItems;
	std::vector<std::pair<WidgetCategory,bool>> multiDirectionItems;
	std::vector<std::pair<WidgetCategory,bool>> solarItems;
	std::vector<std::pair<WidgetCategory,bool>> partnerItems;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct Gdiconfig : public Jsonable
{
	Gdiconfig();

	virtual ~Gdiconfig() {}

	void load();
	void save();

	int getActiveNotebookTab( const wxString &key );
	void setActiveNotebookTab( const wxString &key, const int &value );

	// json:start
	UranianAspectStyle uranianAspectStyle;

	WesternAspectConfig westernAspects;

  std::map<WidgetCategory,uint> skins;
  std::map<WidgetCategory,uint> geoms;

	ColorConfig colors;
	ChartColorConfig chartColors;
	BarDiagramConfig bardiagram;

	int tableStyleId = 0;

	ChartConfig chartWestern;
	ChartConfig chartUranian;
	ChartConfig chartVarga;
	ChartConfig chartSbc;
	ChartConfig chartUniversal;

	ViewPreferencesConfig prefs;
	ViewConfig view;

	AtlasConfig atlas;
	MultipleViewConfig multipleView;

	FontConfig fonts;
	ToolbarConfig toolbar;

	PrintoutConfig printout;
	PageConfig page;
	PdfConfig pdf;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

#endif

