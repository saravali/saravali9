/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/AtlasImporter.cpp
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

#include "AtlasImporter.h"

#include "AtlasDao.h"
#include "FileConfig.h"
#include "logger.h"
#include "mathbase.h"

#include <wx/filename.h>
#include <wx/stopwatch.h>
#include <wx/string.h>
#include <wx/textfile.h>
#include <wx/txtstrm.h>
#include <wx/thread.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#include <format>

#define IMPORT_THREAD_SLEEP_MILLISEC 100
#define LIFECYCLE_THREAD_SLEEP_MILLISEC 5

#define QUERY_BUNDLESIZE 100

#define THREE_DOTS " ..."

static wxMutex theMutex;

/*************************************************//**
*
*
*
******************************************************/
class AtlasImportSharedSection
{
public:
	AtlasImportSharedSection()
	{
		init();
	}

	void init()
	{
		threadOrder = ThreadOrder::None;
		threadStatus = ThreadStatus::Waiting;

		progress = totalSize = readSize = currentLine = 0;
		errorCount = 0;
		hasNews = false;

		sqlfile.Clear();
		dbfile.Clear();
		errorMessage.Clear();
	}

	long progress;
	long totalSize;
	long readSize;
	long currentLine;
	int errorCount;
	wxString errorMessage;

	ThreadOrder threadOrder;
	ThreadStatus threadStatus;
	bool hasNews;

	wxString sqlfile;
	wxString dbfile;
};

/*************************************************//**
*
* 
*
******************************************************/
class AtlasImportLifeCycleWorker : public wxThread
{
public:
	AtlasImportLifeCycleWorker( AtlasImportSharedSection *sharedSection )
			: wxThread( wxTHREAD_JOINABLE ),
			sharedSection( sharedSection )
	{
		//SetPriority( WXTHREAD_DEFAULT_PRIORITY );
		sharedSection->threadStatus = ThreadStatus::Waiting;
	}

	/*****************************************************
	**
	**   AtlasImportLifeCycleWorker   ---   Entry
	**
	******************************************************/
	ExitCode Entry()
	{
		while ( true )
		{
			Sleep( LIFECYCLE_THREAD_SLEEP_MILLISEC );
			if ( sharedSection->threadOrder == ThreadOrder::Exit )
			{
				infolog << "AtlasImportLifeCycleWorker received interrupt order";
				dao = new AtlasDao( sharedSection->dbfile );
				dao->interruptAllQueries();
				infolog << "AtlasImportLifeCycleWorker: DB operations canceled";
				delete dao;
				break;
			}
		}
		return 0;
	}

private:
	AtlasImportSharedSection *sharedSection;
	AtlasDao *dao;
};

/*************************************************//**
*
*
*
******************************************************/
class AtlasImportWorker : public wxThread
{
public:

	AtlasImportWorker( AtlasImportSharedSection *sharedSection )
			: wxThread( wxTHREAD_JOINABLE ),
			sharedSection( sharedSection )
	{
		//SetPriority( WXTHREAD_DEFAULT_PRIORITY );
		sharedSection->threadStatus = ThreadStatus::Waiting;
	}

	~AtlasImportWorker()
	{
		sharedSection->threadStatus = ThreadStatus::Finished;
	}

	/*****************************************************
	**
	**   AtlasImportWorker   ---   Entry
	**
	******************************************************/
	ExitCode Entry()
	{
		sharedSection->threadStatus = ThreadStatus::Waiting;

		while ( sharedSection->threadOrder != ThreadOrder::Exit )
		{
			Sleep( IMPORT_THREAD_SLEEP_MILLISEC );
			if ( sharedSection->threadOrder == ThreadOrder::Work )
			{
				TestDestroy();
				sharedSection->progress = 0;
				sharedSection->threadOrder = ThreadOrder::None;
				sharedSection->threadStatus = ThreadStatus::Working;
				sharedSection->errorCount = 0;
				totalsize = wxFileName::GetSize( sharedSection->sqlfile ).ToULong();
				doit();

				// set status to finished if not canceled or error
				if (  sharedSection->threadStatus == ThreadStatus::Working )
				{
					sharedSection->threadStatus = ThreadStatus::Finished;
				}
				sharedSection->hasNews = true;
			}
		}
		return 0;
	}

private:

	/*****************************************************
	**
	**   AtlasImportWorker   ---   doit
	**
	******************************************************/
	int doit()
	{
		if ( wxFileName::Exists( sharedSection->dbfile ))
		{
			FileConfig::get()->backupFile( sharedSection->dbfile );
			wxRemoveFile( sharedSection->dbfile );
		}

		dao = new AtlasDao( sharedSection->dbfile );
		sharedSection->progress = 0;
		sharedSection->hasNews = true;
		currentQuery.Clear();
		qb.clear();

		const int ret = readSqlFile();

		delete dao;
		return ret;
	}

	/*****************************************************
	**
	**   AtlasImportWorker   ---   readSqlFile
	**
	******************************************************/
	int readSqlFile()
	{
		wxString buf;
		wxTextFile textfile;

		if ( ! textfile.Open( sharedSection->sqlfile, wxConvUTF8 ))
		{
			errorlog << "cannot open file filename " << sharedSection->sqlfile;
			sharedSection->errorCount++;
			return 1;
		}

		sharedSection->totalSize = (long)wxFileName::GetSize( sharedSection->sqlfile ).ToULong();

		for ( buf = textfile.GetFirstLine(); ! textfile.Eof(); buf = textfile.GetNextLine())
		{
			sharedSection->currentLine++;
			if ( checkCancelOrExit() ) break;
			addQueryPart( buf );
			if ( checkCancelOrExit() ) break;
		}
		textfile.Close();
		if ( qb.size() > 0 ) execQueryBundle();
		return 0;
	}


	/*****************************************************
	**
	**   AtlasImportWorker   ---   addQueryPart
	**
	******************************************************/
	void addQueryPart( wxString buf )
	{
		buf = buf.Trim( true ).Trim( false );

		// Comment, skip
		if ( buf.StartsWith( "--" )) return;

		if ( ! currentQuery.IsEmpty()) currentQuery << " ";
		currentQuery << buf;

		if ( buf.Find( ';' ) != wxNOT_FOUND )
		{
			sharedSection->readSize += buf.Length();
			sharedSection->readSize += 2;
			sharedSection->progress = (long)(100.0 * (double)sharedSection->readSize / (double)sharedSection->totalSize);
			qb.push_back( currentQuery );
			currentQuery.Clear();
			if ( qb.size() > 0 &&  ( qb.size() % QUERY_BUNDLESIZE ) == 0 )
			{
				execQueryBundle();
			}
		}
	}

	/*****************************************************
	**
	**   AtlasImportWorker   ---   execQueryBundle
	**
	******************************************************/
	int execQueryBundle()
	{
		int ret = dao->executeQueryBundle( qb, false );
		if( ret != (int)qb.size())
		{
			theMutex.Lock();
			sharedSection->errorCount++;
			sharedSection->errorMessage = dao->getLastErrorMessage();
			sharedSection->threadStatus = ThreadStatus::Error;
			theMutex.Unlock();
		}
		qb.clear();
		return ret;
	}

	/*****************************************************
	**
	**   AtlasImportWorker   ---   checkCancelOrExit
	**
	******************************************************/
	bool checkCancelOrExit()
	{
		if ( sharedSection->threadOrder == ThreadOrder::Exit )
		{
			wxRemoveFile( sharedSection->dbfile );
			sharedSection->threadStatus = ThreadStatus::Finished;
			return true;
		}
		else if ( sharedSection->threadOrder == ThreadOrder::Cancel )
		{
			wxRemoveFile( sharedSection->dbfile );
			sharedSection->threadStatus = ThreadStatus::Canceled;
			sharedSection->threadOrder = ThreadOrder::None;
			return true;
		}
		else if ( sharedSection->errorCount > 0 )
		{
			wxRemoveFile( sharedSection->dbfile );
			sharedSection->threadStatus = ThreadStatus::Error;
			sharedSection->threadOrder = ThreadOrder::None;
			return true;
		}
		return false;
	}

	AtlasImportSharedSection *sharedSection;
	unsigned long totalsize;
	AtlasDao *dao;
	wxString currentQuery;
	std::list<wxString> qb;
};


/*****************************************************
**
**   AtlasImporter   ---   Constructor
**
******************************************************/
AtlasImporter::AtlasImporter()
{
	sharedSection = new AtlasImportSharedSection;
	lifeCycleWorker = new AtlasImportLifeCycleWorker( sharedSection );
	wxThreadError te1 = lifeCycleWorker->Create();
	if ( te1 != wxTHREAD_NO_ERROR )
	{
		errorlog << "AtlasImporter: create life cycle thread: code " << te1;
	}
	wxThreadError te2 = lifeCycleWorker->Run();
	if ( te2 != wxTHREAD_NO_ERROR )
	{
		errorlog << "AtlasImporter: run life cycle thread: code " << te2;
	}
	debuglog << "Atlas life cycle thread started";

	importWorker = new AtlasImportWorker( sharedSection );
	wxThreadError te3 = importWorker->Create();
	if ( te3 != wxTHREAD_NO_ERROR )
	{
		errorlog << "AtlasImporter: create import thread: code " << te3;
	}
	wxThreadError te4 = importWorker->Run();
	if ( te4 != wxTHREAD_NO_ERROR )
	{
		errorlog << "AtlasImporter: run import thread: code " << te4;
	}
	debuglog << "AtlasImportWorker started";
}

/*****************************************************
**
**   AtlasImporter   ---   Destructor
**
******************************************************/
AtlasImporter::~AtlasImporter()
{
	sharedSection->threadOrder = ThreadOrder::Exit;
	if ( importWorker->IsRunning())
	{
		debuglog << "AtlasImportDialog destructor waiting for import worker to join ...";
		wxLongLong starttime = wxGetLocalTimeMillis();
		importWorker->Wait();
		wxLongLong duration = wxGetLocalTimeMillis() - starttime;
		infolog << "Import worker stopped in " << duration.ToLong() << " millisec";
	}
	delete importWorker;

	delete sharedSection;
}
	
/*****************************************************
**
**   AtlasImporter   ---   various
**
******************************************************/
ThreadStatus AtlasImporter::getImportStatus()
{
	assert( sharedSection );
	return sharedSection->threadStatus;
}
bool AtlasImporter::hasNews()
{
	assert( sharedSection );
	return sharedSection->hasNews;
}
void AtlasImporter::aknowledgeNews()
{
	assert( sharedSection );
	sharedSection->hasNews = false;
}
void AtlasImporter::abort()
{
	assert( sharedSection );
	sharedSection->threadOrder = ThreadOrder::Cancel;
}
void AtlasImporter::run()
{
	assert( sharedSection );
	sharedSection->threadOrder = ThreadOrder::Work;
}
void AtlasImporter::setImportFile( wxString s )
{
	assert( sharedSection );
	theMutex.Lock();
	sharedSection->sqlfile = s;
	theMutex.Unlock();
}
void AtlasImporter::setDatabaseFile( wxString s )
{
	assert( sharedSection );
	theMutex.Lock();
	sharedSection->dbfile = s;
	theMutex.Unlock();
}

/*****************************************************
**
**   AtlasImporter   ---   writeErrorStatus
**
******************************************************/
wxString AtlasImporter::writeErrorStatus()
{
	wxString s;
	if ( sharedSection->errorCount > 0 )
	{
			s << "Database filename: " << sharedSection->dbfile << "\n"
			<< "SQL filename: " << sharedSection->sqlfile << "\n"
			<< "Error message: " << sharedSection->errorMessage;
	}
	else
	{
		s = _( "No error" );
	}

	debuglog << "AtlasImporter::writeErrorStatus: " << s;
	return s;
}

/*****************************************************
**
**   AtlasImporter   ---   writeRunStatus
**
******************************************************/
wxString AtlasImporter::writeRunStatus()
{
	wxString label;
	if ( sharedSection->threadStatus == ThreadStatus::Working )
	{
		if ( sharedSection->progress == 0 )
		{
			label << tr( "Reading file" ) << THREE_DOTS;
		}
		else if ( sharedSection->progress < 99 )
		{
			label = std::format( "{}: {}%", tr( "Insert" ), sharedSection->progress );
		}
		else
		{
			label << _( "Commit" ) << THREE_DOTS;
		}
	}
	return label;
}

