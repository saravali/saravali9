/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/FontTool.cpp
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

#include "FontTool.h"
#include "gdiconfig.h"

extern Gdiconfig *gdiconfig;

//#define MAX_FONT_SIZE 500
#define DEFAULT_FONT_NAME "serif"
//#define DEFAULT_FONT_MONO_NAME "monospace"

//#define DEBUG_FONTTOOL

const double FontTool::defaultSizeFactors[7] = { .75, .83, 1.0, 1.2, 1.44, 1.73, 2 };

/**************************************************************
***
**   FontTool   ---   getShrinkrateList
***
***************************************************************/
std::vector<int> FontTool::getShrinkrateList( const TextItemSubType type )
{
	const int pointSize = getWxFontForItem( type, gdiconfig->fonts.graphicFontSize ).GetPointSize();
  return getShrinkrateList( pointSize );
}

/**************************************************************
***
**   FontTool   ---   getShrinkrateList
***
***************************************************************/
std::vector<int> FontTool::getShrinkrateList( const int &baseSize )
{
	std::vector<int> l = { 0 };
	int i = 1;
	while( baseSize - i >= MIN_FONT_SIZE )
	{
		// continue for odd font sizes > 12
		if ( baseSize - i > 12 && ! (( baseSize - i ) % 2 )) continue;
		l.push_back( i++ );
	}
	return l;
}

/*****************************************************
**
**   FontTool   ---   getScaledTextSize
**
******************************************************/
int FontTool::getScaledTextSize( const int &index, const int &baseSize )
{
	assert( index >= 0 && index < 7 );
	// return (int)( baseSize * defaultSizeFactors[index]);
	return createFontSize( (int)( baseSize * defaultSizeFactors[index]) );
}

/*****************************************************
**
**   FontTool   ---   fillTextSizesArray
**
******************************************************/
void FontTool::fillTextSizesArray( int *sizes, const int &as )
{
	for( int i = 0; i < 7; i++ ) sizes[i] = createFontSize(as * defaultSizeFactors[i] );
}

/*****************************************************
**
**   FontTool   ---   createFontSize
**
******************************************************/
int FontTool::createFontSize( const int &basesize )
{
	if ( basesize >= MIN_FONT_SIZE && basesize <= MAX_FONT_SIZE )
	{
		return basesize;
	}
	else if ( basesize < MIN_FONT_SIZE )
	{
		debuglog << "FontTool::createFontSize with too small base size " << basesize << ", using " << MIN_FONT_SIZE;
		return MIN_FONT_SIZE;
	}
	else if ( basesize > MAX_FONT_SIZE )
	{
		debuglog << "FontTool::createFontSize with too large base size " << basesize << ", using " << MAX_FONT_SIZE;
		return MAX_FONT_SIZE;
	}
	else assert( false );
	return 0;
}

/*************************************************//**
*
*   FontTool   ---   getWxFontForItem
*
******************************************************/
wxFont FontTool::getWxFontForItem( const TextItemSubType &type, const int &basicSize )
{
	constexpr wxFontFamily family = wxFONTFAMILY_DEFAULT;
	wxFontWeight weight = wxFONTWEIGHT_NORMAL;
	int pointSize = basicSize;
	wxFontStyle style = wxFONTSTYLE_NORMAL;
	bool underline = false;
	wxString facename = gdiconfig->fonts.graphicFontFacename;

  switch( type )
  {
    case TextItemSubType::Header1:
      pointSize = getScaledTextSize( 4, basicSize );
      weight = wxFONTWEIGHT_BOLD;
    break;
    case TextItemSubType::Header2:
      pointSize = getScaledTextSize( 3, basicSize );
      weight = wxFONTWEIGHT_BOLD;
    break;
    case TextItemSubType::Paragraph:
    case TextItemSubType::Line:
    case TextItemSubType::TableCell:
    case TextItemSubType::ErrorMessage:
      pointSize = getScaledTextSize( 2, basicSize );
    break;
    case TextItemSubType::TableHeader:
      weight = wxFONTWEIGHT_BOLD;
    break;
    case TextItemSubType::Footnote:
      pointSize = getScaledTextSize( 1, basicSize );
    break;
    default:
      assert( false );
    break;
  }
#ifdef DEBUG_FONTTOOL
  debuglog << "Finished FontTool::getFontForItem size " << pointSize
    << " type " << static_cast<int>( type ) ;
#endif
  return wxFont( pointSize, family, style, weight, underline, facename );
}

