/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Table.h
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

#ifndef _TABLE_H_
#define _TABLE_H_

#include "MDocument.h"

#include <vector>
#include <wx/string.h>

/*************************************************//**
*
* 
*
******************************************************/
class TableRow : public DocItem
{
	friend class Table;
public:

	TableRow( const uint nb_cols = 0,  const bool isHeader = false );

	TableRow( const std::vector<wxString>&, const bool isHeader = false );

	TableRow( const std::vector<MString>&, const bool isHeader = false );

	void addValue( const wxString&, const bool isHeader = false );

	void addValue( const MString&, const bool isHeader = false );

	void addInteger( const int&, const bool isHeader = false );

	// add n-empty colums
	void addValue( const int n = 1, const bool isHeader = false );

	void resetRectangles();

	DocItemText &getTextItem( const unsigned& );

	bool isItemEmpty( const uint &col ) const { return value[col].tf.isEmpty(); }

	bool isHeaderRow() const;

	bool isHeader( const uint& ) const;

	void setValue( const unsigned&, const wxString&, const bool isHeader = false, const int span = 1 );

	void setValue( const unsigned&, const MString&, const bool isHeader = false, const int span = 1 );

	MString getValue( const unsigned& ) const;

	void setHeader( const uint &col, const bool& );

	bool isEmpty();

	uint getSize() const { return value.size(); }

	void resize( const uint &new_cols );

	virtual void translate( const double &x0, const double &y0 ) override;

private:
	std::vector<DocItemText> value;
};

/*************************************************//**
*
*
*
******************************************************/
struct TableFormat
{
	std::vector<double> col_width;
	std::vector<double> row_height;
	std::vector<Align> col_alignment;
  std::vector<bool> col_break;

	// all cols and rows have same size
	bool uniformLayout; 

	bool showVerticalBorders = true;
	bool showHorizontalBorders = true;
	bool showEmptyColumns = true;

	bool expand = false;
};

/*************************************************//**
*
*
*
******************************************************/
class Table : public DocItem
{
public:

	Table( const wxString title = "" );

	Table( const uint &cols, const uint rows = 0, const wxString title = "" );
	Table( const uint &cols, const uint &rows, const MString &title );

	// title of the table
	void setTitle( const wxString &s ) { setTitle( MString( s )); }
	void setTitle( const MString& );

	// header of specific cols or cells
	void setHeader( const uint &col, const wxString&, const int span = 1 );
	void setHeader( const uint &col, const MString&, const int span = 1 );
	void setHeaderEntry( const uint &col, const uint &row, const wxString&, const int span = 1 );
	void setHeaderEntry( const uint &col, const uint &row, const MString&, const int span = 1 );

	// all items of the row will be set to type TextItemSubType::TableHeader
	void setAllHeaderRow( const uint &row );

	void setEntry( const uint &col, const uint &row, const wxString&, const bool isHeader = false, const int span = 1 );
	//void setEntry( const uint &col, const uint &row, const MToken&, const bool isHeader = false );
	void setEntry( const uint &col, const uint &row, const MString&, const bool isHeader = false, const int span = 1 );
	MString getEntry( const uint &col, const uint &row ) const;

	DocItemText &getTextItem( const uint &col, const uint &row );
	bool isHeader( const uint &col, const uint &row ) const;
	bool isHeaderRow( const uint &row ) const;

	bool isEmptyCell( const uint &col, const uint &row );
	bool isEmptyHeader( const uint &col, const uint &row );

	virtual void translate( const double &x0, const double &y0 ) override;

	void resize( const uint &ols, const uint rows );

	std::vector<TableRow> contents;

	void addRow( TableRow& );
	void addRow( const std::list<MString>&, const bool isHeaderRow = false );
	void addRow( const std::list<wxString>&, const bool isHeaderRow = false );

	void addHeaderRow( const std::list<wxString> &l ) { addRow( l, true ); }
	void addHeaderRow( const std::list<MString> &l ) { addRow( l, true ); }
	void addEmptyRow( const bool isHeader = false );

	void resetRectangles();
	void setCenterAll();

	uint getNbRows() const { return contents.size(); }
	uint getNbCols() const { return nb_cols; }

	DocItemText title;

	bool isEmptyCol( uint &c ) { return col_empty[c]; }
	bool isEmptyRow( uint &r ) { return contents[r].isEmpty(); }

	TableFormat format;

	// set cells to be skipped if covered by a spanned cell
	void calculateSpans();

private:

	void init( const uint &rows );

	uint nb_cols = 0;

	std::vector<bool> col_empty;

public:
	int errorcount = 0;
};

#endif


