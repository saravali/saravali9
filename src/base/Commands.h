/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Commands.h
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


#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "bootstrap.h"
#include "constants.h"
#include "Planet.h"

#include <wx/accel.h>
#include <wx/string.h>

#include <map>

/**************************************************************
***
**  Command and view IDs
***
****************************************************************/
enum class CommandId
{
	// json:enum
	None = -1,

	// These values are stored in system menu on macOS
	AppAbout = wxID_ABOUT,
	AppExit = wxID_EXIT,
	AppConfigure = wxID_PREFERENCES,

	AppFirst = 10000,

	AppNew,
	AppNewSingle,
	AppNewMultipleChoice,
	AppNewMultiple,
	AppNewMultipleLast = AppNewMultiple + 100,

	AppOpen,
	AppFileRecent,
	AppFileRecentLast = AppFileRecent + 100,

	AppConfigGeneral,
	AppConfigCalculation,
	AppConfigGui,
	AppConfigPrintout,
	AppAtlasdialog,
	AppHelp,
	AppYearlyHoroscope,

	AppHora,
	AppEphemeris,
	AppEclipse,
	AppPartner,
	AppYogaEditor,
	AppFileAndDirectoryReport,
	AppLicense,

	AppListLargeIcons,
	AppListSmallIcons,
	AppListTable,
	AppToggleStatusbar,
	AppToggleMainToolbar,
	AppToggleLogWindow,
	AppSaveConfig,
	AppLast = AppSaveConfig,

	ViewpointVedic,
	ViewpointWestern,
	ViewpointUranian,
	ViewpointAstronomical,

	// toggle planet items
	ObToggleFirstPlanet,
	ObToggleLastPlanet = ObToggleFirstPlanet + (int)PlanetId::ArabicParts + 1,

	// status flags for main window: handled by MainView -> TextViewBase 
	ColToggleDeclination,
	ColToggleLatitude,
	ColToggleSpeed,
	ColToggleT4Longitude,
	ColToggleT8Longitude,
	ColToggleT16Longitude,
	ColToggleAntiscia,
	ColToggleDiginity,
	ColToggleNavamsa,
	ColToggleLord,
	ColToggleNakshatra,
	ColTogglePada,
	ColToggleKp,
	ColToggleShastiamsa,
	ColToggleDasavarga,
	ColToggleKaraka,
	ColToggleAv,
	ColToggleHousePos,
	ColToggleElement,
	ColToggleSignification,
	ColToggleSignQuality,
	ColToggleWesternAspectarium,

	// generic toolbar commands
	NewEntryChart,
	NewContextHoroscope,
	ToggleSplit,
	Now,
	SelectDasa,
	GotoYear,
	Filter,
	ClearFilter,
	LaunchViewer,
	Tip,

	NextGeometry,
	PreviousGeometry,
	NextSkin,
	PreviousSkin,

	ZoomIn,
	ZoomOriginal,
	ZoomOut,
	ZoomChartIn,
	ZoomChartOriginal,
	ZoomChartOut,

	// handle by text widget
	Copy,
	SelectAll,

	ExportPlainText,
	ExportCsv,
	ExportPlainHtml,
	ExportHtml,
	ExportTextClipboard,

	ExportImageJpeg,
	ExportImagePng,
	ExportImageSvg,
	ExportImageClipboard,

	 // key wrapper
	KeyArrowUp,
	KeyArrowDown,
	KeyF1,
	KeyLeft,
	KeyRight,
	KeyPageDown,
	KeyPageUp,

	// delegated to application object
	Close,
	CloseDoc,
	EditData,
	Animate,
	Save,
	SaveAs,
	Print,
	QuickPrint,

	// creation of new child views -> delegate to application object
	NewText,
	NewDirection,
	NewSolar,
	NewYoga,
	NewDasaTree,
	NewDasaTreeComposite,
	NewDasaChart,
	NewMultiDirection,
	NewAshtakavarga,
	NewPrintPreview,
	
	NewUniversalRadix,
	NewUranian,
	NewSensitivePoints,
	NewPp,
	NewEventFinder,
	NewShadbala,
	NewMainView,
	NewLogWindow,
	NewEmpty,

	NewUniversalChart,
	NewWesternChart,
	NewVarga,
	NewSbc,
	NewUranianChart,

	NewD1Chart,
	NewD9Chart,
	NewD2Chart,
	NewD3Chart,
	NewD4Chart,
	NewD6Chart,
	NewD7Chart,
	NewD8Chart,
	NewD10Chart,
	NewD12Chart,
	NewD16Chart,
	NewD20Chart,
	NewD24Chart,
	NewD27Chart,
	NewD30Chart,
	NewD40Chart,
	NewD45Chart,
	NewD60Chart,
	NewD108Chart,
	NewD144Chart,
	NewBhavaChart,

	FirstNewVargaChart = NewD1Chart,
	LastNewVargaChart = NewBhavaChart,

	ChartGeom,
	ChartGeomLast = ChartGeom + 100,

	ChartSkin,
	ChartSkinLast = ChartSkin + 100,

	ChildFirst = ViewpointVedic,
	ChildLast = ChartSkinLast,

	First = AppFirst,
	Last = ChildLast
};


/**************************************************************
***
**  Bitmap IDs
***
***************************************************************/
enum class BitmapId
{
	None = -1,
	Application = 0,

	// global commands
	Close,
	New,
	Open,
	Quit,
	Save,
	Saveas,

	// view without document
	Eclipse,
	Ephem,
	Hora,
	Yogaeditor,

	// view with document
	Ashtakavarga,
	Dasa,
	DasaChart,
	Mview,
	Partner,
	Sbc,
	Shadbala,
	Solar,
	Sview,
	Text,
	Direction,
	MultiDirection,
	Uranian,
	UranianChart,
	SensitivePoints,
	EventFinder,
	PlanetaryPictures,
	Varga,
	WesternChart,
	Yoga,

	// vedic charts
	Chart1,
	Chart9,
	Chart2,
	Chart3,
	Chart4,
	Chart6,
	Chart7,
	Chart8,
	Chart10,
	Chart12,
	Chart16,
	Chart20,
	Chart24,
	Chart27,
	Chart30,
	Chart40,
	Chart45,
	Chart60,
	Chart108,
	Chart144,
	BhavaChart,

	Print,
	Error,

	// misc modal dialogs
	About,
	Atlas,
	Config,

	// commands on document dependend views
	Edit,
	Quickprint,

	// dasa tree view
	Folder,
	FolderExpanded,
	Dasatree,
	DasatreeExpanded,

	// general toolbar command
	ArrowDown,
	ArrowLeft,
	ArrowRight,
	ArrowUp,
	Copy,
	ExportCsv,
	ExportHtml,
	ExportImage,
	ExportText,
	Filter,
	Delete,
	Goto,
	Help,
	Now,
	Split,
	Tip,
	Wizard,
	ZoomIn,
	ZoomOriginal,
	ZoomOut, 

	// Navigation
	Backward3,
	Backward2,
	Backward1,
	Forward1,
	Forward2,
	Forward3,

	Separator,

	First = Application,
	Last = Separator,

	ViewImagelistFirst = Eclipse,
	ViewImagelistLast = Error
};

/*************************************************//**
*
*  encapsulation of a command, used for construction of menu and toolbar items
*
******************************************************/
struct MCommand
{
	MCommand( const wxString &text, const wxString &shortText = "" )
			: bmpId( BitmapId::None ),
			text( text ),
			shortText( shortText ) {}

	MCommand( const BitmapId &bmpId, const wxString &text, const wxString &shortText = "" )
			: bmpId( bmpId ),
			text( text ),
			shortText( shortText ) {}

	wxString getShortText() const;

	const BitmapId bmpId = BitmapId::None;

	const wxString text;

	const wxString shortText;
};

enum class AcceleratorTableId
{
	// json:enum
	None = -1,
	All,
	Minimal,
};

/*************************************************//**
*
*  holds the list of commands and handles lookups for key events
*
******************************************************/
class CommandHandler : public Singleton<CommandHandler>
{
	friend class Singleton<CommandHandler>;

public:

	MCommand *getCommand( const CommandId& );

	const std::map<CommandId, MCommand*> &getCommands() const { return cmds; }

	wxAcceleratorEntry *getAcceleratorEntry( const CommandId& );

	wxAcceleratorTable getAcceleratorTable( const AcceleratorTableId = AcceleratorTableId::All ) const;

	const std::list<wxAcceleratorEntry> &getAccelerators() const { return accelerators; }

	BitmapId getBitmapId( const CommandId& ) const;

protected:

	void init();

	void initAccelerators();

	std::map<CommandId, MCommand*> cmds;

	std::list<wxAcceleratorEntry> accelerators;

private:

	CommandHandler();
};

#endif

