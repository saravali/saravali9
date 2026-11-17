/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/MDocument.h
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

#ifndef _MDOCUMENT_H_
#define _MDOCUMENT_H_

#include "bootstrap.h"
#include "constants.h"
#include "gdibase.h"
#include "mstring.h"

#include <list>
#include <vector>

class Table;
class DocItemText;

enum class DocItemType
{
	// json:enum
	None = -1,
	Text,
	Table,
	TableRow,
	Cell,
	Chart,
	VargaChartGrid,
	DasaReport,
	WidgetGrid,
	Bar,
	PageBreak
};

enum class TextItemSubType
{
	// json:enum
	None = -1,
	Header1,
	Header2,
	Paragraph,
	Line,
	TableCell,
	TableHeader,
	Footnote,
	ErrorMessage
};

/*************************************************//**
*
*  
*
******************************************************/
class DocItem
{
public:
	DocItem( const DocItemType &type, const Align align = Align::Center )
	 : type( type ), align( align ) {}

	virtual ~DocItem() {}

	virtual void setRect( const MRect &r ) { rect = r; }

	void setRect( const double &x, const double &y, const double w, const double h ) { setRect( MRect( x, y, w, h )); }

	MRect getRect() const { return rect; }

	/*
	* move item to a specific point
	*/
	void moveTo( const double &x0, const double &y0 );

	void moveTo( const MComplex &p ) { moveTo( p.real(), p.imag()); }

	/*
	* translate item by the given values
	*/
	virtual void translate( const double &x0, const double &y0 );

	friend mostream& operator <<( mostream&, const DocItem& );

	DocItemType type;

	Align align;

	// first has number 0
	int pagenumber = -1;

	MRect rect;
};

/*************************************************//**
*
*   Helper class for DocItemText holding a set of MToken (without spaces) representing a word.
*
*  Tokens are references by their index.
*
******************************************************/
struct TokenWord
{
	TokenWord()
	{
		width = 0;
		height = 0;
	}

	void clear()
	{
		width = 0;
		height = 0;
		tokens.clear();
	}

	bool isEmpty() const { return tokens.size() == 0; }

	double width = 0;

	double height = 0;

	/**
	*  Indices of the MToken elements belonging the word.
	*/
	std::vector<uint> tokens;
};

/*************************************************//**
*
*   Helper class for DocItemText holding a set of TokenWord filling a line.
*
******************************************************/
struct DocTextLine
{
	DocTextLine()
	{
		rect = MRect();
	}

	void clear()
	{
		rect = MRect();
		words.clear();
	}

	bool isEmpty() const { return words.size() == 0; }

	/**
	*  bounding rectangle for the line.
	*/
	MRect rect;

	/**
	*  Vector holding the IDs of the TokenWord elements in the line.
	*
	*/
	std::vector<uint> words;
};

/*************************************************//**
*
*  This item holds formatted text.
*
******************************************************/
class DocItemText : public DocItem
{
public:

	DocItemText( const TextItemSubType st = TextItemSubType::None,
		const wxString t = "",
		const Align align = Align::West )
	 : DocItem( DocItemType::Text, align ), 
	 	subType( st ),
		tf( t )
	{
		width = height = 0;
	}

	DocItemText( const TextItemSubType &st, const MString &f, const Align align = Align::West )
	 : DocItem( DocItemType::Text, align ), subType( st ), tf( f ) {}

	DocItemText( const wxString &t, const MRect &rect, const Align align = Align::West )
	 : DocItem( DocItemType::Text, align ), subType( TextItemSubType::Line ), tf( t )
	{
		this->rect = rect;
	}

	DocItemText( const MString &f, const MRect &rect, const Align align = Align::West )
	 : DocItem( DocItemType::Text, align ), subType( TextItemSubType::Line ), tf( f )
	{
		this->rect = rect;
	}

	DocItemText( const MRect &rect, const Align align = Align::West )
	 : DocItem( DocItemType::Text, align ), subType( TextItemSubType::Line )
	{
		this->rect = rect;
	}

	DocItemText( const Align &align )
	 : DocItem( DocItemType::Text, align ), subType( TextItemSubType::Line )
	{
	}

	virtual void translate( const double &x0, const double &y0 ) override;

	bool isEmpty() const { return tf.isEmpty(); }

	bool isFormatted() const { return words.size() > 0; }

	void resetFormat();

	// Header, Paragraph, Line etc
	TextItemSubType subType;

	// this is the contents
	MString tf;

	// text item has own height and width. is used for determimation of text size resp. formatting in target widget
	double width = 0.0;

	double height = 0.0;

	// size of tokens after preformatting
	std::vector<MComplex> tokenSize;

	std::vector<TokenWord> words;

	std::list<DocTextLine> lines;

	MComplex spaceExtent;

	// See html table span. Only used for table cells.
	int tableSpan = 1;

	// Only for tables: cell is not shown because it is covered by a spanned cell
	bool isSkipped = false;
};

/*************************************************//**
*
*
*
******************************************************/
class DocItemChart : public DocItem
{
public:

	DocItemChart() : DocItem( DocItemType::Chart ) {}

	WidgetCategory category = WidgetCategory::None;

	Viewpoint viewpoint = Viewpoint::None;

	Varga varga = Varga::Rasi;

	// = 360 deg
	int division = 1;

	// width in percent
	double width = 100.0; 
};

/*************************************************//**
*
*
*
******************************************************/
class DocItemPageBreak : public DocItem
{
public:

	DocItemPageBreak( PageOrientation orientation = PageOrientation::Portrait )
		: DocItem( DocItemType::PageBreak ),
		orientation( orientation )
	{
	}

	PageOrientation orientation = PageOrientation::Portrait;
};

/*************************************************//**
*
*
*
******************************************************/
class DocItemVargaChartGrid : public DocItem
{
public:

	DocItemVargaChartGrid( const Viewpoint &viewpoint, const std::list<Varga> &vargas )
	 : DocItem( DocItemType::VargaChartGrid ), 
		viewpoint( viewpoint ),
		vargas( vargas )
	{
	}

	Viewpoint viewpoint = Viewpoint::None;

	uint cols = 2;

	std::list<Varga> vargas;
};

/*************************************************//**
*
* MDocument is a container for specific (generally unformatted) elements like
* - Text: header, lines, labels etc.
* - Tables
* - Inline widgets
* 
* Documents can be converted to standard text formats (like HTML or plain text) using a specific Exporter.
* This happens in most of the text based widgets of the GUI application.
* 
*
******************************************************/
class MDocument
{
public:

	~MDocument();

	bool isEmpty() { return items.size() == 0; }

	void clear();
	void addItem( DocItem* );

	void centerItems();
	//void translate( const double &x0, const double &y0 );

	int getPageCount() const;

	void addHeader1( const wxString&, const Align = Align::Center );
	void addHeader2( const wxString&, const Align = Align::West );
	void addParagraph( const wxString&, const Align = Align::West );
	void addLine( const wxString&, const Align = Align::West );

	void addHeader1( const MString&, const Align = Align::West );
	void addHeader2( const MString&, const Align = Align::West );
	void addParagraph( const MString&, const Align = Align::West );
	void addLine( const MString&, const Align = Align::West );
	void addDefinitionLine( const wxString&, const wxString&, const Align = Align::West );

	std::vector<DocItem_ptr> items;

	bool autoPageBreak = false;

	wxString header;
	wxString footer;
};

#endif // _MDOCUMENT_H_


