/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Dasa.h
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
#ifndef _DASA_H_
#define _DASA_H_

#include "ConfigListItems.h"
#include "Expert.h"
#include "mstring.h"
#include "Viewpoint.h"

#include <list>
#include <memory>
#include <variant>
#include <wx/string.h>

class Dasa;
typedef std::shared_ptr<Dasa> Dasa_ptr;

class DasaExpert;
typedef std::shared_ptr<DasaExpert> DasaExpert_ptr;

#define DLORD_NONE DasaLord( PlanetId::None ) 

/**
* Dasa levels logic:
*
* Root is -1
* Mahadasa is 0
* Antardasas and deeper levels start at index 1.
*
* There may be levels below -1 in tree view because the folders may be parents of root Dasa.
*
*/
enum {
  DASALEVEL_ROOT = -1,
  DASALEVEL_MAHADASA,
  DASALEVEL_ANTARDASA
};

#define DASALEVEL_TREEROOT DASALEVEL_ROOT - 2 // = -3
#define DASALEVEL_FOLDER DASALEVEL_ROOT - 1   // = -2

/*************************************************//**
*
* \brief Data container with Dasa's attributes like lord, start, end, parent Dasa
*
* This class is inherited by KalachakraDasa which has more attributes.
*
* The class GraphicalDasa in *gdi* layer has more features like text representation
* and aggregation of children.
*
******************************************************/
class Dasa
{
	friend class AshtottariDasaExpert;
	friend class GenericNakshatraDasaExpert;

public:

	/**
	*  Constructor for a Dasa with expert and parent.
	*
	*/
	Dasa( DasaExpert_ptr, Dasa_ptr parent = nullptr, const DasaLevel = DasaLevel::Root, const DasaLord = DLORD_NONE );

	/**
	*  Constructor for a Dasa with expert and parent.
	*
	*/
	Dasa( Dasa_ptr parent );

	/**
	* Used by Solardasa
	*/
	Dasa( Dasa_ptr parent, const DasaLord&, const double &startjd, const double &endjd );

	virtual ~Dasa();

	DasaExpert_ptr getExpert() const { return expert; }

	DasaId getDasaId() const;

	/**
	*  Calculation depth of the Dasa.
	*
	*  Surrounding folders is tree representation have special (negative) levels
	*/
	DasaLevel getLevel() const { return level; }

	DasaLord getDasaLord() const { return lord; }

	double getStartJD() const { return start_jd; }

	double getEndJD() const { return end_jd; }

	/**
	* Time between birth and the end of the first Mahadasa.
	*/
	double getBalance() const { return balance; }

	/**
	* Total duration of one Mahadasa cycle. This is fixed in case of most Dasas (e.g. 120 year for Vimsottari)
	* but may also be more specific, e.g Kalachakra Dasa has four different Paramayus values. According to
	* the calculation rule the Paramayus can change if one cycle of Mahadasa is fulfilled.
	*/
	double getParamayus() const { return paramayus; }

	/*
	* Parent Dasa object or null.
	*/
	Dasa_ptr getParent() const { return parent; }

protected:

	DasaExpert_ptr expert;

	Dasa_ptr parent;

	DasaLord lord = DLORD_NONE;

	double start_jd = 0.0;

	double end_jd = 0.0;

	double balance = 0.0;

	double paramayus = 0.0;

	const DasaLevel level = DasaLevel::Root;

#ifdef DEBUG_SHARED_PTR_LIFECYLCE

	inline static int nb_created = 0;

	inline static int nb_deleted = 0;
#endif
};

/*************************************************//**
*
*  \brief Data container for three Krishnamurti Paddhati (kp) lords.
*
******************************************************/
struct KpData
{
	KpData( const DasaLord l = DLORD_NONE, const DasaLord sl = DLORD_NONE, const DasaLord ssl = DLORD_NONE )
		: lord( l ),
			sublord( sl ),
			subsublord( ssl )
	{
	}

	DasaLord lord = DLORD_NONE;

	DasaLord sublord = DLORD_NONE;

	DasaLord subsublord = DLORD_NONE;

	friend bool operator==( const KpData&, const KpData& );
};

/*************************************************//**
*
*  \brief Event in KP calculation (used Ephemeris view)
*
******************************************************/
struct KPEvent
{
public:

	const DasaLord lord = DLORD_NONE;

	const DasaLord sublord = DLORD_NONE;

	const double len = 0;

	double jd = 0;

	const int dasaindex = 0;
};

/*************************************************//**
*
* \brief Central class for Dasa calculation.
*
* This class is abstract. Implementations are KalachakraDasaExpert, AshtotattariDasaExpert
* and GenericDasaExpert (with implementations for all other Dasas incl Vimsottari and Yogini).
*
* Required steps for Dasa calculation
* --------------------------------
*
* 1. createBlankRoot( DasaExpert_ptr, Dasa_ptr parent )
*  		is the overal root and container for Mahadasas.
*     the parent Dasa can be null or a text based tree node (Dasa Tree view).
*
*	2. std::list<Dasa_ptr> getFirstLevel( ContextProps_ptr, Dasa_ptr root, const int maxMahadasaCount = 0 )
*     creates the list of Mahadasas.
*			maxMahadasaCount > 0 can be used to specify the exact number of Mahadasas, otherwise this number will be
*			determined by DasaCalculationConfig.startOfLastMahadasa (number of years from birth for last Mahadasa start).
* 
* 3...n  std::list<Dasa_ptr> getNextLevel( ContextProps_ptr, Dasa_ptr parent )
*   creates Antardasas, Pratyantardasas etc based on the containing Dasa
*
*
******************************************************/
class DasaExpert
{
public:

	DasaExpert( const DasaId &dasaId ) : dasaId( dasaId ) {}

	virtual ~DasaExpert();

	/**
	* Get JD for start of last Mahadasa, depends on DasaConfig.startOfLastMahadasa (in years).
	*/
	static double getMaxEndJD( Horoscope_ptr );

	/**
	* Convenience method: get year length (in years), depends on DasaConfig.yearLength.
	*/
	static double getYearLength();

	static bool isPlanetDasa( const DasaId &dasaId ) { return dasaId != DasaId::Kalachakra; }

	/**
	* DasaExpert_ptr ref must point to "this".
	*/
	Dasa_ptr createBlankRoot( DasaExpert_ptr, Dasa_ptr = nullptr );

	/**
	* Calculate the Mahadasa level.
	* 
	* Root Dasa as input required. Horoscope reference is contained in ContextProperties.
	*
	* Number of Mahadasas is either determined by DasaCalculationConfig.startOfLastMahadasa or
	* (override) maxMahadasaCount if > 0
	*/
	virtual std::list<Dasa_ptr> getFirstLevel( ContextProps_ptr, Dasa_ptr root, const int maxMahadasaCount = 0 ) = 0;

	/**
	* Calculate the Antardasa and deeper levels.
	*
	* Parent Dasa as input required.
	*/
	virtual std::list<Dasa_ptr> getNextLevel( ContextProps_ptr, Dasa_ptr ) = 0;

	DasaId getDasaId() const { return dasaId; }

	DasaLord getBirthMahaDasaLord( DasaExpert_ptr, ContextProps_ptr );

	virtual void writeDasaHeader( MDocument_ptr, Horoscope_ptr, Dasa_ptr );

	virtual void describeDasa( MDocument_ptr, Horoscope_ptr, Dasa_ptr );

	MString getDasaLordName( Dasa* );
	MString getDasaLordName( Dasa*, const PlanetConfig&, const PlanetPropertyConfig& );

	MString getDasaLordName( Dasa_ptr d ) { return getDasaLordName( d.get()); }
	MString getDasaLordName( Dasa_ptr d, const PlanetConfig &fcfg, const PlanetPropertyConfig &pcfg )
		{ return getDasaLordName( d.get(), fcfg, pcfg ); }

	MString getDasaLordName( const DasaLord& );
	MString getDasaLordName( const DasaLord&, const PlanetConfig&, const PlanetPropertyConfig& );

	virtual wxString getExtraLordName( Dasa_ptr, const PlanetConfig&, const PlanetPropertyConfig& )
		{ return ""; }

	/**
	*   Dasa lord and extra description (Yogini lord, gatis) if available.
	*/
	MString getDasaDescription( Dasa_ptr, const PlanetConfig&, const PlanetPropertyConfig& );

	virtual KpData getKPLords( const double& ) = 0;
	virtual bool hasKpFeatures() const { return false; }
	virtual std::list<KPEvent> getKPEventList( const double &startlen, const double &Endlen, const double &startjd ) = 0;

	virtual int getNbLords() const = 0;

protected:

	/*
	* To be overridden if KalachakraDasa is used.
	*/
	virtual Dasa_ptr createNewDasa( DasaExpert_ptr, Dasa_ptr parent, const DasaLevel& );

	const DasaId dasaId;
};

/*************************************************//**
*
* \brief Factory creating references to Dasa Expert instances.
*
******************************************************/
class DasaExpertFactory
{
public:

	DasaExpert *getDasaExpert( const DasaId& );

private:

	DasaExpert *getGenericDasaExpert( const DasaId& );

	DasaExpert *getKalachakraDasaExpert();

	DasaExpert *getAshtottariDasaExpert();
};

#endif

