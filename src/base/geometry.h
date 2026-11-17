/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/geometry.h
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

#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "bootstrap.h"
#include "constants.h"
#include "gdibase.h"

enum class CgElementType
{
	None = -1,
	Disc = 0,
	Annulus,

	Signs,
	Houses,
	Nakshatras,
	DasaLords,
	WesternPlanets,
	UranianPlanets,
	Angles,
	Aspects,
	Image,

	UranianDial,
	UranianAspects,

	EastGrid,
	SouthGrid,
	DiamondGrid,
	SbcGrid,

	Error,
	First = Disc,
	Last = Error
};
mostream& operator <<( mostream&, const CgElementType& );

/*************************************************//**
*
*
*
******************************************************/
class CgElement : public Jsonable
{
public:
	CgElement( const CgElementType type, const double r = 100.0 ) : type( type ), radius( r )
	{
		show = true;
	}

	CgElement( const CgElement& ) = default;

	virtual ~CgElement() {}

	CgElementType getElementType() const { return type; }

	virtual CgElement* clone() = 0;

	virtual double getOuterRadius( const int& ) const { return radius; }

private: 
	const CgElementType type;

public:

	// json:start
	double radius;

	bool show;

	wxPen pen;

	MBrush brush;

	wxColour textColor;
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
class CgDisc : public CgElement
{
public:
	CgDisc( const CgElementType type = CgElementType::Disc, const double r = 100.0,
		const ChartShape geom = ChartShape::Inherited )
		: CgElement( type, r ),
			geometryType( geom )
	{
		markerLength = 1.0;
	}

	virtual CgElement* clone() override { return new CgDisc( *this ); }

	CgDisc magnify( const double &w ) const
	{
		CgDisc disc( *this );
		disc.radius += w;
		return disc;
	}

	// json:start
	double markerLength;

	wxPen markerPen;

	ChartShape geometryType;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*
*
******************************************************/
class CgError : public CgDisc
{
public:
	CgError( wxString s, const double r = 100.0,
		const ChartShape geom = ChartShape::Inherited )
		: CgDisc( CgElementType::Error, r, geom ),
		message( s )
	{
	}

	virtual CgElement* clone() override { return new CgError( *this ); }

	// json:start
	wxString message;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*
*
******************************************************/
class CgInlayImage : public CgDisc
{
public:
	CgInlayImage( const double radius = 100.0, const ChartShape geom = ChartShape::Inherited )
	 : CgDisc( CgElementType::Image, radius, geom )
	 {
			align = Align::Center;
	 }

	virtual CgElement* clone() override { return new CgInlayImage( *this ); }

	// json:start
	Align align;
	wxString filename;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*
*
******************************************************/
class CgVedicChartGrid : public CgDisc
{
public:
	CgVedicChartGrid(
		const CgElementType &type,
		const double radius = 90.0,
		const ChartShape geom = ChartShape::Inherited )
	 : CgDisc( type, radius, geom )
	 {
			straightFrame = false;
			showFieldNames = false;
	 }

	virtual CgElement* clone() override { return new CgVedicChartGrid( *this ); }

	// json:start
	// TODO was macht der noch mal? es ging um die durchgezeichneten Linien bei Eastindan Chart
	bool straightFrame;

	// i.e. sign or Nakshatra name
	bool showFieldNames;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*
*
******************************************************/
class CgAnnulus : public CgDisc
{
public:
	CgAnnulus(
		const CgElementType type = CgElementType::Annulus,
		const double radius = 100.0,
		const double innerRadius = 50.0,
		const ChartShape geom = ChartShape::Inherited )
	 : CgDisc( type, radius, geom ),
	 	innerRadius( innerRadius )
	 {
			showInnerFrame = true;
			showOuterFrame = true;
	 }

	virtual CgElement* clone() override { return new CgAnnulus( *this ); }

	CgDisc innerDisc() const { return CgDisc( CgElementType::Disc, innerRadius, geometryType ); }

	// json:start
	double innerRadius;
	bool showInnerFrame;
	bool showOuterFrame;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*
*
******************************************************/
class CgLabelledAnnulus : public CgAnnulus
{
public:

	CgLabelledAnnulus(
		const CgElementType type,
		const double radius = 100.0,
		const double innerRadius = 50.0,
		const double rLabels = 80,
		const ChartShape geom = ChartShape::Inherited )
		: CgAnnulus( type, radius, innerRadius, geom ),
		rLabels( rLabels )
	{
	}

	virtual CgElement* clone() override { return new CgLabelledAnnulus( *this ); }

	// json:start
	// radius of labels
	double rLabels = 80;

	// in percent
	int labelSize = 100;

	bool useSymbols = true;

	TextFormat textLength = TextFormat::Medium;

	bool showLabels = true;

	bool showConnectors = true;

	bool rotateLabels = false;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*
*
******************************************************/
struct CgDegreeMarker : public Jsonable
{
	// json:start
	bool show = true;

	double wDegree1 = 3;
	double wDegree5 = 4;
	double wDegree10 = 5;

	bool show1DegreeFrame = false;
	bool show5DegreeFrame = false;
	bool show10DegreeFrame = false;

	wxPen pen1;
	wxPen pen5;
	wxPen pen10;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*
*
******************************************************/
class CgPlanetScale : public CgLabelledAnnulus 
{
public:

	CgPlanetScale(
		const CgElementType type = CgElementType::WesternPlanets,
		const double radius = 79,
		const double innerRadius = 50.0,
		const double rLabels = 73,
		const ChartShape geom = ChartShape::Inherited )
		: CgLabelledAnnulus( type, radius, innerRadius, rLabels, geom )
	{
		labelSize = 200;
	}

	// implementation in ChartGeometry.cpp
	virtual CgElement* clone() override { return new CgPlanetScale( *this ); }

	// implementation in ChartGeometry.cpp
	virtual double getOuterRadius( const int& ) const override;

	// returns radius/rLabels for all ChartIds
	std::pair<double, double> getParams4ChartId( const int& ) const;

	// json:start
	int secondaryScaleOffset = 15;

	int furtherScaleOffset = 10;

	int furtherScaleLabelOffset = 5;

	CgDegreeMarker firstScaleMarker;

	CgDegreeMarker secondaryScaleMarker;

	CgDegreeMarker furtherScaleMarker;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*
*
******************************************************/
class CgUranianDial : public CgLabelledAnnulus
{
public:
	CgUranianDial(
		const double radius = 79.0,
		const double innerRadius = 59.0,
		const double rLabels = 68,
		const ChartShape geom = ChartShape::Inherited )
		: CgLabelledAnnulus( CgElementType::UranianDial, radius, innerRadius, rLabels, geom )
	{
	}

	virtual CgElement* clone() override { return new CgUranianDial( *this ); }

	// json:start
	// there are degree labels in higher divisions
	int degreeLabelSize = 70;

	// radius of degree labels
	double rDegreeLabels = 75;

	// annulus will have different annulus radius if this param is set
	double higherDivisionInnerRadius = innerRadius;

	// show zodial signs in divisions D-2,3,4,6
	bool showHigherDivisionSigns = true;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

#endif

