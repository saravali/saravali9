/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/AtlasDao.h
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

#ifndef ATLASDAO_H
#define ATLASDAO_H

#include "BaseDao.h"

/*************************************************//**
*
* Struct for atlas data transport.
*
******************************************************/
struct AtlasEntry
{
	int id = 0;

	int rowid = 0;

	wxString name;

	wxString asciiname;

	wxString aliases;

	wxString country_code;

	wxString country;

	wxString feature_code;

	wxString feature_class;

	wxString feature_name;

	wxString admin1_code;

	wxString admin;

	int population = 0;

	int elevation = 0;

	double longitude = 0;

	double latitude = 0;

	wxString timezone;

	double tzoffset = 0;
};

/*************************************************//**
*
*
*
******************************************************/
struct AtlasCountry
{
	wxString iso;
	wxString name;
};

/*************************************************//**
*
*
*
******************************************************/
struct TimezoneEntry
{
	wxString name;

	wxString country_code;

	double tzoffset = 0;;
};

/*************************************************//**
*
*
*
******************************************************/
class AtlasDao : public BaseDao
{
public:

	AtlasDao( const wxString s = wxEmptyString ) : BaseDao( s ) {}

	std::vector<AtlasEntry> getEntries( wxString filter, wxString country,
		const int &mode, const int &limit, const int &offset );

	AtlasEntry getFullEntry( int id );

	int getMatchCount( wxString filter, wxString country, const int &mode );

	std::list<AtlasCountry> getAllCountries();

	wxString getCountryName( wxString iso );

	wxString getFeatureName( wxString feature_class, wxString feature_code );

	wxString getAdminName( wxString country_code, wxString admin1_code );

	wxString getCountryCodeForName( wxString name );

	wxString getAdminCodeForCountryAndName( wxString country_code, wxString name );

	std::list<wxString> getAllAdminNamesForCountry( const wxString &country_code );

	std::list<TimezoneEntry> getAllTimezones();

  void saveEntry( AtlasEntry& );

	void saveAliasNames( const int &id, wxString country_code, wxString names );

	void saveExistingEntry( const AtlasEntry &entry );

	void saveNewEntry(  AtlasEntry &entry );

	void deleteEntry( const int& id );

private:

	bool entryExists( const int &id );

	int getNewFeatureId();

};


#endif

