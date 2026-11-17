/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/ShadbalaDiagrams.h
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

#ifndef _SHADBALADIAGRAMS_H_
#define _SHADBALADIAGRAMS_H_


#include "BarDiagram.h"

class ShadbalaExpert;
enum class BalaId;

/*************************************************//**
*
*  This class represents a Shadvarga overview diagram.
*
******************************************************/
class ShadbalaDiagram : public BarDiagram
{
public:
	ShadbalaDiagram( ContextProps_ptr props, ShadbalaExpert *expert, const BalaId &type,
		const BarDiagramConfig *barconfig = nullptr );
	
	virtual void OnDataChanged() override;

private:

	ShadbalaExpert *expert;

	const BalaId type;
};

/*************************************************//**
*
*  This class contains the Saptavargaja overview diagram for Shadbala.
*
******************************************************/
class SaptavargajaDiagram : public BarDiagram
{
public:
	SaptavargajaDiagram( ContextProps_ptr props, ShadbalaExpert *expert, const int &vargaId,
		const BarDiagramConfig *barconfig = nullptr );
	
	virtual void OnDataChanged() override;

private:

	ShadbalaExpert *expert;

	const int vargaId;
};

#endif // _SHADBALADIAGRAMS_H_

