/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/YdlInterpreter.cpp
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

#include "YdlInterpreter.h"

#include "astrobase.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "Expert.h"
#include "Horoscope.h"
#include "Jaimini.h"
#include "logger.h"
#include "mathbase.h"
#include "Varga.h"
#include "VedicAspect.h"
#include "YdlExpression.h"

#define DEBUG_YDLINTERPRETER

/*****************************************************
**
**   YdlInterpreter   ---   Constructor
**
******************************************************/
YdlInterpreter::YdlInterpreter( ContextProps_ptr props, const Varga varga, const Sign lagna )
	: props( props ),
	varga( varga ),
	artificialLagna( lagna )
{
	expert = new VedicExpert( props, varga );
	Horoscope_ptr horoscope = props->getRadixHoroscope();
	assert( horoscope );

	setupPlanetsInSign();
}

/*****************************************************
**
**   YdlInterpreter   ---   Constructor
**
******************************************************/
YdlInterpreter::YdlInterpreter( const std::map<PlanetId,Sign> &data )
	: fake_data( data )
{
	setupPlanetsInSign();
}

/*****************************************************
**
**   YdlInterpreter   ---   Destructor
**
******************************************************/
YdlInterpreter::~YdlInterpreter()
{
	if ( expert ) delete expert;
}

/*****************************************************
**
**   YdlInterpreter   ---   setupPlanetsInSign
**
******************************************************/
void YdlInterpreter::setupPlanetsInSign()
{
	for ( int i = 0; i < 12; i++ ) nb_planets_in_sign[i] = 0;
	for( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		const Sign rasi = this->getRasi( p, varga );
		if ( is_valid_enum( rasi ))
		{
			nb_planets_in_sign[(int)rasi]++;
		}
	}
#ifdef DEBUG_YDLINTERPRETER
	debuglog << "Number of planets in sign";
	for( Sign sign : Enum<Sign>())
	{
		debuglog << sign << ": " << nb_planets_in_sign[ (int)sign ];
	}
#endif
}

/*****************************************************
**
**   YdlInterpreter   ---   evaluateExpression
**
******************************************************/
double YdlInterpreter::evaluateExpression( Expression *expression )
{
	assert( expression );
#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::evaluateExpression type " << expression->type;
#endif

	if ( expression->getNestedErrorStatus() != ExpressionErrorStatus::Ok ) return -1;

	switch( expression->type )
	{
		using enum  ExpressionTypeId;

		case Return:
		{
			ReturnExpression *re = dynamic_cast<ReturnExpression*>( expression );
			assert( re );
			return evaluateReturnExpression( re );
		}
		break;
		case VariableAlias:
		{
			VariableAliasExpression *vae = dynamic_cast<VariableAliasExpression*>( expression );
			assert( vae );
			//debuglog << "Evaluate VariableAliasExpression name " << vae->name;
			auto v = rule->variables.find( vae->name );
			if ( v != rule->variables.end())
			{
				if ( v->second )
				{
#ifdef DEBUG_YDLINTERPRETER
					debuglog << "VariableAlias " << vae->name << " typ " << v->second->outputtype
						<< " value " << evaluateExpression( v->second );
#endif
					return evaluateExpression( v->second );
				}
				else return -1;
			}
			else
			{
				errorlog << "Evaluate VariableAliasExpression name " << vae->name;
				return -1;
			}
		}
		break;
		case Constant:
		{
			ConstantExpression *ce = dynamic_cast<ConstantExpression*>( expression );
			assert( ce );
			return evaluateConstantExpression( ce );
		}
		break;
		case Void:
		{
			VoidExpression *ve = dynamic_cast<VoidExpression*>( expression );
			assert( ve );
			return evaluateVoidExpression( ve );
		}
		break;
		case Unary:
		{
			UnaryExpression *ue = dynamic_cast<UnaryExpression*>( expression );
			assert( ue );
			return evaluateUnaryExpression( ue );
		}
		break;
		case Binary:
		{
			BinaryExpression *be = dynamic_cast<BinaryExpression*>( expression );
			assert( be );
			return evaluateBinaryExpression( be );
		}
		break;
		case Variadic:
		{
			VariadicExpression *ve = dynamic_cast<VariadicExpression*>( expression );
			assert( ve );
			return evaluateVariadicExpression( ve );
		}
		break;
		default:
			assert( false );
		break;
	}
	return -1;
}

/*****************************************************
**
**   YdlInterpreter   ---   checkInputValue
**
******************************************************/
ExpressionErrorStatus YdlInterpreter::checkInputValue( Expression *e, const double &val ) const
{
	assert( e );
	switch( e->outputtype )
	{
		case RuntimeTypeId::House:
			if( ! is_valid_enum( (House)val ))
			{
				errorlog << "Runtime error: invalid value " << val << " for house.";
				return ExpressionErrorStatus::RuntimeError;
			}
		break;
		case RuntimeTypeId::Planet:
			if ( ! is_valid_enum( (PlanetId)val ))
			{
				errorlog << "Runtime error: invalid value " << val << " for planet.";
				return ExpressionErrorStatus::RuntimeError;
			}
		break;
		case RuntimeTypeId::Sign:
		{
			if ( ! is_valid_enum( (Sign)val ))
			{
				errorlog << "Runtime error: invalid value " << val << " for sign.";
				return ExpressionErrorStatus::RuntimeError;
			}
		}
		break;
		case RuntimeTypeId::Nakshatra:
			if ( ! is_valid_enum( (Nakshatra27)val ))
			{
				errorlog << "Runtime error: invalid value " << val << " for Nakshatra.";
				return ExpressionErrorStatus::RuntimeError;
			}
		break;
		case RuntimeTypeId::Varga:
			if ( ! is_valid_enum( (Varga)val ))
			{
				errorlog << "Runtime error: invalid value " << val << " for Varga.";
				return ExpressionErrorStatus::RuntimeError;
			}
		break;

		case RuntimeTypeId::Integer:
		case RuntimeTypeId::Boolean:
		case RuntimeTypeId::FloatingPoint:
		case RuntimeTypeId::Void:
			return ExpressionErrorStatus::Ok;
		break;

		case RuntimeTypeId::None:
		default:
			return ExpressionErrorStatus::RuntimeError;
		break;
	}
	return ExpressionErrorStatus::Ok;
}


/*****************************************************
**
**   YdlInterpreter   ---   evaluateReturnExpression
**
******************************************************/
double YdlInterpreter::evaluateReturnExpression( ReturnExpression *re )
{
	assert( re );
	assert ( re->exp1 );

	const double result = evaluateExpression( re->exp1 );

	// find runtime errors
	if ( re->getNestedErrorStatus() != ExpressionErrorStatus::Ok ) return -1;
	return result;
}

/*****************************************************
**
**   YdlInterpreter   ---   evaluateConstantExpression
**
******************************************************/
double YdlInterpreter::evaluateConstantExpression( ConstantExpression *exp )
{
#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::evaluateConstantExpression type " << exp->outputtype << " value " << exp->value;
#endif
	assert( exp );
	return exp->value;
}

/*****************************************************
**
**   YdlInterpreter   ---   evaluateVoidExpression
**
******************************************************/
double YdlInterpreter::evaluateVoidExpression( VoidExpression *ve )
{
	assert( ve );
	switch( ve->vid )
	{
		case VoidExpressionId::NumberOfOccupiedSigns:
			return this->getNumberOfOccupiedSigns();
		break;

		case VoidExpressionId::GetAtmakaraka:
			return (double)this->getKaraka( CharaKaraka::Atma );
		break;
    case VoidExpressionId::GetAtmatyakaraka:
			return (double)this->getKaraka( CharaKaraka::Amatya );
		break;
    case VoidExpressionId::GetBhratrukaraka:
			return (double)this->getKaraka( CharaKaraka::Bhratru );
		break;
    case VoidExpressionId::GetMatrukaraka:
			return (double)this->getKaraka( CharaKaraka::Matru );
		break;
    case VoidExpressionId::GetPitrukaraka:
			return (double)this->getKaraka( CharaKaraka::Pitru );
		break;
    case VoidExpressionId::GetPutrakaraka:
			return (double)this->getKaraka( CharaKaraka::Putra );
		break;
    case VoidExpressionId::GetGnatikaraka:
			return (double)this->getKaraka( CharaKaraka::Gnati );
		break;
    case VoidExpressionId::GetDhanyaraka:
			return (double)this->getKaraka( CharaKaraka::Dara );
    break;
    case VoidExpressionId::GetKarakamsaLagna:
			return (double)getLord( (Sign)getKaraka( CharaKaraka::Atma ));
    break;

		default:
			assert( false );
		break;
	}
	return -1;
}

/*****************************************************
**
**   YdlInterpreter   ---   getKaraka
**
******************************************************/
PlanetId YdlInterpreter::getKaraka( const CharaKaraka &k ) const
{
	JaiminiExpert jexpert( props, Varga::Rasi );
	jexpert.calcCharaKarakas();
	return jexpert.getCharaKaraka( k );
}

/*****************************************************
**
**   YdlInterpreter   ---   evaluateUnaryExpression
**
******************************************************/
double YdlInterpreter::evaluateUnaryExpression( UnaryExpression *ue )
{
	assert( ue );
	assert( ue->exp1 );
#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::evaluateUnaryExpression " << ue->uid;
#endif

	if ( ue->exp1->getNestedErrorStatus() != ExpressionErrorStatus::Ok ) return -1;

	const double val = this->evaluateExpression( ue->exp1 );

#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::evaluateUnaryExpression " << ue->uid << " exp1 type " << ue->exp1->type << " value " << val;
#endif

	const ExpressionErrorStatus errExp1 = checkInputValue( ue->exp1, val );
	if ( errExp1 != ExpressionErrorStatus::Ok )
	{
		ue->errorStatus = errExp1;
		return -1;
	}

	switch ( ue->uid )
	{
		using enum UnaryExpressionId;

		case Not:
			return ! (bool)val;
		break;
		case Red12:
			return red12( (int)val );
		break;

		case GetLordOfHouse:
		{
#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::evaluateUnaryExpression " << ue->uid << " artificialLagna " << artificialLagna << "val " << val;
#endif
			int h = -1;
			if ( is_valid_enum( artificialLagna ))
			{
				h = red12( (int)val + (int)artificialLagna );
			}
			else
			{
				h = red12( (int)getRasi( PlanetId::Ascendant, varga ) + (int)val );
			}
			const PlanetId p = getLord( (Sign)h, Viewpoint::Vedic );
#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::evaluateUnaryExpression h " << h << " p " << p;
#endif
			return (double)p;
		}
		break;

		case GetRasi:
			return (double)this->getRasi( (PlanetId)val, varga );
		break;

		case GetLordOfSign:
			return (double)getLord( (Sign)val );
		break;

		case GetBhava:
			return (double)this->getBhava( (PlanetId)val );
		break;

		case GetNakshatra:
			return (double)this->getNakshatra( (PlanetId)val );
		break;

		case CountPlanetsInSign:
		{
			Sign sign = (Sign) val;
			assert_valid_enum<Sign>( sign );
			return nb_planets_in_sign[ (int)sign ];
		}
		break;

		case CountPlanetsInHouse:
		{
			const House house = (House)val;
			assert_valid_enum<House>( house );
			int count = 0;
			for ( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
			{
				if ( getBhava( p ) == house ) count++;
			}
			return count;
		}
		break;

		case IsBenefic:
			return (bool)this->isBenefic( (PlanetId)val );
		break;
		case IsMalefic:
			return (bool)( ! this->isBenefic( (PlanetId)val ));
		break;

		case IsPlanetInKendra:
		{
			const House house = this->getBhava( (PlanetId)val );
			if ( house == House::None ) return false;
			return( (int)house % 3 ) == 0;
		}
		break;
		case IsPlanetInApoklima:
		{
			const House house = this->getBhava( (PlanetId)val );
			if ( house == House::None ) return false;
			return( (int)house % 3 ) == 1;
		}
		break;
		case IsPlanetInPanaphara:
		{
			const House house = this->getBhava( (PlanetId)val );
			if ( house == House::None ) return false;
			return( (int)house % 3 ) == 2;
		}
		break;

		case IsPlanetInMovableRasi:
			return( ((int)this->getRasi( (PlanetId)val, varga ) % 3 ) == 0 );
		break;
		case IsPlanetInFixedRasi:
			return( ((int)this->getRasi( (PlanetId)val, varga ) % 3 ) == 1 );
		break;
		case IsPlanetInDualRasi:
			return( ((int)this->getRasi( (PlanetId)val, varga ) % 3 ) == 2 );
		break;

		case IsPlanetInExaltationSign:
			return hasExaltationRasi( (PlanetId)val, this->getRasi( (PlanetId)val, varga ));
		break;
  	case IsPlanetInDebilationSign:
			return hasDebilationRasi( (PlanetId)val, this->getRasi( (PlanetId)val, varga ));
		break;
		case IsPlanetInMulatrikonaSign:
			return this->getRasi( (PlanetId)val, varga ) == getMulatrikonaRasi( (PlanetId)val );
		break;

		case GetLongitude:
			if ( expert )
			{
				return expert->getLongitude( (PlanetId)val );
			}
			else
			{
				errorlog << "Method getLongitude not supported in fake mode";
				return -1;
			}
		break;
		case IsRetrograde:
			if ( expert )
			{
				return expert->isRetrograde( (PlanetId)val );
			}
			else
			{
				errorlog << "Method isRetrograde not supported in fake mode";
				return -1;
			}
		break;

		case IsPlanetInVargottama:
		{
		 const Sign rasipos = getRasi( (PlanetId)val, Varga::Rasi );
		 const Sign navamsapos = getRasi( (PlanetId)val, Varga::Navamsa );
		 return rasipos == navamsapos;
		}
		break;
		case IsPlanetInMulatrikona:
			if ( expert )
			{
				return isInMulatrikona( (PlanetId)val, expert->getLongitude( (PlanetId)val ));
			}
			else
			{
				errorlog << "Method isInMulatrikona not supported in fake mode";
				return -1;
			}
		break;

		case UnaryExpressionId::TypeCastFloatingPoint:
			return val;
		break;
		case UnaryExpressionId::TypeCastInt:
		case UnaryExpressionId::TypeCastPlanet:
		case UnaryExpressionId::TypeCastSign:
		case UnaryExpressionId::TypeCastNakshatra:
		case UnaryExpressionId::TypeCastHouse:
		case UnaryExpressionId::TypeCastVarga:
			return (int)val;
		break;
		case UnaryExpressionId::TypeCastBoolean:
			return (bool)val;
		break;

		default:
			assert( false );
		break;
	}
	assert( false ); // impossible
	return -1;
}

/*****************************************************
**
**   YdlInterpreter   ---   evaluateBinaryExpression
**
******************************************************/
double YdlInterpreter::evaluateBinaryExpression( BinaryExpression *be )
{
	assert( be );
	assert( be->exp1 );
	assert( be->exp2 );

	const double d1 = this->evaluateExpression( be->exp1 );
	const ExpressionErrorStatus errExp1 = checkInputValue( be->exp1, d1 );
	if ( errExp1 != ExpressionErrorStatus::Ok )
	{
		be->errorStatus = errExp1;
		return -1;
	}

	const double d2 = this->evaluateExpression( be->exp2 );
	const ExpressionErrorStatus errExp2 = checkInputValue( be->exp2, d2 );
	if ( errExp2 != ExpressionErrorStatus::Ok )
	{
		be->errorStatus = errExp2;
		return -1;
	}
#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::evaluateBinaryExpression bid " << be->bid << " result1 " << d1 << " result 2 " << d2;
#endif

	switch( be->bid )
	{
		using enum BinaryExpressionId;

		case And:
			return (double)( d1 && d2 );
		break;
		case Or:
			return (double)( d1 || d2 );
		break;

		case IsEqual:
			return (double)( d1 == d2 );
		break;
		case IsNotEqual:
			return (double)( d1 != d2 );
		break;

		case Plus:
			return (double)( d1 + d2 );
		break;
		case Minus:
			return (double)( d1 - d2 );
		break;
		case Div:
			if ( d2 != 0 )
			{
				return (double)( d1 / d2 );
			}
			else
			{
				be->errorStatus = ExpressionErrorStatus::RuntimeError;
#ifdef DEBUG_YDLINTERPRETER
				debuglog << " ERROR STATUS " << be->errorStatus;
#endif
				return -1;
			}
		break;
		case Mult:
			return (double)( d1 * d2 );
		break;
		case Mod:
			return (double)( (int)d1 % (int)d2 );
		break;

		case IsMutualKendra:
		{
			const int r1 = (int)this->getRasi( (PlanetId)d1, varga );
			const int r2 = (int)this->getRasi( (PlanetId)d2, varga );
			return (red12( r1 - r2) % 3 ) ? 0 : 1 ;
		}
		break;
		case IsMutualTrikona:
		{
			const int r1 = (int)this->getRasi( (PlanetId)d1, varga );
			const int r2 = (int)this->getRasi( (PlanetId)d2, varga );
			return (red12( r1 - r2) % 4 ) ? 0 : 1 ;
		}
		break;

		case IsGrahaDrishti:
		{
			Sign targetSign = (Sign)d2;
			if ( be->exp2->outputtype == RuntimeTypeId::Planet )
			{
				targetSign = getRasi( (PlanetId)d2, varga );
			}
			const Sign sourceSign = getRasi( (PlanetId)d1, varga ); 
			const int rasidiff = red12( (int)targetSign - (int)sourceSign );
#ifdef DEBUG_YDLINTERPRETER
			debuglog << "Ydl evaluate Graha Drishti type1 " << be->exp1->outputtype << " value1 " << d1
				<< " type2 " << be->exp2->outputtype << " value2 " << d2 << " sourceSign "
				<< sourceSign << " targetSign " << targetSign;
#endif
			const VedicAspectStrength strength = VedicAspectExpert::getSingleGrahaDrishti( (PlanetId)d1, rasidiff );
			return strength == VedicAspectStrength::Full;
		}
		break;
		case IsRasiDrishti:
		{
			Sign sourceSign = (Sign)d1;
			if ( be->exp1->outputtype == RuntimeTypeId::Planet )
			{
				sourceSign = getRasi( (PlanetId)d1, varga );
			}
			Sign targetSign = (Sign)d2;
			if ( be->exp2->outputtype == RuntimeTypeId::Planet )
			{
				targetSign = getRasi( (PlanetId)d2, varga );
			}
			for( const auto &a : VedicAspectExpert::getRasiDrishtis( sourceSign ))
			{
				//debuglog << "RasiDrishti sign " << sourceSign << " has aspect on " << a;
				if ( a == targetSign ) return true;
			}
			return false;
		}
		break;

		case GetRasiInVarga: 
			return (double)getRasi( (PlanetId)d1, (Varga)d2 );
		break;
		case GetHouseInVarga:
		{
			Sign ascrasi = this->getRasi( PlanetId::Ascendant, (Varga)d2 );
			Sign prasi = this->getRasi( (PlanetId)d1, (Varga)d2 );
#ifdef DEBUG_YDLINTERPRETER
			debuglog << "GetHouseInVarga  ascrasi " << ascrasi << " prasi " << prasi
				<< " red12 " << red12( (int)prasi - (int)ascrasi );
#endif
			return red12( (int)prasi - (int)ascrasi );
		}
		break;

		case Less:
			return (double)( d1 < d2 );
		break;
		case LessEqual:
			return (double)( d1 <= d2 );
		break;
		case Greater:
			return (double)( d1 > d2 );
		break;
		case GreaterEqual:
			return (double)( d1 >= d2 );
		break;

		default:
			assert( false );
		break;
	}

	// should not happen
	assert( false );
	return -1;
}

/*****************************************************
**
**   YdlInterpreter   ---   evaluateVariadicExpression
**
******************************************************/
double YdlInterpreter::evaluateVariadicExpression( VariadicExpression *ve )
{
	assert( ve );
#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::evaluateVariadicExpression vid " << ve->vid;
#endif
	double ret = 0.0;

	// TODO das muss zuerst gemacht werden, damit runtime nur bei exp mit ok kommt.
	//if ( ue->exp1->getNestedErrorStatus() != ExpressionErrorStatus::Ok ) return -1;
	// usw

	assert( ve->exp1 );
	const double d1 = this->evaluateExpression( ve->exp1 );
	const ExpressionErrorStatus errExp1 = checkInputValue( ve->exp1, d1 );
	if ( errExp1 != ExpressionErrorStatus::Ok )
	{
		ve->errorStatus = errExp1;
		return -1;
	}

/*
	// TODO evaluate hierher auslagern und value check
	const double d2 = this->evaluateExpression( be->exp2 );
	const ExpressionErrorStatus errExp2 = checkInputValue( be->exp2, d2 );
	if ( errExp2 != ExpressionErrorStatus::Ok )
	{
		be->errorStatus = errExp2;
		return -1;
	}
*/
	switch( ve->vid )
	{
		using enum VariadicExpressionId;

		case IsPlanetInSign:
		{
			const Sign sign = this->getRasi( (PlanetId)d1, varga );
			for( Expression *e : ve->expList )
			{
				assert( e );
				assert( e->outputtype == RuntimeTypeId::Sign );
				const Sign rasi = (Sign)this->evaluateExpression( e );
				if ( rasi == sign ) return true;
			}
			return false;
		}
		break;

		case IsPlanetInHouse:
		{
			const House hradix = (House)this->getBhava( (PlanetId)d1 );
#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::evaluateVariadicExpression IsPlanetInHouse planet " << (PlanetId)d1 << " hradix " << hradix;
#endif
			for( Expression *e : ve->expList )
			{
				assert( e );
				assert( e->outputtype == RuntimeTypeId::House );
				const House house = (House)this->evaluateExpression( e );
#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::evaluateVariadicExpression IsPlanetInHouse (2) house " << house;
#endif
				if ( house == hradix ) return true;
			}
			return false;
		}
		break;

		case IsRelativeHouseOccupiedBy:
		{
			const Sign radixpos = this->getRasi( (PlanetId)d1, varga );

			assert( ve->exp2 );
			assert( ve->exp2->outputtype == RuntimeTypeId::House );
			const House housepos = (House)this->evaluateExpression( ve->exp2 );
			const int relativeHouse = red12( (int)housepos + (int)radixpos );

#ifdef DEBUG_YDLINTERPRETER
			debuglog << "evaluateVariadicExpression IsRelativeHouseOccupiedBy planet pos " << (int)radixpos << " House " << (int)housepos
				<< " relative house " << relativeHouse;
#endif

			for( Expression *e : ve->expList )
			{
				assert( e );
				assert( e->outputtype == RuntimeTypeId::Planet );
				const int pos = (int)this->getRasi( (PlanetId)this->evaluateExpression( e ), varga );
#ifdef DEBUG_YDLINTERPRETER
				debuglog << "evaluateVariadicExpression loop pos " << pos;
#endif
				if ( pos == relativeHouse ) return true;
			}
			return false;
		}
		break;

		case EqualsOneOf:
		{
			for( Expression *e : ve->expList )
			{
				assert( e );
				if ( d1 == this->evaluateExpression( e )) return true;
			}
			return false;
		}
		break;

		default:
			assert( false );
		break;
	}
	return ret;
}

/*****************************************************
**
**   YdlInterpreter   ---   evaluateRule
**
******************************************************/
double YdlInterpreter::evaluateRule( Rule *r )
{
	rule = r;
	assert( rule );
	double ret = 0.0;
	rule->resetRuntimeErrorStatus();

#ifdef DEBUG_YDLINTERPRETER
	debuglog << "Start YdlInterpreter::evaluateYogaRule expression count " << rule->expressions.size();
#endif

	if ( rule->expressions.size() == 0 )
	{
		errorlog << "YdlInterpreter::evaluateYogaRule has no expressions";
		rule->errorStatus = ExpressionErrorStatus::RuntimeError;
		return false;
	}

	for( const auto &e : rule->expressions )
	{
		assert( e );
		ret = evaluateExpression( e );
#ifdef DEBUG_YDLINTERPRETER
		debuglog << "YdlInterpreter::evaluateYogaRule IN LOOP " << ret;
#endif

		if ( e->type == ExpressionTypeId::Return )
		{
			if ( e->outputtype != RuntimeTypeId::Boolean )
			{
				warnlog << "Output type of return expression is " << e->outputtype << ", should be boolean.";
			}
			break;
		}
	}
#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::evaluateYogaRule return " << ret;
#endif
	if ( rule->getOutputType() != RuntimeTypeId::Boolean )
	{
		warnlog << "Output type of expression is " << rule->getOutputType() << ", should be boolean.";
	}
	rule = nullptr;
	return ret;
}

/*****************************************************
**
**   YdlInterpreter   ---   getRasi
**
******************************************************/
Sign YdlInterpreter::getRasi( const PlanetId &factorId, const Varga &varga )
{
	assert_valid_enum_or_none<PlanetId>( factorId );
	if ( expert )
	{
		return expert->getVargaData( factorId, varga ).getRasi();
	}
	// there is no higher varga in fake mode
	if ( varga != Varga::Rasi )
		errorlog << "YdlInterpreter::getRasi does not support higher vargas, using Rasi";

	if ( const auto &a = fake_data.find( factorId ); a != fake_data.end())
	{
		return a->second;
	}
	return Sign::None;
}

/*****************************************************
**
**   YdlInterpreter   ---   getNakshatra
**
******************************************************/
Nakshatra27 YdlInterpreter::getNakshatra( const PlanetId &ob ) const
{
	assert_valid_enum_or_none<PlanetId>( ob );
	if ( expert )
	{
		return ::getNakshatra27( expert->getLongitude( ob ));
	}
	return Nakshatra27::None;
}

/*****************************************************
**
**   YdlInterpreter   ---   getBhava
**
******************************************************/
House YdlInterpreter::getBhava( const PlanetId &ob )
{
	assert_valid_enum_or_none<PlanetId>( ob );
#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::getBhava ob " << ob << " artificialLagna " << artificialLagna;
#endif

	Sign ascrasi = artificialLagna == Sign::None ? this->getRasi( PlanetId::Ascendant, varga ) : artificialLagna;
	Sign prasi = this->getRasi( ob, varga );

#ifdef DEBUG_YDLINTERPRETER
	debuglog << "YdlInterpreter::getBhava ascrasi " << ascrasi << " prasi " << prasi << " diff " << (House)red12( (int)prasi - (int)ascrasi );
#endif
	if ( ascrasi != Sign::None && prasi != Sign::None )
	{
		return (House)red12( (int)prasi - (int)ascrasi );
	}
	return House::None;
}

/*****************************************************
**
**   YdlInterpreter   ---   isBenefic
**
******************************************************/
bool YdlInterpreter::isBenefic( const PlanetId &ob ) const
{
	if ( expert )
	{
		Horoscope_ptr horoscope = expert->getProps()->getRadixHoroscope();
		assert( horoscope );
		return horoscope->isBenefic( ob, expert->getViewpoint() );
	}

	// mercury and moon not considered here
	const static std::set<PlanetId> malefics = { PlanetId::Sun, PlanetId::Mars, PlanetId::Saturn, PlanetId::Rahu, PlanetId::Ketu };
	if ( malefics.contains(ob )) return false;

	return true;
}

/*****************************************************
**
**   YdlInterpreter   ---   getNumberOfOccupiedSigns
**
******************************************************/
int YdlInterpreter::getNumberOfOccupiedSigns()
{
	int count = 0;


	for ( const Sign sign : Enum<Sign>())
	{
#ifdef DEBUG_YDLINTERPRETER
		debuglog << "Number of planets in " << sign << " is " << nb_planets_in_sign[(int)sign];
#endif
		if ( nb_planets_in_sign[(int)sign] > 0 ) count++;
	}
	return count;
}


