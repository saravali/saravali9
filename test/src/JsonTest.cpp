/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       test/src/JsonTest.cpp
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

#include "astroconfig.h"
#include "ChartConfig.h"
#include "ChartGeometry.h"
#include "FileConfig.h"
#include "gdibase.h"
#include "gdiconfig.h"
#include "logger.h"
#include "PrintoutConfig.h"
#include "Sheet.h"
#include "TestRunner.h"

#include <wx/dir.h>
#include <wx/filename.h>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;


TEST( Json, SimpleTest )
{
	FileConfig *fc = FileConfig::get();
	wxArrayString filelist;

	const wxString configdir = fc->getConfigDir();
	EXPECT_TRUE( wxFileName::DirExists( configdir ));

	{ // list config dir
		const uint filecount = wxDir::GetAllFiles( configdir, &filelist );
		EXPECT_GT( filecount, 0u );
	}

	const wxString yogadir = fc->getResourceDir( ConfigResource::Yoga );
	EXPECT_TRUE( wxFileName::DirExists( yogadir ));

	{ // list config dir
		const uint filecount = wxDir::GetAllFiles( yogadir, &filelist, "*.json" );
		EXPECT_GT( filecount, 0u );
	}
}

TEST( Json, ChartTest )
{
	ChartGeometryLoader *loader = ChartGeometryLoader::get();
	const std::vector<ChartGeometry> &geoms = loader->getConfigs();
	EXPECT_GT( geoms.size(), 0u );

	for( uint i = 0; i < geoms.size(); i++ )
	{
		EXPECT_GT( geoms[i].name.size(), 0u );
		EXPECT_GT( geoms[i].elements.size(), 0u );
	}
}

TEST( Json, PrintoutTest )
{
	SheetSpecLoader *loader = SheetSpecLoader::get();
	const std::vector<SheetSpec> &printouts = loader->getConfigs();
	EXPECT_GT( printouts.size(), 0u );

	for( uint i = 0; i < printouts.size(); i++ )
	{
		EXPECT_GT( printouts[i].name.size(), 0u );
		EXPECT_GT( printouts[i].items.size(), 0u );
	}

}




