/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/LanguageConfig.cpp
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

#include "LanguageConfig.h"

#include "astroconfig.h"
#include "FileConfig.h"
#include "logger.h"

extern Astroconfig *astroconfig;

//#define DEBUG_LANGUAGE_CONFIG

/*****************************************************
**
**   LanguageConfig   ---   Constructor
**
******************************************************/
LanguageConfig::LanguageConfig()
{
	availableLanguages =
	{
		"en",  // English
		"de",  // German
		"it",  // Italian
		"pl",  // Polish
		"ru",  // Russian
		"te",  // Telugu
		"hu",  // Hungarian
		"ro",  // Romanian
		"es",  // Spanish
		"fr",  // French
		"ta"   // Tamil
	};

	for( const auto &l : astroconfig->general.additionalLanguageCodes )
	{
		availableLanguages.insert( l );
	}

#ifdef DEBUG_LANGUAGE_CONFIG

	debuglog << "Info from wxLocale";
	for( const auto &l : availableLanguages )
	{
		const wxLanguageInfo *info = wxLocale::FindLanguageInfo( l );
		if ( info )
		{
			debuglog << "LanguageConfig Locale description " << info->Description
				<< " native " << info->DescriptionNative << " canonical " << info->GetCanonicalWithRegion();
		}
		else
		{
			errorlog << "No info for language " << l;
		}
	}
	const int currentLocale = wxLocale::GetSystemLanguage();
	debuglog << "System langugage: " << currentLocale;
	const wxLanguageInfo *info = wxLocale::GetLanguageInfo( currentLocale );
	if ( info )
	{
		// DescriptionNative and GetCanonicalWithRegion not available in wx 3.0
		debuglog << "LanguageConfig Locale description " << info->Description
			<< " native " << info->DescriptionNative << " canonical " << info->GetCanonicalWithRegion();
	}
	else
	{
		errorlog << "No info for default language";
	}

#endif
}

/*****************************************************
**
**   LanguageConfig   ---   getLanguageChoiceItems
**
******************************************************/
std::vector<std::pair<wxString,wxString>> LanguageConfig::getLanguageChoiceItems() const
{
	wxString defLangDesription = _( "Default Language" );

	const int currentLocale = wxLocale::GetSystemLanguage();
	const wxLanguageInfo *info = wxLocale::GetLanguageInfo( currentLocale );
	if ( info )
	{
		debuglog << "LanguageConfig Locale description " << info->Description;
		defLangDesription << ": " << info->DescriptionNative;
	}
	else
	{
		warnlog << "No info for default language";
	}

	std::vector<std::pair<wxString,wxString>> result = 
	{
		{ "", defLangDesription }
	};
	
	for( const auto &al : availableLanguages )
	{
		const wxLanguageInfo *info = wxLocale::FindLanguageInfo( al );
		if ( info )
		{
			wxString s = info->Description;
			debuglog << "LanguageConfig::getLanguageChoiceItems found " << s << " wxID " << info->Language;
			result.push_back( std::make_pair( al, s ));
		}
		else 
		{
			warnlog << "LanguageConfig::getLanguageChoiceItems no info for language code " << al;
		}
	}
	return result;
}

/*****************************************************
**
**   LanguageConfig   ---   getDoubleSeparationCharacter
**
******************************************************/
wxString LanguageConfig::getDoubleSeparationCharacter() const
{
	wxString ll = locale->GetInfo( wxLOCALE_DECIMAL_POINT );
	debuglog << "LanguageConfig::getDoubleSeparationCharacter " << ll;
	return ll;
}

/*****************************************************
**
**   LanguageConfig   ---   setLanguage
**
******************************************************/
void LanguageConfig::setLanguage()
{
	setLanguage( astroconfig->general.languageCode );
}

/*****************************************************
**
**   LanguageConfig   ---   setLanguage
**
******************************************************/
void LanguageConfig::setLanguage( const wxString &langCode )
{
	debuglog << "LanguageConfig::setLanguage langCode \"" << langCode << "\"";
	FileConfig *fc = FileConfig::get();

	if ( locale ) delete locale;
	locale = new wxLocale();

	wxLocale::AddCatalogLookupPathPrefix( fc->getResourceDir() );
	wxLocale::AddCatalogLookupPathPrefix( fc->getExecutableDir() );
	wxLocale::AddCatalogLookupPathPrefix( fc->getResourceDir( ConfigResource::I18n ));
	wxLocale::AddCatalogLookupPathPrefix( "." );

	int languageCode = wxLANGUAGE_DEFAULT;
	if ( ! langCode.IsEmpty())
	{
		const wxLanguageInfo *info = wxLocale::FindLanguageInfo( langCode );
		if ( info )
		{
			debuglog << "LanguageConfig found locale " << langCode << ", id is " << info->Language;
			languageCode = info->Language;
		}
		else
		{
			errorlog << "No info for language " << langCode;
		}
	}

	locale->Init( languageCode );
	locale->AddCatalog( APPLICATION_NAME );
}


