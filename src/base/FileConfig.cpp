/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/FileConfig.cpp
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

#include "FileConfig.h"

#include "enumserializer.h"
#include "gdiconfig.h"
#include "Lang.h"
#include "logger.h"
#include "constants.h"
#include "mathbase.h"
#include "MDocument.h"
#include "Table.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>

extern Gdiconfig *gdiconfig;

#define ATLAS_DEFAULT_DBFILENAME  "atlas.db"
#define ATLAS_DEFAULT_SQLFILENAME "geonames1000.sql"
#define LICENSE_FILE_NAME "license.txt"
#define LOCAL_INSTALL_TESTFILE "saravali_local_installation.txt"
#define RESOURCE_DIR_NAME "resources"
#define DEBUG_FILECONFIG

#define DEBUG_BOOTSTRAP
#ifdef DEBUG_BOOTSTRAP
#include <iostream>
#endif


/*****************************************************
**
**   FileConfig   ---   Constructor
**
******************************************************/
FileConfig::FileConfig()
{
#ifdef DEBUG_FILECONFIG
	debuglog << "Constructor FileConfig FILE " << __FILE__;
#endif	
}

#define CONFIG_DIR_NAME ".saravali-9.0"
/*****************************************************
**
**   FileConfig   ---   initHomedir
**
******************************************************/
void FileConfig::initHomedir()
{
	wxStandardPaths::Get().SetFileLayout( wxStandardPathsBase::FileLayout_XDG );
	configdir = wxStandardPaths::Get().GetUserConfigDir();
	if ( ! configdir.EndsWith( wxFileName::GetPathSeparator()))
	{
		configdir << wxFileName::GetPathSeparator();
	}
	configdir << APPLICATION_NAME << wxFileName::GetPathSeparator();

	if ( ! wxFileName::DirExists( configdir ))
	{
		infolog << "Create config directory " << configdir;
		if ( ! wxFileName::Mkdir( configdir ))
		{
			errorlog << "Cannot create config dir " << configdir;
		}
	}

	// 2x config file names 
	astroconfigfile = configdir + "astroconfig.json";
	gdiconfigfile = configdir + "gdiconfig.json";

	std::cout << "Config Dir " << wxStandardPaths::Get().GetConfigDir() << std::endl;
	std::cout << "User Dir " << wxStandardPaths::Get().GetUserConfigDir() << std::endl;
	std::cout << "exec Dir " << wxStandardPaths::Get().GetExecutablePath() << std::endl;

#ifdef DEBUG_BOOTSTRAP
	std::cout << std::endl;
	std::cout << "File config in Home dir:" << std::endl;
	std::cout << "------------------------" << std::endl;
	std::cout << "Config dir " << configdir << std::endl;
	std::cout << "Astro config file " << astroconfigfile << std::endl;
	std::cout << "Gdi config file " << gdiconfigfile << std::endl;
	std::cout << std::endl;
#endif
}

/*****************************************************
**
**   FileConfig   ---   initDatadir
**
******************************************************/
void FileConfig::initDatadir( const wxString &argv0 )
{
	//wxString rawname = argv0;
	wxString rawname = wxStandardPaths::Get().GetExecutablePath();

#ifndef __WXMSW__

#if	wxCHECK_VERSION( 3, 1, 5 ) // the ResolveLink method appears in wxWidgets 3.1.5

	// Step 0: resolve link if executable is a symbolic link ( not on windows)
	if ( wxFileName::Exists( rawname, wxFILE_EXISTS_SYMLINK ))
	{
#ifdef DEBUG_FILECONFIG
		debuglog << "FileConfig::init found symbolic link " << rawname;
#endif	
		bool isLink = true;
		int count = 0;
		constexpr int maxcount = 10; // avoid infinite loops

		while( isLink && count++ < maxcount )
		{
			//wxFileName fn = wxFileName( rawname );
			wxFileName rfn = wxFileName( rawname ).ResolveLink();
			rawname = rfn.GetFullPath();
			isLink = wxFileName::Exists( rawname, wxFILE_EXISTS_SYMLINK );
#ifdef DEBUG_FILECONFIG
			debuglog << "FileConfig::init symbolic link count " << count << " resolved to full path " << rawname
				<< " isLink " << std::boolalpha << isLink;
#endif	
		}
	}
#else
	warnlog << "Found old wxWidgets version (3.2 recommended). Symbolic links to executable file will not be resolved.";
#endif
#endif // ifndef __WXMSW__

	// Step 1: determine name and path of executable file passed as argv0 (and link resolution)
	{
		wxFileName fn( rawname );
		if ( ! fn.MakeAbsolute())
		{
			errorlog << "Cannot determine exe name " << argv0;
		}
		exename = fn.GetFullPath();
		exedir = fn.GetPath();
		appendSeparator( exedir );
#ifdef DEBUG_FILECONFIG
		debuglog << "exename " << exename << " exe dir " << exedir;
#endif	
	}

	// Step 2: determine resource directory
	{
		bool done = false;

		// 2a) test if we are within a macOS package
#ifdef __WXOSX__
#define MAC_RESOURCE_DIRECTORY "Saravali9.app/Contents/Resources"
	if ( wxFileName::DirExists( MAC_RESOURCE_DIRECTORY ))
	{
		resourcedir = MAC_RESOURCE_DIRECTORY;
		done = true;
	}
#endif

		// 2b) this a local installation
		if ( ! done )
		{
			const wxString testfile = exedir + "../" + RESOURCE_DIR_NAME + "/" + LOCAL_INSTALL_TESTFILE;
#ifdef DEBUG_FILECONFIG
			debuglog << "testfile in case 2a " << testfile;
#endif	
			if ( wxFileName::FileExists( testfile ))
			{
				resourcedir = exedir + "../resources";
#ifdef DEBUG_FILECONFIG
				debuglog << "Testfile 2a FOUND, resourcedir is " << resourcedir;
#endif	
				done = true;
			}
#ifdef DEBUG_FILECONFIG
			else
			{
				debuglog << "testfile " << testfile << " not found, try next";
			}
#endif	
		}
		
		// 2c) local installation on Visual c++
#ifdef __WXMSW__
		if (!done)
		{
			const wxString testfile = exedir + "../../src/" + RESOURCE_DIR_NAME + "/" + LOCAL_INSTALL_TESTFILE;
			if ( wxFileName::FileExists( testfile ))
			{
				resourcedir = exedir + "../../src/resources";
				done = true;
			}
		}
#endif

		// 2d) use DATAROOTDIR which is passed by the compiler
		if ( ! done )
		{
#if defined( DATAROOTDIR )
#ifdef DEBUG_FILECONFIG
			debuglog << "FileConfig::initDatadir DATAROOTDIR " << DATAROOTDIR;
#endif	
			wxString dir = DATAROOTDIR;
			if ( wxFileName::DirExists( dir ))
			{
				appendSeparator( dir );
				dir += APPLICATION_NAME;
				if ( wxFileName::DirExists( dir ))
				{
					resourcedir = dir;
#ifdef DEBUG_FILECONFIG
					debuglog << "resourcedir in case 2c " << resourcedir;
#endif	
					done = true;
				}
				else
				{
					errorlog << "Resource dir candidate " << dir << " does not exist.";
				}
			}
			else
			{
				errorlog << "Data root directory " << dir << " passed by complier does not exist.";
			}
#endif
		}

		// 2e) finally use current working directory
		if ( ! done )
		{
			//resourcedir = ".";
			resourcedir = exedir + "resources";
			appendSeparator( resourcedir );
		}
#ifdef DEBUG_FILECONFIG
		debuglog << "resourcedir after 2d " << resourcedir;
#endif	

		// make directory name absolute
		wxFileName fn( resourcedir );
		if ( ! fn.MakeAbsolute())
		{
			errorlog << "Cannot make resourcedir name absolute " << resourcedir;
		}
		resourcedir = fn.GetFullPath();

		// finally append seprator if necessary
		appendSeparator( resourcedir );
#ifdef DEBUG_FILECONFIG
		debuglog << "Finished determination of resource directory, result " << resourcedir;
#endif	
	}

	licensefile = resourcedir + LICENSE_FILE_NAME;

	// Step 4: Create local resource dirs and copy files if necessary
	#if 1
	for( const auto &cr : Enum<ConfigResource>() )
	{
		const ConfigResourceDefinition def = getConfigResourceDefinition( cr );
		const wxString localConfigDir = getConfigDir() + def.tokenName;

		if ( ! wxFileName::DirExists( localConfigDir ))
		{
			infolog << "FileConfig::setupLocalResourcedir: create local resource directory " << localConfigDir;
			if ( ! wxFileName::Mkdir( localConfigDir )) errorlog << "FileConfig::setupLocalResourcedir: Cannot create resource directory " << localConfigDir;
		}
	}

	#else
	for( const auto &cr : Enum<ConfigResource>() )
	{
		const ConfigResourceDefinition def = getConfigResourceDefinition( cr );
		if ( def.storeType == ConfigResourceStoreType::LocalMultipleFiles
			|| def.storeType == ConfigResourceStoreType::LocalSingleFile )
		{
			setupLocalResourcedir( cr );
		}
	}
	#endif

#ifdef DEBUG_FILECONFIG
	debuglog;
	debuglog << "Name of executable " << exename;
	debuglog << "Directory of executable " << exedir;
	debuglog;
	debuglog << "File config in data dir:";
	debuglog << "------------------------";
	debuglog << "Resource dir " << resourcedir;
	for( const auto &cr : Enum<ConfigResource>() )
	{
		debuglog << "Resource dir for " << cr << " is " << getResourceDir( cr );
	}
	debuglog << "License file " << licensefile;
	debuglog;
#endif
}

/*****************************************************
**
**   FileConfig   ---   setupLocalResourcedir
**
******************************************************/
void FileConfig::setupLocalResourcedir( const ConfigResource &cr )
{
	const wxString localdir = getLocalResourceDir( cr );
	if ( ! wxFileName::DirExists( localdir ))
	{
		infolog << "FileConfig::setupLocalResourcedir: create resource directory " << localdir;
		if ( ! wxFileName::Mkdir( localdir )) errorlog << "FileConfig::setupLocalResourcedir: Cannot create resource directory " << localdir;
	}

	const ConfigResourceDefinition def = getConfigResourceDefinition( cr );
	const wxString globaldir = getGlobalResourceDir( cr );

	for( const auto &globalfilename : getMatchingFiles( globaldir, { def.fileExtension }, /*getFullPath*/ true ))
	{
		wxString name = wxFileName( globalfilename ).GetFullName();
		wxString localfilename = localdir + name;
		const bool found = wxFileName::Exists( localfilename );

		#ifdef DEBUG_FILECONFIG
		debuglog << "FileConfig::setupLocalResourcedir: global file " << globalfilename << " name only " << name
			<< " local file " << localfilename << " exists " << std::boolalpha << found;
		#endif	

		if ( ! found )
		{
			wxString targetfile = localdir + name;
			debuglog << "FileConfig::setupLocalResourcedir: copy resource file " << name << " to target " << targetfile;
			if ( ! wxCopyFile( globalfilename, targetfile ))
			{
				errorlog << "cannot copy file  " << globalfilename << " to target " << targetfile;
			}
		}
	}
}

/*****************************************************
**
**   FileConfig   ---   getConfigResourceDefinition
**
******************************************************/
ConfigResourceDefinition FileConfig::getConfigResourceDefinition( const ConfigResource &type ) const
{
	wxString tokenName;
	ConfigResourceStoreType storeType = ConfigResourceStoreType::Global;
	wxString extension = "json";

	switch( type )
	{
		case ConfigResource::AtlasDB:
			tokenName = "atlas";
			storeType = ConfigResourceStoreType::LocalSingleFile;
			extension = "db";
		break;
		case ConfigResource::AtlasSQL:
			tokenName = "atlas";
			extension = "sql";
		break;
		case ConfigResource::Dasa:
			tokenName = "dasa";
		break;
		case ConfigResource::Geom:
			tokenName = "chart";
		break;
		case ConfigResource::Pic:
			tokenName = "pic";
			extension = "png";
		break;
		case ConfigResource::Sheet:
			storeType = ConfigResourceStoreType::LocalMultipleFiles;
			tokenName = "print";
		break;
		case ConfigResource::Skin:
			tokenName = "skin";
		break;
		case ConfigResource::Table:
			storeType = ConfigResourceStoreType::LocalMultipleFiles;
			tokenName = "table";
		break;
		case ConfigResource::Uranian:
			tokenName = "uranian";
		break;
		case ConfigResource::Yoga:
			tokenName = "yoga";
			storeType = ConfigResourceStoreType::LocalMultipleFiles;
		break;
		case ConfigResource::I18n:
			tokenName = "po";
		break;
		default:
			assert( false );
		break;
	}
	return ConfigResourceDefinition( tokenName, storeType, extension );
}

/*****************************************************
**
**   FileConfig   ---   getResourceDir
**
******************************************************/
wxString FileConfig::getResourceDir( const ConfigResource &cr ) const
{
	const ConfigResourceDefinition def = getConfigResourceDefinition( cr );

	wxString m;
	m <<  ( def.storeType == ConfigResourceStoreType::Global ? getResourceDir() : getConfigDir())
		<< def.tokenName << wxFileName::GetPathSeparator();

#ifdef DEBUG_FILECONFIG
	debuglog << "FileConfig::getResourceDir resource " << cr << " result " << m;
#endif	
	return m;
}

/*****************************************************
**
**   FileConfig   ---   getGlobalResourceDir
**
******************************************************/
wxString FileConfig::getGlobalResourceDir( const ConfigResource &cr ) const
{
	return getResourceDir() + getConfigResourceDefinition( cr ).tokenName + wxFileName::GetPathSeparator();
}

/*****************************************************
**
**   FileConfig   ---   getLocalResourceDir
**
******************************************************/
wxString FileConfig::getLocalResourceDir( const ConfigResource &cr ) const
{
	return getConfigDir() + getConfigResourceDefinition( cr ).tokenName + wxFileName::GetPathSeparator();
}

/*****************************************************
**
**   FileConfig   ---   getMatchingFiles
**
******************************************************/
std::list<wxString> FileConfig::getMatchingFiles( const wxString &directory,
    const std::list<wxString> extensions, const bool getFullPath )
{
	std::list<wxString> l;
	wxArrayString filelist;

	wxDir::GetAllFiles( directory, &filelist, "", wxDIR_FILES );

  for( size_t i = 0; i < filelist.GetCount(); i++ )
  {
#ifdef DEBUG_FILECONFIG
		debuglog << "FileConfig::getMatchingFiles item " << filelist[i];
#endif	
    wxFileName fn( filelist[i] );
    const wxString ext = fn.GetExt().Lower();
		
    if ( std::find( extensions.begin(), extensions.end(), ext ) != extensions.end())
    {
			/*
			if ( getFullPath )
			{
				fn.MakeAbsolute();
			}
			*/
			fn.MakeAbsolute();

      wxString name;
			if ( getFullPath )
			{
				name = fn.GetFullPath();
			}
			else
			{
				name = fn.GetFullName();
			}

#ifdef DEBUG_FILECONFIG
      debuglog << "FileConfig::getMatchingFiles ADD item \'" << filelist[i] << "\' name \'" << name << "\' ext \'" << ext << "\'";
#endif	

      l.push_back( name );
    }
  }
	l.sort();
#ifdef DEBUG_FILECONFIG
	debuglog << "FileConfig::getMatchingFiles has " << l.size() << " items";
#endif	
	return l;
}

/*****************************************************
**
**   FileConfig   ---   getDefaultAtlasSqlFile
**
******************************************************/
wxString FileConfig::getDefaultAtlasSqlFile() const
{
	return getResourceDir( ConfigResource::AtlasSQL ) + ATLAS_DEFAULT_SQLFILENAME;
}

/*****************************************************
**
**   FileConfig   ---   getDefaultAtlasDbFile
**
******************************************************/
wxString FileConfig::getDefaultAtlasDbFile() const
{
	const wxString s =  getResourceDir( ConfigResource::AtlasDB ) + ATLAS_DEFAULT_DBFILENAME;
	debuglog << "FileConfig::getDefaultAtlasDbFile filename " <<  s;

	return s;
}

/*****************************************************
**
**   FileConfig   ---   getDefaultEphemDir
**
******************************************************/
wxString FileConfig::getDefaultEphemDir() const
{
// standard path for swe data files on Debian
#if ! defined __WXMSW__ && ! defined __WXOSX__
	if ( wxFileName::DirExists( DEBIAN_SWE_STANDARD_DATA_PATH )) return DEBIAN_SWE_STANDARD_DATA_PATH;
#endif
	return SWE_STANDARD_DATA_PATH;
}

/*****************************************************
**
**   FileConfig   ---   copyResourceFile2ConfigDir
**
******************************************************/
bool FileConfig::copyResourceFile2ConfigDir( wxString basename, const bool force )
{
	wxString targetfile = configdir + basename;
	if ( force || ! wxFileName::FileExists( targetfile ))
	{
		wxString sourcefile = resourcedir + basename;
		if ( ! wxFileName::FileExists( sourcefile ))
		{
			errorlog << "File " << sourcefile << " does not exist, using defaults" ;
			return false;
		}
		else
		{
			infolog << "copy "  << sourcefile << " to " << targetfile;
			if ( ! wxCopyFile( sourcefile, targetfile ))
			{
				errorlog << "could not copy "  << sourcefile << " to " << targetfile ;
				return false;
			}
		}
	}
	return true;
}

/*****************************************************
**
**   FileConfig   ---   backupFile
**
******************************************************/
bool FileConfig::backupFile( const wxString &filename )
{
	if ( ! wxFileName::FileExists( filename )) return true;
	if ( gdiconfig->view.backupFileMode == 0 ) return true;

	const wxString targetfilename = filename + ".bak";
	if ( wxFileName::FileExists( targetfilename ))
	{
		wxRemoveFile( targetfilename );
	}
	return wxCopyFile( filename, targetfilename );
}

/*****************************************************
**
**   FileConfig   ---   locateFile
**
******************************************************/
wxString FileConfig::locateFile( const wxString &f )
{
	debuglog << "FileConfig::locateFile filename " << f;
	wxString path;
	if ( wxGetEnv( "PATH", &path ))
	{
		debuglog << "FileConfig::locateFile path " << path;
		wxString filename;
		wxStringTokenizer tokenizer( path, ":" );
		while( tokenizer.HasMoreTokens() )
		{
			filename = tokenizer.GetNextToken() + wxFileName::GetPathSeparator() + f;
			debuglog << "FileConfig::locateFile filename " << filename;
			if ( wxFileName::FileExists( filename ))
			{
				debuglog << "Found PDF viewer at " << filename;
				//infolog << "Found PDF viewer at " << filename;
				return filename;
			}
		}
	}
	return "";
}

/*****************************************************
**
**   FileConfig   ---   getPdfViewerDefaultFilename
**
******************************************************/
wxString FileConfig::getPdfViewerDefaultFilename()
{
#ifdef __WXOSX__
	return MAC_PDF_START_COMMAND;
#endif

#ifndef __WXMSW__
	const std::list<wxString> pdfviewers = { "atril", "evince", "kpdf", "xpdf",
		"okular", "kghostview", "acroread", "epdfview", "xreader" };
	wxString s;

	for ( const auto &a : pdfviewers )
	{
		s = locateFile( a );
		if ( ! s.empty())
		{
			debuglog << "Found PDF viewer: " << s;
			return s;
		}
	}
#endif
	return "";
}

/*****************************************************
**
**   FileConfig   ---   appendSeparator
**
******************************************************/
void FileConfig::appendSeparator( wxString &filename )
{
	if ( filename.size() > 0 && filename[ filename.size() - 1 ] != wxFileName::GetPathSeparator() )
		filename += wxFileName::GetPathSeparator();
}

/*****************************************************
**
**   FileConfig   ---   writeFileAndDirectoryReport
**
******************************************************/
void FileConfig::writeFileAndDirectoryReport( MDocument_ptr doc )
{
	Table *dtable = new Table( tr( "Directories" ));
	dtable->addHeaderRow( 
	{
		tr( "Type" ),
		tr( "Location" ),
		tr( "Exists" ),
	});

	const std::list<std::pair<wxString, wxString>> dirlist =
	{
		{ "Current Working directory", wxGetCwd() },
		{ "Resources", getResourceDir() },
		{ "Local Configurations", getConfigDir() },
		{ "Directory of Executable", getExecutableDir() },
	};
	for( const auto &a : dirlist )
	{
		dtable->addRow( 
		{
			a.first,
			a.second,
			std::format( "{}", wxFileName::DirExists( a.second ))
		});
	}
	doc->addItem( dtable );

	Table *ftable = new Table( tr( "Common Files" ));
	ftable->addHeaderRow( 
	{
		tr( "Type" ),
		tr( "Filename" ),
		tr( "Exists" ),
	});

	const std::list<std::pair<wxString, wxString>> dirlist2 =
	{
		{ "Executable", getExecutableName() },
		{ "Astro Config File", getAstroConfigFilename() },
		{ "Gdi Config File", getGdiConfigFilename() },
		{ "License File", getLicenseFilename() },
		{ "Default Atlas DB", getDefaultAtlasDbFile() },
		{ "Current Atlas DB", gdiconfig->atlas.databaseFile },
		{ "Default Atlas SQL File", getDefaultAtlasSqlFile() },
		{ "Current Atlas SQL File", gdiconfig->atlas.sqlFile },
	};
	for( const auto &a : dirlist2 )
	{
		ftable->addRow(
		{
			a.first,
			a.second,
			std::format( "{}", wxFileName::FileExists( a.second ))
		});
	}
	doc->addItem( ftable );

	Table *rtable = new Table( tr( "Resource Directories" ));
	rtable->addHeaderRow( 
	{
		tr( "Name" ),
		tr( "Type" ),
		tr( "Extension" ),
		tr( "Directory" ),
		tr( "Exists" ),
		tr( "Files in Directory" ),
	});

	for( const auto &cr : Enum<ConfigResource>() )
	{
#ifndef __WXMSW__
		if ( cr == ConfigResource::I18n ) continue;
#endif

		TableRow row;
		const ConfigResourceDefinition def = getConfigResourceDefinition( cr );

		row.addValue( enum_to_string( cr ));
		row.addValue( enum_to_string( def.storeType ));
		row.addValue( def.fileExtension );

		wxString resdir = getResourceDir( cr );
		row.addValue( resdir );
		row.addValue( std::format( "{}", wxFileName::DirExists( resdir )));

		// number of files
		row.addValue( std::format( "{}", getMatchingFiles( resdir, { def.fileExtension }).size()));

		rtable->addRow( row );
	}
	doc->addItem( rtable );
}


