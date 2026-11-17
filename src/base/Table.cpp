/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Table.cpp
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

#include "Table.h"
#include "logger.h"

#include <format>

#define DEBUG_TABLE

/*****************************************************
**
**   TableRow   ---   Constructor
**
******************************************************/
TableRow::TableRow( const uint cols, const bool isHeader ) : DocItem( DocItemType::TableRow )
{
	for( uint c = 0; c < cols; c++ )
	{
		value.push_back( DocItemText( isHeader ? TextItemSubType::TableHeader : TextItemSubType::TableCell ));
	}
}

/*****************************************************
**
**   TableRow   ---   Constructor
**
******************************************************/
TableRow::TableRow( const std::vector<wxString> &l, const bool isHeader ) : DocItem( DocItemType::TableRow )
{
	for( const wxString &s : l )
	{
		value.push_back( DocItemText( isHeader ? TextItemSubType::TableHeader : TextItemSubType::TableCell, s ));
	}
}

/*****************************************************
**
**   TableRow   ---   Constructor
**
******************************************************/
TableRow::TableRow( const std::vector<MString> &l, const bool isHeader ) : DocItem( DocItemType::TableRow )
{
	for( const MString &s : l )
	{
		value.push_back( DocItemText( isHeader ? TextItemSubType::TableHeader : TextItemSubType::TableCell, s ));
	}
}

/*****************************************************
**
**   TableRow   ---   translate
**
******************************************************/
void TableRow::translate( const double &x0, const double &y0 )
{
	for( DocItemText &item : value )
	{
		item.translate( x0, y0 );
	}
	rect.translate( x0, x0 );
}

/*****************************************************
**
**   TableRow   ---   getTextItem
**
******************************************************/
DocItemText &TableRow::getTextItem( const uint & i )
{
	assert( i < value.size());
	return value[i];
}

/*****************************************************
**
**   TableRow   ---   setValue
**
******************************************************/
void TableRow::setValue( const unsigned &i, const wxString &s, const bool isHeader, const int span )
{
	assert( i < value.size());
	value[i].tf = s;
	value[i].subType = isHeader ? TextItemSubType::TableHeader : TextItemSubType::TableCell;
	value[i].tableSpan = span;
}

/*****************************************************
**
**   TableRow   ---   setValue
**
******************************************************/
void TableRow::setValue( const unsigned &col, const MString &s, const bool isHeader, const int span )
{
	assert( col < value.size());
	value[col].tf = s;
	value[col].subType = isHeader ? TextItemSubType::TableHeader : TextItemSubType::TableCell;
	value[col].tableSpan = span;
}

/*****************************************************
**
**   TableRow   ---   getValue
**
******************************************************/
MString TableRow::getValue( const unsigned &col ) const
{
	assert( col < value.size());
	return value[col].tf;
}

/*****************************************************
**
**   TableRow   ---   isHeaderRow
**
******************************************************/
bool TableRow::isHeaderRow() const
{
	for( const DocItemText &item : value )
	{
		if ( item.subType != TextItemSubType::TableHeader ) return false;
	}
	return true;
}

/*****************************************************
**
**   TableRow   ---   isHeader
**
******************************************************/
bool TableRow::isHeader( const uint &col ) const
{
	assert( col < value.size() );
	return value[col].subType == TextItemSubType::TableHeader;
}

/*****************************************************
**
**   TableRow   ---   setHeader
**
******************************************************/
void TableRow::setHeader( const uint &col, const bool &b )
{
	assert( col < value.size() );
	value[col].subType = b ? TextItemSubType::TableHeader : TextItemSubType::TableCell;
}

/*****************************************************
**
**   TableRow   ---   isEmpty
**
******************************************************/
bool TableRow::isEmpty()
{
	for ( uint i = 0; i < value.size(); i++ )
	{
		if ( ! value[i].tf.isEmpty() || value[i].subType == TextItemSubType::TableHeader ) return false;
	}
	return true;
}

/*****************************************************
**
**   TableRow   ---   addValue
**
******************************************************/
void TableRow::addValue( const wxString &s, const bool isHeader )
{
	value.emplace_back( isHeader ? TextItemSubType::TableHeader : TextItemSubType::TableCell, MString( s ));
}

/*****************************************************
**
**   TableRow   ---   addInteger
**
******************************************************/
void TableRow::addInteger( const int &i, const bool isHeader )
{
	value.emplace_back( isHeader ? TextItemSubType::TableHeader : TextItemSubType::TableCell, MString( std::format( "{}", i )));
}

/*****************************************************
**
**   TableRow   ---   addValue
**
******************************************************/
void TableRow::addValue( const MString &s, const bool isHeader )
{
	value.emplace_back( isHeader ? TextItemSubType::TableHeader : TextItemSubType::TableCell, s );
}

/*****************************************************
**
**   TableRow   ---   addValue
**
******************************************************/
void TableRow::addValue( const int v, const bool isHeader )
{
	for( int i = 0; i < v; i++ )
		value.push_back( DocItemText( isHeader ? TextItemSubType::TableHeader : TextItemSubType::TableCell, "" ));
}

/*****************************************************
**
**   TableRow   ---  resize
**
******************************************************/
void TableRow::resize( const uint &new_cols )
{
#ifdef DEBUG_TABLE
	debuglog << "TableRow::resize size " << value.size() << " new size " << new_cols;
#endif
	if ( new_cols >= value.size())
	{
		for( uint i = value.size(); i <= new_cols; i++ )
			value.push_back( DocItemText( TextItemSubType::TableCell, "" ));
	}
}

/*****************************************************
**
**   TableRow   ---  resetRectangles
**
******************************************************/
void TableRow::resetRectangles()
{
	rect.reset();
	for( uint i = 0; i < value.size(); i++ )
	{
		value[i].rect.reset();
	}
}

/*****************************************************
**
**   Table   ---   Constructor
**
******************************************************/
Table::Table( const wxString title )
	: DocItem( DocItemType::Table ),
	title( TextItemSubType::Header2, title )
{
}

/*****************************************************
**
**   Table   ---   Constructor
**
******************************************************/
Table::Table( const uint &cols, const uint rows, const wxString title )
	: DocItem( DocItemType::Table ),
	title( TextItemSubType::Header2, title ),
	nb_cols( cols )
{
	init( rows );
}

/*****************************************************
**
**   Table   ---   Constructor
**
******************************************************/
Table::Table( const uint &cols, const uint &rows, const MString &title )
	: DocItem( DocItemType::Table ),
	title( TextItemSubType::Header2, title ),
	nb_cols( cols )
{
	init( rows );
}

/*****************************************************
**
**   Table   ---   init
**
******************************************************/
void Table::init( const uint &rows )
{
	uint r, c;

	format.uniformLayout = false; 
	format.col_width.clear();
	format.row_height.clear();

	for ( c = 0; c < nb_cols; c++ )
	{
		col_empty.push_back( true );

		format.col_alignment.push_back( Align::West );
		format.col_break.push_back( false );
		format.col_width.push_back( 0 );
	}

	for ( r = 0; r < rows; r++ )
	{
		contents.push_back( TableRow( nb_cols ));
		format.row_height.push_back( 0 );
	}
}

/*****************************************************
**
**   Table   ---   resize
**
******************************************************/
void Table::resize( const uint &new_cols, const uint new_rows )
{
#ifdef DEBUG_TABLE
	debuglog << "Start Table::resize size cols " << nb_cols << " rows " << contents.size()
		<< " new size cols " << new_cols << " rows " << new_rows;
#endif
	if ( new_cols >= nb_cols )
	{
		for( auto &r : contents )
		{
			r.resize( new_cols );
			col_empty.push_back( false );

			format.col_alignment.push_back( Align::West );
			format.col_break.push_back( false );
			format.col_width.push_back( 0 );
		}
		nb_cols = new_cols + 1;
	}

	if ( new_rows >= contents.size())
	{
		for( uint r = contents.size(); r <= new_rows; r++ )
		{
			contents.push_back( TableRow( nb_cols ));
			format.row_height.push_back( 0 );
		}
	}
	
#ifdef DEBUG_TABLE
	debuglog << "Finished Table::resize size now cols " << nb_cols << " rows " << contents.size();
#endif
}

/*****************************************************
**
**   Table   ---   resetRectangles
**
******************************************************/
void Table::resetRectangles()
{
  rect.reset();
  title.rect.reset();
  for( uint c = 0; c < nb_cols; c++ )
  {
		format.col_width[c] = 0;
	}
  for( uint r = 0; r < contents.size(); r++ )
  {
    contents[r].resetRectangles();
  }
}

/*****************************************************
**
**   Table   ---   translate
**
******************************************************/
void Table::translate( const double &x0, const double &y0 )
{
	//debuglog << "Table::translate " << x0 << " " << y0;
	//debuglog << "TT rect1 " << rect;
	title.translate( x0, y0 );

	for( TableRow &row : contents )
	{
		row.translate( x0, y0 );
	}
	rect.translate( x0, y0 );
	//debuglog << "TT rect2 " << rect;
}

/*****************************************************
**
**   Table   ---   setCenterAll
**
******************************************************/
void Table::setCenterAll()
{
	for ( uint col = 0; col < getNbCols(); col++ )
	{
		format.col_alignment[col] = Align::Center;
	}
}

/*****************************************************
**
**   Table   ---   setTitle
**
******************************************************/
void Table::setTitle( const MString &s )
{
	title.tf = s;
}

/*****************************************************
**
**   Table   ---   setHeader
**
******************************************************/
void Table::setHeader( const uint &col, const wxString &s1, const int span )
{
	contents[0].setValue( col, MString( s1 ), span );
	contents[0].setHeader( col, true );
}

/*****************************************************
**
**   Table   ---   setHeader
**
******************************************************/
void Table::setHeader( const uint &col, const MString &f, const int span )
{
	assert( contents.size() > 0 );
	assert( col < nb_cols );
	contents[0].setValue( col, f, span );
	contents[0].setHeader( col, true );
}

/*****************************************************
**
**   Table   ---   setAllHeaderRow
**
******************************************************/
void Table::setAllHeaderRow( const uint &row )
{
	if ( row >= contents.size())
	{
		return;
	}
	for( uint col = 0; col < nb_cols; col++ )
	{
		DocItemText &item = contents[ row ].getTextItem( col );
		item.subType = TextItemSubType::TableHeader;
	}
}

/*****************************************************
**
**   Table   ---   setHeaderEntry
**
******************************************************/
void Table::setHeaderEntry( const uint &col, const uint &row, const wxString &s1, const int span )
{
	setEntry( col, row, s1, true, span );
}

/*****************************************************
**
**   Table   ---   setHeaderEntry
**
******************************************************/
void Table::setHeaderEntry( const uint &col, const uint &row, const MString &f, const int span )
{
	setEntry( col, row, f, true, span );
}

/*****************************************************
**
**   Table   ---   setEntry
**
******************************************************/
void Table::setEntry( const uint &col, const uint &row, const MString &f, const bool isHeader, const int span )
{
	if( col >= nb_cols || row >= contents.size())
	{
		resize( col, row );
	}
	assert( row < contents.size() );
	assert( col < nb_cols );

	if ( col_empty[col] &&  ! f.isEmpty() ) col_empty[col] = false;
	contents[row].setValue( col, f, isHeader, span );
}

/*****************************************************
**
**   Table   ---   setEntry
**
******************************************************/
void Table::setEntry( const uint &col, const uint &row, const wxString &s1, const bool isHeader, const int span )
{
	//debuglog << "Table::setEntry col " << col << " row " << row << " contents.size " << contents.size() << " nb_cols " << nb_cols ;
	if( col >= nb_cols || row >= contents.size())
	{
		resize( col, row );
	}
	assert( col < nb_cols );
	assert( row < contents.size() );

	if ( col_empty[col] &&  ! s1.empty() ) col_empty[col] = false;
	contents[row].setValue( col, s1, isHeader, span );
}

/*****************************************************
**
**   Table   ---   getEntry
**
******************************************************/
MString Table::getEntry( const uint &col, const uint &row ) const
{
	if( col >= nb_cols || row >= contents.size())
	{
		return MString( std::format( "wrong coordinates {} {}", col, row ));
	}
	assert( row < contents.size() );
	assert( col < nb_cols );
	return contents[row].getValue( col );
}

/*****************************************************
**
**   Table   ---   getTextItem
**
******************************************************/
DocItemText &Table::getTextItem( const uint &col, const uint &row )
{
	assert( row < contents.size() );
	assert( col < nb_cols );
	return contents[row].getTextItem( col );
}

/*****************************************************
**
**   Table   ---   isHeader
**
******************************************************/
bool Table::isHeader( const uint &col, const uint &row ) const
{
	if( col >= nb_cols || row >= contents.size())
	{
		return false;
	}
	assert( row < contents.size() );
	assert( col < nb_cols );
	return contents[row].isHeader( col );
}

/*****************************************************
**
**   Table   ---   isHeaderRow
**
******************************************************/
bool Table::isHeaderRow( const uint &row ) const
{
	if( row >= contents.size())
	{
		return false;
	}
	assert( row < contents.size() );
	return contents[row].isHeaderRow();
}

/*****************************************************
**
**   Table   ---   isEmptyCell
**
******************************************************/
bool Table::isEmptyCell( const uint &col, const uint &row )
{
	if( col >= nb_cols || row >= contents.size())
	{
		return false;
	}
	return getTextItem( col, row ).isEmpty();
}

/*****************************************************
**
**   Table   ---   isEmptyHeader
**
******************************************************/
bool Table::isEmptyHeader( const uint &col, const uint &row )
{
	if( col >= nb_cols || row >= contents.size())
	{
		return false;
	}
	return isHeader( col, row ) && getTextItem( col, row ).isEmpty();
}

/*****************************************************
**
**   Table   ---   addEmptyRow
**
******************************************************/
void Table::addEmptyRow( const bool isHeader )
{
	contents.emplace_back( nb_cols, isHeader );
}

/*****************************************************
**
**   Table   ---   addRow
**
******************************************************/
void Table::addRow( const std::list<MString> &l, const bool isHeaderRow )
{
	TableRow row;
  for( const MString &s : l ) row.addValue( s, isHeaderRow );
  addRow( row );
}

/*****************************************************
**
**   Table   ---   addRow
**
******************************************************/
void Table::addRow( const std::list<wxString> &l, const bool isHeaderRow )
{
	TableRow row;
  for( const wxString &s : l ) row.addValue( s, isHeaderRow );
  addRow( row );
}

/*****************************************************
**
**   Table   ---   addRow
**
******************************************************/
void Table::addRow( TableRow &r )
{
	// this is the first row
	if ( nb_cols == 0 && contents.size() == 0 )
	{
		nb_cols = r.getSize();
		init( 0 );
	}

	// resize this table
	if ( r.getSize() > nb_cols )
	{
		resize( r.getSize(), contents.size());
	}
	//resize the input row
	else if ( r.getSize() < nb_cols )
	{
		r.resize( nb_cols );
	}

	assert( r.getSize() == nb_cols );

	contents.push_back( r );
	for( uint c = 0; c < nb_cols; c++ )
	{
		if ( ! r.isItemEmpty( c ))
		{
			col_empty[c] = false;
			continue;
		}
	}
	format.row_height.push_back( 0 );
}

/*****************************************************
**
**   Table   ---   calculateSpans
**
******************************************************/
void Table::calculateSpans()
{
	for( auto &row : contents )
	{
		for( uint col = 0; col < nb_cols; col++ )
		{
			row.value[col].isSkipped = false;
		}
		for( uint col = 0; col < nb_cols; col++ )
		{
			if ( ! row.value[col].isSkipped  && row.value[col].tableSpan > 1 )
			{
				//for( uint i = 1; i < row.value[col].tableSpan; i++ )
				for( int i = 1; i < row.value[col].tableSpan; i++ )
				{
					if ( col + i < nb_cols )
					{
						row.value[col + i].isSkipped = true;
					}
				}
			}
		}
	}
}


