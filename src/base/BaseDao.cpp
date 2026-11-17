/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/BaseDao.cpp
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

#include "BaseDao.h"

#include "FileConfig.h"
#include "gdiconfig.h"
#include "logger.h"

#include "wx/filename.h"
#include "wx/wxsqlite3.h"

#ifndef _cplusplus
extern "C" {
#endif
#include "sqlite3mc_amalgamation.h"
#ifndef _cplusplus
}
#endif

#define DEBUG_BASEDAO

extern Gdiconfig *gdiconfig;

#define MAX_ERROR_STACK_SIZE 10

/*****************************************************
**
**   BaseDao   ---   Constructor
**
******************************************************/
BaseDao::BaseDao( const wxString dbfile )
		: dbfile( dbfile )
{
	dbhandle = 0;
}

/*****************************************************
**
**   BaseDao   ---   Destructor
**
******************************************************/
BaseDao::~BaseDao()
{
	closeDbHandle();
}

/*****************************************************
**
**   BaseDao   ---   executeQueryBundle
**
******************************************************/
int BaseDao::executeQueryBundle( std::list<wxString> &qb, const bool &dbMustExist )
{
#ifdef DEBUG_BASEDAO
	debuglog << "BaseDao::executeQueryBundle execute " << qb.size() << " queries";
	//static int totalcount = 0;
#endif
	int count = 0;
	if ( qb.size() == 0 ) return 0;
	wxString queryString;
	try
	{
		wxSQLite3::Database *db = getDbHandle( dbMustExist );
		if ( db )
		{
			for ( const auto &a : qb )
			{
				queryString = a;
#ifdef DEBUG_BASEDAO
				//debuglog << "BaseDao::executeQueryBundle execute query " << count << " sql: " << queryString;
#endif
				db->ExecuteUpdate( queryString );
				count++;
			}
		}
	}
	catch( wxSQLite3::Exception &e )
	{
		handleException( e );
	}
	return count;
}


/*****************************************************
**
**   BaseDao   ---   getDbFileName
**
******************************************************/
wxString BaseDao::getDbFileName() const
{
	if ( gdiconfig->atlas.databaseFile.empty() )
	{
		gdiconfig->atlas.databaseFile = FileConfig::get()->getDefaultAtlasDbFile();
	}
	return gdiconfig->atlas.databaseFile;
}

/*****************************************************
**
**   BaseDao   ---   isDbStatusOkay
**
******************************************************/
bool BaseDao::isDbStatusOkay()
{
	errorStack.clear();
	getDbHandle();
	return( errorStack.size() == 0 );
}

/*****************************************************
**
**   BaseDao   ---   getDbHandle
**
******************************************************/
wxSQLite3::Database *BaseDao::getDbHandle( const bool dbFileMustExist ) 
{
	if ( ! dbhandle )
	{
		wxString dbName;

		// dbfile
		if ( ! dbfile.IsEmpty())
		{
			dbName = dbfile;
		}
		else
		{
			dbName = getDbFileName();
		}
		try
		{
			int flags = wxSQLite3::OPEN_READWRITE;
			if ( ! dbFileMustExist ) flags |= wxSQLite3::OPEN_CREATE;

			dbhandle = new wxSQLite3::Database();
			dbhandle->Open( dbName, wxEmptyString, flags );
		}
		catch( wxSQLite3::Exception &e )
		{
			handleException( e );
		}
	}
	return dbhandle;
}

/*****************************************************
**
**   BaseDao   ---   closeDbHandle
**
******************************************************/
void BaseDao::closeDbHandle()
{
	if ( dbhandle )
	{
		try
		{
			dbhandle->Close();
			delete dbhandle;
		}
		catch( wxSQLite3::Exception& )
		{
			warnlog << "Close dbhandle failed";
		}
	}
	dbhandle = 0;
}

/*****************************************************
**
**   BaseDao   ---   interruptAllQueries
**
******************************************************/
void BaseDao::interruptAllQueries()
{
	wxSQLite3::Database *db = getDbHandle();
	if ( db )
	{
		try
		{
			db->Interrupt();
		}
		catch( wxSQLite3::Exception &e )
		{
			handleException( e );
		}
	}
}

/*****************************************************
**
**   BaseDao   ---   getVersion
**
******************************************************/
wxString BaseDao::getSqliteVersion() const
{
	return SQLITE3MC_VERSION_STRING;
}
wxString BaseDao::getWxSqliteVersion() const
{
	return wxSQLITE3_VERSION_STRING;
}

/*****************************************************
**
**   BaseDao   ---   lastDbStatusWasOkay
**
******************************************************/
bool BaseDao::lastDbStatusWasOkay() const
{
	return errorStack.size() == 0;
}

/*****************************************************
**
**   BaseDao   ---   getLastErrorMessage
**
******************************************************/
wxString BaseDao::getLastErrorMessage() const
{
	if ( errorStack.size() == 0 ) return _( "No error" );
	wxString s;
	s << "Error code " << errorStack[0].errorCode << ", message: " << errorStack[0].message;
	return s;
}

/*****************************************************
**
**   BaseDao   ---   handleException
**
******************************************************/
void BaseDao::handleException( wxSQLite3::Exception &e )
{
	errorStack.push_back( DbErrorEntry( e.GetErrorCode(), e.GetMessage() ));
	closeDbHandle();

	errorlog << "Query " << queryString << ", Exception code " << e.GetErrorCode() 
		<< ", message: " << e.GetMessage();
}


