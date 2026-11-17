/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/textclient.cpp
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

#include "astroconfig.h"
#include "ChartGeometry.h"
#include "ChartGeometry.h"
#include "constants.h"
#include "ContextProperties.h"
#include "ConfigLoaderImpls.h"
#include "DateFormatter.h"
#include "enumserializer.h"
#include "Exporter.h"
#include "FileConfig.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "Lang.h"
#include "LanguageConfig.h"
#include "Painter.h"
#include "PdfGenerator.h"
#include "Sheet.h"
#include "Table.h"
#include "TextHelper.h"

#include <iostream>
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/pdfdoc.h>

Astroconfig *astroconfig = nullptr;
Gdiconfig *gdiconfig = nullptr;

enum class TextClientOutputFormat { Png, Jpeg, Pdf };

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
  { wxCMD_LINE_OPTION, "", "date", "Set UT date YYYY-MM-DD HH:MM:SS", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_OPTION, "", "location", "Set location \"name LL:LL[:LL] BB:BB[:BB] (+/-)TZ\"",
			wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_OPTION, "", "file", "Set input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_OPTION, "", "outputfile", "Set output file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_SWITCH, "", "basedata", "Show base data of horoscope", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
 
  { wxCMD_LINE_SWITCH, "", "png", "Set graphics format PNG (default)", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_SWITCH, "", "jpeg", "Set graphics format JPEG", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },

  { wxCMD_LINE_SWITCH, "", "brushtest", "Test wxBrush functionality, default output file is brushtest.png",
			wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_SWITCH, "", "pentest", "Test wxPen functionality, default output file is pentest.png",
			wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },

  { wxCMD_LINE_OPTION, "", "printout", "Create PDF printout <id>", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_SWITCH, "", "printout-list", "Show list of PDF printouts", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_SWITCH, "", "pdf-pentest", "wxPen test, PDF version", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_SWITCH, "", "pdf-brushtest", "Brush test, PDF version", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_SWITCH, "", "pdftest", "simple test", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },

  { wxCMD_LINE_OPTION, "", "vargachart", "Create Varga chart <vargaid>", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_SWITCH, "", "westernchart", "Create Western chart", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_SWITCH, "", "sbc", "Create Sarvatobhadra chart", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_SWITCH, "", "uranianchart", "Create Uranian chart", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_SWITCH, "", "universalchart", "Create Universal chart", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },

  { wxCMD_LINE_OPTION, "", "xsize", "Set x size of output to <id> (default 800)", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_OPTION, "", "ysize", "Set y size of output to <id> ( default 800)", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },

  { wxCMD_LINE_OPTION, "", "geom", "Set geometry of output to <id> (default 0)", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_OPTION, "", "skin", "Set skin of output to <id> (default 0)", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },

  { wxCMD_LINE_SWITCH, "", "version", "Show application version", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "h", "help", "show this help message", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },

	wxCMD_LINE_DESC_END
};

/*************************************************//**
**
*     CLASS   ---   GdiTextclient
**
******************************************************/
class GdiTextclient : public wxApp
{
public:

	/*************************************************//**
	**
	*     GdiTextclient   ---   Destructor
	**
	******************************************************/
	~GdiTextclient()
	{
		delete astroconfig;
		delete gdiconfig;
	}

	/*************************************************//**
	**
	*     GdiTextclient   ---   OnInit
	**
	******************************************************/
	bool OnInit()
	{
		wxString s;
		long p = 0;

		FileConfig *fileconfig = FileConfig::get();
		fileconfig->initHomedir();

		astroconfig = new Astroconfig;
		gdiconfig = new Gdiconfig;

		LogAdapter *logadapter = LogAdapter::get();
#ifndef  __WXMSW__
		logadapter->enableStdoutLogAppender( true );
#endif

		astroconfig->load();
		gdiconfig->load();

		fileconfig->initDatadir( argv[0] );
		LanguageConfig::get()->setLanguage();
		logadapter->setStdoutLogLevel( gdiconfig->view.stdoutLogLevel );

		h = Horoscope_ptr( new Horoscope());
		props = ContextProps_ptr( new ContextProperties );
		props->setRadixHoroscope( h );

		wxInitAllImageHandlers();

		wxCmdLineParser parser( cmdLineDesc, argc, argv );
		parser.Parse( true );

		if ( parser.Found( "location", &s ))
		{
			// TODO
			debuglog << "Location is: " << s;
		}

		if ( parser.Found( "basedata" ))
		{
			// TODO
			// TextHelper( props, doc ).writePlanets( ch );
			//showTextAnalysis( WidgetCategory::TextBaseData );
		}

		if ( parser.Found( "outputfile", &s )) { outputfile = s; }

		if ( parser.Found( "jpeg" )) bitmapType = wxBITMAP_TYPE_JPEG;

		if ( parser.Found( "pentest" )) do_pentest();
		if ( parser.Found( "brushtest" )) do_brushtest();

		if ( parser.Found( "pdf-pentest" )) pdf_pentest();
		if ( parser.Found( "pdf-brushtest" )) pdf_brushtest();
		if ( parser.Found( "pdftest" )) pdftest();

		if ( parser.Found( "xsize", &p )) xsize = (int)p;
		if ( parser.Found( "ysize", &p )) ysize = (int)p;
		if ( parser.Found( "geom", &p )) geom = (int)p;
		if ( parser.Found( "skin", &p )) skin = (int)p;

		if ( parser.Found( "file", &s ))
		{
			try
			{
				debuglog << "GdiTextclient: open file " << s;
				h->openFile( s );
			}
			catch( std::exception &e )
			{
				errorlog << "GdiTextclient:could not open input file" << s << ", message " << e.what();
				return false;
			}
		}
		else
		{
			if ( parser.Found( "location", &s ))
			{
				debuglog << "GdiTextclient: using location: " << s;
				h->dl.location = Location( s );
			}
			else h->dl.location = astroconfig->standardLocation;
			
			if ( parser.Found( "date", &s ))
			{
				debuglog << "GdiTextclient: using date: " << s;
				h->dl.location.dst = 0;
        h->dl.location.tz = 0;

				wxString err;
				if ( ! DateFormatter::get()->parseDateAndTime( s, h->dl.date.jd, err ))
				{
					errorlog << "Date string \"" << s << "\" cannot be parsed, error was " << err;
          return 1;
				}
			}
			else
			{
				h->dl.location.dst = DateFormatter::get()->getSystemDstOffset( h->dl.date.jd );
			}
		}
		h->update();

		if ( parser.Found( "vargachart", &p )) create_chart( WidgetCategory::VargaChart, (Varga)p );
		if ( parser.Found( "westernchart" )) create_chart( WidgetCategory::WesternChart );
		if ( parser.Found( "sbc" )) create_chart( WidgetCategory::SarvatobhadraChart );
		if ( parser.Found( "uranianchart" )) create_chart( WidgetCategory::UranianChart );
		if ( parser.Found( "universalchart" )) create_chart( WidgetCategory::UniversalChart );

		if ( parser.Found( "printout", &p )) print_pdftestpage( p ); 
		if ( parser.Found( "printout-list" )) show_printoutlist(); 

		if ( ! writecount ) debuglog << "Use " << argv[0] << " -h or --help to show usage";


		// Workaround: program hangs up here if return true is given.
		exit( 0 );
		//return true;
	}

protected:

	/*************************************************//**
	**
	*     GdiTextclient   ---   do_brushtest
	**
	******************************************************/
	void do_brushtest()
	{
		debuglog << "Start test brush x " << xsize << " y " << xsize;
		writecount++;

		wxBitmap bitmap( xsize, ysize );
    wxMemoryDC dc;
    dc.SelectObject( bitmap );

		wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
		assert( gc );
		Painter *painter = PainterFactory().createGcPainter( gc );
		assert( painter );

		brushtest_internal( painter );
		delete painter;
		delete gc;

		if ( outputfile.IsEmpty()) outputfile = bitmapType == wxBITMAP_TYPE_JPEG ?  "brushtest.jpg" : "brushtest.png";
		bitmap.SaveFile( outputfile, bitmapType );
	}

	/*************************************************//**
	**
	*     GdiTextclient   ---   pdf_brushtest
	**
	******************************************************/
	void pdf_brushtest()
	{
		writecount++;
		// DIN A4 	210 x 297 mm 	21,0 cm x 29,7 cm
		xsize = 150;
		ysize = 250;
		wxPdfDocument *pdfdoc = new wxPdfDocument( wxPORTRAIT, "mm" );
		pdfdoc->SetCompression( true );

		pdfdoc->AddPage();
		Painter *painter = PainterFactory().createPdfPainter( pdfdoc );
		brushtest_internal( painter );

		if ( outputfile.IsEmpty()) outputfile = "pdftest.pdf";
		pdfdoc->SaveAsFile( outputfile );

		delete painter;
		delete pdfdoc;
	}


	/*************************************************//**
	**
	*     GdiTextclient   ---   brushtest_internal
	**
	******************************************************/
	void brushtest_internal( Painter *painter )
	{
		writecount++;
		painter->setTransparentPen();
		//painter->setDefaultBrush();
		painter->setBrush( *wxLIGHT_GREY );
		painter->drawRectangle( MRect( 0, 0, xsize, ysize ));
		painter->setDefaultPen();
		painter->setTransparentPen();

		const std::list<MBrush> brushes =
		{
			MBrush( wxNullColour, wxBRUSHSTYLE_TRANSPARENT ),
			MBrush( *wxBLACK ),
			MBrush( *wxBLUE ),
			MBrush( wxColour( 0, 255, 0, 50 )),
			MBrush( "madera.jpg", 0 ),
			MBrush( "madera.jpg", 45 ),
			MBrush( "madera.jpg", 180 ),
			MBrush( "madera.jpg", 10 ),
			MBrush( "madera.jpg", 70 ),
			MBrush( wxColour( "green" ), wxBRUSHSTYLE_VERTICAL_HATCH ),
			MBrush( wxColour( "blue" ), wxBRUSHSTYLE_HORIZONTAL_HATCH ),
			MBrush( wxColour( "brown" ), wxBRUSHSTYLE_CROSS_HATCH ),
			MBrush( wxColour( "blue" ), wxBRUSHSTYLE_BDIAGONAL_HATCH ),
			MBrush( wxColour( "red" ), wxBRUSHSTYLE_FDIAGONAL_HATCH ),
			MBrush( wxColour( "orange" ), wxBRUSHSTYLE_CROSSDIAG_HATCH )
		};

		const double xstep = xsize / brushes.size();
		constexpr double xtol = 1;
		const double ystep = ysize/2;

		double x = 0;

		for( const MBrush &brush : brushes )
		{
			painter->setBrush( brush );
			painter->drawRectangle( x + xtol, xtol, xstep - 2 * xtol, ystep - 2 * xtol );

			painter->drawEllipse( x + xtol, ystep + xtol, xstep - 2 * xtol, ystep - 2 * xtol );
			x += xstep;
		}
		painter->drawLine( 0, ystep, xsize, ystep );
	}


	/*************************************************//**
	**
	*     GdiTextclient   ---   pdf_pentest
	**
	******************************************************/
	void pdf_pentest()
	{
		writecount++;
		// DIN A4 	210 x 297 mm 	21,0 cm x 29,7 cm
		xsize = 150;
		ysize = 250;
		wxPdfDocument *pdfdoc = new wxPdfDocument( wxPORTRAIT, "mm" );
		pdfdoc->SetCompression( true );

		pdfdoc->AddPage();
		Painter *painter = PainterFactory().createPdfPainter( pdfdoc );
		pentest_internal( MRect( 10, 10, 170, 270 ), painter );

		if ( outputfile.IsEmpty()) outputfile = "pdftest.pdf";
		pdfdoc->SaveAsFile( outputfile );

		delete painter;
		delete pdfdoc;
	}

	/*************************************************//**
	**
	*     GdiTextclient   ---   do_pentest
	**
	******************************************************/
	void do_pentest()
	{
		writecount++;
		debuglog << "Start test pen x " << xsize << " y " << xsize;

		wxBitmap bitmap( xsize, ysize );
    wxMemoryDC dc;
    dc.SelectObject( bitmap );

		wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
		assert( gc );
		Painter *painter = PainterFactory().createGcPainter( gc );
		assert( painter );

		pentest_internal( MRect( 0, 0, xsize, ysize ), painter );
		delete painter;
		delete gc;

		if ( outputfile.IsEmpty()) outputfile = bitmapType == wxBITMAP_TYPE_JPEG ?  "pentest.jpg" : "pentest.png";
		bitmap.SaveFile( outputfile, bitmapType );
	}

	/*************************************************//**
	**
	*     GdiTextclient   ---   pentest_internal
	**
	******************************************************/
	void pentest_internal( const MRect &rect, Painter *painter )
	{
		painter->setTransparentPen();
		painter->setDefaultBrush();
		//painter->drawRectangle( MRect( 0, 0, xsize, ysize ));
		painter->drawRectangle( rect );

		const std::list<wxPen> pens =
		{
			wxPen( "black", 1, wxPENSTYLE_SOLID ),
			wxPen( "black", 2, wxPENSTYLE_SOLID ),
			wxPen( "black", 3, wxPENSTYLE_SOLID ),
			wxPen( "red", 1, wxPENSTYLE_SOLID ),
			wxPen( "black", 10, wxPENSTYLE_SOLID ),
			*wxTRANSPARENT_PEN,
			wxPen( "black", 1, wxPENSTYLE_DOT ),
			wxPen( "black", 1, wxPENSTYLE_LONG_DASH ),
			wxPen( "black", 1, wxPENSTYLE_SHORT_DASH ),
			wxPen( "black", 1, wxPENSTYLE_DOT_DASH ),

		};
		const double xstep = ysize / ( pens.size() + 2 );
		double x = xstep;
		for( const wxPen &pen : pens )
		{
			if ( pen.GetStyle() == wxPENSTYLE_TRANSPARENT )
			{
				painter->setGraphicFont();
				painter->rawTextOut( rect.x, x, "(transparent)" );
			}
			painter->setPen( pen );
			painter->drawLine( rect.x, x, rect.x + rect.width, x );
			x += xstep;
		}
	}

	/*************************************************//**
	**
	*     GdiTextclient   ---   create_chart
	**
	******************************************************/
	void create_chart( const WidgetCategory &category, const Varga varga = Varga::Rasi )
	{
		writecount++;
		debuglog << "Start create chart category " << category
			<< " varga " << varga << " x " << xsize << " y " << xsize;

		wxBitmap bitmap( xsize, ysize );
    wxMemoryDC dc;
    dc.SelectObject( bitmap );

		wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
		assert( gc );
		Painter *painter = PainterFactory().createGcPainter( gc );
		assert( painter );

		ChartGeometryLoader *gloader = ChartGeometryLoader::get();
		const std::vector<std::pair<uint,wxString>> a = gloader->getAllIdsAndNames( category );
		const int gg = ( geom >= 0 && geom < (int)a.size() ) ? a[geom].first : a[0].first;

		props->setGeometry( category, gg );
		debuglog << "GdiTextclient: chart geom id " << geom << " sup id " << gg
			<< " has name " << gloader->getConfig( gg ).name;
		props->setSkin( category, skin );

		// flood fill
		WidgetSkinLoader *skinloader = WidgetSkinLoader::get();
		const WidgetSkin &skin = skinloader->getConfig( props->getSkin( category ));
		if ( skin.brush.isOk() && skin.brush.style != wxBRUSHSTYLE_TRANSPARENT )
		{
			painter->setBrush( skin.brush );
		}
		else
		{
			painter->setBrush( gdiconfig->colors.bgColor );
		}
		painter->drawRectangle( MRect( 0, 0, xsize, ysize ));

		{
			//HybridChart *chart = new HybridChart( props, category, varga );
			GdiWidget *chart = new HybridChart( props, category, varga );
			chart->OnDataChanged();
			chart->doLayout( painter, MRect( 0, 0, xsize, ysize ));
			chart->doPaint( painter, chart->getRect() );
			delete chart;
		}
		delete painter;
		delete gc;

		if ( outputfile.IsEmpty()) outputfile = bitmapType == wxBITMAP_TYPE_JPEG ?  "chart.jpg" : "chart.png";
		bitmap.SaveFile( outputfile, bitmapType );

	}

	/*************************************************//**
	**
	*     GdiTextclient   ---   show_printoutlist
	**
	******************************************************/
	void show_printoutlist()
	{
		writecount++;
		MDocument_ptr doc( new MDocument );
		SheetSpecLoader *loader = SheetSpecLoader::get();

		doc->addHeader1( "Printout items" );

		Table *table = new Table( 3 );
		int i = 0;
		for( const auto &a : loader->getConfigs())
		{
			TableRow row;
			row.addInteger( i++ );
			row.addValue( a.name );
			row.addValue( a.description );

			table->addRow( row );
		}
		doc->addItem( table );
		Exporter *exporter = ExporterFactory().getExporter( WidgetExportType::Text );
		mcout << exporter->exportDocument( doc ) << std::endl;
	}

	/*************************************************//**
	**
	*     GdiTextclient   ---   print_pdftestpage
	**
	******************************************************/
	void print_pdftestpage( const int &p )
	{
		writecount++;
		if ( outputfile.IsEmpty()) outputfile = "printout_testpage.pdf";
		PrintoutConfig pcfg = gdiconfig->printout;
		pcfg.sheetId = p;
		PdfGenerator( h, pcfg, gdiconfig->page ).generatePdf( outputfile );
	}

	/*************************************************//**
	**
	*     GdiTextclient   ---   pdftest
	**
	******************************************************/
	void pdftest()
	{
		writecount++;
		// DIN A4 	210 x 297 mm 	21,0 cm x 29,7 cm
		xsize = 150;
		ysize = 250;
		wxPdfDocument *pdfdoc = new wxPdfDocument( wxPORTRAIT, "mm" );
		pdfdoc->SetCompression( true );

		pdfdoc->SetFillColour( *wxBLUE );

		pdfdoc->AddPage();

		wxPdfArrayDouble dash5;
    wxPdfLineStyle style5(0.25, wxPDF_LINECAP_BUTT, wxPDF_LINEJOIN_MITER, dash5, 0., wxColour(0, 0, 0));

    wxPdfArrayDouble dash6;
    dash6.Add(3.5);
    dash6.Add(3.5);
    wxPdfLineStyle style6(0.5, wxPDF_LINECAP_BUTT, wxPDF_LINEJOIN_MITER, dash6, 0., wxColour(0, 255, 0));


		pdfdoc->SetLineStyle(style5);
    pdfdoc->Ellipse(100,105,40,20);
    pdfdoc->SetLineStyle(style6);
    pdfdoc->Ellipse(100,105,20,10, 0, 90, 180, wxPDF_STYLE_DRAW);
    pdfdoc->Ellipse(100,105,20,10, 0, 270, 360, wxPDF_STYLE_FILLDRAW);

    pdfdoc->Ellipse(50,105,20,10, 0, 0, 360, wxPDF_STYLE_FILLDRAW);

		if ( outputfile.IsEmpty()) outputfile = "pdftest.pdf";
		pdfdoc->SaveAsFile( outputfile );

		//delete painter;
		delete pdfdoc;
	}

	ContextProps_ptr props;
	Horoscope_ptr h;

	// workaround for showing help if no cmd line param is given
	int writecount = 0;

	wxBitmapType bitmapType = wxBITMAP_TYPE_PNG;
	int xsize = 800;
	int ysize = 800;
	int geom;
	int skin = 0;

	//TextClientOutputFormat outputFormat;
	wxString outputfile;

};


#ifdef __MINGW64__
IMPLEMENT_APP_CONSOLE(GdiTextclient);
#else
wxIMPLEMENT_APP(GdiTextclient);
#endif


