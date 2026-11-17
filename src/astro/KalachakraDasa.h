/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/KalachakraDasa.h
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

#ifndef _KALACHAKRADASA_H_
#define _KALACHAKRADASA_H_

#include "Dasa.h"
#include "constants.h"

/*************************************************//**
*
* Implementation of Kalachakra data container
*
******************************************************/
class KalachakraDasa : public Dasa
{
	friend class KalachakraDasaExpert;

public:

	KalachakraDasa( Dasa_ptr parent ) : Dasa( parent )
	{
		pada = 0;
		subgroup = KalachakraSubgroup::None;
		gati = KalachakraGati::Default;
		deha = Sign::None;
		jeeva = Sign::None;
		amsa = Sign::None;
		paramayus = 0;

		padaBalance = 0.0;
		firstMdBalance = 0.0;
	}

	KalachakraDasa( DasaExpert_ptr expert, Dasa_ptr parent = nullptr, const DasaLevel level = DasaLevel::Root,
		const DasaLord lord = DLORD_NONE )
		: Dasa( expert, parent, level, lord )
	{
		pada = 0;
		subgroup = KalachakraSubgroup::None;
		gati = KalachakraGati::Default;
		deha = Sign::None;
		jeeva = Sign::None;
		amsa = Sign::None;
		paramayus = 0;

		padaBalance = 0.0;
		firstMdBalance = 0.0;
	}

	/**
	* The subgroup is one of the five prototype Nakshatras (Aswini .. Mrigasira).
	*/
	KalachakraSubgroup getSubgroup() const { return subgroup; }

	int getPada() const { return pada; }

	/**
	* Group is either Savya or Apsavya
	*/
	KalachakraGroup getGroup() const
	{
		switch( subgroup )
		{
			case KalachakraSubgroup::None:
				return KalachakraGroup::None;
			break;
			case KalachakraSubgroup::Aswini:
			case KalachakraSubgroup::Bharani:
				return KalachakraGroup::Savya;
			break;
			case KalachakraSubgroup::Rohini:
			case KalachakraSubgroup::Mrigasira:
				return KalachakraGroup::Apsavya;
			break;
			default:
				assert( false );
			break;
		}
		return KalachakraGroup::None;
	}
	
	Sign getAmsa() const { return amsa; }

	Sign getDeha() const { return deha; }

	Sign getJeeva() const { return jeeva; }

	double getPadaBalance() const { return padaBalance; }

	double getFirstMdBalance() const { return firstMdBalance; }

	bool isWrap() const { return gati != KalachakraGati::Default; }

	KalachakraGati getGati() const { return gati; }

protected:

	int pada;

	KalachakraSubgroup subgroup;

	KalachakraGati gati;

	Sign deha;

	Sign jeeva;

	Sign amsa;

	double padaBalance;

	double firstMdBalance;
};

#endif // _KALACHAKRADASA_H_

