/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/AshtakavargaDiagrams.cpp
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

#include "AshtakavargaDiagrams.h"

#include "Ashtakavarga.h"

#include <variant>

/*****************************************************
**
**   AvPindaDiagram   ---   Constructor
**
******************************************************/
AvPindaDiagram::AvPindaDiagram( ContextProps_ptr props, AshtakavargaExpert *expert, const PindaType &type,
	const BarDiagramConfig *barconfig  )
	: BarDiagram( props, expert->getPindaName( type ), barconfig ),
	expert( expert ),
	type( type )
{
	for( PlanetId p : PlanetHelper::createAshtakaVargaPlanets())
  {
		addItem( p );
	}
}

/*****************************************************
**
**   AvPindaDiagram   ---   OnDataChanged
**
******************************************************/
void AvPindaDiagram::OnDataChanged()
{
	for( auto &item : items )
	{
		assert( std::holds_alternative<PlanetId>( item.object ));
		item.value = expert->getPinda( type, std::get<PlanetId>( item.object ) );
	}
}

/*****************************************************
**
**   AvRasiDiagram   ---   Constructor
**
******************************************************/
AvRasiDiagram::AvRasiDiagram( ContextProps_ptr props,
	AshtakavargaExpert *expert,
	const AvType &type,
	const BarDiagramConfig *barconfig )
	: BarDiagram( props, expert->getAvTypeName( type ), barconfig ),
	expert( expert ),
	type( type )
{
	for ( Sign rasi : Enum<Sign>() )
  {
		addItem( rasi );
	}
}

/*****************************************************
**
**   AvRasiDiagram   ---   OnDataChanged
**
******************************************************/
void AvRasiDiagram::OnDataChanged()
{
	for( auto &item : items )
	{
		assert( std::holds_alternative<Sign>( item.object ));
		item.value = expert->getSignSarva( type, std::get<Sign>( item.object ) );
	}
}

