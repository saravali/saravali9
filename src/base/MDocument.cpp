/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/MDocument.cpp
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

#include "MDocument.h"

#include "astroconfig.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"

extern Astroconfig *astroconfig;

/*****************************************************
**
**   DocItem   ---   moveTo
**
******************************************************/
void DocItem::moveTo( const double &x0, const double &y0 )
{
	translate( x0 - rect.x, y0 - rect.y );
}

/*****************************************************
**
**   DocItem   ---   translate
**
******************************************************/
void DocItem::translate( const double &x0, const double &y0 )
{
	rect.translate( x0, y0 );
}

/*****************************************************
**
**   DocItem   ---   operator<<
**
******************************************************/
mostream& operator <<( mostream &o, const DocItem &item )
{
	o << " type{" << (int)item.type << "} rect{" << item.rect << "}";
	return o;
}

/*****************************************************
**
**   DocItemText   ---   translate
**
******************************************************/
void DocItemText::translate( const double &x0, const double &y0 )
{
	debuglog << "DocItemText::translate xy " << x0 << "/" << y0 << " rect " << rect << " tf " << tf;
	for( DocTextLine &line : lines )
	{
		line.rect.translate( x0, y0 );
	}
	rect.translate( x0, y0 );
}

/*****************************************************
**
**   DocItemText   ---   resetFormat
**
******************************************************/
void DocItemText::resetFormat()
{
	//rect.reset();
	lines.clear();
	tokenSize.clear();
	words.clear();
	spaceExtent = 0;
}

/*****************************************************
**
**   MDocument   ---   Destructor
**
******************************************************/
MDocument::~MDocument()
{
	clear();
}

/*****************************************************
**
**   MDocument   ---   clear
**
******************************************************/
void MDocument::clear()
{
	items.clear();
}

/*****************************************************
**
**   MDocument   ---   centerItems
**
******************************************************/
void MDocument::centerItems()
{
	double xrightmax = 0;
  for( auto item : items )
	{
		xrightmax = std::max( xrightmax, item->rect.width );
	}
  for( auto item : items )
	{
		item->rect.x += .5 * xrightmax - .5 * item->rect.width;
	}
}

/*****************************************************
**
**   MDocument   ---   translate
**
******************************************************/
/*
void MDocument::translate( const double &x0, const double &y0 )
{
  for( DocItem *item : items ) item->translate( x0, y0 );
}
*/

/*****************************************************
**
**   MDocument   ---   getPageCount
**
******************************************************/
int MDocument::getPageCount() const
{
	if ( items.size() )
	{
		return items.back()->pagenumber;
	}
	else return 1;
}

/*****************************************************
**
**   MDocument   ---   addItem * methods
**
******************************************************/
void MDocument::addItem( DocItem *item )
{
	items.push_back( DocItem_ptr( item ));
}
void MDocument::addHeader1( const wxString &s, const Align align )
	{ addItem( new DocItemText( TextItemSubType::Header1, s, align )); }
void MDocument::addHeader2( const wxString &s, const Align align )
	{ addItem( new DocItemText( TextItemSubType::Header2, s, align )); }
void MDocument::addParagraph( const wxString &s, const Align align )
	{ addItem( new DocItemText( TextItemSubType::Paragraph, s, align )); }
void MDocument::addLine( const wxString &s, const Align align )
	{ addItem( new DocItemText( TextItemSubType::Line, s, align )); }

void MDocument::addDefinitionLine( const wxString &s1, const wxString &s2, const Align align )
{
	const wxString s = s1 + ": " + s2;
	addItem( new DocItemText( TextItemSubType::Line, s, align ));
}

void MDocument::addHeader1( const MString &s, const Align align )
	{ addItem( new DocItemText( TextItemSubType::Header1, s, align )); }
void MDocument::addHeader2( const MString &s, const Align align )
	{ addItem( new DocItemText( TextItemSubType::Header2, s, align )); }
void MDocument::addParagraph( const MString &s, const Align align )
	{ addItem( new DocItemText( TextItemSubType::Paragraph, s, align )); }
void MDocument::addLine( const MString &s, const Align align )
	{ addItem( new DocItemText( TextItemSubType::Line, s, align )); }


