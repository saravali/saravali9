/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/YdlInterpreter.h
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

#ifndef _YDLINTERPRETER_H_
#define _YDLINTERPRETER_H_

#include "constants.h"
#include "Planet.h"
#include "YdlExpression.h"

#include <wx/string.h>
#include <map>

class VedicExpert;

/*************************************************//**
*
*  \brief Interpreter for Yoga Definition Language (Ydl) rule trees 
*
******************************************************/
class YdlInterpreter
{
public:
	
	YdlInterpreter( ContextProps_ptr, const Varga = Varga::Rasi, const Sign artificialLagna = Sign::None );

	YdlInterpreter( const std::map<PlanetId,Sign>& );

	~YdlInterpreter();

	double evaluateRule( Rule* );

protected:

	void setupPlanetsInSign();

	double evaluateExpression( Expression* );

	Sign getRasi( const PlanetId&, const Varga& );

	Nakshatra27 getNakshatra( const PlanetId& ) const;

	House getBhava( const PlanetId& );

	bool isBenefic( const PlanetId& ) const;

	int getNumberOfOccupiedSigns();

	PlanetId getKaraka( const CharaKaraka& ) const;

	ExpressionErrorStatus checkInputValue( Expression*, const double &value ) const;

	double evaluateReturnExpression( ReturnExpression* );

	double evaluateConstantExpression( ConstantExpression* );

	double evaluateVoidExpression( VoidExpression* );

	double evaluateUnaryExpression( UnaryExpression* );

	double evaluateBinaryExpression( BinaryExpression* );

	double evaluateVariadicExpression( VariadicExpression* );

	ContextProps_ptr props;

	Varga varga = Varga::Rasi;

	Sign artificialLagna = Sign::None;

	std::map<PlanetId,Sign> fake_data;

	int nb_planets_in_sign[12];

	Rule *rule = nullptr;

	VedicExpert *expert = nullptr;

};


#endif // _YDLINTERPRETER_H_


