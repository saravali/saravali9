/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/GraphicalDasa.h
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

#ifndef _GRAPHICALDASA_H_
#define _GRAPHICALDASA_H_

#include "Dasa.h"
#include "MDocument.h"

#include <list>

struct GraphicalDasa;
typedef std::shared_ptr<GraphicalDasa> GraphicalDasa_ptr;

/*************************************************//**
*
* 
*
******************************************************/
struct GraphicalDasa : public DocItemText
{
public:

	GraphicalDasa( Dasa_ptr d ) : DocItemText( Align::West ), dasa( d ) {}

  GraphicalDasa( const DasaLevel &level, const wxString &s )
    : DocItemText( s, MRect(), Align::West )
	{
		dasa = Dasa_ptr( new Dasa( nullptr, nullptr, level ));
	}

	DasaExpert_ptr getExpert()
	{
		assert( dasa );
		return dasa->getExpert();
	}

	DasaLevel getLevel()
	{
		assert( dasa.get());
		return dasa->getLevel();
	}

	Dasa_ptr dasa;

	bool expanded = false;

	std::list<GraphicalDasa_ptr> children;
};


#endif // _GRAPHICALDASA_H_

