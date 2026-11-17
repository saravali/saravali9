/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/textclient.cpp
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

#include "Ashtakoota.h"
#include "astroconfig.h"
#include "Calculator.h"
#include "CompositeHoroscope.h"
#include "config.h"
#include "ContextProperties.h"
#include "DateFormatter.h"
#include "Direction.h"
#include "Eclipse.h"
#include "enumserializer.h"
#include "Ephemeris.h"
#include "EventFinder.h"
#include "Exporter.h"
#include "FileConfig.h"
#include "gdiconfig.h"
#include "Hora.h"
#include "JsonHandler.h"
#include "Lang.h"
#include "LanguageConfig.h"
#include "logger.h"
#include "constants.h"
#include "MatchWriter.h"
#include "mathbase.h"
#include "PlanetaryPicture.h"
#include "SpWriter.h"
#include "Shadbala.h"
#include "MDocument.h"
#include "SolarHoroscope.h"
#include "Term.h"
#include "TextHelper.h"
#include "Uranian.h"
#include "Varga.h"
#include "VedicAspect.h"
#include "WesternAspect.h"
#include "YdlInterpreter.h"
#include "Yoga.h"

#include <iostream>
#include <string>
#include <wx/app.h>
#include <wx/cmdline.h>

Astroconfig *astroconfig = nullptr;
Gdiconfig *gdiconfig = nullptr;

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
	{ wxCMD_LINE_SWITCH, "h", "help",
		"show this help message", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
	{ wxCMD_LINE_SWITCH, "", "version",
		"show application version", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "directory-report",
		"show file and directory report", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_OPTION, "", "file", "Open given file and use data for calculation", 
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_OPTION, "", "date", "Set UT date YYYY-MM-DD HH:MM:SS",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "", "ldate", "Set local date YYYY-MM-DD HH:MM:SS",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_OPTION, "", "second-date", "Set second date (e.g. for directions, partner) YYYY-MM-DD HH:MM:SS",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "", "year", "set second year (Directions, Ephemeris, Solar)",
		wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "", "month", "set second month (January=1, Directions and Ephemeris)",
		wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "", "day", "set second day (Directions)",
		wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_OPTION, "", "location", "Set location \"name LL:LL[:LL] BB:BB[:BB] (+/-)TZ\"",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_SWITCH, "", "vedic", "Force Vedic viewpoint",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "western", "Force western viewpoint",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "uranian", "Force uranian viewpoint",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "astronomical", "Force astronomical viewpoint",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_SWITCH, "", "html", "print output in Saravali specific formatted HTML (instead of text)",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "plain-html", "print output in plain HTML format (instead of text)",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "csv", "print output in CSV format (instead of text)",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	// Methods without Horoscope class
	{ wxCMD_LINE_SWITCH, "", "eclipses", "Show eclipses", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "hora", "Show Hora", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "ephem", "Show ephemeris", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "ephem-declinations", "Show ephemeris declinations",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "ephem-sunrise", "Show ephemeris sunrise/Tithi",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "ephem-ingress", "Show ephemeris ingress events",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "ephem-aspects", "Show ephemeris aspects",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "ephem-lunar", "Show ephemeris Lunar events",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "", "ephem-kp", "Show ephemeris kp events for Dasa #n",
		wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },

	// methods based upon Horoscope class
	{ wxCMD_LINE_SWITCH, "", "basedata", "Show base data of horoscope", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "planets", "List planets (like in main window)",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_SWITCH, "", "balance", "Show object balance", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "bhavas", "Show Bhavas", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "kp", "Show Krishnamurti Paddhati", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "nakshatras", "Show Nakshatras", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "vargas", "Show Vargas", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "ashtakavarga", "Show Ashtakavarga", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "jaimini", "Show Jaimini calculation", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "arabic", "Show Arabic parts", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "shadbala",   "Show Shadbala", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "uranian-houses", "Show Uranian Houses", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	// Yoga section
	{ wxCMD_LINE_SWITCH, "", "yogas", "Show Yogas", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "", "yoga-compile", "Compile a Yoga rule", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "", "yoga-evaluate", "Evaluate a Yoga rule", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_OPTION, "", "dasas", "Show Dasa nb #n (short form)",
		wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "", "dasas-compact", "Show Dasa nb #n (compact form)",
		wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "", "dasas-long", "Show Dasa nb #n (short form)",
		wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_SWITCH, "", "aspectarium",   "Show aspectarium", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "aspectlist",   "Show aspect list", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_SWITCH, "", "transits", "Show transits", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "solar-arc", "Show Solar arc", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "progressions", "Show progressions (secondary directions: 1 day = 1 year)",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "lunations", "Show Synodic Lunations", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "profections", "Show Profections", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_OPTION, "", "solar", "Show Solar chart", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "multidirections", "Show multidirections", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_SWITCH, "", "uranian-matches", "Show Uranian Matches",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "", "sensitive", "Show Uranian sensitive points analysis #n",
		wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "pictures", "Show Uranian planetary pictures",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "", "eventfinder", "Show Uranian events for year #n",
		wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_OPTION, "", "file2", "Open second file for partner chart",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "partner-vedic", "Show Vedic partner analysis",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "", "partner-composite",
		"Show partner composite chart",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_NONE, "", "", "",
		wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
};


/*************************************************//**
**
*   CLASS SaravaliCli
**
******************************************************/
class SaravaliCli
{
public:

	/**************************************************************
	***
	**    SaravaliCli   ---   Constructor
	***
	***************************************************************/
	SaravaliCli( int argc, char * argv[] )
		: argc ( argc ),
			argv( argv )
	{
		texthelper = nullptr;
		writecount = 0;

		FileConfig *fileconfig = FileConfig::get();
		fileconfig->initHomedir();

		astroconfig = new Astroconfig;
		gdiconfig = new Gdiconfig;

		LogAdapter *logadapter = LogAdapter::get();
#ifndef  __WXMSW__
		logadapter->enableStdoutLogAppender( true );

		// get temporary debug logging during config load
		logadapter->setStdoutLogLevel( LogLevel::Debug );
#endif

		astroconfig->load();
		gdiconfig->load();

		fileconfig->initDatadir( argv[0] );
		logadapter->setStdoutLogLevel( gdiconfig->view.stdoutLogLevel );

		LanguageConfig::get()->setLanguage();

		viewpoint = astroconfig->general.defaultViewpoint;

		props = ContextProps_ptr( new ContextProperties());
		doc = MDocument_ptr( new MDocument );
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   Destructor
	***
	***************************************************************/
	~SaravaliCli()
	{
		if ( texthelper ) delete texthelper;
		delete astroconfig;
		delete gdiconfig;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   run
	***
	***************************************************************/
	int run()
	{
		writecount = 0;
		double tjd = MDate::getCurrentJD();
		WidgetExportType exportType = WidgetExportType::Text;
		StopWatch sw;
		wxString s;
		long l = 0;

		try
		{
			wxCmdLineParser parser( cmdLineDesc, argc, argv );
			parser.Parse( true );

			if ( parser.Found( "help" ))
			{
				// is done automatically
				//parser.Usage();
				return 0;
			}

			if ( parser.Found( "version" ))
			{
				infolog << "version " << PACKAGE_VERSION;
				return 0;
			}

			if ( parser.Found( "directory-report" ))
			{
				FileConfig::get()->writeFileAndDirectoryReport( doc );
				writecount++;
			}

			// writer type
			if ( parser.Found( "html" )) { exportType = WidgetExportType::Html; }
			else if ( parser.Found( "plain-html" )) { exportType = WidgetExportType::PlainHtml; }
			else if ( parser.Found( "csv" )) { exportType = WidgetExportType::Csv; }

			// viewpoint override by params
			if ( parser.Found( "vedic" )) { viewpoint = Viewpoint::Vedic; }
			else if ( parser.Found( "western" )) { viewpoint = Viewpoint::Western; }
			else if ( parser.Found( "uranian" )) { viewpoint = Viewpoint::Uranian; }
			else if ( parser.Found( "astronomical" )) { viewpoint = Viewpoint::Astronomical; }
			props->setPreferredViewpoint( viewpoint );


			dataset.location = astroconfig->standardLocation;
			if ( parser.Found( "location", &s ))
			{
				dataset.location = Location( s );
			}

			// test (1) for incompatible parameters
			if ( parser.Found( "date", &s ) && parser.Found( "ldate", &s ))
			{
				errorlog << "Params --date and --ldate cannot be combined.";
				return 1;
			}

			// test (2) for incompatible parameters
			if ( parser.Found( "file", &s ) && ( parser.Found( "date", &s ) && parser.Found( "ldate", &s )))
			{
				errorlog << "Params --file and --date or --ldate cannot be combined.";
				return 1;
			}

			// Create Horoscope
			h = Horoscope_ptr( new Horoscope());

			if ( parser.Found( "file", &s ))
			{
				try
				{
					dataset = DateLocationLoader().loadFile( s ); 
				}
				catch( std::exception& )
				{
					errorlog << "could not open file \"" << s << "\"";
					exit( 1 );
				}
			}
			// parse UT date
			else if ( parser.Found( "date", &s ))
			{
				dataset.location.dst = 0;
				dataset.location.tz = 0;

				wxString err;
				if ( ! DateFormatter::get()->parseDateAndTime( s, dataset.date.jd, err ))
				{
					errorlog << "Date string \"" << s << "\" cannot be parsed, error was " << err;
					return 1;
				}
				debuglog << " ut date is now " << dataset;
			}
			// parse local date
			else if ( parser.Found( "ldate", &s ))
			{
				wxString err;
				if ( ! DateFormatter::get()->parseDateAndTime( s, dataset.date.jd, err ))
				{
					errorlog << "Date string \"" << s << "\" cannot be parsed, error was " << err;
					return 1;
				}
				dataset.location.dst = DateFormatter::get()->getSystemDstOffset( dataset.date.jd );
				dataset.date.jd -= ( dataset.location.tz + dataset.location.dst ) / 24.0;
			}
			// nor --file or --date or --ldate given -> local dst and tz
			else
			{
				dataset.location.dst = DateFormatter::get()->getSystemDstOffset( dataset.date.jd );
				//dataset.date.jd -= ( dataset.location.tz + dataset.location.dst ) / 24.0;
			}

			h->setDate( dataset.date.jd );
			h->dl.location = dataset.location;

			h->update();
			props->setHoroscope( HoroscopeRole::Radix, h );

			if ( parser.Found( "second-date", &s ))
			{
				wxString err;
				if ( ! DateFormatter::get()->parseDateAndTime( s, tjd, err ))
				{
					errorlog << "Second date string \"" << s << "\" cannot be parsed, error was " << err;
					return 1;
				}
			}
			auto [ tyear, tmonth, tday, tut ] = CalculatorFactory().getCalculator()->getDateIntsFromJD( tjd );
			if ( parser.Found( "month", &l )) { tmonth = (int)l; }
			if ( parser.Found( "year", &l )) { tyear = (int)l; }

			if ( parser.Found( "eclipses" )) showEclipses( tyear );
			if ( parser.Found( "hora" )) showHora( tjd );

			// ephemeris
			if ( parser.Found( "ephem" ))  showEphemeris( tyear, tmonth, 0 );
			if ( parser.Found( "ephem-declinations" )) showEphemeris( tyear, tmonth, 1 );
			if ( parser.Found( "ephem-sunrise" )) showEphemeris( tyear, tmonth, 2 );
			if ( parser.Found( "ephem-ingress" )) showEphemeris( tyear, tmonth, 3 );
			if ( parser.Found( "ephem-lunar" )) showEphemeris( tyear, tmonth, 4 );
			if ( parser.Found( "ephem-kp", &l ))
			{
				showEphemeris( tyear, tmonth, 5, (DasaId)l );
			}

			texthelper = new TextHelper( props, doc );

			if ( parser.Found( "basedata" )) showTextAnalysis( WidgetCategory::TextBaseData );
			if ( parser.Found( "planets" )) showPlanets();

			if ( parser.Found( "balance" )) showTextAnalysis( WidgetCategory::TextPlanetBalance );
			if ( parser.Found( "bhavas" )) showTextAnalysis( WidgetCategory::TextVedicHouses );
			if ( parser.Found( "kp" )) showTextAnalysis( WidgetCategory::TextKp );
			if ( parser.Found( "nakshatras" )) showTextAnalysis( WidgetCategory::TextNakshatra );
			if ( parser.Found( "ashtakavarga" ))   showTextAnalysis( WidgetCategory::TextAshtakavarga );
			if ( parser.Found( "jaimini" )) showTextAnalysis( WidgetCategory::TextJaimini );
			if ( parser.Found( "vargas" )) showTextAnalysis( WidgetCategory::TextVarga );
			if ( parser.Found( "arabic" )) showTextAnalysis( WidgetCategory::TextArabicParts );
			if ( parser.Found( "shadbala" )) showShadbala();
			if ( parser.Found( "uranian-houses" )) showTextAnalysis( WidgetCategory::TextUranianHouses );

			if ( parser.Found( "yogas" )) showYogas();
			if ( parser.Found( "yoga-compile", &s )) doYogaRule( s, false );
			if ( parser.Found( "yoga-evaluate", &s )) doYogaRule( s, true );

			if ( parser.Found( "dasas", &l ))
			{
				showTextAnalysis( WidgetCategory::TextDasaShort, Varga::Rasi, (DasaId)l );
			}
			if ( parser.Found( "dasas-compact", &l ))
			{
				showTextAnalysis( WidgetCategory::TextDasaCompact, Varga::Rasi, (DasaId)l );
			}
			if ( parser.Found( "dasas-long", &l ))
			{
				showTextAnalysis( WidgetCategory::TextDasaComprehensive, Varga::Rasi, (DasaId)l );
			}

			if ( parser.Found( "aspectarium" ))
			{
				showTextAnalysis( viewpoint == Viewpoint::Vedic ?
					WidgetCategory::TextVedicAspectarium : WidgetCategory::TextWesternAspectarium );
			}
			if ( parser.Found( "aspectlist" ))
			{
				showTextAnalysis( viewpoint == Viewpoint::Vedic ?
					WidgetCategory::TextVedicAspectarium : WidgetCategory::TextWesternAspectList );
			}

			if ( parser.Found( "transits" )) showDirections( tjd, HoroscopeRole::Transit );
			if ( parser.Found( "solar-arc" )) showDirections( tjd, HoroscopeRole::SolarArc );
			if ( parser.Found( "progressions" )) showDirections( tjd, HoroscopeRole::Progressive );
			if ( parser.Found( "lunations" )) showDirections( tjd, HoroscopeRole::Lunations );
			if ( parser.Found( "profections" )) showDirections( tjd, HoroscopeRole::Profections );

			if ( parser.Found( "solar", &l )) showSolarChart( l );
			if ( parser.Found( "multidirections" )) showMultiDirections( tjd );

			if ( parser.Found( "uranian-matches" )) showUranian();
			if ( parser.Found( "sensitive", &l )) { showSensitivePoints( l ); }
			if ( parser.Found( "pictures" )) { showPlanetaryPictures(); }
			if ( parser.Found( "eventfinder", &l )) calculateEvents( l );

			if ( parser.Found( "partner-vedic" ) || parser.Found( "partner-composite" ))
			{
				props->setHoroscope( HoroscopeRole::Radix, nullptr );
				props->setHoroscope( HoroscopeRole::Partner1, h );

				Horoscope_ptr h1 = Horoscope_ptr( new Horoscope());
				if ( parser.Found( "file2", &s ))
				{
					try
					{
						h1->openFile( s );
					}
					catch( std::exception &e )
					{
						errorlog << "could not open second file" << s << ". reason " << e.what();
						exit( 1 );
					}
				}
				else
				{
					errorlog << "Second file must be given with option '--file2'";
					exit( 1 );
				}
				h1->update();
				props->setHoroscope( HoroscopeRole::Partner2, h1 );

				if ( parser.Found( "partner-vedic" ) ) showPartnerAnalysis( 0 );
				if ( parser.Found( "partner-composite" ) ) showPartnerAnalysis( 1 );
			}

			Exporter *exporter = ExporterFactory().getExporter( exportType );
			mcout << exporter->exportDocument( doc ) << std::endl;
			delete exporter;

			if ( ! writecount )
			{
				parser.Usage();
			}
		}
		catch( std::runtime_error &e )
		{
			errorlog << "Runtime error " << e.what();
			return -1;
		}
		infolog << "FINISHED textclient::run " << sw.getElapsedMicrosec() << " microsec";
		return 0;
	}

protected:

	Horoscope_ptr h;

	Viewpoint viewpoint = Viewpoint::None;

	ContextProps_ptr props;

	DateLocation dataset;

	TextHelper *texthelper = nullptr;

	int writecount = 0;

	MDocument_ptr doc;

	int argc = 0;

	char **argv;

	/**************************************************************
	***
	**    SaravaliCli   ---   showEclipses
	***
	***************************************************************/
	void showEclipses( const int &year )
	{
		props->isLocaltime = gdiconfig->prefs.eclipseTimezone;
		EclipseExpert ec( props );
		Calculator *calculator = CalculatorFactory().getCalculator();
		const double ejd = calculator->calc_jd( year, 1, 1, 0 );
		ec.update( ejd, ejd + 365 * 3, 0, 0 );
		ec.write( doc );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showHora
	***
	***************************************************************/
	void showHora( const double &jd )
	{
		HoraExpert expert;
		expert.update( jd );
		expert.write( doc, false );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showEphemeris
	***
	***************************************************************/
	void showEphemeris( const int &year, const int &month, const int &type, const DasaId dasa = DasaId::Vimsottari  )
	{
		EphemExpert e( props );
		e.setShowHeader( true );
		e.prepareMonth( year, month );

		switch ( type )
		{
			case 0:
				e.writeCoordinates( doc, CoordinateMode::Longitude );
			break;
			case 1:
				e.writeCoordinates( doc, CoordinateMode::Declination );
			break;
			case 2:
				e.writeSunrise( doc );
			break;
			case 3:
				e.writeIngress( doc );
			break;
			case 4:
				e.writeLunar( doc );
			break;

			case 5:
				if ( is_valid_enum( dasa ))
				{
					e.writeKp( doc, dasa );
				}
				else
				{
					errorlog << "invalid dasa id " << (int)dasa;
				}
			break;
		}
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showPlanets
	***
	***************************************************************/
	void showPlanets()
	{
		Horoscope_ptr horoscope = props->getRadixHoroscope();
		assert( horoscope );
		texthelper->writePlanets( horoscope );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showYogas
	***
	***************************************************************/
	void showYogas()
	{
		YogaExpert *expert = YogaExpert::get();
		expert->updateAndWrite( doc, props );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   doYogaRule
	***
	***************************************************************/
	void doYogaRule( const wxString &rulestr, const bool &doEvaluate )
	{
		YogaExpert *expert = YogaExpert::get();

		Rule *rule = expert->parseRule( rulestr );
		if ( ! rule )
		{
			doc->addLine( std::format( "Error: rule {} cannot be compiled", rulestr ));
			return;
		}
		doc->addLine( std::format( "Rule compiled. Error status: {}", enum_to_string( rule->errorStatus )));

		if ( doEvaluate )
		{
			const double d = YdlInterpreter( props ).evaluateRule( rule );
			const ExpressionErrorStatus status = rule->getNestedErrorStatus();
			if ( status == ExpressionErrorStatus::Ok )
			{
				doc->addLine( expert->resultToString( d, rule->getOutputType()));
			}
			else
			{
				doc->addLine( std::format( "ErrorStatus: {}", enum_to_string( status )));
			}
		}
		delete rule;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showShabala
	***
	***************************************************************/
	void showShadbala()
	{
		ShadbalaExpert expert( props );
		expert.updateAllBalas();
		expert.write( doc );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showSolarChart
	***
	***************************************************************/
	void showSolarChart( const int &year )
	{
		writecount++;

	  SolarHoroscopeExpert expert( props );
		expert.setSolarReturnYear( year );
		expert.update();
		expert.writeSolarReturnData( doc );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showPlanetList
	***
	***************************************************************/
	void showPlanetList( const Viewpoint &viewpoint )
	{
		Horoscope_ptr horoscope = props->getRadixHoroscope();
		assert( horoscope );
		texthelper->writePlanets( horoscope, viewpoint );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showTextAnalysis
	***
	***************************************************************/
	void showTextAnalysis( const WidgetCategory &cat, const Varga varga = Varga::Rasi,
		const DasaId dasa = DasaId::Vimsottari )
	{
		texthelper->writeTextAnalysis( cat, varga, dasa );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showMultiDirections
	***
	***************************************************************/
	void showMultiDirections( const double &directionJD )
	{
		DirectionExpert expert( props );
		expert.setDirectionModes( astroconfig->directions.multiDirectionRoles );
		expert.calculateMultiDirections( directionJD );
		expert.writeMultiDirectionReport( doc );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showDirections
	***
	***************************************************************/
	void showDirections( const double &directionJD, const HoroscopeRole &role )
	{
		DirectionExpert expert( props );
		expert.calculateDirections( directionJD, role );
		expert.writeDirectionReport( doc );

		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showSensitivePoints
	***
	***************************************************************/
	void showSensitivePoints( const int &type )
	{
		UranianSection usection = astroconfig->getUranianSection( UranianContext::RadixMatches );
		usection.matchDivision = gdiconfig->prefs.divisionSpWidget;
		SensitivePointWriter( h, props, &usection ).writeMatrix( doc, (TermId)type, PlanetId::Sun );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showPlanetaryPictures
	***
	***************************************************************/
	void showPlanetaryPictures()
	{
		UranianSection usection = astroconfig->getUranianSection( UranianContext::RadixPp );

		PlanetaryPictureExpert helper( props, &usection );
		helper.createPictures();
		helper.writePictureTable( doc );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   calculateEvents
	***
	***************************************************************/
	void calculateEvents( const int &year )
	{
		auto section = astroconfig->getUranianSection( UranianContext::EventFinder );
		EventFinder finder( props, &section );
		//finder.calculateEvents( year );

		finder.startCalculation( year );
		finder.waitForCalculation();
		finder.cancelCalculation();

		MatchWriter writer( props, &section, viewpoint );
		writer.writeMatches( doc, finder.getMatches() );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showUranian
	***
	***************************************************************/
	void showUranian()
	{
		UranianSection *usection = &astroconfig->getUranianSection( UranianContext::RadixMatches );
		UranianExpert expert( props, usection );
		expert.calculateRadix();

		MatchWriter writer( props, usection, Viewpoint::Uranian );
		writer.writeMatches( doc, expert.getMatches()  );
		writecount++;
	}

	/**************************************************************
	***
	**    SaravaliCli   ---   showPartnerAnalysis
	***
	***************************************************************/
	void showPartnerAnalysis( const int &type )
	{
		if ( type == 0 )
		{
			AshtakootaExpert expert( props );
			expert.update();
			expert.write( doc );
		}
		else
		{
			Horoscope_ptr ch( new CompositeHoroscope( props ));
			ch->update();
			TextHelper( props, doc ).writePlanets( ch );
		}
		writecount++;
	}

};


/*************************************************//**
**
*   CLASS TextClientApplication
**
******************************************************/
class TextClientApplication : public wxApp
{
public:

protected:
	virtual bool OnInit() override;

	virtual int OnRun() override
	{
		return 0;
	}

};

IMPLEMENT_APP_CONSOLE( TextClientApplication )

/**************************************************************
***
**    TextClientApplication   ---   OnInit
***
***************************************************************/
bool TextClientApplication::OnInit()
{
	SetAppName( APP_NAME );
	SetVendorName( VENDOR_NAME );
	SaravaliCli textclient( argc, argv );
	textclient.run();

	return true;
}

