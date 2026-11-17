/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/logger.h
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

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "bootstrap.h"

#include <wx/log.h>
#include <wx/string.h>
#include <list>

class LogAppender;

enum class LogLevel
{
	// json:enum
	None = -1,
	Fatal,
	Error,
	Warning,
	Info,
	Debug,

	First = Fatal,
	Last = Debug
};

/*************************************************//**
*
* 
*
******************************************************/
class LogAdapter : public Singleton<LogAdapter>
{
	friend class Singleton<LogAdapter>;

public:

	// wx logger section
	void enableWxLogAppender( const bool = true );

	virtual void setWxLogLevel( const LogLevel& );

	LogLevel getWxLogLevel() const { return wxLevel; }

	// stdout logger section
	void enableStdoutLogAppender( const bool = true );

	void setStdoutLogLevel( const LogLevel& );

	LogLevel getStdoutLogLevel() const { return stdoutLogLevel; }

	LogLevel getMaxLogLevel() const { return std::max( wxLevel, stdoutLogLevel ); }

	bool isDebugLevel() const { return getMaxLogLevel() >= LogLevel::Debug; }

	wxString writeMessage( const LogLevel&, const char *file, const int &line, const wxString &message,
		const bool &includeTimestamp );

	void logMessage( const LogLevel&, const char *file, const int &line, const mstringstream& );

	wxLogLevel convertToWxLogLevel( const LogLevel& );

private:

	LogAdapter() {}

	LogLevel wxLevel = LogLevel::Info;

	LogLevel stdoutLogLevel = LogLevel::Info;

	wxString getTimestamp();

	LogAppender *wxLogAppender = nullptr;

	LogAppender *stdoutLogAppender = nullptr;
};

/*************************************************//**
*
* Simple log writer supporting c++ stream based input.
*
* Will be created as anonymous class and delivers message to LogAdapter in destructor.
*
******************************************************/
class LogWriter
{
public:

	LogWriter( const LogLevel &level, const char *file, const int &line )
		: level( level ), file( file ), line( line ) {}

	~LogWriter();

  template <typename T> inline LogWriter& operator<<( const T& log )
	{
		contents << log;
    return *this;
  }

protected:

	const LogLevel level = LogLevel::Info;

	const char *file = nullptr;
	
	const int line = -1;
	
	const char *func = nullptr;

	mstringstream contents;
};

#define debuglog if ( LogAdapter::get()->getMaxLogLevel() >= LogLevel::Debug ) LogWriter( LogLevel::Debug, __FILE__, __LINE__ )
#define infolog  if ( LogAdapter::get()->getMaxLogLevel() >= LogLevel::Info ) LogWriter( LogLevel::Info, __FILE__, __LINE__ )
#define warnlog  if ( LogAdapter::get()->getMaxLogLevel() >= LogLevel::Warning ) LogWriter( LogLevel::Warning, __FILE__, __LINE__ )
#define errorlog if ( LogAdapter::get()->getMaxLogLevel() >= LogLevel::Error ) LogWriter( LogLevel::Error, __FILE__, __LINE__ )
#define fatallog if ( LogAdapter::get()->getMaxLogLevel() >= LogLevel::Fatal ) LogWriter( LogLevel::Fatal, __FILE__, __LINE__ )

inline bool isDebugEnabled()
{
	return LogAdapter::get()->isDebugLevel();
}

#endif // _LOGGER_H_


