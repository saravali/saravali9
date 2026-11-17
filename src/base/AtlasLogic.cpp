/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/AtlasLogic.cpp
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

#include "AtlasLogic.h"

#include "AtlasDao.h"
#include "gdiconfig.h"
#include "logger.h"
#include "mathbase.h"

#include <wx/timer.h>
#include <wx/thread.h>

extern Gdiconfig *gdiconfig;

#define COUNT_THREAD_SLEEP_MILLISEC 50
#define FETCH_THREAD_SLEEP_MILLISEC 50
#define LIFECYCLE_THREAD_SLEEP_MILLISEC 5

//#define DEB_ATLAS_LOGIC

static wxMutex theMutex;

/*************************************************//**
*
*
*
******************************************************/
class AtlasLogicSharedSection
{
public:
	AtlasLogicSharedSection()
	{
		init();
	}

	void init()
	{
		countHasNews = countHasOrder = false;
		fetchHasNews = fetchHasOrder = false;
		count = 0;
		fetchOffset = 0;
		fetchIsWorking = false;
		doExit = false;
	}

	bool countHasNews;
	bool countHasOrder;
	int count;

	bool fetchHasNews;
	bool fetchHasOrder;
	bool fetchIsWorking;
	int fetchOffset;

	bool doExit;
};

/*************************************************//**
*
*
*
******************************************************/
class AtlasLogicWorker : public wxThread
{
public:

	AtlasLogicWorker( AtlasLogic *logic )
		: wxThread( wxTHREAD_JOINABLE  ),
			logic( logic )
	{
		this->sharedSection = logic->sharedSection;
		assert( sharedSection );
		this->dao = new AtlasDao();
		//SetPriority( WXTHREAD_DEFAULT_PRIORITY );
	}

	~AtlasLogicWorker() {
		delete this->dao;
	}

protected:
	AtlasLogic *logic;

	AtlasLogicSharedSection *sharedSection;
	AtlasDao *dao;
};

/*************************************************//**
*
*   Worker for grid table entry calculation
*
******************************************************/
class AtlasLogicFetchWorker : public AtlasLogicWorker
{
public:
	AtlasLogicFetchWorker( AtlasLogic *logic ) : AtlasLogicWorker( logic ) {}

	/*****************************************************
	**
	**   AtlasLogicFetchWorker   ---   Entry
	**
	******************************************************/
	ExitCode Entry()
	{
		while ( sharedSection->doExit == false )
		{
			Sleep( FETCH_THREAD_SLEEP_MILLISEC );
			if ( sharedSection->fetchHasOrder )
			{
				sharedSection->fetchIsWorking = true;
#ifdef DEB_ATLAS_LOGIC
				debuglog << "AtlasLogicFetchWorker: location fetch started, filter " << logic->filter
					<< " country " << logic->country << " mode " << logic->mode << " offset " <<  sharedSection->fetchOffset;
				wxLongLong starttime = wxGetLocalTimeMillis();
#endif

				sharedSection->fetchHasOrder = false;
				logic->resetEntries();

				std::vector<AtlasEntry> l = dao->getEntries( logic->filter, logic->country, logic->mode,
					ATLAS_MAX_GRID_ELEMENTS, sharedSection->fetchOffset );

				if ( l.size() > 0 )
				{
					theMutex.Lock();
					for ( uint i = 0; i < l.size(); i++ )
					{
						l[i].country = logic->getCountryName( l[i].country_code );
						l[i].admin = logic->getAdminName( l[i].country_code, l[i].admin1_code );

						logic->entries[i] = new AtlasEntry( l[i] );
					}
#ifdef DEB_ATLAS_LOGIC
					wxLongLong duration = wxGetLocalTimeMillis() - starttime;
					debuglog << "AtlasLogicFetchWorker: location fetch finished, " << l.size() << " results in "
						<< duration.ToLong() << " millisec";
#endif

					if ( sharedSection->fetchHasOrder )
					{
						// filter conditions have changed meanwhile: do not write results
						warnlog << "AtlasLogicFetchWorker has new order before finishing request";
					}
					else
					{
						sharedSection->fetchHasNews = true;
						sharedSection->fetchIsWorking = false;
					}
					theMutex.Unlock();
				}
			}
		}
		return 0;
	}
};

/*************************************************//**
*
*   Worker for calculation of the number of matching records
*
******************************************************/
class AtlasLogicCountWorker : public AtlasLogicWorker
{
public:
	AtlasLogicCountWorker( AtlasLogic *logic ) : AtlasLogicWorker( logic ) {}

	/*****************************************************
	**
	**   AtlasLogicCountWorker   ---   Entry
	**
	******************************************************/
	ExitCode Entry()
	{
		while ( sharedSection->doExit == false )
		{
			Sleep( COUNT_THREAD_SLEEP_MILLISEC );
			if ( sharedSection->countHasOrder )
			{
#ifdef DEB_ATLAS_LOGIC
				debuglog << "AtlasLogicCountWorker: location count started, filter " << logic->filter
					<< " country " << logic->country << " mode " << logic->mode;
				wxLongLong starttime = wxGetLocalTimeMillis();
#endif
				sharedSection->countHasOrder = false;
				int c = dao->getMatchCount( logic->filter, logic->country, logic->mode );

				if ( sharedSection->countHasOrder )
				{
					// new order arrived, filter conditions have changed meanwhile: do not write results
					warnlog << "AtlasLogicCountWorker has new order before finishing request";
				}
				else
				{
					theMutex.Lock();
					sharedSection->countHasNews = true;
					sharedSection->count = c;
					theMutex.Unlock();
#ifdef DEB_ATLAS_LOGIC
					wxLongLong duration = wxGetLocalTimeMillis() - starttime;
					debuglog << "AtlasLogicCountWorker: Location count finished, " << c << " results in "
						<< duration.ToLong() << " millisec";
#endif
				}
			}
		}
		return 0;
	}
};

/*************************************************//**
*
* 
*
******************************************************/
class AtlasLogicLifeCycleWorker : public AtlasLogicWorker
{
public:
	AtlasLogicLifeCycleWorker( AtlasLogic *logic ) : AtlasLogicWorker( logic ) {}

	/*****************************************************
	**
	**   AtlasLogicLifeCycleWorker   ---   Entry
	**
	******************************************************/
	ExitCode Entry()
	{
		while ( true )
		{
			Sleep( LIFECYCLE_THREAD_SLEEP_MILLISEC );
			if ( sharedSection->doExit )
			{
#ifdef DEB_ATLAS_LOGIC
				debuglog << "AtlasLogicLifeCycleWorker received interrupt order";
#endif
				dao->interruptAllQueries();
#ifdef DEB_ATLAS_LOGIC
				infolog << "AtlasLogicLifeCycleWorker: DB operations canceled";
#endif
				break;
			}
		}
		return 0;
	}
};

/*****************************************************
**
**   AtlasLogic   ---   Constructor
**
******************************************************/
AtlasLogic::AtlasLogic( const bool runWorkerThreads )
{
	dao = new AtlasDao();
	for ( int i = 0; i < ATLAS_MAX_GRID_ELEMENTS; i++ ) entries[i] = 0;

	sharedSection = 0;
	fetchWorker = 0;
	countWorker = 0;
	lifeCycleWorker = 0;

	if ( runWorkerThreads )
	{
		sharedSection = new AtlasLogicSharedSection;

		lifeCycleWorker = new AtlasLogicLifeCycleWorker( this );
		wxThreadError te1 = lifeCycleWorker->Create();
		if ( te1 != wxTHREAD_NO_ERROR )
		{
			errorlog << "AtlasLogic ERROR: cannot create lifeCycleWorker thread, error code " << te1;
		}
		wxThreadError te2 = lifeCycleWorker->Run();
		if ( te2 != wxTHREAD_NO_ERROR )
		{
			errorlog << "AtlasLogic ERROR: cannot start lifeCycleWorker thread, error code " << te2;
		}
#ifdef DEB_ATLAS_LOGIC
		debuglog << "AtlasLogicLifeCycleWorker started";
#endif

		countWorker = new AtlasLogicCountWorker( this );
		wxThreadError te3 = countWorker->Create();
		if ( te3 != wxTHREAD_NO_ERROR )
		{
			errorlog << "AtlasLogic: cannot create countWorker thread, error code " << te1;
		}
		wxThreadError te4 = countWorker->Run();
		if ( te4 != wxTHREAD_NO_ERROR )
		{
			errorlog << "AtlasLogic: cannot start countWorker thread, error code " << te2;
		}
#ifdef DEB_ATLAS_LOGIC
		debuglog << "AtlasLogicCountWorker started";
#endif

		fetchWorker = new AtlasLogicFetchWorker( this );
		wxThreadError te5 = fetchWorker->Create();
		if ( te5 != wxTHREAD_NO_ERROR )
		{
			errorlog << "AtlasLogic: cannot create fetchWorker thread, error code " << te3;
		}
		wxThreadError te6 = fetchWorker->Run();
		if ( te6 != wxTHREAD_NO_ERROR )
		{
			errorlog << "AtlasLogic: cannot start fetchWorker thread, error code " << te4;
		}
#ifdef DEB_ATLAS_LOGIC
		infolog << "AtlasLogicFetchWorker started";
#endif
	}
}

/*****************************************************
**
**   AtlasLogic   ---   Destructor
**
******************************************************/
AtlasLogic::~AtlasLogic()
{
	if (  sharedSection )  sharedSection->doExit = true;

	if ( lifeCycleWorker )
	{
		if ( lifeCycleWorker->IsRunning())
		{
#ifdef DEB_ATLAS_LOGIC
			debuglog << "AtlasLogic destructor waiting for life cycle worker to join ...";
			wxLongLong starttime = wxGetLocalTimeMillis();
#endif
			lifeCycleWorker->Wait();
#ifdef DEB_ATLAS_LOGIC
			wxLongLong duration = wxGetLocalTimeMillis() - starttime;
			infolog << "Life cycle worker stopped in " << duration.ToLong() << " millisec";
#endif
		}
#ifdef DEB_ATLAS_LOGIC
		else debuglog << "AtlasLogic destructor: life cycle worker not running";
#endif
		delete lifeCycleWorker;
	}

	if ( countWorker )
	{
		if ( countWorker->IsRunning())
		{
#ifdef DEB_ATLAS_LOGIC
			debuglog << "AtlasLogic destructor waiting for count worker to join ...";
			wxLongLong starttime = wxGetLocalTimeMillis();
#endif
			countWorker->Wait();
#ifdef DEB_ATLAS_LOGIC
			wxLongLong duration = wxGetLocalTimeMillis() - starttime;
			infolog << "Count worker stopped in " << duration.ToLong() << " millisec";
#endif
		}
#ifdef DEB_ATLAS_LOGIC
		else debuglog << "AtlasLogic destructor: count worker not running";
#endif
		delete countWorker;
	}

	if ( fetchWorker )
	{
		if ( fetchWorker->IsRunning())
		{
#ifdef DEB_ATLAS_LOGIC
			debuglog << "AtlasLogic destructor waiting for fetch worker to join ...";
			wxLongLong starttime = wxGetLocalTimeMillis();
#endif
			fetchWorker->Wait();
#ifdef DEB_ATLAS_LOGIC
			wxLongLong duration = wxGetLocalTimeMillis() - starttime;
			infolog << "Fetch worker stopped in " << duration.ToLong() << " millisec";
#endif
		}
#ifdef DEB_ATLAS_LOGIC
		else debuglog << "AtlasLogic destructor: fetch worker not running";
#endif
		delete fetchWorker;
	}

	// free memory of list elements
	resetEntries();

	if ( sharedSection ) delete sharedSection;
	delete dao;
}

/*****************************************************
**
**   AtlasLogic   ---   getFavouriteCountries
**
******************************************************/
std::list<AtlasCountry> AtlasLogic::getFavouriteCountries( std::vector<wxString> countries )
{
	wxString name;
	std::list<AtlasCountry> l;

	if ( countries.size() == 0 )
	{
		for( const auto &c : gdiconfig->atlas.favouriteCountries )
		{
			countries.push_back( c );
		}
	}

	for( uint i = 0; i < countries.size(); i++ )
	{
		name = dao->getCountryName( countries[i] );
		if ( ! name.IsEmpty())
		{
			l.push_back( AtlasCountry( countries[i], name ));
		}
	}
	l.sort( []( const AtlasCountry &c1, const AtlasCountry &c2 ) -> bool { return ( c1.name < c2.name ); } );
	return l;
}

/*****************************************************
**
**   AtlasLogic   ---   getAllCountries
**
******************************************************/
std::list<AtlasCountry> AtlasLogic::getAllCountries()
{
	return dao->getAllCountries();
}

/*****************************************************
**
**   AtlasLogic   ---   getAllTimezones
**
******************************************************/
std::list<TimezoneEntry> AtlasLogic::getAllTimezones()
{
	return dao->getAllTimezones();
}

/*****************************************************
**
**   AtlasLogic   ---   getFeatureName
**
******************************************************/
wxString AtlasLogic::getFeatureName( wxString feature_class, wxString feature_code )
{
	wxString name;

	if ( feature_class.IsEmpty()) return wxEmptyString;
	if ( feature_code.IsEmpty()) return wxEmptyString;

	wxString key = feature_class + "." + feature_code;
	if ( featurenames.find( key ) == featurenames.end())
	{
		name = dao->getFeatureName( feature_class, feature_code );
		featurenames[key] = name;
	}
	else
	{
		name = featurenames[key];
	}
	return name;
}

/*****************************************************
**
**   AtlasLogic   ---   getCountryName
**
******************************************************/
wxString AtlasLogic::getCountryName( wxString iso )
{
	wxString name;

	if ( iso.IsEmpty()) return wxEmptyString;
	if ( countrynames.find( iso ) == countrynames.end())
	{
		name = dao->getCountryName( iso );
		countrynames[ iso ] = name;
	}
	else
	{
		name = countrynames[iso];
	}
	return name;
}

/*****************************************************
**
**   AtlasLogic   ---   getCountryCodeForName
**
******************************************************/
wxString AtlasLogic::getCountryCodeForName( wxString name )
{
	if ( name.IsEmpty()) return wxEmptyString;
	return dao->getCountryCodeForName( name );
}

/*****************************************************
**
**   AtlasLogic   ---   getAdminCodeForCountryAndName
**
******************************************************/
wxString AtlasLogic::getAdminCodeForCountryAndName( wxString country_code, wxString name )
{
	if ( name.IsEmpty()) return wxEmptyString;
	return dao->getAdminCodeForCountryAndName( country_code, name );
}

/*****************************************************
**
**   AtlasLogic   ---   getAdminName
**
******************************************************/
wxString AtlasLogic::getAdminName( wxString country_code, wxString admin1_code )
{
	wxString name;
	wxString key;
	if ( ! admin1_code.IsEmpty())
	{
		key = country_code + "." + admin1_code;
		if ( adminnames.find( key ) == adminnames.end())
		{
			name = dao->getAdminName( country_code, admin1_code );
			adminnames[ key ] = name;
		}
		else
		{
			name = adminnames[ key ];
		}
	}
	return name;
}

/*****************************************************
**
**   AtlasLogic   ---   getAllAdminNamesForCountry
**
******************************************************/
std::list<wxString> AtlasLogic::getAllAdminNamesForCountry( const wxString &country_code )
{
	return dao->getAllAdminNamesForCountry( country_code );
}

/*****************************************************
**
**   AtlasLogic   ---   resetEntries
**
******************************************************/
void AtlasLogic::resetEntries()
{
	for ( int i = 0; i < ATLAS_MAX_GRID_ELEMENTS; i++ )
	{
		if ( entries[i] ) delete entries[i];
		entries[i] = 0;
	}
}

/*****************************************************
**
**   AtlasLogic   ---   updateFilter
**
******************************************************/
void AtlasLogic::updateFilter()
{
	theMutex.Lock();
	resetEntries();

	assert( sharedSection );
	sharedSection->countHasOrder = true;
	sharedSection->fetchHasOrder = true;
	theMutex.Unlock();
}

/*****************************************************
**
**   AtlasLogic   ---   setFilterConditions
**
******************************************************/
void AtlasLogic::setFilterConditions( wxString f, wxString c, const int& m )
{
	filter = f.Trim( true ).Trim( false );
	filter.Replace( "*", "%" );
	filter.Replace( "?", "_" );
	country = c;
	mode = m;

	assert( sharedSection );
	sharedSection->fetchOffset = 0;
	updateFilter();
}

/*****************************************************
**
**   AtlasLogic   ---   getEntryByRowId
**
******************************************************/
AtlasEntry *AtlasLogic::getEntryByRowId( const int &rowid )
{
	assert( sharedSection );

	theMutex.Lock();
	for ( int i = 0; i < ATLAS_MAX_GRID_ELEMENTS; i++ )
	{
		if ( entries[i] != 0 && entries[i]->rowid == rowid )
		{
			theMutex.Unlock();
			return entries[i];
		}
	}
	theMutex.Unlock();

	if ( rowid > 0 && ! sharedSection->fetchHasOrder && ! sharedSection->fetchIsWorking )
	{
		theMutex.Lock();
		resetEntries();
		sharedSection->fetchOffset = std::max( 0, rowid - 15 );
		sharedSection->fetchHasOrder = true;
		theMutex.Unlock();
	}
	return 0;
}

/*****************************************************
**
**   AtlasLogic   ---   getFullEntry
**
******************************************************/
AtlasEntry AtlasLogic::getFullEntry( const int &featureid )
{
	AtlasEntry entry = dao->getFullEntry( featureid );
	entry.country = getCountryName( entry.country_code );
	entry.admin = getAdminName( entry.country_code, entry.admin1_code );
	return entry;
}

bool AtlasLogic::countHasNews()
{
	assert( sharedSection );
	return sharedSection->countHasNews;
}
int AtlasLogic::getCount()
{
	assert( sharedSection );
	return sharedSection->count;
}
void AtlasLogic::aknowledgeCountHasNews()
{
	assert( sharedSection );
	sharedSection->countHasNews = false;
}

bool AtlasLogic::fetchHasNews()
{
	assert( sharedSection );
	return sharedSection->fetchHasNews;
}
void AtlasLogic::aknowledgeFetchHasNews()
{
	assert( sharedSection );
	sharedSection->fetchHasNews = false;
}

/*****************************************************
**
**   AtlasLogic   ---   saveEntry
**
******************************************************/
void AtlasLogic::saveEntry( AtlasEntry &entry )
{
	dao->saveEntry( entry );
}

/*****************************************************
**
**   AtlasLogic   ---   deleteEntry
**
******************************************************/
void AtlasLogic::deleteEntry( const int& id )
{
	dao->deleteEntry( id );
}

