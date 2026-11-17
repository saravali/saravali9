/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/AtlasDao.cpp
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


#include "AtlasDao.h"

#include "constants.h"
#include "logger.h"

#include "wx/tokenzr.h"
#include "wx/wxsqlite3.h"

#define MAX_ERROR_STACK_SIZE 10

// Field usercode in table geonames: data from system (geonames.sql) / edited by user / new entry
enum { GEONAMES_USERCODE_SYSTEM = 0, GEONAMES_USERCODE_EDITED, GEONAMES_USERCODE_NEW };

// View names for atlas select
static const std::vector<wxString> atlas_viewname = { "atlas", "atlas_ascii", "atlas_alias" };

/*****************************************************
**
**   AtlasDao   ---   getCountryName
**
******************************************************/
wxString AtlasDao::getCountryName( wxString iso )
{
	wxString s;
	queryString = "SELECT name FROM countries where iso = ?";
	try
	{
		wxSQLite3::Database *db = getDbHandle();
		if ( db )
		{
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			stmt.Bind( 1, iso );
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			if (set.NextRow()) s = set.GetAsString( 0 );
			stmt.Finalize();
		}
	}
	catch( wxSQLite3::Exception &e )
	{
		handleException( e );
	}
	return s;
}

/*****************************************************
**
**   AtlasDao   ---   getAllAdminNamesForCountry
**
******************************************************/
std::list<wxString> AtlasDao::getAllAdminNamesForCountry( const wxString &country_name )
{
	std::list<wxString> l;
	try
	{
		wxSQLite3::Database *db = getDbHandle();
		if ( db )
		{
			queryString = "SELECT iso FROM countries where name = ?";
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			stmt.Bind( 1, country_name );
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			if (set.NextRow())
			{
				wxString country_code = set.GetAsString( 0 );
				queryString = "SELECT name FROM admincodes where country = ?";
				wxSQLite3::Statement stmt2 = db->PrepareStatement( queryString );
				stmt2.Bind( 1, country_code );
				wxSQLite3::ResultSet set2 = stmt2.ExecuteQuery();
				while (set2.NextRow())
				{
					l.push_back( set2.GetAsString( 0 ));
				}
				stmt2.Finalize();
			}
			stmt.Finalize();
		}
	}
	catch( wxSQLite3::Exception &e )
	{
		handleException( e );
	}
	return l;
}

/*****************************************************
**
**   AtlasDao   ---   getFeatureName
**
******************************************************/
wxString AtlasDao::getFeatureName( wxString feature_class, wxString feature_code )
{
	wxString s = feature_code;
	wxSQLite3::Database *db = getDbHandle();
	if ( db )
	{
		try
		{
			queryString = "SELECT description1 FROM featurecodes where feature_class =? and feature_code=?";
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			stmt.Bind( 1, feature_code );
			stmt.Bind( 2, feature_class );
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			if (set.NextRow()) s = set.GetAsString( 0 );
			else
				warnlog << "no feature name for class " << feature_class << " and code " << feature_code;
			stmt.Finalize();
		}
		catch( wxSQLite3::Exception &e )
		{
			handleException( e );
		}
	}
	return s;
}

/*****************************************************
**
**   AtlasDao   ---   getAdminName
**
******************************************************/
wxString AtlasDao::getAdminName( wxString country_code, wxString admin1_code )
{
	wxSQLite3::Database *db = getDbHandle();
	if ( db )
	{
		try
		{
			queryString = "SELECT name FROM admincodes where country =? and admin1code=?";
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			stmt.Bind( 1, country_code );
			stmt.Bind( 2, admin1_code );
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			if (set.NextRow()) return set.GetAsString( 0 );
			stmt.Finalize();
		}
		catch( wxSQLite3::Exception &e )
		{
			handleException( e );
		}
	}
	return wxEmptyString;
}

/*****************************************************
**
**   AtlasDao   ---   getCountryCodeForName
**
******************************************************/
wxString AtlasDao::getCountryCodeForName( wxString name )
{
	wxSQLite3::Database *db = getDbHandle();
	if ( db )
	{
		try
		{
			queryString = "SELECT iso FROM countries where name =?";
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			stmt.Bind( 1, name );
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			if (set.NextRow()) return set.GetAsString( 0 );
			stmt.Finalize();
		}
		catch( wxSQLite3::Exception &e )
		{
			handleException( e );
		}
	}
	return wxEmptyString;
}

/*****************************************************
**
**   AtlasDao   ---   getAdminCodeForCountryAndName
**
******************************************************/
wxString AtlasDao::getAdminCodeForCountryAndName( wxString country_code, wxString name )
{
	wxSQLite3::Database *db = getDbHandle();
	if ( db )
	{
		try
		{
			queryString = "SELECT admin1code FROM admincodes where country =? and name=?";
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			stmt.Bind( 1, country_code );
			stmt.Bind( 2, name );
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			if (set.NextRow()) return set.GetAsString( 0 );
			stmt.Finalize();
		}
		catch( wxSQLite3::Exception &e )
		{
			handleException( e );
		}
	}
	return wxEmptyString;
}

/*****************************************************
**
**   AtlasDao   ---   getAllTimezones
**
******************************************************/
std::list<TimezoneEntry> AtlasDao::getAllTimezones()
{
	std::list<TimezoneEntry> list;
	try
	{
		wxSQLite3::Database *db = getDbHandle();
		if ( db )
		{
			queryString = "SELECT name,country_code,offset FROM timezones";
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			while (set.NextRow())
			{
				list.push_back( TimezoneEntry( set.GetAsString( 0 ), set.GetAsString( 1 ), set.GetDouble( 2 ) ));
			}
			set.Finalize();
			stmt.Finalize();
		}
	}
	catch( wxSQLite3::Exception &e )
	{
		handleException( e );
	}
	return list;
}

/*****************************************************
**
**   AtlasDao   ---   getAllCountries
**
******************************************************/
std::list<AtlasCountry> AtlasDao::getAllCountries()
{
	std::list<AtlasCountry> list;
	try
	{
		wxSQLite3::Database *db = getDbHandle();
		if ( db )
		{
			queryString = "SELECT iso,name FROM countries";
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			while (set.NextRow())
			{
				list.push_back( AtlasCountry( set.GetAsString( 0 ), set.GetAsString( 1 ) ));
			}
			set.Finalize();
			stmt.Finalize();
		}
	}
	catch( wxSQLite3::Exception &e )
	{
		handleException( e );
	}
	list.sort( []( const AtlasCountry &c1, const AtlasCountry &c2 ) -> bool { return ( c1.name < c2.name ); } );
	return list;
}

/*****************************************************
**
**   AtlasDao   ---   getEntries
**
******************************************************/
std::vector<AtlasEntry> AtlasDao::getEntries( wxString filter, wxString country_code, const int &mode, const int &limit, const int &offset )
{
	std::vector<AtlasEntry> list;
	assert( mode >= 0 && mode < 3 );
	bool f = ! filter.IsEmpty();
	bool c = ! country_code.IsEmpty();

	queryString.Clear();
	queryString << "SELECT id,name,country_code,admin1_code,latitude,longitude FROM " << atlas_viewname[mode];

	if ( f && !c ) queryString << " WHERE name like ?";
	else if ( ! f && c ) queryString << " WHERE country_code=?";
	else if ( f && c ) queryString <<  " WHERE name like ? AND country_code=?";

	queryString << " ORDER BY NAME LIMIT ? OFFSET ?";

	wxSQLite3::Database *db = getDbHandle();
	if ( db )
	{
		try
		{
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			int index = 1;
			if ( f && !c )
			{
				stmt.Bind( index++, filter + "%" );
			}
			else if ( ! f && c )
			{
				stmt.Bind( index++, country_code );
			}
			else if ( f && c )
			{
				stmt.Bind( index++, filter + "%" );
				stmt.Bind( index++, country_code );
			}
			stmt.Bind( index++, limit );
			stmt.Bind( index++, offset );

			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			int count = 0;
			while (set.NextRow() && count < ATLAS_MAX_GRID_ELEMENTS )
			{
				AtlasEntry entry;
				entry.id = set.GetInt( 0 );
				entry.name = set.GetAsString( 1 );
				entry.country_code = set.GetAsString( 2 );
				entry.admin1_code = set.GetAsString( 3 );
				entry.latitude = set.GetDouble( 4 );
				entry.longitude = set.GetDouble( 5 );
				entry.rowid = offset + count++;
				list.push_back( entry );
			}
			stmt.Finalize();
		}
		catch( wxSQLite3::Exception &e )
		{
			handleException( e );
		}
	}
	return list;
}

/*****************************************************
**
**   AtlasDao   ---   getMatchCount
**
******************************************************/
int AtlasDao::getMatchCount( wxString filter, wxString country_code, const int &mode )
{
	bool f = ! filter.IsEmpty();
	bool c = ! country_code.IsEmpty();

	queryString.Clear();
	queryString << "SELECT COUNT(*) FROM " << atlas_viewname[mode];
	if ( f && !c )
	{
		queryString << " WHERE name like ?";
	}
	else if ( ! f && c )
	{
		queryString << " WHERE country_code=?";
	}
	else if ( f && c )
	{
		queryString <<  " WHERE name like ? AND country_code=?";
	}

	int count = 0;
	wxSQLite3::Database *db = getDbHandle();
	if ( db )
	{
		try
		{
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			if ( f && !c )
			{
				stmt.Bind( 1, filter + "%"  );
			}
			else if ( ! f && c )
			{
				stmt.Bind( 1, country_code );
			}
			else if ( f && c )
			{
				stmt.Bind( 1, filter + "%" );
				stmt.Bind( 2, country_code );
			}
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			if (set.NextRow() )
			{
				count = set.GetInt( 0 );
			}
			stmt.Finalize();
		}
		catch( wxSQLite3::Exception &e )
		{
			handleException( e );
		}
	}
	return count;
}

/*****************************************************
**
**   AtlasDao   ---   getFullEntry
**
******************************************************/
AtlasEntry AtlasDao::getFullEntry( int id )
{
	AtlasEntry entry;
	queryString.Clear();
	queryString << "SELECT g.featureid,g.name,g.asciiname,g.country_code,g.feature_class,g.feature_code,g.admin1_code,"
		<< "g.population,g.elevation,g.latitude,g.longitude,g.timezone,t.offset FROM geonames g, timezones t "
		<< "WHERE g.timezone=t.name AND g.featureid = ?";

	try
	{
		wxSQLite3::Database *db = getDbHandle();
		if ( db )
		{
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			stmt.Bind( 1, id );
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			if (set.NextRow())
			{
				entry.id = set.GetInt( 0 );
				entry.name = set.GetAsString( 1 );
				entry.asciiname = set.GetAsString( 2 );
				entry.country_code = set.GetAsString( 3 );
				entry.feature_code = set.GetAsString( 4 );
				entry.feature_class = set.GetAsString( 5 );
				entry.admin1_code = set.GetAsString( 6 );
				entry.population = set.GetInt( 7 );
				entry.elevation = set.GetInt( 8 );
				entry.latitude = set.GetDouble( 9 );
				entry.longitude = set.GetDouble( 10 );
				entry.timezone = set.GetAsString( 11 );
				entry.tzoffset = set.GetDouble( 12 );
			}
			set.Finalize();
			stmt.Finalize();

			queryString = "SELECT alias FROM geonames_aliases WHERE featureid= ?";
			wxSQLite3::Statement stmt2 = db->PrepareStatement( queryString );
			stmt2.Bind( 1, id );
			wxSQLite3::ResultSet set2 = stmt2.ExecuteQuery();
			int count = 0;
			while (set2.NextRow())
			{
				if ( count > 0 ) entry.aliases += ", ";
				entry.aliases += set2.GetAsString( 0 );
				count++;
			}
			set2.Finalize();
			stmt2.Finalize();
		}
	}
	catch( wxSQLite3::Exception &e )
	{
		handleException( e );
	}

	return entry;
}

/*****************************************************
**
**   AtlasDao   ---   saveEntry
**
******************************************************/
void AtlasDao::saveEntry( AtlasEntry &entry )
{
	bool exists = entryExists( entry.id );
	if ( exists ) saveExistingEntry( entry );
	else saveNewEntry( entry );
	saveAliasNames( entry.id, entry.country_code, entry.aliases );
}

/*****************************************************
**
**   AtlasDao   ---   saveExistingEntry
**
******************************************************/
void AtlasDao::saveExistingEntry( const AtlasEntry &entry )
{
	queryString.Clear();
	queryString << "UPDATE geonames SET name=?, asciiname=?, latitude=?, longitude=?, country_code=?, "
		<< "admin1_code=?, population=?, elevation=?, timezone=?, usercode=? WHERE featureid=?";

	wxSQLite3::Database *db = getDbHandle();
	if ( db )
	{
		try
		{
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			stmt.Bind( 1, entry.name );
			stmt.Bind( 2, entry.asciiname );
			stmt.Bind( 3, entry.latitude );
			stmt.Bind( 4, entry.longitude );
			stmt.Bind( 5, entry.country_code );
			stmt.Bind( 6, entry.admin1_code );
			stmt.Bind( 7, entry.population );
			stmt.Bind( 8, entry.elevation );
			stmt.Bind( 9, entry.timezone );
			stmt.Bind( 10, GEONAMES_USERCODE_EDITED );
			stmt.Bind( 11, entry.id );

			stmt.ExecuteUpdate();
			stmt.Finalize();
		}
		catch( wxSQLite3::Exception &e )
		{
			handleException( e );
		}
	}
}

/*****************************************************
**
**   AtlasDao   ---   saveNewEntry
**
******************************************************/
void AtlasDao::saveNewEntry(  AtlasEntry &entry )
{

	if( entry.id == 0 )
	{
		entry.id = getNewFeatureId();
	}
	try
	{
		wxSQLite3::Database *db = getDbHandle();
		if ( db )
		{
			queryString.Clear();
			queryString << "INSERT INTO geonames (featureid, name, asciiname, latitude, longitude, country_code, "
				<< "admin1_code, population, elevation, timezone, usercode ) "
				<< "VALUES( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )";

			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			stmt.Bind( 1, entry.id );
			stmt.Bind( 2, entry.name );
			stmt.Bind( 3, entry.asciiname );
			stmt.Bind( 4, entry.latitude );
			stmt.Bind( 5, entry.longitude );
			stmt.Bind( 6, entry.country_code );
			stmt.Bind( 7, entry.admin1_code );
			stmt.Bind( 8, entry.population );
			stmt.Bind( 9, entry.elevation );
			stmt.Bind( 10, entry.timezone );
			stmt.Bind( 11, GEONAMES_USERCODE_NEW );

			stmt.ExecuteUpdate();
			stmt.Finalize();
		}
	}
	catch( wxSQLite3::Exception &e )
	{
		handleException( e );
	}
}

/*****************************************************
**
**   AtlasDao   ---   saveAliasNames
**
******************************************************/
void AtlasDao::saveAliasNames( const int &id, wxString /* country_code */, wxString names )
{
	wxString s;
	uint i;
	bool b;

	wxSQLite3::Database *db = getDbHandle();
	if ( db )
	{
		try
		{
			// alias names in database
			std::vector<wxString> db_names;

			// alias names in database that occur in input string, other ones must be deleted from database
			std::vector<bool> found;

			queryString = "SELECT alias FROM geonames_aliases WHERE featureid=?";
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			stmt.Bind( 1, id );
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			while (set.NextRow())
			{
				db_names.push_back( set.GetString( 0 ));
				found.push_back( false );
			}
			stmt.Finalize();

			wxStringTokenizer tk( names, "," );
			while( tk.HasMoreTokens())
		  {
				s = tk.GetNextToken().Trim( true ). Trim( false );
				b = false;
				for( i = 0; i < db_names.size(); i++ )
				{
					if ( db_names[i] == s )
					{
						found[i] = true;
						b = true;
						break;
					}
				}
				if ( ! b )
				{
					// string s not found in DB, insert
					queryString = "INSERT INTO geonames_aliases VALUES( ?, ? )";
					wxSQLite3::Statement stmt2 = db->PrepareStatement( queryString );
					stmt2.Bind( 1, id );
					stmt2.Bind( 2, s );
					stmt2.ExecuteUpdate();
					stmt2.Finalize();
				}
			}

			// find DB items to delete
			for( i = 0; i < db_names.size(); i++ )
			{
				if ( found[i] == false )
				{
					queryString = "DELETE FROM geonames_aliases WHERE featureid=? and alias=?";
					wxSQLite3::Statement stmt3 = db->PrepareStatement( queryString );
					stmt3.Bind( 1, id );
					stmt3.Bind( 2, db_names[i] );
					stmt3.ExecuteUpdate();
					stmt3.Finalize();
				}
			}
		}
		catch( wxSQLite3::Exception &e )
		{
			handleException( e );
		}
	}
}

/*****************************************************
**
**   AtlasDao   ---   getNewFeatureId
**
******************************************************/
int AtlasDao::getNewFeatureId()
{
	int id = 0;
	wxSQLite3::Database *db = getDbHandle();
	if ( db )
	{
		try
		{
			queryString = "SELECT MAX( featureid ) FROM geonames";
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			if (set.NextRow()) id = set.GetInt( 0 );
			stmt.Finalize();
			id++;
		}
		catch( wxSQLite3::Exception &e )
		{
			handleException( e );
		}
	}
	return id;
}

/*****************************************************
**
**   AtlasDao   ---   entryExists
**
******************************************************/
bool AtlasDao::entryExists( const int &id )
{
	bool b = false;
	wxSQLite3::Database *db = getDbHandle();
	if ( db )
	{
		try
		{
			queryString = "SELECT EXISTS( SELECT 1 FROM geonames WHERE featureid = ?)";
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			stmt.Bind( 1, id );
			wxSQLite3::ResultSet set = stmt.ExecuteQuery();
			if (set.NextRow()) b = set.GetInt( 0 );
			stmt.Finalize();
		}
		catch( wxSQLite3::Exception &e )
		{
			handleException( e );
		}
	}
	return b;
}

/*****************************************************
**
**   AtlasDao   ---   deleteEntry
**
******************************************************/
void AtlasDao::deleteEntry( const int& id )
{
	try
	{
		wxSQLite3::Database *db = getDbHandle();
		if ( db )
		{
			queryString = "DELETE FROM geonames WHERE featureid =?";
			wxSQLite3::Statement stmt = db->PrepareStatement( queryString );
			stmt.Bind( 1, id );
			stmt.ExecuteUpdate();
			stmt.Finalize();

			queryString = "DELETE FROM geonames_aliases WHERE featureid = ?";
			wxSQLite3::Statement stmt2 = db->PrepareStatement( queryString );
			stmt2.Bind( 1, id );
			stmt2.ExecuteUpdate();
			stmt2.Finalize();
		}
	}
	catch( wxSQLite3::Exception &e )
	{
		handleException( e );
	}
}

