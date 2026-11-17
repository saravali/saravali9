/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/datelocation.h
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
#ifndef _DATELOCATION_H_
#define _DATELOCATION_H_

#include "constants.h"

#include <wx/string.h>

/*************************************************//**
*
*  simple data class
*
******************************************************/
class MDate
{
public:

	MDate();

	MDate( const double &jd );

	MDate( const int &year, const int &month, const int &day, const double time = 0.0 );

	MDate( const wxString& );

	static double getCurrentJD();

	void setCurrentDate();

	void addOneMonth();

	void set( const int &year, const int &month, const int &day, const double &time );

	friend mostream& operator<< ( mostream&, const MDate& );

	double jd;
};

/*************************************************//**
*
*  contains data of the location of a chart
*
******************************************************/
struct Location : public Jsonable
{
	Location() {}

	Location( const Location& ) = default;

	Location( const wxString& );

	Location& operator= ( const Location& ) = default;

	virtual ~Location() {}

	void set( wxString locName, const double &longitude, const double &latitude,
		const double &timeZone, const double &dst, const DaylightSavingMode = DaylightSavingMode::Automatic );

	friend mostream& operator<< ( mostream&, const Location& );

	// json:start
	double latitude = 0.0;
	double longitude = 0.0;
	double tz = 0.0;
	double dst = 0.0;
	wxString locName;
	DaylightSavingMode dstMode = DaylightSavingMode::Automatic;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*  data for a chart (date, location, name etc.)
*
******************************************************/
struct DateLocation
{
	DateLocation( const wxString filename = "" );

	int getWeekDay() const;

	double getLocalMeanTime();

	friend mostream& operator<< ( mostream&, const DateLocation& );

	wxString name;

	MDate date;

	Location location;

	wxString remark;

	wxString filename;

	bool dirty = false;
};

/*************************************************//**
*
*  File IO for DateLocation
*
******************************************************/
class DateLocationLoader
{
public:

	DateLocation loadFile( const wxString&, const bool isAtxFile = false );

	void save( const DateLocation& );

private:

	DateLocation loadAtxFile( const wxString& );

	DateLocation loadMtxFile( const wxString& );
};

#endif // _DATELOCATION_H_


