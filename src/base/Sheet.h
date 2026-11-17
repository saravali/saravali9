/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/Sheet.h
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
#ifndef _SHEET_H_
#define _SHEET_H_

#include "ConfigListLoader.h"
#include "constants.h"
#include "logger.h"
#include "MDocument.h"
#include "Viewpoint.h"

/*************************************************//**
*
*  This enum classifies the elements of a printout sheet.
*
******************************************************/
enum class SheetItemType
{
	// json:enum
	None = -1,
	Text, /**< Text item can be header or normal text. See SheetItemText */
	Table, /**< Creates a table with base data or planetary postions. See SheetItemTableDefinition and ColumnGroupDefinition */

	Chart, /**< Only internally used. Use WesternChart etc. */
	WesternChart, /**< Similar to western chart in the GUI application */
	UranianChart,  /**< Similar to Uranian chart in the GUI application */
	VargaChart, /**< Similar to Varga chart in the GUI application */
	SarvatobhadraChart, /**< Similar to SBC in the GUI application */
	VargaChartGrid,	/**< See SheetItemVargaChartGrid */
	DasaSummary,  /**< See SheetItemDasa */
	SPTable, /**< See SheetItemSPTable */

	Aspectarium, /**< Creates a standalone Western Aspectarium table. Use ColumnGroup::WesternAspectarium if you want to have it integrated into planet table */
	PageBreak,  /**< Forced page break. See SheetItemPageBreak */

	First = Text,
	Last = PageBreak
};

/*************************************************//**
*
*  Base class for a sheet element.
*
******************************************************/
struct SheetItem
{
	SheetItem( const SheetItemType&, const Viewpoint = Viewpoint::None );

	virtual ~SheetItem() {}

	virtual SheetItem *clone() { return new SheetItem( *this ); }

	SheetItemType type;

	bool expand = false;

	Viewpoint viewpoint;

	TextFormat maxTextFormat = TextFormat::Long;

	/**
	* Degree-like numbers can shortened. 
	* "Minute" shows degrees like MM:SS
	*/
	DegPrecision maxDegPrecision = DegPrecision::Minute;
};

/*************************************************//**
*
*  Text item with e.g. header or a message
*
******************************************************/
struct SheetItemText : public SheetItem
{
	SheetItemText( const wxString contents = "", TextItemSubType subType = TextItemSubType::Header1 )
		: SheetItem( SheetItemType::Text ),
		contents( contents ),
		subType( subType )
		{}

	virtual SheetItemText *clone() { return new SheetItemText( *this ); }

	wxString contents;

	TextItemSubType subType = TextItemSubType::None;

};

/*************************************************//**
*
* Encapsulation of a dasa report in a sheet.
*
* Supported report type are: short (text summary), compact (table) and long (comprehensive text).
*
******************************************************/
struct SheetItemDasa : public SheetItem
{
	SheetItemDasa(
		const DasaReportType &reportType = DasaReportType::Short,
		const DasaId dasaId = DasaId::Vimsottari,
		const Viewpoint viewpoint = Viewpoint::Vedic )
		: SheetItem( SheetItemType::DasaSummary, viewpoint ), reportType( reportType ), dasaId( dasaId ) {}

	virtual SheetItem *clone() override { return new SheetItemDasa( *this ); }

	DasaReportType reportType = DasaReportType::Compact;

	DasaId dasaId = DasaId::Vimsottari;
};

/*************************************************//**
*
* 
*
* 
*
******************************************************/
struct SheetItemSPTable : public SheetItem
{
	SheetItemSPTable(
		const TermId &termId = TermId::Midpoint,
		const int division = 1,
		const Viewpoint viewpoint = Viewpoint::Uranian )
		: SheetItem( SheetItemType::SPTable, viewpoint ), termId( termId ), division( division ) {}

	virtual SheetItem *clone() override { return new SheetItemSPTable( *this ); }

	TermId termId = TermId::Midpoint;

	int division = 1;
};

/*************************************************//**
*
*  Encapsulation of a graphical chart in a sheet
*
******************************************************/
struct SheetItemChart : public SheetItem
{
	SheetItemChart( const WidgetCategory cat = WidgetCategory::None, const Viewpoint viewpoint = Viewpoint::None )
	: SheetItem( SheetItemType::Chart, viewpoint ), category( cat ) {}

	virtual SheetItem *clone() override { return new SheetItemChart( *this ); }

	WidgetCategory category = WidgetCategory::None;

	Varga varga = Varga::Rasi;

	// = 360 deg
	int division = 1;

	// width in percent
	double width = 100.0; 
};

/*************************************************//**
*
* Encapsulates a group of Vedic charts in a sheet.
*
* Number of columns can be configured. There is always one row.
*
******************************************************/
struct SheetItemVargaChartGrid : public SheetItem
{
	SheetItemVargaChartGrid( const Viewpoint viewpoint = Viewpoint::None )
		: SheetItem( SheetItemType::VargaChartGrid, viewpoint ) {}

	virtual SheetItem *clone() override { return new SheetItemVargaChartGrid( *this ); }

	uint cols = 2;

	std::list<Varga> vargas;
};

/*************************************************//**
*
* 
*
******************************************************/
struct ColumnGroupDefinition : public Jsonable
{
	uint getColumnCount() const;

	uint getRowCount() const;

	// json:start
	bool autoHeader = true;

	wxString commonHeader;

	//std::list<PlanetId> planets;
	Planets planets;

	std::list<ColumnGroup> columnGroups;

	std::list<BasedataAttribute> basedata;

	// json:end

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* 
*
******************************************************/
struct SheetItemTableDefinition : public SheetItem
{
	SheetItemTableDefinition() : SheetItem( SheetItemType::Table ) {}

	virtual SheetItem *clone() override { return new SheetItemTableDefinition( *this ); }

	std::list<ColumnGroupDefinition> groups;

	wxString header;

	bool showVerticalBorders = true;

	bool showHorizontalBorders = true;

	bool showEmptyColumns = true;

};

/*************************************************//**
*
*  Forced page break. Page orientation (Portrait/Landscape) of the next and following pages can be set.
*  Param PageOrientation::Indefinite keeps the orientation of the previous page.
*
*
******************************************************/
struct SheetItemPageBreak : public SheetItem
{
	SheetItemPageBreak() : SheetItem( SheetItemType::PageBreak ) {}

	virtual SheetItem *clone() override { return new SheetItemPageBreak( *this ); }

	PageOrientation orientation = PageOrientation::Portrait;
};

/*************************************************//**
*
* 
*
******************************************************/
struct SheetSpecSummary
{
	uint itemCount = 0;

	uint basedataCount = 0;

	uint columnCount = 0;

	uint chartCount = 0;

	Planets planets;
};

/*************************************************//**
*
* This class encapsulates the full specification of a sheet, used for printout.
*
******************************************************/
struct SheetSpec : public Jsonable
{
	// json:noimpl
	SheetSpec() {}

	SheetSpec( const SheetSpec& );

	~SheetSpec();

	/**
		See SheetSpecSummary
	*/
	SheetSpecSummary getSummary() const;

	/**
		Name shown in the Printout configuration dialog.
		Also used for inheritance of configs (see SheetSpec).
	*/
	wxString name;

	/**
		Desciption shown in the Printout configuration dialog.
	*/
	wxString description;

	Viewpoint viewpoint = Viewpoint::None;

	/**
		Page break must be manually configured if autoPageBreak is false.
		Otherwise (true) page break is inserted automatically when the items exceed the maximum page height.
	*/
	bool autoPageBreak = false;

#ifdef CPP26
	[[=skip_json]]
	std::list<SheetItem*> items;
#else
	std::list<SheetItem*> items;
#endif

	// these are dummies because fromJsonValue in SheetSpecLoader is handwritten
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override {}
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* Reads sheet configs, used for printouts
*
******************************************************/
class SheetSpecLoader : public ConfigLoader<SheetSpec>, public Singleton<SheetSpecLoader>
{
	friend class Singleton<SheetSpecLoader>;

private:

	SheetSpecLoader() : ConfigLoader<SheetSpec>( ConfigResource::Sheet ) {}

	virtual void loadDefaultConfigs() override;

#ifdef CPP26
	virtual void read_single_node( SheetSpec&, const Json::Value&, JsonParser& ) override;
#else

	virtual void loadSingleConfig( const Json::Value&, JsonHandler& ) override;

	SheetItem *createItem( const Json::Value&, JsonHandler& );

	void addItemList( std::list<SheetItem*>&, const Json::Value&, JsonHandler& );
#endif

};

#endif // _SHEET_H_

