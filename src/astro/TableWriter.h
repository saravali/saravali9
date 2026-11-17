/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/TableWriter.h
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

#ifndef _TABLEWRITER_H_
#define _TABLEWRITER_H_

#include "constants.h"
#include "Viewpoint.h"

#include <list>
#include <variant>

class PlanetBalanceExpert;
class Table;

/*************************************************//**
*
*   CLASS   ---   TableWriter
*
******************************************************/
class TableWriter
{
public:
	TableWriter( ContextProps_ptr, Horoscope_ptr );

	Table *createPlanetListTable();

	/**
	* PlanetPropertyConfig see Viewpoint.h
	*/
	void writePlanetListTable( Planets, const PlanetConfig&, PlanetPropertyConfig, Table*,
		const uint colOffset = 0, const int rowOffset = 0, const bool doPrintTableHeader = true );

	// Wrapper
	Table *createPlanetListTable( const Planets&, const PlanetConfig&, const PlanetPropertyConfig& );

	/**
	* Writes base data attributes (given by list of BasedataAttribute) into a given table.
	* the column offset (i.e. the index of the first column) and row offset can be changed.
	*
	*/
	void writeBaseDataAttributes( Table*, std::list<BasedataAttribute>, const PlanetConfig&,
		const uint colOffset = 0, const int rowOffset = 0, const bool doPrintTableHeader = true );

	Table *createBaseDataTable( const std::list<BasedataAttribute>& );

private:

	uint getGroupColumnCount( const Planets&, const ColumnGroup& );

	uint getPlanetListColumnCount( const Planets&, const PlanetPropertyConfig& );

	void writeCustomDataEntry( Table*, const BasedataAttribute&, const uint &colOffset, const uint &rowOffset );

	void addBalanceItem( PlanetBalanceExpert&, Table*, const uint &col, const uint &row,
		const std::variant<Element,SignQuality,ChartSide>& );

	int kpdepth;

	ContextProps_ptr props;

	Horoscope_ptr h;

	Viewpoint viewpoint;

};
 
#endif // _TABLEWRITER_H_

