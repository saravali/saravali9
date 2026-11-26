// generated

#include "enumserializer.h"
#include "JsonConverter.h"
#include "json/json.h"

#include "constants.h"
#include "logger.h"
#include "JsonConverter.h"
#include "JsonHandler.h"
#include "YdlExpression.h"
#include "Planet.h"
#include "mstring.h"
#include "Commands.h"
#include "Viewpoint.h"
#include "gdibase.h"
#include "geometry.h"
#include "AspectConfig.h"
#include "datelocation.h"
#include "astroconfig.h"
#include "ChartConfig.h"
#include "ColorConfig.h"
#include "ConfigListItems.h"
#include "PrintoutConfig.h"
#include "MDocument.h"
#include "Sheet.h"
#include "gdiconfig.h"
#include "ConfigListItems.h"


// IN class PlanetConfig defined in src/base/Viewpoint.h line 226
void PlanetConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "items" ] )
	{
		extractFromJson( v[ "items" ], items, jh ); 
	}
	if ( v[ "arabicParts" ] )
	{
		extractFromJson( v[ "arabicParts" ], arabicParts, jh ); 
	}
	if ( v[ "textFormat" ] )
	{
		extractFromJson( v[ "textFormat" ], textFormat, jh ); 
	}
	if ( v[ "nodeNames" ] )
	{
		extractFromJson( v[ "nodeNames" ], nodeNames, jh ); 
	}
	if ( v[ "useVedicNames" ] )
	{
		extractFromJson( v[ "useVedicNames" ], useVedicNames, jh ); 
	}
	if ( v[ "uranusSymbol" ] )
	{
		extractFromJson( v[ "uranusSymbol" ], uranusSymbol, jh ); 
	}
	if ( v[ "plutoSymbol" ] )
	{
		extractFromJson( v[ "plutoSymbol" ], plutoSymbol, jh ); 
	}
	if ( v[ "viewpoint" ] )
	{
		extractFromJson( v[ "viewpoint" ], viewpoint, jh ); 
	}
}


// OUT class PlanetConfig defined in src/base/Viewpoint.h line 226
Json::Value PlanetConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "items" ] = writeToJson( items, jh );
	v[ "arabicParts" ] = writeToJson( arabicParts, jh );
	v[ "textFormat" ] = writeToJson( textFormat, jh );
	v[ "nodeNames" ] = writeToJson( nodeNames, jh );
	v[ "useVedicNames" ] = writeToJson( useVedicNames, jh );
	v[ "uranusSymbol" ] = writeToJson( uranusSymbol, jh );
	v[ "plutoSymbol" ] = writeToJson( plutoSymbol, jh );
	v[ "viewpoint" ] = writeToJson( viewpoint, jh );
	return v;
}


// IN class PlanetPropertyConfig defined in src/base/Viewpoint.h line 260
void PlanetPropertyConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "columnGroups" ] )
	{
		extractFromJson( v[ "columnGroups" ], columnGroups, jh ); 
	}
	if ( v[ "textFormat" ] )
	{
		extractFromJson( v[ "textFormat" ], textFormat, jh ); 
	}
	if ( v[ "vedicSignNames" ] )
	{
		extractFromJson( v[ "vedicSignNames" ], vedicSignNames, jh ); 
	}
	if ( v[ "positionStyle" ] )
	{
		extractFromJson( v[ "positionStyle" ], positionStyle, jh ); 
	}
	if ( v[ "precision" ] )
	{
		extractFromJson( v[ "precision" ], precision, jh ); 
	}
	if ( v[ "retroFormat" ] )
	{
		extractFromJson( v[ "retroFormat" ], retroFormat, jh ); 
	}
	if ( v[ "nakshatraTextFormat" ] )
	{
		extractFromJson( v[ "nakshatraTextFormat" ], nakshatraTextFormat, jh ); 
	}
	if ( v[ "translateSignNames" ] )
	{
		extractFromJson( v[ "translateSignNames" ], translateSignNames, jh ); 
	}
	if ( v[ "capricornSymbol" ] )
	{
		extractFromJson( v[ "capricornSymbol" ], capricornSymbol, jh ); 
	}
}


// OUT class PlanetPropertyConfig defined in src/base/Viewpoint.h line 260
Json::Value PlanetPropertyConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "columnGroups" ] = writeToJson( columnGroups, jh );
	v[ "textFormat" ] = writeToJson( textFormat, jh );
	v[ "vedicSignNames" ] = writeToJson( vedicSignNames, jh );
	v[ "positionStyle" ] = writeToJson( positionStyle, jh );
	v[ "precision" ] = writeToJson( precision, jh );
	v[ "retroFormat" ] = writeToJson( retroFormat, jh );
	v[ "nakshatraTextFormat" ] = writeToJson( nakshatraTextFormat, jh );
	v[ "translateSignNames" ] = writeToJson( translateSignNames, jh );
	v[ "capricornSymbol" ] = writeToJson( capricornSymbol, jh );
	return v;
}


// IN class CalculationConfig defined in src/base/Viewpoint.h line 295
void CalculationConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "lunarNodeMode" ] )
	{
		extractFromJson( v[ "lunarNodeMode" ], lunarNodeMode, jh ); 
	}
	if ( v[ "houseSystem" ] )
	{
		extractFromJson( v[ "houseSystem" ], houseSystem, jh ); 
	}
	if ( v[ "houseStart" ] )
	{
		extractFromJson( v[ "houseStart" ], houseStart, jh ); 
	}
	if ( v[ "ayanamsa" ] )
	{
		extractFromJson( v[ "ayanamsa" ], ayanamsa, jh ); 
	}
	if ( v[ "viewpoint" ] )
	{
		extractFromJson( v[ "viewpoint" ], viewpoint, jh ); 
	}
}


// OUT class CalculationConfig defined in src/base/Viewpoint.h line 295
Json::Value CalculationConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "lunarNodeMode" ] = writeToJson( lunarNodeMode, jh );
	v[ "houseSystem" ] = writeToJson( houseSystem, jh );
	v[ "houseStart" ] = writeToJson( houseStart, jh );
	v[ "ayanamsa" ] = writeToJson( ayanamsa, jh );
	v[ "viewpoint" ] = writeToJson( viewpoint, jh );
	return v;
}


// IN class BasedataConfig defined in src/base/Viewpoint.h line 325
void BasedataConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "showBasedataInPlanetListWidget" ] )
	{
		extractFromJson( v[ "showBasedataInPlanetListWidget" ], showBasedataInPlanetListWidget, jh ); 
	}
	if ( v[ "basedataItems" ] )
	{
		extractFromJson( v[ "basedataItems" ], basedataItems, jh ); 
	}
	if ( v[ "viewpoint" ] )
	{
		extractFromJson( v[ "viewpoint" ], viewpoint, jh ); 
	}
}


// OUT class BasedataConfig defined in src/base/Viewpoint.h line 325
Json::Value BasedataConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "showBasedataInPlanetListWidget" ] = writeToJson( showBasedataInPlanetListWidget, jh );
	v[ "basedataItems" ] = writeToJson( basedataItems, jh );
	v[ "viewpoint" ] = writeToJson( viewpoint, jh );
	return v;
}


// IN class ViewpointConfig defined in src/base/Viewpoint.h line 353
void ViewpointConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "planets" ] )
	{
		extractFromJson( v[ "planets" ], planets, jh ); 
	}
	if ( v[ "planetProps" ] )
	{
		extractFromJson( v[ "planetProps" ], planetProps, jh ); 
	}
	if ( v[ "calculation" ] )
	{
		extractFromJson( v[ "calculation" ], calculation, jh ); 
	}
	if ( v[ "basedata" ] )
	{
		extractFromJson( v[ "basedata" ], basedata, jh ); 
	}
}


// OUT class ViewpointConfig defined in src/base/Viewpoint.h line 353
Json::Value ViewpointConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "planets" ] = writeToJson( planets, jh );
	v[ "planetProps" ] = writeToJson( planetProps, jh );
	v[ "calculation" ] = writeToJson( calculation, jh );
	v[ "basedata" ] = writeToJson( basedata, jh );
	return v;
}


// IN class MRect defined in src/base/gdibase.h line 58
void MRect::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "x" ] )
	{
		extractFromJson( v[ "x" ], x, jh ); 
	}
	if ( v[ "y" ] )
	{
		extractFromJson( v[ "y" ], y, jh ); 
	}
	if ( v[ "width" ] )
	{
		extractFromJson( v[ "width" ], width, jh ); 
	}
	if ( v[ "height" ] )
	{
		extractFromJson( v[ "height" ], height, jh ); 
	}
}


// OUT class MRect defined in src/base/gdibase.h line 58
Json::Value MRect::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "x" ] = writeToJson( x, jh );
	v[ "y" ] = writeToJson( y, jh );
	v[ "width" ] = writeToJson( width, jh );
	v[ "height" ] = writeToJson( height, jh );
	return v;
}


// STREAM output class MRect defined in src/base/gdibase.h
mostream& operator <<( mostream &o, const MRect &a )
{
	o << "x={" << a.x << "} "
    << "y={" << a.y << "} "
    << "width={" << a.width << "} "
    << "height={" << a.height << "} ";
	return o;
}


// IN class CgElement defined in src/base/geometry.h line 78
void CgElement::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "radius" ] )
	{
		extractFromJson( v[ "radius" ], radius, jh ); 
	}
	if ( v[ "show" ] )
	{
		extractFromJson( v[ "show" ], show, jh ); 
	}
	if ( v[ "pen" ] )
	{
		extractFromJson( v[ "pen" ], pen, jh ); 
	}
	if ( v[ "brush" ] )
	{
		extractFromJson( v[ "brush" ], brush, jh ); 
	}
	if ( v[ "textColor" ] )
	{
		extractFromJson( v[ "textColor" ], textColor, jh ); 
	}
}


// OUT class CgElement defined in src/base/geometry.h line 78
Json::Value CgElement::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "radius" ] = writeToJson( radius, jh );
	v[ "show" ] = writeToJson( show, jh );
	v[ "pen" ] = writeToJson( pen, jh );
	v[ "brush" ] = writeToJson( brush, jh );
	v[ "textColor" ] = writeToJson( textColor, jh );
	return v;
}


// IN class CgDisk defined in src/base/geometry.h line 121
void CgDisk::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "markerLength" ] )
	{
		extractFromJson( v[ "markerLength" ], markerLength, jh ); 
	}
	if ( v[ "markerPen" ] )
	{
		extractFromJson( v[ "markerPen" ], markerPen, jh ); 
	}
	if ( v[ "geometryType" ] )
	{
		extractFromJson( v[ "geometryType" ], geometryType, jh ); 
	}
	if ( v[ "radius" ] )
	{
		extractFromJson( v[ "radius" ], radius, jh ); 
	}
	if ( v[ "show" ] )
	{
		extractFromJson( v[ "show" ], show, jh ); 
	}
	if ( v[ "pen" ] )
	{
		extractFromJson( v[ "pen" ], pen, jh ); 
	}
	if ( v[ "brush" ] )
	{
		extractFromJson( v[ "brush" ], brush, jh ); 
	}
	if ( v[ "textColor" ] )
	{
		extractFromJson( v[ "textColor" ], textColor, jh ); 
	}
}


// OUT class CgDisk defined in src/base/geometry.h line 121
Json::Value CgDisk::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "markerLength" ] = writeToJson( markerLength, jh );
	v[ "markerPen" ] = writeToJson( markerPen, jh );
	v[ "geometryType" ] = writeToJson( geometryType, jh );
	v[ "radius" ] = writeToJson( radius, jh );
	v[ "show" ] = writeToJson( show, jh );
	v[ "pen" ] = writeToJson( pen, jh );
	v[ "brush" ] = writeToJson( brush, jh );
	v[ "textColor" ] = writeToJson( textColor, jh );
	return v;
}


// IN class CgError defined in src/base/geometry.h line 158
void CgError::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "message" ] )
	{
		extractFromJson( v[ "message" ], message, jh ); 
	}
	if ( v[ "markerLength" ] )
	{
		extractFromJson( v[ "markerLength" ], markerLength, jh ); 
	}
	if ( v[ "markerPen" ] )
	{
		extractFromJson( v[ "markerPen" ], markerPen, jh ); 
	}
	if ( v[ "geometryType" ] )
	{
		extractFromJson( v[ "geometryType" ], geometryType, jh ); 
	}
	if ( v[ "radius" ] )
	{
		extractFromJson( v[ "radius" ], radius, jh ); 
	}
	if ( v[ "show" ] )
	{
		extractFromJson( v[ "show" ], show, jh ); 
	}
	if ( v[ "pen" ] )
	{
		extractFromJson( v[ "pen" ], pen, jh ); 
	}
	if ( v[ "brush" ] )
	{
		extractFromJson( v[ "brush" ], brush, jh ); 
	}
	if ( v[ "textColor" ] )
	{
		extractFromJson( v[ "textColor" ], textColor, jh ); 
	}
}


// OUT class CgError defined in src/base/geometry.h line 158
Json::Value CgError::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "message" ] = writeToJson( message, jh );
	v[ "markerLength" ] = writeToJson( markerLength, jh );
	v[ "markerPen" ] = writeToJson( markerPen, jh );
	v[ "geometryType" ] = writeToJson( geometryType, jh );
	v[ "radius" ] = writeToJson( radius, jh );
	v[ "show" ] = writeToJson( show, jh );
	v[ "pen" ] = writeToJson( pen, jh );
	v[ "brush" ] = writeToJson( brush, jh );
	v[ "textColor" ] = writeToJson( textColor, jh );
	return v;
}


// IN class CgInlayImage defined in src/base/geometry.h line 183
void CgInlayImage::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "align" ] )
	{
		extractFromJson( v[ "align" ], align, jh ); 
	}
	if ( v[ "filename" ] )
	{
		extractFromJson( v[ "filename" ], filename, jh ); 
	}
	if ( v[ "markerLength" ] )
	{
		extractFromJson( v[ "markerLength" ], markerLength, jh ); 
	}
	if ( v[ "markerPen" ] )
	{
		extractFromJson( v[ "markerPen" ], markerPen, jh ); 
	}
	if ( v[ "geometryType" ] )
	{
		extractFromJson( v[ "geometryType" ], geometryType, jh ); 
	}
	if ( v[ "radius" ] )
	{
		extractFromJson( v[ "radius" ], radius, jh ); 
	}
	if ( v[ "show" ] )
	{
		extractFromJson( v[ "show" ], show, jh ); 
	}
	if ( v[ "pen" ] )
	{
		extractFromJson( v[ "pen" ], pen, jh ); 
	}
	if ( v[ "brush" ] )
	{
		extractFromJson( v[ "brush" ], brush, jh ); 
	}
	if ( v[ "textColor" ] )
	{
		extractFromJson( v[ "textColor" ], textColor, jh ); 
	}
}


// OUT class CgInlayImage defined in src/base/geometry.h line 183
Json::Value CgInlayImage::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "align" ] = writeToJson( align, jh );
	v[ "filename" ] = writeToJson( filename, jh );
	v[ "markerLength" ] = writeToJson( markerLength, jh );
	v[ "markerPen" ] = writeToJson( markerPen, jh );
	v[ "geometryType" ] = writeToJson( geometryType, jh );
	v[ "radius" ] = writeToJson( radius, jh );
	v[ "show" ] = writeToJson( show, jh );
	v[ "pen" ] = writeToJson( pen, jh );
	v[ "brush" ] = writeToJson( brush, jh );
	v[ "textColor" ] = writeToJson( textColor, jh );
	return v;
}


// IN class CgVedicChartGrid defined in src/base/geometry.h line 208
void CgVedicChartGrid::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "straightFrame" ] )
	{
		extractFromJson( v[ "straightFrame" ], straightFrame, jh ); 
	}
	if ( v[ "showFieldNames" ] )
	{
		extractFromJson( v[ "showFieldNames" ], showFieldNames, jh ); 
	}
	if ( v[ "markerLength" ] )
	{
		extractFromJson( v[ "markerLength" ], markerLength, jh ); 
	}
	if ( v[ "markerPen" ] )
	{
		extractFromJson( v[ "markerPen" ], markerPen, jh ); 
	}
	if ( v[ "geometryType" ] )
	{
		extractFromJson( v[ "geometryType" ], geometryType, jh ); 
	}
	if ( v[ "radius" ] )
	{
		extractFromJson( v[ "radius" ], radius, jh ); 
	}
	if ( v[ "show" ] )
	{
		extractFromJson( v[ "show" ], show, jh ); 
	}
	if ( v[ "pen" ] )
	{
		extractFromJson( v[ "pen" ], pen, jh ); 
	}
	if ( v[ "brush" ] )
	{
		extractFromJson( v[ "brush" ], brush, jh ); 
	}
	if ( v[ "textColor" ] )
	{
		extractFromJson( v[ "textColor" ], textColor, jh ); 
	}
}


// OUT class CgVedicChartGrid defined in src/base/geometry.h line 208
Json::Value CgVedicChartGrid::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "straightFrame" ] = writeToJson( straightFrame, jh );
	v[ "showFieldNames" ] = writeToJson( showFieldNames, jh );
	v[ "markerLength" ] = writeToJson( markerLength, jh );
	v[ "markerPen" ] = writeToJson( markerPen, jh );
	v[ "geometryType" ] = writeToJson( geometryType, jh );
	v[ "radius" ] = writeToJson( radius, jh );
	v[ "show" ] = writeToJson( show, jh );
	v[ "pen" ] = writeToJson( pen, jh );
	v[ "brush" ] = writeToJson( brush, jh );
	v[ "textColor" ] = writeToJson( textColor, jh );
	return v;
}


// IN class CgAnnulus defined in src/base/geometry.h line 240
void CgAnnulus::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "innerRadius" ] )
	{
		extractFromJson( v[ "innerRadius" ], innerRadius, jh ); 
	}
	if ( v[ "showInnerFrame" ] )
	{
		extractFromJson( v[ "showInnerFrame" ], showInnerFrame, jh ); 
	}
	if ( v[ "showOuterFrame" ] )
	{
		extractFromJson( v[ "showOuterFrame" ], showOuterFrame, jh ); 
	}
	if ( v[ "markerLength" ] )
	{
		extractFromJson( v[ "markerLength" ], markerLength, jh ); 
	}
	if ( v[ "markerPen" ] )
	{
		extractFromJson( v[ "markerPen" ], markerPen, jh ); 
	}
	if ( v[ "geometryType" ] )
	{
		extractFromJson( v[ "geometryType" ], geometryType, jh ); 
	}
	if ( v[ "radius" ] )
	{
		extractFromJson( v[ "radius" ], radius, jh ); 
	}
	if ( v[ "show" ] )
	{
		extractFromJson( v[ "show" ], show, jh ); 
	}
	if ( v[ "pen" ] )
	{
		extractFromJson( v[ "pen" ], pen, jh ); 
	}
	if ( v[ "brush" ] )
	{
		extractFromJson( v[ "brush" ], brush, jh ); 
	}
	if ( v[ "textColor" ] )
	{
		extractFromJson( v[ "textColor" ], textColor, jh ); 
	}
}


// OUT class CgAnnulus defined in src/base/geometry.h line 240
Json::Value CgAnnulus::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "innerRadius" ] = writeToJson( innerRadius, jh );
	v[ "showInnerFrame" ] = writeToJson( showInnerFrame, jh );
	v[ "showOuterFrame" ] = writeToJson( showOuterFrame, jh );
	v[ "markerLength" ] = writeToJson( markerLength, jh );
	v[ "markerPen" ] = writeToJson( markerPen, jh );
	v[ "geometryType" ] = writeToJson( geometryType, jh );
	v[ "radius" ] = writeToJson( radius, jh );
	v[ "show" ] = writeToJson( show, jh );
	v[ "pen" ] = writeToJson( pen, jh );
	v[ "brush" ] = writeToJson( brush, jh );
	v[ "textColor" ] = writeToJson( textColor, jh );
	return v;
}


// IN class DegreeMarkerConfig defined in src/base/geometry.h line 274
void DegreeMarkerConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "show" ] )
	{
		extractFromJson( v[ "show" ], show, jh ); 
	}
	if ( v[ "radius" ] )
	{
		extractFromJson( v[ "radius" ], radius, jh ); 
	}
	if ( v[ "wDegree1" ] )
	{
		extractFromJson( v[ "wDegree1" ], wDegree1, jh ); 
	}
	if ( v[ "wDegree5" ] )
	{
		extractFromJson( v[ "wDegree5" ], wDegree5, jh ); 
	}
	if ( v[ "wDegree10" ] )
	{
		extractFromJson( v[ "wDegree10" ], wDegree10, jh ); 
	}
	if ( v[ "show1DegreeFrame" ] )
	{
		extractFromJson( v[ "show1DegreeFrame" ], show1DegreeFrame, jh ); 
	}
	if ( v[ "show5DegreeFrame" ] )
	{
		extractFromJson( v[ "show5DegreeFrame" ], show5DegreeFrame, jh ); 
	}
	if ( v[ "show10DegreeFrame" ] )
	{
		extractFromJson( v[ "show10DegreeFrame" ], show10DegreeFrame, jh ); 
	}
	if ( v[ "pen1" ] )
	{
		extractFromJson( v[ "pen1" ], pen1, jh ); 
	}
	if ( v[ "pen5" ] )
	{
		extractFromJson( v[ "pen5" ], pen5, jh ); 
	}
	if ( v[ "pen10" ] )
	{
		extractFromJson( v[ "pen10" ], pen10, jh ); 
	}
}


// OUT class DegreeMarkerConfig defined in src/base/geometry.h line 274
Json::Value DegreeMarkerConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "show" ] = writeToJson( show, jh );
	v[ "radius" ] = writeToJson( radius, jh );
	v[ "wDegree1" ] = writeToJson( wDegree1, jh );
	v[ "wDegree5" ] = writeToJson( wDegree5, jh );
	v[ "wDegree10" ] = writeToJson( wDegree10, jh );
	v[ "show1DegreeFrame" ] = writeToJson( show1DegreeFrame, jh );
	v[ "show5DegreeFrame" ] = writeToJson( show5DegreeFrame, jh );
	v[ "show10DegreeFrame" ] = writeToJson( show10DegreeFrame, jh );
	v[ "pen1" ] = writeToJson( pen1, jh );
	v[ "pen5" ] = writeToJson( pen5, jh );
	v[ "pen10" ] = writeToJson( pen10, jh );
	return v;
}


// IN class CgLabelledAnnulus defined in src/base/geometry.h line 303
void CgLabelledAnnulus::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "rLabels" ] )
	{
		extractFromJson( v[ "rLabels" ], rLabels, jh ); 
	}
	if ( v[ "labelSize" ] )
	{
		extractFromJson( v[ "labelSize" ], labelSize, jh ); 
	}
	if ( v[ "useSymbols" ] )
	{
		extractFromJson( v[ "useSymbols" ], useSymbols, jh ); 
	}
	if ( v[ "textLength" ] )
	{
		extractFromJson( v[ "textLength" ], textLength, jh ); 
	}
	if ( v[ "showLabels" ] )
	{
		extractFromJson( v[ "showLabels" ], showLabels, jh ); 
	}
	if ( v[ "showConnectors" ] )
	{
		extractFromJson( v[ "showConnectors" ], showConnectors, jh ); 
	}
	if ( v[ "rotateLabels" ] )
	{
		extractFromJson( v[ "rotateLabels" ], rotateLabels, jh ); 
	}
	if ( v[ "degreeMarkerCfg" ] )
	{
		extractFromJson( v[ "degreeMarkerCfg" ], degreeMarkerCfg, jh ); 
	}
	if ( v[ "innerRadius" ] )
	{
		extractFromJson( v[ "innerRadius" ], innerRadius, jh ); 
	}
	if ( v[ "showInnerFrame" ] )
	{
		extractFromJson( v[ "showInnerFrame" ], showInnerFrame, jh ); 
	}
	if ( v[ "showOuterFrame" ] )
	{
		extractFromJson( v[ "showOuterFrame" ], showOuterFrame, jh ); 
	}
	if ( v[ "markerLength" ] )
	{
		extractFromJson( v[ "markerLength" ], markerLength, jh ); 
	}
	if ( v[ "markerPen" ] )
	{
		extractFromJson( v[ "markerPen" ], markerPen, jh ); 
	}
	if ( v[ "geometryType" ] )
	{
		extractFromJson( v[ "geometryType" ], geometryType, jh ); 
	}
	if ( v[ "radius" ] )
	{
		extractFromJson( v[ "radius" ], radius, jh ); 
	}
	if ( v[ "show" ] )
	{
		extractFromJson( v[ "show" ], show, jh ); 
	}
	if ( v[ "pen" ] )
	{
		extractFromJson( v[ "pen" ], pen, jh ); 
	}
	if ( v[ "brush" ] )
	{
		extractFromJson( v[ "brush" ], brush, jh ); 
	}
	if ( v[ "textColor" ] )
	{
		extractFromJson( v[ "textColor" ], textColor, jh ); 
	}
}


// OUT class CgLabelledAnnulus defined in src/base/geometry.h line 303
Json::Value CgLabelledAnnulus::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "rLabels" ] = writeToJson( rLabels, jh );
	v[ "labelSize" ] = writeToJson( labelSize, jh );
	v[ "useSymbols" ] = writeToJson( useSymbols, jh );
	v[ "textLength" ] = writeToJson( textLength, jh );
	v[ "showLabels" ] = writeToJson( showLabels, jh );
	v[ "showConnectors" ] = writeToJson( showConnectors, jh );
	v[ "rotateLabels" ] = writeToJson( rotateLabels, jh );
	v[ "degreeMarkerCfg" ] = writeToJson( degreeMarkerCfg, jh );
	v[ "innerRadius" ] = writeToJson( innerRadius, jh );
	v[ "showInnerFrame" ] = writeToJson( showInnerFrame, jh );
	v[ "showOuterFrame" ] = writeToJson( showOuterFrame, jh );
	v[ "markerLength" ] = writeToJson( markerLength, jh );
	v[ "markerPen" ] = writeToJson( markerPen, jh );
	v[ "geometryType" ] = writeToJson( geometryType, jh );
	v[ "radius" ] = writeToJson( radius, jh );
	v[ "show" ] = writeToJson( show, jh );
	v[ "pen" ] = writeToJson( pen, jh );
	v[ "brush" ] = writeToJson( brush, jh );
	v[ "textColor" ] = writeToJson( textColor, jh );
	return v;
}


// IN class CgPlanetRing defined in src/base/geometry.h line 375
void CgPlanetRing::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "nextOuterRadiusDelta" ] )
	{
		extractFromJson( v[ "nextOuterRadiusDelta" ], nextOuterRadiusDelta, jh ); 
	}
	if ( v[ "rLabels" ] )
	{
		extractFromJson( v[ "rLabels" ], rLabels, jh ); 
	}
	if ( v[ "labelSize" ] )
	{
		extractFromJson( v[ "labelSize" ], labelSize, jh ); 
	}
	if ( v[ "useSymbols" ] )
	{
		extractFromJson( v[ "useSymbols" ], useSymbols, jh ); 
	}
	if ( v[ "textLength" ] )
	{
		extractFromJson( v[ "textLength" ], textLength, jh ); 
	}
	if ( v[ "showLabels" ] )
	{
		extractFromJson( v[ "showLabels" ], showLabels, jh ); 
	}
	if ( v[ "showConnectors" ] )
	{
		extractFromJson( v[ "showConnectors" ], showConnectors, jh ); 
	}
	if ( v[ "rotateLabels" ] )
	{
		extractFromJson( v[ "rotateLabels" ], rotateLabels, jh ); 
	}
	if ( v[ "degreeMarkerCfg" ] )
	{
		extractFromJson( v[ "degreeMarkerCfg" ], degreeMarkerCfg, jh ); 
	}
	if ( v[ "innerRadius" ] )
	{
		extractFromJson( v[ "innerRadius" ], innerRadius, jh ); 
	}
	if ( v[ "showInnerFrame" ] )
	{
		extractFromJson( v[ "showInnerFrame" ], showInnerFrame, jh ); 
	}
	if ( v[ "showOuterFrame" ] )
	{
		extractFromJson( v[ "showOuterFrame" ], showOuterFrame, jh ); 
	}
	if ( v[ "markerLength" ] )
	{
		extractFromJson( v[ "markerLength" ], markerLength, jh ); 
	}
	if ( v[ "markerPen" ] )
	{
		extractFromJson( v[ "markerPen" ], markerPen, jh ); 
	}
	if ( v[ "geometryType" ] )
	{
		extractFromJson( v[ "geometryType" ], geometryType, jh ); 
	}
	if ( v[ "radius" ] )
	{
		extractFromJson( v[ "radius" ], radius, jh ); 
	}
	if ( v[ "show" ] )
	{
		extractFromJson( v[ "show" ], show, jh ); 
	}
	if ( v[ "pen" ] )
	{
		extractFromJson( v[ "pen" ], pen, jh ); 
	}
	if ( v[ "brush" ] )
	{
		extractFromJson( v[ "brush" ], brush, jh ); 
	}
	if ( v[ "textColor" ] )
	{
		extractFromJson( v[ "textColor" ], textColor, jh ); 
	}
}


// OUT class CgPlanetRing defined in src/base/geometry.h line 375
Json::Value CgPlanetRing::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "nextOuterRadiusDelta" ] = writeToJson( nextOuterRadiusDelta, jh );
	v[ "rLabels" ] = writeToJson( rLabels, jh );
	v[ "labelSize" ] = writeToJson( labelSize, jh );
	v[ "useSymbols" ] = writeToJson( useSymbols, jh );
	v[ "textLength" ] = writeToJson( textLength, jh );
	v[ "showLabels" ] = writeToJson( showLabels, jh );
	v[ "showConnectors" ] = writeToJson( showConnectors, jh );
	v[ "rotateLabels" ] = writeToJson( rotateLabels, jh );
	v[ "degreeMarkerCfg" ] = writeToJson( degreeMarkerCfg, jh );
	v[ "innerRadius" ] = writeToJson( innerRadius, jh );
	v[ "showInnerFrame" ] = writeToJson( showInnerFrame, jh );
	v[ "showOuterFrame" ] = writeToJson( showOuterFrame, jh );
	v[ "markerLength" ] = writeToJson( markerLength, jh );
	v[ "markerPen" ] = writeToJson( markerPen, jh );
	v[ "geometryType" ] = writeToJson( geometryType, jh );
	v[ "radius" ] = writeToJson( radius, jh );
	v[ "show" ] = writeToJson( show, jh );
	v[ "pen" ] = writeToJson( pen, jh );
	v[ "brush" ] = writeToJson( brush, jh );
	v[ "textColor" ] = writeToJson( textColor, jh );
	return v;
}


// IN class CgUranianDisk defined in src/base/geometry.h line 407
void CgUranianDisk::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "degreeMarker" ] )
	{
		extractFromJson( v[ "degreeMarker" ], degreeMarker, jh ); 
	}
	if ( v[ "degreeLabelSize" ] )
	{
		extractFromJson( v[ "degreeLabelSize" ], degreeLabelSize, jh ); 
	}
	if ( v[ "paintDegreeNumbers" ] )
	{
		extractFromJson( v[ "paintDegreeNumbers" ], paintDegreeNumbers, jh ); 
	}
	if ( v[ "showSigns" ] )
	{
		extractFromJson( v[ "showSigns" ], showSigns, jh ); 
	}
	if ( v[ "signInnerRadius" ] )
	{
		extractFromJson( v[ "signInnerRadius" ], signInnerRadius, jh ); 
	}
	if ( v[ "signOuterRadius" ] )
	{
		extractFromJson( v[ "signOuterRadius" ], signOuterRadius, jh ); 
	}
	if ( v[ "signRLabels" ] )
	{
		extractFromJson( v[ "signRLabels" ], signRLabels, jh ); 
	}
	if ( v[ "showHouses" ] )
	{
		extractFromJson( v[ "showHouses" ], showHouses, jh ); 
	}
	if ( v[ "houseInnerRadius" ] )
	{
		extractFromJson( v[ "houseInnerRadius" ], houseInnerRadius, jh ); 
	}
	if ( v[ "houseOuterRadius" ] )
	{
		extractFromJson( v[ "houseOuterRadius" ], houseOuterRadius, jh ); 
	}
	if ( v[ "houseRLabels" ] )
	{
		extractFromJson( v[ "houseRLabels" ], houseRLabels, jh ); 
	}
	if ( v[ "rLabels" ] )
	{
		extractFromJson( v[ "rLabels" ], rLabels, jh ); 
	}
	if ( v[ "labelSize" ] )
	{
		extractFromJson( v[ "labelSize" ], labelSize, jh ); 
	}
	if ( v[ "useSymbols" ] )
	{
		extractFromJson( v[ "useSymbols" ], useSymbols, jh ); 
	}
	if ( v[ "textLength" ] )
	{
		extractFromJson( v[ "textLength" ], textLength, jh ); 
	}
	if ( v[ "showLabels" ] )
	{
		extractFromJson( v[ "showLabels" ], showLabels, jh ); 
	}
	if ( v[ "showConnectors" ] )
	{
		extractFromJson( v[ "showConnectors" ], showConnectors, jh ); 
	}
	if ( v[ "rotateLabels" ] )
	{
		extractFromJson( v[ "rotateLabels" ], rotateLabels, jh ); 
	}
	if ( v[ "degreeMarkerCfg" ] )
	{
		extractFromJson( v[ "degreeMarkerCfg" ], degreeMarkerCfg, jh ); 
	}
	if ( v[ "innerRadius" ] )
	{
		extractFromJson( v[ "innerRadius" ], innerRadius, jh ); 
	}
	if ( v[ "showInnerFrame" ] )
	{
		extractFromJson( v[ "showInnerFrame" ], showInnerFrame, jh ); 
	}
	if ( v[ "showOuterFrame" ] )
	{
		extractFromJson( v[ "showOuterFrame" ], showOuterFrame, jh ); 
	}
	if ( v[ "markerLength" ] )
	{
		extractFromJson( v[ "markerLength" ], markerLength, jh ); 
	}
	if ( v[ "markerPen" ] )
	{
		extractFromJson( v[ "markerPen" ], markerPen, jh ); 
	}
	if ( v[ "geometryType" ] )
	{
		extractFromJson( v[ "geometryType" ], geometryType, jh ); 
	}
	if ( v[ "radius" ] )
	{
		extractFromJson( v[ "radius" ], radius, jh ); 
	}
	if ( v[ "show" ] )
	{
		extractFromJson( v[ "show" ], show, jh ); 
	}
	if ( v[ "pen" ] )
	{
		extractFromJson( v[ "pen" ], pen, jh ); 
	}
	if ( v[ "brush" ] )
	{
		extractFromJson( v[ "brush" ], brush, jh ); 
	}
	if ( v[ "textColor" ] )
	{
		extractFromJson( v[ "textColor" ], textColor, jh ); 
	}
}


// OUT class CgUranianDisk defined in src/base/geometry.h line 407
Json::Value CgUranianDisk::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "degreeMarker" ] = writeToJson( degreeMarker, jh );
	v[ "degreeLabelSize" ] = writeToJson( degreeLabelSize, jh );
	v[ "paintDegreeNumbers" ] = writeToJson( paintDegreeNumbers, jh );
	v[ "showSigns" ] = writeToJson( showSigns, jh );
	v[ "signInnerRadius" ] = writeToJson( signInnerRadius, jh );
	v[ "signOuterRadius" ] = writeToJson( signOuterRadius, jh );
	v[ "signRLabels" ] = writeToJson( signRLabels, jh );
	v[ "showHouses" ] = writeToJson( showHouses, jh );
	v[ "houseInnerRadius" ] = writeToJson( houseInnerRadius, jh );
	v[ "houseOuterRadius" ] = writeToJson( houseOuterRadius, jh );
	v[ "houseRLabels" ] = writeToJson( houseRLabels, jh );
	v[ "rLabels" ] = writeToJson( rLabels, jh );
	v[ "labelSize" ] = writeToJson( labelSize, jh );
	v[ "useSymbols" ] = writeToJson( useSymbols, jh );
	v[ "textLength" ] = writeToJson( textLength, jh );
	v[ "showLabels" ] = writeToJson( showLabels, jh );
	v[ "showConnectors" ] = writeToJson( showConnectors, jh );
	v[ "rotateLabels" ] = writeToJson( rotateLabels, jh );
	v[ "degreeMarkerCfg" ] = writeToJson( degreeMarkerCfg, jh );
	v[ "innerRadius" ] = writeToJson( innerRadius, jh );
	v[ "showInnerFrame" ] = writeToJson( showInnerFrame, jh );
	v[ "showOuterFrame" ] = writeToJson( showOuterFrame, jh );
	v[ "markerLength" ] = writeToJson( markerLength, jh );
	v[ "markerPen" ] = writeToJson( markerPen, jh );
	v[ "geometryType" ] = writeToJson( geometryType, jh );
	v[ "radius" ] = writeToJson( radius, jh );
	v[ "show" ] = writeToJson( show, jh );
	v[ "pen" ] = writeToJson( pen, jh );
	v[ "brush" ] = writeToJson( brush, jh );
	v[ "textColor" ] = writeToJson( textColor, jh );
	return v;
}


// IN class SingleWesternAspectConfig defined in src/base/AspectConfig.h line 46
void SingleWesternAspectConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "active" ] )
	{
		extractFromJson( v[ "active" ], active, jh ); 
	}
	if ( v[ "orbis" ] )
	{
		extractFromJson( v[ "orbis" ], orbis, jh ); 
	}
	if ( v[ "pen" ] )
	{
		extractFromJson( v[ "pen" ], pen, jh ); 
	}
}


// OUT class SingleWesternAspectConfig defined in src/base/AspectConfig.h line 46
Json::Value SingleWesternAspectConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "active" ] = writeToJson( active, jh );
	v[ "orbis" ] = writeToJson( orbis, jh );
	v[ "pen" ] = writeToJson( pen, jh );
	return v;
}


// IN class WesternAspectConfig defined in src/base/AspectConfig.h line 74
void WesternAspectConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "showAspects" ] )
	{
		extractFromJson( v[ "showAspects" ], showAspects, jh ); 
	}
	if ( v[ "useColors" ] )
	{
		extractFromJson( v[ "useColors" ], useColors, jh ); 
	}
	if ( v[ "showSymbols" ] )
	{
		extractFromJson( v[ "showSymbols" ], showSymbols, jh ); 
	}
	if ( v[ "showOrbis" ] )
	{
		extractFromJson( v[ "showOrbis" ], showOrbis, jh ); 
	}
	if ( v[ "typeConfigs" ] )
	{
		extractFromJson( v[ "typeConfigs" ], typeConfigs, jh ); 
	}
}


// OUT class WesternAspectConfig defined in src/base/AspectConfig.h line 74
Json::Value WesternAspectConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "showAspects" ] = writeToJson( showAspects, jh );
	v[ "useColors" ] = writeToJson( useColors, jh );
	v[ "showSymbols" ] = writeToJson( showSymbols, jh );
	v[ "showOrbis" ] = writeToJson( showOrbis, jh );
	v[ "typeConfigs" ] = writeToJson( typeConfigs, jh );
	return v;
}


// IN class UranianAspectPenStyle defined in src/base/AspectConfig.h line 104
void UranianAspectPenStyle::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "mainPen" ] )
	{
		extractFromJson( v[ "mainPen" ], mainPen, jh ); 
	}
	if ( v[ "connectorPen" ] )
	{
		extractFromJson( v[ "connectorPen" ], connectorPen, jh ); 
	}
}


// OUT class UranianAspectPenStyle defined in src/base/AspectConfig.h line 104
Json::Value UranianAspectPenStyle::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "mainPen" ] = writeToJson( mainPen, jh );
	v[ "connectorPen" ] = writeToJson( connectorPen, jh );
	return v;
}


// IN class UranianAspectStyle defined in src/base/AspectConfig.h line 124
void UranianAspectStyle::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "showAspects" ] )
	{
		extractFromJson( v[ "showAspects" ], showAspects, jh ); 
	}
	if ( v[ "useColors" ] )
	{
		extractFromJson( v[ "useColors" ], useColors, jh ); 
	}
	if ( v[ "drawConnector" ] )
	{
		extractFromJson( v[ "drawConnector" ], drawConnector, jh ); 
	}
	if ( v[ "showDivision" ] )
	{
		extractFromJson( v[ "showDivision" ], showDivision, jh ); 
	}
	if ( v[ "showOrbis" ] )
	{
		extractFromJson( v[ "showOrbis" ], showOrbis, jh ); 
	}
	if ( v[ "penStyles" ] )
	{
		extractFromJson( v[ "penStyles" ], penStyles, jh ); 
	}
}


// OUT class UranianAspectStyle defined in src/base/AspectConfig.h line 124
Json::Value UranianAspectStyle::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "showAspects" ] = writeToJson( showAspects, jh );
	v[ "useColors" ] = writeToJson( useColors, jh );
	v[ "drawConnector" ] = writeToJson( drawConnector, jh );
	v[ "showDivision" ] = writeToJson( showDivision, jh );
	v[ "showOrbis" ] = writeToJson( showOrbis, jh );
	v[ "penStyles" ] = writeToJson( penStyles, jh );
	return v;
}


// IN class Location defined in src/base/datelocation.h line 75
void Location::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "latitude" ] )
	{
		extractFromJson( v[ "latitude" ], latitude, jh ); 
	}
	if ( v[ "longitude" ] )
	{
		extractFromJson( v[ "longitude" ], longitude, jh ); 
	}
	if ( v[ "tz" ] )
	{
		extractFromJson( v[ "tz" ], tz, jh ); 
	}
	if ( v[ "dst" ] )
	{
		extractFromJson( v[ "dst" ], dst, jh ); 
	}
	if ( v[ "locName" ] )
	{
		extractFromJson( v[ "locName" ], locName, jh ); 
	}
	if ( v[ "dstMode" ] )
	{
		extractFromJson( v[ "dstMode" ], dstMode, jh ); 
	}
}


// OUT class Location defined in src/base/datelocation.h line 75
Json::Value Location::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "latitude" ] = writeToJson( latitude, jh );
	v[ "longitude" ] = writeToJson( longitude, jh );
	v[ "tz" ] = writeToJson( tz, jh );
	v[ "dst" ] = writeToJson( dst, jh );
	v[ "locName" ] = writeToJson( locName, jh );
	v[ "dstMode" ] = writeToJson( dstMode, jh );
	return v;
}


// IN class GeneralConfig defined in src/base/astroconfig.h line 52
void GeneralConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "defaultViewpoint" ] )
	{
		extractFromJson( v[ "defaultViewpoint" ], defaultViewpoint, jh ); 
	}
	if ( v[ "viewpointChangePolicy" ] )
	{
		extractFromJson( v[ "viewpointChangePolicy" ], viewpointChangePolicy, jh ); 
	}
	if ( v[ "languageCode" ] )
	{
		extractFromJson( v[ "languageCode" ], languageCode, jh ); 
	}
	if ( v[ "additionalLanguageCodes" ] )
	{
		extractFromJson( v[ "additionalLanguageCodes" ], additionalLanguageCodes, jh ); 
	}
	if ( v[ "dateFormat" ] )
	{
		extractFromJson( v[ "dateFormat" ], dateFormat, jh ); 
	}
	if ( v[ "statusbarShowLogMessages" ] )
	{
		extractFromJson( v[ "statusbarShowLogMessages" ], statusbarShowLogMessages, jh ); 
	}
	if ( v[ "statusbarShowAscPosition" ] )
	{
		extractFromJson( v[ "statusbarShowAscPosition" ], statusbarShowAscPosition, jh ); 
	}
	if ( v[ "statusbarShowMoonPosition" ] )
	{
		extractFromJson( v[ "statusbarShowMoonPosition" ], statusbarShowMoonPosition, jh ); 
	}
	if ( v[ "statusMessageTimeout" ] )
	{
		extractFromJson( v[ "statusMessageTimeout" ], statusMessageTimeout, jh ); 
	}
	if ( v[ "openNewDocOnStart" ] )
	{
		extractFromJson( v[ "openNewDocOnStart" ], openNewDocOnStart, jh ); 
	}
	if ( v[ "askOnQuit" ] )
	{
		extractFromJson( v[ "askOnQuit" ], askOnQuit, jh ); 
	}
}


// OUT class GeneralConfig defined in src/base/astroconfig.h line 52
Json::Value GeneralConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "defaultViewpoint" ] = writeToJson( defaultViewpoint, jh );
	v[ "viewpointChangePolicy" ] = writeToJson( viewpointChangePolicy, jh );
	v[ "languageCode" ] = writeToJson( languageCode, jh );
	v[ "additionalLanguageCodes" ] = writeToJson( additionalLanguageCodes, jh );
	v[ "dateFormat" ] = writeToJson( dateFormat, jh );
	v[ "statusbarShowLogMessages" ] = writeToJson( statusbarShowLogMessages, jh );
	v[ "statusbarShowAscPosition" ] = writeToJson( statusbarShowAscPosition, jh );
	v[ "statusbarShowMoonPosition" ] = writeToJson( statusbarShowMoonPosition, jh );
	v[ "statusMessageTimeout" ] = writeToJson( statusMessageTimeout, jh );
	v[ "openNewDocOnStart" ] = writeToJson( openNewDocOnStart, jh );
	v[ "askOnQuit" ] = writeToJson( askOnQuit, jh );
	return v;
}


// IN class MatchFilter defined in src/base/astroconfig.h line 86
void MatchFilter::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "rule" ] )
	{
		extractFromJson( v[ "rule" ], rule, jh ); 
	}
	if ( v[ "side" ] )
	{
		extractFromJson( v[ "side" ], side, jh ); 
	}
	if ( v[ "selectedPlanets" ] )
	{
		extractFromJson( v[ "selectedPlanets" ], selectedPlanets, jh ); 
	}
}


// OUT class MatchFilter defined in src/base/astroconfig.h line 86
Json::Value MatchFilter::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "rule" ] = writeToJson( rule, jh );
	v[ "side" ] = writeToJson( side, jh );
	v[ "selectedPlanets" ] = writeToJson( selectedPlanets, jh );
	return v;
}


// IN class RoleRestrictor defined in src/base/astroconfig.h line 115
void RoleRestrictor::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "role" ] )
	{
		extractFromJson( v[ "role" ], role, jh ); 
	}
	if ( v[ "enabled" ] )
	{
		extractFromJson( v[ "enabled" ], enabled, jh ); 
	}
	if ( v[ "filter" ] )
	{
		extractFromJson( v[ "filter" ], filter, jh ); 
	}
}


// OUT class RoleRestrictor defined in src/base/astroconfig.h line 115
Json::Value RoleRestrictor::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "role" ] = writeToJson( role, jh );
	v[ "enabled" ] = writeToJson( enabled, jh );
	v[ "filter" ] = writeToJson( filter, jh );
	return v;
}


// IN class UranianSection defined in src/base/astroconfig.h line 138
void UranianSection::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "includeAntiscia" ] )
	{
		extractFromJson( v[ "includeAntiscia" ], includeAntiscia, jh ); 
	}
	if ( v[ "showPpLongitude" ] )
	{
		extractFromJson( v[ "showPpLongitude" ], showPpLongitude, jh ); 
	}
	if ( v[ "showPpScore" ] )
	{
		extractFromJson( v[ "showPpScore" ], showPpScore, jh ); 
	}
	if ( v[ "chartFormulae" ] )
	{
		extractFromJson( v[ "chartFormulae" ], chartFormulae, jh ); 
	}
	if ( v[ "chartDivision" ] )
	{
		extractFromJson( v[ "chartDivision" ], chartDivision, jh ); 
	}
	if ( v[ "progressionMode" ] )
	{
		extractFromJson( v[ "progressionMode" ], progressionMode, jh ); 
	}
	if ( v[ "sortOrder" ] )
	{
		extractFromJson( v[ "sortOrder" ], sortOrder, jh ); 
	}
	if ( v[ "roleRestrictors" ] )
	{
		extractFromJson( v[ "roleRestrictors" ], roleRestrictors, jh ); 
	}
	if ( v[ "matchDivision" ] )
	{
		extractFromJson( v[ "matchDivision" ], matchDivision, jh ); 
	}
	if ( v[ "matchFormulae" ] )
	{
		extractFromJson( v[ "matchFormulae" ], matchFormulae, jh ); 
	}
	if ( v[ "orbis" ] )
	{
		extractFromJson( v[ "orbis" ], orbis, jh ); 
	}
	if ( v[ "matchesShowDuedate" ] )
	{
		extractFromJson( v[ "matchesShowDuedate" ], matchesShowDuedate, jh ); 
	}
	if ( v[ "matchesShowDuedateTime" ] )
	{
		extractFromJson( v[ "matchesShowDuedateTime" ], matchesShowDuedateTime, jh ); 
	}
	if ( v[ "matchesShowOrbis" ] )
	{
		extractFromJson( v[ "matchesShowOrbis" ], matchesShowOrbis, jh ); 
	}
	if ( v[ "matchesShowSpeed" ] )
	{
		extractFromJson( v[ "matchesShowSpeed" ], matchesShowSpeed, jh ); 
	}
	if ( v[ "matchesShowResiduum" ] )
	{
		extractFromJson( v[ "matchesShowResiduum" ], matchesShowResiduum, jh ); 
	}
	if ( v[ "matchesShowRole" ] )
	{
		extractFromJson( v[ "matchesShowRole" ], matchesShowRole, jh ); 
	}
	if ( v[ "matchesShowText" ] )
	{
		extractFromJson( v[ "matchesShowText" ], matchesShowText, jh ); 
	}
}


// OUT class UranianSection defined in src/base/astroconfig.h line 138
Json::Value UranianSection::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "includeAntiscia" ] = writeToJson( includeAntiscia, jh );
	v[ "showPpLongitude" ] = writeToJson( showPpLongitude, jh );
	v[ "showPpScore" ] = writeToJson( showPpScore, jh );
	v[ "chartFormulae" ] = writeToJson( chartFormulae, jh );
	v[ "chartDivision" ] = writeToJson( chartDivision, jh );
	v[ "progressionMode" ] = writeToJson( progressionMode, jh );
	v[ "sortOrder" ] = writeToJson( sortOrder, jh );
	v[ "roleRestrictors" ] = writeToJson( roleRestrictors, jh );
	v[ "matchDivision" ] = writeToJson( matchDivision, jh );
	v[ "matchFormulae" ] = writeToJson( matchFormulae, jh );
	v[ "orbis" ] = writeToJson( orbis, jh );
	v[ "matchesShowDuedate" ] = writeToJson( matchesShowDuedate, jh );
	v[ "matchesShowDuedateTime" ] = writeToJson( matchesShowDuedateTime, jh );
	v[ "matchesShowOrbis" ] = writeToJson( matchesShowOrbis, jh );
	v[ "matchesShowSpeed" ] = writeToJson( matchesShowSpeed, jh );
	v[ "matchesShowResiduum" ] = writeToJson( matchesShowResiduum, jh );
	v[ "matchesShowRole" ] = writeToJson( matchesShowRole, jh );
	v[ "matchesShowText" ] = writeToJson( matchesShowText, jh );
	return v;
}


// IN class UranianConfig defined in src/base/astroconfig.h line 230
void UranianConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "antisciaDisplayStyle" ] )
	{
		extractFromJson( v[ "antisciaDisplayStyle" ], antisciaDisplayStyle, jh ); 
	}
	if ( v[ "matchFormat" ] )
	{
		extractFromJson( v[ "matchFormat" ], matchFormat, jh ); 
	}
	if ( v[ "divisionPrimes" ] )
	{
		extractFromJson( v[ "divisionPrimes" ], divisionPrimes, jh ); 
	}
	if ( v[ "maximumDivision" ] )
	{
		extractFromJson( v[ "maximumDivision" ], maximumDivision, jh ); 
	}
	if ( v[ "maxMatchCount" ] )
	{
		extractFromJson( v[ "maxMatchCount" ], maxMatchCount, jh ); 
	}
	if ( v[ "doTrimTableItems" ] )
	{
		extractFromJson( v[ "doTrimTableItems" ], doTrimTableItems, jh ); 
	}
}


// OUT class UranianConfig defined in src/base/astroconfig.h line 230
Json::Value UranianConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "antisciaDisplayStyle" ] = writeToJson( antisciaDisplayStyle, jh );
	v[ "matchFormat" ] = writeToJson( matchFormat, jh );
	v[ "divisionPrimes" ] = writeToJson( divisionPrimes, jh );
	v[ "maximumDivision" ] = writeToJson( maximumDivision, jh );
	v[ "maxMatchCount" ] = writeToJson( maxMatchCount, jh );
	v[ "doTrimTableItems" ] = writeToJson( doTrimTableItems, jh );
	return v;
}


// IN class YearLengthConfig defined in src/base/astroconfig.h line 261
void YearLengthConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "type" ] )
	{
		extractFromJson( v[ "type" ], type, jh ); 
	}
	if ( v[ "customValue" ] )
	{
		extractFromJson( v[ "customValue" ], customValue, jh ); 
	}
}


// OUT class YearLengthConfig defined in src/base/astroconfig.h line 261
Json::Value YearLengthConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "type" ] = writeToJson( type, jh );
	v[ "customValue" ] = writeToJson( customValue, jh );
	return v;
}


// IN class DirectionConfig defined in src/base/astroconfig.h line 280
void DirectionConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "yearLength" ] )
	{
		extractFromJson( v[ "yearLength" ], yearLength, jh ); 
	}
	if ( v[ "multiDirectionRoles" ] )
	{
		extractFromJson( v[ "multiDirectionRoles" ], multiDirectionRoles, jh ); 
	}
	if ( v[ "eventFinderItems" ] )
	{
		extractFromJson( v[ "eventFinderItems" ], eventFinderItems, jh ); 
	}
}


// OUT class DirectionConfig defined in src/base/astroconfig.h line 280
Json::Value DirectionConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "yearLength" ] = writeToJson( yearLength, jh );
	v[ "multiDirectionRoles" ] = writeToJson( multiDirectionRoles, jh );
	v[ "eventFinderItems" ] = writeToJson( eventFinderItems, jh );
	return v;
}


// IN class KalavelaElement defined in src/base/astroconfig.h line 301
void KalavelaElement::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "lord" ] )
	{
		extractFromJson( v[ "lord" ], lord, jh ); 
	}
	if ( v[ "portion" ] )
	{
		extractFromJson( v[ "portion" ], portion, jh ); 
	}
}


// OUT class KalavelaElement defined in src/base/astroconfig.h line 301
Json::Value KalavelaElement::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "lord" ] = writeToJson( lord, jh );
	v[ "portion" ] = writeToJson( portion, jh );
	return v;
}


// IN class KalavelaConfig defined in src/base/astroconfig.h line 324
void KalavelaConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "elements" ] )
	{
		extractFromJson( v[ "elements" ], elements, jh ); 
	}
}


// OUT class KalavelaConfig defined in src/base/astroconfig.h line 324
Json::Value KalavelaConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "elements" ] = writeToJson( elements, jh );
	return v;
}


// IN class VargaConfig defined in src/base/astroconfig.h line 341
void VargaConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "moonBenefic" ] )
	{
		extractFromJson( v[ "moonBenefic" ], moonBenefic, jh ); 
	}
	if ( v[ "mercuryBenefic" ] )
	{
		extractFromJson( v[ "mercuryBenefic" ], mercuryBenefic, jh ); 
	}
	if ( v[ "tempFriendMode" ] )
	{
		extractFromJson( v[ "tempFriendMode" ], tempFriendMode, jh ); 
	}
	if ( v[ "vargaHoraMode" ] )
	{
		extractFromJson( v[ "vargaHoraMode" ], vargaHoraMode, jh ); 
	}
	if ( v[ "vargaDrekkanaMode" ] )
	{
		extractFromJson( v[ "vargaDrekkanaMode" ], vargaDrekkanaMode, jh ); 
	}
	if ( v[ "vargaChaturtamsaMode" ] )
	{
		extractFromJson( v[ "vargaChaturtamsaMode" ], vargaChaturtamsaMode, jh ); 
	}
}


// OUT class VargaConfig defined in src/base/astroconfig.h line 341
Json::Value VargaConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "moonBenefic" ] = writeToJson( moonBenefic, jh );
	v[ "mercuryBenefic" ] = writeToJson( mercuryBenefic, jh );
	v[ "tempFriendMode" ] = writeToJson( tempFriendMode, jh );
	v[ "vargaHoraMode" ] = writeToJson( vargaHoraMode, jh );
	v[ "vargaDrekkanaMode" ] = writeToJson( vargaDrekkanaMode, jh );
	v[ "vargaChaturtamsaMode" ] = writeToJson( vargaChaturtamsaMode, jh );
	return v;
}


// IN class DasaCalculationConfig defined in src/base/astroconfig.h line 364
void DasaCalculationConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "startOfLastMahadasa" ] )
	{
		extractFromJson( v[ "startOfLastMahadasa" ], startOfLastMahadasa, jh ); 
	}
	if ( v[ "nakshatraPortionMode" ] )
	{
		extractFromJson( v[ "nakshatraPortionMode" ], nakshatraPortionMode, jh ); 
	}
	if ( v[ "kalachakraMode" ] )
	{
		extractFromJson( v[ "kalachakraMode" ], kalachakraMode, jh ); 
	}
	if ( v[ "dasaDateDisplayMode" ] )
	{
		extractFromJson( v[ "dasaDateDisplayMode" ], dasaDateDisplayMode, jh ); 
	}
	if ( v[ "yearLength" ] )
	{
		extractFromJson( v[ "yearLength" ], yearLength, jh ); 
	}
}


// OUT class DasaCalculationConfig defined in src/base/astroconfig.h line 364
Json::Value DasaCalculationConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "startOfLastMahadasa" ] = writeToJson( startOfLastMahadasa, jh );
	v[ "nakshatraPortionMode" ] = writeToJson( nakshatraPortionMode, jh );
	v[ "kalachakraMode" ] = writeToJson( kalachakraMode, jh );
	v[ "dasaDateDisplayMode" ] = writeToJson( dasaDateDisplayMode, jh );
	v[ "yearLength" ] = writeToJson( yearLength, jh );
	return v;
}


// IN class AshtakavargaConfig defined in src/base/astroconfig.h line 388
void AshtakavargaConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "ashtakavargaMode" ] )
	{
		extractFromJson( v[ "ashtakavargaMode" ], ashtakavargaMode, jh ); 
	}
	if ( v[ "ashtakavargaSarva" ] )
	{
		extractFromJson( v[ "ashtakavargaSarva" ], ashtakavargaSarva, jh ); 
	}
	if ( v[ "ashtakavargaNumberMode" ] )
	{
		extractFromJson( v[ "ashtakavargaNumberMode" ], ashtakavargaNumberMode, jh ); 
	}
	if ( v[ "ashtakavargaSodhyaPindaMode" ] )
	{
		extractFromJson( v[ "ashtakavargaSodhyaPindaMode" ], ashtakavargaSodhyaPindaMode, jh ); 
	}
}


// OUT class AshtakavargaConfig defined in src/base/astroconfig.h line 388
Json::Value AshtakavargaConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "ashtakavargaMode" ] = writeToJson( ashtakavargaMode, jh );
	v[ "ashtakavargaSarva" ] = writeToJson( ashtakavargaSarva, jh );
	v[ "ashtakavargaNumberMode" ] = writeToJson( ashtakavargaNumberMode, jh );
	v[ "ashtakavargaSodhyaPindaMode" ] = writeToJson( ashtakavargaSodhyaPindaMode, jh );
	return v;
}


// IN class JaiminiConfig defined in src/base/astroconfig.h line 408
void JaiminiConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "arudhaMode" ] )
	{
		extractFromJson( v[ "arudhaMode" ], arudhaMode, jh ); 
	}
	if ( v[ "charaKarakaMode" ] )
	{
		extractFromJson( v[ "charaKarakaMode" ], charaKarakaMode, jh ); 
	}
}


// OUT class JaiminiConfig defined in src/base/astroconfig.h line 408
Json::Value JaiminiConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "arudhaMode" ] = writeToJson( arudhaMode, jh );
	v[ "charaKarakaMode" ] = writeToJson( charaKarakaMode, jh );
	return v;
}


// IN class AnimationConfig defined in src/base/astroconfig.h line 424
void AnimationConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "interval" ] )
	{
		extractFromJson( v[ "interval" ], interval, jh ); 
	}
	if ( v[ "mode" ] )
	{
		extractFromJson( v[ "mode" ], mode, jh ); 
	}
	if ( v[ "stepLength" ] )
	{
		extractFromJson( v[ "stepLength" ], stepLength, jh ); 
	}
	if ( v[ "stepDimension" ] )
	{
		extractFromJson( v[ "stepDimension" ], stepDimension, jh ); 
	}
}


// OUT class AnimationConfig defined in src/base/astroconfig.h line 424
Json::Value AnimationConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "interval" ] = writeToJson( interval, jh );
	v[ "mode" ] = writeToJson( mode, jh );
	v[ "stepLength" ] = writeToJson( stepLength, jh );
	v[ "stepDimension" ] = writeToJson( stepDimension, jh );
	return v;
}


// IN class EphemConfig defined in src/base/astroconfig.h line 442
void EphemConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "useEphemerisTime" ] )
	{
		extractFromJson( v[ "useEphemerisTime" ], useEphemerisTime, jh ); 
	}
	if ( v[ "ephemCoordinateSystem" ] )
	{
		extractFromJson( v[ "ephemCoordinateSystem" ], ephemCoordinateSystem, jh ); 
	}
	if ( v[ "useTruePositions" ] )
	{
		extractFromJson( v[ "useTruePositions" ], useTruePositions, jh ); 
	}
	if ( v[ "sunrise_refrac" ] )
	{
		extractFromJson( v[ "sunrise_refrac" ], sunrise_refrac, jh ); 
	}
	if ( v[ "sunrise_def" ] )
	{
		extractFromJson( v[ "sunrise_def" ], sunrise_def, jh ); 
	}
	if ( v[ "sePath" ] )
	{
		extractFromJson( v[ "sePath" ], sePath, jh ); 
	}
}


// OUT class EphemConfig defined in src/base/astroconfig.h line 442
Json::Value EphemConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "useEphemerisTime" ] = writeToJson( useEphemerisTime, jh );
	v[ "ephemCoordinateSystem" ] = writeToJson( ephemCoordinateSystem, jh );
	v[ "useTruePositions" ] = writeToJson( useTruePositions, jh );
	v[ "sunrise_refrac" ] = writeToJson( sunrise_refrac, jh );
	v[ "sunrise_def" ] = writeToJson( sunrise_def, jh );
	v[ "sePath" ] = writeToJson( sePath, jh );
	return v;
}


// IN class CustomAyanamsaConfig defined in src/base/astroconfig.h line 466
void CustomAyanamsaConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "t0" ] )
	{
		extractFromJson( v[ "t0" ], t0, jh ); 
	}
	if ( v[ "ayan_t0" ] )
	{
		extractFromJson( v[ "ayan_t0" ], ayan_t0, jh ); 
	}
	if ( v[ "aya_constant_period" ] )
	{
		extractFromJson( v[ "aya_constant_period" ], aya_constant_period, jh ); 
	}
	if ( v[ "hasConstantPeriod" ] )
	{
		extractFromJson( v[ "hasConstantPeriod" ], hasConstantPeriod, jh ); 
	}
	if ( v[ "t0_ut" ] )
	{
		extractFromJson( v[ "t0_ut" ], t0_ut, jh ); 
	}
	if ( v[ "precessionModel" ] )
	{
		extractFromJson( v[ "precessionModel" ], precessionModel, jh ); 
	}
}


// OUT class CustomAyanamsaConfig defined in src/base/astroconfig.h line 466
Json::Value CustomAyanamsaConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "t0" ] = writeToJson( t0, jh );
	v[ "ayan_t0" ] = writeToJson( ayan_t0, jh );
	v[ "aya_constant_period" ] = writeToJson( aya_constant_period, jh );
	v[ "hasConstantPeriod" ] = writeToJson( hasConstantPeriod, jh );
	v[ "t0_ut" ] = writeToJson( t0_ut, jh );
	v[ "precessionModel" ] = writeToJson( precessionModel, jh );
	return v;
}


// IN class Astroconfig defined in src/base/astroconfig.h line 488
void Astroconfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "general" ] )
	{
		extractFromJson( v[ "general" ], general, jh ); 
	}
	if ( v[ "directions" ] )
	{
		extractFromJson( v[ "directions" ], directions, jh ); 
	}
	if ( v[ "varga" ] )
	{
		extractFromJson( v[ "varga" ], varga, jh ); 
	}
	if ( v[ "dasa" ] )
	{
		extractFromJson( v[ "dasa" ], dasa, jh ); 
	}
	if ( v[ "ashtakavarga" ] )
	{
		extractFromJson( v[ "ashtakavarga" ], ashtakavarga, jh ); 
	}
	if ( v[ "jaimini" ] )
	{
		extractFromJson( v[ "jaimini" ], jaimini, jh ); 
	}
	if ( v[ "kalavela" ] )
	{
		extractFromJson( v[ "kalavela" ], kalavela, jh ); 
	}
	if ( v[ "animation" ] )
	{
		extractFromJson( v[ "animation" ], animation, jh ); 
	}
	if ( v[ "ephem" ] )
	{
		extractFromJson( v[ "ephem" ], ephem, jh ); 
	}
	if ( v[ "customAyanamsa" ] )
	{
		extractFromJson( v[ "customAyanamsa" ], customAyanamsa, jh ); 
	}
	if ( v[ "standardLocation" ] )
	{
		extractFromJson( v[ "standardLocation" ], standardLocation, jh ); 
	}
	if ( v[ "viewpointConfigs" ] )
	{
		extractFromJson( v[ "viewpointConfigs" ], viewpointConfigs, jh ); 
	}
	if ( v[ "objectPropertyConfigs" ] )
	{
		extractFromJson( v[ "objectPropertyConfigs" ], objectPropertyConfigs, jh ); 
	}
	if ( v[ "uranian" ] )
	{
		extractFromJson( v[ "uranian" ], uranian, jh ); 
	}
	if ( v[ "uranianSections" ] )
	{
		extractFromJson( v[ "uranianSections" ], uranianSections, jh ); 
	}
}


// OUT class Astroconfig defined in src/base/astroconfig.h line 488
Json::Value Astroconfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "general" ] = writeToJson( general, jh );
	v[ "directions" ] = writeToJson( directions, jh );
	v[ "varga" ] = writeToJson( varga, jh );
	v[ "dasa" ] = writeToJson( dasa, jh );
	v[ "ashtakavarga" ] = writeToJson( ashtakavarga, jh );
	v[ "jaimini" ] = writeToJson( jaimini, jh );
	v[ "kalavela" ] = writeToJson( kalavela, jh );
	v[ "animation" ] = writeToJson( animation, jh );
	v[ "ephem" ] = writeToJson( ephem, jh );
	v[ "customAyanamsa" ] = writeToJson( customAyanamsa, jh );
	v[ "standardLocation" ] = writeToJson( standardLocation, jh );
	v[ "viewpointConfigs" ] = writeToJson( viewpointConfigs, jh );
	v[ "objectPropertyConfigs" ] = writeToJson( objectPropertyConfigs, jh );
	v[ "uranian" ] = writeToJson( uranian, jh );
	v[ "uranianSections" ] = writeToJson( uranianSections, jh );
	return v;
}


// IN class ChartConfig defined in src/base/ChartConfig.h line 44
void ChartConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "textFormat" ] )
	{
		extractFromJson( v[ "textFormat" ], textFormat, jh ); 
	}
	if ( v[ "retroStyle" ] )
	{
		extractFromJson( v[ "retroStyle" ], retroStyle, jh ); 
	}
	if ( v[ "relativeFontSize" ] )
	{
		extractFromJson( v[ "relativeFontSize" ], relativeFontSize, jh ); 
	}
	if ( v[ "highlightOnMouseOver" ] )
	{
		extractFromJson( v[ "highlightOnMouseOver" ], highlightOnMouseOver, jh ); 
	}
	if ( v[ "zoomOnMouseOver" ] )
	{
		extractFromJson( v[ "zoomOnMouseOver" ], zoomOnMouseOver, jh ); 
	}
	if ( v[ "zoomFactor" ] )
	{
		extractFromJson( v[ "zoomFactor" ], zoomFactor, jh ); 
	}
	if ( v[ "showPlanetColors" ] )
	{
		extractFromJson( v[ "showPlanetColors" ], showPlanetColors, jh ); 
	}
	if ( v[ "showHouses" ] )
	{
		extractFromJson( v[ "showHouses" ], showHouses, jh ); 
	}
	if ( v[ "showSignColors" ] )
	{
		extractFromJson( v[ "showSignColors" ], showSignColors, jh ); 
	}
	if ( v[ "showHouseColors" ] )
	{
		extractFromJson( v[ "showHouseColors" ], showHouseColors, jh ); 
	}
	if ( v[ "chartOrientation" ] )
	{
		extractFromJson( v[ "chartOrientation" ], chartOrientation, jh ); 
	}
	if ( v[ "houseNumberStyle" ] )
	{
		extractFromJson( v[ "houseNumberStyle" ], houseNumberStyle, jh ); 
	}
	if ( v[ "uranianChartOrigin" ] )
	{
		extractFromJson( v[ "uranianChartOrigin" ], uranianChartOrigin, jh ); 
	}
	if ( v[ "showMatchesOnMouseOver" ] )
	{
		extractFromJson( v[ "showMatchesOnMouseOver" ], showMatchesOnMouseOver, jh ); 
	}
	if ( v[ "division" ] )
	{
		extractFromJson( v[ "division" ], division, jh ); 
	}
	if ( v[ "chartCenterType" ] )
	{
		extractFromJson( v[ "chartCenterType" ], chartCenterType, jh ); 
	}
	if ( v[ "showArudhas" ] )
	{
		extractFromJson( v[ "showArudhas" ], showArudhas, jh ); 
	}
	if ( v[ "showAshtakavarga" ] )
	{
		extractFromJson( v[ "showAshtakavarga" ], showAshtakavarga, jh ); 
	}
	if ( v[ "northIndianSignDisplayType" ] )
	{
		extractFromJson( v[ "northIndianSignDisplayType" ], northIndianSignDisplayType, jh ); 
	}
	if ( v[ "southIndianAscendantMarkup" ] )
	{
		extractFromJson( v[ "southIndianAscendantMarkup" ], southIndianAscendantMarkup, jh ); 
	}
	if ( v[ "centerInfoType" ] )
	{
		extractFromJson( v[ "centerInfoType" ], centerInfoType, jh ); 
	}
	if ( v[ "aspectType" ] )
	{
		extractFromJson( v[ "aspectType" ], aspectType, jh ); 
	}
	if ( v[ "grahaDrishtiMinimumStrength" ] )
	{
		extractFromJson( v[ "grahaDrishtiMinimumStrength" ], grahaDrishtiMinimumStrength, jh ); 
	}
	if ( v[ "beneficMaleficDisplay" ] )
	{
		extractFromJson( v[ "beneficMaleficDisplay" ], beneficMaleficDisplay, jh ); 
	}
	if ( v[ "includeAsMc" ] )
	{
		extractFromJson( v[ "includeAsMc" ], includeAsMc, jh ); 
	}
	if ( v[ "showSbcDevanagariCharacters" ] )
	{
		extractFromJson( v[ "showSbcDevanagariCharacters" ], showSbcDevanagariCharacters, jh ); 
	}
	if ( v[ "showSbcNakshatraQuality" ] )
	{
		extractFromJson( v[ "showSbcNakshatraQuality" ], showSbcNakshatraQuality, jh ); 
	}
}


// OUT class ChartConfig defined in src/base/ChartConfig.h line 44
Json::Value ChartConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "textFormat" ] = writeToJson( textFormat, jh );
	v[ "retroStyle" ] = writeToJson( retroStyle, jh );
	v[ "relativeFontSize" ] = writeToJson( relativeFontSize, jh );
	v[ "highlightOnMouseOver" ] = writeToJson( highlightOnMouseOver, jh );
	v[ "zoomOnMouseOver" ] = writeToJson( zoomOnMouseOver, jh );
	v[ "zoomFactor" ] = writeToJson( zoomFactor, jh );
	v[ "showPlanetColors" ] = writeToJson( showPlanetColors, jh );
	v[ "showHouses" ] = writeToJson( showHouses, jh );
	v[ "showSignColors" ] = writeToJson( showSignColors, jh );
	v[ "showHouseColors" ] = writeToJson( showHouseColors, jh );
	v[ "chartOrientation" ] = writeToJson( chartOrientation, jh );
	v[ "houseNumberStyle" ] = writeToJson( houseNumberStyle, jh );
	v[ "uranianChartOrigin" ] = writeToJson( uranianChartOrigin, jh );
	v[ "showMatchesOnMouseOver" ] = writeToJson( showMatchesOnMouseOver, jh );
	v[ "division" ] = writeToJson( division, jh );
	v[ "chartCenterType" ] = writeToJson( chartCenterType, jh );
	v[ "showArudhas" ] = writeToJson( showArudhas, jh );
	v[ "showAshtakavarga" ] = writeToJson( showAshtakavarga, jh );
	v[ "northIndianSignDisplayType" ] = writeToJson( northIndianSignDisplayType, jh );
	v[ "southIndianAscendantMarkup" ] = writeToJson( southIndianAscendantMarkup, jh );
	v[ "centerInfoType" ] = writeToJson( centerInfoType, jh );
	v[ "aspectType" ] = writeToJson( aspectType, jh );
	v[ "grahaDrishtiMinimumStrength" ] = writeToJson( grahaDrishtiMinimumStrength, jh );
	v[ "beneficMaleficDisplay" ] = writeToJson( beneficMaleficDisplay, jh );
	v[ "includeAsMc" ] = writeToJson( includeAsMc, jh );
	v[ "showSbcDevanagariCharacters" ] = writeToJson( showSbcDevanagariCharacters, jh );
	v[ "showSbcNakshatraQuality" ] = writeToJson( showSbcNakshatraQuality, jh );
	return v;
}


// IN class BarDiagramStyle defined in src/base/ColorConfig.h line 56
void BarDiagramStyle::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "pen" ] )
	{
		extractFromJson( v[ "pen" ], pen, jh ); 
	}
	if ( v[ "brush" ] )
	{
		extractFromJson( v[ "brush" ], brush, jh ); 
	}
}


// OUT class BarDiagramStyle defined in src/base/ColorConfig.h line 56
Json::Value BarDiagramStyle::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "pen" ] = writeToJson( pen, jh );
	v[ "brush" ] = writeToJson( brush, jh );
	return v;
}


// IN class BarDiagramConfig defined in src/base/ColorConfig.h line 77
void BarDiagramConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "styles" ] )
	{
		extractFromJson( v[ "styles" ], styles, jh ); 
	}
	if ( v[ "cornerRadius" ] )
	{
		extractFromJson( v[ "cornerRadius" ], cornerRadius, jh ); 
	}
	if ( v[ "doItemZoom" ] )
	{
		extractFromJson( v[ "doItemZoom" ], doItemZoom, jh ); 
	}
	if ( v[ "itemZoom" ] )
	{
		extractFromJson( v[ "itemZoom" ], itemZoom, jh ); 
	}
}


// OUT class BarDiagramConfig defined in src/base/ColorConfig.h line 77
Json::Value BarDiagramConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "styles" ] = writeToJson( styles, jh );
	v[ "cornerRadius" ] = writeToJson( cornerRadius, jh );
	v[ "doItemZoom" ] = writeToJson( doItemZoom, jh );
	v[ "itemZoom" ] = writeToJson( itemZoom, jh );
	return v;
}


// IN class BgPalette defined in src/base/ColorConfig.h line 111
void BgPalette::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "brushes" ] )
	{
		extractFromJson( v[ "brushes" ], brushes, jh ); 
	}
	if ( v[ "textColor" ] )
	{
		extractFromJson( v[ "textColor" ], textColor, jh ); 
	}
}


// OUT class BgPalette defined in src/base/ColorConfig.h line 111
Json::Value BgPalette::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "brushes" ] = writeToJson( brushes, jh );
	v[ "textColor" ] = writeToJson( textColor, jh );
	return v;
}


// IN class ChartFieldPattern defined in src/base/ColorConfig.h line 135
void ChartFieldPattern::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "paletteName" ] )
	{
		extractFromJson( v[ "paletteName" ], paletteName, jh ); 
	}
	if ( v[ "palette" ] )
	{
		extractFromJson( v[ "palette" ], palette, jh ); 
	}
}


// OUT class ChartFieldPattern defined in src/base/ColorConfig.h line 135
Json::Value ChartFieldPattern::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "paletteName" ] = writeToJson( paletteName, jh );
	v[ "palette" ] = writeToJson( palette, jh );
	return v;
}


// IN class ChartColorConfig defined in src/base/ColorConfig.h line 156
void ChartColorConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "signFgColors" ] )
	{
		extractFromJson( v[ "signFgColors" ], signFgColors, jh ); 
	}
	if ( v[ "houseFgColors" ] )
	{
		extractFromJson( v[ "houseFgColors" ], houseFgColors, jh ); 
	}
	if ( v[ "planetFgColors" ] )
	{
		extractFromJson( v[ "planetFgColors" ], planetFgColors, jh ); 
	}
	if ( v[ "signPalette" ] )
	{
		extractFromJson( v[ "signPalette" ], signPalette, jh ); 
	}
	if ( v[ "housePalette" ] )
	{
		extractFromJson( v[ "housePalette" ], housePalette, jh ); 
	}
}


// OUT class ChartColorConfig defined in src/base/ColorConfig.h line 156
Json::Value ChartColorConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "signFgColors" ] = writeToJson( signFgColors, jh );
	v[ "houseFgColors" ] = writeToJson( houseFgColors, jh );
	v[ "planetFgColors" ] = writeToJson( planetFgColors, jh );
	v[ "signPalette" ] = writeToJson( signPalette, jh );
	v[ "housePalette" ] = writeToJson( housePalette, jh );
	return v;
}


// IN class ColorConfig defined in src/base/ColorConfig.h line 211
void ColorConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "defaultPen" ] )
	{
		extractFromJson( v[ "defaultPen" ], defaultPen, jh ); 
	}
	if ( v[ "bgColor" ] )
	{
		extractFromJson( v[ "bgColor" ], bgColor, jh ); 
	}
	if ( v[ "textFgColor" ] )
	{
		extractFromJson( v[ "textFgColor" ], textFgColor, jh ); 
	}
	if ( v[ "markedFieldsBgColor" ] )
	{
		extractFromJson( v[ "markedFieldsBgColor" ], markedFieldsBgColor, jh ); 
	}
	if ( v[ "beneficFgColor" ] )
	{
		extractFromJson( v[ "beneficFgColor" ], beneficFgColor, jh ); 
	}
	if ( v[ "maleficFgColor" ] )
	{
		extractFromJson( v[ "maleficFgColor" ], maleficFgColor, jh ); 
	}
	if ( v[ "transitFgColor" ] )
	{
		extractFromJson( v[ "transitFgColor" ], transitFgColor, jh ); 
	}
	if ( v[ "errorBgColor" ] )
	{
		extractFromJson( v[ "errorBgColor" ], errorBgColor, jh ); 
	}
	if ( v[ "chartMarkupBgColor" ] )
	{
		extractFromJson( v[ "chartMarkupBgColor" ], chartMarkupBgColor, jh ); 
	}
}


// OUT class ColorConfig defined in src/base/ColorConfig.h line 211
Json::Value ColorConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "defaultPen" ] = writeToJson( defaultPen, jh );
	v[ "bgColor" ] = writeToJson( bgColor, jh );
	v[ "textFgColor" ] = writeToJson( textFgColor, jh );
	v[ "markedFieldsBgColor" ] = writeToJson( markedFieldsBgColor, jh );
	v[ "beneficFgColor" ] = writeToJson( beneficFgColor, jh );
	v[ "maleficFgColor" ] = writeToJson( maleficFgColor, jh );
	v[ "transitFgColor" ] = writeToJson( transitFgColor, jh );
	v[ "errorBgColor" ] = writeToJson( errorBgColor, jh );
	v[ "chartMarkupBgColor" ] = writeToJson( chartMarkupBgColor, jh );
	return v;
}


// IN class DasaConfig defined in src/base/ConfigListItems.h line 53
void DasaConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "description" ] )
	{
		extractFromJson( v[ "description" ], description, jh ); 
	}
	if ( v[ "effect" ] )
	{
		extractFromJson( v[ "effect" ], effect, jh ); 
	}
	if ( v[ "source" ] )
	{
		extractFromJson( v[ "source" ], source, jh ); 
	}
	if ( v[ "sourcefile" ] )
	{
		extractFromJson( v[ "sourcefile" ], sourcefile, jh ); 
	}
	if ( v[ "lord" ] )
	{
		extractFromJson( v[ "lord" ], lord, jh ); 
	}
	if ( v[ "sublord" ] )
	{
		extractFromJson( v[ "sublord" ], sublord, jh ); 
	}
	if ( v[ "subsublord" ] )
	{
		extractFromJson( v[ "subsublord" ], subsublord, jh ); 
	}
	if ( v[ "isvalid" ] )
	{
		extractFromJson( v[ "isvalid" ], isvalid, jh ); 
	}
	if ( v[ "dasaId" ] )
	{
		extractFromJson( v[ "dasaId" ], dasaId, jh ); 
	}
}


// OUT class DasaConfig defined in src/base/ConfigListItems.h line 53
Json::Value DasaConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "description" ] = writeToJson( description, jh );
	v[ "effect" ] = writeToJson( effect, jh );
	v[ "source" ] = writeToJson( source, jh );
	v[ "sourcefile" ] = writeToJson( sourcefile, jh );
	v[ "lord" ] = writeToJson( lord, jh );
	v[ "sublord" ] = writeToJson( sublord, jh );
	v[ "subsublord" ] = writeToJson( subsublord, jh );
	v[ "isvalid" ] = writeToJson( isvalid, jh );
	v[ "dasaId" ] = writeToJson( dasaId, jh );
	return v;
}


// IN class TableStyle defined in src/base/ConfigListItems.h line 76
void TableStyle::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "name" ] )
	{
		extractFromJson( v[ "name" ], name, jh ); 
	}
	if ( v[ "useHeaderColors" ] )
	{
		extractFromJson( v[ "useHeaderColors" ], useHeaderColors, jh ); 
	}
	if ( v[ "headerTextColor" ] )
	{
		extractFromJson( v[ "headerTextColor" ], headerTextColor, jh ); 
	}
	if ( v[ "headerBgColor" ] )
	{
		extractFromJson( v[ "headerBgColor" ], headerBgColor, jh ); 
	}
	if ( v[ "useCellColors" ] )
	{
		extractFromJson( v[ "useCellColors" ], useCellColors, jh ); 
	}
	if ( v[ "cellTextColor" ] )
	{
		extractFromJson( v[ "cellTextColor" ], cellTextColor, jh ); 
	}
	if ( v[ "cellBgMode" ] )
	{
		extractFromJson( v[ "cellBgMode" ], cellBgMode, jh ); 
	}
	if ( v[ "allCellBgColor" ] )
	{
		extractFromJson( v[ "allCellBgColor" ], allCellBgColor, jh ); 
	}
	if ( v[ "oddCellBgColor" ] )
	{
		extractFromJson( v[ "oddCellBgColor" ], oddCellBgColor, jh ); 
	}
	if ( v[ "evenCellBgColor" ] )
	{
		extractFromJson( v[ "evenCellBgColor" ], evenCellBgColor, jh ); 
	}
	if ( v[ "showGrid" ] )
	{
		extractFromJson( v[ "showGrid" ], showGrid, jh ); 
	}
}


// OUT class TableStyle defined in src/base/ConfigListItems.h line 76
Json::Value TableStyle::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "name" ] = writeToJson( name, jh );
	v[ "useHeaderColors" ] = writeToJson( useHeaderColors, jh );
	v[ "headerTextColor" ] = writeToJson( headerTextColor, jh );
	v[ "headerBgColor" ] = writeToJson( headerBgColor, jh );
	v[ "useCellColors" ] = writeToJson( useCellColors, jh );
	v[ "cellTextColor" ] = writeToJson( cellTextColor, jh );
	v[ "cellBgMode" ] = writeToJson( cellBgMode, jh );
	v[ "allCellBgColor" ] = writeToJson( allCellBgColor, jh );
	v[ "oddCellBgColor" ] = writeToJson( oddCellBgColor, jh );
	v[ "evenCellBgColor" ] = writeToJson( evenCellBgColor, jh );
	v[ "showGrid" ] = writeToJson( showGrid, jh );
	return v;
}


// IN class WidgetSkin defined in src/base/ConfigListItems.h line 113
void WidgetSkin::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "name" ] )
	{
		extractFromJson( v[ "name" ], name, jh ); 
	}
	if ( v[ "pen" ] )
	{
		extractFromJson( v[ "pen" ], pen, jh ); 
	}
	if ( v[ "brush" ] )
	{
		extractFromJson( v[ "brush" ], brush, jh ); 
	}
	if ( v[ "textColor" ] )
	{
		extractFromJson( v[ "textColor" ], textColor, jh ); 
	}
	if ( v[ "markedFieldBrush" ] )
	{
		extractFromJson( v[ "markedFieldBrush" ], markedFieldBrush, jh ); 
	}
	if ( v[ "signPattern" ] )
	{
		extractFromJson( v[ "signPattern" ], signPattern, jh ); 
	}
	if ( v[ "housePattern" ] )
	{
		extractFromJson( v[ "housePattern" ], housePattern, jh ); 
	}
	if ( v[ "nakshatraPattern" ] )
	{
		extractFromJson( v[ "nakshatraPattern" ], nakshatraPattern, jh ); 
	}
}


// OUT class WidgetSkin defined in src/base/ConfigListItems.h line 113
Json::Value WidgetSkin::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "name" ] = writeToJson( name, jh );
	v[ "pen" ] = writeToJson( pen, jh );
	v[ "brush" ] = writeToJson( brush, jh );
	v[ "textColor" ] = writeToJson( textColor, jh );
	v[ "markedFieldBrush" ] = writeToJson( markedFieldBrush, jh );
	v[ "signPattern" ] = writeToJson( signPattern, jh );
	v[ "housePattern" ] = writeToJson( housePattern, jh );
	v[ "nakshatraPattern" ] = writeToJson( nakshatraPattern, jh );
	return v;
}


// IN class YogaConfig defined in src/base/ConfigListItems.h line 145
void YogaConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "description" ] )
	{
		extractFromJson( v[ "description" ], description, jh ); 
	}
	if ( v[ "group" ] )
	{
		extractFromJson( v[ "group" ], group, jh ); 
	}
	if ( v[ "effect" ] )
	{
		extractFromJson( v[ "effect" ], effect, jh ); 
	}
	if ( v[ "source" ] )
	{
		extractFromJson( v[ "source" ], source, jh ); 
	}
	if ( v[ "rule" ] )
	{
		extractFromJson( v[ "rule" ], rule, jh ); 
	}
}


// OUT class YogaConfig defined in src/base/ConfigListItems.h line 145
Json::Value YogaConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "description" ] = writeToJson( description, jh );
	v[ "group" ] = writeToJson( group, jh );
	v[ "effect" ] = writeToJson( effect, jh );
	v[ "source" ] = writeToJson( source, jh );
	v[ "rule" ] = writeToJson( rule, jh );
	return v;
}


// IN class PageMarginal defined in src/base/PrintoutConfig.h line 74
void PageMarginal::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "enabled" ] )
	{
		extractFromJson( v[ "enabled" ], enabled, jh ); 
	}
	if ( v[ "text" ] )
	{
		extractFromJson( v[ "text" ], text, jh ); 
	}
	if ( v[ "showSeparator" ] )
	{
		extractFromJson( v[ "showSeparator" ], showSeparator, jh ); 
	}
	if ( v[ "orientation" ] )
	{
		extractFromJson( v[ "orientation" ], orientation, jh ); 
	}
	if ( v[ "showOnFirstPage" ] )
	{
		extractFromJson( v[ "showOnFirstPage" ], showOnFirstPage, jh ); 
	}
}


// OUT class PageMarginal defined in src/base/PrintoutConfig.h line 74
Json::Value PageMarginal::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "enabled" ] = writeToJson( enabled, jh );
	v[ "text" ] = writeToJson( text, jh );
	v[ "showSeparator" ] = writeToJson( showSeparator, jh );
	v[ "orientation" ] = writeToJson( orientation, jh );
	v[ "showOnFirstPage" ] = writeToJson( showOnFirstPage, jh );
	return v;
}


// IN class PageConfig defined in src/base/PrintoutConfig.h line 101
void PageConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "paperSize" ] )
	{
		extractFromJson( v[ "paperSize" ], paperSize, jh ); 
	}
	if ( v[ "horizontalMargin" ] )
	{
		extractFromJson( v[ "horizontalMargin" ], horizontalMargin, jh ); 
	}
	if ( v[ "verticalMargin" ] )
	{
		extractFromJson( v[ "verticalMargin" ], verticalMargin, jh ); 
	}
	if ( v[ "basicFontSize" ] )
	{
		extractFromJson( v[ "basicFontSize" ], basicFontSize, jh ); 
	}
	if ( v[ "header" ] )
	{
		extractFromJson( v[ "header" ], header, jh ); 
	}
	if ( v[ "footer" ] )
	{
		extractFromJson( v[ "footer" ], footer, jh ); 
	}
}


// OUT class PageConfig defined in src/base/PrintoutConfig.h line 101
Json::Value PageConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "paperSize" ] = writeToJson( paperSize, jh );
	v[ "horizontalMargin" ] = writeToJson( horizontalMargin, jh );
	v[ "verticalMargin" ] = writeToJson( verticalMargin, jh );
	v[ "basicFontSize" ] = writeToJson( basicFontSize, jh );
	v[ "header" ] = writeToJson( header, jh );
	v[ "footer" ] = writeToJson( footer, jh );
	return v;
}


// IN class PdfConfig defined in src/base/PrintoutConfig.h line 126
void PdfConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "askOverwrite" ] )
	{
		extractFromJson( v[ "askOverwrite" ], askOverwrite, jh ); 
	}
	if ( v[ "compressPdf" ] )
	{
		extractFromJson( v[ "compressPdf" ], compressPdf, jh ); 
	}
	if ( v[ "launchPdfViewer" ] )
	{
		extractFromJson( v[ "launchPdfViewer" ], launchPdfViewer, jh ); 
	}
	if ( v[ "pdfViewerCommand" ] )
	{
		extractFromJson( v[ "pdfViewerCommand" ], pdfViewerCommand, jh ); 
	}
}


// OUT class PdfConfig defined in src/base/PrintoutConfig.h line 126
Json::Value PdfConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "askOverwrite" ] = writeToJson( askOverwrite, jh );
	v[ "compressPdf" ] = writeToJson( compressPdf, jh );
	v[ "launchPdfViewer" ] = writeToJson( launchPdfViewer, jh );
	v[ "pdfViewerCommand" ] = writeToJson( pdfViewerCommand, jh );
	return v;
}


// IN class PrintoutConfig defined in src/base/PrintoutConfig.h line 146
void PrintoutConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "sheetId" ] )
	{
		extractFromJson( v[ "sheetId" ], sheetId, jh ); 
	}
	if ( v[ "skins" ] )
	{
		extractFromJson( v[ "skins" ], skins, jh ); 
	}
	if ( v[ "geoms" ] )
	{
		extractFromJson( v[ "geoms" ], geoms, jh ); 
	}
	if ( v[ "tableStyleId" ] )
	{
		extractFromJson( v[ "tableStyleId" ], tableStyleId, jh ); 
	}
}


// OUT class PrintoutConfig defined in src/base/PrintoutConfig.h line 146
Json::Value PrintoutConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "sheetId" ] = writeToJson( sheetId, jh );
	v[ "skins" ] = writeToJson( skins, jh );
	v[ "geoms" ] = writeToJson( geoms, jh );
	v[ "tableStyleId" ] = writeToJson( tableStyleId, jh );
	return v;
}


// IN class ColumnGroupDefinition defined in src/base/Sheet.h line 210
void ColumnGroupDefinition::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "autoHeader" ] )
	{
		extractFromJson( v[ "autoHeader" ], autoHeader, jh ); 
	}
	if ( v[ "commonHeader" ] )
	{
		extractFromJson( v[ "commonHeader" ], commonHeader, jh ); 
	}
	if ( v[ "planets" ] )
	{
		extractFromJson( v[ "planets" ], planets, jh ); 
	}
	if ( v[ "columnGroups" ] )
	{
		extractFromJson( v[ "columnGroups" ], columnGroups, jh ); 
	}
	if ( v[ "basedata" ] )
	{
		extractFromJson( v[ "basedata" ], basedata, jh ); 
	}
}


// OUT class ColumnGroupDefinition defined in src/base/Sheet.h line 210
Json::Value ColumnGroupDefinition::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "autoHeader" ] = writeToJson( autoHeader, jh );
	v[ "commonHeader" ] = writeToJson( commonHeader, jh );
	v[ "planets" ] = writeToJson( planets, jh );
	v[ "columnGroups" ] = writeToJson( columnGroups, jh );
	v[ "basedata" ] = writeToJson( basedata, jh );
	return v;
}


// IN class ToolbarConfig defined in src/base/gdiconfig.h line 57
void ToolbarConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "mainToolbarItems" ] )
	{
		extractFromJson( v[ "mainToolbarItems" ], mainToolbarItems, jh ); 
	}
	if ( v[ "toolbarStyle" ] )
	{
		extractFromJson( v[ "toolbarStyle" ], toolbarStyle, jh ); 
	}
}


// OUT class ToolbarConfig defined in src/base/gdiconfig.h line 57
Json::Value ToolbarConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "mainToolbarItems" ] = writeToJson( mainToolbarItems, jh );
	v[ "toolbarStyle" ] = writeToJson( toolbarStyle, jh );
	return v;
}


// IN class ViewPreferencesConfig defined in src/base/gdiconfig.h line 75
void ViewPreferencesConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "widgetSize" ] )
	{
		extractFromJson( v[ "widgetSize" ], widgetSize, jh ); 
	}
	if ( v[ "widgetPosition" ] )
	{
		extractFromJson( v[ "widgetPosition" ], widgetPosition, jh ); 
	}
	if ( v[ "activeNotebookTab" ] )
	{
		extractFromJson( v[ "activeNotebookTab" ], activeNotebookTab, jh ); 
	}
	if ( v[ "splitterSashPos" ] )
	{
		extractFromJson( v[ "splitterSashPos" ], splitterSashPos, jh ); 
	}
	if ( v[ "doSplitDasaTree" ] )
	{
		extractFromJson( v[ "doSplitDasaTree" ], doSplitDasaTree, jh ); 
	}
	if ( v[ "dasaTreeTextViewWidth" ] )
	{
		extractFromJson( v[ "dasaTreeTextViewWidth" ], dasaTreeTextViewWidth, jh ); 
	}
	if ( v[ "textWindowPage" ] )
	{
		extractFromJson( v[ "textWindowPage" ], textWindowPage, jh ); 
	}
	if ( v[ "doNotShowAgainTopics" ] )
	{
		extractFromJson( v[ "doNotShowAgainTopics" ], doNotShowAgainTopics, jh ); 
	}
	if ( v[ "listStyle" ] )
	{
		extractFromJson( v[ "listStyle" ], listStyle, jh ); 
	}
	if ( v[ "showMainToolbar" ] )
	{
		extractFromJson( v[ "showMainToolbar" ], showMainToolbar, jh ); 
	}
	if ( v[ "showStatusBar" ] )
	{
		extractFromJson( v[ "showStatusBar" ], showStatusBar, jh ); 
	}
	if ( v[ "showLogWindow" ] )
	{
		extractFromJson( v[ "showLogWindow" ], showLogWindow, jh ); 
	}
	if ( v[ "dasaChartAntarDasaLevel" ] )
	{
		extractFromJson( v[ "dasaChartAntarDasaLevel" ], dasaChartAntarDasaLevel, jh ); 
	}
	if ( v[ "dasaChartSelectedDasas" ] )
	{
		extractFromJson( v[ "dasaChartSelectedDasas" ], dasaChartSelectedDasas, jh ); 
	}
	if ( v[ "yogaSourceFilter" ] )
	{
		extractFromJson( v[ "yogaSourceFilter" ], yogaSourceFilter, jh ); 
	}
	if ( v[ "yogaGroupFilter" ] )
	{
		extractFromJson( v[ "yogaGroupFilter" ], yogaGroupFilter, jh ); 
	}
	if ( v[ "yogaCustomFilter" ] )
	{
		extractFromJson( v[ "yogaCustomFilter" ], yogaCustomFilter, jh ); 
	}
	if ( v[ "directionMode" ] )
	{
		extractFromJson( v[ "directionMode" ], directionMode, jh ); 
	}
	if ( v[ "transitIsLocalTime" ] )
	{
		extractFromJson( v[ "transitIsLocalTime" ], transitIsLocalTime, jh ); 
	}
	if ( v[ "multitransitIsLocalTime" ] )
	{
		extractFromJson( v[ "multitransitIsLocalTime" ], multitransitIsLocalTime, jh ); 
	}
	if ( v[ "divisionSpWidget" ] )
	{
		extractFromJson( v[ "divisionSpWidget" ], divisionSpWidget, jh ); 
	}
	if ( v[ "termSpWidget" ] )
	{
		extractFromJson( v[ "termSpWidget" ], termSpWidget, jh ); 
	}
	if ( v[ "modeSpWidget" ] )
	{
		extractFromJson( v[ "modeSpWidget" ], modeSpWidget, jh ); 
	}
	if ( v[ "periodVWidgetDurationChoice" ] )
	{
		extractFromJson( v[ "periodVWidgetDurationChoice" ], periodVWidgetDurationChoice, jh ); 
	}
	if ( v[ "ephemTimezone" ] )
	{
		extractFromJson( v[ "ephemTimezone" ], ephemTimezone, jh ); 
	}
	if ( v[ "coordinateModeEphem" ] )
	{
		extractFromJson( v[ "coordinateModeEphem" ], coordinateModeEphem, jh ); 
	}
	if ( v[ "divisionEphem" ] )
	{
		extractFromJson( v[ "divisionEphem" ], divisionEphem, jh ); 
	}
	if ( v[ "eclipseTimezone" ] )
	{
		extractFromJson( v[ "eclipseTimezone" ], eclipseTimezone, jh ); 
	}
	if ( v[ "horaTimezone" ] )
	{
		extractFromJson( v[ "horaTimezone" ], horaTimezone, jh ); 
	}
	if ( v[ "printoutConfletSelectedCategory" ] )
	{
		extractFromJson( v[ "printoutConfletSelectedCategory" ], printoutConfletSelectedCategory, jh ); 
	}
	if ( v[ "chartPreviewShowTransits" ] )
	{
		extractFromJson( v[ "chartPreviewShowTransits" ], chartPreviewShowTransits, jh ); 
	}
	if ( v[ "chartPreviewDivision" ] )
	{
		extractFromJson( v[ "chartPreviewDivision" ], chartPreviewDivision, jh ); 
	}
	if ( v[ "recentFiles" ] )
	{
		extractFromJson( v[ "recentFiles" ], recentFiles, jh ); 
	}
	if ( v[ "defOpenPath" ] )
	{
		extractFromJson( v[ "defOpenPath" ], defOpenPath, jh ); 
	}
	if ( v[ "defSavePath" ] )
	{
		extractFromJson( v[ "defSavePath" ], defSavePath, jh ); 
	}
	if ( v[ "defExportPath" ] )
	{
		extractFromJson( v[ "defExportPath" ], defExportPath, jh ); 
	}
	if ( v[ "defPdfSavePath" ] )
	{
		extractFromJson( v[ "defPdfSavePath" ], defPdfSavePath, jh ); 
	}
	if ( v[ "lastYogaConfigFile" ] )
	{
		extractFromJson( v[ "lastYogaConfigFile" ], lastYogaConfigFile, jh ); 
	}
	if ( v[ "yogaEditorOpenPath" ] )
	{
		extractFromJson( v[ "yogaEditorOpenPath" ], yogaEditorOpenPath, jh ); 
	}
	if ( v[ "yogaEditorSavePath" ] )
	{
		extractFromJson( v[ "yogaEditorSavePath" ], yogaEditorSavePath, jh ); 
	}
}


// OUT class ViewPreferencesConfig defined in src/base/gdiconfig.h line 75
Json::Value ViewPreferencesConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "widgetSize" ] = writeToJson( widgetSize, jh );
	v[ "widgetPosition" ] = writeToJson( widgetPosition, jh );
	v[ "activeNotebookTab" ] = writeToJson( activeNotebookTab, jh );
	v[ "splitterSashPos" ] = writeToJson( splitterSashPos, jh );
	v[ "doSplitDasaTree" ] = writeToJson( doSplitDasaTree, jh );
	v[ "dasaTreeTextViewWidth" ] = writeToJson( dasaTreeTextViewWidth, jh );
	v[ "textWindowPage" ] = writeToJson( textWindowPage, jh );
	v[ "doNotShowAgainTopics" ] = writeToJson( doNotShowAgainTopics, jh );
	v[ "listStyle" ] = writeToJson( listStyle, jh );
	v[ "showMainToolbar" ] = writeToJson( showMainToolbar, jh );
	v[ "showStatusBar" ] = writeToJson( showStatusBar, jh );
	v[ "showLogWindow" ] = writeToJson( showLogWindow, jh );
	v[ "dasaChartAntarDasaLevel" ] = writeToJson( dasaChartAntarDasaLevel, jh );
	v[ "dasaChartSelectedDasas" ] = writeToJson( dasaChartSelectedDasas, jh );
	v[ "yogaSourceFilter" ] = writeToJson( yogaSourceFilter, jh );
	v[ "yogaGroupFilter" ] = writeToJson( yogaGroupFilter, jh );
	v[ "yogaCustomFilter" ] = writeToJson( yogaCustomFilter, jh );
	v[ "directionMode" ] = writeToJson( directionMode, jh );
	v[ "transitIsLocalTime" ] = writeToJson( transitIsLocalTime, jh );
	v[ "multitransitIsLocalTime" ] = writeToJson( multitransitIsLocalTime, jh );
	v[ "divisionSpWidget" ] = writeToJson( divisionSpWidget, jh );
	v[ "termSpWidget" ] = writeToJson( termSpWidget, jh );
	v[ "modeSpWidget" ] = writeToJson( modeSpWidget, jh );
	v[ "periodVWidgetDurationChoice" ] = writeToJson( periodVWidgetDurationChoice, jh );
	v[ "ephemTimezone" ] = writeToJson( ephemTimezone, jh );
	v[ "coordinateModeEphem" ] = writeToJson( coordinateModeEphem, jh );
	v[ "divisionEphem" ] = writeToJson( divisionEphem, jh );
	v[ "eclipseTimezone" ] = writeToJson( eclipseTimezone, jh );
	v[ "horaTimezone" ] = writeToJson( horaTimezone, jh );
	v[ "printoutConfletSelectedCategory" ] = writeToJson( printoutConfletSelectedCategory, jh );
	v[ "chartPreviewShowTransits" ] = writeToJson( chartPreviewShowTransits, jh );
	v[ "chartPreviewDivision" ] = writeToJson( chartPreviewDivision, jh );
	v[ "recentFiles" ] = writeToJson( recentFiles, jh );
	v[ "defOpenPath" ] = writeToJson( defOpenPath, jh );
	v[ "defSavePath" ] = writeToJson( defSavePath, jh );
	v[ "defExportPath" ] = writeToJson( defExportPath, jh );
	v[ "defPdfSavePath" ] = writeToJson( defPdfSavePath, jh );
	v[ "lastYogaConfigFile" ] = writeToJson( lastYogaConfigFile, jh );
	v[ "yogaEditorOpenPath" ] = writeToJson( yogaEditorOpenPath, jh );
	v[ "yogaEditorSavePath" ] = writeToJson( yogaEditorSavePath, jh );
	return v;
}


// IN class ViewConfig defined in src/base/gdiconfig.h line 167
void ViewConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "doHandleResponsiveWidgetEvents" ] )
	{
		extractFromJson( v[ "doHandleResponsiveWidgetEvents" ], doHandleResponsiveWidgetEvents, jh ); 
	}
	if ( v[ "showStatusInfo" ] )
	{
		extractFromJson( v[ "showStatusInfo" ], showStatusInfo, jh ); 
	}
	if ( v[ "childViewFrameWidth" ] )
	{
		extractFromJson( v[ "childViewFrameWidth" ], childViewFrameWidth, jh ); 
	}
	if ( v[ "showTextViewHeaders" ] )
	{
		extractFromJson( v[ "showTextViewHeaders" ], showTextViewHeaders, jh ); 
	}
	if ( v[ "textWidgetUseGdiNative" ] )
	{
		extractFromJson( v[ "textWidgetUseGdiNative" ], textWidgetUseGdiNative, jh ); 
	}
	if ( v[ "defOpenFiletype" ] )
	{
		extractFromJson( v[ "defOpenFiletype" ], defOpenFiletype, jh ); 
	}
	if ( v[ "exportAskOnOverwrite" ] )
	{
		extractFromJson( v[ "exportAskOnOverwrite" ], exportAskOnOverwrite, jh ); 
	}
	if ( v[ "graphicExportSizeMode" ] )
	{
		extractFromJson( v[ "graphicExportSizeMode" ], graphicExportSizeMode, jh ); 
	}
	if ( v[ "validatorFeedbackOnError" ] )
	{
		extractFromJson( v[ "validatorFeedbackOnError" ], validatorFeedbackOnError, jh ); 
	}
	if ( v[ "backupFileMode" ] )
	{
		extractFromJson( v[ "backupFileMode" ], backupFileMode, jh ); 
	}
	if ( v[ "maxRecentFiles" ] )
	{
		extractFromJson( v[ "maxRecentFiles" ], maxRecentFiles, jh ); 
	}
	if ( v[ "uiLogLevel" ] )
	{
		extractFromJson( v[ "uiLogLevel" ], uiLogLevel, jh ); 
	}
	if ( v[ "enableUiNotification" ] )
	{
		extractFromJson( v[ "enableUiNotification" ], enableUiNotification, jh ); 
	}
	if ( v[ "uiNotificationTimeout" ] )
	{
		extractFromJson( v[ "uiNotificationTimeout" ], uiNotificationTimeout, jh ); 
	}
	if ( v[ "uiNotificationLogLevel" ] )
	{
		extractFromJson( v[ "uiNotificationLogLevel" ], uiNotificationLogLevel, jh ); 
	}
	if ( v[ "enableStdoutLogging" ] )
	{
		extractFromJson( v[ "enableStdoutLogging" ], enableStdoutLogging, jh ); 
	}
	if ( v[ "stdoutLogLevel" ] )
	{
		extractFromJson( v[ "stdoutLogLevel" ], stdoutLogLevel, jh ); 
	}
	if ( v[ "jd" ] )
	{
		extractFromJson( v[ "jd" ], jd, jh ); 
	}
	if ( v[ "time" ] )
	{
		extractFromJson( v[ "time" ], time, jh ); 
	}
}


// OUT class ViewConfig defined in src/base/gdiconfig.h line 167
Json::Value ViewConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "doHandleResponsiveWidgetEvents" ] = writeToJson( doHandleResponsiveWidgetEvents, jh );
	v[ "showStatusInfo" ] = writeToJson( showStatusInfo, jh );
	v[ "childViewFrameWidth" ] = writeToJson( childViewFrameWidth, jh );
	v[ "showTextViewHeaders" ] = writeToJson( showTextViewHeaders, jh );
	v[ "textWidgetUseGdiNative" ] = writeToJson( textWidgetUseGdiNative, jh );
	v[ "defOpenFiletype" ] = writeToJson( defOpenFiletype, jh );
	v[ "exportAskOnOverwrite" ] = writeToJson( exportAskOnOverwrite, jh );
	v[ "graphicExportSizeMode" ] = writeToJson( graphicExportSizeMode, jh );
	v[ "validatorFeedbackOnError" ] = writeToJson( validatorFeedbackOnError, jh );
	v[ "backupFileMode" ] = writeToJson( backupFileMode, jh );
	v[ "maxRecentFiles" ] = writeToJson( maxRecentFiles, jh );
	v[ "uiLogLevel" ] = writeToJson( uiLogLevel, jh );
	v[ "enableUiNotification" ] = writeToJson( enableUiNotification, jh );
	v[ "uiNotificationTimeout" ] = writeToJson( uiNotificationTimeout, jh );
	v[ "uiNotificationLogLevel" ] = writeToJson( uiNotificationLogLevel, jh );
	v[ "enableStdoutLogging" ] = writeToJson( enableStdoutLogging, jh );
	v[ "stdoutLogLevel" ] = writeToJson( stdoutLogLevel, jh );
	v[ "jd" ] = writeToJson( jd, jh );
	v[ "time" ] = writeToJson( time, jh );
	return v;
}


// IN class FontConfig defined in src/base/gdiconfig.h line 223
void FontConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "standardFontFacename" ] )
	{
		extractFromJson( v[ "standardFontFacename" ], standardFontFacename, jh ); 
	}
	if ( v[ "fixedFontFacename" ] )
	{
		extractFromJson( v[ "fixedFontFacename" ], fixedFontFacename, jh ); 
	}
	if ( v[ "textFontSize" ] )
	{
		extractFromJson( v[ "textFontSize" ], textFontSize, jh ); 
	}
	if ( v[ "graphicFontFacename" ] )
	{
		extractFromJson( v[ "graphicFontFacename" ], graphicFontFacename, jh ); 
	}
	if ( v[ "graphicFontSize" ] )
	{
		extractFromJson( v[ "graphicFontSize" ], graphicFontSize, jh ); 
	}
	if ( v[ "symbolFontFacename" ] )
	{
		extractFromJson( v[ "symbolFontFacename" ], symbolFontFacename, jh ); 
	}
	if ( v[ "symbolFontSize" ] )
	{
		extractFromJson( v[ "symbolFontSize" ], symbolFontSize, jh ); 
	}
}


// OUT class FontConfig defined in src/base/gdiconfig.h line 223
Json::Value FontConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "standardFontFacename" ] = writeToJson( standardFontFacename, jh );
	v[ "fixedFontFacename" ] = writeToJson( fixedFontFacename, jh );
	v[ "textFontSize" ] = writeToJson( textFontSize, jh );
	v[ "graphicFontFacename" ] = writeToJson( graphicFontFacename, jh );
	v[ "graphicFontSize" ] = writeToJson( graphicFontSize, jh );
	v[ "symbolFontFacename" ] = writeToJson( symbolFontFacename, jh );
	v[ "symbolFontSize" ] = writeToJson( symbolFontSize, jh );
	return v;
}


// IN class AtlasConfig defined in src/base/gdiconfig.h line 246
void AtlasConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "databaseFile" ] )
	{
		extractFromJson( v[ "databaseFile" ], databaseFile, jh ); 
	}
	if ( v[ "sqlFile" ] )
	{
		extractFromJson( v[ "sqlFile" ], sqlFile, jh ); 
	}
	if ( v[ "favouriteCountries" ] )
	{
		extractFromJson( v[ "favouriteCountries" ], favouriteCountries, jh ); 
	}
	if ( v[ "filterMode" ] )
	{
		extractFromJson( v[ "filterMode" ], filterMode, jh ); 
	}
	if ( v[ "lastSelectedCountry" ] )
	{
		extractFromJson( v[ "lastSelectedCountry" ], lastSelectedCountry, jh ); 
	}
	if ( v[ "gridColumnSizes" ] )
	{
		extractFromJson( v[ "gridColumnSizes" ], gridColumnSizes, jh ); 
	}
	if ( v[ "filterCaseSensitive" ] )
	{
		extractFromJson( v[ "filterCaseSensitive" ], filterCaseSensitive, jh ); 
	}
	if ( v[ "cacheSize" ] )
	{
		extractFromJson( v[ "cacheSize" ], cacheSize, jh ); 
	}
}


// OUT class AtlasConfig defined in src/base/gdiconfig.h line 246
Json::Value AtlasConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "databaseFile" ] = writeToJson( databaseFile, jh );
	v[ "sqlFile" ] = writeToJson( sqlFile, jh );
	v[ "favouriteCountries" ] = writeToJson( favouriteCountries, jh );
	v[ "filterMode" ] = writeToJson( filterMode, jh );
	v[ "lastSelectedCountry" ] = writeToJson( lastSelectedCountry, jh );
	v[ "gridColumnSizes" ] = writeToJson( gridColumnSizes, jh );
	v[ "filterCaseSensitive" ] = writeToJson( filterCaseSensitive, jh );
	v[ "cacheSize" ] = writeToJson( cacheSize, jh );
	return v;
}


// IN class MultipleViewConfig defined in src/base/gdiconfig.h line 270
void MultipleViewConfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "useMultipleViews" ] )
	{
		extractFromJson( v[ "useMultipleViews" ], useMultipleViews, jh ); 
	}
	if ( v[ "defaultView" ] )
	{
		extractFromJson( v[ "defaultView" ], defaultView, jh ); 
	}
	if ( v[ "notebookStyle" ] )
	{
		extractFromJson( v[ "notebookStyle" ], notebookStyle, jh ); 
	}
	if ( v[ "notebookOrientation" ] )
	{
		extractFromJson( v[ "notebookOrientation" ], notebookOrientation, jh ); 
	}
	if ( v[ "radixItems" ] )
	{
		extractFromJson( v[ "radixItems" ], radixItems, jh ); 
	}
	if ( v[ "directionItems" ] )
	{
		extractFromJson( v[ "directionItems" ], directionItems, jh ); 
	}
	if ( v[ "multiDirectionItems" ] )
	{
		extractFromJson( v[ "multiDirectionItems" ], multiDirectionItems, jh ); 
	}
	if ( v[ "solarItems" ] )
	{
		extractFromJson( v[ "solarItems" ], solarItems, jh ); 
	}
	if ( v[ "partnerItems" ] )
	{
		extractFromJson( v[ "partnerItems" ], partnerItems, jh ); 
	}
}


// OUT class MultipleViewConfig defined in src/base/gdiconfig.h line 270
Json::Value MultipleViewConfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "useMultipleViews" ] = writeToJson( useMultipleViews, jh );
	v[ "defaultView" ] = writeToJson( defaultView, jh );
	v[ "notebookStyle" ] = writeToJson( notebookStyle, jh );
	v[ "notebookOrientation" ] = writeToJson( notebookOrientation, jh );
	v[ "radixItems" ] = writeToJson( radixItems, jh );
	v[ "directionItems" ] = writeToJson( directionItems, jh );
	v[ "multiDirectionItems" ] = writeToJson( multiDirectionItems, jh );
	v[ "solarItems" ] = writeToJson( solarItems, jh );
	v[ "partnerItems" ] = writeToJson( partnerItems, jh );
	return v;
}


// IN class Gdiconfig defined in src/base/gdiconfig.h line 309
void Gdiconfig::fromJsonValue( const Json::Value &v, JsonHandler &jh )
{
	if ( v[ "uranianAspectStyle" ] )
	{
		extractFromJson( v[ "uranianAspectStyle" ], uranianAspectStyle, jh ); 
	}
	if ( v[ "westernAspects" ] )
	{
		extractFromJson( v[ "westernAspects" ], westernAspects, jh ); 
	}
	if ( v[ "skins" ] )
	{
		extractFromJson( v[ "skins" ], skins, jh ); 
	}
	if ( v[ "geoms" ] )
	{
		extractFromJson( v[ "geoms" ], geoms, jh ); 
	}
	if ( v[ "colors" ] )
	{
		extractFromJson( v[ "colors" ], colors, jh ); 
	}
	if ( v[ "chartColors" ] )
	{
		extractFromJson( v[ "chartColors" ], chartColors, jh ); 
	}
	if ( v[ "bardiagram" ] )
	{
		extractFromJson( v[ "bardiagram" ], bardiagram, jh ); 
	}
	if ( v[ "tableStyleId" ] )
	{
		extractFromJson( v[ "tableStyleId" ], tableStyleId, jh ); 
	}
	if ( v[ "chartWestern" ] )
	{
		extractFromJson( v[ "chartWestern" ], chartWestern, jh ); 
	}
	if ( v[ "chartUranian" ] )
	{
		extractFromJson( v[ "chartUranian" ], chartUranian, jh ); 
	}
	if ( v[ "chartVarga" ] )
	{
		extractFromJson( v[ "chartVarga" ], chartVarga, jh ); 
	}
	if ( v[ "chartSbc" ] )
	{
		extractFromJson( v[ "chartSbc" ], chartSbc, jh ); 
	}
	if ( v[ "chartUniversal" ] )
	{
		extractFromJson( v[ "chartUniversal" ], chartUniversal, jh ); 
	}
	if ( v[ "prefs" ] )
	{
		extractFromJson( v[ "prefs" ], prefs, jh ); 
	}
	if ( v[ "view" ] )
	{
		extractFromJson( v[ "view" ], view, jh ); 
	}
	if ( v[ "atlas" ] )
	{
		extractFromJson( v[ "atlas" ], atlas, jh ); 
	}
	if ( v[ "multipleView" ] )
	{
		extractFromJson( v[ "multipleView" ], multipleView, jh ); 
	}
	if ( v[ "fonts" ] )
	{
		extractFromJson( v[ "fonts" ], fonts, jh ); 
	}
	if ( v[ "toolbar" ] )
	{
		extractFromJson( v[ "toolbar" ], toolbar, jh ); 
	}
	if ( v[ "printout" ] )
	{
		extractFromJson( v[ "printout" ], printout, jh ); 
	}
	if ( v[ "page" ] )
	{
		extractFromJson( v[ "page" ], page, jh ); 
	}
	if ( v[ "pdf" ] )
	{
		extractFromJson( v[ "pdf" ], pdf, jh ); 
	}
}


// OUT class Gdiconfig defined in src/base/gdiconfig.h line 309
Json::Value Gdiconfig::toJsonValue( JsonHandler &jh ) const
{
	Json::Value v;
	v[ "uranianAspectStyle" ] = writeToJson( uranianAspectStyle, jh );
	v[ "westernAspects" ] = writeToJson( westernAspects, jh );
	v[ "skins" ] = writeToJson( skins, jh );
	v[ "geoms" ] = writeToJson( geoms, jh );
	v[ "colors" ] = writeToJson( colors, jh );
	v[ "chartColors" ] = writeToJson( chartColors, jh );
	v[ "bardiagram" ] = writeToJson( bardiagram, jh );
	v[ "tableStyleId" ] = writeToJson( tableStyleId, jh );
	v[ "chartWestern" ] = writeToJson( chartWestern, jh );
	v[ "chartUranian" ] = writeToJson( chartUranian, jh );
	v[ "chartVarga" ] = writeToJson( chartVarga, jh );
	v[ "chartSbc" ] = writeToJson( chartSbc, jh );
	v[ "chartUniversal" ] = writeToJson( chartUniversal, jh );
	v[ "prefs" ] = writeToJson( prefs, jh );
	v[ "view" ] = writeToJson( view, jh );
	v[ "atlas" ] = writeToJson( atlas, jh );
	v[ "multipleView" ] = writeToJson( multipleView, jh );
	v[ "fonts" ] = writeToJson( fonts, jh );
	v[ "toolbar" ] = writeToJson( toolbar, jh );
	v[ "printout" ] = writeToJson( printout, jh );
	v[ "page" ] = writeToJson( page, jh );
	v[ "pdf" ] = writeToJson( pdf, jh );
	return v;
}

