/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/AspectConfig.h
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
#ifndef _ASPECTCONFIG_H_
#define _ASPECTCONFIG_H_

#include "constants.h"

#include <map>
#include <wx/pen.h>

/*************************************************//**
*
* Astrological and graphical configuration for a single aspect in Western Chart.
*
******************************************************/
struct SingleWesternAspectConfig : public Jsonable
{
	SingleWesternAspectConfig()
	{
		pen = wxPen( *wxBLACK );
	}
	SingleWesternAspectConfig( const bool &active, const double &orbis, const wxPen &pen )
		: active( active ), orbis( orbis ), pen( pen ) {} 

	friend mostream& operator <<( mostream&, const SingleWesternAspectConfig& );

	// json:start
	bool active = true;

	double orbis = 1.0;

	wxPen pen;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*
*
******************************************************/
struct WesternAspectConfig : public Jsonable
{
	WesternAspectConfig();

	SingleWesternAspectConfig &getConfig( const WesternAspectId& );

	SingleWesternAspectConfig defaultConfig;

	// json:start
	bool showAspects = true;

	bool useColors = true;

	bool showSymbols = false;

	bool showOrbis = false;

	// key for type configs is WesternAspectId, not WesternAspectType alias std::pair<WesternAspectId,int>  (ignoring numerator)
	std::map<WesternAspectId,SingleWesternAspectConfig> typeConfigs;

	// impls hand-written in JsonHandler.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*  
*
******************************************************/
struct UranianAspectPenStyle : public Jsonable
{
	UranianAspectPenStyle() {}
	UranianAspectPenStyle( const wxPen &pen1, const wxPen & pen2 ) : mainPen( pen1 ), connectorPen( pen2 ) {}

	// json:start
	wxPen mainPen;

	wxPen connectorPen;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*  
*
******************************************************/
struct UranianAspectStyle : public Jsonable
{
	UranianAspectStyle();

	UranianAspectPenStyle getPenStyle( const TermId& ) const;

	// json:start
	bool showAspects = true;

	bool useColors = true;

	bool drawConnector = true;

	bool showDivision = false;

	bool showOrbis = false;

	std::map<TermId,UranianAspectPenStyle> penStyles;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*  loads aspect configuration from config stores information about default config
*
******************************************************/
class AspectConfigLoader : public Singleton<AspectConfigLoader>
{
	friend class Singleton<AspectConfigLoader>;
public:

	std::map<WesternAspectId,SingleWesternAspectConfig> getDefaultConfigs();

	wxString getAspectShortDescription( const WesternAspectType& );

private:

	AspectConfigLoader();

	int getDenominator( const WesternAspectId& );

	const std::map<WesternAspectId,SingleWesternAspectConfig> defaultConfigs;

};

#endif

