/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ColorConfig.h
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

#ifndef _COLORCONFIG_H_
#define _COLORCONFIG_H_

#include "gdibase.h"
#include "constants.h"
#include "Planet.h"

#include <array>
#include <map>
#include <variant>
#include <vector>

#include <wx/colour.h>

#define MAX_BARDIAGRAM_COLORS 22 // 12 + 10
#define ASSERT_VALID_DASA_COLOR( p ) assert( p >= 0 && p < MAX_DASA_COLORS );

/*************************************************//**
*
*  configuration for bar diagram styles, i.e. graphical Dasa, varga etc diagrams
*
******************************************************/
struct BarDiagramStyle : public Jsonable
{
	BarDiagramStyle() {}
	BarDiagramStyle( const wxPen &pen, const MBrush &brush ) :	pen( pen ), brush( brush ) {} 

	bool isOk() const;

	// json:start
	wxPen pen;
	MBrush brush;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct BarDiagramConfig : public Jsonable
{
	BarDiagramConfig();
	bool isOk() const;

	void setDefaultConfig();

	/**
	*  Convert object/sign id to position in the list.
	*/
	static int getConfigId( const std::variant<PlanetId,Sign>& );

	/**
	*  Convert position in the list to corresponding object/sign id.
	*/
	static std::variant<PlanetId,Sign> getByGraphicalId( const int& );

	// json:start
	std::vector<BarDiagramStyle> styles;

	int cornerRadius;
	bool doItemZoom;
	int itemZoom;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct BgPalette : public Jsonable
{
	virtual ~BgPalette() {}

	bool isOk() const { return brushes.size() > 0; }

	MBrush getBrush( const uint &i ) const;
	//friend mostream& operator <<( mostream&, const BgPalette& );

	// json:start
	std::vector<MBrush> brushes;

	wxColour textColor;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct ChartFieldPattern : public Jsonable
{
	// json:start
	wxString paletteName;

	BgPalette palette;

	bool isValid() const { return ! paletteName.empty() || palette.isOk(); }

	MBrush getBrush( const uint& ) const;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*  configuration for chart colors
*
******************************************************/
struct ChartColorConfig : public Jsonable
{
	ChartColorConfig() { setDefaultConfig(); }

	void setDefaultConfig();

	wxColour &getSignFgColor( const Sign& );
	wxColour &getHouseFgColor( const House& );
	wxColour &getPlanetColor( const PlanetId& );

	// json:start
	// sign and house colors per element
	std::array<wxColour,4> signFgColors;
	std::array<wxColour,4> houseFgColors;

	std::array<wxColour,MAX_EPHEM_FACTORS> planetFgColors;

	BgPalette signPalette;
	BgPalette housePalette;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
class PaletteManager : public Singleton<PaletteManager>
{
	friend class Singleton<PaletteManager>;

public:

	bool hasPalette( wxString name );
	BgPalette *getPalette( wxString name );

	std::vector<wxString> getAllKeys();
	void setPalette( const wxString name, const BgPalette& );
	void loadFallbackConfig();

private:
	PaletteManager();
	void init();

	std::map<wxString, BgPalette*> m;
};

/*************************************************//**
*
*  general configuration for colors
*
******************************************************/
struct ColorConfig : public Jsonable
{
	ColorConfig();
	void setDefaultConfig();
	bool isOk() const;

	wxPen getDefaultPen();

	// save access to foreground color
	wxColour getDefaultFgColour();

	// json:start
	wxPen defaultPen;

	wxColour bgColor;
	wxColour textFgColor;
	wxColour markedFieldsBgColor;
	wxColour beneficFgColor;
	wxColour maleficFgColor;
	wxColour transitFgColor;
	wxColour errorBgColor;
	wxColour chartMarkupBgColor;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

#endif

