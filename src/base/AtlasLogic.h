/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/AtlasLogic.h
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

#ifndef ATLASLOGIC_H
#define ATLASLOGIC_H

#include "AtlasDao.h"
#include "constants.h"

#include <list>
#include <map>
#include <vector>
#include <wx/event.h>

/*************************************************//**
*
*
*
******************************************************/
class AtlasLogic
{
	friend class AtlasLogicWorker;
	friend class AtlasLogicCountWorker;
	friend class AtlasLogicFetchWorker;

public:

	AtlasLogic( const bool runWorkerThreads = false );
	~AtlasLogic();

	std::list<AtlasCountry> getAllCountries();
	std::list<AtlasCountry> getFavouriteCountries( std::vector<wxString> );
	wxString getCountryCodeForName( wxString name );

	std::list<TimezoneEntry> getAllTimezones();

	std::list<wxString> getAllAdminNamesForCountry( const wxString &country_code );
	wxString getAdminCodeForCountryAndName( wxString country_code, wxString name );

	void setFilterConditions( wxString f, wxString c, const int& m );
	void updateFilter();

	AtlasEntry *getEntryByRowId( const int &rowid );
	AtlasEntry getFullEntry( const int &featureid );

	bool countHasNews();
	int getCount();
	void aknowledgeCountHasNews();

	bool fetchHasNews();
	void aknowledgeFetchHasNews();

	void saveEntry( AtlasEntry& );
	void deleteEntry( const int& id );

private:

	class AtlasLogicSharedSection *sharedSection;
	class AtlasLogicCountWorker *countWorker;
	class AtlasLogicFetchWorker *fetchWorker;
	class AtlasLogicLifeCycleWorker *lifeCycleWorker;

	class AtlasDao *dao;
	std::map<wxString, wxString> countrynames;
	std::map<wxString, wxString> featurenames;
	std::map<wxString, wxString> adminnames;

	wxString getCountryName( wxString iso );
	wxString getAdminName( wxString country_code, wxString admin1_code );
	wxString getFeatureName( wxString feature_class, wxString feature_code );
	void resetEntries();

	wxString country;
	wxString filter;
	int mode;
	AtlasEntry *entries[ATLAS_MAX_GRID_ELEMENTS];

protected:


};


#endif

