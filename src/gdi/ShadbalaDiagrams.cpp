/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/ShadbalaDiagrams.cpp
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

#include "ShadbalaDiagrams.h"

#include "Lang.h"
#include "Shadbala.h"

#include <variant>

/*****************************************************
**
**   ShadbalaDiagram   ---   Constructor
**
******************************************************/
ShadbalaDiagram::ShadbalaDiagram( ContextProps_ptr props, ShadbalaExpert *expert,
	const BalaId &type, const BarDiagramConfig *barconfig )
	: BarDiagram( props, expert->getBalaIdTitle( type ), barconfig ),
	expert( expert ),
	type( type )
{
	for( const PlanetId &p : PlanetHelper::createVedicOnlyPlanets())
  {
		addItem( p );
	}
}
	
/*****************************************************
**
**   ShadbalaDiagram   ---   OnDataChanged
**
******************************************************/
void ShadbalaDiagram::OnDataChanged()
{
	for( auto &item : items )
	{
		assert( std::holds_alternative<PlanetId>( item.object ));
		item.value = expert->getBala( type, std::get<PlanetId>( item.object ));
	}
}

/*****************************************************
**
**   SaptavargajaDiagram   ---   Constructor
**
******************************************************/
SaptavargajaDiagram::SaptavargajaDiagram( ContextProps_ptr props, ShadbalaExpert *expert,
	const int &vargaId, const BarDiagramConfig *barconfig )
	: BarDiagram( props, Lang().getVargaName( expert->sapta_vargas[ vargaId ] ), barconfig ),
	expert( expert ),
	vargaId( vargaId )
{
	for( const PlanetId &p : PlanetHelper::createVedicOnlyPlanets())
  {
		addItem( p );
	}
}
	
/*****************************************************
**
**   SaptavargajaDiagram   ---   OnDataChanged
**
******************************************************/
void SaptavargajaDiagram::OnDataChanged()
{
	for( auto &item : items )
	{
		assert( std::holds_alternative<PlanetId>( item.object ));
		item.value = expert->getSaptavargajaBala( std::get<PlanetId>( item.object ), vargaId );
	}
}

