/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/GenericDasa.h
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

#ifndef _GENERICDASA_H_
#define _GENERICDASA_H_

#include "Dasa.h"

/*************************************************//**
*
* \brief expert class for generic Dasas (following the pattern of Vimsottari, Yogini etc.)
*
******************************************************/
class GenericNakshatraDasaExpert : public DasaExpert
{
public:

	GenericNakshatraDasaExpert( const DasaId &dasa ) : DasaExpert( dasa ) {}

	virtual std::list<Dasa_ptr> getFirstLevel( ContextProps_ptr, Dasa_ptr, const int maxMahadasaCount = 0 ) override;

	virtual std::list<Dasa_ptr> getNextLevel( ContextProps_ptr, Dasa_ptr ) override;

	virtual bool hasKpFeatures() const override { return true; }

	virtual KpData getKPLords( const double &len ) override;

	virtual std::list<KPEvent> getKPEventList( const double &startlen,
		const double &endlen, const double &startjd ) override;

	std::list<Dasa_ptr> createSolarDasas( ContextProps_ptr, const double &solarjd, const int &year );

	virtual int getDasaIndex4Nakshatra( const Nakshatra27& ) = 0;
	virtual int getDasaDuration( const int& ) const = 0;
	virtual DasaLord getDasaLordByIndex( const int& ) = 0;

protected:

	// neu
	void updateChildren( Dasa_ptr, std::list<Dasa_ptr>& );

	int findDasaLordIndex( const DasaLord& );

	virtual int getParamayus() const = 0;

	void getKPLordRecursive( int &lord_index, double &elapsed, double &totallen );

	virtual std::pair<double, Nakshatra27> getPortion( ContextProps_ptr, Horoscope_ptr );
};

#endif

