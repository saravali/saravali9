/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/DasaTree.h
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

#ifndef _DASATREE_H_
#define _DASATREE_H_

#include "GdiWidget.h"
#include "GraphicalDasa.h"

#include <map>
#include <wx/bitmap.h>
#include <wx/string.h>

/*************************************************//**
*
*   CLASS   ---   DasaTree
*
******************************************************/
class DasaTree : public GdiWidget
{
public:

	DasaTree( ContextProps_ptr );

	virtual MComplex doLayout( Painter* ) override;

	virtual void doPaint( Painter*, const MRect &refreshRect ) override;

	virtual void OnDataChanged() override;

	virtual WidgetResponse onResponsiveChartEvent( const MComplex&, const int &keymod ) override;

	virtual WidgetResponse handleMouseLeftEvent( const MComplex& ) override;

	virtual WidgetResponse handleLeftDoubleClickEvent( const MComplex& ) override;

	/**
	* Returns a pointer to the selected item or null.
	*/
	GraphicalDasa_ptr getCurrentItem() const { return currentItem; }

	/**
	* Convenience method returns the JD of the selected item (or zero).
	*/
	double getSelectedJD() const;

	wxString getFullTitle( GraphicalDasa_ptr ) const;

private:

	GraphicalDasa_ptr createFolder( const std::list<DasaId>&, const wxString &title );

	/*
	* Expand or collapse an item.
	*
	* The children will be calculated on expand if not already there.
	*/
	void expandItem( GraphicalDasa_ptr, const bool doExpand = true );

	/**
	* Calculates rect coordinates of a dasa item.
	*
	* The method is recursive, so a call on root item will calculate all items.
	* Hidden items (end date before birth) are skipped.
	*/
	double calculateItemCoordinates( GraphicalDasa_ptr, const double &y0 );

	/** 
	* Write text to dasa items
	*
	*/
	void writeItemText( GraphicalDasa_ptr item, const bool recursive = true );

	/**
	* Finds the item matching a given point.
	*
	* Return is a pointer to the item or null.
	*/
	GraphicalDasa_ptr findItem( GraphicalDasa_ptr, const MComplex &p );

	/**
	* Updates the dasas and their text on chart data change.
	*/
	void updateItemContents( GraphicalDasa_ptr );

	/**
	* Decide whether to hide or show a specific item.
	*
	* Items are hidden if Dasa is (completely ) before birth.
	*/
	bool isVisible( GraphicalDasa_ptr );

	/**
	* Paints a dasa item and calls itself on the children.
	*
	* A call on root item paints all items.
	*/
	void paintItem( Painter*, GraphicalDasa_ptr, const MRect &refreshRect );

	/**
	* Calculates the icon coordinates (including indent).
	*/
	MRect getIconRect( GraphicalDasa_ptr ) const;

	MRect getItemRect( const DasaLevel &level, const double &y0, const double minwidth = ITEM_MINWIDTH ) const;

	/**
	* Dump tree to console.
	*/
	void dumpItem( GraphicalDasa_ptr );

	GraphicalDasa_ptr rootItem;

	GraphicalDasa_ptr currentItem;

	MComplex treesize;

	constexpr inline static double ITEM_MINWIDTH = 500.0;

	constexpr inline static double ITEM_INDENT  = 32.0;

	constexpr inline static double ITEM_HEIGHT  = 32.0;

	std::map<DasaId, DasaExpert_ptr> experts;
};

#endif // _DASATREE_H_

