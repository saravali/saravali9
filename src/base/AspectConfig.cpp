/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/AspectConfig.cpp
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

#include "AspectConfig.h"

#include "enumserializer.h"
#include "logger.h"

#include <format>

/*****************************************************
**
**   WesternAspectConfig   ---   Constructor
**
******************************************************/
WesternAspectConfig::WesternAspectConfig()
{
	typeConfigs = AspectConfigLoader::get()->getDefaultConfigs();
}

/*****************************************************
**
**   WesternAspectConfig   ---   getConfig
**
******************************************************/
SingleWesternAspectConfig &WesternAspectConfig::getConfig( const WesternAspectId &type )
{
	if ( typeConfigs.size() == 0 )
	{
		typeConfigs = AspectConfigLoader::get()->getDefaultConfigs();
	}
	const auto &cfg = typeConfigs.find( type );
	if ( cfg != typeConfigs.end())
	{
		return cfg->second;
	}
	else
	{
		errorlog << "WesternAspectConfig::getConfig cannot find AspectType " << type
			<< ", size of aspect config " << typeConfigs.size();;
	}
	return defaultConfig;
}

/*****************************************************
**
**   UranianAspectStyle   ---   Constructor
**
******************************************************/
UranianAspectStyle::UranianAspectStyle()
{
	penStyles = {
		{ TermId::Single, { wxPen( "red", 3, wxPENSTYLE_SOLID ), wxPen( "red", 1, wxPENSTYLE_LONG_DASH ) }},
		{ TermId::Midpoint, { wxPen( "green", 3, wxPENSTYLE_SOLID ), wxPen( "green", 1, wxPENSTYLE_LONG_DASH ) }},
		{ TermId::ReflectionPoint, { wxPen( "blue", 3, wxPENSTYLE_SOLID ), wxPen( "blue", 1, wxPENSTYLE_LONG_DASH ) }},
		{ TermId::Sum, { wxPen( "grey", 3, wxPENSTYLE_SOLID ), wxPen( "grey", 1, wxPENSTYLE_LONG_DASH ) }},
		{ TermId::Difference, { wxPen( "orange", 3, wxPENSTYLE_SOLID ), wxPen( "orange", 1, wxPENSTYLE_LONG_DASH ) }},
	};
}

/*****************************************************
**
**   UranianAspectStyle   ---   getPenStyle
**
******************************************************/
UranianAspectPenStyle UranianAspectStyle::getPenStyle( const TermId &termId ) const
{
	const auto a = penStyles.find( termId );
	if ( a != penStyles.end())
	{
		return a->second;
	}
	else
	{
		errorlog << "UranianAspectStyle::getPenStyle no style for term id " << termId;
		return UranianAspectPenStyle();
	}
}

/*****************************************************
**
**   AspectConfigLoader   ---   Constructor
**
******************************************************/
AspectConfigLoader::AspectConfigLoader()
	: Singleton(),
	defaultConfigs(
	{
		// Major
		{ WesternAspectId::Conjunction, { true, 5.0, wxPen( wxColour( 255, 200, 40 ), 10 ) }}, // orange
		{ WesternAspectId::Opposition, { true, 3.0, wxPen( wxColour( 0, 0, 255 ), 2  ) }}, // blue
		{ WesternAspectId::Trine, { true, 3.0, wxPen( wxColour( 0, 255, 0 ), 2 ) }}, // green
		{ WesternAspectId::Square, { true, 3.0, wxPen( wxColour( 255, 0, 0 ), 2 ) }}, // red
		{ WesternAspectId::Sextile, { true, 3.0, wxPen( wxColour( 0, 255, 0 ), 2 ) }}, // green

		// Minor
		{ WesternAspectId::Quincunx, { true, 1.0, wxPen( wxColour( 255, 0, 225 )) }}, // 5/12
		{ WesternAspectId::Semisquare, { false, 1.0, wxPen( wxColour( 218, 89, 85 )) }}, // 1/8
		{ WesternAspectId::Sesquisquare, { false, 1.0, wxPen( wxColour( 218, 89, 85 )) }}, // 3/8
		{ WesternAspectId::Semisextile, { false, 1.0, wxPen( wxColour( 85, 218, 113 )) }}, // 1/12

		{ WesternAspectId::Quintile, { false, 1.0, wxPen( wxColour( 85, 215, 218 )) }}, // 1/5
		{ WesternAspectId::Septile, { false, 1.0, wxPen( wxColour( 85, 189, 218 )) }}, // 1/7
		{ WesternAspectId::Novile, { false, 1.0, wxPen( wxColour( 218, 85, 182 )) }}, // 1/9

		// Declination
		{ WesternAspectId::Parallele, { false, 2.0, wxPen( wxColour( 133, 85, 218 )) }}, // parallele
		{ WesternAspectId::Contraparallele, { false, 2.0, wxPen( wxColour( 153, 218, 85 )) }}, // antiparallel
	})
{}

/*****************************************************
**
**   AspectConfigLoader   ---   getDefaultConfigs
**
******************************************************/
std::map<WesternAspectId,SingleWesternAspectConfig> AspectConfigLoader::getDefaultConfigs()
{
	return defaultConfigs;
}

/*****************************************************
**
**   AspectConfigLoader   ---   getAspectShortDescription
**
******************************************************/
wxString AspectConfigLoader::getAspectShortDescription( const WesternAspectType &type )
{
  switch( type.first )
  {
    case WesternAspectId::Parallele:
      return wxString( "par" );
    break;
    case WesternAspectId::Contraparallele:
      return wxString( "cpa" );
    break;
    default:
    break;
  }
  return std::format( "{}/{}", type.second, getDenominator( type.first ));
}

/*****************************************************
**
**   AspectConfigLoader   ---   getDenominator
**
******************************************************/
int AspectConfigLoader::getDenominator( const WesternAspectId &type )
{
  switch( type )
  {
    using enum WesternAspectId;
    case Opposition:
      return 2;
    break;
    case Trine:
      return 3;
    break;
    case Square:
      return 4;
    break;
    case Quintile:
      return 5;
    break;
    case Sextile:
      return 6;
    break;
    case Septile:
      return 7;
    break;
    case Semisquare:
    case Sesquisquare:
      return 8;
    break;
    case Novile:
      return 9;
    break;
    case Quincunx:
    case Semisextile:
      return 12;
    break;
    case Parallele:
    case Contraparallele:
    case Conjunction:
    default:
      return 1;
    break;
  }
}


