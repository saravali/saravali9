/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/ImageProvider.h
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

#ifndef _IMAGEPROVIDER_H_
#define _IMAGEPROVIDER_H_

#include "bootstrap.h"
#include "constants.h"

#include <map>
#include <wx/icon.h>
#include <wx/bmpbndl.h>
#include <wx/image.h>

enum class BitmapId;
class MImageCache;

/*************************************************//**
*
*  encapsulates access to pictures and icons
*
******************************************************/
class ImageProvider : public Singleton<ImageProvider>
{
  friend class Singleton<ImageProvider>;

public:

	~ImageProvider();

	/**
	* rotateHue is in degress, i.e. betwenn 0 and 360
	*/
	wxImage getImage( const wxString &filename, const double rotateHue = 0 );

	wxImage getImage( const BitmapId&, const IconSize& );

	wxBitmap getBitmap( const BitmapId&, const IconSize&, const double rotateHue = 0.0 );

	wxBitmapBundle getBitmapBundle( const BitmapId& );

	wxBitmap getDasaTreeBitmap( const DasaLevel &level, const bool &expanded );

	/**
	* rotateHue is in degress, i.e. betwenn 0 and 360
	*/
	wxBitmap getBitmap( const wxString &filename, const double rotateHue = 0 );

	wxIcon getIcon( const BitmapId& );

private:

	ImageProvider();

	void initFilenames();

	wxImage loadImage( const wxString &filename, const double rotateHue = 0 );

	std::map<BitmapId, wxString> bitmapFilenames;

	wxImage errorImage;
};

#endif


