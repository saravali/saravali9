/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/YdlParser.yy
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

%{

#include <iostream>

#include "enumserializer.h"
#include "logger.h"
#include "YdlExpression.h"

#include <wx/string.h>
#include <stdio.h>

#define YYDEBUG 1

#define YYERROR_VERBOSE

int yyerror ( const char* s);
extern int yylex( void );
extern int yylineno;
extern char *yytext;

Rule *rule = nullptr;
ExpressionErrorStatus parserErrorStatus = ExpressionErrorStatus::Ok;

std::list<Expression*> expression_list;

%}

%union {
  int ival;
	char *sval;
	double dval;
	Expression *expval;
};

/*
 Caution: BOOLEAN already set under MinGW64, rename it
*/

%token <ival> LPAR RPAR COLON COMMA PERCENT DIGIT OR AND NOT ISMUTUALKENDRA ISMUTUALTRIKONA
%token <ival> DESCR GETLORDOFHOUSE ISPLANETINRASI ISEQUAL ISNOTEQUAL EQUAL
%token <ival> RETURN PLANET GETRASI GETBHAVA PLUS MINUS MULT DIV MOD LT LE GT GE
%token <ival> COUNTPLANETSINSIGN COUNTPLANETSINHOUSE NUMBER_OF_OCCUPIED_SIGNS ISGRAHADRISHTI ISRASIDRISHTI GETLORD
%token <ival> ISBENEFIC ISMALEFIC ISPLANETINKENDRA ISPLANETINAPOKLIMA ISPLANETINPANAPHARA
%token <ival> ISPLANETINDUALRASI ISPLANETINFIXEDRASI ISPLANETINMOVABLERASI RED12 GETNAKSHATRA RED27
%token <ival> ISPLANETINEXALTATIONSIGN ISPLANETINDEBILATIONSIGN ISPLANETINMULATRIKONASIGN
%token <ival> ISPLANETINSIGN ISPLANETINHOUSE ISRELATIVEHOUSEOCCUPIEDBY
%token <ival> GETLONGITUDE ISRETROGRADE ISINVARGOTTAMA ISINMULATRIKONA GETRASIINVARGA GETHOUSEINVARGA
%token <ival> HOUSE SIGN NAKSHATRA BOOLEAN VARGA EQUALSONEOF
%token <ival> TYPE_INT TYPE_DOUBLE TYPE_PLANET TYPE_SIGN TYPE_NAKSHATRA TYPE_HOUSE TYPE_BOOLEAN TYPE_VARGA

%token <dval> FLOAT

%token <sval> VARIABLE

%type <expval> exp return_exp unary_exp binary_exp getlordofhouse_exp and_exp or_exp getrasi_exp getbhava_exp
%type <expval> isequal_exp isnotequal_exp plus_exp minus_exp div_exp mult_exp mod_exp ismutualkendra_exp ismutualtrikona_exp
%type <expval> countplanetsinsign_exp countplanetsinhouse_exp isgrahadrishti_exp israsidrishti_exp getlordofsign_exp isbenefic_exp ismalefic_exp
%type <expval> isplanetinexaltationsign_exp isplanetindebilationsign_exp isplanetinmulatrikonasign_exp
%type <expval> isplanetinkendra_exp isplanetinapoklima_exp isplanetinpanaphara_exp lt_exp le_exp gt_exp ge_exp
%type <expval> isplanetindualrasi_exp isplanetinfixedrasi_exp isplanetinmovablerasi_exp not_exp red12_exp getnakshatra_exp
%type <expval> getlongitude_exp isretrograde_exp isinvargottama_exp isinmulatrikona_exp getrasiinvarga_exp gethouseinvarga_exp
%type <expval> typecast_int_exp typecast_double_exp typecast_planet_exp typecast_house_exp typecast_nakshatra_exp
%type <expval> typecast_boolean_exp typecast_sign_exp
%type <expval> assignment_exp variable_alias_exp
%type <expval> variadic_exp isplanetinsign_exp isplanetinhouse_exp isrelativehouseoccupiedby_exp equalsoneof_exp


%left RETURN
%left AND OR
%left ISEQUAL ISNOTEQUAL LT LE GT GE
%left PLUS MINUS
%left LPAR RPAR
%left NOT
%left MULT DIV MOD

%%
rule_decl :
{
	debuglog << "YdlParser NEW RULE\n"; 
	rule = new Rule;
} exp_list;

exp_list: stmt | exp_list stmt;

stmt : exp COLON { rule->addExpression( $1 ); } | assignment_exp COLON;

assignment_exp : VARIABLE EQUAL exp
{
	debuglog << "Assignment " << $1;
	assert( $3 );
	assert( rule );
	auto e = rule->variables.find( $1 );
	if ( e != rule->variables.end())
	{
		if ( e->second )
		{
			debuglog << "Assignment: variable " << $1 << " already set, type is " << e->second->outputtype
				<< " type of new expression: " << $3->outputtype;

			if ( e->second->outputtype != $3->outputtype )
			{
				errorlog << "Assignment: variable type error for " << $1 << " type is " << e->second->outputtype
				<< " type of new expression: " << $3->outputtype;
				parserErrorStatus = ExpressionErrorStatus::TypeError;
				rule->errorStatus = parserErrorStatus;
				YYABORT;
			}
			delete e->second;
		}
	}
	rule->variables[$1] = $3;
}

exp : unary_exp | binary_exp | return_exp | variadic_exp | variable_alias_exp
	| LPAR exp RPAR { $$ = $2; }
	| NUMBER_OF_OCCUPIED_SIGNS LPAR RPAR { $$ = new VoidExpression( VoidExpressionId::NumberOfOccupiedSigns ); }
	| DIGIT { $$ = new ConstantExpression( RuntimeTypeId::Integer, $1 ); }
	| FLOAT { $$ = new ConstantExpression( RuntimeTypeId::FloatingPoint, $1 ); }
	| HOUSE { $$ = new ConstantExpression( RuntimeTypeId::House, $1 ); }
	| PLANET { $$ = new ConstantExpression( RuntimeTypeId::Planet, $1 ); }
	| SIGN { $$ = new ConstantExpression( RuntimeTypeId::Sign, $1 ); }
	| VARGA { $$ = new ConstantExpression( RuntimeTypeId::Varga, $1 ); }
	| NAKSHATRA { $$ = new ConstantExpression( RuntimeTypeId::Nakshatra, $1 ); }
	| BOOLEAN { $$ = new ConstantExpression( RuntimeTypeId::Boolean, $1 ); }

return_exp : RETURN exp
{
	infolog << "Return expression found, no further parsing";
	$$ = new ReturnExpression( $2 );
	//YYACCEPT; // no further parsing after return
};

variable_alias_exp : VARIABLE
{
	infolog << "VariableAliasExpression " << $1;
	$$ = new VariableAliasExpression( $1 );
};

	/*
assignment_exp : VARIABLE EQUAL exp
{
	assert( rule );
	auto e = rule->variables.find( $1 );
	if ( e != rule->variables.end())
	{
		if ( e->second ) delete e->second;
	}
	rule->variables[$1] = $3;
	//$$ = $3;
	$$ = new AssignmentExpression( $1, $3 );
};
	*/

unary_exp : not_exp | getlordofhouse_exp | getbhava_exp | getrasi_exp | getlordofsign_exp
	| planetsinsign_exp | planetsinhouse_exp
	| ismalefic_exp | isbenefic_exp | countplanetsinsign_exp | countplanetsinhouse_exp
	| isplanetinkendra_exp | isplanetinapoklima_exp | isplanetinpanaphara_exp
	| isplanetindualrasi_exp | isplanetinfixedrasi_exp | isplanetinmovablerasi_exp
	| getlongitude_exp | isretrograde_exp | isinvargottama_exp | isinmulatrikona_exp | getrasiinvarga_exp
	| red12_exp | getnakshatra_exp
	| typecast_int_exp | typecast_double_exp | typecast_planet_exp | typecast_house_exp
	| typecast_nakshatra_exp | typecast_boolean_exp | typecast_sign_exp
;

not_exp            : NOT exp { $$ = new UnaryExpression( $2, UnaryExpressionId::Not ); };

red12_exp          : RED12 LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::Red12 ); };

getlordofhouse_exp : GETLORDOFHOUSE LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::GetLordOfHouse ); };

getlordofsign_exp  : GETLORDOFSIGN LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::GetLordOfSign ); };

getrasi_exp        : GETRASI LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::GetRasi ); };

getnakshatra_exp   : GETNAKSHATRA LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::GetNakshatra ); };

getbhava_exp       : GETBHAVA LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::GetBhava ); };

countplanetsinsign_exp  : COUNTPLANETSINSIGN LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::CountPlanetsInSign ); };

countplanetsinhouse_exp : COUNTPLANETSINHOUSE LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::CountPlanetsInHouse ); };

isbenefic_exp      : ISBENEFIC LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::IsBenefic ); };

ismalefic_exp      : ISMALEFIC LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::IsMalefic ); };

isplanetinkendra_exp      : ISPLANETINKENDRA LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::IsPlanetInKendra ); };

isplanetinapoklima_exp    : ISPLANETINAPOKLIMA LPAR exp RPAR { $$ 
	= new UnaryExpression( $3, UnaryExpressionId::IsPlanetInApoklima ); };

isplanetinpanaphara_exp   : ISPLANETINPANAPHARA LPAR exp RPAR { $$ 
	= new UnaryExpression( $3, UnaryExpressionId::IsPlanetInPanaphara ); };

isplanetindualrasi_exp    : ISPLANETINDUALRASI LPAR exp RPAR { $$ 
	= new UnaryExpression( $3, UnaryExpressionId::IsPlanetInDualRasi ); };

isplanetinfixedrasi_exp   : ISPLANETINFIXEDRASI LPAR exp RPAR { $$ 
	= new UnaryExpression( $3, UnaryExpressionId::IsPlanetInFixedRasi ); };

isplanetinmovablerasi_exp : ISPLANETINMOVABLERASI LPAR exp RPAR { $$ 
	= new UnaryExpression( $3, UnaryExpressionId::IsPlanetInMovableRasi ); };

typecast_int_exp       : LPAR TYPE_INT       RPAR exp { $$ = new UnaryExpression( $4, UnaryExpressionId::TypeCastInt ); };
typecast_double_exp    : LPAR TYPE_DOUBLE    RPAR exp { $$ = new UnaryExpression( $4, UnaryExpressionId::TypeCastFloatingPoint ); };
typecast_planet_exp    : LPAR TYPE_PLANET    RPAR exp { $$ = new UnaryExpression( $4, UnaryExpressionId::TypeCastPlanet ); };
typecast_house_exp     : LPAR TYPE_HOUSE     RPAR exp { $$ = new UnaryExpression( $4, UnaryExpressionId::TypeCastHouse ); };
typecast_sign_exp      : LPAR TYPE_SIGN      RPAR exp { $$ = new UnaryExpression( $4, UnaryExpressionId::TypeCastSign ); };
typecast_nakshatra_exp : LPAR TYPE_NAKSHATRA RPAR exp { $$ = new UnaryExpression( $4, UnaryExpressionId::TypeCastNakshatra ); };
typecast_boolean_exp   : LPAR TYPE_BOOLEAN   RPAR exp { $$ = new UnaryExpression( $4, UnaryExpressionId::TypeCastBoolean ); };

getlongitude_exp : GETLONGITUDE LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::GetLongitude ); };
isretrograde_exp : ISRETROGRADE LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::IsRetrograde ); };
isinvargottama_exp : ISINVARGOTTAMA LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::IsPlanetInVargottama ); };
isinmulatrikona_exp : ISINMULATRIKONA LPAR exp RPAR { $$ = new UnaryExpression( $3, UnaryExpressionId::IsPlanetInMulatrikona ); };

binary_exp: and_exp | or_exp | isequal_exp | isnotequal_exp | plus_exp | minus_exp | div_exp | mult_exp
	| isplanetinexaltationsign_exp | isplanetindebilationsign_exp | isplanetinmulatrikonasign_exp | gethouseinvarga_exp
	| mod_exp | ismutualkendra_exp | ismutualtrikona_exp | isgrahadrishti_exp | israsidrishti_exp
	| lt_exp | le_exp | gt_exp | ge_exp
;

and_exp            : exp AND exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::And ); } ;
or_exp             : exp OR exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::Or ); } ;
isequal_exp        : exp ISEQUAL exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::IsEqual ); } ;
isnotequal_exp     : exp ISNOTEQUAL exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::IsNotEqual ); } ;
plus_exp           : exp PLUS exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::Plus ); } ;
minus_exp          : exp MINUS exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::Minus ); } ;
mult_exp           : exp MULT exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::Mult ); } ;
div_exp            : exp DIV exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::Div ); } ;
mod_exp            : exp MOD exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::Mod ); } ;
lt_exp             : exp LT exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::Less ); } ;
le_exp             : exp LE exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::LessEqual ); } ;
gt_exp             : exp GT exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::Greater ); } ;
ge_exp             : exp GE exp { $$ = new BinaryExpression( $1, $3, BinaryExpressionId::GreaterEqual ); } ;

ismutualkendra_exp  : ISMUTUALKENDRA LPAR exp COMMA exp RPAR
	{ $$ = new BinaryExpression( $3, $5, BinaryExpressionId::IsMutualKendra ); } ;
ismutualtrikona_exp  : ISMUTUALTRIKONA LPAR exp COMMA exp RPAR
	{ $$ = new BinaryExpression( $3, $5, BinaryExpressionId::IsMutualTrikona ); } ;
isgrahadrishti_exp   : ISGRAHADRISHTI LPAR exp COMMA exp RPAR
	{ $$ = new BinaryExpression( $3, $5, BinaryExpressionId::IsGrahaDrishti ); } ;
israsidrishti_exp    : ISRASIDRISHTI LPAR exp COMMA exp RPAR
	{ $$ = new BinaryExpression( $3, $5, BinaryExpressionId::IsRasiDrishti ); } ;

isplanetinexaltationsign_exp : ISPLANETINEXALTATIONSIGN LPAR exp RPAR
	{ $$ = new UnaryExpression( $3, UnaryExpressionId::IsPlanetInExaltationSign ); };

isplanetindebilationsign_exp : ISPLANETINDEBILATIONSIGN LPAR exp RPAR
	{ $$ = new UnaryExpression( $3, UnaryExpressionId::IsPlanetInDebilationSign ); };

isplanetinmulatrikonasign_exp : ISPLANETINMULATRIKONASIGN LPAR exp RPAR
	{ $$ = new UnaryExpression( $3, UnaryExpressionId::IsPlanetInMulatrikonaSign ); };

getrasiinvarga_exp : GETRASIINVARGA LPAR exp COMMA exp RPAR
	{ $$ = new BinaryExpression( $3, $5, BinaryExpressionId::GetRasiInVarga ); };

gethouseinvarga_exp : GETHOUSEINVARGA LPAR exp COMMA exp RPAR
	{ $$ = new BinaryExpression( $3, $5, BinaryExpressionId::GetHouseInVarga ); };

variadic_exp : isplanetinsign_exp | isplanetinhouse_exp | isrelativehouseoccupiedby_exp | equalsoneof_exp;

param_list: exp
{
	expression_list.clear();
	expression_list.push_back( $1 );
}

| exp COMMA param_list {
	expression_list.push_front( $1 );
}

isplanetinsign_exp : ISPLANETINSIGN LPAR exp COMMA param_list RPAR
{
	$$ = new VariadicExpression( $3, nullptr, expression_list, VariadicExpressionId::IsPlanetInSign );
};

isplanetinhouse_exp : ISPLANETINHOUSE LPAR exp COMMA param_list RPAR
{
	$$ = new VariadicExpression( $3, nullptr, expression_list, VariadicExpressionId::IsPlanetInHouse );
};

isrelativehouseoccupiedby_exp: ISRELATIVEHOUSEOCCUPIEDBY LPAR exp COMMA exp COMMA param_list RPAR
{
	$$ = new VariadicExpression( $3, $5, expression_list, VariadicExpressionId::IsRelativeHouseOccupiedBy );
}

equalsoneof_exp: EQUALSONEOF LPAR exp COMMA param_list RPAR
{
	$$ = new VariadicExpression( $3, nullptr, expression_list, VariadicExpressionId::EqualsOneOf );
}

%%
/*****************************************************
**
**   yyerror
**
******************************************************/
int yyerror ( const char* s)
{
	mstringstream ms;
	ms << "Parse error at line " << ( yylineno + 1 ) << " near token \"" << yytext << "\" (message: " << s << ")";
	errorlog << ms.str();
	yylineno = 1;
	parserErrorStatus = ExpressionErrorStatus::ParserError;
	return 1;
}

