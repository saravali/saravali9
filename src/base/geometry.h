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
	Disk = 0,
	Annulus,

	Signs,
	Houses,
	Nakshatras,
	DasaLords,

	WesternSigns,
	WesternPlanetRing,
	Angles,
	Aspects,
	Image,

	UranianDisk,
	UranianAspects,
	UranianPlanetRing,

	EastGrid,
	SouthGrid,
	DiamondGrid,
	SbcGrid,

	Error,
	First = Disk,
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
class CgDisk : public CgElement
{
public:
	CgDisk( const CgElementType type = CgElementType::Disk, const double r = 100.0,
		const ChartShape geom = ChartShape::Inherited )
		: CgElement( type, r ),
			geometryType( geom )
	{
		markerLength = 1.0;
	}

	virtual CgElement* clone() override { return new CgDisk( *this ); }

	CgDisk magnify( const double &w ) const
	{
		CgDisk disc( *this );
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
class CgError : public CgDisk
{
public:
	CgError( wxString s, const double r = 100.0,
		const ChartShape geom = ChartShape::Inherited )
		: CgDisk( CgElementType::Error, r, geom ),
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
class CgInlayImage : public CgDisk
{
public:
	CgInlayImage( const double radius = 100.0, const ChartShape geom = ChartShape::Inherited )
	 : CgDisk( CgElementType::Image, radius, geom )
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
class CgVedicChartGrid : public CgDisk
{
public:
	CgVedicChartGrid(
		const CgElementType &type,
		const double radius = 90.0,
		const ChartShape geom = ChartShape::Inherited )
	 : CgDisk( type, radius, geom )
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
class CgAnnulus : public CgDisk
{
public:
	CgAnnulus(
		const CgElementType &type,
		const double radius = 100.0,
		const double innerRadius = 50.0,
		const ChartShape geom = ChartShape::Inherited )
	 : CgDisk( type, radius, geom ),
	 	innerRadius( innerRadius )
	 {
			showInnerFrame = true;
			showOuterFrame = true;
	 }

	virtual CgElement* clone() override { return new CgAnnulus( *this ); }

	CgDisk innerDisc() const { return CgDisk( CgElementType::Disk, innerRadius, geometryType ); }

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
struct DegreeMarkerConfig : public Jsonable
{
	// json:start
	bool show = true;
	double radius = 100;

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

	DegreeMarkerConfig degreeMarkerCfg;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
* Data structure of planets in western and uranian chart, used in calss PlanetRing.
*
	Params
	------
	radius: outer radius of the planets
	innerRadius: connection with the disk (can be larger in case of western chart)
	rLabels. radius of first role planets
	markerLength: radius of the markers from disk to the point where the markers get a buckle (in case of corrected lomgitude)
	nextOuterRadiusDelta: distance from ring radius to next ring radius

	Remark: PlanetRing does not prolabels a degree scale

	Scenarios
	---------
	1. All Planets outside a disk (UranianDisk or (unusual) zodiacal signs with no planets inside
	Condition: rLabels >= radius.
	 - First set of planets is at rLabels
	 - Area of next role begins at radius + n * nextOuterRadiusDelta
	 - Next set of planets rLabels + n * nextOuterRadiusDelta
	 - innerRadius is UNUSED

	2. First set of planets (radix) inside, other sets (like directions, solar, second partner) outside the disk: Western chart
	Condition: rLabels < radius
	 - First set of planets is at rLabels
	 - Marker lines of first set from innerRadius to rLabels
	 - Next set of planets at TODO
	 - Next area of planets at radius + n * nextOuterRadiusDelta
*
******************************************************/
class CgPlanetRing : public CgLabelledAnnulus 
{
public:

	CgPlanetRing(
		const CgElementType &type,
		const double radius = 100,
		const double innerRadius = 84,
		const double rLabels = 95,
		const ChartShape geom = ChartShape::Inherited )
		: CgLabelledAnnulus( type, radius, innerRadius, rLabels, geom )
	{
		labelSize = 200;
		markerLength = 2.0;
		nextOuterRadiusDelta = 20;
	}

	virtual CgElement* clone() override { return new CgPlanetRing( *this ); }

	// json:start
	double nextOuterRadiusDelta;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

/*************************************************//**
*
*
*
******************************************************/
class CgUranianDisk : public CgLabelledAnnulus
{
public:
	CgUranianDisk(
		const double radius = 84.0,
		const double innerRadius = 15.0,
		const double rLabels = 78,
		const ChartShape geom = ChartShape::Inherited )
		: CgLabelledAnnulus( CgElementType::UranianDisk, radius, innerRadius, rLabels, geom )
	{
		signOuterRadius = .8 * radius;
		signInnerRadius = .6 * radius;
		signRLabels = .7 * radius;

		houseOuterRadius = .6 * radius;
		houseInnerRadius = .4 * radius;
		houseRLabels = .5 * radius;
	}

	virtual CgElement* clone() override { return new CgUranianDisk( *this ); }

	// json:start
	DegreeMarkerConfig degreeMarker;

	int degreeLabelSize = 70;

	bool paintDegreeNumbers = true;

	// all radius items below are relative, i.e. between 0..1
  bool showSigns = true;
  double signInnerRadius;
  double signOuterRadius;
  double signRLabels;

  bool showHouses = true;
  double houseInnerRadius;
  double houseOuterRadius;
  double houseRLabels;

	// impls in jsonstubs.cpp
	virtual void fromJsonValue( const Json::Value&, JsonHandler& ) override;
	virtual Json::Value toJsonValue( JsonHandler& ) const override;
};

#endif

