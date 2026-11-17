/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/DasaTree.cpp
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

#include "DasaTree.h"

#include "astroconfig.h"
#include "ContextProperties.h"
#include "Commands.h"
#include "Dasa.h"
#include "DasaTool.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "ImageProvider.h"
#include "Lang.h"
#include "mathbase.h"
#include "Painter.h"
#include "DocHelper.h"

#include <iostream>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

//#define DEBUG_DASATREE

/*****************************************************
**
**   DasaTree   ---   Constructor
**
******************************************************/
DasaTree::DasaTree( ContextProps_ptr props ) : GdiWidget( props, WidgetCategory::DasaTree )
{
	{
		DasaExpertFactory factory;
		assert( props->getRadixHoroscope());
		for ( const DasaId &id : Enum<DasaId>() )
		{
			experts[id] = DasaExpert_ptr( factory.getDasaExpert( id ));
		}
	}

	currentItem = nullptr;
	treesize = MComplex(500, 500 );

	// root means here root of the visual tree.
	rootItem = GraphicalDasa_ptr( new GraphicalDasa( DasaLevel::TreeRoot, tr( "Dasa Tree" )));
	rootItem->expanded = true;

	rootItem->children.push_back(
		createFolder(
		{
			DasaId::Vimsottari,
			DasaId::Yogini,
			DasaId::Kalachakra
		},
		tr( "Standard Dasas" ))
	);

	rootItem->children.push_back(
		createFolder(
		{
			DasaId::Ashtottari,
			DasaId::Shodshottari,
			DasaId::Dvadashottari,
			DasaId::Panchottari,
			DasaId::Shatabdika,
			DasaId::ChaturashitiSama,
			DasaId::DvisaptatiSama,
			DasaId::ShatTrimsatSama
		},
		tr( "Conditional Dasas" ))
	);

	rootItem->children.push_back(
		createFolder(
		{
			DasaId::LagnaVimsottari
		},
		tr( "Special Dasas" ))
	);
}

/*****************************************************
**
**   DasaTree   ---   OnDataChanged
**
******************************************************/
void DasaTree::OnDataChanged()
{
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::OnDataChanged";
#endif
	updateItemContents( rootItem );
}

/*****************************************************
**
**   DasaTree   ---   doLayout
**
******************************************************/
MComplex DasaTree::doLayout( Painter* )
{
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::doLayout treesize " << treesize;
#endif

	writeItemText( rootItem );
	const double y = calculateItemCoordinates( rootItem, 0.0 );
	treesize = MComplex( std::min( rect.width, ITEM_MINWIDTH ), y );
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::doLayout treesize " << treesize;
#endif
	return treesize;
}

/*****************************************************
**
**   DasaTree   ---  createFolder
**
******************************************************/
GraphicalDasa_ptr DasaTree::createFolder( const std::list<DasaId> &ids, const wxString &title )
{
	GraphicalDasa_ptr folderItem( new GraphicalDasa( DasaLevel::Folder, title ));
	folderItem->expanded = true;

	for( const DasaId id : ids )
	{
		GraphicalDasa_ptr item( new GraphicalDasa( experts[id]->createBlankRoot( experts[id], folderItem->dasa )));

		for( auto &d : experts[id]->getFirstLevel( props, item->dasa ))
		{
			item->children.push_back( GraphicalDasa_ptr( new GraphicalDasa( d )));
		}
		folderItem->children.push_back( item );
	}
	return folderItem;
}

/*****************************************************
**
**   DasaTree   ---  getItemRect
**
******************************************************/
MRect DasaTree::getItemRect( const DasaLevel &level, const double &y0, const double minwidth ) const
{
	const double height = std::max( ITEM_HEIGHT, props->getZoom() * ITEM_HEIGHT );
	return MRect( ( (int)level + 3 ) * ITEM_INDENT, y0, minwidth, height );
}

/*****************************************************
**
**   DasaTree   ---   getIconRect
**
******************************************************/
MRect DasaTree::getIconRect( GraphicalDasa_ptr item ) const
{
	assert( item );

	double ytop = item->rect.y;
	if ( props->getZoom() > 1 )
	{
		ytop += .25 * ( props->getZoom() * ITEM_HEIGHT );
	}
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::getIconRect item rect " << item->rect << " zoom " << props->getZoom() << " ytop " << ytop;;
#endif
	return getItemRect( item->getLevel(), ytop, ITEM_INDENT );
}

/*****************************************************
**
**   DasaTree   ---   calculateItemCoordinates
**
******************************************************/
double DasaTree::calculateItemCoordinates( GraphicalDasa_ptr item, const double &y0 )
{
	double y = y0;
	if ( isVisible( item ))
	{
		// Indent: tree root -3 plus icon = level + 4
		item->rect = getItemRect( item->getLevel() + 1, y );
		y += item->rect.height;;

		item->resetFormat();

		if ( item->expanded )
		{
			for( auto &subitem : item->children )
			{
				y = calculateItemCoordinates( subitem, y );
			}
		}
#ifdef DEBUG_DASATREE
		debuglog << "DasaTree::calculateItemCoordinates rect " << item->rect
			<< " y " << y << " text " << item->tf;
#endif
	}
	else // not to be shown, i.e. before birth
	{
		item->rect = MRect();
	}
	return y;
}

/*****************************************************
**
**   DasaTree   ---   writeItemText
**
******************************************************/
void DasaTree::writeItemText( GraphicalDasa_ptr item, const bool recursive )
{
	assert( item );
	//DasaExpert_ptr expert = item->getExpert();
	auto expert = item->getExpert();
	constexpr bool isLocaltime = true;

	if ( expert && item->getLevel() == DasaLevel::Root )
	{
		item->tf = DasaTool::get()->getDasaName( expert->getDasaId() );
		item->resetFormat();
	}
	else if ( expert && item->getLevel() >= DasaLevel::Mahadasa ) 
	{
		Horoscope_ptr horoscope = props->getRadixHoroscope();
		assert( horoscope );
		item->tf = expert->getDasaDescription( item->dasa,
		  astroconfig->getViewpointConfig( getViewpoint() ).planets,
			astroconfig->getViewpointConfig( getViewpoint() ).planetProps );

		if ( item->dasa->getEndJD() >= horoscope->getJD())
		{
			DateFormatter *fmt = DateFormatter::get();
			item->tf << " " << fmt->formatDate( std::max( horoscope->getJD(), item->dasa->getStartJD()), isLocaltime )
				<< " - " << fmt->formatDate( item->dasa->getEndJD(), isLocaltime );
		}
		else
		{
			// wrong entry, should be hidden
			item->tf << " --";
		}
		item->resetFormat();
	}
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::writeItemText level " << item->getLevel() << " text " << item->tf;
#endif

	if ( recursive && item->expanded )
	{
		for( auto &child : item->children )
		{
			writeItemText( child );
		}
	}
#if 0
	// dump only on first call
	if ( item && item->getLevel() == DASALEVEL_TREEROOT )
	{
		debuglog << "Start ITEM";
		dumpItem( rootItem );
		debuglog << "END ITEM";
	}
#endif
}

/*****************************************************
**
**   DasaTree   ---   updateItemContents
**
******************************************************/
void DasaTree::updateItemContents( GraphicalDasa_ptr item )
{
	assert( item );
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::updateItemContents level " << item->getLevel();
#endif

	if ( item->getLevel() == DasaLevel::TreeRoot 
		|| item->getLevel() == DasaLevel::Folder )
	{
		for( auto &child : item->children )
		{
			assert( child );
			updateItemContents( child );
		}
	}
	else if ( item->getLevel() == DasaLevel::Root )
	{
		if ( item->children.size() > 0 )
		{
			DasaExpert_ptr expert = item->getExpert();
			assert( expert );

			/*
			* The number of children of rootnode and number of Mahadasas may differ.
			*/
			auto dasas = expert->getFirstLevel( props, item->dasa );
			const uint dasaCount = dasas.size();
			const uint childrenCount = item->children.size();
			#ifdef DEBUG_DASATREE
				debuglog << "DasaTree::updateItemContents dasaCount " << dasaCount << " childrenCount " << childrenCount;
			#endif

			// append empty nodes
			if ( dasaCount > childrenCount )
			{
				#ifdef DEBUG_DASATREE
					debuglog << "DasaTree::updateItemContents append " << ( dasaCount - childrenCount ) << " children";
				#endif
				for( uint i = 0; i < dasaCount - childrenCount; i++ )
				{
					item->children.push_back( GraphicalDasa_ptr( new GraphicalDasa( Dasa_ptr( new Dasa( expert, item->dasa, DasaLevel::Mahadasa )))));
				}
			}
			// remove obsolete nodes
			else if ( dasaCount < childrenCount )
			{
				#ifdef DEBUG_DASATREE
					debuglog << "DasaTree::updateItemContents remove " << ( childrenCount - dasaCount ) << " children";
				#endif
				for( uint i = 0; i < childrenCount - dasaCount; i++ )
				{
					item->children.pop_back();
				}
			}
			assert( item->children.size() == dasaCount );

			auto iter = item->children.begin();
			for( auto &d : expert->getFirstLevel( props, item->dasa ))
			{
				iter->get()->dasa = d;
				iter = std::ranges::next( iter );
			}
		}
	}
	else if ( item->getLevel() > DasaLevel::Mahadasa )
	{
		if ( item->children.size() > 0 )
		{
			DasaExpert_ptr expert = item->getExpert();
			assert( expert );

			auto iter = item->children.begin();
			for( auto &d : expert->getNextLevel( props, item->dasa ))
			{
				assert( iter != item->children.end());
				iter->get()->dasa = d;
				iter = std::ranges::next( iter );
			}
		}
	}
	else
	{
		warnlog << "DasaTree::updateItemContents: supicious item " << item->getLevel() << " " << item->tf;
	}
}

/*****************************************************
**
**   DasaTree   ---   paintItem
**
******************************************************/
void DasaTree::paintItem( Painter *painter, GraphicalDasa_ptr item, const MRect &refreshRect )
{
	assert( item );
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::paintItem level " << item->getLevel() << " rect " << item->rect << " text " << item->tf;
#endif
	if ( ! isVisible( item )) return;

	painter->setTextColor( getSkinnedTextColor());

	//if ( true )
	if ( item->rect.intersects( refreshRect ))
	{
		DocHelper util( painter );
		const MRect iconRect = getIconRect( item );

		// mark current item
		if ( item == currentItem )
		{
			painter->setBrush( getSkinnedMarkedFieldBrush());
			painter->drawRectangle( MRect( iconRect.getRight(), item->rect.y, item->width, item->rect.height ));
			painter->setTransparentBrush();
		}

		painter->drawBitmap( ImageProvider::get()->getDasaTreeBitmap( item->getLevel(), item->expanded ), iconRect.x, iconRect.y );

		setGraphicFont( painter );

		// assume that item is correctly formatted if it hat lines
		if ( item->isFormatted())
		{
			util.drawTextItem( *item );
		}
		else
		{
			util.formatAndDrawTextItem( *item );
		}
	}
	else
	{
#ifdef DEBUG_DASATREE
		debuglog << "item with rect " << item->rect << " is outside refreshRect " << refreshRect;
#endif
	}

	// paint children
	if ( item->expanded )
	{
		for( auto &child : item->children )
		{
			if ( isVisible( child )) { paintItem( painter, child, refreshRect ); }
		}
	}
}

/*****************************************************
**
**   DasaTree   ---   doPaint
**
******************************************************/
void DasaTree::doPaint( Painter *painter, const MRect &refreshRect )
{
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::doPaint refreshRect " << refreshRect;
#endif
	StopWatch sw;

	const WidgetSkin &skin = getSkin();
	painter->setPen( skin.pen );
	painter->setTextColor( skin.textColor );
	painter->setTransparentBrush();

	paintItem( painter, rootItem, refreshRect );

	infolog << "FINISHED DasaTree::doPaint " << sw.getElapsedMillisec() << " millisec";
}

/*****************************************************
**
**   DasaTree   ---   expandItem
**
******************************************************/
void DasaTree::expandItem( GraphicalDasa_ptr item, const bool doExpand )
{
	if ( ! item ) return;
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::expandItem level " << item->getLevel()
		<< " expanded " << item->expanded
		<< " children " << item->children.size();
#endif

	// calculate next level if item is to be expanded and no children are found
	if ( ! item->expanded
		&& item->getLevel() >= DasaLevel::Root 
		&& item->children.size() == 0 )
	{
		DasaExpert_ptr expert = item->getExpert();
		assert( expert );
		if ( item->getLevel() == DasaLevel::Root )
		{
			for( auto &d : expert->getFirstLevel( props, item->dasa ))
			{
				item->children.push_back( GraphicalDasa_ptr( new GraphicalDasa( d )));
			}
#ifdef DEBUG_DASATREE
			debuglog << "DasaTree::expandItem createMahadasa children size " << item->children.size();
#endif
		}
		else if ( item->getLevel() >= DasaLevel::Mahadasa )
		{
			for( auto &d : expert->getNextLevel( props, item->dasa ))
			{
				item->children.push_back( GraphicalDasa_ptr( new GraphicalDasa( d )));
			}
		}
		else
		{
			assert( false );
		}
	}
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::expandItem finished, rect " << item->rect
		<< " children size " << item->children.size();
#endif

	item->expanded = doExpand;
	writeItemText( item );
	calculateItemCoordinates( rootItem, 0.0 );
}

/*****************************************************
**
**   DasaTree   ---   findItem
**
******************************************************/
GraphicalDasa_ptr DasaTree::findItem( GraphicalDasa_ptr item, const MComplex &point )
{
	if ( ! item || ! isVisible( item )) return nullptr;

#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::findItem " << item->tf << " rect " << item->rect << " point " << point;
#endif

	// calculate full rect containing text item and icon
	MRect iconRect = getIconRect( item );
	const MRect fullRect( iconRect.x, item->getRect().y, iconRect.width + item->getRect().width, item->getRect().height );

	//if ( item->rect.containsPoint( point ))
	if ( fullRect.containsPoint( point ))
	{
#ifdef DEBUG_DASATREE
		debuglog << "DasaTree::findItem found item " << item->tf;
#endif
		return item;
	}
	else
	{
#ifdef DEBUG_DASATREE
		debuglog << "DasaTree::findItem point " << point << " not in item " << item->tf;
#endif
	}
	if ( item->expanded )
	{
		for( auto &child : item->children )
		{
			GraphicalDasa_ptr subsubitem = findItem( child, point );
			if ( subsubitem ) return subsubitem;
		}
	}
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::findItem nothing found";
#endif
	return nullptr;
}

/*****************************************************
**
**   DasaTree   ---   handleMouseLeftEvent
**
******************************************************/
WidgetResponse DasaTree::handleMouseLeftEvent( const MComplex &p )
{
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::handleMouseLeftEvent " << p;
#endif
	WidgetResponse response( false );

	GraphicalDasa_ptr item = findItem( rootItem, p );
	if ( item )
	{
#ifdef DEBUG_DASATREE
		debuglog << "DasaTree::handleMouseLeftEvent found item " << item->tf << " rect " << item->getRect()
			<< " icon rect " << getIconRect( item );
#endif

		// Problem mit Icon rect: das ist außerhalb des items, d.h. nicht teil von findItem

		// click into icon area toggles expand status
		if ( getIconRect( item ).containsPoint( p ))
		{
			expandItem( item, !item->expanded );
			//invalidateLayout();
			response.coordinatesChanged = true;
			currentItem = item;
			response.doRefresh = true;
			response.notifyParent = true;
		}
		// selection changed
		if ( item != currentItem )
		{
			response.refreshRect = item->rect;
			if ( currentItem ) response.refreshRect = response.refreshRect.join( currentItem->rect );

			currentItem = item;
			response.doRefresh = true;
			response.notifyParent = true;
		}
	}
	else
	{
		// switch off selection
		if ( currentItem )
		{
			response.doRefresh = true;
			response.notifyParent = true;
			response.refreshRect = currentItem->rect;
			currentItem = nullptr;
		}
	}
	return response;
}

/*****************************************************
**
**   DasaTree   ---   handleLeftDoubleClickEvent
**
******************************************************/
WidgetResponse DasaTree::handleLeftDoubleClickEvent( const MComplex &p )
{
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::handleLeftDoubleClickEvent " << p;
#endif
	WidgetResponse response( false );
	GraphicalDasa_ptr item = findItem( rootItem, p );

	if ( item )
	{
#ifdef DEBUG_DASATREE
		debuglog << "DasaTree::handleLeftDoubleClickEvent found item " << item->tf;
#endif
		response.doRefresh = true;
		response.notifyParent = true;
		response.coordinatesChanged = true;

		expandItem( item, !item->expanded );
		currentItem = item;
		//invalidateLayout();
	}
	else
	{
		if ( currentItem )
		{
			response.notifyParent = true;
			currentItem = nullptr;
		}
	}
	return response;
}

/*****************************************************
**
**   DasaTree   ---   onResponsiveChartEvent
**
******************************************************/
#ifdef DEBUG_DASATREE
WidgetResponse DasaTree::onResponsiveChartEvent( const MComplex &p, const int &keymod )
{
	debuglog << "DasaTree::onResponsiveChartEvent p " << p << " keymod " << keymod;
#else
WidgetResponse DasaTree::onResponsiveChartEvent( const MComplex&, const int& )
{
#endif

	WidgetResponse response( false );
#ifdef DEBUG_DASATREE
	debuglog << "DasaTree::onResponsiveChartEvent doRefresh " << response.doRefresh
		<< " refreshRect " << response.refreshRect;
#endif
	return response;
}

/*****************************************************
**
**   DasaTree   ---   getSelectedJD
**
******************************************************/
double DasaTree::getSelectedJD() const
{
	if ( currentItem && currentItem->dasa->getDasaLord() != DLORD_NONE )
	{
		return currentItem->dasa->getStartJD();
	}
	else return 0;
}

/*****************************************************
**
**   DasaTree   ---   isVisible
**
******************************************************/
bool DasaTree::isVisible( GraphicalDasa_ptr gd )
{
	// show root and folder items
	if ( ! gd ) return true;
	if ( ! gd->getExpert() ) return true;
	if ( ! gd->dasa->getEndJD() ) return true;

	Horoscope_ptr horoscope = props->getRadixHoroscope();
	if ( ! horoscope ) return true;

	// ist leer, wohl weil root dasa rausfällt
	return gd->dasa->getEndJD() >= horoscope->getJD();
}

/*****************************************************
**
**   DasaTree   ---   getFullTitle
**
******************************************************/
wxString DasaTree::getFullTitle( GraphicalDasa_ptr dasa ) const
{
	assert( dasa );
	DasaExpert_ptr expert = dasa->getExpert();
	wxString s;
	DasaTool *tool = DasaTool::get();

	switch( dasa->getLevel() )
	{
		case DasaLevel::TreeRoot:
		case DasaLevel::Folder:
			s << dasa->tf.toWxString();
		break;
		case DasaLevel::Root:
			assert( expert );
			s << tool->getDasaName( expert->getDasaId()); // << " " << tr( "Dasa" );
		break;
		case DasaLevel::Mahadasa:
			assert( expert );
			s << tool->getDasaLevelName( DasaLevel::Mahadasa ) << ": " << tool->getDasaName( expert->getDasaId());
		break;
		case DasaLevel::Antardasa:
		default:
			assert( expert );
			s << tool->getDasaLevelName( dasa->getLevel() )
				<< ": " << tool->getDasaName( expert->getDasaId());
		break;
	}
	return s;
}

/*****************************************************
**
**   DasaTree   ---   dumpItem
**
******************************************************/
void DasaTree::dumpItem( GraphicalDasa_ptr item )
{
	if ( ! item )
	{
		mcout << "[None]" << std::endl;
		return;
	}
	if ( item->expanded ) { mcout << "E"; }
	else { mcout << "C"; }
	if ( isVisible( item )) { mcout << "V"; }
	else { mcout << "H"; }

	for( int i = -3; i < (int)item->getLevel(); i++ )
	{
		mcout << "	";
	}
	mcout << "R=" << item->rect << " ";
	if ( item->tf.isEmpty())
	{
		mcout << "[empty: " << item->dasa->getStartJD() << "/" 
			<< item->dasa->getEndJD() << "/"
			<< (int)std::get<PlanetId>( item->dasa->getDasaLord())<< "]";
	}
	else { mcout << "#" << item->tf << "#"; }

	mcout << std::endl;
	for( auto &child : item->children ) { dumpItem( child ); }
}


