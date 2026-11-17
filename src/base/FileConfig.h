/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/FileConfig.h
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
#ifndef _FILECONFIG_H_
#define _FILECONFIG_H_

#include "constants.h"

#include <list>
#include <wx/string.h>

/*************************************************//**
*
*  Resource data is loaded from files. Several types (yoga, table) support storage by user.
*
******************************************************/
struct ConfigResourceDefinition
{
	const wxString tokenName; // name of directory

	const ConfigResourceStoreType storeType;

	const wxString fileExtension; // like json, sql, db
};

/*************************************************//**
*
*  Handling of various file and directory tasks.
*
******************************************************/
class FileConfig : public Singleton<FileConfig>
{
	friend class Singleton<FileConfig>;

public:

	/**
	*  This method does on the minmum required for creating/reading the astroconfig and gdiconfig files.
	*  This is required for appropriate log target configuration.
	*
	*  The method creates the config dir under user's Home if necessary.
	*  Furthermore filenames under home dir are calculated.
	*/
	void initHomedir();

	/**
	*  Second phase of file config. Main steps are
	*  - Resolve symboliclink to excutable if necessary
	*  - Find out fully qualified name of excutable and its directory
	*  - Determine resource dir. This may be either local installation (development mode) or global (after installation).
	*  - Copy resource of local type local config dir.
	*/
	void initDatadir( const wxString &argv0 );

	wxString getExecutableName() const { return exename; }
	wxString getExecutableDir() const { return exedir; }

	wxString getConfigDir() const { return configdir; }

	wxString getAstroConfigFilename() const { return astroconfigfile; }
	wxString getGdiConfigFilename() const { return gdiconfigfile; }

	wxString getResourceDir() const { return resourcedir; }

	wxString getDefaultEphemDir() const;

	wxString getLicenseFilename() const { return licensefile; }

	bool backupFile( const wxString &filename );

	ConfigResourceDefinition getConfigResourceDefinition( const ConfigResource& ) const;

	wxString getResourceDir( const ConfigResource &cr ) const;

	bool copyResourceFile2ConfigDir( wxString basename, const bool force = false );

	wxString getDefaultAtlasSqlFile() const; 

	wxString getDefaultAtlasDbFile() const;

	// find file on path
	wxString locateFile( const wxString &file );

	// try to find several common PDF viewers on the path (*NIX)
	wxString getPdfViewerDefaultFilename();

	/**
	* Gives a list of files matching specific file extensions.
	*
	* Input is given like "json", the method will match also uppercase of mixed case extensions.
	*
	* Input param "extensions" must be lowercase. 
	*
	* The last param "getFullPath" determines if only the filename+ext is given or the full path.
	*/
	std::list<wxString> getMatchingFiles( const wxString &directory,
    const std::list<wxString> extensions = std::list<wxString>(),
    const bool getFullPath = true );

	void writeFileAndDirectoryReport( MDocument_ptr );

private:

	FileConfig();

	void appendSeparator( wxString &filename );

	wxString getGlobalResourceDir( const ConfigResource& ) const;

	wxString getLocalResourceDir( const ConfigResource& ) const;

	/**
	* Only used for resources of type local (Yoga)
	*
	* 1. make local config directory if necessary
	* 2. copy files from global resoruce directory to local dir
	*/
	void setupLocalResourcedir( const ConfigResource& );

	// fully qualified name of excutable
	wxString exename;

	// dir of excutable file
	wxString exedir;

	// local config dir
	wxString configdir;

	// global resource dir
	wxString resourcedir;

	// filenames
	wxString licensefile, astroconfigfile, gdiconfigfile;
};

#endif

