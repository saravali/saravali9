/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/YdlExpression.cpp
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

#include "YdlExpression.h"

#include "enumserializer.h"
#include "logger.h"

#include <map>

extern ExpressionErrorStatus parserErrorStatus;
extern Rule *rule;

#define DEBUG_YDLEXPRESSION
//#define PEDANTIC_TYPE_CHECKING
#define DEBUG_EXPRESSION_LIFECYCLE

/*****************************************************
**
**   Expression   ---   Constructor
**
******************************************************/
Expression::Expression( const ExpressionTypeId &t )
	: type( t )
{
	errorStatus = parserErrorStatus;

#ifdef DEBUG_EXPRESSION_LIFECYCLE
	create_count++;
	infolog << "Expression Constructor: " << create_count << " instances created, " << delete_count << " instances deleted.";
#endif
}

/*****************************************************
**
**   Expression   ---   Destructor
**
******************************************************/
Expression::~Expression()
{
#ifdef DEBUG_EXPRESSION_LIFECYCLE
	delete_count++;
	infolog << "Expression Destructor: " << create_count << " instances created, " << delete_count << " instances deleted.";
#endif
}

/*****************************************************
**
**   Expression   ---   resetRuntimeErrorStatus
**
******************************************************/
void Expression::resetRuntimeErrorStatus()
{
	for( Expression *e : getNestedExpressions() )
	{
		if ( e->errorStatus == ExpressionErrorStatus::RuntimeError )
		{
			e->errorStatus = ExpressionErrorStatus::Ok;
		}
	}
}

/*****************************************************
**
**   Expression   ---   getNestedErrorStatus
**
******************************************************/
ExpressionErrorStatus Expression::getNestedErrorStatus() const
{
	for( Expression *e : getNestedExpressions() )
	{
		assert( e );
		if ( e->errorStatus != ExpressionErrorStatus::Ok ) return e->errorStatus;
	}
	return ExpressionErrorStatus::Ok;
}

/*****************************************************
**
**   ReturnExpression   ---   Constructor
**
******************************************************/
ReturnExpression::ReturnExpression( Expression *e ) : Expression( ExpressionTypeId::Return )
{
	assert( e );
	this->exp1 = e;
	outputtype = e->outputtype;
	debuglog << "ReturnExpression outputtype " << outputtype;

	if ( e->errorStatus != ExpressionErrorStatus::Ok )
	{
		errorStatus = e->errorStatus;
	}
}

/*****************************************************
**
**   ReturnExpression   ---   Destructor
**
******************************************************/
ReturnExpression::~ReturnExpression()
{
	if ( exp1 ) delete exp1;
}

/*****************************************************
**
**   ReturnExpression   ---   supportsHigherVargas
**
******************************************************/
bool ReturnExpression::supportsHigherVargas() const
{
	assert( exp1 );
	return exp1->supportsHigherVargas();
}

/*****************************************************
**
**   ReturnExpression   ---   getNestedExpressions
**
******************************************************/
std::list<Expression*> ReturnExpression::getNestedExpressions() const
{
	std::list<Expression*> l = { (Expression*)this };
	if ( exp1 ) l.push_back( exp1 );
	return l;
}

/*****************************************************
**
**   VariableAliasExpression   ---   Constructor
**
******************************************************/
VariableAliasExpression::VariableAliasExpression( const wxString &name )
	: Expression( ExpressionTypeId::VariableAlias ),
	name( name )
{
	debuglog << "VariableAliasExpression name " << name;

	assert( rule );
	auto v = rule->variables.find( name );
	if ( v == rule->variables.end())
	{
		errorlog << "Wrong variable \"" << name << "\" found, must be assigned first";
		errorStatus = ExpressionErrorStatus::WrongVariableError;
	}
	else
	{
		if ( v->second ) outputtype = v->second->outputtype;
	}
	debuglog << "Finished constructor VariableAliasExpression name " << name << " outputtype " << outputtype;
}

/*****************************************************
**
**   VariableAliasExpression   ---   supportsHigherVargas
**
******************************************************/
bool VariableAliasExpression::supportsHigherVargas() const
{
	assert( rule );
	auto v = rule->variables.find( name );
	if ( v != rule->variables.end())
	{
		if ( v->second ) return v->second->supportsHigherVargas();
	}
	return true;
}

/*****************************************************
**
**   VariableAliasExpression   ---   getNestedExpressions
**
******************************************************/
std::list<Expression*> VariableAliasExpression::getNestedExpressions() const
{
	assert( rule );
	auto v = rule->variables.find( name );
	if ( v != rule->variables.end())
	{
		if ( v->second ) return v->second->getNestedExpressions();
	}
	return {};
}

/*****************************************************
**
**   VoidExpression   ---   Constructor
**
******************************************************/
VoidExpression::VoidExpression( const VoidExpressionId &id )
	: Expression( ExpressionTypeId::Void ),
		vid( id )
{
	switch( vid )
	{
		case VoidExpressionId::NumberOfOccupiedSigns:
			outputtype = RuntimeTypeId::Integer;
		break;

		// Jaimini
		case VoidExpressionId::GetAtmakaraka:
		case VoidExpressionId::GetAtmatyakaraka:
		case VoidExpressionId::GetBhratrukaraka:
		case VoidExpressionId::GetMatrukaraka:
		case VoidExpressionId::GetPitrukaraka:
		case VoidExpressionId::GetPutrakaraka:
		case VoidExpressionId::GetGnatikaraka:
		case VoidExpressionId::GetDhanyaraka:
			outputtype = RuntimeTypeId::Planet;
		break;
		case VoidExpressionId::GetKarakamsaLagna:
			outputtype = RuntimeTypeId::Sign;
		break;

		default:
			assert( false );
		break;
	}
}

const static std::map<UnaryExpressionId, std::pair<RuntimeTypeId, RuntimeTypeId>> type_mapping_unary_expression = 
{
	{ UnaryExpressionId::GetRasi, { RuntimeTypeId::Planet, RuntimeTypeId::Sign }},
	{ UnaryExpressionId::GetBhava, { RuntimeTypeId::Planet, RuntimeTypeId::House }},
	{ UnaryExpressionId::GetNakshatra, { RuntimeTypeId::Planet, RuntimeTypeId::Nakshatra }},
	{ UnaryExpressionId::GetLordOfHouse, { RuntimeTypeId::House, RuntimeTypeId::Planet }},
	{ UnaryExpressionId::GetLordOfSign, { RuntimeTypeId::Sign, RuntimeTypeId::Planet }},
	{ UnaryExpressionId::CountPlanetsInSign, { RuntimeTypeId::Sign, RuntimeTypeId::Integer }},
	{ UnaryExpressionId::CountPlanetsInHouse, { RuntimeTypeId::House, RuntimeTypeId::Integer }},
	{ UnaryExpressionId::IsBenefic, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},
	{ UnaryExpressionId::IsMalefic, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},
	{ UnaryExpressionId::IsPlanetInKendra, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},
	{ UnaryExpressionId::IsPlanetInApoklima, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},
	{ UnaryExpressionId::IsPlanetInPanaphara, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},
	{ UnaryExpressionId::IsPlanetInDualRasi, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},
	{ UnaryExpressionId::IsPlanetInFixedRasi, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},
	{ UnaryExpressionId::IsPlanetInMovableRasi, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},

	{ UnaryExpressionId::IsPlanetInExaltationSign, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},
	{ UnaryExpressionId::IsPlanetInDebilationSign, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},
	{ UnaryExpressionId::IsPlanetInMulatrikonaSign, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},

	{ UnaryExpressionId::GetLongitude, { RuntimeTypeId::Planet, RuntimeTypeId::FloatingPoint }},
	{ UnaryExpressionId::IsRetrograde, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},
	{ UnaryExpressionId::GetNakshatra, { RuntimeTypeId::Planet, RuntimeTypeId::Nakshatra }},

	{ UnaryExpressionId::IsPlanetInMulatrikona, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},
	{ UnaryExpressionId::IsPlanetInVargottama, { RuntimeTypeId::Planet, RuntimeTypeId::Boolean }},

	{ UnaryExpressionId::Not, { RuntimeTypeId::Arbitrary, RuntimeTypeId::Boolean }},
	{ UnaryExpressionId::Red12, { RuntimeTypeId::Arbitrary, RuntimeTypeId::Integer }},
	{ UnaryExpressionId::TypeCastInt, { RuntimeTypeId::Arbitrary, RuntimeTypeId::Integer }},
	{ UnaryExpressionId::TypeCastFloatingPoint, { RuntimeTypeId::Arbitrary, RuntimeTypeId::FloatingPoint }},
	{ UnaryExpressionId::TypeCastPlanet, { RuntimeTypeId::Arbitrary, RuntimeTypeId::Planet }},
	{ UnaryExpressionId::TypeCastSign, { RuntimeTypeId::Arbitrary, RuntimeTypeId::Sign }},
	{ UnaryExpressionId::TypeCastNakshatra, { RuntimeTypeId::Arbitrary, RuntimeTypeId::Nakshatra }},
	{ UnaryExpressionId::TypeCastHouse, { RuntimeTypeId::Arbitrary, RuntimeTypeId::House }},
	{ UnaryExpressionId::TypeCastVarga, { RuntimeTypeId::Arbitrary, RuntimeTypeId::Varga }},
	{ UnaryExpressionId::TypeCastBoolean, { RuntimeTypeId::Arbitrary, RuntimeTypeId::Boolean }},
};

/*****************************************************
**
**   UnaryExpression   ---   Constructor
**
******************************************************/
UnaryExpression::UnaryExpression( Expression *e, const UnaryExpressionId &id )
	: Expression( ExpressionTypeId::Unary ),
		uid( id )
{
	assert( e );
	this->exp1 = e;
	if ( e->errorStatus != ExpressionErrorStatus::Ok )
	{
		errorStatus = e->errorStatus;
	}

	const auto &a = type_mapping_unary_expression.find( id );
	assert( a != type_mapping_unary_expression.end());

	outputtype = a->second.second;
	if ( a->second.first != RuntimeTypeId::Arbitrary && e->outputtype != a->second.first )
	{
		errorStatus = ExpressionErrorStatus::TypeError;

		mstringstream s;
		s << "Parameter of \"" << a->first << "\" is \"" << e->outputtype 
			<<  "\", expected type was \"" << a->second.first << "\"";
		errorlog << ( s.str());
	}
}

/*****************************************************
**
**   UnaryExpression   ---   Destructor
**
******************************************************/
UnaryExpression::~UnaryExpression()
{
	if ( exp1 ) delete exp1;
}

/*****************************************************
**
**   UnaryExpression   ---   supportsHigherVargas
**
******************************************************/
bool UnaryExpression::supportsHigherVargas() const
{
	assert( exp1 );
	if ( ! exp1->supportsHigherVargas() ) return false;

	return ( uid >= UnaryExpressionId::FirstHigherVargas && uid <= UnaryExpressionId::LastHigherVargas );
}

/*****************************************************
**
**   UnaryExpression   ---   getNestedExpressions
**
******************************************************/
std::list<Expression*> UnaryExpression::getNestedExpressions() const
{
	assert( exp1 );
	std::list<Expression*> l = { (Expression*)this, exp1 };
	const auto lnested = exp1->getNestedExpressions();
	l.insert( l.end(), lnested.begin(), lnested.end());
	return l;
}

const static std::map<BinaryExpressionId, std::tuple<RuntimeTypeId, RuntimeTypeId, RuntimeTypeId>>
	type_mapping_binary_expression = 
{
	{ BinaryExpressionId::And, { RuntimeTypeId::Arbitrary, RuntimeTypeId::Arbitrary, RuntimeTypeId::Boolean }},
};

/*****************************************************
**
**   BinaryExpression   ---   addSameInputTypeError
**
******************************************************/
void BinaryExpression::addSameInputTypeError()
{
	assert( exp1 );
	assert( exp2 );

	//errorStatus = ExpressionErrorStatus::ParserError;

	mstringstream ms;
	ms << "Error in binary expression " << bid << ". Parameters must have same type. Types are " << this->exp1->outputtype
		<< " and " << this->exp2->outputtype;
	//errorlog << ms.str();
	warnlog << ms.str();
}

/*****************************************************
**
**   BinaryExpression   ---   Constructor
**
******************************************************/
BinaryExpression::BinaryExpression( Expression *e1, Expression *e2, const BinaryExpressionId &id )
: Expression( ExpressionTypeId::Binary ),
	bid( id )
{
	assert( e1 );
	assert( e2 );

	this->exp1 = e1;
	this->exp2 = e2;

	if ( e1->errorStatus != ExpressionErrorStatus::Ok )
	{
		errorStatus = e1->errorStatus;
	}
	else if ( e2->errorStatus != ExpressionErrorStatus::Ok )
	{
		errorStatus = e2->errorStatus;
	}

	switch ( bid )
	{
		using enum BinaryExpressionId;

		case And:
			outputtype = RuntimeTypeId::Boolean;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != RuntimeTypeId::Boolean ) addCompilerTypeError( 1, e1->outputtype );
			if ( e2->outputtype != RuntimeTypeId::Boolean ) addCompilerTypeError( 2, e2->outputtype );
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;
		case Or:
			outputtype = RuntimeTypeId::Boolean;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != RuntimeTypeId::Boolean ) addCompilerTypeError( 1, e1->outputtype );
			if ( e2->outputtype != RuntimeTypeId::Boolean ) addCompilerTypeError( 2, e2->outputtype );
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;
		case IsEqual:
			outputtype = RuntimeTypeId::Boolean;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;
		case IsNotEqual:
			outputtype = RuntimeTypeId::Boolean;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;
		case Plus:
			outputtype = RuntimeTypeId::FloatingPoint;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;
		case Minus:
			outputtype = RuntimeTypeId::FloatingPoint;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;
		case Div:
			outputtype = RuntimeTypeId::FloatingPoint;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;
		case Mult:
			outputtype = RuntimeTypeId::FloatingPoint;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;
		case Mod:
			outputtype = RuntimeTypeId::Integer;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != RuntimeTypeId::Integer ) addCompilerTypeError( 1, e1->outputtype );
			if ( e2->outputtype != RuntimeTypeId::Integer ) addCompilerTypeError( 2, e2->outputtype );
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;

		case IsMutualKendra:
			outputtype = RuntimeTypeId::Boolean;
			if ( e1->outputtype != RuntimeTypeId::Planet ) addCompilerTypeError( 1, e1->outputtype );
			if ( e2->outputtype != RuntimeTypeId::Planet ) addCompilerTypeError( 2, e2->outputtype );
		break;
		case IsMutualTrikona:
			outputtype = RuntimeTypeId::Boolean;
			if ( e1->outputtype != RuntimeTypeId::Planet ) addCompilerTypeError( 1, e1->outputtype );
			if ( e2->outputtype != RuntimeTypeId::Planet ) addCompilerTypeError( 2, e2->outputtype );
		break;

		case IsGrahaDrishti:
			outputtype = RuntimeTypeId::Boolean;

			// Graha drishti may be planet-planet or planet-sign
			if ( e1->outputtype != RuntimeTypeId::Planet ) addCompilerTypeError( 1, e1->outputtype );
			if ( e2->outputtype != RuntimeTypeId::Planet && e2->outputtype != RuntimeTypeId::Sign )
			{
				addCompilerTypeError( 2, e2->outputtype );
			}
		break;
		case IsRasiDrishti:
			outputtype = RuntimeTypeId::Boolean;

			// Rasi drishti may be planet-planet, sign-planet, sign-sign  or planet-sign
			if ( e1->outputtype != RuntimeTypeId::Planet && e1->outputtype != RuntimeTypeId::Sign )
			{
				addCompilerTypeError( 1, e1->outputtype );
			}
			if ( e2->outputtype != RuntimeTypeId::Planet && e2->outputtype != RuntimeTypeId::Sign )
			{
				addCompilerTypeError( 2, e2->outputtype );
			}
		break;

		case GetRasiInVarga:
			outputtype = RuntimeTypeId::Sign;
			if ( e1->outputtype != RuntimeTypeId::Planet ) addCompilerTypeError( 1, e1->outputtype );
			if ( e2->outputtype != RuntimeTypeId::Varga ) addCompilerTypeError( 2, e2->outputtype );
		break;
		case GetHouseInVarga:
			outputtype = RuntimeTypeId::House;
			if ( e1->outputtype != RuntimeTypeId::Planet ) addCompilerTypeError( 1, e1->outputtype );
			if ( e2->outputtype != RuntimeTypeId::Varga ) addCompilerTypeError( 2, e2->outputtype );
		break;

		case Less:
			outputtype = RuntimeTypeId::Boolean;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;
		case LessEqual:
			outputtype = RuntimeTypeId::Boolean;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;
		case Greater:
			outputtype = RuntimeTypeId::Boolean;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;
		case GreaterEqual:
			outputtype = RuntimeTypeId::Boolean;
#ifdef PEDANTIC_TYPE_CHECKING
			if ( e1->outputtype != e2->outputtype ) addSameInputTypeError();
#endif
		break;

		default:
			errorlog << std::format( "Wrong cid {} for dual Expression", enum_to_string( bid ));
		break;
	}
}

/*****************************************************
**
**   BinaryExpression   ---   Destructor
**
******************************************************/
BinaryExpression::~BinaryExpression()
{
	if ( exp1 ) delete exp1;
	if ( exp2 ) delete exp2;
}

/*****************************************************
**
**   BinaryExpression   ---   supportsHigherVargas
**
******************************************************/
bool BinaryExpression::supportsHigherVargas() const
{
	assert( exp1 );
	assert( exp2 );
	if ( ! exp1->supportsHigherVargas() ) return false;
	if ( ! exp2->supportsHigherVargas() ) return false;

	return ( bid >= BinaryExpressionId::FirstHigherVargas && bid <= BinaryExpressionId::LastHigherVargas );
}

/*****************************************************
**
**   BinaryExpression   ---   getNestedExpressions
**
******************************************************/
std::list<Expression*> BinaryExpression::getNestedExpressions() const
{
	assert( exp1 );
	assert( exp2 );
	std::list<Expression*> l = { (Expression*)this, exp1, exp2 };

	const auto lnested1 = exp1->getNestedExpressions();
	l.insert( l.end(), lnested1.begin(), lnested1.end());
	const auto lnested2 = exp2->getNestedExpressions();
	l.insert( l.end(), lnested2.begin(), lnested2.end());

	return l;
}

/*****************************************************
**
**   BinaryExpression   ---   addCompilerTypeError
**
******************************************************/
void BinaryExpression::addCompilerTypeError( const int &param, const RuntimeTypeId &runtime_type )
{
	Expression *exp = param == 1 ? exp1 : exp2;
	assert( exp );

	this->errorStatus = ExpressionErrorStatus::TypeError;
	errorlog << std::format( "Parameter #{} of [{}] is {}, expected type was {}", param, enum_to_string( bid ),
		enum_to_string( runtime_type ), enum_to_string( exp->outputtype ));
}

/*****************************************************
**
**   VariadicExpression   ---   Constructor
**
******************************************************/
VariadicExpression::VariadicExpression( Expression *e1, Expression *e2, const std::list<Expression*> &l,
	const VariadicExpressionId &id )
	: Expression( ExpressionTypeId::Variadic ),
	exp1( e1 ), exp2( e2 ), expList( l ), vid( id )
{
	assert( exp1 );

#ifdef DEBUG_YDLEXPRESSION
	debuglog << "VariadicExpression param1 type " << exp1->type << " outputtype " << exp1->outputtype
		<< " param size " << expList.size();
	for( Expression *e : expList )
	{
		assert( e );
		debuglog << "VariadicExpression type " << e->type << " outputtype " << e->outputtype;
		if ( e->type == ExpressionTypeId::Constant )
		{
      ConstantExpression *ce = dynamic_cast<ConstantExpression*>( e );
      assert( ce );
      debuglog << "Constant expr has value " << ce->value;
		}
	}
#endif

	switch( vid )
	{
		using enum VariadicExpressionId;

		case IsPlanetInSign:
			checkTypeError( RuntimeTypeId::Planet, RuntimeTypeId::None, RuntimeTypeId::Sign );
			outputtype = RuntimeTypeId::Boolean;
		break;
		case IsPlanetInHouse:
			checkTypeError( RuntimeTypeId::Planet, RuntimeTypeId::None, RuntimeTypeId::House );
			outputtype = RuntimeTypeId::Boolean;
		break;
		case EqualsOneOf:
			outputtype = RuntimeTypeId::Boolean;
		break;
		case IsRelativeHouseOccupiedBy:
			outputtype = RuntimeTypeId::Boolean;
			checkTypeError( RuntimeTypeId::Planet, RuntimeTypeId::House, RuntimeTypeId::Planet );
		break;

		default:
			assert( false );
		break;
	}
}

/*****************************************************
**
**   VariadicExpression   ---   Destructor
**
******************************************************/
VariadicExpression::~VariadicExpression()
{
	if ( exp1 ) delete exp1;
	if ( exp2 ) delete exp2;
	for( Expression *e : expList )
	{
		if ( e ) delete e;
	}
	expList.clear();
}

/*****************************************************
**
**   VariadicExpression   ---   checkTypeError
**
******************************************************/
void VariadicExpression::checkTypeError( const RuntimeTypeId &te1, const RuntimeTypeId &te2, const RuntimeTypeId &tvar )
{
	assert( exp1 );

	if ( exp1->outputtype != te1 )
	{
		this->errorStatus = ExpressionErrorStatus::TypeError;

		mstringstream s;
		s << "Variadic expression [" << vid << "] has param1 " << te1
		<< ", expected type was " << exp1->outputtype;
		errorlog << ( s.str());
	}
	if ( exp2 && exp2->outputtype != te2 )
	{
		this->errorStatus = ExpressionErrorStatus::TypeError;

		mstringstream s;
		s << "Variadic expression [" << vid << "] has param2 " << te2
		<< ", expected type was " << exp2->outputtype;
		errorlog << ( s.str());
	}

	int i = 2;
	for( Expression *e : expList )
	{
		assert( e );
		if ( e->outputtype != tvar )
		{
			mstringstream s;
			s << "Variadic expression [" << vid << "] has variadic param " << i << " with type " << e->outputtype
			<< ", expected type was " << tvar;
			errorlog << ( s.str());
		}
		i++;
	}
}

/*****************************************************
**
**   VariadicExpression   ---   supportsHigherVargas
**
******************************************************/
bool VariadicExpression::supportsHigherVargas() const
{
	return true;
}

/*****************************************************
**
**   VariadicExpression   ---   getNestedExpressions
**
******************************************************/
std::list<Expression*> VariadicExpression::getNestedExpressions() const
{

	std::list<Expression*> l = { (Expression*)this };

	if ( exp1 )
	{
		l.push_back( exp1 );
		const auto lnested = exp1->getNestedExpressions();
		l.insert( l.end(), lnested.begin(), lnested.end());
	}
	if ( exp2 )
	{
		l.push_back( exp2 );
		const auto lnested = exp2->getNestedExpressions();
		l.insert( l.end(), lnested.begin(), lnested.end());
	}

	for( Expression *e : expList )
	{
		assert( e );
		l.push_back( e );
		const auto lnested = e->getNestedExpressions();
		l.insert( l.end(), lnested.begin(), lnested.end());
	}
	return l;
}

/*****************************************************
**
**   Rule   ---   Consructor
**
******************************************************/
Rule::Rule()
{
#ifdef DEBUG_EXPRESSION_LIFECYCLE
	create_count++;
	infolog << "Rule Constructor: " << create_count << " instances created, " << delete_count << " instances deleted.";
#endif
}

/*****************************************************
**
**   Rule   ---   Destructor
**
******************************************************/
Rule::~Rule()
{
	for( Expression *e : expressions )
	{
		delete e;
	}
	expressions.clear();

	for( auto &a : variables )
	{
		if ( a.second ) delete a.second;
	}
	variables.clear();

#ifdef DEBUG_EXPRESSION_LIFECYCLE
	delete_count++;
	infolog << "Rule Destructor: " << create_count << " instances created, " << delete_count << " instances deleted.";
#endif
}

/*****************************************************
**
**   Rule   ---   supportsHigherVargas
**
******************************************************/
bool Rule::supportsHigherVargas() const
{
	for( const Expression *e : expressions )
	{
		assert( e );
		if ( ! e->supportsHigherVargas() ) return false;
	}
	return true;
}

/*****************************************************
**
**   Rule   ---   getNestedErrorStatus
**
******************************************************/
ExpressionErrorStatus Rule::getNestedErrorStatus() const
{
	if ( errorStatus != ExpressionErrorStatus::Ok ) return errorStatus;

	for( const Expression *e : expressions )
	{
		assert( e );
		const ExpressionErrorStatus stat = e->getNestedErrorStatus();
		if ( stat != ExpressionErrorStatus::Ok ) return stat;
	}
	return ExpressionErrorStatus::Ok;
}

/*****************************************************
**
**   Rule   ---   getNestedExpressions
**
******************************************************/
std::list<Expression*> Rule::getNestedExpressions() const
{
	std::list<Expression*> l = { (Expression*)this };
	for( Expression *e : expressions )
	{
		assert( e );
		l.push_back( e );
		const auto lnested = e->getNestedExpressions();
		l.insert( l.end(), lnested.begin(), lnested.end());
	}
	return l;
}

/*****************************************************
**
**   Rule   ---   resetRuntimeErrorStatus
**
******************************************************/
void Rule::resetRuntimeErrorStatus()
{
	if ( errorStatus == ExpressionErrorStatus::RuntimeError )
	{
		errorStatus = ExpressionErrorStatus::Ok;
	}
	for( Expression *e : expressions )
	{
		assert( e );
		e->resetRuntimeErrorStatus();
	}
}

/*****************************************************
**
**   Rule   ---   addExpression
**
******************************************************/
void Rule::addExpression( Expression *e )
{
	assert( e );
	expressions.push_back( e );
	debuglog << "Rule::addExpression type " << e->type << " outputtype " << e->outputtype;

	if ( e->errorStatus != ExpressionErrorStatus::Ok )
	{
		errorStatus = e->errorStatus;
	}
}

/*****************************************************
**
**   Rule   ---   getOutputType
**
******************************************************/
RuntimeTypeId Rule::getOutputType() const
{
	RuntimeTypeId ot = RuntimeTypeId::None;

	for( const Expression *e : expressions )
	{
		assert( e );
		ot = e->outputtype;
		if ( e->type == ExpressionTypeId::Return )
		{
			break;
		}
	}
	return ot;
}




