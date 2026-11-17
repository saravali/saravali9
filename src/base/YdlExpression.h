/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/YdlExpression.h
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

#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include "bootstrap.h"

#include <map>
#include <list>
#include <vector>
#include <wx/string.h>

// Expression types are later for type casting
enum class ExpressionTypeId
{
	// json:enum
	None = -1,
	Return, /**< Stop interpreting Expression and return the value of the last Expression  */
	VariableAlias, /**< Variable assignment  */
	Constant, /**< Constant like House or Sign  */
	Void, /**< Expression has exactly no parameter  */
	Unary, /**< Expression has exactly one parameter  */
	Binary, /**< Expression has exactly two parameters  */
	Variadic, /**< Expression has one or more parameters (flexible)  */

	First = Return,
	Last = Variadic
};
 
/**< TODO  */

// Runtime types: there is strict type checking
enum class RuntimeTypeId
{
	// json:enum
	None = -1,
	Integer, /**< Integer number, can be negative  */
	FloatingPoint, /**< Allowed tokens are numbers, "+/-" and decimal point "."  */
	House, /**< See House  */
	Planet, /**< See PlanetId  */
	Sign, /**< See Sign  */
	Boolean, /**< True, false, yes or no  */
	Void,  /**< No value  */
	Arbitrary, /**< Arbitrary numeric value (void not allowed). This value is used in type casts or mathematical operations (internally is double=floating point). */

	// no higher Vargas
	Nakshatra, /**< See Nakshatra27  */
	Varga, /**< See Varga  */

	First = Integer,
	Last = Varga,
	FirstHigherVargas = First,
	LastHigherVargas = Arbitrary
};

/**
* Enumeration for functions that need no input parameter.
*/
enum class VoidExpressionId
{
	// json:enum
	None = -1,
	NumberOfOccupiedSigns, /**< Return integer of signs occupied by by one or more planet (Sun, .. Saturn) */

	// Jaimini
	GetAtmakaraka, /**< Return PlanetId of Atmakaraka in Jaimini Astrology */
	GetAtmatyakaraka, /**< Return PlanetId of Amatyakaraka in Jaimini Astrology */
	GetBhratrukaraka, /**< Return PlanetId of Bhratrukaraka (coborn) */
	GetMatrukaraka, /**< Return PlanetId of Matrukaraka (mother) */
	GetPitrukaraka, /**< Return PlanetId of Pitrukaraka (father) */
	GetPutrakaraka, /**< Return PlanetId of Putrakaraka (offspring) */
	GetGnatikaraka, /**< Return PlanetId of Gnatikaraka (relatives) */
	GetDhanyaraka, /**< Return PlanetId of Dhayakaraka (spouse and husband) */
	GetKarakamsaLagna, /**< Return Sign of Karakamsa lagna */

	First = NumberOfOccupiedSigns,
	Last = GetKarakamsaLagna
};

/**
* Unary expression have exactly one parameter
*/
enum class UnaryExpressionId
{
	// json:enum
	None = -1,
	GetRasi, /**< Get sign of a planet in Rasi chart. Param: PlanetId. Return: Sign. */
	GetBhava, /**< Get house of a planet. Param: PlanetId. Return: House. */
	GetLordOfHouse, /**< Get planet ruling the corresponding House, depends only on Ascendant. Param: House. Return: PlanetId. */
	GetLordOfSign, /**< Get planet ruling a sign, does not depend on ascendant. Param: Sign. Return: PlanetId. */
	CountPlanetsInSign, /**< Count the number of planets (excluding nodes and angles) in the Sign. Param: Sign. Return: Integer. */
	CountPlanetsInHouse, /**< Count the number of planets (excluding nodes and angles) in the House. Param: House. Return: Integer. */
	IsBenefic, /**< Calculate if a planet is benefic (Moon and Mercury according to configuraton). Param: PlanetId. Return: Boolean.*/
	IsMalefic, /**< Calculate if a planet is malefic (Moon and Mercury according to configuraton). Param: PlanetId. Return: Boolean.*/

	IsPlanetInKendra, /**< Planet is in 1st, 4th, 7th or 10th house from the ascendant. Param: PlanetId. Return: Boolean.*/
	IsPlanetInApoklima, /**< Planet is in 2nd, 5th, 8th or 11th house from the ascendant. Param: PlanetId. Return: Boolean.*/
	IsPlanetInPanaphara, /**< Planet is in 3rd, 6th, 9th or 12th house from the ascendant. Param: PlanetId. Return: Boolean.*/

	IsPlanetInDualRasi, /**< Planet is in Aries, Cancer, Libra or Capricorn. Param: PlanetId. Return: Boolean.*/
	IsPlanetInFixedRasi, /**< Planet is in Taurus, Leo, Scorpio or Aquarius. Param: PlanetId. Return: Boolean.*/
	IsPlanetInMovableRasi, /**< Planet is in Gemini, Virgo, Sagittarius or Pisces. Param: PlanetId. Return: Boolean.*/

	// dignity
	IsPlanetInExaltationSign, /**< Planet is in exaltation Sign (all Vargas). Param: PlanetId, Return: Boolean  */
	IsPlanetInDebilationSign, /**< Planet is in debilation Sign (all Vargas). Param: PlanetId, Return: Boolean  */
	IsPlanetInMulatrikonaSign,  /**< Planet is in Mulatrikona Sign (all Vargas). Param: PlanetId, Return: Boolean  */

	Not, /**< Logical NOT operator, Param: Numeric, Return: Boolean  */
	Red12, /**< Reduces any number mod 12. Param: Numeric, Return: Numeric  */
	TypeCastInt, /**< Cast arbitrary numeric value to integer. */
	TypeCastFloatingPoint, /**< Cast arbitrary numeric value to FloatingPoint. */
	TypeCastPlanet, /**< Cast arbitrary numeric value to PlanetId. */
	TypeCastSign, /**< Cast arbitrary numeric value to Sign. */
	TypeCastNakshatra, /**< Cast arbitrary numeric value to Nakshatra27. */
	TypeCastHouse, /**< Cast arbitrary numeric value to House. */
	TypeCastVarga, /**< Cast arbitrary numeric value to Varga. */
	TypeCastBoolean, /**< Cast arbitrary numeric value to Boolean. */

	GetLongitude, /**< Get longitude of a planet or other object (Rasi chart only). Param: PlanetId, Return: FloatingPoint  */
	IsRetrograde, /**< Planet has retrograde motion. Param: PlanetId, Return: Boolean  */
	GetNakshatra, /**< Nakshatra of a planet (Rasi only). Param: PlanetId, Return: Nakshatra27  */

	IsPlanetInVargottama, /**< Planet is in own Rasi and Navamsa. Param: PlanetId, Return: Boolean  */
	IsPlanetInMulatrikona, /**< Planet longitude is Mulatrikona (Rasi only). Param: PlanetId, Return: Boolean. */

	First = GetRasi,
	Last = IsPlanetInMulatrikona,
	FirstHigherVargas = First,
	LastHigherVargas = TypeCastBoolean
};

/**
* Binary expressions have exactly two parameters.
*/
enum class BinaryExpressionId
{
	// json:enum
	None = -1,

	// logical operators -> boolean
	And, /**< Logical AND operator. Params: 2x numerical, Return: Boolean  */
	Or,  /**< Logical OR operator. Params: 2x numerical, Return: Boolean  */
	IsEqual, /**< Comparison operator. Params: 2x numerical, Return: Boolean  */
	IsNotEqual, /**< Comparison operator. Params: 2x numerical, Return: Boolean  */

	// mathematical operators -> numerical
	Plus, /**< Plus operator. Params: 2x numerical, Return: numerical  */
	Minus, /**< Minus operator. Params: 2x numerical, Return: numerical  */
	Div, /**< Division operator. Params: 2x numerical, Return: numerical  */ 
	Mult, /**< Multiplication operator. Params: 2x numerical, Return: numerical  */
	Mod,  /**< TODO Modulo operation for numeric value and integer. Params: numerical+integer, Return: numerical  */

	// comparison -> boolean
	Less, /**< Comparison operator "<". Params: 2x numerical, Return: Boolean  */
	LessEqual, /**< Comparison operatori " <=". Params: 2x numerical, Return: Boolean  */
	Greater, /**< Comparison operator ">". Params: 2x numerical, Return: Boolean  */
	GreaterEqual, /**< Comparison operator ">=". Params: 2x numerical, Return: Boolean  */
	Assignment, /**< Variable assignment (a=b). Params: 2x numerical (right value must be known), Return: void  */

	// relations of two planets -> boolean
	IsMutualKendra, /**< Two planets in mutual kendra, i.e. in 1st, 7th, 4th or 10th house. Params: 2x PlanetId, return: Boolean  */
	IsMutualTrikona, /**< Two planets in mutual trikona, i.e. in 1st, 5th or 9th house. Params: 2x PlanetId, return: Boolean  */
	IsGrahaDrishti, /**< Planet 1 has Graha Drishti to planet 2, i.e. in 7th house or special rules for Mars, Jupiter, Saturn. Params: 2x PlanetId, return: Boolean  */
	IsRasiDrishti, /**< Sign 1 has Rasi Drishti to Sign 2 (Jaimini astrology). Params: 2x Sign, return: Boolean  */

	// higher Varga support
	GetRasiInVarga, /**< Get Rasi of a planet in Varga chart. Params: PlanetId and Varga, Return Sign.  */
	GetHouseInVarga, /**< Get Bhava of a planet in Varga chart. Params: PlanetId and Varga, Return Sign.  */

	First = And,
	Last = GetHouseInVarga,
	FirstHigherVargas = First,
	LastHigherVargas = GreaterEqual
};

/**
* Variadic expressions have one or more parameters.
*/
enum class VariadicExpressionId
{
	// json:enum
	None = -1,
	IsPlanetInHouse, /**< Planet is in one of the given houses. Params: PlanetId + list of one or more House, Return: Boolean.*/
	IsPlanetInSign, /**< Planet is in one of the given signs. Params: PlanetId + list of one or more Sign, Return: Boolean.*/
	EqualsOneOf, /**< Numeric value equals one of the given values. Params: Numeric + list of one or numeric values, Return: Boolean.*/
	IsRelativeHouseOccupiedBy, /**< Computes whether a house from a planet is occupied by one of the planets in a list. Params: PlanetId + House + list of one or more PlanetId, Return: Boolean.*/

	First = IsPlanetInHouse,
	Last = IsRelativeHouseOccupiedBy,
	FirstHigherVargas = First,
	LastHigherVargas = Last
};

enum class ExpressionErrorStatus
{
	// json:enum
	None = -1,
	Ok,
	LexicalError,
	ParserError,
	TypeError,
	WrongVariableError,
	InvalidValueError,
	RuntimeError
};

/*************************************************//**
*
*  General Expression class. Only derived classes are used.
*
******************************************************/
class Expression
{
public:
	Expression( const ExpressionTypeId& );

	virtual ~Expression();

	/**
	*  Some funtions do not work in higher Varga chart e.g. because they depend on longitude.
	*
	*  This method asures that no such function or constant is included in the Exression.
	*  
	*/
	virtual bool supportsHigherVargas() const = 0;

	/**
	* Returns the first error status found in the nested expressions or Ok if no error found.
	*
	* N.B. this can not be done in the constructor because of runtime errors.
	*/
	ExpressionErrorStatus getNestedErrorStatus() const;

	virtual std::list<Expression*> getNestedExpressions() const { return { (Expression*)this }; }

	void resetRuntimeErrorStatus();

	const ExpressionTypeId type;

	RuntimeTypeId outputtype = RuntimeTypeId::None;

	ExpressionErrorStatus errorStatus = ExpressionErrorStatus::Ok;

	static inline int create_count = 0;

	static inline int delete_count = 0;
};

/*************************************************//**
*
*  The expression defined by the "return" statement
*
******************************************************/
class ReturnExpression : public Expression
{
public:
	ReturnExpression( Expression* );

	~ReturnExpression();

	virtual bool supportsHigherVargas() const override;

	virtual std::list<Expression*> getNestedExpressions() const override;

	Expression *exp1 = nullptr;
};

/*************************************************//**
*
*  Variable alias
*
******************************************************/
class VariableAliasExpression : public Expression
{
public:
	VariableAliasExpression( const wxString& );

	virtual bool supportsHigherVargas() const override;

	virtual std::list<Expression*> getNestedExpressions() const override;

	const wxString name;
};

/*************************************************//**
*
*  A constant value
*
******************************************************/
class ConstantExpression : public Expression
{
public:

	ConstantExpression( const RuntimeTypeId &id, const double &value )
		: Expression( ExpressionTypeId::Constant ),
			value( value )
	{
		outputtype = id;
	}

	virtual bool supportsHigherVargas() const override
	{
		return ( outputtype >= RuntimeTypeId::FirstHigherVargas && outputtype <= RuntimeTypeId::LastHigherVargas );
	}

	const double value;
};

/*************************************************//**
*
* Expression without parameter
*
******************************************************/
class VoidExpression : public Expression
{
public:

	VoidExpression( const VoidExpressionId& );

	virtual bool supportsHigherVargas() const override { return true; }

	const VoidExpressionId vid;
};

/*************************************************//**
*
* Expression with one parameter
*
******************************************************/
class UnaryExpression : public Expression
{
public:
	UnaryExpression( Expression*, const UnaryExpressionId& );

	~UnaryExpression();

	virtual bool supportsHigherVargas() const override;

	virtual std::list<Expression*> getNestedExpressions() const override;

	const UnaryExpressionId uid;

	Expression *exp1 = nullptr;
};

/*************************************************//**
*
*  expression with 2 params
*
******************************************************/
class BinaryExpression : public Expression
{
public:

	BinaryExpression( Expression*, Expression*, const BinaryExpressionId& );

	~BinaryExpression();

	virtual bool supportsHigherVargas() const override;

	virtual std::list<Expression*> getNestedExpressions() const override;

	const BinaryExpressionId bid;

	Expression *exp1 = nullptr;

	Expression *exp2 = nullptr;

private:

	void addSameInputTypeError();

	void addCompilerTypeError( const int &param, const RuntimeTypeId &runtime_type );
};

/*************************************************//**
*
*  expression with one or two fixed and a variable number (>1) of variable params
*
******************************************************/
class VariadicExpression : public Expression
{
public:

	VariadicExpression( Expression*, Expression*, const std::list<Expression*>&, const VariadicExpressionId& );

	~VariadicExpression();

	virtual bool supportsHigherVargas() const override;

	virtual std::list<Expression*> getNestedExpressions() const override;

	Expression *exp1 = nullptr;

	Expression *exp2 = nullptr;

	std::list<Expression*> expList;

	const VariadicExpressionId vid;

private:

	void checkTypeError( const RuntimeTypeId &te1, const RuntimeTypeId &te2, const RuntimeTypeId &tvar );

};

/*************************************************//**
*
*  The Rule class contains a vector of expressions
*
******************************************************/
class Rule
{
public:

	Rule();

	virtual ~Rule();

	bool supportsHigherVargas() const;

	ExpressionErrorStatus getNestedErrorStatus() const;

	std::list<Expression*> getNestedExpressions() const;

	void resetRuntimeErrorStatus();

	ExpressionErrorStatus errorStatus = ExpressionErrorStatus::Ok;

	void addExpression( Expression* );

	RuntimeTypeId getOutputType() const;

	std::map<wxString, Expression* > variables;

	std::vector<Expression*> expressions;

	static inline int create_count = 0;

	static inline int delete_count = 0;
};

#endif


