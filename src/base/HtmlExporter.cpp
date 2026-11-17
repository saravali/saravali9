/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/HtmlExporter.cpp
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

#include "ConfigListItems.h"
#include "ConfigLoaderImpls.h"
#include "gdiconfig.h"
#include "Lang.h"
#include "SymbolProvider.h"
#include "Table.h"

extern Gdiconfig *gdiconfig;

#define COLOR_FLAGS wxC2S_HTML_SYNTAX

//#define DEBUG_HTMLEXPORTER

/*************************************************//**
*
* \brief Converts from MDocument to plain HTML.
*
******************************************************/
class PlainHtmlExporter : public Exporter
{
public:

	PlainHtmlExporter() : Exporter() {}

	virtual wxString exportDocument( MDocument_ptr doc ) override
	{
		wxString s;
		s << writeHtmlHead();

		for( auto item : doc->items )
		{
			if ( item->type == DocItemType::Table )
			{
				Table *table = dynamic_cast<Table*>( item.get() );
				assert( table );
				s << generateTable( table );
			}
			else if ( item->type == DocItemType::WidgetGrid )
			{
				s << "<h1>WARN: widgets not supported in Html views</h1>\n";
			}
			else if ( item->type == DocItemType::Text )
			{
				DocItemText *ti = dynamic_cast<DocItemText*>( item.get() );
				assert( ti );
				wxString t = convertToHtml( *ti );

				switch( ti->subType )
				{
					case TextItemSubType::Header1:
						s << "<h1>" << t << "</h1>\n";
					break;
					case TextItemSubType::Header2:
						s << "<h2>" << t << "</h2>\n";
					break;
					case TextItemSubType::Paragraph:
						s << t << "<p>" ;
					break;
					case TextItemSubType::Line:
						s << t << "<br>\n";
					break;
					case TextItemSubType::Footnote:
						s << "<small>" << t << "</small>" << "<br>\n";
					break;
					default:
						assert( false );
					break;
				}
			}
			else assert( false );
		}

		s << "</body>\n";
		s << "</html>\n";
		return s;
	}

protected:

	/*****************************************************
	**
	**   PlainHtmlExporter   ---   writeHtmlHead
	**
	******************************************************/
	virtual wxString writeHtmlHead()
	{
		wxString s;
		s << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//en\">\n"
			<< "<html>\n"
			<< "<head>\n"
			<< "<meta charset=\"utf-8\"/>\n"
			<< "</head>\n"
			<< "<body>\n";
		return s;
	}

	/*****************************************************
	**
	**   PlainHtmlExporter   ---   generateTable
	**
	******************************************************/
	virtual wxString generateTable( Table *table )
	{
		wxString s;
		if ( ! table->title.isEmpty())
		{
			s << "<h1>" << convertToHtml( table->title ) << "</h1>\n";
		}

		uint c, r;
		s << "<table>\n>";

		for ( r = 0; r < (unsigned)table->getNbRows(); r++ )
		{
			if ( table->isEmptyRow( r )) continue;

			s << "<tr>\n";
			for ( c = 0; c < table->getNbCols(); c++ )
			{
				if ( table->isHeader( c, r ))
				{
					s << "<th>" << convertToHtml( table->getTextItem( c, r )) << "</th>\n";
				}
				else if ( table->isEmptyCol( c ))
				{
					s << "<td width=10></td>\n";
				}
				else
				{
					s << "<td>" << convertToHtml( table->getTextItem( c, r ))  << "</td>\n";
				}
			}
			s << "</tr>\n";
		}
		s << "</table><p>\n";
		return s;
	}

	/*****************************************************
	**
	**   PlainHtmlExporter   ---   convertToHtml
	**
	******************************************************/
	wxString convertToHtml( const DocItemText &item ) const
	{
		wxString s;
		for( const MToken &token : item.tf.tokens )
		{
			s << convertTokenToHtml( token );
		}
		return s;
	}

	/*****************************************************
	**
	**   PlainHtmlExporter   ---   convertTokenToHtml
	**
	******************************************************/
	wxString convertTokenToHtml( const MToken &token ) const
	{
		wxString m;
		wxString ret;
#ifdef DEBUG_HTMLEXPORTER
		debuglog << "PlainHtmlExporter::convertTokenToHtml " << token << " token id " << token.tokenId;
#endif

		wxString contents = token.contents;
		contents.Replace( "<", "&lt;" );
		contents.Replace( ">", "&gt;" );

		switch( token.tokenId )
		{
			case TokenId::Symbol:
				m << "<font face='" << SYMBOL_FONT_NAME << "'>" << contents << "</font>";
			break;
			case TokenId::NoBreakSpace:
				m << "&nbsp;";
			break;
			case TokenId::DegreeSymbol:
				m << "&deg;";
			break;
			case TokenId::MinuteSymbol:
				m << "&apos;";
			break;
			case TokenId::SecondSymbol:
				m << "&quot;";
			break;
			case TokenId::Space:
				m << " ";
			break;
			default:
				m << contents;
				//m << token.contents;
			break;
		}

		switch( token.fontFormat )
		{
			case TokenFontPosition::Subscriptum:
				ret << "<sub>" << m << "</sub>";
				return ret;
			break;
			case TokenFontPosition::Superscriptum:
				ret << "<sup>" << m << "</sup>";
				return ret;
			break;
			default:
				return m;
			break;
		}
		return "[unreachable code]";
	}

	/*****************************************************
	**
	**   PlainHtmlExporter   ---   escapeHtmlSymbol
	**
	******************************************************/
	wxString escapeHtmlSymbol( char t ) const
	{
		wxString s;
		s << "<font face='" << SYMBOL_FONT_NAME << "'>" << t << "</font>";
		return s;
	}
};


/*****************************************************
**
**   ExporterFactory   ---   getPlainHtmlExporter
**
******************************************************/
Exporter *ExporterFactory::getPlainHtmlExporter()
{
	return new PlainHtmlExporter();
}

/*************************************************//**
*
* \brief Converts from MDocument to formatted HTML.
*
******************************************************/
class HtmlExporter : public PlainHtmlExporter
{
public:

	HtmlExporter() : PlainHtmlExporter() {}

protected:

	/*****************************************************
	**
	**   HtmlExporter   ---   writeHtmlHead
	**
	******************************************************/
	virtual wxString writeHtmlHead() override
	{
		if ( ! colorcfg ) colorcfg = &gdiconfig->colors;
		wxString s;
		s << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//en\">\n" 
			<< "<html>\n"
			<< "<head/>\n"
			<< "<meta charset=\"utf-8\"/>\n"
			<< "<body "
			<< " bgcolor=" << colorcfg->bgColor.GetAsString( COLOR_FLAGS )
			<< " text=" << colorcfg->textFgColor.GetAsString( COLOR_FLAGS )
			<< ">\n";
		return s;
	}

	/*****************************************************
	**
	**   HtmlExporter   ---   generateTable
	**
	******************************************************/
	virtual wxString generateTable( Table *table ) override
	{
		if ( ! tableStyle )
		{
			tableStyle = &TableStyleLoader::get()->getConfig( gdiconfig->tableStyleId );

		}
#ifdef DEBUG_HTMLEXPORTER
		debuglog << "HtmlExporter::generateTable useheadercolors " << tableStyle->useHeaderColors 
		<< " headerTextColor " << tableStyle->headerTextColor.GetAsString( COLOR_FLAGS );
#endif

		wxString s;
		if ( ! table->title.isEmpty())
		{
			s << "<h1>" << convertToHtml( table->title ) << "</h1>\n";
		}

		if ( tableStyle->showGrid ) s << "<table border=1 cellspacing=0>\n";
		else s << "<table>\n";

		for ( uint r = 0; r < (uint)table->getNbRows(); r++ )
		{
			if ( table->isEmptyRow( r )) continue;

			const bool headerRow = table->isHeaderRow( r );
			wxColour textColor;
			wxColour cellColor;

			if ( headerRow )
			{
				if ( tableStyle->useHeaderColors )
				{
					s << "<tr bgcolor=\"" << tableStyle->headerBgColor.GetAsString( COLOR_FLAGS ) << "\">";
					textColor = tableStyle->headerTextColor.GetAsString( COLOR_FLAGS );
				}
				else
				{
					s << "<tr>";
				}
			}
			else
			{
				s << "	<tr";
				if ( tableStyle->useCellColors )
				{
					switch( tableStyle->cellBgMode )
					{
						case 0: // transparent
							cellColor = wxColour();
						break;
						case 1:
							cellColor = tableStyle->allCellBgColor;
						break;
						case 2:
						if ( r % 2 )
								cellColor = tableStyle->oddCellBgColor;
							else
								cellColor = tableStyle->evenCellBgColor;
						break;
						default:
						break;
					}
					if ( cellColor.IsOk())
						s << " bgcolor=\"" << cellColor.GetAsString( COLOR_FLAGS ) << "\"";
					textColor = tableStyle->cellTextColor.GetAsString( COLOR_FLAGS );
				}
				s << ">\n";
			}
			for ( uint c = 0; c < table->getNbCols(); c++ )
			{
				wxColour fgColor;

				// there are single header entries 
				if ( table->isHeader( c, r ) && tableStyle->useHeaderColors )
				{
					fgColor = tableStyle->headerTextColor;
					cellColor = tableStyle->headerBgColor;
				}
				else if ( tableStyle->useCellColors )
				{
					fgColor = textColor;
				}

				if ( table->isHeader( c, r )) s << "\n		<th";
				else s << "\n		<td";

				if ( ! table->format.col_break[c] ) s << " nowrap";

				// set background explicitely for single header entries
				if ( !headerRow && table->isHeader( c, r ) && tableStyle->useHeaderColors )
				{
					s << " bgcolor=\"" << tableStyle->headerBgColor.GetAsString( COLOR_FLAGS ) << "\"";
				}
				s << ">";

				if ( fgColor.IsOk())
				{
					s << "<font color=\"" << fgColor.GetAsString( COLOR_FLAGS ) << "\">"
						<< convertToHtml( table->getTextItem( c, r ))
						<< "</font>";
				}
				else
				{
					s << convertToHtml( table->getTextItem( c, r ));
				}
				//s << convertToHtml( table->getTextItem( c, r ));
				if ( table->isHeader( c, r )) s << "</th>";
				else s << "</td>";
			}

			s << "\n	</tr>\n";
		}
		s << "</table><p>\n";
		return s;
	}

};

/*****************************************************
**
**   ExporterFactory   ---   getHtmlExporter
**
******************************************************/
Exporter *ExporterFactory::getHtmlExporter()
{
	return new HtmlExporter;
}



