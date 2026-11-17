/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/BaseDao.h
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

#ifndef BASEDAO_H
#define BASEDAO_H

#include <list>
#include <vector>
#include <wx/string.h>

//class wxSQLite3::Database;
namespace wxSQLite3
{
	class Database;
	class Exception;
};

/*************************************************//**
*
*
*
******************************************************/
struct DbErrorEntry
{
	int errorCode;

	wxString message;
};

/*************************************************//**
*
*
*
******************************************************/
class BaseDao
{
public:

	BaseDao( const wxString = wxEmptyString );

	~BaseDao();

	int executeQueryBundle( std::list<wxString>&, const bool &dbMustExist );

	wxString getDbFileName() const;

	void closeDbHandle();

	wxString getLastQuery() const { return queryString; }

	bool lastDbStatusWasOkay() const;

	bool isDbStatusOkay();

	void interruptAllQueries();

	wxString getLastErrorMessage() const;

	std::vector<DbErrorEntry> getErrorStack() const { return errorStack; }

	wxString getSqliteVersion() const;

	wxString getWxSqliteVersion() const;

protected:

	wxSQLite3::Database *getDbHandle( const bool dbFileMustExist = true );

	void handleException( wxSQLite3::Exception& );

	wxString queryString;

	std::vector<DbErrorEntry> errorStack;

private:

	const wxString dbfile;

	class wxSQLite3::Database *dbhandle;
};


#endif

