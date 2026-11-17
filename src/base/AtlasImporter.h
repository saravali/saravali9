/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/AtlasImporter.h
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

#ifndef _ATLASIMPORTER_H
#define _ATLASIMPORTER_H

#include <wx/string.h>

class AtlasImportLifeCycleWorker;
class AtlasImportWorker;
class AtlasImportSharedSection;

enum class ThreadStatus
{ 
	None = -1,
	Waiting,
	Working,
	Canceled,
	Finished,
	Error
};

enum class ThreadOrder
{
	None = -1,
	Work,
	Cancel,
	Exit
};

/*************************************************//**
*
*  
*
******************************************************/
class AtlasImporter
{
public:

	AtlasImporter();
	~AtlasImporter();

	bool isRunning() { return getImportStatus() == ThreadStatus::Working; }
	bool isCanceled() { return getImportStatus() == ThreadStatus::Canceled;; }
	bool isFinished() { return getImportStatus() == ThreadStatus::Finished; }
	bool hasErrors() { return getImportStatus() == ThreadStatus::Error; }

	bool hasNews();
	void aknowledgeNews();
	void abort();
	void run();

	ThreadStatus getImportStatus();

	void setImportFile( wxString );
	void setDatabaseFile( wxString );

	wxString writeRunStatus();
	wxString writeErrorStatus();

protected:
	AtlasImportSharedSection *sharedSection;
	AtlasImportLifeCycleWorker *lifeCycleWorker;
	AtlasImportWorker *importWorker;
};


#endif

