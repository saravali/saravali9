/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/FieldWriter.h
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

#ifndef FIELDWRITER_H_
#define FIELDWRITER_H_

#include "constants.h"
#include "mstring.h"

class AshtakavargaExpert;
class CeFieldGrid;

/*************************************************//**
*
*  
*
******************************************************/
class FieldWriter
{
public:

	FieldWriter( CeFieldGrid *grid ) : grid( grid ) {}

	virtual ~FieldWriter() {}

	virtual void writePlanetFields() = 0;

	virtual MString getCenterString() = 0;

	virtual int getPlanetField( const PlanetId&, const HoroscopeRole& ) const { return -1; }

	bool getPlanetRetro( const PlanetId&, const HoroscopeRole& );

protected:

	CeFieldGrid *grid;

};

/*************************************************//**
*
*  
*
******************************************************/
class FieldWriterFactory
{
public:

	FieldWriter *createVargaFieldWriter( CeFieldGrid* );

	FieldWriter *createSbcFieldWriter( CeFieldGrid* );

	FieldWriter *createAshtakavargaFieldWriter( CeFieldGrid*,
		const AvType &avtype, AshtakavargaExpert *expert, const PlanetId &ob );

};

#endif

