/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/ImageProvider.cpp
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

#include "ImageProvider.h"

#include "Commands.h"
#include "Dasa.h"
#include "enumserializer.h"
#include "FileConfig.h"
#include "logger.h"
#include "constants.h"
#include "mathbase.h"

#include "error.xpm"

#include <wx/bitmap.h>
#include <wx/filename.h>

#include "ImageProvider_generated.cpp"

//#define DEBUG_IMAGEPROVIDER

/*****************************************************
**
**   ImageProvider   ---   Constructor
**
******************************************************/
ImageProvider::ImageProvider()
{
	initFilenames();
	errorImage = wxImage( error_xpm );
}

/*****************************************************
**
**   ImageProvider   ---   Destructor
**
******************************************************/
ImageProvider::~ImageProvider()
{
}

/*****************************************************
**
**   ImageProvider   ---   getImage
**
******************************************************/
wxImage ImageProvider::getImage( const wxString &filename, const double angle )
{
#ifdef DEBUG_IMAGEPROVIDER
	debuglog << "ImageProvider::getImage filename " << filename << " angle " << angle;
#endif
	return loadImage( filename, angle );
}

/*****************************************************
**
**   ImageProvider   ---   getImage
**
******************************************************/
wxImage ImageProvider::getImage( const BitmapId &id, const IconSize &size )
{
	wxString s;
	switch( size )
	{
		case IconSize::Small:
			s << "16x16/";
		break;
		case IconSize::Medium:
			s << "24x24/";
		break;
		case IconSize::Large:
		default:
			s << "32x32/";
		break;
	}

	const auto &filename = bitmapFilenames.find( id );
	if ( filename != bitmapFilenames.end() )
	{
		s << filename->second;
		return getImage( s, 0.0 );
	}
	else return errorImage;
}

/*****************************************************
**
**   ImageProvider   ---   getBitmap
**
******************************************************/
wxBitmap ImageProvider::getBitmap( const BitmapId &id, const IconSize &size, const double rotateHue )
{
	wxImage image = getImage( id, size );
	if ( rotateHue )
	{
		image.RotateHue( rotateHue );
	}
	return wxBitmap( image );
}

/*****************************************************
**
**   ImageProvider   ---   getBitmapBundle
**
******************************************************/
wxBitmapBundle ImageProvider::getBitmapBundle( const BitmapId &id )
{
	wxVector<wxBitmap> bitmaps;
	for( const IconSize size : Enum<IconSize>())
	{
		bitmaps.push_back( wxBitmap( getImage( id, size )));
	}
	return wxBitmapBundle::FromBitmaps( bitmaps );
}

/*****************************************************
**
**   ImageProvider   ---   getDasaTreeBitmap
**
******************************************************/
wxBitmap ImageProvider::getDasaTreeBitmap( const DasaLevel &level, const bool &expanded )
{
	constexpr IconSize size = IconSize::Large;
	BitmapId id = BitmapId::Error;
	double rotateHue = 0.0;

	switch( level )
	{
		case DasaLevel::TreeRoot:
			id = BitmapId::Dasa;
		break;
		case DasaLevel::Folder:
			id = expanded ? BitmapId::FolderExpanded : BitmapId::Folder;
		break;
		default:
		{
			id = expanded ? BitmapId::DasatreeExpanded : BitmapId::Dasatree;
			switch( level )
			{
				case DasaLevel::Root: // -1
					rotateHue = 0; // red
				break;
				case DasaLevel::Mahadasa: // 0
					rotateHue = 0.21; // yellow
				break;
				case DasaLevel::Antardasa: // 1
					rotateHue = 0.4;  // green
				break;
				case DasaLevel::Pratyantardasa:
					rotateHue = 0.6; // blue
				break;
				default:
					rotateHue = 0.8; // lilac
				break;
			}
		}
		break;
	}
#ifdef DEBUG_IMAGEPROVIDER
	debuglog << "DasaTree::getBitmap level " << level << " expanded " << std::boolalpha << expanded << " id " << (int)id
		<< " rotateHue " << rotateHue;;
#endif
	return getBitmap( id, size, rotateHue );
}

/*****************************************************
**
**   ImageProvider   ---   getBitmap
**
******************************************************/
wxBitmap ImageProvider::getBitmap( const wxString &filename, const double angle )
{
	return wxBitmap( getImage( filename, angle ));
}

/*****************************************************
**
**   ImageProvider   ---   getIcon
**
******************************************************/
wxIcon ImageProvider::getIcon( const BitmapId &id )
{
	wxIcon icon;
	icon.CopyFromBitmap( getBitmap( id, IconSize::Large ));
	return icon;
}

/*****************************************************
**
**   ImageProvider   ---   loadImage
**
******************************************************/
wxImage ImageProvider::loadImage( const wxString &filename, const double angle )
{
	const wxString fullname = FileConfig::get()->getResourceDir( ConfigResource::Pic ) + filename;
#ifdef DEBUG_IMAGEPROVIDER
	debuglog << "ImageProvider::loadImage " << filename << " angle " << angle << " fullname " << fullname;
#endif

	if ( ! wxFileName::FileExists( fullname ))
	{
		errorlog << "ImageProvider::loadImage: image file " << fullname << " not found";
		return errorImage;
	}
	if ( ! wxFileName::IsFileReadable( fullname ))
	{
		errorlog << "ImageProvider::loadImage: image file " << fullname << " not readable";
		return errorImage;
	}

	wxImage image( fullname );
	if ( angle )
	{
		image.RotateHue( angle / 360.0 );
	}
	return image;
}

