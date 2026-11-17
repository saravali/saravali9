/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ConfigLoaderImpls.h
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

#ifndef _CONFIGLOADERIMPLS_H_
#define _CONFIGLOADERIMPLS_H_

#include "ColorConfig.h"
#include "ConfigListItems.h"
#include "ConfigListLoader.h"
#include "constants.h"

/*************************************************//**
*
*
*
******************************************************/
class DasaConfigLoader : public ConfigLoader<DasaConfig>, public Singleton<DasaConfigLoader>
{
  friend class Singleton<DasaConfigLoader>;

	DasaConfigLoader() : ConfigLoader<DasaConfig>( ConfigResource::Dasa ) {}

};

/*************************************************//**
*
* 
*
******************************************************/
class TableStyleLoader : public ConfigLoader<TableStyle>, public Singleton<TableStyleLoader>
{
	friend class Singleton<TableStyleLoader>;

public:

	void saveConfigs();

private:

	TableStyleLoader() : ConfigLoader<TableStyle>( ConfigResource::Table )	{}
};

/*************************************************//**
*
* 
*
******************************************************/
class WidgetSkinLoader : public ConfigLoader<WidgetSkin>, public Singleton<WidgetSkinLoader>
{
	friend class Singleton<WidgetSkinLoader>;

	WidgetSkinLoader() : ConfigLoader<WidgetSkin>( ConfigResource::Skin )	{}
};

/*************************************************//**
*
*  reads and contains the system wide Yoga configs
*
******************************************************/
class YogaConfigLoader : public ConfigLoader<YogaConfig>, public Singleton<YogaConfigLoader>
{
  friend class Singleton<YogaConfigLoader>;

public:

	void saveConfigs( wxString filename, std::vector<YogaConfig>& );

	void loadConfigs( wxString filename, std::vector<YogaConfig>& );

private:


#ifdef CPP26
	virtual void postLoad() override;
#else
	virtual void loadSingleConfig( const Json::Value&, JsonHandler& ) override;
#endif

	Rule *parseRule( const wxString& );

	YogaConfigLoader() : ConfigLoader<YogaConfig>( ConfigResource::Yoga ) {}
};


#endif // _CONFIGLOADERIMPLS_H_


