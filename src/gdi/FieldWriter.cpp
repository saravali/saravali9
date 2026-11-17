/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/FieldWriter.cpp
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

#include "FieldWriter.h"

#include "Ashtakavarga.h"
#include "astrobase.h"
#include "astroconfig.h"
#include "CeFieldGrid.h"
#include "ChartConfig.h"
#include "ChartGeometry.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "Horoscope.h"
#include "HybridChart.h"
#include "Jaimini.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "Varga.h"

#include <format>

extern Astroconfig *astroconfig;

/*****************************************************
**
**   FieldWriter   ---   getPlanetRetro
**
******************************************************/
bool FieldWriter::getPlanetRetro( const PlanetId &planet, const HoroscopeRole &role )
{
	HybridChart *chart = grid->getChart();
	const Viewpoint &viewpoint = chart->getViewpoint();
	ContextProps_ptr props = chart->getProps();

	Horoscope_ptr h = props->getHoroscope( role );
	if ( ! h )
	{
		warnlog << "horoscope in VedicVargaChart::getPlanetField for role " << role << " is NULL, returning Aries";
		return false;
	}
	return h->isRetrograde( planet, viewpoint );
}

/*************************************************//**
*
*     CLASS    ---   VargaFieldWriter
*
******************************************************/
class VargaFieldWriter : public FieldWriter
{
public:

	/*****************************************************
	**
	**   VargaFieldWriter   ---   Constructor
	**
	******************************************************/
	VargaFieldWriter( CeFieldGrid *grid ) : FieldWriter( grid ) {}

	/*****************************************************
	**
	**   VargaFieldWriter   ---   writePlanetFields
	**
	******************************************************/
	virtual void writePlanetFields() override
	{
		debuglog << "VargaFieldWriter::writePlanetFields";
		assert( grid->fields.size() == grid->field_count );
		uint field;
		HybridChart *chart = grid->chart;

		const WidgetCategory &category = chart->getWidgetCategory();
		ContextProps_ptr props = chart->getProps();
		const DataContext datatype = props->getDataContext();

		const Viewpoint &viewpoint = chart->getViewpoint();
		ChartConfig *chartConfig = chart->getChartConfig( WidgetCategory::VargaChart );
		PlanetConfig planetCfg = astroconfig->getViewpointConfig( viewpoint ).planets;
		planetCfg.textFormat = chartConfig->textFormat;

	  AshtakavargaExpert *ashtakavargaExpert = nullptr;
		const bool doAv = datatype == DataContext::Direction
			&& category == WidgetCategory::VargaChart
			&& chartConfig->showAshtakavarga;
		if ( doAv )
		{
			ashtakavargaExpert = new AshtakavargaExpert( chart->getProps(), chart->getVarga());
			ashtakavargaExpert->update();
		}

		for ( const PlanetId planet : props->getPlanets( viewpoint, { FilterGroup::Houses, FilterGroup::CardinalPoints } ))
		{
			debuglog << "VargaFieldWriter::writePlanetFields: " << planet;
			for( const HoroscopeRole &role : chart->getChartRoles())
			{
				// loop for ascendant in south indian chart if asc is painted as diagonal sign markup
				if ( planet == PlanetId::Ascendant && role == HoroscopeRole::Radix )
				{
					const ChartGeometry &geom = chart->getGeometry();
					if ( geom.containsElementType( CgElementType::SouthGrid )
						&& chartConfig->southIndianAscendantMarkup != 0 )
					{
						continue;
					}
				}

				Horoscope_ptr h = props->getHoroscope( role );
				if ( ! h ) continue;
				VargaExpert expert( h.get(), viewpoint );

				field = a_red( getPlanetField( planet, role ) - grid->positionOffset, grid->field_count );
				debuglog << "VargaFieldWriter::writePlanetFields: " << (int)planet << " field " << field
					<< " offset " << grid->positionOffset;

				if ( field < grid->fields.size())
				{
					// TODO raus im Grid
					//MString &tf = grid->fields[field].getContents( chart_id );
					MString &tf = grid->fields[field].getContents( role );

					if ( ! tf.isEmpty()) tf << MToken( TokenId::Space );
					
					// retrogression 
					if ( chartConfig->retroStyle != RetroDisplayStyle::Nothing
						&& getPlanetRetro( planet, role )
						&& ( ! PlanetHelper::isNode( planet )
							|| astroconfig->getViewpointConfig( viewpoint ).calculation.lunarNodeMode == LunarNodeMode::True ))
					{
						switch( chartConfig->retroStyle )
						{
							case RetroDisplayStyle::SimpleR: // planet(r)
								tf << MToken( planet, planetCfg ) << MToken( MovingDirection::Retrograde );
							break;
							case RetroDisplayStyle::Brackets: // (planet)
								tf << "(" << MToken( planet, planetCfg ) << ")";
							break;
							case RetroDisplayStyle::Symbol:
							default:
								tf << MToken( planet, planetCfg ) << MToken( MovingDirection::Retrograde, TextFormat::Symbol );
							break;
						}
					}
					else
					{
						tf << MToken( planet, planetCfg );
					}

					if ( doAv && role != HoroscopeRole::Radix && PlanetHelper::isAvPlanet( planet ))
					{
						assert( field < 12 );
						const int av = ashtakavargaExpert->getItem( AvType::Rekha, planet,
							(Sign)getPlanetField( planet, role ));
						tf << " " << std::format( "{}", av );
					}
				}
				else
				{
					errorlog << "field count " << grid->field_count  << " too small for index " << field;
				}
			}
		}

		// Arudhas for chart id 0
		if ( chartConfig->showArudhas )
		{
			int rasi;
			wxString lname, sname;

			Horoscope_ptr h = chart->getFirstHoroscope();
			assert( h );
			assert( chart->getVarga() != Varga::None );

			JaiminiExpert expert( chart->getProps(), chart->getVarga() );

			for ( int i = 0; i < 12; i++ )
			{
				rasi = red12( expert.calcPada( i ) - grid->positionOffset );

				if ( i == 0 )
				{
					sname = tr( "AL" );
					lname = tr( "Arudha L" );
				}
				else if ( i == 11 )
				{
					sname = tr( "UL" );
					lname = tr( "Upapada L" );
				}
				else
				{
					sname = "A" + std::format( "{}", i+1 );
					lname = sname;
				}
				//TODO kurz oder lang abhängig von WriterConfig
				grid->fields[rasi].getContents( chart->getFirstRole() ) << " " << lname;
				//fields[rasi].getContents( chart_id ).textitems.push_back( ChartTextItem( lname, sname, false ));
			}
		}
	  if ( ashtakavargaExpert ) delete ashtakavargaExpert;

		for( auto &field: grid->fields )
		{
			debuglog << "Field Contents: " << field.getContents( chart->getFirstRole());
		}
	}

	/*****************************************************
	**
	**   VargaFieldWriter   ---   getCenterString
	**
	******************************************************/
	virtual MString getCenterString() override
	{
		MString tf;
		wxString signification;
		Lang lang;

		HybridChart *chart = grid->chart;
		const Varga varga = chart->getVarga();

		const VedicChartCenterType type = chart->getChartConfig( WidgetCategory::VargaChart )->centerInfoType;
		if ( type == VedicChartCenterType::Nothing ) return tf;

		if ( varga == Varga::Bhava )
		{
			tf << tr( "Bhava" );
		}
		else
		{
			switch( type )
			{
				case VedicChartCenterType::VargaName:
					tf << lang.getVargaName( varga );
				break;
				case VedicChartCenterType::Signification:
					 tf << lang.getVargaName( varga );
				signification = lang.getVargaSignification( varga );
				if ( ! signification.empty() )
				{
				 tf << MToken( TokenId::LineBreak ) << signification;
				}
			break;
			case VedicChartCenterType::Division:
			{
				tf << std::format( "D-{}", VargaExpert::getVargaDivision( varga ));
			}
			break;
			case VedicChartCenterType::DivisionSignification:
			{
				tf << std::format( "D-{}\n{}", VargaExpert::getVargaDivision( varga ), lang.getVargaSignification( varga ));
			}
			break;
			case VedicChartCenterType::Nothing:
			default:
			break;
		}
	}
	return tf;
	}

	protected:

	/*****************************************************
	**
	**   VargaFieldWriter   ---   getPlanetField
	**
	******************************************************/
	virtual int getPlanetField( const PlanetId &planet, const HoroscopeRole &role ) const override
	{
		HybridChart *chart = grid->chart;
		ContextProps_ptr props = chart->getProps();
		Horoscope_ptr h = props->getHoroscope( role );
		const Viewpoint &viewpoint = chart->getViewpoint();

		if ( ! h )
		{
			warnlog << "horoscope in VedicVargaChart::getPlanetField for role " << role << " is NULL, returning Aries";
			return 0;
		}
		VargaExpert expert( h.get(), viewpoint );
		return (int)expert.calcVarga( h->getLongitude( planet, viewpoint ), chart->getVarga() );
	}
};

/*************************************************//**
*
*  
*
******************************************************/
class AshtakavargaFieldWriter : public FieldWriter
{
public:

	/*****************************************************
	**
	**   AshtakavargaFieldWriter   ---   Constructor
	**
	******************************************************/
	AshtakavargaFieldWriter( CeFieldGrid *grid, const AvType &avtype, AshtakavargaExpert *expert, const PlanetId &ob )
	: FieldWriter( grid ),
		avtype( avtype ),
		expert( expert ),
		ob( ob )
	{
	}

	/*****************************************************
	**
	**   AshtakavargaFieldWriter   ---   writePlanetFields
	**
	******************************************************/
	virtual void writePlanetFields() override
	{
		debuglog << "AshtakavargaFieldWriter::writePlanetFields " << (int)ob ;
		assert( grid->fields.size() == grid->field_count );
		assert( expert );

		for ( int i = 0; i < 12; i++ )
		{
			MString &tf = grid->fields[i].getContents( HoroscopeRole::Radix );
			if ( ob == PlanetId::None )
			{
				tf << createFieldContents( expert->getSignSarva( avtype, (Sign)i ), false );
			}
			else
			{
				tf << createFieldContents( expert->getItem( avtype, ob, (Sign)i ), true );
			}

		}
	}

	/*****************************************************
	**
	**   AshtakavargaFieldWriter   ---   getCenterString
	**
	******************************************************/
	virtual MString getCenterString() override
	{
		if ( ob == PlanetId::None )
		{
			return MString( tr( "Sarva" ));
		}
		else
		{
			return MToken( ob, grid->getChart()->getViewpoint() );
		}
	}

	protected:

	/*****************************************************
	**
	**   AshtakavargaFieldWriter   ---   createFieldContents
	**
	******************************************************/
	MString createFieldContents( const int &v, const bool &graphicSupport )
	{
		MString m;
		wxString s;
		/*
		astroconfig->ashtakavarga.ashtakavargaNumberMode:
			0: number
			1: line
			2: dot
		*/
		int mode = astroconfig->ashtakavarga.ashtakavargaNumberMode;
		if ( graphicSupport == false ) mode = 0;


		if ( graphicSupport && mode )
		{
			const char avitem = mode == 1 ? 'I' : 'o';
			for ( int j = 0; j < v; j++ )
			{
				if ( j > 0 && ! ( j % 5 ))
				{
					m << s << " ";
					s.clear();
				}
				s += avitem;
			}
			m << s;
		}
		else
		{
			m << std::format( "{}", v );
		}
		return m;
	}

	const AvType avtype;
	AshtakavargaExpert *expert;
	const PlanetId ob;
};

/*************************************************//**
*
*  
*
******************************************************/
class SbcFieldWriter : public FieldWriter
{
public:

	/*****************************************************
	**
	**   SbcFieldWriter   ---   Constructor
	**
	******************************************************/
	SbcFieldWriter( CeFieldGrid *grid ) : FieldWriter( grid ) {}

	virtual void writePlanetFields() override
	{
		debuglog << "SbcFieldWriterProxy::writePlanetFields";
		assert( grid->fields.size() == grid->field_count );
		uint field;
		HybridChart *chart = grid->chart;
		ContextProps_ptr props = chart->getProps();
		ChartConfig *sbccfg = chart->getChartConfig( WidgetCategory::SarvatobhadraChart );

		const Viewpoint &viewpoint = chart->getViewpoint();
		PlanetConfig planetCfg = astroconfig->getViewpointConfig( viewpoint ).planets;
		planetCfg.textFormat = sbccfg->textFormat;

		for ( const PlanetId planet : props->getPlanets( viewpoint, { FilterGroup::Houses, FilterGroup::CardinalPoints } ))
		{
			for( const HoroscopeRole &role : chart->getChartRoles())
			{
				Horoscope_ptr h = props->getHoroscope( role );
				if ( ! h ) continue;
				VargaExpert expert( h.get(), viewpoint );

				field = a_red( getPlanetField( planet, role ) - grid->positionOffset, grid->field_count );

				if ( field < grid->fields.size())
				{
					MString &tf = grid->fields[field].getContents( role );

					if ( ! tf.isEmpty()) tf << MToken( TokenId::Space );

					// retrogression 
					if ( sbccfg->retroStyle != RetroDisplayStyle::Nothing
						&& getPlanetRetro( planet, role )
						&& PlanetHelper::isNode( planet ))
					{
						switch( sbccfg->retroStyle )
						{
							case RetroDisplayStyle::SimpleR: // planet(r)
								tf << MToken( planet, planetCfg ) << MToken( MovingDirection::Retrograde );
							break;
							case RetroDisplayStyle::Brackets: // (planet)
								tf << "(" << MToken( planet, planetCfg ) << ")";
							break;
							case RetroDisplayStyle::Symbol:
							default:
								tf << MToken( planet, planetCfg );
							break;
						}
					}
					else
					{
						tf << MToken( planet, planetCfg );
					}

				}
				else
				{
					errorlog << "field count " << grid->field_count  << " too small for index " << field ;
				}
			}
		}
	}

	virtual MString getCenterString() override
		{ return MString( "" ); }

protected:

	virtual int getPlanetField( const PlanetId &planet, const HoroscopeRole &role ) const override
	{
		ContextProps_ptr props = grid->chart->getProps();
		Horoscope_ptr h = props->getHoroscope( role );
		const Viewpoint &viewpoint = grid->chart->getViewpoint();

		if ( ! h )
		{
			warnlog << "horoscope in VedicVargaChart::getPlanetField for role " << role << " is NULL, returning Aries";
			return 0;
		}
		return (int)::getNakshatra28( h->getLongitude( planet, viewpoint ));
	}
};

/*************************************************//**
*
*   FACTORY
*
******************************************************/
FieldWriter *FieldWriterFactory::createVargaFieldWriter( CeFieldGrid *grid )
{ return new VargaFieldWriter( grid ); }

FieldWriter *FieldWriterFactory::createSbcFieldWriter( CeFieldGrid *grid )
{ return new SbcFieldWriter( grid ); }

FieldWriter *FieldWriterFactory::createAshtakavargaFieldWriter( CeFieldGrid *grid,
	const AvType &avtype, AshtakavargaExpert *expert, const PlanetId &ob )
	{ return new  AshtakavargaFieldWriter( grid, avtype, expert, ob ); }



