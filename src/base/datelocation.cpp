/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/datelocation.cpp
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

#include "datelocation.h"

#include "bootstrap.h"
#include "Calculator.h"
#include "constants.h"
#include "DateFormatter.h"
#include "Formatter.h"
#include "logger.h"
#include "mathbase.h"

#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <math.h>
#include <format>

/*****************************************************
**
**   MDate   ---   Constructor
**
******************************************************/
MDate::MDate()
{
	setCurrentDate();
}

/*****************************************************
**
**   MDate   ---   Constructor
**
******************************************************/
MDate::MDate( const double &jd )
{
	this->jd = jd;
}

/*****************************************************
**
**   MDate   ---   Constructor
**
******************************************************/
MDate::MDate( const int &year, const int &month, const int &day, const double time )
{
	set( year, month, day, time );
}

/*****************************************************
**
**   MDate   ---   Constructor
**
******************************************************/
MDate::MDate( const wxString &thedate )
{
	setCurrentDate();
	wxString err;
	if ( ! DateFormatter::get()->parseDateAndTime( thedate, jd, err ))
	{
		errorlog << "invalid date string " << thedate << ", error message: " << err;
	}
}

/*****************************************************
**
**   MDate   ---   getCurrentJD
**
******************************************************/
// static
double MDate::getCurrentJD()
{
	time_t tt;
	time( &tt );

	// 1.1.1970 + seconds elapsed / number of seconds per day
	//return( 2440587.5 + (double)tt / 86400.0 );
	return timeToJD( tt );
}

/*****************************************************
**
**   MDate   ---   setCurrentDate
**
******************************************************/
void MDate::setCurrentDate()
{
	jd = getCurrentJD();
}

/*****************************************************
**
**   MDate   ---   addOneMonth
**
******************************************************/
void MDate::addOneMonth()
{
	auto[ year, month, day, ut ] = CalculatorFactory().getCalculator()->getDateIntsFromJD( jd );
	if ( month == 12 )
	{
		month = 1;
		++year;
	}
	else
	{
		++month;
	}
	set( year, month, day, ut );
}

/*****************************************************
**
**   MDate   ---   set
**
******************************************************/
void MDate::set( const int &year, const int &month, const int &day, const double &time )
{
	jd = CalculatorFactory().getCalculator()->calc_jd( year, month, day, time );
}

/*****************************************************
**
**   MDate   ---   operator<<
**
******************************************************/
mostream& operator<< ( mostream& stream, const MDate &d )
{
	stream << DateFormatter::get()->formatUTDatetime( d.jd );
	return stream;
}

/*****************************************************
**
**   Location   ---   Constructor
**
******************************************************/
Location::Location( const wxString &locstring )
{
	const wxUniChar MINUS = '-';
	Formatter *formatter =  Formatter::get();
	wxString token;
	uint i;
	
	wxStringTokenizer tokenizer( locstring, " " );

	if ( tokenizer.HasMoreTokens() )
	{
    locName = tokenizer.GetNextToken();
	}

	// longitude
	if ( tokenizer.HasMoreTokens() )
	{
    wxString token = tokenizer.GetNextToken();
		bool negative = false;
		if ( token.Left( 1 ) == MINUS )
		{
			negative = true;
			i = 1;
		}
		else i = 0;
		wxString err;
		if ( ! formatter->parseDegreeString( token.substr( i ), longitude, 180.0, err ))
		{
			errorlog << "cannot parse longitude string " << token << " error was " << err;
		}
		if ( negative ) longitude *= -1;
		debuglog << "longitude token " << token << " value " << longitude;
	}

	// latitude
	if ( tokenizer.HasMoreTokens() )
	{
    wxString token = tokenizer.GetNextToken();
		bool negative = false;
		if ( token.Left( 1 ) == MINUS )
		{
			negative = true;
			i = 1;
		}
		else i = 0;
		wxString err;
		if ( ! formatter->parseDegreeString( token.substr( i ), latitude, 180.0, err ))
		{
			errorlog << "cannot parse latitude string " << token << " error was " << err;
		}
		if ( negative ) latitude *= -1;
		debuglog << "latitude token " << token << " value " << latitude;
	}

	// time zone
	if ( tokenizer.HasMoreTokens() )
	{
    wxString token = tokenizer.GetNextToken();
		if ( ! stringToDouble( token, tz )) errorlog << "cannot parse time zone string " << token;
	}
	set( locName, longitude, latitude, tz, 0.0 );
}

/*****************************************************
**
**   Location   ---   set
**
******************************************************/
void Location::set( wxString locname, const double &longitude, const double &latitude,
	const double &tz, const double &dst, const DaylightSavingMode dstMode )
{
	this->locName = locname;
	this->longitude = longitude;
	this->latitude = latitude;
	this->tz = tz;
	this->dst = dst;
	this->dstMode = dstMode;
}

/*****************************************************
**
**   Location   ---   operator<<
**
******************************************************/
mostream& operator<< ( mostream& stream, const Location &l )
{
	Formatter *formatter = Formatter::get();

	stream << tr( "Location" ) << ": " << l.locName << std::endl
		<< tr( "Longitude" ) << " : " << formatter->formatLongitude( l.longitude ) << std::endl
		<< tr( "Latitude" ) << " : " << formatter->formatLatitude( l.latitude );
	return stream;
}

/*****************************************************
**
**   DateLocation   ---   Constructor
**
******************************************************/
DateLocation::DateLocation( const wxString fname )
{
	filename = fname;
	name = tr( "nn" );
}

/*****************************************************
**
**   DateLocation   ---   getWeekDay
**
******************************************************/
int DateLocation::getWeekDay() const
{
	return (int)( date.jd + ( location.tz + location.dst )/24.0 + 1.5 ) % 7;
}

/*****************************************************
**
**   DateLocation   ---   getLocalMeanTime
**
******************************************************/
double DateLocation::getLocalMeanTime()
{
	double t = getTimeFromJD( date.jd );
	t += location.longitude / 15.0;
	return t;
}

/*****************************************************
**
**   DateLocation   ---   operator<<
**
******************************************************/
mostream& operator<< ( mostream &o, const DateLocation &d )
{
	Formatter *f = Formatter::get();
	o << "   Data set" << std::endl
		<< "---------------------------" << std::endl
		<< "Name: " << d.name << std::endl
		<< d.date << std::endl;

	const double time = a_red( getTimeFromJD( d.date.jd ) + d.location.tz + d.location.dst, 24 );
	o << "Local Time: " << f->formatTime( time ) << std::endl
		<< "Time Zone: " << d.location.tz << std::endl
		<< "DST: " << d.location.dst << std::endl

		<< d.location << std::endl
		<< "Remark: " << d.remark << std::endl
		<< "---------------------------";
	return o;
}

/*****************************************************
**
**   DateLocationLoader   ---   save
**
******************************************************/
void DateLocationLoader::save( const DateLocation &datelocation )
{
	if ( datelocation.filename.size() == 0 )
	{
		throw std::invalid_argument( "filename empty" );
	}
	wxTextFile file( datelocation.filename );
	if ( wxFileName::FileExists( datelocation.filename ))
	{
		if ( ! file.Open()) throw std::invalid_argument( "cannot open file in write mode" );
		file.Clear();
	}
	else
	{
		if ( ! file.Create()) throw std::invalid_argument( "cannot open file in write mode" );
	}

	const double jjd = datelocation.date.jd
		+ ( datelocation.location.tz + datelocation.location.dst ) / 24;

	const auto[ year, month, day ] = DateFormatter::get()->calculateDateIntegers( jjd );

	file.AddLine( "#Maitreya 3.0" );
	file.AddLine( datelocation.name );
	file.AddLine( std::format( "{}", day ));
	file.AddLine( std::format( "{}", month ));
	file.AddLine( std::format( "{}", year ));

	// time
	const double jjd2 = jjd - .5;
	const double dd = ( jjd2 - (double)(int)jjd2) * 24;
	file.AddLine( std::format( "{:.8f}", dd ));

	file.AddLine( std::format( "{}", datelocation.location.tz ));
	file.AddLine( std::format( "{}", datelocation.location.dst ));
	file.AddLine( datelocation.location.locName );

	{
		double longitude = datelocation.location.longitude;
		if ( longitude > 0 ) longitude += .000005;
		else if ( longitude < 0 ) longitude -= .000005;
		file.AddLine( std::format( "{}", longitude ));
	}

	{
		double latitude = datelocation.location.latitude;
		if ( latitude > 0 ) latitude += .000005;
		else if ( latitude < 0 ) latitude -= .000005;
		file.AddLine( std::format( "{}", latitude ));
	}
	debuglog << "DateLocationLoader::save remark " << datelocation.remark;
	file.AddLine( datelocation.remark );
	file.Write();
}

/*****************************************************
**
**   DateLocationLoader   ---   loadFile
**
******************************************************/
DateLocation DateLocationLoader::loadFile( const wxString &fname, const bool isAtxFile )
{
	debuglog << "DateLocationLoader::loadFile " << fname << " atx " << std::boolalpha << isAtxFile;
	const wxString atxtoken = ".atx";
	if ( ! wxFileName::IsFileReadable( fname ))
	{
		errorlog << "readable file " << fname << " does not exist";
    throw std::invalid_argument( "invalid file name" );
	}
	if ( fname.size() > 5 && fname.substr( fname.size() - 4 ) == atxtoken )
	{
		return loadAtxFile( fname );
	}
	return loadMtxFile( fname );
}

/*****************************************************
**
**   DateLocationLoader   ---   loadAtxFile
**
******************************************************/
DateLocation DateLocationLoader::loadAtxFile( const wxString &filename )
{
	constexpr size_t min_atx_line_count = 11;
	int day, month, year;
	double time, minute, timezone, longitude, latitude, lmin, bmin;
	int hour, ldeg, bdeg;
	wxString line;
	wxString locname;

	wxTextFile file( filename );
  if ( ! file.Open())
  {
    throw std::invalid_argument( "cannot open file" );
  }
	if ( file.GetLineCount() < min_atx_line_count ) throw std::length_error( "file too short" );

	DateLocation datelocation( filename );
  datelocation.name = file.GetFirstLine();

  // day
  line = file.GetNextLine();
  if ( ! stringToInt( line, day ) ||  day <= 0 || day > 31 ) throw std::out_of_range( "invalid day param" );

	// month
  line = file.GetNextLine();
	if ( ! stringToInt( line, month ) ||  month <= 0 || month > 12 ) throw std::out_of_range( "invalid month param" );

	// year
  line = file.GetNextLine();
	if ( ! stringToInt( line, year ) ||  year <= -5001 || year > 10000 ) throw std::out_of_range( "invalid year param" );

	// hour
  line = file.GetNextLine();
	if ( ! stringToInt( line, hour ) || hour < 0 || hour > 24 ) throw std::out_of_range( "invalid hour param" );

	// minute
  line = file.GetNextLine();
	if ( ! stringToDouble( line, minute ) || minute < 0 || minute > 60.0 ) throw std::out_of_range( "invalid minute param" );
	time = minute + 60 * hour;
	time /= 60;

	// time zome
  line = file.GetNextLine();
	if ( ! stringToDouble( line, timezone ) || timezone < -12 || timezone > 12.0 )
		throw std::out_of_range( "invalid time zone param" );
	time -= timezone;

	// location name
  locname = file.GetNextLine();

	// longitude degrees
  line = file.GetNextLine();
	if ( ! stringToInt( line, ldeg ) || ldeg < -180.0 || ldeg > 180.0 ) throw std::out_of_range( "invalid longitude param" );

	// longitude minute
  line = file.GetNextLine();
	if ( ! stringToDouble( line, lmin ) || lmin < 0 || lmin > 60.0 ) throw std::out_of_range( "invalid longitude param" );
	longitude = lmin + 60 * ldeg;
	longitude /= 60;

	// latitude degrees
  line = file.GetNextLine();
	if ( ! stringToInt( line, bdeg ) || bdeg < -180.0 || bdeg > 180.0 ) throw std::out_of_range( "invalid longitude param" );

	// latitude minute
  line = file.GetNextLine();
	if ( ! stringToDouble( line, bmin ) || bmin < 0 || bmin > 60.0 ) throw std::out_of_range( "invalid longitude param" );
	latitude = bmin + 60 * bdeg;
	latitude /= 60;

	datelocation.date.set( year, month, day, time );
	datelocation.location.set( locname, longitude, latitude, timezone, 0 );
	return datelocation;
}

/*****************************************************
**
**   DateLocationLoader   ---   loadMtxFile
**
******************************************************/
DateLocation DateLocationLoader::loadMtxFile( const wxString &filename )
{
	constexpr size_t min_line_count = 11;

	int day, month, year;
	double time, timezone, longitude, latitude, dst;
	wxString line;
	wxString locname;

	DateLocation datelocation( filename );

	day = month = year = 0;
	time = timezone = longitude = latitude = dst = 0.0;

	wxTextFile file( filename );
  if ( ! file.Open())
  {
    throw std::invalid_argument( "cannot open file" );
  }
	if ( file.GetLineCount() < min_line_count ) throw std::length_error( "file too short" );

  // Commented line
  line = file.GetFirstLine();
  if ( ! line.StartsWith( "#" ))
  {
    throw std::invalid_argument( "wrong file format: first line must start with \"#\"" );
  }
  datelocation.name = file.GetNextLine();

  // day
  line = file.GetNextLine();
  if ( ! stringToInt( line, day ) ||  day <= 0 || day > 31 ) throw std::out_of_range( "invalid day param" );

	// month
  line = file.GetNextLine();
	if ( ! stringToInt( line, month ) ||  month <= 0 || month > 12 ) throw std::out_of_range( "invalid month param" );

	// year
  line = file.GetNextLine();
	if ( ! stringToInt( line, year ) ||  year <= -5001 || year > 10000 ) throw std::out_of_range( "invalid year param" );

	// time
  line = file.GetNextLine();
	if ( ! stringToDouble( line, time )) throw std::out_of_range( "invalid time param" );
	if ( time < 0 || time > 24.0 ) throw std::out_of_range( "invalid time param" );

	// time zome
  line = file.GetNextLine();
	if ( ! stringToDouble( line, timezone ) || timezone < -12 || timezone > 12.0 )
		throw std::out_of_range( "invalid time zone param" );
	time -= timezone;

	// dst
  line = file.GetNextLine();
	if ( ! stringToDouble( line, dst ) || dst < -12 || dst > 12.0 )
		throw std::out_of_range( "invalid dst param" );
	time -= dst;

	// location name
  locname = file.GetNextLine();

	// longitude
  line = file.GetNextLine();
	if ( ! stringToDouble( line, longitude ) || longitude < -180.0 || longitude > 180.0 )
		throw std::out_of_range( "invalid longitude param" );

	// latitude
  line = file.GetNextLine();
	if ( ! stringToDouble( line, latitude ) || latitude < -180.0 || latitude > 180.0 )
		throw std::out_of_range( "invalid latitude param" );

	datelocation.date.set( year, month, day, time );
	datelocation.location.set( locname, longitude, latitude, timezone, dst );

	// remark
	while( ! file.Eof())
	{
		datelocation.remark << file.GetNextLine() << Endl;
	}
	return datelocation;
}


