/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gui/MenuProvider.cpp
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

#include "MenuProvider.h"

#include "astroconfig.h"
#include "BasicWidget.h"
#include "CategoryHelper.h"
#include "ChartGeometry.h"
#include "ChildWindow.h"
#include "ConfigLoaderImpls.h"
#include "ContextProperties.h"
#include "Commands.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "Horoscope.h"
#include "ImageProvider.h"
#include "Lang.h"
#include "MultipleView.h"
#include "Varga.h"
#include "WindowDirectory.h"
#include "Viewpoint.h"

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define DEBUG_MENUPROVIDER

/**************************************************************
***
**    MyMenu   ---   Constructor
***
***************************************************************/
MyMenu::MyMenu( const wxString title )
	: wxMenu( title )
{
	handler = CommandHandler::get();
}

/**************************************************************
***
**    MyMenu   ---   addItem
***
***************************************************************/
wxMenuItem *MyMenu::addItem( const CommandId &id )
{
#ifdef DEBUG_MENUPROVIDER
	debuglog << "MyMenu::addItem " << (int)id;
#endif	
	wxMenuItem *item = nullptr;
	const MCommand *const command = handler->getCommand( id );
	if ( command )
	{
		int commandId = (int)id;
		assert( command );

#ifdef DEBUG_MENUPROVIDER
		debuglog << "MyMenu::addItem " << command->text;
#endif	

		item = new wxMenuItem( this, commandId, command->text );
		item->SetAccel( handler->getAcceleratorEntry( id ));

		if ( command->bmpId != BitmapId::None )
		{
			item->SetBitmap( ImageProvider::get()->getBitmapBundle( command->bmpId ));
		}
		Append( item );
	}
	//else assert( 0 );
	else warnlog << "invalid menmu item, cannot create command, id was " << id;
	return item;
}

/**************************************************************
***
**    MyMenu   ---   enable
***
***************************************************************/
void MyMenu::enable( const CommandId &cmdid, const bool &doEnable )
{
	Enable( (int)cmdid, doEnable );
}

/**************************************************************
***
**    MyMenu   ---   check
***
***************************************************************/
void MyMenu::check( const CommandId &cmdid, const bool &doEnable )
{
	Check( (int)cmdid, doEnable );
}
/**************************************************************
***
**    MyMenu   ---   append
***
***************************************************************/
void MyMenu::append( const CommandId &cmdid, const wxString &title )
{
	Append( (int)cmdid, title );
}

/**************************************************************
***
**    MyMenu   ---   addCheckItem
***
***************************************************************/
void MyMenu::addCheckItem( const CommandId &id, const bool checked )
{
	MCommand *command = handler->getCommand( id );
#ifdef DEBUG_MENUPROVIDER
		debuglog << "MyMenu::addCheckItem " << id << " text " << command->text;
#endif	
	if ( command )
	{
		wxMenuItem *item = AppendCheckItem( (int)id, command->text );
		item->SetAccel( handler->getAcceleratorEntry( id ));
		item->Check(  checked );
	}
	else assert( 0 );
}

/**************************************************************
***
**    MyMenu   ---   addCheckItem
***
***************************************************************/
void MyMenu::addCheckItem( const CommandId &id, const wxString title, const bool checked )
{
#ifdef DEBUG_MENUPROVIDER
		debuglog << "MyMenu::addCheckItem " << id << " title " << title;
#endif	
	AppendCheckItem( (int)id, title );
	Check( (int)id, checked );
}

/**************************************************************
***
**    MyMenu   ---   addItemList
***
***************************************************************/
void MyMenu::addItemList( const std::list<CommandId> &items )
{
	for( const auto &item : items )
	{
		if ( item != CommandId::None ) addItem( item );
		else AppendSeparator();
	}
}

/**************************************************************
***
**    MyMenu   ---   addWindowListMenu
***
***************************************************************/
void MyMenu::addWindowListMenu()
{
	MyMenu *wlmenu = new MyMenu();
	wlmenu->AppendRadioItem( (int)CommandId::AppListLargeIcons, tr( "Big Icons" ) );
	wlmenu->AppendRadioItem( (int)CommandId::AppListSmallIcons, tr( "Small Icons" ) );
	wlmenu->AppendRadioItem( (int)CommandId::AppListTable, tr( "Table" ) );
	switch ( gdiconfig->prefs.listStyle )
	{
		case 1:
			wlmenu->check( CommandId::AppListSmallIcons, true );
		break;
		case 2:
			wlmenu->check( CommandId::AppListTable, true );
		break;
		default:
			wlmenu->check( CommandId::AppListLargeIcons, true );
		break;
	}
	Append( -1, tr("Window List"), wlmenu );
}

/**************************************************************
***
**    MyMenu   ---   addVargaMenu
***
***************************************************************/
void MyMenu::addVargaMenu()
{
	MyMenu *menu = new MyMenu( wxEmptyString ); //, view );
	for ( int i = 0; i < enum_count<Varga>(); i++ ) menu->addItem( CommandId::NewD1Chart + i );
	Append( -1, tr( "Varga"), menu );
}

/*****************************************************
**
**   AppMenuBar   ---   Constructor
**
******************************************************/
AppMenuBar::AppMenuBar( const int &style )
		: wxMenuBar( style )
{
	{
		const static std::list<CommandId> fileitems =
		{
			CommandId::None, CommandId::Save, CommandId::SaveAs, CommandId::CloseDoc, CommandId::None, CommandId::AppExit
		};

		filemenu = new MyMenu;
		filemenu->addItem( CommandId::AppNew );

		newmenu = new MyMenu;
		filemenu->Append( -1, tr( "New ..." ), newmenu );

		filemenu->addItem( CommandId::AppOpen );

		recentmenu = new MyMenu;
		for( uint i = 0; i < gdiconfig->prefs.recentFiles.size(); i++ )
		{
			recentfiles.Add( gdiconfig->prefs.recentFiles[i] );
		}
		for ( size_t n = 0; n < recentfiles.GetCount(); n++ )
			recentmenu->append( CommandId::AppFileRecent + n, recentfiles[n] );
		filemenu->Append( -1, tr("Open Recent"), recentmenu );

		filemenu->addItemList( fileitems );
		Append( filemenu, tr( "&File" ));
	}

	{
		const static std::list<CommandId> horoscopeitems =
		{
			CommandId::NewText, CommandId::NewUniversalChart, CommandId::NewDirection,
			CommandId::NewSolar, CommandId::NewUniversalRadix, CommandId::None,
			CommandId::EditData, CommandId::Animate, CommandId::None,
			CommandId::Print, CommandId::QuickPrint
		};

		horoscopemenu = new MyMenu;
		horoscopemenu->addItemList( horoscopeitems );
		Append( horoscopemenu, tr( "&Horoscope"));
	}

	{
		const static std::list<CommandId> vedicitems =
		{
			CommandId::NewVarga, CommandId::NewSbc, CommandId::None,
			CommandId::NewDasaTree, CommandId::NewDasaTreeComposite, CommandId::NewDasaChart, CommandId::None,
			CommandId::NewYoga, CommandId::NewAshtakavarga, CommandId::NewShadbala
		};
		vedicmenu = new MyMenu;
		vedicmenu->addVargaMenu();
		vedicmenu->addItemList( vedicitems );
		Append( vedicmenu, tr( "&Vedic"));
	}

	{
		const static std::list<CommandId> westernitems =
		{
			CommandId::NewWesternChart, CommandId::None,
			CommandId::NewUranian, CommandId::NewUranianChart, CommandId::NewSensitivePoints,
			CommandId::NewPp, CommandId::NewEventFinder, CommandId::NewMultiDirection, CommandId::None
		};
		westernmenu = new MyMenu;
		westernmenu->addItemList( westernitems );
		Append( westernmenu, tr( "&Western"));
	}

	{
		windowmenu = new MyMenu;
		windowmenu->AppendSeparator();

		windowmenu->addCheckItem( CommandId::ViewpointVedic );
		windowmenu->addCheckItem( CommandId::ViewpointWestern );
		windowmenu->addCheckItem( CommandId::ViewpointUranian );
		windowmenu->addCheckItem( CommandId::ViewpointAstronomical );
		windowmenu->AppendSeparator();
		windowmenu->addItem( CommandId::Close );
		Append( windowmenu,
#ifdef __WXOSX__
		tr( "Viewpoint"));
#else
		tr( "&Window"));
#endif
	}

	{
		const static std::list<CommandId> extraitems =
		{
			CommandId::AppEphemeris, CommandId::AppEclipse, CommandId::AppHora, CommandId::AppYearlyHoroscope,
			CommandId::AppPartner, CommandId::None,
			CommandId::AppConfigure, CommandId::AppConfigGui, CommandId::AppConfigPrintout, CommandId::None,
			CommandId::AppAtlasdialog, CommandId::AppYogaEditor,
			CommandId::AppFileAndDirectoryReport, CommandId::AppSaveConfig, CommandId::None
		};
		extramenu = new MyMenu;
		extramenu->addItemList( extraitems );
		extramenu->addWindowListMenu();
		extramenu->addCheckItem( CommandId::AppToggleMainToolbar, gdiconfig->prefs.showMainToolbar );
		extramenu->addCheckItem( CommandId::AppToggleStatusbar, gdiconfig->prefs.showStatusBar );
		extramenu->addCheckItem( CommandId::AppToggleLogWindow, gdiconfig->prefs.showLogWindow );
		Append( extramenu, tr( "&Extras"));
	}

	{
		helpmenu = new MyMenu;
		helpmenu->addItem( CommandId::AppHelp );
		helpmenu->addItem( CommandId::AppLicense );
		helpmenu->addItem( CommandId::AppAbout );
		Append( helpmenu, tr( "&Help"));
	}

	Connect( (int)CommandId::AppConfigure, wxEVT_MENU_OPEN, wxMenuEventHandler( AppMenuBar::onMViewMenuOpen ));
	Connect( wxID_ANY, WD_WINDOW_ACTIVATED, wxCommandEventHandler( AppMenuBar::OnUpdateMenus ));
	Connect( wxID_ANY, WD_WINDOW_ADDED, wxCommandEventHandler( AppMenuBar::OnUpdateMenus ));
	Connect( wxID_ANY, WD_WINDOW_REMOVED, wxCommandEventHandler( AppMenuBar::OnUpdateMenus ));

	WindowDirectory::get()->subscribe( this );
}

/*****************************************************
**
**   AppMenuBar   ---   Destructor
**
******************************************************/
AppMenuBar::~AppMenuBar()
{
	WindowDirectory::get()->unsubscribe( this );
}

/*****************************************************
**
**   AppMenuBar   ---   updateMenus
**
******************************************************/
void AppMenuBar::updateMenus( ChildWindow* )
{
	bool isDoc = false;

	ChildWindow *window = WindowDirectory::get()->getActiveWindow();
#ifdef DEBUG_MENUPROVIDER
	debuglog << "AppMenuBar::updateMenus view " << window;
#endif	

	if ( window )
	{
		ContextProps_ptr props = window->getProps();
		if ( props->hasRadixHoroscope() ) isDoc = true;

		const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::None );

		bool enableViewpointItems = false;
		switch( astroconfig->general.viewpointChangePolicy )
		{
			case ViewpointChangePolicy::Fixed:
				enableViewpointItems = false;
			break;
			case ViewpointChangePolicy::Medium:
			{
				const Viewpoint &vp = ViewpointHelper::get()->getStickyViewVp( window->getViewId() );
				enableViewpointItems = ( vp == Viewpoint::None );
				//const ViewpointPreference &vp = ViewpointHelper::get()->calculateViewpointPreference( child->getViewId() );
				//enableViewpointItems = ( vp == ViewpointPreference::Universal );
#ifdef DEBUG_MENUPROVIDER
				debuglog << "AppMenuBar::updateMenus pref " << vp
					<< " enableViewpointItems " << std::boolalpha << enableViewpointItems;
#endif	
			}
			break;
			case ViewpointChangePolicy::Flexible:
			default:
				enableViewpointItems = true;
			break;
		}
#ifdef DEBUG_MENUPROVIDER
		debuglog << "AppMenuBar::updateMenus isDoc " << std::boolalpha << isDoc
			<< " enableViewpointItems " << std::boolalpha << enableViewpointItems;
#endif	

		windowmenu->enable( CommandId::ViewpointVedic, enableViewpointItems );
		windowmenu->enable( CommandId::ViewpointWestern, enableViewpointItems );
		windowmenu->enable( CommandId::ViewpointUranian, enableViewpointItems );
		windowmenu->enable( CommandId::ViewpointAstronomical, enableViewpointItems );
		windowmenu->enable( CommandId::Close, true );

		windowmenu->check( CommandId::ViewpointVedic, viewpoint == Viewpoint::Vedic );
		windowmenu->check( CommandId::ViewpointWestern, viewpoint == Viewpoint::Western );
		windowmenu->check( CommandId::ViewpointUranian, viewpoint == Viewpoint::Uranian );
		windowmenu->check( CommandId::ViewpointAstronomical, viewpoint == Viewpoint::Astronomical );
	}
	else // no child
	{
#ifdef DEBUG_MENUPROVIDER
		debuglog << "AppMenuBar::updateMenus, child is NULL";
#endif	
	}


	filemenu->enable( CommandId::AppFileRecent, recentfiles.GetCount() > 0  );
	filemenu->enable( CommandId::CloseDoc, isDoc );
	filemenu->enable( CommandId::Save, isDoc );
	filemenu->enable( CommandId::SaveAs, isDoc );

	// Horoscope, vedic and western menus
	EnableTop( 1, isDoc );
	EnableTop( 2, isDoc );
	EnableTop( 3, isDoc );

	// window menu
	EnableTop( 4, window != nullptr );
}

/*****************************************************
**
**   AppMenuBar   ---   OnOpenMenu
**
******************************************************/
void AppMenuBar::OnOpenMenu( wxMenu *menu )
{
#ifdef DEBUG_MENUPROVIDER
	debuglog << "AppMenuBar::OnOpenMenu";
#endif	
	if ( menu == filemenu && ! newmenu->GetMenuItemCount() )
	{
		newmenu->addItem( CommandId::AppNewSingle );
		newmenu->AppendSeparator();

		for( int i = 0; const auto &def : MultipleViewHandler::get()->getBuiltinDefs())
		{
			newmenu->append( CommandId::AppNewMultiple + i, def.name );
			++i;
		}
		newmenu->UpdateUI();
	}
	if ( menu == extramenu )
	{
		extramenu->Check( (int)CommandId::AppToggleLogWindow, gdiconfig->prefs.showLogWindow );
	}
}

/*****************************************************
**
**   AppMenuBar   ---   addToRecentFiles
**
******************************************************/
void AppMenuBar::addToRecentFiles( const wxString &filename )
{
	const uint maxrecent = (uint)gdiconfig->view.maxRecentFiles;
	uint recentcount = recentmenu->GetMenuItemCount();
	size_t n;

	recent_files.push_front( filename );
	//recent_files.advance();

	/* TODO
		1. duplikate rausnehmen
		2. bei zu grosser Anzahl letzten rausnehmen
	*/

	// clear recent file menu
	for ( n = 0; n < recentcount; n++ ) recentmenu->Delete( (int)CommandId::AppFileRecent + n );

	// Clear obsolete items ( if number of files has changed via ConfigDialog)
	if ( recentfiles.GetCount() > maxrecent )
	{
		for ( n = 0; n < recentfiles.GetCount() - maxrecent; n++ )
			recentfiles.RemoveAt( maxrecent + n );
	}

	// update file list
	for ( n = 0; n < recentfiles.GetCount(); n++ )
	{
		if ( recentfiles[n] == filename )
		{
			recentfiles.RemoveAt( n );
			break;
		}
	}
	if ( recentfiles.GetCount() == maxrecent ) recentfiles.RemoveAt( recentfiles.GetCount() - 1 );
	recentfiles.Insert( filename, 0 );
	for ( n = 0; n < recentfiles.GetCount(); n++ )
	{
		recentmenu->append( CommandId::AppFileRecent + n, recentfiles[n] );
	}
}

/*****************************************************
**
**   AppMenuBar   ---   getRecentFilesEntry
**
******************************************************/
wxString AppMenuBar::getRecentFilesEntry( const int &commandId )
{
	const int fileindex = commandId - (int)CommandId::AppFileRecent;
	assert( fileindex >= 0 && fileindex < (int)recentfiles.GetCount());
	return recentfiles[ fileindex ];
}

/*****************************************************
**
**   AppMenuBar   ---   saveRecentFiles
**
******************************************************/
void AppMenuBar::saveRecentFiles()
{
	gdiconfig->prefs.recentFiles.clear();
	for ( size_t n = 0; n < recentfiles.GetCount(); n++ )
	{
		gdiconfig->prefs.recentFiles.push_back( recentfiles[n] );
	}
}

/*****************************************************
**
**   AppMenuBar   ---   onMViewMenuOpen
**
******************************************************/
void AppMenuBar::onMViewMenuOpen( wxMenuEvent& )
{
#ifdef DEBUG_MENUPROVIDER
	debuglog << "AppMenuBar::onMViewMenuOpen";
#endif	
}

/*****************************************************
**
**   AppMenuBar   ---   OnUpdateMenus
**
******************************************************/
void AppMenuBar::OnUpdateMenus( wxCommandEvent &event )
{
	const int index = (int)event.GetExtraLong();
#ifdef DEBUG_MENUPROVIDER
	debuglog << "AppMenuBar::OnUpdateMenus index " << index;
#endif	
	updateMenus( WindowDirectory::get()->getWindow( index ));
}

/**************************************************************
***
**   ContextMenuProvider   ---   getChildWindowListCtrlMenu
***
***************************************************************/
MyMenu *ContextMenuProvider::getChildWindowListCtrlMenu( ChildWindow *child )
{
	if ( ! child )
	{
		errorlog << "ContextMenuProvider::getChildWindowListCtrlMenu: child is NULL";
		return nullptr;
	}
	MyMenu *menu = new MyMenu( child->GetTitle());
	ContextProps_ptr props = child->getProps();
	menu->AppendSubMenu( getNewViewMenu( props ), tr( "New View"));
	menu->AppendSeparator();

	if ( props->hasRadixHoroscope())
	{
		menu->addItem( CommandId::EditData );
		menu->addCheckItem( CommandId::Animate, props->isAnimated() );
		menu->AppendSeparator();
	}
	menu->addItem( CommandId::Close );
	return menu;
}

/**************************************************************
***
**   ContextMenuProvider   ---   getWidgetMenu
***
***************************************************************/
MyMenu *ContextMenuProvider::getWidgetMenu( BasicWidget *widget )
{
	assert( widget );
	ContextProps_ptr props = widget->getProps();
	const WidgetCategory category = widget->getWidgetCategory();
#ifdef DEBUG_MENUPROVIDER
	debuglog << "ContextMenuProvider::getWidgetMenu category " << category;
#endif	

	MyMenu *menu = new MyMenu( std::format( "Widget: {} ({})", enum_to_string( category ), enum_to_string( widget->getViewpoint())));

	menu->AppendSubMenu( getNewViewMenu( props ), tr( "New View"));
	menu->AppendSeparator();

	if ( props->hasRadixHoroscope() )
	{
		menu->addItem( CommandId::EditData );
		menu->addCheckItem( CommandId::Animate, props->isAnimated() );
		menu->AppendSeparator();
	}

	// Factor list menu
	if ( IS_OBJECTLIST_WIDGET( category )
		|| IS_CHART_WIDGET( category )
		|| category == WidgetCategory::TextPlanetBalance
		|| category == WidgetCategory::TextVedicHouses
		|| category == WidgetCategory::TextUranianHouses
		|| category == WidgetCategory::TextKp
		|| category == WidgetCategory::TextNakshatra
		|| category == WidgetCategory::TextVarga
		|| category == WidgetCategory::TextVedicAspectarium
		|| category == WidgetCategory::TextWesternAspectarium
		|| category == WidgetCategory::TextWesternAspectList
		|| category == WidgetCategory::TextSynastry
		|| category == WidgetCategory::WesternChart
		|| category == WidgetCategory::WesternChart
		|| category == WidgetCategory::WesternChart
		|| category == WidgetCategory::TextUranianMatches
		|| category == WidgetCategory::TextSensitivePoints
		|| category == WidgetCategory::TextPlanetListCombined
		|| category == WidgetCategory::MatchesWidget
		|| category == WidgetCategory::PpTree
		|| category == WidgetCategory::PpWidget
		|| category == WidgetCategory::SpWidget
	)
	{
		menu->AppendSubMenu( getPlanetListMenu( widget ), tr( "Planets"));
	}

	// Skin menu
	if ( category != WidgetCategory::None && ! IS_TEXT_WIDGET( category ))
	{
		menu->AppendSubMenu( getSkinMenu( props, widget->getWidgetCategory() ), tr( "Skin" ));
	}

	// Geom menu
	if ( IS_CHART_WIDGET( category ))
	{
		menu->AppendSubMenu( getGeomMenu( props, widget->getWidgetCategory() ), tr( "Geometry" ));
	}

	// Columns menu
	if ( IS_OBJECTLIST_WIDGET( category ))
	{
		menu->AppendSubMenu( getColumnGroupMenu( props ), tr( "Columns"));
	}

	appendExportMenu( menu, category );
	menu->AppendSeparator();
	menu->AppendSeparator();
#if 0
	menu->addItem( CommandId::ZoomIn );
	menu->addItem( CommandId::ZoomOut );
	menu->addItem( CommandId::ZoomOriginal );
#else
	appendSizeMenu( menu, category );
#endif
	menu->addItem( CommandId::Close );
	return menu;
}

/**************************************************************
***
**    ContextMenuProvider   ---   appendSizeMenu
***
***************************************************************/
void ContextMenuProvider::appendSizeMenu( MyMenu* menu, const WidgetCategory &category)
{
	MyMenu *sizemenu = new MyMenu( tr( "Sizes" ));
	sizemenu->addItem( CommandId::ZoomIn );
	sizemenu->addItem( CommandId::ZoomOut );
	sizemenu->addItem( CommandId::ZoomOriginal );
	if ( IS_CHART_WIDGET( category ))
	{
		sizemenu->addItem( CommandId::ZoomChartIn );
		sizemenu->addItem( CommandId::ZoomChartOut );
		sizemenu->addItem( CommandId::ZoomChartOriginal );
	}
	menu->AppendSubMenu( sizemenu, tr( "Sizes"));
}

/**************************************************************
***
**    ContextMenuProvider   ---   appendExportMenu
***
***************************************************************/
void ContextMenuProvider::appendExportMenu( MyMenu* menu, const WidgetCategory &category )
{
	debuglog << "ContextMenuProvider::appendExportMenu category " << category;
	if ( IS_CHART_WIDGET( category ))
	{
		MyMenu *exportmenu = new MyMenu( tr( "Export Image" ));
		exportmenu->append( CommandId::ExportImageClipboard, tr( "Clipboard" ));
		exportmenu->append( CommandId::ExportImageJpeg, tr( "File/Jpeg" ));
		exportmenu->append( CommandId::ExportImagePng, tr( "File/Png" ));
		exportmenu->append( CommandId::ExportImageSvg, tr( "File/Svg" ));
		menu->AppendSubMenu( exportmenu, tr( "Export Image") + " ..." );
	}
	else if ( IS_TEXT_WIDGET( category ))
	{
		MyMenu *exportmenu = new MyMenu( tr( "Export Text" ));
		exportmenu->addItem( CommandId::ExportTextClipboard );
		exportmenu->addItem( CommandId::ExportPlainText );
		exportmenu->addItem( CommandId::ExportCsv );
		exportmenu->addItem( CommandId::ExportHtml );
		exportmenu->addItem( CommandId::ExportPlainHtml );
		menu->AppendSubMenu( exportmenu, tr( "Export Text" ) + " ..." );
	}
}

/**************************************************************
***
**    ContextMenuProvider   ---   getGeomMenu
***
***************************************************************/
MyMenu *ContextMenuProvider::getGeomMenu( ContextProps_ptr props, const WidgetCategory &category  )
{
	MyMenu *menu = new MyMenu( tr( "Geometry" ));

	const WidgetCategory supercat = CategoryHelper::get()->getGeomCategory( category );
	const uint checked_geom = props->getGeometry( category );
#ifdef DEBUG_MENUPROVIDER
	debuglog << "ContextMenuProvider::getGeomMenu geom " << checked_geom;
#endif	

  const std::vector<std::pair<uint,wxString>> l = ChartGeometryLoader::get()->getAllIdsAndNames( supercat );
	for( uint i = 0; i < l.size(); i++ )
	{
#ifdef DEBUG_MENUPROVIDER
		debuglog << "ContextMenuProvider::getGeomMenu " << i << " first " << l[i].first
			<< " second " << l[i].second ;
#endif	
		menu->addCheckItem( CommandId::ChartGeom + i, l[i].second, l[i].first == checked_geom );
	}

	menu->AppendSeparator();
	menu->addItem( CommandId::NextGeometry );
	menu->addItem( CommandId::PreviousGeometry );
	return menu;
}

/**************************************************************
***
**    ContextMenuProvider   ---   getSkinMenu
***
***************************************************************/
MyMenu *ContextMenuProvider::getSkinMenu( ContextProps_ptr props, const WidgetCategory &category )
{
	MyMenu *menu = new MyMenu( tr( "Skin" ));

	const std::vector<wxString> l = WidgetSkinLoader::get()->getNames();
	for( uint i = 0; i < l.size(); i++ )
	{
		menu->addCheckItem( CommandId::ChartSkin + i, l[i], i == props->getSkin( category ));
	}

	menu->AppendSeparator();
	menu->addItem( CommandId::NextSkin );
	menu->addItem( CommandId::PreviousSkin );
	return menu;
}

/**************************************************************
***
**   ContextMenuProvider   ---   getPlanetListMenu
***
***************************************************************/
MyMenu *ContextMenuProvider::getPlanetListMenu( BasicWidget *widget )
{
	Lang lang;
	const Viewpoint &viewpoint = widget->getViewpoint();
	MyMenu *menu = new MyMenu( tr( "Planets" ));

	for( const auto &a: widget->getProps()->getPlanetPairs( viewpoint ))
	{
		const CommandId commandId = (CommandId)( (int)CommandId::ObToggleFirstPlanet + (int)a.first );
		assert( commandId >= CommandId::ObToggleFirstPlanet && commandId <= CommandId::ObToggleLastPlanet );

#ifdef DEBUG_MENUPROVIDER
	debuglog << "ContextMenuProvider::getPlanetListMenu Planet " << a.first << " commandId " << commandId;
#endif	
		menu->addCheckItem( commandId, lang.getPlanetName( a.first, viewpoint, TextFormat::Long ), a.second );
	}
	return menu;
}

/**************************************************************
***
**    ContextMenuProvider   ---   getColumnGroupMenu
***
***************************************************************/
MyMenu *ContextMenuProvider::getColumnGroupMenu( ContextProps_ptr props )
{
	MyMenu *menu = new MyMenu( tr( "Columns" ));
	ViewpointHelper *vh = ViewpointHelper::get();
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::None );

	for( const auto &a : props->getColumnGroups( viewpoint ))
	{
		const CommandId command = vh->columnGroupToCommandId( a.first );
		if ( command != CommandId::None )
		{
			menu->addCheckItem( command, vh->getColumnGroupName( a.first, TextFormat::Long ), a.second );
		}
	}
	return menu;
}

/**************************************************************
***
**   ContextMenuProvider   ---   getAppWindowContextMenu
***
***************************************************************/
MyMenu *ContextMenuProvider::getAppWindowContextMenu()
{
	const std::list<CommandId> appmenuitems =
	{
		CommandId::AppNew, CommandId::AppOpen, CommandId::None,
		CommandId::AppEphemeris, CommandId::AppEclipse, CommandId::AppHora,
		CommandId::AppPartner, CommandId::None
	};


	MyMenu *menu = new MyMenu;
	menu->addItemList( appmenuitems );
	menu->addWindowListMenu();
	menu->addItem( CommandId::AppConfigure );
	menu->addItem( CommandId::AppAtlasdialog );
	menu->AppendSeparator();
	menu->addCheckItem( CommandId::AppToggleMainToolbar, gdiconfig->prefs.showMainToolbar );
	menu->addCheckItem( CommandId::AppToggleStatusbar, gdiconfig->prefs.showStatusBar );
	menu->addCheckItem( CommandId::AppToggleLogWindow, gdiconfig->prefs.showLogWindow );
	menu->AppendSeparator();
	menu->addItem( CommandId::AppExit );
	return menu;
}

/**************************************************************
***
**   ContextMenuProvider   ---   getToolbarNewMenu
***
***************************************************************/
MyMenu *ContextMenuProvider::getToolbarNewMenu()
{
	MyMenu *newmenu = new MyMenu;
	newmenu->addItem( CommandId::AppNewSingle );

	newmenu->AppendSeparator();
	int i = 0;
	for( const auto &def : MultipleViewHandler::get()->getBuiltinDefs())
	{
		newmenu->append( CommandId::AppNewMultiple + i, def.name );
		++i;
	}
	return newmenu;
}

/**************************************************************
***
**   ContextMenuProvider   ---   getNewViewMenu
***
***************************************************************/
MyMenu *ContextMenuProvider::getNewViewMenu( ContextProps_ptr props )
{
	const std::list<CommandId> vedicitems =
	{
		CommandId::None, CommandId::NewDasaTree, CommandId::NewDasaTreeComposite,
		CommandId::NewDasaChart, CommandId::NewDirection, CommandId::None,
		CommandId::NewYoga, CommandId::NewAshtakavarga,
		CommandId::NewShadbala, CommandId::NewSolar
	};

	const std::list<CommandId> westernitems =
	{
		CommandId::None, CommandId::NewUranian, CommandId::NewUranianChart, CommandId::NewSensitivePoints,
		CommandId::NewEventFinder, CommandId::NewDirection, CommandId::NewSolar
	};
	const std::list<CommandId> appitems =
	{
		CommandId::AppEphemeris, CommandId::AppEclipse, CommandId::AppHora, CommandId::AppPartner
	};

	MyMenu *menu = new MyMenu( tr( "New View" ) );
	const Viewpoint viewpoint = props->getEffectiveViewpoint( Viewpoint::None );
	bool vedicmode = viewpoint == Viewpoint::Vedic;

	if ( wxGetKeyState( WXK_SHIFT )) vedicmode = ! vedicmode;

	if ( props->hasRadixHoroscope())
	{
		// Varga submenu or western chart
		if ( vedicmode ) menu->addVargaMenu();
		else menu->addItem( CommandId::NewWesternChart );

		if ( vedicmode )
		{
			menu->addItem( CommandId::NewVarga );
			menu->addItem( CommandId::NewSbc );
		}
		menu->addItem( CommandId::NewText );
		menu->addItemList( vedicmode ? vedicitems : westernitems );
	}
	else
	{
		menu->addItemList( appitems );
	}
	return menu;
}

