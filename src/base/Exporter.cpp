/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Exporter.cpp
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

#include "Exporter.h"

#include "astroconfig.h"
#include "logger.h"
#include "MDocument.h"
#include "Table.h"

extern Astroconfig *astroconfig;

/*****************************************************
**
**   Exporter   ---   convertMString
**
******************************************************/
wxString Exporter::convertMString( const MString &s ) const
{
	mstringstream ms;
	ms << s;
	//ms << s.debugDump();
	return ms.str();
}

/*************************************************//**
*
*    CLASS    ExporterText
*
******************************************************/
class ExporterText : public Exporter
{
public:

ExporterText() : Exporter() {}

virtual wxString exportDocument( MDocument_ptr doc ) override
{
	mstringstream s;
	for( auto item : doc->items )
	{
		s << exportItem( item.get() ) ;
		//debuglog << "Exporter::exportDocument " << s.str();
	}
	return s.str();
}

protected:

wxString exportItem( DocItem* item )
{
	mstringstream s;
	switch( item->type )
	{
		case DocItemType::Table:
		{
			Table *table = dynamic_cast<Table*>( item );
			s << generateTable( table );
		}
		break;
		case DocItemType::WidgetGrid:
		{
			s << "WARN: widgets not supported in text views" ;
		}
		break;
		case DocItemType::Text:
		{
			DocItemText *ti = dynamic_cast<DocItemText*>( item );
			switch( ti->subType )
			{
				case TextItemSubType::Header1:
				{
					const wxString t = convertMString( ti->tf );
					s << std::endl << t << std::endl;
					for ( uint i = 0; i < t.size(); i++ ) s << "=";
					s << std::endl;
				}
				break;
				case TextItemSubType::Header2:
				{
					const wxString t = convertMString( ti->tf );
					s << std::endl << t << std::endl;
					for ( uint i = 0; i < t.size(); i++ ) s << "-";
					s << std::endl;
				}
				break;
				case TextItemSubType::Paragraph:
				case TextItemSubType::Line:
					s << ti->tf << std::endl;
				break;
				default:
					errorlog << "invalid text item subType " << static_cast<int>( ti->subType ) ;
					assert( false );
				break;
			}
		}
		break;
		default:
		{
			s << "invalid document item type " << static_cast<int>( item->type ) << std::endl;
			//assert( false );
		}
		break;
	}
	return s.str();
}

/*****************************************************
**
**   ExporterText   ---   generateTable
**
******************************************************/
virtual wxString generateTable( Table *table )
{
	uint i, c, r;

	// total width
	//uint w_total = 0;

	// width of each column
	std::vector<uint> w_col( table->getNbCols(), 0 );
	wxString tt;
	mstringstream s;

	// write table header if necessary
	if ( ! table->title.isEmpty() )
	{
		tt = convertMString( table->title.tf );
		s << tt << std::endl;
		for ( i = 0; i < tt.size(); i++ ) s << "-";
		s << std::endl;
	}

	// table contents is mstring and must be transformed to plain text
	std::vector< std::vector<wxString> > plaintext( table->getNbCols() );
	for ( c = 0; c < table->getNbCols(); c++ )
	{
		plaintext[c] = std::vector<wxString>( table->getNbRows() );
		for ( r = 0; r < table->getNbRows(); r++ )
		{
			plaintext[c][r] = convertMString( table->getTextItem( c, r ).tf );
			w_col[c] = std::max( w_col[c], (uint)plaintext[c][r].size());
		}
	}

	for ( c = 0; c < table->getNbCols(); c++ )
	{
		w_col[c] += 1;
		//w_total += w_col[c];
		//infolog << "COL width " << c << " now " << w_col[c] ;
	}

	// write contents
	for ( r = 0; r < table->getNbRows(); r++ )
	{
		for ( c = 0; c < table->getNbCols(); c++ )
		{
			// does not work correctly for special signs like degree, minute, second
			//s << std::setw( w_col[c] ) << plaintext[c][r];

			for ( uint i = 0; i < w_col[c] - plaintext[c][r].size(); i++ )
			{
				s << " ";
			}
			s << plaintext[c][r];
			if ( c < table->getNbCols() -1 ) s << "|";
		}
		s << std::endl;

		// unterline first row if header
		if ( r == 0 && table->isHeaderRow( 0 ))
		{
			for ( c = 0; c < table->getNbCols(); c++ )
			{
				for( uint i = 0; i < w_col[c]; ++i ) s << "-";
				if ( c < table->getNbCols() -1 ) s << "|";
			}
			s << std::endl;
		}
	}
	s << std::endl;
	return s.str();
}
};

/*************************************************//**
*
*    CLASS    ExporterCsv
*
******************************************************/
class ExporterCsv : public ExporterText
{
public:

ExporterCsv() : ExporterText() {}

protected:

/*****************************************************
**
**   ExporterCsv   ---   generateTable
**
******************************************************/
virtual wxString generateTable( Table *table ) override
{
	uint c, r;
	mstringstream s;

	// write table header if necessary
	if ( ! table->title.isEmpty() )
	{
		s << table->title;
	}

	for ( r = 0; r < table->getNbRows(); r++ )
	{
		for ( c = 0; c < table->getNbCols(); c++ )
		{
			s << table->getTextItem( c, r ).tf;
			if ( c < table->getNbCols() - 1 ) s << CSV_DELIMITER;
		}
		s << std::endl;
	}
	s << std::endl;
	return s.str();
}
};

/*****************************************************
**
**   ExporterFactory   ---   getExporter
**
******************************************************/
Exporter *ExporterFactory::getExporter( const WidgetExportType &type )
{
	debuglog << "ExporterFactory::getExporter type " << (int)type;
	switch( type )
	{
		case WidgetExportType::Text:
			return new ExporterText();
		break;
		case WidgetExportType::Html:
			return getHtmlExporter();
		break;
		case WidgetExportType::PlainHtml:
			return getPlainHtmlExporter();
		break;
		case WidgetExportType::Csv:
			return new ExporterCsv();
		break;
		default:
			assert( false );
		break;
	}
	return nullptr;
}



