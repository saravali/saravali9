/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/PdfGenerator.cpp
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

#include "PdfGenerator.h"

#include "astroconfig.h"
#include "ChartGeometry.h"
#include "ConfigLoaderImpls.h"
#include "config.h"
#include "ContextProperties.h"
#include "DateFormatter.h"
#include "DocHelper.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "Painter.h"
#include "PrintoutConfig.h"
#include "PrintoutTextHelper.h"
#include "Sheet.h"
#include "Table.h"

#include <wx/pdfdoc.h>
#include <wx/pdfdc.h>

#include <format>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define PDF_UNIT "pt"
#define DEBUG_PDFGENERATOR
#define PRINT_CENTIMETERGRID

// frame around the standard printable area (within page margins)
#define PDFGENERATOR_SHOW_ITEMFRAME

// wxpdfdc is avaliable in wxpdfdoc version 1.4. otherwise use handwritten PdfPainter
#define PDFGENERATOR_USE_WXPDFDC

//#define DOCHELPER_EXTRA_SPACE MComplex( .1, .1 )
#define DOCHELPER_EXTRA_SPACE MComplex( 1, 1 )

/*****************************************************
**
**   PdfGenerator   ---   Constructor
**
******************************************************/
PdfGenerator::PdfGenerator( Horoscope_ptr h, const PrintoutConfig &printCfg, const PageConfig &pcfg )
	: horoscope( h ),
	printoutCfg( printCfg ),
	pageCfg( pcfg ),
	sheetSpec( SheetSpecLoader::get()->getConfig( printoutCfg.sheetId ))
{
#ifdef DEBUG_PDFGENERATOR
	debuglog << "PdfGenerator::PdfGenerator sheetId " << printoutCfg.sheetId << " cfg name " << sheetSpec.name
		<< " number of items: " << sheetSpec.items.size() << " autoPageBreak " << sheetSpec.autoPageBreak;
#endif	

	props = ContextProps_ptr( new ContextProperties );
	props->setRadixHoroscope( horoscope );

	// 1. set viewpoint
	if ( sheetSpec.viewpoint == Viewpoint::None )
	{
		warnlog << "PdfGenerator spec " << sheetSpec.name << " has no viewpoint, using default "
			<< astroconfig->general.defaultViewpoint;
		props->setPreferredViewpoint( astroconfig->general.defaultViewpoint );
	}
	else
	{
		props->setPreferredViewpoint( sheetSpec.viewpoint );
	}
	const Viewpoint viewpoint = props->getPreferredViewpoint();

	SheetSpecSummary summary = sheetSpec.getSummary();

	// planets are extracted for charts. Houses not displayes there.
	Planets planets;
	for( const auto p : summary.planets )
	{
		if ( ! PlanetHelper::isHouse( p )) planets.push_back( p );
	}
	props->setPlanets( planets, viewpoint );

#ifdef DEBUG_PDFGENERATOR
	debuglog << "PdfGenerator::PdfGenerator viewpoint " << viewpoint << ", planets:";
	for( auto p : summary.planets )
	{
		debuglog << "PdfGenerator::PdfGenerator -- > Planet: " << p;
	}
#endif	
	
	// set geometry and skins
	for( const auto &geom : printoutCfg.geoms ) { props->setGeometry( geom.first, geom.second ); }
	for( const auto &skin : printoutCfg.skins ) { props->setSkin( skin.first, skin.second ); }

	wxPaperSize wxp = wxPAPER_A4;
	switch( gdiconfig->page.paperSize )
	{
		case PaperSize::Letter:
			wxp = wxPAPER_LETTER;
		break;
		case PaperSize::Legal:
			wxp = wxPAPER_LEGAL;
		break;
		default:
		break;
	}

	pdfdoc = new wxPdfDocument( wxPORTRAIT, PDF_UNIT, wxp );

	// Document Properties
	pdfdoc->SetCreator( std::format( "{} {}", GUI_APP_NAME, PACKAGE_VERSION ));
	pdfdoc->SetCompression( gdiconfig->pdf.compressPdf );

	// coordinates in Portrait mode. the methods getPageCoordinates flip this in case of Landscape mode.
	originalPageCoordinates = MComplex( pdfdoc->GetPageWidth(), pdfdoc->GetPageHeight());

#ifdef DEBUG_PDFGENERATOR
	const PageCoordinates e = getPageCoordinates( PageOrientation::Portrait );
	debuglog << "PdfGenerator::PdfGenerator spec " << sheetSpec.name
		<< " PageCoordinates xmargin " << e.xMargin << " ymargin " << e.yMargin
		<< " xMax " << e.xMax << " yMax " << e.yMax
		<< " wxwidth " << pdfdoc->GetPageWidth() << " wxheight " << pdfdoc->GetPageHeight()
		<< " wxtop margin " << pdfdoc->GetTopMargin() << " wxleft " << pdfdoc->GetLeftMargin();
#endif	
}

/*****************************************************
**
**   PdfGenerator   ---   Destructor
**
******************************************************/
PdfGenerator::~PdfGenerator()
{
	delete pdfdoc;
}

/*****************************************************
**
**   PdfGenerator   ---   generatePdf
**
******************************************************/
bool PdfGenerator::generatePdf( const wxString &filename )
{
	#ifdef DEBUG_PDFGENERATOR
	debuglog << "Start PdfGenerator::generatePdf filename " << filename << ", sheet spec id " << printoutCfg.sheetId;
	#endif	

	doc = PrintoutTextHelper( props ).generateDocument( sheetSpec );

#ifdef PDFGENERATOR_USE_WXPDFDC
	assert( pdfdoc );
	wxPdfDC dc( pdfdoc, pdfdoc->GetPageWidth(), pdfdoc->GetPageHeight() );
	wxGraphicsContext *gc = dc.GetGraphicsContext();
	Painter *painter = PainterFactory().createGcPainter( gc );

#else

	Painter *painter = PainterFactory().createPdfPainter( pdfdoc );
#endif

	formatItems( painter );
	layoutDocument();
	newPage( painter, PageOrientation::Portrait, 0 );
	printItems( painter );

#ifdef PDFGENERATOR_USE_WXPDFDC
	gc->Flush();
	// no delete here because wxGraphicsContext is deleted in wx pwpdfdoc impl
	//delete gc;
#endif

	delete painter;
	return pdfdoc->SaveAsFile( filename );
}

/*****************************************************
**
**   PdfGenerator   ---   getPageCoordinates
**
******************************************************/
PageCoordinates PdfGenerator::getPageCoordinates( const PageOrientation orientation )
{
	PageCoordinates pc;

	switch( orientation )
	{
		case PageOrientation::Landscape:
			pc.xMargin = gdiconfig->page.verticalMargin;
			pc.yMargin = gdiconfig->page.horizontalMargin;
			pc.xMax = originalPageCoordinates.imag() - 2 * pc.xMargin;
			pc.yMax = originalPageCoordinates.real() - 2 * pc.yMargin;
		break;

		case PageOrientation::Portrait:
		default:
			pc.xMargin = gdiconfig->page.horizontalMargin;
			pc.yMargin = gdiconfig->page.verticalMargin;
			pc.xMax = originalPageCoordinates.real() - 2 * pc.xMargin;
			pc.yMax = originalPageCoordinates.imag() - 2 * pc.yMargin;
		break;
	}

	#ifdef DEBUG_PDFGENERATOR
	debuglog << "Start PdfGenerator::getPageCoordinates orientation " << orientation << " xMargin " << pc.xMargin << " yMargin " << pc.yMargin
		<< " xMax " << pc.xMax << " yMax " << pc.yMax;
	#endif	
	return pc;
}

/****************************painter, *************************
**
**   PdfGenerator   ---   formatItems
**
******************************************************/
void PdfGenerator::formatItems( Painter *painter )
{
	assert( painter );
	DocHelper helper( painter, DOCHELPER_EXTRA_SPACE );
	PageOrientation currentOrientation = PageOrientation::Portrait;

	for( DocItem_ptr docitem : doc->items )
	{
		PageCoordinates e = getPageCoordinates( currentOrientation );
#ifdef DEBUG_PDFGENERATOR
		debuglog << "PdfGenerator::formatItems type " << docitem->type 
			<< " PageCoordinates xmargin " << e.xMargin << " ymargin " << e.yMargin
			<< " xMax " << e.xMax << " yMax " << e.yMax;
#endif	

		switch( docitem->type )
		{
			case DocItemType::Text:
			{
				DocItemText *textitem = dynamic_cast<DocItemText*>( docitem.get() );
				assert( textitem );
				setFont( painter, textitem->subType );
				helper.formatAndFitTextItem( *textitem, e.xMax );
#ifdef DEBUG_PDFGENERATOR
				debuglog << "PdfGenerator::formatItems text '" << textitem->tf << "' subType " << textitem->subType
					<< " rect " << textitem->rect << " Align " << textitem->align;
#endif	
			}
			break;
			case DocItemType::Table:
			{
				Table *table = dynamic_cast<Table*>( docitem.get() );
				assert( table );
				formatTable( painter, table, currentOrientation );
#ifdef DEBUG_PDFGENERATOR
				debuglog << "PdfGenerator::formatItems Table rect " << table->getRect();
#endif	
			}
			break;
			case DocItemType::Chart:
			{
				DocItemChart *chartitem = dynamic_cast<DocItemChart*>( docitem.get() );
				assert( chartitem );
				double width = .01 * chartitem->width * e.xMax;

				chartitem->setRect( 0, 0, width, width * getChartXyRatio( chartitem->category ));
#ifdef DEBUG_PDFGENERATOR
				debuglog << "PdfGenerator::formatItems chart rect " << chartitem->getRect();
#endif	
			}
			break;
			case DocItemType::VargaChartGrid:
			{
				DocItemVargaChartGrid *griditem = dynamic_cast<DocItemVargaChartGrid*>( docitem.get() );
				assert( griditem );
				const double ratio = getChartXyRatio( WidgetCategory::VargaChart );

				if ( griditem->cols < 1 ) griditem->cols = 2;
				uint rows = (int)(( griditem->vargas.size() + griditem->cols - 1 ) / griditem->cols );
				assert( rows > 0 );
				//griditem->setRect( 0, 0, xMax, std::min( ( xMax * ratio ) / griditem->cols, yMax ));
				griditem->setRect( 0, 0, e.xMax, std::min( ( rows * e.xMax * ratio ) / griditem->cols, e.yMax ));

#ifdef DEBUG_PDFGENERATOR
				debuglog << "PdfGenerator::formatItems varga grid cols " << griditem->cols << " rows " << rows
					<< " ratio " << ratio << " rect " << griditem->rect;
#endif	
				//griditem->setRect( 0, 0, xMax, std::min( .5 * xMax * ratio * rows, yMax ));

#ifdef DEBUG_PDFGENERATOR
				debuglog << "PdfGenerator::formatItems VargaChartGrid grid size " << griditem->vargas.size() << " rows " << rows
					<< " ratio " << ratio << " rect " << griditem->getRect();
#endif	
			}
			break;

			case DocItemType::PageBreak:
			{
				DocItemPageBreak *pbitem = dynamic_cast<DocItemPageBreak*>( docitem.get() );
				assert( pbitem );
				currentOrientation = pbitem->orientation;
			}
			break;

			default:
				warnlog << "PdfGenerator::formatItems: unsupported doc item type " << docitem->type;
			break;
		}
	}
}

/*****************************************************
**
**   PdfGenerator   ---   getChartXyRatio
**
******************************************************/
double PdfGenerator::getChartXyRatio( const WidgetCategory &cat )
{
	double ratio = 1.0;
	if ( cat == WidgetCategory::SarvatobhadraChart )
	{
		ratio = .7;
	}
	else if ( cat == WidgetCategory::VargaChart )
	{
		const uint geomId = props->getGeometry( cat );
		const ChartGeometry geom = ChartGeometryLoader::get()->getConfig( geomId );
		if ( geom.containsElementType( CgElementType::SouthGrid ))
		{
			ratio = .7;
		}
	}
#ifdef DEBUG_PDFGENERATOR
	debuglog << "PdfGenerator::getChartXyRatio WidgetCategory " << cat << " ratio " << ratio;
#endif	

	return ratio;
}

/*****************************************************
**
**   PdfGenerator   ---   formatTable
**
******************************************************/
void PdfGenerator::formatTable( Painter *painter, Table *table, const PageOrientation &orientation )
{
	assert( table );
#ifdef DEBUG_PDFGENERATOR
	debuglog << "Start PdfGenerator::formatTable expand " << std::boolalpha << table->format.expand << " orientation " << orientation;
#endif	
	const PageCoordinates e = getPageCoordinates( orientation );

	constexpr double minColWidth = 0;
	constexpr double minRowHeight = 10;
	constexpr double extraTableCellWidth = 0;
	constexpr double extraTableCellHeight = 3;
	constexpr double extraHeanderFactor = 1.5;

	DocHelper helper( painter, DOCHELPER_EXTRA_SPACE );

	// step 1: format table title
	if ( ! table->title.isEmpty())
	{
		setFont( painter, TextItemSubType::Header2 );
		table->title.setRect( 0, 0, e.xMax, extraHeanderFactor * painter->getFontSize());
		table->title.align = Align::North;
		helper.formatTextItem( table->title, e.xMax );
		table->setRect( 0, 0, 0, table->title.getRect().height );
	}

	for( uint row = 0; row < table->getNbRows(); row++ )
	{
		table->format.row_height[ row ] = minRowHeight;
	}

	// step 2: format table cells and calculate columns width and row height
	for( uint col = 0; col < table->getNbCols(); col++ )
	{
		table->format.col_width[ col ] = minColWidth;
		if ( table->isEmptyCol( col ) && ! table->format.showEmptyColumns )
		{
			continue;
		}
		for( uint row = 0; row < table->getNbRows(); row++ )
		{
			DocItemText &textitem = table->getTextItem( col, row );

			setFont( painter, table->isHeader( col, row ) ? TextItemSubType::TableHeader : TextItemSubType::TableCell );

			// set item alignment ( only horizontal alignment supported)
			if ( table->format.col_alignment[col] & Align::Right )
			{
				textitem.align = Align::West;
			}

			helper.formatTextItem( textitem, e.xMax );

			// assumed that item with span > 1 do not contribute to the columns width
			if ( textitem.tableSpan == 1 && ! textitem.isSkipped )
			{
				table->format.col_width[col] = std::max( table->format.col_width[col], textitem.width + extraTableCellWidth );
			}
			table->format.row_height[row] = std::max( table->format.row_height[row], textitem.height + extraTableCellHeight );
#ifdef DEBUG_PDFGENERATOR
			debuglog << "PdfGenerator::formatTable col " << col << " row " << row << " contents '" << textitem.tf << "' item rect " << textitem.rect
				<< " span " << textitem.tableSpan << " isSkipped " << std::boolalpha << textitem.isSkipped
				<< " colwidth " << table->format.col_width[col] << " rowHeight " << table->format.row_height[row];
#endif	
		}
	}

	// step 3: calculate total width and height, set table rect
	double totalWidth = 0.0;
	for( uint col = 0; col < table->getNbCols(); col++ )
	{
		// TODO bool doJoinTableHeaders = true -> skip
		totalWidth += table->format.col_width[col];
	}

	if ( table->format.expand )
	{
		const double xspaceLeft = e.xMax - totalWidth;
		const double spacePerCol = xspaceLeft / table->getNbCols();

		#ifdef DEBUG_PDFGENERATOR
		debuglog << "PdfGenerator::formatTable EXPAND: table width " << table->rect.width << " space left " << xspaceLeft
			<< " per column " << spacePerCol;
		#endif	

		if ( xspaceLeft > 0 )
		{
			for( uint col = 0; col < table->getNbCols(); col++ )
			{
				table->format.col_width[ col ] += spacePerCol;
				totalWidth = e.xMax;
			}
		}
	}

	double totalHeight = 0.0;
	for( uint row = 0; row < table->getNbRows(); row++ )
	{
		totalHeight += table->format.row_height[row];
	}
	table->setRect( 0, 0, totalWidth, table->getRect().height + totalHeight );
#ifdef DEBUG_PDFGENERATOR
	debuglog << "table rect " << table->getRect();
#endif	

	// step 4: set cell rects
	double xcurrent = 0;
	const double ystart = table->title.isEmpty() ? 0 : table->title.getRect().height;
	for( uint col = 0; col < table->getNbCols(); col++ )
	{
		double ycurrent = ystart;
		for( uint row = 0; row < table->getNbRows(); row++ )
		{
			DocItemText &textitem = table->getTextItem( col, row );

			double width = table->format.col_width[col];

			// cells with span get additinal width from other columns
			if ( textitem.tableSpan > 1 )
			{
				textitem.align = Align::Center;
				for( int i = 1; i < textitem.tableSpan; i++ )
				{
					if ( col + i < table->getNbCols() )
					{
						width += table->format.col_width[ col + i ];
					}
				}
			}

			textitem.setRect( xcurrent, ycurrent, width, table->format.row_height[row] );
			helper.calculateTextAlign( textitem );
			ycurrent += table->format.row_height[row];
		}
		xcurrent += table->format.col_width[col];
	}
}

/*****************************************************
**
**   PdfGenerator   ---   layoutDocument
**
******************************************************/
void PdfGenerator::layoutDocument()
{
#ifdef DEBUG_PDFGENERATOR
	debuglog << "Start PdfGenerator::layoutDocument autoPageBreak " << std::boolalpha << doc->autoPageBreak;
#endif	
	uint currentpage = 0;
	PageOrientation currentOrientation = PageOrientation::Portrait;

	PageCoordinates e = getPageCoordinates( currentOrientation );
	double ycurrent = e.yMargin;

	for( DocItem_ptr docitem : doc->items )
	{
		#ifdef DEBUG_PDFGENERATOR
		debuglog << "PdfGenerator::layoutDocument page " << currentpage << " ycurrent " << ycurrent
			<< " autoPageBreak " << std::boolalpha << doc->autoPageBreak
			<< " type " << docitem->type << " item height " << docitem->getRect().height
			<< " maxy " << ( e.yMargin + e.yMax) << " item pagenumber " << docitem->pagenumber;
		#endif	

		docitem->pagenumber = currentpage;

		// manual page break
		if ( docitem->type == DocItemType::PageBreak )
		{
			layoutPage( currentOrientation, currentpage );

			DocItemPageBreak *pitem = dynamic_cast<DocItemPageBreak*>( docitem.get() );
			assert( pitem );
			currentOrientation = pitem->orientation;
			e = getPageCoordinates( currentOrientation );
			
			currentpage++;
			ycurrent = e.yMargin;
		}
		// automatic page break
		else if ( doc->autoPageBreak && ycurrent + docitem->getRect().height > e.yMargin + e.yMax )
		{
#ifdef DEBUG_PDFGENERATOR
	debuglog << "PdfGenerator::layoutDocument init auto page break";
#endif	

			docitem->pagenumber = currentpage + 1;
			layoutPage( currentOrientation, currentpage );

			currentpage++;
			currentOrientation = PageOrientation::Portrait;
			ycurrent = e.yMargin;
			e = getPageCoordinates( currentOrientation );
		}
		ycurrent += docitem->getRect().height;
	}

	pagecount = currentpage + 1;
	layoutPage( currentOrientation, currentpage );

#ifdef DEBUG_PDFGENERATOR
	debuglog << "Finished PdfGenerator::layoutDocument total pages: " << pagecount;
#endif	
}

/*****************************************************
**
**   PdfGenerator   ---   layoutPage
**
******************************************************/
void PdfGenerator::layoutPage( const PageOrientation &orientation, const int &p )
{
	const PageCoordinates e = getPageCoordinates( orientation );
#ifdef DEBUG_PDFGENERATOR
	debuglog << "Start PdfGenerator::layoutPage " << p
		<< " PageCoordinates xmargin " << e.xMargin << " ymargin " << e.yMargin
		<< " xMax " << e.xMax << " yMax " << e.yMax;
#endif	

	double totalItemHeight = 0.0;
	int itemCount = 0;
	for ( const DocItem_ptr &item : doc->items )
	{
		if ( item->pagenumber != p ) continue;

		totalItemHeight += item->getRect().height;
		itemCount++;
	}
	if ( itemCount == 0 ) return;

	// centeretemd vertically if total height is more than this number, otherwise: align top
	constexpr double minimalVerticalCenterHeight = .6;
	double yPerItem = 0;
	double ycurrent = e.yMargin;
	if ( totalItemHeight >  minimalVerticalCenterHeight * e.yMax )
	{
		yPerItem = ( e.yMax - totalItemHeight ) / ( itemCount + 1 );
		ycurrent += .5 * yPerItem;
	}

	//const double yPerItem = ( yMax - totalItemHeight ) / ( itemCount + 1 );
	//double ycurrent = topMargin + .5 * yPerItem;

#ifdef DEBUG_PDFGENERATOR
	debuglog << "PdfGenerator::layoutPage " << p << " item count " << itemCount << " totalItemHeight " << totalItemHeight << " yMax " << e.yMax
		<< " yPerItem " << yPerItem << " ycurrent " << ycurrent << " xmax " << e.xMax;
#endif	

	for ( const DocItem_ptr &item : doc->items )
	{
		if ( item->pagenumber != p ) continue;

		// x align
		double xleft = e.xMargin;
		if ( item->align & Align::HCenter )
		{
			xleft = e.xMargin + .5 * ( e.xMax - item->getRect().width );
		}
		else if ( item->align & Align::Right )
		{
			xleft = e.xMargin + e.xMax - item->getRect().width;
		}

		// step 2: calculate y position and move item
#ifdef DEBUG_PDFGENERATOR
		debuglog << "PdfGenerator::layoutPage (1) " << p << " item rect " << item->getRect() << " xleft " << xleft;
#endif	
		item->moveTo( xleft, ycurrent );
#ifdef DEBUG_PDFGENERATOR
		debuglog << "PdfGenerator::layoutPage (2) " << p << " item rect " << item->getRect() << " xleft " << xleft;
#endif	
		
		ycurrent += item->getRect().height + yPerItem;
	}
}

/*****************************************************
**
**   PdfGenerator   ---   printItems
**
******************************************************/
void PdfGenerator::printItems( Painter *painter )
{
	assert( painter );
	DocHelper helper( painter, DOCHELPER_EXTRA_SPACE );
	int currentpage = 0;
#ifdef DEBUG_PDFGENERATOR
		debuglog << "Start PdfGenerator::printItems with " << doc->items.size() << " items";
#endif	

	for( DocItem_ptr docitem : doc->items )
	{
		if ( docitem->type == DocItemType::PageBreak )
		{
			DocItemPageBreak *pitem = dynamic_cast<DocItemPageBreak*>( docitem.get() );
			assert( pitem );
			currentpage++;
			newPage( painter, pitem->orientation, currentpage );
			continue;
		}
		else if ( docitem->pagenumber != currentpage )
		{
			currentpage = docitem->pagenumber;
			newPage( painter, PageOrientation::Portrait, currentpage );
		}
#ifdef DEBUG_PDFGENERATOR
		debuglog << "PdfGenerator::printItems page " << currentpage << " type " << docitem->type << " rect " << docitem->rect
			<< " item page " << docitem->pagenumber;
#endif	

#ifdef PDFGENERATOR_SHOW_PAGEFRAME
  painter->setPen( wxPen( *wxRED, 1, wxPENSTYLE_SHORT_DASH ));
  painter->setTransparentBrush();
  painter->drawRectangle( MRect( docitem->getRect()));
  painter->setDefaultPen();
#endif
		switch( docitem->type )
		{
			case DocItemType::Text:
			{
				DocItemText *textitem = dynamic_cast<DocItemText*>( docitem.get() );
				assert( textitem );
				if ( textitem->subType == TextItemSubType::ErrorMessage )
				{
					painter->setPen( wxPen( *wxRED, 3, wxPENSTYLE_SOLID ));
					painter->setTransparentBrush();
					painter->drawRectangle( MRect( docitem->getRect()));
					painter->setDefaultPen();
				}
				setFont( painter, textitem->subType );
				helper.drawTextItem( *textitem );
#ifdef DEBUG_PDFGENERATOR
				debuglog << "PdfGenerator::printItems text item subType " << textitem->subType << " Contents '" << textitem->tf << "'";
#endif	
			}
			break;
			case DocItemType::Table:
			{
				Table *table = dynamic_cast<Table*>( docitem.get() );
				assert( table );
				printTable( painter, table );
			}
			break;
			case DocItemType::VargaChartGrid:
			{
				DocItemVargaChartGrid *griditem = dynamic_cast<DocItemVargaChartGrid*>( docitem.get() );
				assert( griditem );
				printVargaChartGrid( painter, griditem );
			}
			break;
			case DocItemType::Chart:
			{
				DocItemChart *chartitem = dynamic_cast<DocItemChart*>( docitem.get() );
				assert( chartitem );
				printChart( painter, chartitem );
			}
			break;

			case DocItemType::PageBreak:
				// was handled before
				assert( false );
			break;

			default:
				warnlog << "PdfGenerator::printItems: unsupported doc item type " << docitem->type;
			break;
		}
	}
}

/*****************************************************
**
**   PdfGenerator   ---   printVargaChartGrid
**
******************************************************/
void PdfGenerator::printVargaChartGrid( Painter *painter, DocItemVargaChartGrid *item )
{
	assert( painter );
	assert( item );
#ifdef DEBUG_PDFGENERATOR
	debuglog << "Start printVargaChartGrid rect " << item->getRect();
#endif	

	assert( item->vargas.size() > 1 );
	assert( item->cols > 0 );

	uint rows = (int)(( item->vargas.size() + item->cols - 1 ) / item->cols );
	assert( rows > 0 );

	int count = 0;
	double ycurrent = item->getRect().y;
	const double chartWidth = item->getRect().width / item->cols;
#ifdef DEBUG_PDFGENERATOR
	debuglog << "PdfGenerator::printVargaChartGrid item cols " << item->cols << " rows " << rows
		<< " varga size " << item->vargas.size() << " chartWidth " << chartWidth
		<< " item width " << item->getRect().width;
#endif	

	double xleft = item->getRect().x;
	for( const Varga &varga : item->vargas )
  {
		if ( count % item->cols ) xleft += chartWidth;

		const double chartHeight = item->getRect().height / rows;
		const MRect chartRect( xleft, ycurrent, chartWidth, chartHeight );

		WidgetSkinLoader *skinloader = WidgetSkinLoader::get();
		const WidgetSkin &skin = skinloader->getConfig( props->getSkin( WidgetCategory::VargaChart ));

		// flood fill
		if ( skin.brush.isOk() && skin.brush.style != wxBRUSHSTYLE_TRANSPARENT )
		{
			painter->setTransparentPen();
			painter->setBrush( skin.brush );
			painter->drawRectangle( chartRect );
		}

		HybridChart *chart = new HybridChart( props, WidgetCategory::VargaChart, varga );
		chart->setRect( chartRect );
		chart->OnDataChanged();
		chart->doLayout( painter );
#ifdef DEBUG_PDFGENERATOR
		debuglog << "printVargaChartGrid varga " << varga << " rect " << chart->getRect();
#endif	

		chart->doPaint( painter, chart->getRect() );
		delete chart;
		
		count++;
		if ( ! ( count % item->cols )) // next row
		{
			xleft = item->getRect().x;
			ycurrent += chartHeight;
		}
	}
}

/*****************************************************
**
**   PdfGenerator   ---   printChart
**
******************************************************/
void PdfGenerator::printChart( Painter *painter, DocItemChart *item )
{
	WidgetSkinLoader *skinloader = WidgetSkinLoader::get();
	const WidgetSkin &skin = skinloader->getConfig( props->getSkin( item->category ));

	// flood fill
	if ( skin.brush.isOk() && skin.brush.style != wxBRUSHSTYLE_TRANSPARENT )
	{
		painter->setTransparentPen();
		painter->setBrush( skin.brush );
		painter->drawRectangle( item->getRect() );
	}

	UranianSection *usection = nullptr;
	HybridChart *chart = nullptr;
	if ( item->category == WidgetCategory::UranianChart )
	{
		usection = new UranianSection( astroconfig->getUranianSection( UranianContext::RadixChart ));
		usection->chartDivision = item->division;
#ifdef DEBUG_PDFGENERATOR
		debuglog << "printChart UranianChart division " << item->division;
#endif	
		chart = new HybridChart( props, WidgetCategory::UranianChart, usection );
	}
	else
	{
		chart = new HybridChart( props, item->category, usection );
	}
	assert( chart );

	chart->setRect( item->getRect());
	chart->OnDataChanged();
	chart->doLayout( painter );
#ifdef DEBUG_PDFGENERATOR
	debuglog << "printChart varga " << item->varga << " rect " << chart->getRect();
#endif	

	chart->doPaint( painter, chart->getRect() );
	delete chart;
}

/*****************************************************
**
**   PdfGenerator   ---   printTable
**
******************************************************/
void PdfGenerator::printTable( Painter *painter, Table *table )
{
	assert( painter );
	assert( table );
	DocHelper helper( painter, DOCHELPER_EXTRA_SPACE );
	painter->setTransparentPen();

#ifdef DEBUG_PDFGENERATOR
		debuglog << "PdfGenerator::printTable rect " << table->getRect() << " cols " << table->getNbCols() << " rows " << table->getNbRows();
#endif	

	// step 1 print title
	if ( ! table->title.isEmpty())
	{
#ifdef DEBUG_PDFGENERATOR
		debuglog << "PdfGenerator::printTable title rect " << table->title.getRect();
#endif	
		setFont( painter, TextItemSubType::Header2 );
		helper.drawTextItem( table->title );
	}

	// step 2: fill cells and draw text
	const TableStyle &style = TableStyleLoader::get()->getConfig( printoutCfg.tableStyleId );
	painter->setTransparentPen();

	for( uint col = 0; col < table->getNbCols(); col++ )
	{
		if ( table->isEmptyCol( col )) continue;

		for( uint row = 0; row < table->getNbRows(); row++ )
		{
			const DocItemText &textitem = table->getTextItem( col, row );
			if ( textitem.isSkipped ) continue;

			bool doPaintBackground = false;
			bool overrideTextColor = false;

			if ( table->isHeader( col, row ))
			{
				// font for header cell
				setFont( painter, TextItemSubType::TableHeader );
				if ( style.useHeaderColors && style.headerTextColor.IsOk())
				{
					painter->setTextColor( style.headerTextColor );
					overrideTextColor = true;
				}

				// background for header cell
				if ( style.useHeaderColors && style.headerBgColor.IsOk())
				{
					painter->setBrush( style.headerBgColor );
					doPaintBackground = true;
				}
			}
			else // no header
			{
				// font for non-header
				setFont( painter, TextItemSubType::TableCell );
				if ( style.useCellColors && style.cellTextColor.IsOk())
				{
					painter->setTextColor( style.headerTextColor );
					overrideTextColor = true;
				}
				
				// backgound for non-header
				if ( style.useCellColors )
				{
					switch( style.cellBgMode )
					{
						case 0: // Transparent
						break;
						case 1: // Constant
							if ( style.allCellBgColor.IsOk())
							{
								painter->setBrush( style.allCellBgColor );
								doPaintBackground = true;
							}
						break;
						case 2: // Alternating
							if ( row % 2 ) 
							{
								if ( style.oddCellBgColor.IsOk())
								{
									painter->setBrush( style.oddCellBgColor );
									doPaintBackground = true;
								}
							}
							else
							{
								if ( style.evenCellBgColor.IsOk())
								{
									painter->setBrush( style.evenCellBgColor );
									doPaintBackground = true;
								}
							}
						break;
					}
				}
			}

			// paint background
			if ( doPaintBackground ) painter->drawRectangle( textitem.getRect());

			if ( ! overrideTextColor )
			{
				painter->setDefaultTextColor();
			}
			debuglog << "draw text item col " << col << " row " << row << " align " << textitem.align
				<< " contents " << textitem.tf;
			helper.drawTextItem( textitem );
		}
	}
	painter->setDefaultPen();
	painter->setTransparentBrush();

	// step 3: draw border
	if ( style.showGrid )
	{
		// outer border
		double leftBorder = table->getRect().x;
		double currentWidth = 0.0;
		double ytop = table->getRect().y;
		double height = table->getRect().height;
		if ( ! table->title.isEmpty())
		{
			const double h = table->title.getRect().height;
			ytop += h;
			height -= h;
		}

		// outer frame and horizontal lines: find empty cols
		for( uint col = 0; col < table->getNbCols(); col++ )
		{
			currentWidth += table->format.col_width[ col ];
			uint nextcol = col + 1;
			if ( col == table->getNbCols() - 1 || table->isEmptyCol( nextcol ) )
			{
				#ifdef DEBUG_PDFGENERATOR
				debuglog << "PdfGenerator::printTable found empty col after " << col << " leftBorder " << leftBorder << " currentWidth " << currentWidth;
				#endif	
				painter->drawRectangle( leftBorder, ytop, currentWidth, height );

				// horizontal lines
				if ( table->format.showHorizontalBorders )
				{
					double y = ytop;
					for( uint r = 1; r < table->getNbRows(); r++ )
					{
						y += table->format.row_height[r];
						painter->drawLine( leftBorder, y, leftBorder + currentWidth, y );
					}
					if ( col < table->getNbCols() - 1 && table->isEmptyCol( nextcol ))
					{
						leftBorder += currentWidth + table->format.col_width[ col +1 ];
						currentWidth = -table->format.col_width[ col +1 ];
					}
				}
			}
		}

		// right border per cell
		if ( table->format.showVerticalBorders )
		{
			for( uint col = 0; col < table->getNbCols() - 1; col++ )
			{
				uint nextcol = col + 1;	
				if ( table->isEmptyCol( nextcol )) continue;

				double y = ytop;
				for( uint row = 0; row < table->getNbRows(); row++ )
				{
					DocItemText &textitem = table->getTextItem( col, row );
					if ( ! textitem.isSkipped )
					{
						painter->drawLine( textitem.rect.x + textitem.rect.width, y, textitem.rect.x + textitem.rect.width, y + table->format.row_height[ row ] );
					}
					y += table->format.row_height[ row ];
				}
			}
		}
	}
}

/*****************************************************
**
**   PdfGenerator   ---   printPageGrid
**
******************************************************/
void PdfGenerator::printPageGrid( Painter *painter, const double &stepwidth, const PageOrientation &orientation )
{
	const PageCoordinates e = getPageCoordinates( orientation );
	const double h = e.yMax + 2 * e.yMargin;
	const double w = e.xMax + 2 * e.xMargin;

	double a = stepwidth;
	while ( a < w )
	{
		painter->drawLine( a, 0, a, h );
		a += stepwidth;
	}

	a = stepwidth;
	while ( a < h )
	{
		painter->drawLine( 0, a, w, a );
		a += stepwidth;
	}
}

/*****************************************************
**
**   PdfGenerator   ---   printCentimeterGrid
**
******************************************************/
void PdfGenerator::printCentimeterGrid( Painter *painter, const PageOrientation &orientation )
{
	// 1 centimeter in pt
	constexpr double stepwidth = 28.34646;

  //painter->setPen( wxPen( *wxLIGHT_GREY, 1, wxPENSTYLE_SHORT_DASH ));
  painter->setPen( wxPen( *wxLIGHT_GREY, 1, wxPENSTYLE_DOT ));
	printPageGrid( painter, stepwidth, orientation );

	painter->setDefaultPen();
}

/*****************************************************
**
**   PdfGenerator   ---   newPage
**
******************************************************/
void PdfGenerator::newPage( Painter *painter, const PageOrientation &orientation, const uint &pageno )
{
#ifdef DEBUG_PDFGENERATOR
	debuglog << "PdfGenerator::newPage orientation " << orientation << " pageno " << pageno;
#endif	

	switch( orientation )
	{
		case PageOrientation::Portrait:
			pdfdoc->AddPage( wxPORTRAIT );
		break;
		case PageOrientation::Landscape:
			pdfdoc->AddPage( wxLANDSCAPE );
		break;
		default:
			// keep previous orientation
			pdfdoc->AddPage();
		break;
	}
	painter->setDefaultPen();

	printHeaderFooter( painter, true, orientation, pageno );
	printHeaderFooter( painter, false, orientation, pageno );
#ifdef PRINT_CENTIMETERGRID
	printCentimeterGrid( painter, orientation );
#endif	

#ifdef PDFGENERATOR_SHOW_ITEMFRAME
	const PageCoordinates e = getPageCoordinates( orientation );
  painter->setPen( wxPen( *wxGREEN, 1, wxPENSTYLE_SHORT_DASH ));
  painter->setTransparentBrush();
  painter->drawRectangle( MRect( e.xMargin, e.yMargin, e.xMax, e.yMax ) );
  painter->setDefaultPen();
#endif
}

/*****************************************************
**
**   PdfGenerator   ---   printHeaderFooter
**
******************************************************/
void PdfGenerator::printHeaderFooter( Painter *painter, const bool &isHeader,
	const PageOrientation &orientation, const uint &pageno )
{
	const PageCoordinates e = getPageCoordinates( orientation );

	constexpr double ytol = 12;
	const PageMarginal &marginal = isHeader ? pageCfg.header : pageCfg.footer;
	const double yline = isHeader ? e.yMargin : e.yMargin + e.yMax;
	const MRect textRect( e.xMargin, isHeader ? yline - ytol : yline, e.xMax, ytol );
	
#ifdef DEBUG_PDFGENERATOR
	debuglog << "printHeaderFooter isHeader " << std::boolalpha << " isHeader " << " rect " << textRect;
#endif	

	painter->setDefaultPen();
	if ( marginal.enabled && ( pageno > 1 || marginal.showOnFirstPage ))
	{
		if ( marginal.showSeparator )
		{
			painter->drawLine( e.xMargin, yline, e.xMargin + e.xMax, yline );
		}

		// replace horoscope base variables ($name etc)
		wxString s = PrintoutTextHelper::replaceHoroscopeBasedVariables( marginal.text, horoscope );

		// replace other sheet based (or basic) variables
		s.Replace( "$pagecount", std::format( "{}", pagecount ));
		s.Replace( "$page", std::format( "{}", pageno + 1 ));
		const double jd = MDate::getCurrentJD();
		s.Replace( "$creationdate", DateFormatter::get()->formatDate( jd, true ));
		s.Replace( "$version", VERSION );
		s.Trim( true ).Trim( false );

		Align align = Align::Center;
		switch( marginal.orientation )
		{
			case 1: // center
				align = isHeader ? Align::South : Align::North;
			break;
			case 2: // right
				align = isHeader ? Align::SouthWest : Align::NorthWest;
			break;
			case 0: // left
			default:
				align = isHeader ? Align::SouthEast : Align::NorthEast;
			break;
		}
#ifdef DEBUG_PDFGENERATOR
		debuglog << "printHeaderFooter isHeader " << std::boolalpha << isHeader << " rect " << textRect
			<< " text: " << s;
#endif	

		setFont( painter, TextItemSubType::Footnote );
		DocItemText textitem( s, textRect, align );
		DocHelper helper( painter );
		helper.formatTextItem( textitem, e.xMax );
#ifdef DEBUG_PDFGENERATOR
		debuglog << "printHeaderFooter textitem rect " << textitem.rect << " width " << textitem.width << " height " << textitem.height;
#endif	
		helper.drawTextItem( textitem );
	}
}

/*****************************************************
**
**   PdfGenerator   ---   getTextItemYScalingFactor
**
******************************************************/
double PdfGenerator::getTextItemYScalingFactor( const TextItemSubType &type )
{
	double scalingFactor = 1.0;
	switch( type )
	{
		case TextItemSubType::Header1:
			scalingFactor = 2.0;
		break;
		case TextItemSubType::Header2:
			scalingFactor = 1.6;
		break;
		default:
		break;
	}
	return scalingFactor;
}

/*****************************************************
**
**   PdfGenerator   ---   setFont
**
******************************************************/
void PdfGenerator::setFont( Painter *painter, const TextItemSubType &type )
{
	painter->setFont( type, pageCfg.basicFontSize );
}

