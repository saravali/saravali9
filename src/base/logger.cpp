/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/logger.cpp
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

#include "logger.h"

#include "enumserializer.h"
#include "gdiconfig.h"

#include <wx/datetime.h>
#include <wx/event.h>
#include <wx/log.h>
#include <wx/timer.h>
#include <iostream>

/*************************************************//**
*
* Appender similar to log4j. There are two implementations
* 1. Console, see ccp impl file
* 2. wxLog wrapper for logging to a window
*
*
******************************************************/
class LogAppender
{
public:

	virtual ~LogAppender() {}

	virtual void writeMessage( const LogLevel&, const char *file, const int &line, const mstringstream& ) = 0;

	virtual void setLogLevel( const LogLevel& ) = 0;

	virtual void flush() {}
};

/**************************************************************
***
**   CLASS   ---   WxLogAppender
***
***************************************************************/
class WxLogAppender : public LogAppender
{
public:

	virtual void writeMessage( const LogLevel &level, const char *file,
		const int &line, const mstringstream &o ) override 
	{
		wxLogGeneric( LogAdapter::get()->convertToWxLogLevel( level ), LogAdapter::get()->writeMessage( level, file, line, o.str(), false ));
	}


	virtual void setLogLevel( const LogLevel &level ) override
	{
		storedLevel = LogAdapter::get()->convertToWxLogLevel( level );
		wxLog::SetLogLevel( storedLevel );
	}

private:

	wxLogLevel storedLevel = wxLOG_Info;
};

/**************************************************************
***
**   CLASS   ---   StdOutAppender
***
***************************************************************/
class StdOutAppender : public LogAppender
{
public:

	virtual ~StdOutAppender() override
	{
		flush();
	}

	virtual void writeMessage( const LogLevel &level, const char *file, const int &line, const mstringstream &o ) override 
	{
		const wxString message = LogAdapter::get()->writeMessage( level, file, line, o.str(), true );
		switch( level )
		{
			case LogLevel::Fatal:
			case LogLevel::Error:
				mcerr << LogAdapter::get()->writeMessage( level, file, line, o.str(), true ) << std::endl;
			break;
			default:
				mcout << LogAdapter::get()->writeMessage( level, file, line, o.str(), true ) << std::endl;
			break;
		}
	}

	virtual void setLogLevel( const LogLevel &level ) override
	{
	}

	virtual void flush() override
	{
		std::cout << std::flush;
		std::cerr << std::flush;
	}
};

/**************************************************************
***
**   LogAdapter   ---   logMessage
***
***************************************************************/
void LogAdapter::logMessage( const LogLevel &level, const char *file,
	const int &line, const mstringstream &s )
{
	extern Gdiconfig *gdiconfig;

	//std::cout << " level " << level << " int level " << (int)level << " config level " << gdiconfig->view.stdoutLogLevel
		//<< " int " << (int)gdiconfig->view.stdoutLogLevel << std::endl;

	if ( gdiconfig )
	{
		if ( wxLogAppender && ( level <= gdiconfig->view.uiLogLevel || level <= gdiconfig->view.uiNotificationLogLevel ))
		{
			wxLogAppender->writeMessage( level, file, line, s );
		}

		if ( gdiconfig->view.enableStdoutLogging && stdoutLogAppender && level <= gdiconfig->view.stdoutLogLevel )
		{
			stdoutLogAppender->writeMessage( level, file, line, s );
		}
	}
	else // special mode: logger called before gdiconfig is initialized
	{
		if ( stdoutLogAppender )
		{
			stdoutLogAppender->writeMessage( level, file, line, s );
		}
	}
}

/**************************************************************
***
**   LogAdapter   ---   writeMessage
***
***************************************************************/
wxString LogAdapter::writeMessage( const LogLevel &level, const char *file, const int &line,
	const wxString &message, const bool &includeTimestamp )
{
	mstringstream ms;
	if ( includeTimestamp )
	{
		ms << getTimestamp() << " ";
	}
	ms << enum_to_string( level ) << " " << file << ":" << line << ": " << message;
	return ms.str();
}

/**************************************************************
***
**   LogAdapter   ---   enableWxLogAppender
***
***************************************************************/
void LogAdapter::enableWxLogAppender( const bool b )
{
	if ( wxLogAppender && !b )
	{
		delete wxLogAppender;
		wxLogAppender = nullptr;
	}
	else if ( ! wxLogAppender && b )
	{
		wxLogAppender = new WxLogAppender;
	}
	if ( wxLogAppender )
	{
		wxLogAppender->setLogLevel( wxLevel );
	}
}

/**************************************************************
***
**   LogAdapter   ---   convertToWxLogLevel
***
***************************************************************/
wxLogLevel LogAdapter::convertToWxLogLevel( const LogLevel &level )
{
	switch( level )
	{
		case LogLevel::Debug:
			return wxLOG_Debug;
		break;
		case LogLevel::Warning:
			return wxLOG_Warning;
		break;
		case LogLevel::Error:
			return wxLOG_Error;
		break;
		case LogLevel::Fatal:
			return wxLOG_FatalError;
		break;
		case LogLevel::Info:
		default:
		break;
	}
	return wxLOG_Info;
}

/**************************************************************
***
**   LogAdapter   ---   setWxLogLevel
***
***************************************************************/
void LogAdapter::setWxLogLevel( const LogLevel &level )
{
	wxLevel = level;
	if ( wxLogAppender )
	{
		wxLogAppender->setLogLevel( wxLevel );
	}
}

/**************************************************************
***
**   LogAdapter   ---   enableStdoutLogAppender
***
***************************************************************/
void LogAdapter::enableStdoutLogAppender( const bool b )
{
	if ( stdoutLogAppender && !b )
	{
		delete stdoutLogAppender;
		stdoutLogAppender = nullptr;
	}
	else if ( !stdoutLogAppender && b )
	{
		stdoutLogAppender = new StdOutAppender;
	}
	if ( stdoutLogAppender )
	{
		stdoutLogAppender->setLogLevel( stdoutLogLevel );
	}
}

/**************************************************************
***
**   LogAdapter   ---   setStdoutLogLevel
***
***************************************************************/
void LogAdapter::setStdoutLogLevel( const LogLevel &level )
{
	stdoutLogLevel = level;
	if ( stdoutLogAppender )
	{
		stdoutLogAppender->setLogLevel( level );
	}
}

/**************************************************************
***
**   LogAdapter   ---   getTimestamp
***
***************************************************************/
wxString LogAdapter::getTimestamp()
{
	const wxDateTime now = wxDateTime::Now();
	return now.Format( "%T", wxDateTime::Local );
}

/**************************************************************
***
**   LogWriter   ---   Destructor
***
***************************************************************/
LogWriter::~LogWriter()
{
	LogAdapter::get()->logMessage( level, file, line, contents );
}


