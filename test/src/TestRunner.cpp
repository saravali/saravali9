/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       test/src/TestRunner.cpp
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
#include "datelocation.h"
#include "FileConfig.h"
#include "gdiconfig.h"
#include "logger.h"
#include "TestRunner.h"

#include <mutex>

Astroconfig *astroconfig;
Gdiconfig *gdiconfig;

double gen_longitude( const Sign &sign, const int &deg, const int &min, const int sec )
{
	return ( 30 * (int)sign + deg + min / 60.0 + sec / 3600.0 );
}

double gen_longitude( const int &sign, const int &deg, const int &min, const int sec )
{
	return ( 30 * sign + deg + min / 60.0 + sec / 3600.0 );
}

double gen_degree( const int &deg, const int &min, const int sec )
{
	if ( deg >= 0 ) return ( deg + min / 60.0 + sec / 3600.0 );
	else return( deg - min / 60.0 - sec / 3600.0 );
	//return ( deg + min / 60.0 + sec / 3600.0 );
}

double neg_degree( const int &deg, const int &min, const int sec )
{
	return( - deg - min / 60.0 - sec / 3600.0 );
}

double gen_date( const int &year, const int &month, const int &day, const int hour, const int minute, const int second )
{
	const double t = hour + minute / 60.0 + second / 3600.0;
	return MDate( year, month, day, t ).jd;
}

std::mutex g_singleThread;

void SingleThreadedTests::SetUp()
{
	g_singleThread.lock();
}

void SingleThreadedTests::TearDown()
{
	g_singleThread.unlock();
}

/*****************************************************
**
**  MAIN
**
******************************************************/
int main( int argc, char **argv )
{
  testing::InitGoogleTest(&argc, argv);

	// init resources
	wxString basedir = "..";
	FileConfig *fileconfig = FileConfig::get();
	fileconfig->initHomedir();

	astroconfig = new Astroconfig;
	gdiconfig = new Gdiconfig;

	fileconfig->initDatadir( argv[0] );

	LogAdapter *logadapter = LogAdapter::get();
	logadapter->setStdoutLogLevel( gdiconfig->view.stdoutLogLevel );
	//logadapter->setLogLevel( wxLOG_Warning );

	return RUN_ALL_TESTS();
}


