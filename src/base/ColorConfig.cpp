/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ColorConfig.cpp
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

#include "ColorConfig.h"

#include "gdiconfig.h"
#include "mathbase.h"

extern Gdiconfig *gdiconfig;

/*
#define SIGN_RED "#E396967F"
#define SIGN_YELLOW "#ece5a6"
#define SIGN_GREEN "#b0eca6"
#define SIGN_BLUE "#a6cdec"
*/
#define SIGN_RED "#E396967F"
#define SIGN_YELLOW "#ece5a67F"
#define SIGN_GREEN "#b0eca67F"
#define SIGN_BLUE "#a6cdec7F"

#define DEFAULT_FG_COLOR "BLACK"
#define DEFAULT_BG_COLOR "WHITE"
#define TEXT_FG_COLOR "BLACK"
#define DEFAULT_MARKED_TEXT_BG_COLOR "GREY"
#define DEFAULT_SBC_BENEFIC_COLOR "GREEN"
#define DEFAULT_SBC_MALEFIC_COLOR "MAROON"
#define DEFAULT_TRANSIT_COLOR "BLUE"
#define DEFAULT_ERROR_BG_COLOR "#c15757"
#define DEFAULT_MARKUP_BG_COLOR "#c15757"

/*****************************************************
**
**   BgPalette   ---   getBrush
**
******************************************************/
MBrush BgPalette::getBrush( const uint &i ) const
{
	if ( brushes.size() > 0 )
	{
		return brushes[ i % brushes.size()];
	}
	else
	{
		debuglog << "BgPalette::getBrush. Size of brushes is zero";
		return MBrush( "red" );
	}
}

/*****************************************************
**
**   ChartFieldPattern   ---   getBrush
**
******************************************************/
MBrush ChartFieldPattern::getBrush( const uint &i ) const
{
  if ( ! paletteName.empty() )
  {
    PaletteManager *man = PaletteManager::get();
    BgPalette *palette = man->getPalette( paletteName );
    if ( ! palette )
    {
      warnlog << "ChartFieldPattern::getBrush: no palette named " << paletteName;
			return MBrush( *wxRED );
    }
		else return palette->getBrush( i );
  }
	else if ( this->palette.isOk())
	{
		return this->palette.getBrush( i );
	}
	warnlog << "ChartFieldPattern::getBrush: neither palette name set nor bgpalette ok, setting dummy background for id "
		<< i;
	return MBrush( *wxYELLOW );
}

/*****************************************************
**
**   PaletteManager   ---   Constructor
**
******************************************************/
PaletteManager::PaletteManager()
{
	init();
}

/*****************************************************
**
**   PaletteManager   ---   init
**
******************************************************/
void PaletteManager::init()
{
	loadFallbackConfig();
}

/*****************************************************
**
**   PaletteManager   ---   hasPalette
**
******************************************************/
bool PaletteManager::hasPalette( wxString name )
{
	return m.find( name ) != m.end();
}

/*****************************************************
**
**   PaletteManager   ---   getAllKeys
**
******************************************************/
std::vector<wxString> PaletteManager::getAllKeys()
{
	std::vector<wxString> v;
	for( const auto &item : m )
	{
		v.push_back( item.first );
	}
	return v;
}

/*****************************************************
**
**   PaletteManager   ---   setPalette
**
******************************************************/
void PaletteManager::setPalette( wxString name, const BgPalette &palette )
{
	if ( m.find( name ) != m.end()) delete m[ name ];
	m[ name ] = new BgPalette( palette );
}

/*****************************************************
**
**   PaletteManager   ---   getPalette
**
******************************************************/
BgPalette *PaletteManager::getPalette( wxString name )
{
	if ( name == wxString( "signstock" ))
	{
		return &gdiconfig->chartColors.signPalette;
	}
	if ( name == wxString( "housestock" ))
	{
		return &gdiconfig->chartColors.housePalette;
	}
	else return m[name];
}

/*****************************************************
**
**   PaletteManager   ---   loadFallbackConfig
**
******************************************************/
void PaletteManager::loadFallbackConfig()
{
	{
		BgPalette *p = new BgPalette;
		p->brushes.push_back( MBrush( wxColour( "#db545f7f" )));
		p->brushes.push_back( MBrush( wxColour( "#eda0687f" )));
		p->brushes.push_back( MBrush( wxColour( "#fbfc557f" )));
		p->brushes.push_back( MBrush( wxColour( "#8dd83d7f" )));
		p->brushes.push_back( MBrush( wxColour( "#52c9397f" )));
		p->brushes.push_back( MBrush( wxColour( "#54ca8e7f" )));
		p->brushes.push_back( MBrush( wxColour( "#56c7f57f" )));
		p->brushes.push_back( MBrush( wxColour( "#64aaf57f" )));
		p->brushes.push_back( MBrush( wxColour( "#5686df7f" )));
		p->brushes.push_back( MBrush( wxColour( "#8084d67f" )));
		p->brushes.push_back( MBrush( wxColour( "#f253b07f" )));
		p->brushes.push_back( MBrush( wxColour( "#e1528f7f" )));
		/*
		p->brushes.push_back( MBrush( wxColour( 219, 84, 95, 127 )));
		p->brushes.push_back( MBrush( wxColour( 237, 160, 104, 127 )));
		p->brushes.push_back( MBrush( wxColour( 251, 252, 85, 127 )));
		p->brushes.push_back( MBrush( wxColour( 141, 216, 61, 127 )));
		p->brushes.push_back( MBrush( wxColour( 82, 201, 57, 127 )));
		p->brushes.push_back( MBrush( wxColour( 84, 202, 142, 127 )));
		p->brushes.push_back( MBrush( wxColour( 86, 199, 245, 127 )));
		p->brushes.push_back( MBrush( wxColour( 100, 170, 245, 127 )));
		p->brushes.push_back( MBrush( wxColour( 86, 134, 223, 127 )));
		p->brushes.push_back( MBrush( wxColour( 128, 132, 214, 127 )));
		p->brushes.push_back( MBrush( wxColour( 242, 83, 176, 127 )));
		p->brushes.push_back( MBrush( wxColour( 225, 82, 143, 127 )));
		*/
		p->textColor = DEFAULT_FG_COLOR;
		m[ "rainbow" ] = p;
	}

	{
		BgPalette *p = new BgPalette;
		p->brushes.push_back( MBrush( "#de402b" ));
		p->brushes.push_back( MBrush( "#eaaaa2" ));
		p->textColor = DEFAULT_FG_COLOR;
		m[ "redstyle" ] = p;
	}

	{
		BgPalette *p = new BgPalette;
		p->brushes.push_back( MBrush( wxColour( "#61993B" ))); // Aries: yellowgreen
		p->brushes.push_back( MBrush( wxColour( "#FFFF00" ))); // yellow
		p->brushes.push_back( MBrush( wxColour( "#E2B007" ))); // golden yellow
		p->brushes.push_back( MBrush( wxColour( "#FFA500" ))); // Cancer: orange red
		p->brushes.push_back( MBrush( wxColour( "#FF0000" ))); // red
		p->brushes.push_back( MBrush( wxColour( "#8B3487" ))); // Purpur
		p->brushes.push_back( MBrush( wxColour( "#8F00FF" ))); // Libra: violet
		p->brushes.push_back( MBrush( wxColour( "#2B2C7C" ))); // ultramarine
		p->brushes.push_back( MBrush( wxColour( "#4B0082" ))); // Indigo
		p->brushes.push_back( MBrush( wxColour( "#0000FF" ))); // Capricorn: blue
		p->brushes.push_back( MBrush( wxColour( "#006262" ))); // greenblue
		p->brushes.push_back( MBrush( wxColour( "#00FF00" ))); // green
		p->textColor = DEFAULT_FG_COLOR;
		m[ "witte" ] = p;
	}

	{
		BgPalette *p = new BgPalette;
		p->brushes.push_back( MBrush( wxColour( "#FE9F79" )));
		p->brushes.push_back( MBrush( wxColour( "#D1B38B" )));
		p->brushes.push_back( MBrush( wxColour( "#EFE58B" )));
		p->brushes.push_back( MBrush( wxColour( "#AFDFE5" )));
		p->textColor = DEFAULT_FG_COLOR;
		m[ "desert" ] = p;
	}

	{
		BgPalette *p = new BgPalette;
		wxString bmpname = "madera.jpg";
		p->brushes.push_back( MBrush( bmpname, 240 ));
		p->brushes.push_back( MBrush( bmpname, 180 ));
		p->brushes.push_back( MBrush( bmpname, 120 ));
		p->brushes.push_back( MBrush( bmpname, 15 ));
		m[ "madera" ] = p;
	}

	{
		wxString bmpname = "marble.jpg";
		BgPalette *p = new BgPalette;
		p->brushes.push_back( MBrush( bmpname, 240 ));
		p->brushes.push_back( MBrush( bmpname, 180 ));
		p->brushes.push_back( MBrush( bmpname, 120 ));
		p->brushes.push_back( MBrush( bmpname, 15 ));
		m[ "marble" ] = p;
	}
}

/*****************************************************
**
**   BarDiagramStyle   ---   isOk
**
******************************************************/
bool BarDiagramStyle::isOk() const
{
	if ( ! pen.IsOk()) return false;
	if ( ! brush.isOk()) return false;
	return true;
}

/*****************************************************
**
**   BarDiagramConfig   ---   Constructor
**
******************************************************/
BarDiagramConfig::BarDiagramConfig()
{
	setDefaultConfig();
}

/*****************************************************
**
**   BarDiagramConfig   ---   isOk
**
******************************************************/
bool BarDiagramConfig::isOk() const
{
 	if ( styles.size() != MAX_BARDIAGRAM_COLORS ) return false;
	for( const auto &style : styles )
	{
		if ( ! style.isOk()) return false;
	}
	return true;
}

/*****************************************************
**
**   BarDiagramConfig   ---   setDefaultConfig
**
******************************************************/
void BarDiagramConfig::setDefaultConfig()
{
	cornerRadius = 5;
	doItemZoom = true;
	itemZoom = 1;

	// Signs: Jataka Parijata I, 23; planets: II 19
	styles = {
		// Aries
		{ *wxWHITE_PEN, MBrush( *wxRED ) },
		{ *wxBLACK_PEN, MBrush( *wxWHITE ) },
		{ *wxBLACK_PEN, MBrush( wxColour( 0, 204, 51 ) ) },

		// Cancer
		{ *wxBLACK_PEN, MBrush( wxColour( 255, 160, 255 )) },
		{ *wxBLACK_PEN, MBrush( wxColour( 226, 255, 211 )) },
		{ *wxBLACK_PEN, MBrush( "piebald.png", 0 ) },

		// Libra
		{ *wxWHITE_PEN, MBrush( *wxBLACK ) },
		{ *wxBLACK_PEN, MBrush( wxColour( 229, 169, 104 )) },
		{ *wxWHITE_PEN, MBrush( wxColour( 167, 123, 76 )) },

		// Capricorn
		{ *wxBLACK_PEN, MBrush( "rainbow.png", 0 ) },
		{ *wxWHITE_PEN, MBrush( wxColour( 104, 77, 47 )) },
		{ *wxBLACK_PEN, MBrush( *wxWHITE ) },

		// Sun, ..., Saturn
		{ *wxBLACK_PEN, MBrush( wxColour( 255, 94, 7 )) },
		{ *wxBLACK_PEN, MBrush( *wxWHITE ) },
		{ *wxBLACK_PEN, MBrush( wxColour( 102, 204, 102 )) }, // Merc
		{ *wxBLACK_PEN, MBrush( "rainbow.png", 0 ) },         // Venus
		{ *wxWHITE_PEN, MBrush( wxColour( 204, 0, 0 )) },     // Mars
		{ *wxBLACK_PEN, MBrush( wxColour( 255, 255, 102 )) }, // Jup
		{ *wxWHITE_PEN, MBrush( *wxBLACK ) },

		// Rahu, Ketu, Ascendant
		{ *wxWHITE_PEN, MBrush( wxColour( 82, 109, 198 )) },
		{ *wxBLACK_PEN, MBrush( wxColour( 132, 132, 132 )) },
		{ *wxBLACK_PEN, MBrush( wxColour( 132, 132, 132 )) },
	};
}

/*****************************************************
**
**   BarDiagramConfig   ---   getByGraphicalId
**
******************************************************/
std::variant<PlanetId,Sign> BarDiagramConfig::getByGraphicalId( const int &id )
{
	assert( id >= 0 && id < MAX_BARDIAGRAM_COLORS );
	if ( id >= 0 && id < 12 )
	{
		return (Sign)id;
	}
	else if ( id <= 18 ) return(PlanetId)(id - 12 );
	else if ( id == 19 ) return PlanetId::AscNode;
	else if ( id == 20 ) return PlanetId::DescNode;
	else if ( id == 21 ) return PlanetId::Ascendant;
	else assert( false );
	return PlanetId::None;
}

/*****************************************************
**
**   BarDiagramConfig   ---   getConfigId
**
******************************************************/
int BarDiagramConfig::getConfigId( const std::variant<PlanetId,Sign> &id )
{
	if ( std::holds_alternative<PlanetId>( id ))
	{
		//return getConfigId( std::get<PlanetId>( id ));
		const PlanetId ob = std::get<PlanetId>( id );
		int p = -1;

		if ( (int)ob >= (int)PlanetId::Sun && (int)ob <= (int)PlanetId::Saturn ) p = (int)ob;
		else if ( ob == PlanetId::AscNode ) p = 7;
		else if ( ob == PlanetId::DescNode ) p = 8;
		else if ( ob == PlanetId::Ascendant || (int)ob == 7 ) p = 9; // caution: Ascendant on 7 (AV) TODO
		else assert( false );
		return p + 12;
	}
	else
	{
		const Sign rasi =  std::get<Sign>( id );
		assert_valid_enum<Sign>( rasi );
		return (int)rasi;
	}
}

/*****************************************************
**
**   ColorConfig   ---   Constructor
**
******************************************************/
ColorConfig::ColorConfig()
{
	setDefaultConfig();
}

/*****************************************************
**
**   ColorConfig   ---   setDefaultConfig
**
******************************************************/
void ColorConfig::setDefaultConfig()
{
	bgColor = DEFAULT_BG_COLOR;
	textFgColor = TEXT_FG_COLOR;
	markedFieldsBgColor = DEFAULT_MARKUP_BG_COLOR;
	beneficFgColor = DEFAULT_SBC_BENEFIC_COLOR;
	maleficFgColor = DEFAULT_SBC_MALEFIC_COLOR;
	transitFgColor = DEFAULT_TRANSIT_COLOR;
	errorBgColor = DEFAULT_ERROR_BG_COLOR;
	chartMarkupBgColor = DEFAULT_ERROR_BG_COLOR;

	defaultPen = wxPen( DEFAULT_FG_COLOR, 1, wxPENSTYLE_SOLID );
}

/*****************************************************
**
**   ColorConfig   ---   getDefaultPen
**
******************************************************/
wxPen ColorConfig::getDefaultPen()
{
	if ( defaultPen.IsOk() )
  {
    return defaultPen;
  }
  warnlog << "ColorConfig::getDefaultPen: default pen is not okay, using fallback (black)";
  return *wxBLACK_PEN;
}

/*****************************************************
**
**   ColorConfig   ---   getDefaultFgColour
**
******************************************************/
wxColour ColorConfig::getDefaultFgColour()
{
	if ( defaultPen.GetColour().IsOk() )
  { 
    return defaultPen.GetColour();
  }
  warnlog << "gdiconfig->colors.defaultPen has invalid color, using black";
	return *wxBLACK;
}

/*****************************************************
**
**   ColorConfig   ---   isOk
**
******************************************************/
bool ColorConfig::isOk() const
{
	if ( !  bgColor.IsOk()) return false;
	if ( !  markedFieldsBgColor.IsOk()) return false;
	if ( !  beneficFgColor.IsOk()) return false;
	if ( !  maleficFgColor.IsOk()) return false;
	if ( !  transitFgColor.IsOk()) return false;
	if ( !  errorBgColor.IsOk()) return false;
	if ( !  chartMarkupBgColor.IsOk()) return false;
	if ( !  defaultPen.IsOk()) return false;
	return true;
}

/*****************************************************
**
**   ChartColorConfig   ---   getPlanetColor
**
******************************************************/
wxColour &ChartColorConfig::getPlanetColor( const PlanetId &f )
{
	if( PlanetHelper::isEphemPlanet( f )) return planetFgColors[(int)f];
	else return gdiconfig->colors.textFgColor;
}

/*****************************************************
**
**   ChartColorConfig   ---   getSignFgColor
**
******************************************************/
wxColour &ChartColorConfig::getSignFgColor( const Sign &sign )
{
	if ( ! is_valid_enum( sign ))
	{
		errorlog << "ChartColorConfig::getSignFgColor with invalid sign " << (int)sign;
	}
	return signFgColors[((int)sign) % 4];
}

/*****************************************************
**
**   ChartColorConfig   ---   getHouseFgColor
**
******************************************************/
wxColour &ChartColorConfig::getHouseFgColor( const House &h )
{
	assert_valid_enum<House>( h );
	return houseFgColors[ ((int)h) % 4];
}

/*****************************************************
**
**   ChartColorConfig   ---   setDefaultConfig
**
******************************************************/
void ChartColorConfig::setDefaultConfig()
{
	int i;

	const static char *k_sign_house_fg_color[4] =
	{
		"RED",
		"YELLOW",
		"GREEN",
		"BLUE"
	};
	const static char *k_factor_color[MAX_EPHEM_FACTORS] =
	{
		"RED",    // Sun
		"BLUE",   // Moon
		"GREEN",  // Merc
		"YELLOW", // Ven
		"RED",    // Mars
		"RED",    // Jup
		"YELLOW", // Sat
		"GREEN",  // Uran
		"BLUE",   // Nep
		"BLUE",   // Plu
		"GREY",   // AscNode
		"GREY",   // Desc Node
		"GREY",   // Mean AscNode UNUSED
		"GREY",   // Mean Desc Node UNUSED
		"RED",    // Ascendant
		"YELLOW", // MC
		"GREEN", // Descendant
		"BLUE", // IC
		"YELLOW", // Cupido
		"NAVY",   // Hades
		"GREEN",  // Zeus
		"NAVY",   // Kronos
		"RED",    // Apollon
		"YELLOW", // Admetos
		"RED",    // Vulkanus
		"BLUE",   // Poseidon
		"RED",   // Chiron
		"RED",   // Pholus
		"PURPLE",   // Plan1
		"PURPLE",   //
		"PURPLE",   //
		"PURPLE",   // Plan4
		"GREY"   // Lilith
	};
	for ( i = 0; i < 4; i++ )
	{
		signFgColors[i] = k_sign_house_fg_color[i];
		houseFgColors[i] = k_sign_house_fg_color[i];
	}
	for ( i = 0; i < MAX_EPHEM_FACTORS; i++ )
	{
		planetFgColors[i] = k_factor_color[i];
	}

	signPalette.brushes.push_back( MBrush( wxColour( SIGN_RED )));
	signPalette.brushes.push_back( MBrush( wxColour( SIGN_YELLOW )));
	signPalette.brushes.push_back( MBrush( wxColour( SIGN_GREEN )));
	signPalette.brushes.push_back( MBrush( wxColour( SIGN_BLUE )));

	housePalette.brushes.push_back( MBrush( wxColour( SIGN_RED )));
	housePalette.brushes.push_back( MBrush( wxColour( SIGN_YELLOW )));
	housePalette.brushes.push_back( MBrush( wxColour( SIGN_GREEN )));
	housePalette.brushes.push_back( MBrush( wxColour( SIGN_BLUE )));
}

