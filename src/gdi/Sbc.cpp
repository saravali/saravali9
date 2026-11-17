/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/Sbc.cpp
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

#include "astrobase.h"
#include "astroconfig.h"
#include "ConfigListItems.h"
#include "ContextProperties.h"
#include "gdiconfig.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "Lang.h"
#include "mathbase.h"
#include "mstring.h"
#include "Nakshatra.h"
#include "Painter.h"
#include "SymbolProvider.h"
#include "DocHelper.h"
#include "VedicAspect.h"

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define RTOL_DOUBLE_FRAME .0125

/*************************************************//**
*
*   Chart Element implementation for Sarvatobhadra.
*
******************************************************/
class SbcGrid : public CeFieldGrid
{
public:

	/*****************************************************
	**
	**   SbcGrid   ---   Constructor
	**
	******************************************************/
	SbcGrid( HybridChart *chart, CgVedicChartGrid *grid )
	: CeFieldGrid( chart, grid, 28 )
	{
		FieldWriterFactory factory;
		assert( ! proxy );
		proxy = factory.createSbcFieldWriter( this );

		Align gravitation = Align::None;

		for( int i = 0; i < 28; i++ )
		{
			if ( i >= (int)Nakshatra28::Krittika && i <= (int)Nakshatra28::Aslesha ) { gravitation = Align::West; }
			else if ( i >= (int)Nakshatra28::Magha && i <= (int)Nakshatra28::Visakha ) { gravitation = Align::North; }
			else if ( i >= (int)Nakshatra28::Anuradha && i <= (int)Nakshatra28::Sravana ) { gravitation = Align::East; }
			else { gravitation = Align::South; }

			fields.push_back( ChartField( chart, gravitation ));
		}

		// reset mark state
		for ( int i = 0; i < 9; i++ ) { mark[i].fill( 0 ); }

		const Viewpoint viewpoint = chart->getViewpoint();
		Horoscope_ptr horoscope = chart->getFirstHoroscope();
		if ( horoscope )
		{
			std::pair<int,int> n = getNakshatraIndex(
				::getNakshatra28( horoscope->getLongitude( PlanetId::Moon, viewpoint )));
			mark[n.first][n.second] = 1;
			n = getRasiIndex( getRasi( horoscope->getLongitude( PlanetId::Ascendant, viewpoint )));
			mark[n.first][n.second] = 1;
			n = getWeekdayIndex( horoscope->dl.getWeekDay());
			mark[n.first][n.second] = 1;
		}
	}

	/*****************************************************
	**
	**   SbcGrid   ---   handleMouseLeftEvent
	**
	******************************************************/
	virtual WidgetResponse handleMouseLeftEvent( const MComplex &p ) override
	{
		debuglog << "SbcGrid::handleMouseLeftEvent " << p;

		std::pair<int,int> f = getFieldIndexForPoint( p );
		if ( f.first >= 0 && f.first < 9 && f.second >= 0 && f.second < 9 )
		{
			debuglog << "MATCH " << p << " index " << f.first << " " << f.second
				<< " coord " << getFieldCoord( f.first, f.second );
			mark[f.first][f.second] = !mark[f.first][f.second];
			return WidgetResponse( getFieldCoord( f.first, f.second ));
		}
		return false;
	}

protected:
	/*****************************************************
	**
	**   SbcGrid   ---   setupFieldRectangles
	**
	******************************************************/
	virtual void setupFieldRectangles() override
	{
		xstep = ( p_east.real() - p_west.real()) / 9;

		ystep = ( p_south.imag() - p_north.imag()) / 9;

		if ( xstep <= 0 || ystep <= 0 ) return;
		assert( fields.size() == 28 );

		for( const Nakshatra28 &nak : Enum<Nakshatra28>() )
		{
			fields[(int)nak].rect = getNakshatraCoord( nak );
		}
		pFieldSplit = MComplex( .5 * fields[0].rect.width, .5 * fields[0].rect.height );
	}

	protected:
	/*****************************************************
	**
	**   SbcGrid   ---   writePlanetFields
	**
	******************************************************/
	virtual void writePlanetFields() override
	{
		assert( proxy );
		proxy->writePlanetFields();
		Lang lang;
		ContextProps_ptr props = chart->getProps();
		std::vector<NakshatraProperties> nakshatraProps;
		ChartConfig *cfg = chart->getChartConfig();

		Horoscope_ptr horoscope = chart->getFirstHoroscope();
		const bool showProps = ( horoscope && cfg->showSbcNakshatraQuality );
		if ( showProps )
		{
			nakshatraProps = NakshatraExpert( props ).getSbcNakshatraProperties(
				(int)getNakshatra28( horoscope->getVedicLongitude( PlanetId::Moon )));
		}

		for ( uint i = 0; i < 28; i++ )
		{
			wxString s;
			s << lang.getNakshatra28Name( (Nakshatra28)i, cfg->textFormat );

			if ( showProps )
			{
				assert( nakshatraProps.size() == field_count );
				switch( nakshatraProps[i].taraQuality )
				{
					case InfluenceType::Benefic:
						s << " B";
					break;
					case InfluenceType::Malefic:
						s << " M";
					break;
					default:
						s << " N";
					break;
				}
			}
			fields[i].label = s;
		}
	}

	/*****************************************************
	**
	**   SbcGrid   ---   startPaint
	**
	******************************************************/
	virtual void startPaint( Painter *painter ) override
	{
		painter->setPen( getPen() );
		painter->setBrush( chart->getSkinnedMarkedFieldBrush());

		for ( int i = 0; i < 9; i++ )
		{
			for ( int j = 0; j < 9; j++ ) if ( mark[i][j] ) painter->drawRectangle( getFieldCoord( i, j ));
		}
		setInheritedStyle( painter );
		painter->setTransparentBrush();
	}

	/*****************************************************
	**
	**   SbcGrid   ---   paintGrid
	**
	******************************************************/
	virtual void paintGrid( Painter *painter, const MRect &refreshRect ) override
	{
		debuglog << "SbcGrid::paintGrid";
		Lang lang;
		MRect rect, fieldrect;
		wxString s;
		const WidgetSkin skin = chart->getSkin();
		DocHelper util( painter );

		setInheritedStyle( painter );
		painter->setTransparentBrush();

		for ( int i = 1; i <= 8; i++ )
		{
			// vertical line
			painter->drawLine(
				p_west.real() + i * xstep,
				p_north.imag(),
				p_west.real() + i * xstep,
				p_south.imag() );

			// horizontal line
			painter->drawLine(
				p_west.real(),
				p_north.imag() + i * ystep ,
				p_east.real(),
				p_north.imag() + i * ystep );
		}


		// top line (corners only)
		const bool devasymbol = chart->getChartConfig()->showSbcDevanagariCharacters;
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0908") : wxT("ee"), 0, 0 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0905") : wxT("a"), 8, 0 );

		// Line 2
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u090B") : wxT("ri"), 1, 1 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0917") : wxT("g"), 2, 1 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0938") : wxT("s"), 3, 1 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0926") : wxT("d"), 4, 1 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u091A") : wxT("ch"), 5, 1 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0932") : wxT("j"), 6, 1 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0909") : wxT("u"), 7, 1 );

		// Line 3
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0916") : wxT("kh"), 1, 2 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0910") : wxT("ai"), 2, 2 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u090C") : wxT("li"), 6, 2 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0905") : wxT("a"), 7, 2 );

		// Line 4
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u091C") : wxT("j"), 1, 3 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0905\u0903") : wxT("ah"), 3, 3 );
		drawFieldLabel( painter, refreshRect, lang.getWeekdayName(5).substr(0, 3), 4, 3 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0913") : wxT("o"), 5, 3 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0935") : wxT("v"), 7, 3 );

		// Line 5
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u092D") : wxT("bh"), 1, 4 );
		drawFieldLabel( painter, refreshRect, lang.getWeekdayName(4).substr(0, 3), 3, 4 );
		drawFieldLabel( painter, refreshRect, lang.getWeekdayName(6).substr(0, 3), 4, 4 );

		s = lang.getWeekdayName(0).substr(0, 3) + " " + lang.getWeekdayName(2).substr(0, 3 );
		drawFieldLabel( painter, refreshRect, s, 5, 4);
		drawFieldSymbol(painter, refreshRect, devasymbol ? wxT("\u0915") : wxT("k"), 7, 4 );

		// Line 6
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u092F") : wxT("y"), 1, 5 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0905\u0902") : wxT("am"), 3, 5 );

		s = lang.getWeekdayName( 1 ).substr(0,3) + " " + lang.getWeekdayName( 3 ).substr( 0,3 );
		drawFieldLabel(  painter, refreshRect, s, 4, 5 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0914") : wxT("au"), 5, 5 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0939") : wxT("h"), 7, 5 );

		// Line 7
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0928") : wxT("n"), 1, 6 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u090F") : wxT("e"), 2, 6 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u090C") : wxT("li"), 6, 6 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0921") : wxT("d"), 7, 6 );

		// Line 8
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0916") : wxT("kh"), 1, 7 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0924") : wxT("t"), 2, 7 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u0930") : wxT("r"), 3, 7 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u092A") : wxT("p"), 4, 7 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u091F") : wxT("th"), 5, 7 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u092E") : wxT("m"), 6, 7 );
		drawFieldSymbol( painter, refreshRect, devasymbol ? wxT("\u090A") : wxT("uu"), 7, 7 );

		// Line 9
		drawFieldSymbol(painter, refreshRect, devasymbol ? wxT("\u0907") : wxT("i"), 0, 8);
		drawFieldSymbol(painter, refreshRect, devasymbol ? wxT("\u0906") : wxT("aa"), 8, 8);

		// Signs
		const bool useSymbols = chart->getChartConfig()->textFormat == TextFormat::Symbol;
		if ( useSymbols ) chart->setSymbolFont( painter );
		drawSignSymbol( painter, refreshRect, Sign::Aries, 5, 2 );
		drawSignSymbol( painter, refreshRect, Sign::Taurus, 6, 3 );
		drawSignSymbol( painter, refreshRect, Sign::Gemini, 6, 4 );
		drawSignSymbol( painter, refreshRect, Sign::Cancer, 6, 5 );
		drawSignSymbol( painter, refreshRect, Sign::Leo, 5, 6 );
		drawSignSymbol( painter, refreshRect, Sign::Virgo, 4, 6 );
		drawSignSymbol( painter, refreshRect, Sign::Libra, 3, 6 );
		drawSignSymbol( painter, refreshRect, Sign::Scorpio, 2, 5 );
		drawSignSymbol( painter, refreshRect, Sign::Sagittarius, 2, 4 );
		drawSignSymbol( painter, refreshRect, Sign::Capricorn, 2, 3 );
		drawSignSymbol( painter, refreshRect, Sign::Aquarius, 3, 2 );
		drawSignSymbol( painter, refreshRect, Sign::Pisces, 4, 2 );

		if ( useSymbols ) chart->setGraphicFont( painter );
	}

	/*****************************************************
	**
	**   SbcGrid   ---   drawSignSymbol
	**
	******************************************************/
	void drawSignSymbol( Painter *painter, const MRect &refreshRect, const Sign &sign, const int &col, const int &row )
	{
		const TextFormat format = chart->getChartConfig()->textFormat;
		const Viewpoint viewpoint = chart->getViewpoint();

		const MRect fieldrect = getFieldCoord( col, row );
		if ( fieldrect.intersects( refreshRect ))
		{
			DocHelper util( painter );
			if ( format == TextFormat::Symbol )
			{
				const PlanetPropertyConfig &cfg = astroconfig->getViewpointConfig( chart->getViewpoint() ).planetProps;
				util.drawTextFormatted(
					SymbolProvider().getSignCode( sign, cfg.capricornSymbol ),
					fieldrect,
					Align::South );
			}
			else
			{
				util.drawTextFormatted(
					Lang().getSignName( sign, viewpoint, format ),
					fieldrect,
					Align::South );
			}
		}
	}

	/*****************************************************
	**
	**   SbcGrid   ---   drawFieldLabel
	**
	******************************************************/
	void drawFieldLabel(Painter* painter, const MRect& refreshRect, const wxString &label, const int& col, const int& row)
	{
		const MRect fieldrect = getFieldCoord(col, row);
		if (fieldrect.intersects(refreshRect))
		{
			DocHelper util(painter);
			util.drawTextFormatted( label, fieldrect, Align::South );
		}
	}

	/*****************************************************
	**
	**   SbcGrid   ---   drawFieldSymbol
	**
	******************************************************/
	void drawFieldSymbol(Painter* painter, const MRect& refreshRect, const wxChar* label, const int& col, const int& row)
	{
		const MRect fieldrect = getFieldCoord(col, row);

		if (fieldrect.intersects(refreshRect))
		{
			DocHelper util(painter);
			util.drawTextFormatted( label, fieldrect, Align::South );
		}
	}

	/*****************************************************
	**
	**   SbcGrid   ---   getFieldIndexForPoint
	**
	******************************************************/
	std::pair<int,int> getFieldIndexForPoint( const MComplex &p )
	{
		std::pair<int,int> r = std::pair( -1, -1 );
		assert( xstep > 0 );
		assert( ystep > 0 );

		if ( p.real() >= p_west.real() && p.real() <= p_east.real()
			&& p.imag() >= p_north.imag() && p.imag() <= p_south.imag() )
		{
			r.first = (int)(( p.real() - p_west.real() ) / xstep );
			r.second = (int)(( p.imag() - p_north.imag() ) / ystep );
			assert( r.first >= 0 && r.first < 9 );
			assert( r.second >= 0 && r.second < 9 );
		}
		return r;
	}

	/*****************************************************
	**
	**   SbcGrid   ---   updateAspects
	**
	******************************************************/
	virtual void updateAspects( Horoscope_ptr aspecting_h, Horoscope_ptr /*aspected_h*/ ) override
	{
		ContextProps_ptr props = chart->getProps();
		for( const auto &asp :  VedicAspectExpert( props ).calculateSbcAspects( aspecting_h ))
		{
			aspects.push_back( asp );
		}
	}

	/*****************************************************
	**
	**   SbcGrid   ---   isAspectValid
	**
	******************************************************/
	virtual bool isAspectValid( const  VedicAspectId &a ) const  override
	{
		return( a == VedicAspectId::Vedha );
	}

	/*****************************************************
	**
	**   SbcGrid   ---   doPaintFieldBackground
	**
	******************************************************/
	virtual bool doPaintFieldBackground( const uint &f ) override
	{
		const std::pair<int, int> marked = getNakshatraIndex( (Nakshatra28)f );
		return f != activeField && ! mark[marked.first][marked.second];
	}

	/*****************************************************
	**
	**   SbcGrid   ---   isFieldMarked
	**
	******************************************************/
	/*
	bool isFieldMarked( const uint &f ) const
	{
		// TODO
		const std::pair<int, int> marked = getNakshatraIndex( (Nakshatra28)f );
		return false;
	}
	*/

	/*****************************************************
	**
	**   SbcGrid   ---   getFieldCoord
	**
	******************************************************/
	MRect getFieldCoord( const std::pair<int,int> &p ) const
		{ return getFieldCoord( p.first, p.second ); }

	MRect getFieldCoord( const int &x, const int &y ) const
	{
		MRect r( p_west.real() + x * xstep, p_north.imag() + y * ystep, xstep, ystep );
		return r;
	}

	/*****************************************************
	**
	**   SbcGrid   ---   getWeekdayIndex
	**
	******************************************************/
	std::pair<int,int> getWeekdayIndex( const int &w )
	{
		assert( w >= 0 && w < 7 );
		switch ( w )
		{
		case 0:
		case 2:
			return std::pair( 5, 4 );
			break;
		case 1:
		case 3:
			return std::pair( 4, 5 );
			break;
		case 4:
			return std::pair( 3, 4 );
			break;
		case 5:
			return std::pair( 4, 3 );
			break;
		default:
			return std::pair( 4, 4 );
			break;
		}
	}

	/*****************************************************
	**
	**   SbcGrid   ---   getRasiIndex
	**
	******************************************************/
	std::pair<int,int> getRasiIndex( const Sign &r )
	{
		assert_valid_enum<Sign>( r );
		switch ( r )
		{
		case Sign::Aries:
			return std::pair( 5, 2 );
			break;
		case Sign::Taurus:
			return std::pair( 6, 3 );
			break;
		case Sign::Gemini:
			return std::pair( 6, 4 );
			break;
		case Sign::Cancer:
			return std::pair( 6, 5 );
			break;
		case Sign::Leo:
			return std::pair( 5, 6 );
			break;
		case Sign::Virgo:
			return std::pair( 4, 6 );
			break;
		case Sign::Libra:
			return std::pair( 3, 6 );
			break;
		case Sign::Scorpio:
			return std::pair( 2, 5 );
			break;
		case Sign::Sagittarius:
			return std::pair( 2, 4 );
			break;
		case Sign::Capricorn:
			return std::pair( 2, 3 );
			break;
		case Sign::Aquarius:
			return std::pair( 3, 2 );
			break;
		case Sign::Pisces:
		default:
			return std::pair( 4, 2 );
			break;
		}
	}

	/*****************************************************
	**
	**   SbcGrid   ---   getNakshatraIndex
	**
	******************************************************/
	std::pair<int,int> getNakshatraIndex( const Nakshatra28 &nak ) const
	{
		assert_valid_enum<Nakshatra28>( nak );

		const int i = (int)nak;
		if ( i < 2 )
		{
			return std::pair( i + 6, 0 );
		}
		else if ( i >= 2 && i <= 8 )
		{
			return std::pair( 8, i - 1 );
		}
		else if ( i >= 9 && i <= 15 )
		{
			return std::pair( 16 - i, 8 );
		}
		else if ( i >= 16 && i <= 22 )
		{
			return std::pair( 0, 8 - ( i - 15 ));
		}
		else
		{
			return std::pair( i - 22, 0 );
		}
	}

	/*****************************************************
	**
	**   SbcGrid   ---   getNakshatraCoord
	**
	******************************************************/
	MRect getNakshatraCoord( const Nakshatra28 &i )
	{
		assert_valid_enum<Nakshatra28>( i );
		return getFieldCoord( getNakshatraIndex( i ));
	}

	double xstep, ystep;
	std::array<std::array<bool, 9>, 9> mark;
};

/*****************************************************
**
**   ChartElementFactory   ---   createSbcGrid
**
******************************************************/
ChartElementBase *ChartElementFactory::createSbcGrid( CgElement *cge ) const
{
	CgVedicChartGrid *grid = dynamic_cast<CgVedicChartGrid*>( cge );
	assert( grid );
	return new SbcGrid( chart, grid );
}

