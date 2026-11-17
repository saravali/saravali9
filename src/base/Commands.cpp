/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Commands.cpp
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

#include "Commands.h"
#include "enumserializer.h"
#include "Lang.h"
#include "logger.h"

#include <format>

#define DEBUG_COMMANDHANDLER

// workaround: accelerator tabe must be array and msvc compiler cannot allocate that dynamically
constexpr uint ACCELERATORSIZE = 78;

/*****************************************************
**
**   MCommand   ---   getShortText
**
******************************************************/
wxString MCommand::getShortText() const
{
	if ( ! shortText.empty() ) return shortText;
	if ( ! text.empty() ) return text;
	return "ERROR";
}

/*****************************************************
**
**   CommandHandler   ---   Constructor
**
******************************************************/
CommandHandler::CommandHandler()
{
	init();
	initAccelerators();
}

/*****************************************************
**
**   CommandHandler   ---   getCommand
**
******************************************************/
MCommand *CommandHandler::getCommand( const CommandId &id )
{
#ifdef DEBUG_COMMANDHANDLER
	debuglog << "CommandHandler::getCommand id " << id;
#endif

	assert_valid_enum_or_none<CommandId>( id );
	auto a = cmds.find( id );
	assert ( a != cmds.end() );
	return a->second;
}

/*****************************************************
**
**   CommandHandler   ---   getBitmapId
**
******************************************************/
BitmapId CommandHandler::getBitmapId( const CommandId& ) const
{
	return BitmapId::None;
}

/*****************************************************
**
**   CommandHandler   ---   getAcceleratorEntry
**
******************************************************/
wxAcceleratorEntry *CommandHandler::getAcceleratorEntry( const CommandId &id ) 
{
	for( auto &a : accelerators )
	{
		if ( a.GetCommand() == (int)id )
		{
			return &a;
		}
	}
	return nullptr;
}

/*****************************************************
**
**   CommandHandler   ---   getAcceleratorTable
**
******************************************************/
wxAcceleratorTable CommandHandler::getAcceleratorTable( const AcceleratorTableId id ) const
{

	switch( id )
	{
		case AcceleratorTableId::Minimal:
		{
			wxAcceleratorEntry entries[ 1 ];
			entries[0].Set( wxACCEL_NORMAL, WXK_ESCAPE, (int)CommandId::Close );
		}
		break;
		default:
		{
			wxAcceleratorEntry entries[ ACCELERATORSIZE ];
			for( uint i = 0; const auto &a : accelerators )
			{
				entries[i].Set( a.GetFlags(), a.GetKeyCode(), a.GetCommand() );
				i++;
			}
			return wxAcceleratorTable( ACCELERATORSIZE, entries );
		}
	}
	return wxNullAcceleratorTable;
}

/*****************************************************
**
**   CommandHandler   ---   initAccelerators
**
******************************************************/
void CommandHandler::initAccelerators()
{
	accelerators = 
	{
		{ wxACCEL_CTRL, (int)'N', (int)CommandId::AppNew },
		{ wxACCEL_SHIFT | wxACCEL_CTRL, (int)'N', (int)CommandId::AppNewSingle },
		{ wxACCEL_CTRL, (int)'O', (int)CommandId::AppOpen },
		{ wxACCEL_CTRL, (int)'Q', (int)CommandId::AppExit },

		{ wxACCEL_CTRL, (int)'h', (int)CommandId::AppHora },
		{ wxACCEL_CTRL, (int)'e', (int)CommandId::AppEphemeris },
		{ wxACCEL_SHIFT | wxACCEL_CTRL, (int)'e', (int)CommandId::AppEclipse },
		{ wxACCEL_CTRL, (int)'r', (int)CommandId::AppPartner },
		{ wxACCEL_SHIFT | wxACCEL_CTRL, (int)'y', (int)CommandId::AppYogaEditor },
		{ wxACCEL_CTRL, (int)'j', (int)CommandId::AppYearlyHoroscope },
		{ wxACCEL_CTRL, (int)'l', (int)CommandId::AppAtlasdialog },
		{ wxACCEL_NORMAL, WXK_F1, (int)CommandId::AppHelp },
		{ wxACCEL_CTRL, WXK_F1, (int)CommandId::AppAbout },

#ifdef __WXOSX__ 
		{ wxACCEL_CTRL, (int)',', (int)CommandId::AppConfigure },
#else
		{ wxACCEL_CTRL, (int)'c', (int)CommandId::AppConfigure },
#endif

		{ wxACCEL_NORMAL, WXK_ESCAPE, (int)CommandId::Close },
		{ wxACCEL_CTRL, (int)'W', (int)CommandId::CloseDoc },

		{ wxACCEL_CTRL, (int)'S', (int)CommandId::Save },
		{ wxACCEL_SHIFT | wxACCEL_CTRL, (int)'S', (int)CommandId::SaveAs },
		{ wxACCEL_CTRL, (int)'P', (int)CommandId::Print },
		{ wxACCEL_SHIFT | wxACCEL_CTRL, (int)'P', (int)CommandId::QuickPrint },

		// Universal
		{ wxACCEL_CTRL, WXK_F2, (int)CommandId::NewUniversalRadix },
		{ wxACCEL_CTRL, (int)'b', (int)CommandId::EditData },
		{ wxACCEL_SHIFT | wxACCEL_CTRL, (int)'i', (int)CommandId::Animate },

		{ wxACCEL_CTRL, (int)'1', (int)CommandId::ViewpointVedic },
		{ wxACCEL_CTRL, (int)'2', (int)CommandId::ViewpointWestern },
		{ wxACCEL_CTRL, (int)'3', (int)CommandId::ViewpointUranian },
		{ wxACCEL_CTRL, (int)'4', (int)CommandId::ViewpointAstronomical },

		{ wxACCEL_CTRL, WXK_DOWN, (int)CommandId::NextGeometry },
		{ wxACCEL_CTRL, WXK_UP, (int)CommandId::PreviousGeometry },
		{ wxACCEL_SHIFT, WXK_DOWN, (int)CommandId::NextSkin },
		{ wxACCEL_SHIFT, WXK_UP, (int)CommandId::PreviousSkin },

		{ wxACCEL_CTRL, (int)'+', (int)CommandId::ZoomIn },
		{ wxACCEL_CTRL, (int)'-', (int)CommandId::ZoomOut },
		{ wxACCEL_CTRL, (int)'0', (int)CommandId::ZoomOriginal },
		{ wxACCEL_CTRL, WXK_F12, (int)CommandId::ZoomChartIn },
		{ wxACCEL_SHIFT | wxACCEL_CTRL, WXK_F12, (int)CommandId::ZoomChartOut },
		{ wxACCEL_NORMAL, WXK_F12, (int)CommandId::ZoomChartOriginal },

		{ wxACCEL_NORMAL, WXK_UP, (int)CommandId::KeyArrowUp },
		{ wxACCEL_NORMAL, WXK_DOWN, (int)CommandId::KeyArrowDown },

		{ wxACCEL_NORMAL, WXK_F10, (int)CommandId::NewText },
		{ wxACCEL_CTRL, (int)'t', (int)CommandId::NewDirection },
		{ wxACCEL_SHIFT | wxACCEL_CTRL, (int)'t', (int)CommandId::NewMultiDirection },

		// Vedic
		{ wxACCEL_CTRL, (int)'y', (int)CommandId::NewYoga },
		{ wxACCEL_SHIFT, WXK_F8, (int)CommandId::NewSbc },
		{ wxACCEL_SHIFT, WXK_F10, (int)CommandId::NewSolar },
		{ wxACCEL_CTRL, (int)'V', (int)CommandId::NewVarga },
		{ wxACCEL_SHIFT | wxACCEL_CTRL, (int)'B', (int)CommandId::NewShadbala },
		{ wxACCEL_CTRL, (int)'a', (int)CommandId::NewAshtakavarga },
		{ wxACCEL_SHIFT | wxACCEL_CTRL, (int)'d', (int)CommandId::NewDasaTree },
		{ wxACCEL_CTRL, (int)'d', (int)CommandId::NewDasaTreeComposite },
		{ wxACCEL_CTRL, (int)'g', (int)CommandId::NewDasaChart },

		// Western Chart
		{ wxACCEL_NORMAL, WXK_F2, (int)CommandId::NewWesternChart },

		// Uranian
		{ wxACCEL_CTRL, (int)'m', (int)CommandId::NewUranian },
		{ wxACCEL_NORMAL, WXK_F3, (int)CommandId::NewUranianChart },
		{ wxACCEL_CTRL, (int)'u', (int)CommandId::NewSensitivePoints },
		{ wxACCEL_SHIFT | wxACCEL_CTRL, (int)'u', (int)CommandId::NewPp },
		{ wxACCEL_CTRL, (int)'f', (int)CommandId::NewEventFinder },
	};

	const static std::pair<int,int> varga_shortcuts[(int)Varga::Last + 1] =
	{
		{ wxACCEL_NORMAL, WXK_F4 }, // Rasi
		{ wxACCEL_NORMAL, WXK_F5 }, // Navamsa
		{ wxACCEL_NORMAL, WXK_F6 }, // Hora
		{ wxACCEL_NORMAL, WXK_F7 }, // Drekkana
		{ wxACCEL_NORMAL, WXK_F8 }, // Chaturtamsa

		{ wxACCEL_SHIFT, WXK_F4 }, // Sastamsa
		{ wxACCEL_SHIFT, WXK_F5 }, // Saptamamsa
		{ wxACCEL_SHIFT, WXK_F6 }, // Ashtamsa
		{ wxACCEL_SHIFT, WXK_F7 }, // Dasamsa
		{ wxACCEL_SHIFT, WXK_F8 }, // Dvadasamsa

		{ wxACCEL_CTRL, WXK_F4 }, // Shodasamsa
		{ wxACCEL_CTRL, WXK_F5 }, // Vimsamsa
		{ wxACCEL_CTRL, WXK_F6 }, // Siddhamsa
		{ wxACCEL_CTRL, WXK_F7 }, // Bhamsa
		{ wxACCEL_CTRL, WXK_F8 }, // Trimsamsa

		{ wxACCEL_SHIFT | wxACCEL_CTRL, WXK_F4 }, // Chatvarimsamsa
		{ wxACCEL_SHIFT | wxACCEL_CTRL, WXK_F5 }, // Akshavedamsa
		{ wxACCEL_SHIFT | wxACCEL_CTRL, WXK_F6 }, // Shastiamsa
		{ wxACCEL_SHIFT | wxACCEL_CTRL, WXK_F7 }, // Ashtottaramsa
		{ wxACCEL_SHIFT | wxACCEL_CTRL, WXK_F8 }, // DvadasDvadasamsa

		{ wxACCEL_NORMAL, WXK_F9 }, // Bhava
	};

	for ( int i = 0; Varga v : Enum<Varga>() )
	{
		const int id = (int)CommandId::NewD1Chart + (int)v;
		accelerators.push_back( wxAcceleratorEntry( varga_shortcuts[i].first, varga_shortcuts[i].second, id ));
		i++;
	}

#ifdef DEBUG_COMMANDHANDLER
	debuglog << "Finished CommandHandler::initAccelerators: size is " << accelerators.size();
#endif
	assert( accelerators.size() == ACCELERATORSIZE );

}

/*****************************************************
**
**   CommandHandler   ---   init
**
******************************************************/
void CommandHandler::init()
{
	// App File
	cmds[CommandId::None] = new MCommand( BitmapId::None, "None" );

	cmds[CommandId::AppNew] = new MCommand( BitmapId::New, tr( "New" ));
	cmds[CommandId::AppNewSingle] = new MCommand( BitmapId::Sview, tr( "New Single View" ), tr( "New" ));
	cmds[CommandId::AppNewMultipleChoice] = new MCommand( BitmapId::Mview, tr( "Multiple Child Choice" ));
	cmds[CommandId::AppNewMultiple] = new MCommand( BitmapId::Mview, tr( "Multiple Child" ));
	cmds[CommandId::AppOpen] = new MCommand( BitmapId::Open, tr( "Open" ));
	cmds[CommandId::AppExit] = new MCommand( BitmapId::Quit, tr( "Exit" ));

	cmds[CommandId::Close] = new MCommand( BitmapId::Close, tr( "Close" ), tr( "Close Window" ));
	cmds[CommandId::CloseDoc] = new MCommand( BitmapId::Close, tr( "Close File" ));
	cmds[CommandId::Save] = new MCommand( BitmapId::Save, tr( "Save" ));
	cmds[CommandId::SaveAs] = new MCommand( BitmapId::Saveas, tr( "Save As" ));
	cmds[CommandId::Print] = new MCommand( BitmapId::Print, tr( "Print" ));
	cmds[CommandId::QuickPrint] = new MCommand( BitmapId::Quickprint, tr( "Quick Print" ));

	// child new view
	cmds[CommandId::NewText] = new MCommand( BitmapId::Text, tr( "TextView" ), tr( "Text" ));
	cmds[CommandId::NewDirection] = new MCommand( BitmapId::Direction, tr( "Directions" ));
	cmds[CommandId::NewMultiDirection] = new MCommand( BitmapId::Direction, tr( "Multi Directions" ));
	cmds[CommandId::NewYoga] = new MCommand( BitmapId::Yoga, tr( "Yoga" ));
	cmds[CommandId::NewSbc] = new MCommand( BitmapId::Sbc, tr( "Sarvatobhadra" ), tr( "SBC" ));
	cmds[CommandId::NewSolar] = new MCommand( BitmapId::Solar, tr( "Solar" ));
	cmds[CommandId::NewVarga] = new MCommand( BitmapId::Varga, tr( "Varga Summary" ), tr( "Varga" ));

	cmds[CommandId::NewShadbala] = new MCommand( BitmapId::Shadbala, tr( "Shadbala" ));
	cmds[CommandId::NewUniversalChart] = new MCommand( BitmapId::WesternChart, tr( "Universal Chart" ), tr( "UniChart" ));
	cmds[CommandId::NewWesternChart] = new MCommand( BitmapId::WesternChart, tr( "Western Chart" ), tr( "WChart" ));

	cmds[CommandId::NewUniversalRadix] = new MCommand( BitmapId::Uranian, tr( "Universal Radix" ));
	cmds[CommandId::NewUranian] = new MCommand( BitmapId::Uranian, tr( "Uranian" ));
	cmds[ CommandId::NewSensitivePoints ] = new MCommand( BitmapId::SensitivePoints, tr( "Sensitive Points" ));
	cmds[ CommandId::NewEventFinder ] = new MCommand( BitmapId::EventFinder, tr( "EventFinder" ));
	cmds[ CommandId::NewPp ] = new MCommand( BitmapId::PlanetaryPictures, tr( "Planetary Pictures" ));
	cmds[CommandId::NewUranianChart] = new MCommand( BitmapId::UranianChart, tr( "Uranian Chart" ));

	cmds[CommandId::NewAshtakavarga] = new MCommand( BitmapId::Ashtakavarga, tr( "Ashtakavarga" ), tr( "AV" ));
	cmds[CommandId::NewDasaTree] = new MCommand( BitmapId::Dasa, tr( "Simple Dasa Tree" ), tr( "T-Dasa" ));
	cmds[CommandId::NewDasaTreeComposite] = new MCommand( BitmapId::Dasa, tr( "Dasa Tree" ), tr( "T-Dasa" ));
	cmds[CommandId::NewDasaChart] = new MCommand( BitmapId::DasaChart, tr( "Dasa Chart" ));
	cmds[CommandId::NewLogWindow] = new MCommand( BitmapId::None, tr( "Log Window" ));

	// App New View 
	cmds[CommandId::AppHora] = new MCommand( BitmapId::Hora, tr( "Hora" ));
	cmds[CommandId::AppEclipse] = new MCommand( BitmapId::Eclipse, tr( "Eclipse" ));
	cmds[CommandId::AppPartner] = new MCommand( BitmapId::Partner, tr( "Partner" ));
	cmds[CommandId::AppEphemeris] = new MCommand( BitmapId::Ephem, tr( "Ephemeris" ), tr( "Ephem" ));

	cmds[CommandId::AppFileAndDirectoryReport] = new MCommand( BitmapId::Text, tr( "Files and Directories" ));
	cmds[CommandId::AppLicense] = new MCommand( BitmapId::Text, tr( "License" ));

// configuration on OXs has shortcut "cmd+,". This was mapped automatically on wx2.8 but is missing in wx3
#ifdef __WXOSX__ 
	cmds[CommandId::AppConfigure] = new MCommand( BitmapId::Config, tr( "Configuration" ), tr( "Config" ));
#else
	cmds[CommandId::AppConfigure] = new MCommand( BitmapId::Config, tr( "Configuration" ), tr( "Config" ));
#endif
	cmds[CommandId::AppConfigCalculation] = new MCommand( BitmapId::Config, tr( "Calculation" ), tr( "Config" ));
	cmds[CommandId::AppConfigGui] = new MCommand( BitmapId::Config, tr( "Config Gui" ), tr( "Config" ));
	cmds[CommandId::AppConfigPrintout] = new MCommand( BitmapId::Config, tr( "Config Printout" ), tr( "Config" ));

	cmds[CommandId::AppYogaEditor] = new MCommand( BitmapId::Yogaeditor, tr( "Yoga Editor" ));
	cmds[CommandId::AppYearlyHoroscope] = new MCommand( BitmapId::New, tr( "Yearly Horoscope" ));
	cmds[CommandId::AppAtlasdialog] = new MCommand( BitmapId::Atlas, tr( "Atlas" ));
	cmds[CommandId::AppAbout] = new MCommand( BitmapId::About, tr( "About" ));
	cmds[CommandId::AppHelp] = new MCommand( BitmapId::Help, tr( "Documentation (Web)" ), tr( "Config" ));

	// child varia
	cmds[CommandId::Animate] = new MCommand( BitmapId::None, tr( "Animate Chart" ));
	cmds[CommandId::EditData] = new MCommand( BitmapId::Edit, tr( "Edit Horoscope Data" ), tr( "Edit" ) );

	cmds[ CommandId::ViewpointVedic ] = new MCommand( BitmapId::None, tr( "Vedic Viewpoint" ));
	cmds[ CommandId::ViewpointWestern ] = new MCommand( BitmapId::None, tr( "Western Viewpoint" ));
	cmds[ CommandId::ViewpointUranian ] = new MCommand( BitmapId::None, tr( "Uranian Viewpoint" ));
	cmds[ CommandId::ViewpointAstronomical ] = new MCommand( BitmapId::None, tr( "Astronomical Viewpoint" ));

	cmds[ CommandId::ExportPlainText ] = new MCommand( BitmapId::ExportText, tr( "Plain Text" ));
	cmds[ CommandId::ExportCsv ] = new MCommand( BitmapId::ExportCsv, tr( "CSV Text" ));
	cmds[ CommandId::ExportHtml ] = new MCommand( BitmapId::ExportHtml, tr( "HTML" ));
	cmds[ CommandId::ExportPlainHtml ] = new MCommand( BitmapId::ExportHtml, tr( "Plain HTML" ));
	cmds[ CommandId::ExportTextClipboard ] = new MCommand( BitmapId::ExportImage, tr( "Clipboard" ));

	cmds[ CommandId::ExportImageJpeg ] = new MCommand( BitmapId::ExportImage, tr( "Image/Jpeg" ));
	cmds[ CommandId::ExportImagePng ] = new MCommand( BitmapId::ExportImage, tr( "Image/Png" ));
	cmds[ CommandId::ExportImageSvg ] = new MCommand( BitmapId::ExportImage, tr( "Image/Svg" ));
	cmds[ CommandId::ExportImageClipboard ] = new MCommand( BitmapId::ExportImage, tr( "Clipboard" ));

	cmds[ CommandId::NewEntryChart ] = new MCommand( BitmapId::New, tr( "Entry Chart" ), tr( "Entry" ));
	cmds[ CommandId::NewContextHoroscope ] = new MCommand( BitmapId::New, tr( "Create Context Horoscope" ), tr( "Create Horoscope" ));
	cmds[ CommandId::ToggleSplit ] = new MCommand( BitmapId::Split, tr( "Split/Unsplit" ), tr( "Split" ));
	cmds[ CommandId::Now ] = new MCommand( BitmapId::Now, tr( "Now" ));
	cmds[ CommandId::SelectDasa ] = new MCommand( BitmapId::Dasa, tr( "Select Dasa" ), tr( "Dasa" ));
	cmds[ CommandId::GotoYear ] = new MCommand( BitmapId::Goto, tr( "Goto Year" ), tr( "Goto" ));
	cmds[ CommandId::Filter ] = new MCommand( BitmapId::Filter, tr( "Filter" ));
	cmds[ CommandId::ClearFilter ] = new MCommand( BitmapId::Close, tr( "Clear Filter" ));
	cmds[ CommandId::LaunchViewer ] = new MCommand( BitmapId::Print, tr( "Lanuch Viewer" ), tr( "Launch" ));
	cmds[ CommandId::Tip ] = new MCommand( BitmapId::Tip, tr( "Tip" ));

	cmds[ CommandId::ZoomIn ] = new MCommand( BitmapId::ZoomIn, tr( "Zoom In" ));
	cmds[ CommandId::ZoomOriginal ] = new MCommand( BitmapId::ZoomOriginal, tr( "100%" ));
	cmds[ CommandId::ZoomOut ] = new MCommand( BitmapId::ZoomOut, tr( "Zoom Out" ));

	cmds[ CommandId::NextGeometry ] = new MCommand( tr( "Next Geometry" ));
	cmds[ CommandId::PreviousGeometry ] = new MCommand( tr( "Previous Geometry" ));
	cmds[ CommandId::NextSkin ] = new MCommand( tr( "Next Skin" ));
	cmds[ CommandId::PreviousSkin ] = new MCommand( tr( "Previous Skin" ));

	cmds[ CommandId::ZoomChartIn ] = new MCommand( BitmapId::ZoomIn, tr( "Increase Chart Size" ));
	cmds[ CommandId::ZoomChartOriginal ] = new MCommand( BitmapId::ZoomOriginal, tr( "Default Chart Size" ));
	cmds[ CommandId::ZoomChartOut ] = new MCommand( BitmapId::ZoomOut, tr( "Decrease Chart Size" ));

	// general commands
	cmds[CommandId::KeyArrowUp] = new MCommand( BitmapId::None, tr( "Up" ));
	cmds[CommandId::KeyArrowDown] = new MCommand( BitmapId::None, tr( "Down" ));

	// App Varia
	cmds[CommandId::AppSaveConfig] = new MCommand( BitmapId::Save, tr( "Save Configuration" ));
	cmds[CommandId::AppToggleMainToolbar] = new MCommand( BitmapId::None, tr( "Show Toolbar" ));
	cmds[CommandId::AppToggleStatusbar] = new MCommand( BitmapId::None, tr( "Show Statusbar" ));
	cmds[CommandId::AppToggleLogWindow] = new MCommand( BitmapId::None, tr( "Show Log Window" ));

	Lang lang;
	wxString shortName, longName;

	// generate Varga shortcuts
	for ( Varga v : Enum<Varga>() )
	{
		const CommandId commandId = (CommandId)((int)CommandId::NewD1Chart + (int)v);

		wxString shortName = lang.getVargaName( v );
		wxString longName = shortName;
		if ( vargaDivisions[ (int)v ] )
		{
			longName + " (D-" + std::format( "{}", vargaDivisions[ (int)v ] ) + ")";;
		}
		cmds[commandId] = new MCommand( (BitmapId)((int)BitmapId::Chart1 + (int)v), shortName, longName );
	}
}

