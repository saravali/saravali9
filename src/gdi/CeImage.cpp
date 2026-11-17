/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/CeImage.cpp
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

#include "ChartElementImpl.h"
#include "HybridChart.h"
#include "ImageProvider.h"
#include "logger.h"
#include "Painter.h"

#include <wx/filename.h>

/*************************************************//**
*
*   Chart element containing a (background) image within the chart.
*
******************************************************/
class CeImage : public ChartElement<CgInlayImage>
{
public:

	CeImage( HybridChart *chart, CgInlayImage *image )
		: ChartElement( chart, image ) {}

	/*****************************************************
	**
	**   CeImage   ---   paint
	**
	******************************************************/
	void paint( Painter *painter, const MRect& /*refreshRect*/ )
	{
		//debuglog << "CeImage::paint";
		const MComplex center = chart->getCenter();
		const MComplex xymax = chart->getXyMax();

		CgInlayImage *inlay = getCg();
		assert( inlay );
		if ( ! inlay->show ) return;
		if ( inlay->filename.empty())
		{
			errorlog << "cannot draw inlay image: file empty";
			return;
		}
		debuglog << "CeImage::paint image filename " << inlay->filename;

		setInheritedStyle( painter );
		paintDisc( painter, *inlay );

		ImageProvider *ip = ImageProvider::get();
		wxImage image = ip->getImage( inlay->filename );
		if ( ! image.IsOk())
		{
			warnlog << "cannot create inlay image from file " << inlay->filename;
			return;
		}

		const double height = xymax.imag() * inlay->radius / 50.0;
		const double xnew = image.GetWidth() * height / image.GetHeight();
		painter->drawBitmap( image.Scale( xnew, height ), center.real() - .5 * xnew, center.imag() - .5 * height );
	}

};

/*****************************************************
**
**   Grid element factory
**
******************************************************/
ChartElementBase *ChartElementFactory::createImage( CgElement *cge ) const
{
	CgInlayImage *inlay = dynamic_cast<CgInlayImage*>( cge );
	assert( inlay );
	return new CeImage( chart, inlay );
}

