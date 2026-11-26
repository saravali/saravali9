/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/CeFieldGrid.cpp
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

#include "CeFieldGrid.h"

#include "VedicAspect.h"
#include "ConfigListItems.h"
#include "ContextProperties.h"
#include "DocHelper.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "mathbase.h"
#include "Painter.h"
#include "Varga.h"

extern Gdiconfig *gdiconfig;

//#define DEBUG_CEFIELDGRID

/*****************************************************
**
**   CeFieldGrid   ---   Constructor
**
******************************************************/
CeFieldGrid::CeFieldGrid( HybridChart *chart, CgVedicChartGrid *grid, const int count )
	: ChartElement( chart, grid ), field_count( count )
{
}

/*****************************************************
**
**   CeFieldGrid   ---   Destructor
**
******************************************************/
CeFieldGrid::~CeFieldGrid()
{
	if ( proxy ) delete proxy;
}

/*****************************************************
**
**   CeFieldGrid   ---   onCoordinatesChanged
**
******************************************************/
void CeFieldGrid::onCoordinatesChanged()
{
#ifdef DEBUG_CEFIELDGRID
	debuglog << "Start CeFieldGrid::onCoordinatesChanged";
#endif
	assert( proxy );
	CgVedicChartGrid *cg = getGeom();

	for( ChartField &field: fields ) { field.clear(); }
	//writePlanetFields();

	pFieldSplit = MComplex( 0, 0 );

	// right
	p_east = diskPointToXy( 0, cg );

	// top
	p_north = diskPointToXy( 90, cg );
#ifdef DEBUG_CEFIELDGRID
	debuglog << "Start CeFieldGrid::onCoordinatesChanged p_north " << p_north;
#endif

	// left
	p_west = diskPointToXy( 180, cg );

	// bottom
	p_south = diskPointToXy( 270, cg );

	assert( p_east.real() >= p_west.real());
	assert( p_south.imag() >= p_north.imag());

	p_northwest = diskPointToXy( 135, cg );
	p_southwest = diskPointToXy( 225, cg );
	p_southeast = diskPointToXy( 315, cg );
	p_northeast = diskPointToXy( 45, cg );

	// delegate to impl class
	setupFieldRectangles();
	writePlanetFields();
}

/*****************************************************
**
**   CeFieldGrid   ---   paint
**
******************************************************/
void CeFieldGrid::paint( Painter *painter, const MRect &refreshRect )
{
	uint i;
	ChartConfig *cfg = chart->getChartConfig();
	ContextProps_ptr props = chart->getProps();

	CgVedicChartGrid *cg = getGeom();
#ifdef DEBUG_CEFIELDGRID
	debuglog << "CeFieldGrid::paint geom " << (int)cg->geometryType
		<< " field status " << debugFieldStatus( fieldStatus ) << " rect " << refreshRect;
#endif
	if ( ! cg->show ) return;

	startPaint( painter );
	
	setInheritedStyle( painter );
	painter->setTransparentBrush();
	paintDisk( painter, *cg );

	painter->setPen( getPen() );

	// zoom av
	double zoom = chart->isAshtakavargaChart() ? 150 : 100;
	chart->setGraphicFont( painter, zoom );

	const bool isDoubleChart = chart->isDoubleChart();

	// step 1: calculate transit points;
	if ( isDoubleChart )
	{
		transitRect = MRect(
			p_northwest.real() + pFieldSplit.real(),
			p_northwest.imag() + pFieldSplit.imag(),
			p_southeast.real() - p_northwest.real() - 2 * pFieldSplit.real(),
			p_southeast.imag() - p_northwest.imag() - 2 * pFieldSplit.imag()
		);
	}

	// step 2: draw field background
	const WidgetSkin &skin = chart->getSkin();
	if ( skin.signPattern.isValid())
	{
		for( i = 0; i < fields.size(); i++ )
		{
			if ( ! doPaintFieldBackground( i )) continue;
			painter->setBrush( skin.signPattern.getBrush( i ));

#ifdef DEBUG_CEFIELDGRID
			debuglog << "CeFieldGrid::paint " << i << " rect " << fields[i].rect
				<< " contents " << fields[i].getContents( chart->getFirstRole() );
#endif
			fields[i].paint( painter, false );
		}
	}

	// step 2a: paint active field
	if ( activeField != UINT_FOR_NOT_FOUND )
	{
		assert( activeField < field_count );
		if ( cfg->highlightOnMouseOver )
		{
			if ( skin.markedFieldBrush.isOk())
			{
				painter->setBrush( skin.markedFieldBrush );
			}
			else
			{
				painter->setBrush( gdiconfig->colors.chartMarkupBgColor );
			}
		}
		else if ( skin.signPattern.isValid())
		{
			painter->setBrush( skin.signPattern.getBrush( activeField ));
		}
		fields[activeField].paint( painter, cfg->zoomOnMouseOver );
	}

	// step 3: draw fields and labels (if required)
	for( i = 0; i < fields.size(); i++ )
	{
#ifdef DEBUG_CEFIELDGRID
		debuglog << "CeFieldGrid::paint field " << i << " contents " << fields[i].contents;
#endif
		if ( ! isDoubleChart )
		{
			fields[i].drawText( painter, FieldPart::All );
		}
		else
		{
			fields[i].drawText( painter, FieldPart::Inner, pFieldSplit );
			fields[i].drawText( painter, FieldPart::Outer, pFieldSplit );
		}
		if ( ! fields[i].label.isEmpty()) fields[i].drawLabel( painter );
	}

	// step 4: update aspects if required
	if ( ! aspects.size())
	{
		Horoscope_ptr aspecting_h = chart->getSecondHoroscope();
		Horoscope_ptr aspected_h = chart->getFirstHoroscope();
		if ( ! aspecting_h ) aspecting_h = aspected_h;
		updateAspects( aspecting_h, aspected_h );
	}

	// step 5: draw aspects for all/aspecting/aspected fields according to field status
	if ( fieldStatus.contains( FieldStatus::AllAspects ))
	{
		for( uint j = 0; j < field_count; j++ )
		{
			paintAspectsForSource( painter, j );
		}
	}
	else
	{
		if ( fieldStatus.contains( FieldStatus::Aspected ))
		{
			paintAspectsForSource( painter, activeField );
		}

		if ( fieldStatus.contains( FieldStatus::Aspecting ))
		{
			paintAspectsForTarget( painter, activeField );
		}
	}
	setInheritedStyle( painter );
	painter->setTransparentBrush();

	// delegate to impl
	paintGrid( painter, refreshRect );
	//finishPaint( painter );

	// draw center string, formatting is done before
	if ( ! centerItem.tf.isEmpty())
	{
		DocHelper( painter ).drawTextItem( centerItem );
	}

	//if ( type == DataContext::Transit || type == DataContext::Partner )
	if ( isDoubleChart )
	{
		// draw transit rectangle
		wxPen tpen = getPen();
		tpen.SetStyle( wxPENSTYLE_SHORT_DASH );
		//tpen.style = PenStyle::LongDash;

		//tpen.color = MColor ( "orange" );
		painter->setPen( tpen );
		painter->setTransparentBrush();
		painter->drawRectangle( transitRect );
		setInheritedStyle( painter );
	}
}

/*****************************************************
**
**   CeFieldGrid   ---   field converters
**
******************************************************/
int CeFieldGrid::geomToAstroField( const int &geom_field )
{
	return positionOffset ? a_red( geom_field + positionOffset, field_count ) : geom_field;
}
int CeFieldGrid::astroToGeomField( const int &astro_field )
{
	return positionOffset ? a_red( astro_field - positionOffset, field_count ) : astro_field;
}

/*****************************************************
**
**   CeFieldGrid   ---   paintAspectsForSource
**
******************************************************/
void CeFieldGrid::paintAspectsForSource( Painter *painter, const int &geom_field )
{
	if ( geom_field < 0 ) return;
	assert( geom_field < (int)field_count );
#ifdef DEBUG_CEFIELDGRID
	debuglog << "CeFieldGrid::paintAspectsForSource geom_field " << geom_field << " aspect count " << aspects.size();
#endif

	std::list<std::pair<bool, VedicAspectStrength>> aspect_types;
	const int astro_field = geomToAstroField( geom_field );

	for( int target_field = 0; target_field < (int)field_count; target_field++ )
	{
		aspect_types.clear();
		for( const auto &a : aspects )
		{
			if ( a.aspecting_f == astro_field && a.aspected_f == target_field && isAspectValid( a.type ) )
			{
				aspect_types.push_back( std::make_pair( a.benefic, a.strength ));
			}
		}
#ifdef DEBUG_CEFIELDGRID
		debuglog << "CeFieldGrid::paintAspectsForSource size " << aspect_types.size();
#endif
		if ( aspect_types.size() > 0 )
		{
			paintAspectBundle( painter, geom_field, astroToGeomField( target_field ), aspect_types );
		}
	}
}

/*****************************************************
**
**   CeFieldGrid   ---   paintAspectsForTarget
**
******************************************************/
void CeFieldGrid::paintAspectsForTarget( Painter *painter, const int &geom_field )
{
	if ( geom_field < 0 ) return;
	assert( geom_field < (int)field_count );
#ifdef DEBUG_CEFIELDGRID
	debuglog << "CeFieldGrid::paintAspectsForTarget geom_field " << geom_field << " aspect count " << aspects.size();
#endif

	std::list<std::pair<bool, VedicAspectStrength>> aspect_types;
	//const int astro_field = geomToAstroField( geom_field );

	for( int source_field = 0; source_field < (int)field_count; source_field++ )
	{
		aspect_types.clear();
		for( const auto &a : aspects )
		{
			if ( a.aspecting_f == source_field && a.aspected_f == geom_field && isAspectValid( a.type ) )
			{
				aspect_types.push_back( std::make_pair( a.benefic, a.strength ));
			}
		}
#ifdef DEBUG_CEFIELDGRID
		debuglog << "CeFieldGrid::paintAspectsForTarget size " << aspect_types.size();
#endif
		if ( aspect_types.size() > 0 )
		{
			paintAspectBundle( painter, astroToGeomField( source_field ), geom_field, aspect_types );
		}
	}
}

/*****************************************************
**
**   CeFieldGrid   ---   paintAspectBundle
**
******************************************************/
void CeFieldGrid::paintAspectBundle( Painter *painter, const int &field1, const int &field2,
	const std::list<std::pair<bool, VedicAspectStrength>> &aspect_types )
{
#ifdef DEBUG_CEFIELDGRID
	debuglog << "CeFieldGrid::paintAspectBundle geom_field1 " << field1 << " field2 " << field2 << " size " << aspect_types.size();
#endif
	if ( aspect_types.size() < 1 ) return;

	const auto fieldParts = getActiveFieldParts();
	const MComplex p1 = fields[field1].getCenter( fieldParts.first, pFieldSplit );
	const MComplex p2 = fields[field2].getCenter( fieldParts.second, pFieldSplit );

	constexpr double dist = 4;
	const MComplex c = findOrthogonalPoint( p1 - p2, aspect_types.size() * dist );

	double lambda = 0; // goes from 0..1

	for( const auto &a : aspect_types )
	{
		const MComplex q1 = lambda * ( p1 - c ) + ( 1 - lambda ) * ( p1 + c );
		const MComplex q2 = lambda * ( p2 - c ) + ( 1 - lambda ) * ( p2 + c );
		setAspectPainterStyle( painter, a );
		painter->drawArrow( q1, q2 );
		lambda += 1.0 / aspect_types.size();
	}
}

/*****************************************************
**
**   CeFieldGrid   ---   calculateFieldCenters
**
******************************************************/
std::pair<MComplex, MComplex> CeFieldGrid::calculateFieldCenters( const int &field1, const int &field2,
	const bool &isSeparated ) const
{
	const int g1 = positionOffset ? a_red( field1 - positionOffset, field_count ) : field1;
	const int g2 = positionOffset ? a_red( field2 - positionOffset, field_count ) : field2;

	if ( isSeparated )
	{
		return std::make_pair( 
			fields[g1].calculateModifiedRect( FieldPart::Outer, pFieldSplit ).getCenter(),
			fields[g2].calculateModifiedRect( FieldPart::Inner, pFieldSplit ).getCenter()
		);
	}
	return std::make_pair( fields[g1].getCenter(), fields[g2].getCenter());
}

/*****************************************************
**
**   CeFieldGrid   ---   getActiveFieldParts
**
******************************************************/
std::pair<FieldPart, FieldPart> CeFieldGrid::getActiveFieldParts() const
{
	ContextProps_ptr props = chart->getProps();
	switch( props->getDataContext())
	{
		case DataContext::Direction:
		case DataContext::MultiDirection:
		case DataContext::Solar:
			return std::make_pair( FieldPart::Outer, FieldPart::Inner );
		break;
		case DataContext::Partner:
			if ( ! chart->isCompositeChart())
			{
				if ( fieldStatus.contains( FieldStatus::PosOuter ))
				{
					return std::make_pair( FieldPart::Outer, FieldPart::Inner );
				}
				else if ( fieldStatus.contains( FieldStatus::PosInner ))
				{
					return std::make_pair( FieldPart::Inner, FieldPart::Outer );
				}
			}
		break;
		case DataContext::Radix:
		case DataContext::Ashtakavarga:
		break;
		default:
			assert( false );
		break;
	}
	return std::make_pair( FieldPart::All, FieldPart::All );
}

/*****************************************************
**
**   CeFieldGrid   ---   areAspectsFieldsSeparated
**
******************************************************/
bool CeFieldGrid::areAspectsFieldsSeparated() const
{
	ContextProps_ptr props = chart->getProps();
	switch( props->getDataContext())
	{
		case DataContext::Direction:
		case DataContext::MultiDirection:
		case DataContext::Solar:
			return true;
		break;
		case DataContext::Partner:
			return ( ! chart->isCompositeChart());
		break;
		case DataContext::Radix:
		case DataContext::Ashtakavarga:
			return false;
		break;
		default:
			assert( false );
		break;
	}
	return false;
}

/*****************************************************
**
**   CeFieldGrid   ---   isAspectValid
**
******************************************************/
bool CeFieldGrid::isAspectValid( const VedicAspectId &a ) const
{
	return( a == VedicAspectId::GrahaDrishti || a == VedicAspectId::RasiDrishti );
}

/*****************************************************
**
**   CeFieldGrid   ---   startPaint
**
******************************************************/
void CeFieldGrid::startPaint( Painter *painter )
{
	// format center string here because it will be used for drawGrid in Diamond chart
	centerItem = DocItemText( proxy->getCenterString(), chart->getRect(), Align::Center );
	DocHelper( painter ).formatAndFitTextItem( centerItem );
#ifdef DEBUG_CEFIELDGRID
	debuglog << "VedicChartGrid: centerItem " << proxy->getCenterString() << " rect " << chart->getRect();
#endif
}

/*****************************************************
**
**   CeFieldGrid   ---   doPaintFieldBackground
**
******************************************************/
bool CeFieldGrid::doPaintFieldBackground( const uint &f )
{
	return f != activeField;
}

/*****************************************************
**
**   CeFieldGrid   ---   onResponsiveChartEvent
**
******************************************************/
WidgetResponse CeFieldGrid::onResponsiveChartEvent( const MComplex &p, const int keymod )
{
#ifdef DEBUG_CEFIELDGRID
	debuglog << "CeFieldGrid::onResponsiveChartEvent " << p << " keymod " << keymod;
#endif
	const uint oldactive = activeField;

	activeField = UINT_FOR_NOT_FOUND;
	std::set<FieldStatus> oldFieldStatus = fieldStatus;

	for( uint i = 0; i < fields.size(); i++ )
	{
		if ( fields[i].containsPoint( p ))
		{
			//debuglog << "point " << p << " IN RECT " << i << " rect " << fields[i].rect ;
			activeField = i;
			calculateFieldStatus( p, keymod );
			break;
		}
	}
#ifdef DEBUG_CEFIELDGRID
	debuglog << "CeFieldGrid::onResponsiveChartEvent oldactive " << oldactive << " newactive " << activeField
		<< " old marked " << debugFieldStatus( oldFieldStatus )
		<< " new marked " << debugFieldStatus( fieldStatus );
#endif
	return oldactive != activeField || oldFieldStatus != fieldStatus;
}

/*****************************************************
**
**   CeFieldGrid   ---   addKeybordModifierOptions
**
******************************************************/
void CeFieldGrid::addKeybordModifierOptions( const int &id )
{
	const DataContext charttype = chart->getProps()->getDataContext();
  constexpr int aspectedObjectsMode = 2;
  constexpr int aspectingObjectsMode = 3;
  constexpr int allAspectsMode = 4;

	if ( ! chart->isAshtakavargaChart())
	{
		ContextProps_ptr props = chart->getProps();

		if ( aspectingObjectsMode == id ) fieldStatus.insert( FieldStatus::Aspecting );
		if ( aspectedObjectsMode == id ) fieldStatus.insert( FieldStatus::Aspected );
		if ( allAspectsMode == id && charttype != DataContext::Partner ) fieldStatus.insert( FieldStatus::AllAspects );
	}
}

/*****************************************************
**
**   CeFieldGrid   ---   calculateFieldStatus
**
******************************************************/
void CeFieldGrid::calculateFieldStatus( const MComplex &p, const int &keymod )
{
	ChartConfig *cfg = chart->getChartConfig();
	const DataContext charttype = chart->getProps()->getDataContext();
	fieldStatus.clear();

	if ( cfg->highlightOnMouseOver ) fieldStatus.insert( FieldStatus::Markup ); 
	if ( cfg->zoomOnMouseOver ) fieldStatus.insert( FieldStatus::Zoom );

	if ( keymod & KeyboardModfier::KbShift ) { addKeybordModifierOptions( 2 ); }
	else if ( keymod & KeyboardModfier::KbCtrl ) { addKeybordModifierOptions( 3 ); }
	else if ( keymod & KeyboardModfier::KbAlt ) { addKeybordModifierOptions( 4 ); }
	else { addKeybordModifierOptions( 1 ); }

	// test if mouse position is within transit rect (used for non commutative partner aspects)
	if ( charttype == DataContext::Radix || charttype == DataContext::Direction )
	{
		fieldStatus.insert( FieldStatus::PosFull );
	}
	else
	{
		if (  transitRect.containsPoint( p )) fieldStatus.insert( FieldStatus::PosInner );
		else fieldStatus.insert( FieldStatus::PosOuter );
	}
}

/*****************************************************
**
**   CeFieldGrid   ---   setAspectPainterStyle
**
******************************************************/
void CeFieldGrid::setAspectPainterStyle( Painter *painter, const std::pair<bool, VedicAspectStrength> &aspect_type )
{
	wxPenStyle style;
  wxColour color;
	switch( chart->getChartConfig( WidgetCategory::VargaChart )->beneficMaleficDisplay )
	{
		case 0: // specific
			if ( aspect_type.first ) color = gdiconfig->colors.beneficFgColor;
			else color = gdiconfig->colors.maleficFgColor;
		break;
		case 1: // background
			color = getPen().GetColour();
		break;
		case 2: // grey
			color = "#808080";
		break;
	}

	switch( aspect_type.second )
	{
		case VedicAspectStrength::P25:
			style = wxPENSTYLE_DOT;
		break;
		case VedicAspectStrength::P50:
			style = wxPENSTYLE_SHORT_DASH;
		break;
		case VedicAspectStrength::P75:
			style = wxPENSTYLE_LONG_DASH;
		break;
		case VedicAspectStrength::Full:
			style = wxPENSTYLE_SOLID;
		break;
		default:
			warnlog << "BasicVedicChart::setAspectPen WARN: wrong aspect strength " << aspect_type.second;
			style = wxPENSTYLE_SOLID;
		break;
	}
	painter->setPen( wxPen( color, 1, style ));
	painter->setBrush( color );
}

/*****************************************************
**
**   VedicChartGrid   ---   Constructor
**
******************************************************/
VedicChartGrid::VedicChartGrid( HybridChart *chart, CgVedicChartGrid *torus )
	: CeFieldGrid( chart, torus, 12 )
{
	assert( chart );
	assert( ! proxy );
	FieldWriterFactory factory;

	AshtakavargaExpert *expert = chart->getAshtakavargaExpert();
	if ( expert )
	{
		AvType avtype = chart->getAvType();
		PlanetId ob = chart->getAvObject();
		proxy = factory.createAshtakavargaFieldWriter( this, avtype, expert, ob );
	}
	else
	{
		proxy = factory.createVargaFieldWriter( this );
	}
	assert( proxy );
}

/*****************************************************
**
**   VedicChartGrid   ---   updateAspects
**
******************************************************/
void VedicChartGrid::updateAspects( Horoscope_ptr aspecting_h, Horoscope_ptr aspected_h )
{
	if ( ! aspecting_h || ! aspected_h ) return; // can happen in partner chart

	ContextProps_ptr props = chart->getProps();

	ChartConfig *vargaChartConfig = chart->getChartConfig ( WidgetCategory::VargaChart );
#ifdef DEBUG_CEFIELDGRID
	debuglog << "vargaChartConfig aspectType " << vargaChartConfig->aspectType;
#endif

	VedicAspectExpert expert( props, vargaChartConfig );
	const Varga varga = chart->getVarga();

	aspects = expert.calculateVargaChartAspects( varga, aspecting_h, aspected_h );
	if ( props->getDataContext() == DataContext::Partner )
	{
		for( const auto &asp :  expert.calculateVargaChartAspects( varga, aspected_h, aspecting_h ))
		{
			aspects.push_back( asp );
		}
	}
}

/*****************************************************
**
**   CeFieldGrid   ---   debugFieldStatus
**
******************************************************/
wxString CeFieldGrid::debugFieldStatus( const std::set<FieldStatus> &a )
{
	wxString s;
	s << "(";
	for( const auto x : a )
	{
		s << enum_to_string( x ) << " ";
	}
	s << ")";
	return s;
}


